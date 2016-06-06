//
// IPSFScomp.h
//

#ifndef __IPSFScomp_H__
#define __IPSFScomp_H__

#include <Unknwn.h>

// {B372482E-E9C9-4BF3-B222-7EE9DF39632E}
DEFINE_GUID(CLSID_PSFileSystem,
  0xb372482e, 0xe9c9, 0x4bf3, 0xb2, 0x22, 0x7e, 0xe9, 0xdf, 0x39, 0x63, 0x2e);

// {B55B48CD-82BF-4010-85CB-1EA3059E625F}
DEFINE_GUID(IID_IPSFileSystem,
  0xb55b48cd, 0x82bf, 0x4010, 0x85, 0xcb, 0x1e, 0xa3, 0x5, 0x9e, 0x62, 0x5f);

class IPSFileSystem : public IUnknown
{
public:
  STDMETHOD(LoadFromFile(TCHAR*, BOOL*)) PURE;
  STDMETHOD(CreateFolder(TCHAR*, BOOL*)) PURE;
  STDMETHOD(DeleteItem(TCHAR*, BOOL*)) PURE;
  STDMETHOD(CopyItem(TCHAR* src, TCHAR* dest, BOOL is_move, BOOL*)) PURE;
  STDMETHOD(OpenFile(TCHAR* path, TCHAR* mode, LONG* handle)) PURE;
  STDMETHOD(CloseFile(LONG handle, BOOL*)) PURE;
  STDMETHOD(GetFileSize(LONG handle, ULONG* size, BOOL*)) PURE;
  STDMETHOD(SetFilePos(LONG handle, ULONG pos, BOOL*)) PURE;
  STDMETHOD(WriteData(LONG handle, char* data, ULONG datasize, BOOL*)) PURE;
  STDMETHOD(ReadData(LONG handle, char* data, ULONG datasize, ULONG* bytesread, BOOL*)) PURE;
  STDMETHOD(PrintOut(BSTR*)) PURE;
 
};


#endif
