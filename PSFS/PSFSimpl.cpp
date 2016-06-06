//
// PSFSimpl.cpp
//

#include "stdafx.h"
#include "PSFSimpl.h"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

const int DEFAULT_SHIFT = 4;


/////////////////////////////////////////////////////////////////////////////////////
// CPSFileSystem core implementation
/////////////////////////////////////////////////////////////////////////////////////

std::string CPSFS_item::GetName() const
{
  return (!IsRootItem()) ? name : std::string("\\");
} // CPSFS_item::GetName

std::string CPSFS_item::GetFullName() const
{
//  return (!IsRootItem()) ? (owner_path == "\\" ? "" : owner_path + std::string("\\") + GetName()) : GetName();
  if (IsRootItem())
    return GetName();
  else
  {
    if (owner_path == "\\")
      return std::string("\\") + GetName();
    else
      return owner_path + std::string("\\") + GetName();
  }
} // CPSFS_item::GetFullName

/////////////////////////////////////////////////////////

std::string CPSFS_item::PrintOut(int shift) const
{
  std::string file_flag = (type == psfs_itemFile) ? std::string("*") : std::string("");
  std::string ret = std::string(shift, ' ') + GetName() + file_flag + std::string(" (") + GetFullName() + std::string(")\n");

  if (type == psfs_itemFile)
  {
    ret += std::string(shift, ' ') + std::string("---file content---------------------------------------------------------\n");

    if (content.size() != 0)
    {
      char* b = new char[content.size() + 1];
      memset(b, 0, content.size() + 1);
      memcpy(b, &content[0], content.size());

      ret += b;
      delete b;

      ret += std::string("\n");
    }

    ret += std::string(shift, ' ') + std::string("------------------------------------------------------------------------\n");
  }

  for (auto iter = childs.begin(); iter != childs.end(); ++iter)
    ret += iter->second.PrintOut(shift + DEFAULT_SHIFT);
  return ret;
} // CPSFS_item::PrintOut

void CPSFS_item::InsertItem(CPSFS_item* item)
{
  childs[item->GetName()] = *item;
} // CPSFS_item::InsertItem

void CPSFS_item::DetachItem(CPSFS_item* item)
{
  childs.erase(childs.find(item->GetName()));
} // CPSFS_item::DetachItem

void CPSFS_item::SetData(size_t _pos, char* data, size_t datasize)
{
  if (content.size() - _pos < datasize)
    content.insert(content.end(), datasize - (content.size() - _pos), '.');

  std::copy(data, data + datasize, content.begin() + _pos);
} // CPSFS_item::SetData

void CPSFS_item::GetData(size_t _pos, char* data, size_t datasize, size_t& bytesread)
{
  if (content.size() > datasize + _pos)
  {
    std::copy(content.begin() + _pos, content.begin() + _pos + datasize, data);
    bytesread = datasize;
  }
  else
  {
    std::copy(content.begin() + _pos, content.end(), data);
    bytesread = content.end() - (content.begin() + _pos);
  }
} // CPSFS_item::GetData

bool CPSFS_item::CreateFolderByList(std::list<std::string>& path_list)
{
  if (path_list.size() < 1)
    return false;

  std::string folder_to_create = *path_list.begin();

  if (folder_to_create == std::string(""))
    return false;

  auto iter = childs.find(folder_to_create);

  auto folder = childs.end();

  if (iter == childs.end()) // folder not exists, creating
  {
    childs[folder_to_create] = CPSFS_item(psfs_itemFolder, folder_to_create, GetFullName());
    folder = childs.find(folder_to_create);
  }
  else
  {
    if (iter->second.GetItemType() != psfs_itemFolder)
      return false;
    folder = iter;
    if (path_list.size() == 1)
      return false;
  }

  path_list.pop_front();

  if (path_list.size() == 0)
    return true;

  if (!folder->second.CreateFolderByList(path_list))
    return false;

  return true;
} // CPSFS_item::CreateFolderByList

bool CPSFS_item::CreateFileByList(std::list<std::string>& path_list)
{
  if (path_list.size() < 1)
    return false;

  std::string folder_to_create = *path_list.begin();

  if (folder_to_create == std::string(""))
    return false;

  if (path_list.size() == 1)
  {
    auto iter = childs.find(folder_to_create);

    if (iter == childs.end()) // file not exists, creating
    {
      childs[folder_to_create] = CPSFS_item(psfs_itemFile, folder_to_create, GetFullName());
      return true;
    }
    else
      return false;
  }
  else
  {
    auto iter = childs.find(folder_to_create);

    auto folder = childs.end();

    if (iter == childs.end()) // folder not exists, creating
    {
      childs[folder_to_create] = CPSFS_item(psfs_itemFolder, folder_to_create, GetFullName());
      folder = childs.find(folder_to_create);
    }
    else
    {
      if (iter->second.GetItemType() != psfs_itemFolder)
        return false;
      folder = iter;
      if (path_list.size() == 1)
        return false;
    }

    path_list.pop_front();

    if (!folder->second.CreateFileByList(path_list))
      return false;
  }

  return true;
} // CPSFS_item::CreateFileByList

CPSFS_item* CPSFS_item::GetItemByList(std::list<std::string>& path_list) const
{
  if (path_list.size() < 1)
    return nullptr;

  std::string current_folder = *path_list.begin();

  if (current_folder == "")
    return const_cast<CPSFS_item*>(this);

  auto iter = childs.find(current_folder);

  CPSFS_item* folder = nullptr;

  if (iter == childs.end())
  {
    return nullptr;
  }
  else
  {
    if (path_list.size() == 1) // last item in the path
    {
      return &(const_cast<CPSFS_item&>(iter->second));
    }

    if (iter->second.GetItemType() != psfs_itemFolder)
      return nullptr;
    folder = &(const_cast<CPSFS_item&>(iter->second));

  }

  path_list.pop_front();

  return folder->GetItemByList(path_list);
} // CPSFS_item::GetItemByList

CPSFS_item* CPSFS_item::GetChildItem(const std::string& name) const
{
  auto iter = childs.find(name);
  return (iter != childs.end()) ? &(const_cast<CPSFS_item&>(iter->second)) : nullptr;
} // CPSFS_item::GetChildItem

/////////////////////////////////////////////////////////

CPSFS_impl::CPSFS_impl(const std::string& fname) 
  : m_FileName(fname), 
    m_RootFolder(CPSFS_item::psfs_itemFolder, "", ""),
    m_LastHandle(0)
{ 
  try
  {
    LoadFromFile();
  }
  catch (...){}
} // CPSFS_impl::CPSFS_impl

CPSFS_impl::~CPSFS_impl()
{ 
  try
  {
    StoreToFile();
  }
  catch (...){}
} // CPSFS_impl::~CPSFS_impl

void CPSFS_impl::Flush()
{ 
  try
  {
    StoreToFile();
  }
  catch (...){}
} // CPSFS_impl::Flush

bool CPSFS_impl::CreateFolder(const std::string& path)
{
  std::list<std::string> folders_list;
  bool is_absulute_path;
  if (! SplitPath(path, folders_list, is_absulute_path))
    return false;

  if (! m_RootFolder.CreateFolderByList(folders_list))
    return false;

  return true;
} // CPSFS_impl::CreateFolder

bool CPSFS_impl::CreateFile(const std::string& path)
{
  std::list<std::string> folders_list;
  bool is_absulute_path;
  if (!SplitPath(path, folders_list, is_absulute_path))
    return false;

  if (!m_RootFolder.CreateFileByList(folders_list))
    return false;

  return true;
} // CPSFS_impl::CreateFile

bool CPSFS_impl::DeleteItem(const std::string& path)
{
  CPSFS_item* item = GetItem(path);

  if (item != nullptr && ! item->IsRootItem())
  {
    std::string owner_path = item->GetOwnerPath();
    CPSFS_item* owner = GetItem(owner_path);
    if (owner->GetItemType() == CPSFS_item::psfs_itemFolder)
      owner->DetachItem(item);
  }
  else
    return false;

  return true;
} // CPSFS_impl::DeleteItem

bool CPSFS_impl::CopyItem(const std::string& src_path, const std::string& dest_path, bool is_move)
{
  CPSFS_item* srcitem = GetItem(src_path);
  if (!srcitem)
    return false;

  CPSFS_item* destitem = GetItem(dest_path);
  if (!destitem)
    return false;

  if (destitem->GetItemType() != CPSFS_item::psfs_itemFolder)
    return false;

  if (destitem->GetChildItem(srcitem->GetName()) != nullptr)
    return false;

  destitem->InsertItem(srcitem);

  if (is_move)
  {
    if (! DeleteItem(src_path))
      return false;
  }

  return true;
} // CPSFS_impl::CopyItem

long CPSFS_impl::OpenFile(const std::string& path, const std::string& mode)
{
  CPSFS_item* file = GetItem(path);

  if (mode == std::string("r"))
  {
    if (file == nullptr)
      return 0;

    // is locked for writing?
    for (auto iter = m_Locks.begin(); iter != m_Locks.end(); ++iter)
    {
      if (iter->second.file == file)
      {
        if (iter->second.is_lock_for_writing)
          return false;
        break;
      }
    }

    // mark as been readed
    m_Locks[++m_LastHandle] = CPSFSLock(file);

    return m_LastHandle;
  }
  else
  {
    if (file == nullptr)
    {
      if (! CreateFile(path))
        return 0;

      file = GetItem(path);
    }

    // is locked for reading or writing?
    for (auto iter = m_Locks.begin(); iter != m_Locks.end(); ++iter)
    {
      if (iter->second.file == file)
        return false;
    }

    size_t content_size = file->GetContentSize();

    // mark as been readed
    m_Locks[++m_LastHandle] = CPSFSLock(file, true, (mode == std::string("a")) ? content_size : 0);

    return m_LastHandle;
  }
} // CPSFS_impl::OpenFile

bool CPSFS_impl::CloseFile(long handle)
{
  auto iter = m_Locks.find(handle);
  if (iter == m_Locks.end())
    return false;

  m_Locks.erase(iter);
  return true;
} // CPSFS_impl::CloseFile

bool CPSFS_impl::GetFileSize(long handle, size_t& size)
{
  auto iter = m_Locks.find(handle);
  if (iter == m_Locks.end())
    return false;
  size = iter->second.file->GetContentSize();
  return true;
} // CPSFS_impl::GetFileSize

bool CPSFS_impl::SetFilePos(long handle, size_t pos)
{
  auto iter = m_Locks.find(handle);
  if (iter == m_Locks.end())
    return false;
  if (iter->second.file->GetContentSize() < pos)
    return false;
  iter->second.file_position = pos;
  return true;
} // CPSFS_impl::SetFilePos

bool CPSFS_impl::WriteData(long handle, char* data, size_t datasize)
{
  auto iter = m_Locks.find(handle);
  if (iter == m_Locks.end())
    return false;

  CPSFS_item* file = iter->second.file;

  if (!iter->second.is_lock_for_writing)
    return false;

  file->SetData(iter->second.file_position, data, datasize);

  iter->second.file_position += datasize;

  return true;
} // CPSFS_impl::WriteData

bool CPSFS_impl::ReadData(long handle, char* data, size_t datasize, size_t& bytesread)
{
  auto iter = m_Locks.find(handle);
  if (iter == m_Locks.end())
    return false;

  CPSFS_item* file = iter->second.file;

  if (iter->second.is_lock_for_writing)
    return false;

  file->GetData(iter->second.file_position, data, datasize, bytesread);

  iter->second.file_position += bytesread;

  return true;
} // CPSFS_impl::ReadData

std::string CPSFS_impl::PrintOut() const
{
  return m_RootFolder.PrintOut(DEFAULT_SHIFT);
} // CPSFS_impl::PrintOut

CPSFS_item* CPSFS_impl::GetItem(const std::string& path) const
{
  std::list<std::string> folders_list;
  bool is_absulute_path;
  if (! SplitPath(path, folders_list, is_absulute_path))
    return nullptr;

  return m_RootFolder.GetItemByList(folders_list);
} // CPSFS_impl::GetItem

void CPSFS_impl::LoadFromFile()
{
  std::ifstream ifs(m_FileName);
  if (!ifs.is_open())
    return;

  boost::archive::text_iarchive ia(ifs);

  ia >> m_RootFolder;
} // CPSFS_impl::LoadFromFile

void CPSFS_impl::StoreToFile() 
{
  std::ofstream ofs(m_FileName);

  boost::archive::text_oarchive oa(ofs);

  oa << m_RootFolder;
} // CPSFS_impl::StoreToFile

bool CPSFS_impl::SplitPath(const std::string& path, std::list<std::string>& folders_list, bool& is_absolute_path)
{
  if (path == std::string(""))
    return false;

  boost::split(folders_list, path, boost::is_any_of("\\"), boost::token_compress_on);
  if (folders_list.size() < 2)
  {
    is_absolute_path = true;
  }
  else
  {
    is_absolute_path = (*folders_list.begin() == "");
    if (is_absolute_path)
      folders_list.pop_front();
  }

  if (!is_absolute_path) // at this moment relative paths are not supported
    return false;

  return true;
} // CPSFS_impl::SplitPath


