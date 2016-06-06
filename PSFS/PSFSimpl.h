//
// PSFSimpl.h
//

#ifndef __PSFSimpl_H__
#define __PSFSimpl_H__

#include <list>
#include <vector>

class CPSFS_item
{
public:
  enum PSFS_itemType
  {
    psfs_itemFolder,
    psfs_itemFile,
  };

public:
  CPSFS_item(PSFS_itemType _type = psfs_itemFolder, const std::string& _name = "", const std::string& _owner_path = "") : type(_type), name(_name), owner_path(_owner_path) {}
  virtual ~CPSFS_item() {}

public:
  virtual PSFS_itemType GetItemType() const { return type; }
  virtual std::string   PrintOut(int shift) const; // for debug purposes

public:
  bool IsRootItem() const { return (owner_path == std::string("")); }

  std::string GetName() const; // returns the name of the item ('\' for root folder)
  std::string GetOwnerPath() const { return owner_path; }
  std::string GetFullName() const; // returns the full path and name of this item 

public:
  bool        CreateFolderByList(std::list<std::string>& path_list);
  bool        CreateFileByList(std::list<std::string>& path_list);
  CPSFS_item* GetItemByList(std::list<std::string>& path_list) const;
  CPSFS_item* GetChildItem(const std::string& name) const;

  void        InsertItem(CPSFS_item*);
  void        DetachItem(CPSFS_item*);

  void        SetData(size_t pos, char* data, size_t datasize);
  void        GetData(size_t pos, char* data, size_t datasize, size_t& bytesread);
  size_t      GetContentSize() { return content.size(); }

public:
  //friend class boost::serialization::access;
  // When the class Archive corresponds to an output archive, the
  // & operator is defined similar to <<.  Likewise, when the class Archive
  // is a type of input archive the & operator is defined similar to >>.
  template<class Archive>
  void serialize(Archive & ar, const unsigned int)
  {
    ar & type;
    ar & name;
    ar & owner_path;
    ar & childs;
    ar & content;
  }

private:
  PSFS_itemType type;
  std::string   name;
  std::string   owner_path;

  std::map <std::string, CPSFS_item> childs;
  std::vector<char>                  content;
}; // CPSFS_item


///////////////////////////////////////////////////

struct CPSFSLock
{
  
  CPSFS_item* file;
  bool        is_lock_for_writing; // otherwise - for reading
  size_t      file_position;

  CPSFSLock(CPSFS_item* _file = nullptr, bool _is_lock_for_writing = false, size_t _file_position = 0) : file(_file), is_lock_for_writing(_is_lock_for_writing), file_position(_file_position) {}
}; // CPSFSLock

///////////////////////////////////////////////////

class CPSFS_impl
{
public:
  CPSFS_impl(const std::string& fname);
  ~CPSFS_impl();

public:
  void  Flush();

  bool CreateFolder(const std::string& path);
  bool CreateFile(const std::string& path);
  bool DeleteItem(const std::string& path);
  bool CopyItem(const std::string& src, const std::string& dest, bool is_move);

  long OpenFile(const std::string& path, const std::string& mode);
  bool CloseFile(long handle);
  bool GetFileSize(long handle, size_t& size);
  bool SetFilePos(long handle, size_t pos);
  bool WriteData(long handle, char* data, size_t datasize);
  bool ReadData(long handle, char* data, size_t datasize, size_t& bytesread);

  // for debug proposes
  std::string PrintOut() const;

  static bool SplitPath(const std::string& path, std::list<std::string>& folders_list, bool& is_absolute_path);

private:
  void LoadFromFile();
  void StoreToFile();

  CPSFS_item* GetItem(const std::string& path) const;

private:
  std::string m_FileName;

  CPSFS_item  m_RootFolder;

  std::map<long, CPSFSLock> m_Locks;
  long m_LastHandle;
}; // CPSFS_impl




#endif



