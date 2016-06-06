//
// PSFScomp.h
//

#ifndef __PSFScomp_H__
#define __PSFScomp_H__

#include "IPSFScomp.h"
#include "PSFSimpl.h"

extern long g_objs_count;
extern long g_locks;


class CPSFileSystem : public IPSFileSystem
{
protected:
   // Reference count
   long m_ref;

public:
   CPSFileSystem();
   virtual ~CPSFileSystem();

public:
   // IUnknown
   STDMETHOD( QueryInterface(REFIID, void** ));
   STDMETHOD_(ULONG, AddRef());
   STDMETHOD_(ULONG, Release());

   // IPSFileSystem
   STDMETHOD(LoadFromFile(TCHAR*, BOOL* result));
   STDMETHOD(CreateFolder(TCHAR*, BOOL* result));
   STDMETHOD(Flush(BOOL*));
   STDMETHOD(DeleteItem(TCHAR*, BOOL* result));
   STDMETHOD(CopyItem(TCHAR* src, TCHAR* dest, BOOL is_move, BOOL* result));

   STDMETHOD(OpenFile(TCHAR* path, TCHAR* mode, LONG* handle));  // mode: "r" - reading, "w" - writing (overwrite), "a" - writing (append)
   STDMETHOD(CloseFile(LONG handle, BOOL*));
   STDMETHOD(GetFileSize(LONG handle, ULONG* size, BOOL*));
   STDMETHOD(SetFilePos(LONG handle, ULONG pos, BOOL*));
   STDMETHOD(WriteData(LONG handle, char* data, ULONG datasize, BOOL*));
   STDMETHOD(ReadData(LONG handle, char* data, ULONG datasize, ULONG* bytesread, BOOL*));

   STDMETHOD(PrintOut(BSTR*));

private:
  CPSFS_impl* m_PSFS;
}; // CPSFileSystem

class CPSFileSystemClassFactory : public IClassFactory
{
protected:
   long m_ref;

public:
   CPSFileSystemClassFactory();
   virtual ~CPSFileSystemClassFactory();

   // IUnknown
   STDMETHOD( QueryInterface(REFIID, void** ));
   STDMETHOD_(ULONG, AddRef());
   STDMETHOD_(ULONG, Release());

   // IClassFactory
   STDMETHOD( CreateInstance(LPUNKNOWN, REFIID, void**));
   STDMETHOD( LockServer(BOOL));
}; // CPSFileSystem

#endif