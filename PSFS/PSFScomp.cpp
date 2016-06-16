//
// PSFScomp.cpp
//

#include "stdafx.h"
#include <initguid.h>
#include "PSFScomp.h"
#include <vector>

/////////////////////////////////////////////////////////////////////////////////////
// CPSFileSystem component implementation
/////////////////////////////////////////////////////////////////////////////////////

CPSFileSystem::CPSFileSystem()
  : m_ref(0)
{
  // Увеличить значение внешнего счетчика объектов
  InterlockedIncrement(&g_objs_count); 
} // CPSFileSystem::CPSFileSystem

CPSFileSystem::~CPSFileSystem()
{
  // Уменьшить значение внешнего счетчика объектов
  InterlockedDecrement(&g_objs_count); 
} // CPSFileSystem::~CPSFileSystem

STDMETHODIMP CPSFileSystem::QueryInterface(REFIID riid, void** ppv)
{
  *ppv = 0;

  if (riid == IID_IUnknown || riid == IID_IPSFileSystem)
    *ppv = this;

  if (*ppv)
  {
    AddRef();
    return(S_OK);
  }
  return (E_NOINTERFACE);
} // CPSFileSystem::QueryInterface

STDMETHODIMP_(ULONG) CPSFileSystem::AddRef()
{
  return InterlockedIncrement(&m_ref);
} // CPSFileSystem::AddRef

STDMETHODIMP_(ULONG) CPSFileSystem::Release()
{
  if (InterlockedDecrement(&m_ref) == 0)
  {
    delete this;
    return 0;
  }

  return m_ref;
} // CPSFileSystem::Release

std::string to_string(std::wstring const &s, std::locale const &loc, char default_char = '?')
{
  if (s.empty())
    return std::string();
  std::ctype<wchar_t> const &facet = std::use_facet<std::ctype<wchar_t> >(loc);
  wchar_t const *first = s.c_str();
  wchar_t const *last = first + s.size();
  std::vector<char> result(s.size());

  facet.narrow(first, last, default_char, &result[0]);

  return std::string(result.begin(), result.end());
} // to_string

std::wstring to_widestring(std::string const &s, std::locale const &loc)
{
  if (s.empty())
    return std::wstring();
  std::ctype<wchar_t> const &facet = std::use_facet<std::ctype<wchar_t> >(loc);
  char const *first = s.c_str();
  char const *last = first + s.size();
  std::vector<wchar_t> result(s.size());

  facet.widen(first, last, &result[0]);

  return std::wstring(result.begin(), result.end());
} // to_widestring

STDMETHODIMP CPSFileSystem::LoadFromFile(TCHAR* path, BOOL* ret)
{
  if (m_PSFS.get() != nullptr)
    return S_FALSE;

  m_PSFS = std::unique_ptr<CPSFS_impl>(new CPSFS_impl(to_string(path, std::locale("rus"))));

  *ret = TRUE;

  return S_OK;
} // CPSFileSystem::LoadFromFile

STDMETHODIMP CPSFileSystem::Flush(BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  m_PSFS->Flush();
  *ret = TRUE;
  return S_OK;
} // CPSFileSystem::Flush

STDMETHODIMP CPSFileSystem::CreateFolder(TCHAR* path, BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  *ret = m_PSFS->CreateFolder(to_string(path, std::locale("rus")));

  return S_OK;
} // CPSFileSystem::CreateFolder

STDMETHODIMP CPSFileSystem::DeleteItem(TCHAR* path, BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  *ret = m_PSFS->DeleteItem(to_string(path, std::locale("rus")));

  return S_OK;
} // CPSFileSystem::DeleteItem

STDMETHODIMP CPSFileSystem::CopyItem(TCHAR* src, TCHAR* dest, BOOL is_move, BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  *ret = m_PSFS->CopyItem(to_string(src, std::locale("rus")), to_string(dest, std::locale("rus")), is_move == TRUE);

  return S_OK;
} // CPSFileSystem::CopyItemDeleteItem

STDMETHODIMP CPSFileSystem::OpenFile(TCHAR* path, TCHAR* mode, LONG* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  *ret = m_PSFS->OpenFile(to_string(path, std::locale("rus")), to_string(mode, std::locale("rus")));
  return S_OK;
} // CPSFileSystem::OpenFile

STDMETHODIMP CPSFileSystem::CloseFile(LONG handle, BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  *ret = m_PSFS->CloseFile(handle);
  return S_OK;
} // CPSFileSystem::CloseFile

STDMETHODIMP CPSFileSystem::GetFileSize(LONG handle, ULONG* size, BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  size_t _size = *size;
  *ret = m_PSFS->GetFileSize(handle, _size);
  *size = _size;
  return S_OK;
} // CPSFileSystem::GetFileSize

STDMETHODIMP CPSFileSystem::SetFilePos(LONG handle, ULONG pos, BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  *ret = m_PSFS->SetFilePos(handle, pos);
  return S_OK;
} // CPSFileSystem::SetFilePos

STDMETHODIMP CPSFileSystem::WriteData(LONG handle, char* data, ULONG datasize, BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  *ret = m_PSFS->WriteData(handle, data, (size_t)datasize);
  return S_OK;
} // CPSFileSystem::WriteData

STDMETHODIMP CPSFileSystem::ReadData(LONG handle, char* data, ULONG datasize, ULONG* bytesread, BOOL* ret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  size_t _bytesread = *bytesread;
  *ret = m_PSFS->ReadData(handle, data, (size_t)datasize, _bytesread);
  *bytesread = _bytesread;
  return S_OK;
} // CPSFileSystem::ReadData

STDMETHODIMP CPSFileSystem::PrintOut(BSTR* bret)
{
  if (m_PSFS.get() == nullptr)
    return S_FALSE;

  std::string ret = m_PSFS->PrintOut();

  *bret = SysAllocString(to_widestring(ret, std::locale("rus")).c_str());

  return S_OK;
} // CPSFileSystem::PrintOut

/////////////////////////////////////////////////////////////////////////////////////
// CPSFileSystemClassFactory implementation
/////////////////////////////////////////////////////////////////////////////////////

CPSFileSystemClassFactory::CPSFileSystemClassFactory()
  : m_ref(0)
{
} // CPSFileSystemClassFactory::CPSFileSystemClassFactory

CPSFileSystemClassFactory::~CPSFileSystemClassFactory()
{
} // CPSFileSystemClassFactory::~CPSFileSystemClassFactory

STDMETHODIMP CPSFileSystemClassFactory::QueryInterface(REFIID riid, void** ppv)
{
  *ppv = 0;

  if (riid == IID_IUnknown || riid == IID_IClassFactory)
    *ppv = this;

  if (*ppv)
  {
    AddRef();
    return S_OK;
  }

  return(E_NOINTERFACE);
} // CPSFileSystemClassFactory::QueryInterface

STDMETHODIMP_(ULONG) CPSFileSystemClassFactory::AddRef()
{
  return InterlockedIncrement(&m_ref);
} // CPSFileSystemClassFactory::AddRef

STDMETHODIMP_(ULONG) CPSFileSystemClassFactory::Release()
{
  if (InterlockedDecrement(&m_ref) == 0)
  {
    delete this;
    return 0;
  }

  return m_ref;
} // CPSFileSystemClassFactory::Release

STDMETHODIMP CPSFileSystemClassFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID riid, void** ppvObj)
{
  CPSFileSystem* fs;
  HRESULT hr;

  *ppvObj = 0;

  fs = new CPSFileSystem;

  if (fs == 0)
    return(E_OUTOFMEMORY);

  hr = fs->QueryInterface(riid, ppvObj);

  if (FAILED(hr))
    delete fs;

  return hr;
} // CPSFileSystemClassFactory::CreateInstance

STDMETHODIMP CPSFileSystemClassFactory::LockServer(BOOL fLock)
{
  if (fLock)
    InterlockedIncrement(&g_locks);
  else
    InterlockedDecrement(&g_locks);

  return S_OK;
} // CPSFileSystemClassFactory::LockServer