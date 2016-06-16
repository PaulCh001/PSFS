// PSFS_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <initguid.h>
#include <comutil.h>
#include "..\PSFS\IPSFScomp.h"

#define TestSuccess(call,ret)\
{\
  std::wcout << #call << " - ";\
  call;\
  if (ret)\
    std::wcout << "success" << std::endl;\
  else\
    std::wcout << "ERROR" << std::endl; \
}

#define TestFail(call,ret)\
{\
  std::wcout << #call << " - ";\
  call;\
  if (!ret)\
    std::wcout << "success" << std::endl;\
    else\
    std::wcout << "ERROR" << std::endl; \
}

TCHAR* DATA_FILE_NAME = _T("c:\\_stuff\\data.dat");


int _tmain(int argc, _TCHAR* argv[])
{
  DeleteFile(DATA_FILE_NAME);

  std::wcout << _T("Initializing COM") << std::endl;

  if (FAILED(CoInitialize(NULL)))
  {
    std::wcout << _T("Unable to initialize COM") << std::endl;
    return -1;
  }

  CLSID  clsid;
  HRESULT hr = ::CLSIDFromProgID(_T("PSFS.1"), &clsid);
  if (FAILED(hr))
  {
    std::wcout.setf(std::ios::hex, std::ios::basefield);
    std::wcout << _T("Unable to get CLSID from ProgID. HR = ") << hr << std::endl;

    return -1;
  }

  IClassFactory* class_factory;
  // Получить фабрику классов 
  hr = CoGetClassObject(clsid, CLSCTX_INPROC, nullptr, IID_IClassFactory, (void**)&class_factory);
  if (FAILED(hr))
  {
    std::wcout.setf(std::ios::hex, std::ios::basefield);
    std::wcout << _T("Failed to GetClassObject server instance. HR = ") << hr << std::endl;

    return -1;
  }

  // с помощью фабрики классов создать экземпляр
  // компонента и получить интерфейс IUnknown.
  IUnknown* unk1;
  hr = class_factory->CreateInstance(NULL, IID_IUnknown, (void**)&unk1);

  if (FAILED(hr))
  {
    std::wcout.setf(std::ios::hex, std::ios::basefield);
    std::wcout << _T("Failed to create server instance. HR = ") << hr << std::endl;

    return -1;
  }

  IUnknown* unk2;
  hr = class_factory->CreateInstance(NULL, IID_IUnknown, (void**)&unk2);


  if (FAILED(hr))
  {
    std::wcout.setf(std::ios::hex, std::ios::basefield);
    std::wcout << _T("Failed to create server instance. HR = ") << hr << std::endl;

    return -1;
  }

  // Release the class factory
  class_factory->Release();

  std::wcout << _T("----------------------") << std::endl;
  std::wcout << _T("Unit test start...") << std::endl;
  std::wcout << _T("----------------------") << std::endl;

  std::wcout << _T("Instances creating...") << std::endl;

  IPSFileSystem* fs = nullptr;
  hr = unk1->QueryInterface(IID_IPSFileSystem, (LPVOID*)&fs);

  unk1->Release();

  if (FAILED(hr))
  {
    std::wcout << _T("QueryInterface() for IPSFileSystem failed") << std::endl;
    return -1;
  }

  IPSFileSystem* fs2 = nullptr;
  hr = unk2->QueryInterface(IID_IPSFileSystem, (LPVOID*)&fs2);

  unk2->Release();

  if (FAILED(hr))
  {
    std::wcout << _T("QueryInterface() for IPSFileSystem failed") << std::endl;
    return -1;
  }

  std::wcout << _T("Instances created.") << std::endl;

  BOOL ret;
  BSTR str;

  fs->LoadFromFile(DATA_FILE_NAME, &ret);

  // creating folders
  std::wcout << _T("CREATING FOLDERS TEST") << std::endl;
  TestSuccess(fs->CreateFolder(_T("\\Photo\\Summer2015"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Photo\\Winter 2015 - 2016"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Photo\\to-delete"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Video"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Video\\codecs\\i386"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Video\\codecs\\powerpc"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Video\\codecs\\amd64"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Some sfuff"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Some sfuff\\junk"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Music"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Music\\Good Old Songs"), &ret), ret);
  TestSuccess(fs->CreateFolder(_T("\\Music\\Frank Sinatra"), &ret), ret);
  TestFail(fs->CreateFolder(_T(""), &ret), ret);
  TestFail(fs->CreateFolder(_T("\\"), &ret), ret);
  TestFail(fs->CreateFolder(_T("\\Photo\\Summer2015"), &ret), ret);

  fs->PrintOut(&str);
  std::wcout << _T("**** object content ****") << std::endl << _bstr_t(str) << _T("************************") << std::endl;
  ::SysFreeString(str);

  // deleting folders
  std::wcout << _T("DELETING FOLDERS TEST") << std::endl;
  TestSuccess(fs->DeleteItem(_T("\\Photo\\to-delete"), &ret), ret);
  TestSuccess(fs->DeleteItem(_T("\\Some sfuff"), &ret), ret);
  TestFail(fs->DeleteItem(_T(""), &ret), ret);
  TestFail(fs->DeleteItem(_T("\\"), &ret), ret);
  TestFail(fs->DeleteItem(_T("\\Some sfuff"), &ret), ret);

  fs->PrintOut(&str);
  std::wcout << _T("**** object content ****") << std::endl << _bstr_t(str) << _T("************************") << std::endl;
  ::SysFreeString(str);

  // moving folders
  TestSuccess(fs->CopyItem(_T("\\Music\\Frank Sinatra"), _T("\\Music\\Good Old Songs"), TRUE, &ret), ret);
  TestFail(fs->CopyItem(_T("\\Music\\Frank Sinatra"), _T("\\Music\\Good Old Songs"), TRUE, &ret), ret);

  // copying folders
  TestSuccess(fs->CopyItem(_T("\\Video\\codecs"), _T("\\Music"), FALSE, &ret), ret);
  TestFail(fs->CopyItem(_T("\\Video\\codecs"), _T("\\Music"), FALSE, &ret), ret);

  fs->PrintOut(&str);
  std::wcout << _T("**** object content ****") << std::endl << _bstr_t(str) << _T("************************") << std::endl;
  ::SysFreeString(str);

  // writing file

  LONG handle;
  TestSuccess(fs->OpenFile(_T("\\Music\\Good Old Songs\\Frank Sinatra\\New York, New York"), _T("w"), &handle), handle);
  LONG handle1;
  TestFail(fs->OpenFile(_T("\\Music\\Good Old Songs\\Frank Sinatra\\New York, New York"), _T("w"), &handle1), handle1);

  char buffer1[] =
    "I want to wake up in a city,\n"
    "that never sleeps\n";

  TestSuccess(fs->WriteData(handle, buffer1, sizeof(buffer1)-1, &ret), ret);
  TestSuccess(fs->CloseFile(handle, &ret), ret);

  TestSuccess(fs->OpenFile(_T("\\Music\\Good Old Songs\\Frank Sinatra\\New York, New York"), _T("a"), &handle), handle);

  char buffer2[] =
    "And find I'm A, number one,\n"
    "top of the list\n";

  TestSuccess(fs->WriteData(handle, buffer2, sizeof(buffer2)-1, &ret), ret);
  TestSuccess(fs->CloseFile(handle, &ret), ret);

  TestSuccess(fs->OpenFile(_T("\\Music\\Good Old Songs\\Frank Sinatra\\New York, New York"), _T("a"), &handle), handle);

  char buffer3[] =
    "king of the hill, A number one";

  TestSuccess(fs->WriteData(handle, buffer3, sizeof(buffer3) - 1, &ret), ret);
  TestSuccess(fs->CloseFile(handle, &ret), ret);

  TestFail(fs->OpenFile(_T("\\Music\\Good Old Songs\\Frank Sinatra\\New York, New York.txt"), _T("r"), &handle), handle);

  TestFail(fs->OpenFile(_T("\\Music\\Good Old Songs\\Frank Sinatra\\Kalinka"), _T("r"), &handle), handle);

  // writing and reding file

  TestSuccess(fs->OpenFile(_T("\\Video\\readme.txt"), _T("w"), &handle), handle);
  TestFail(fs->OpenFile(_T("\\Video\\readme.txt"), _T("r"), &handle1), handle1);

  char buffer4[] =
    "1234567890";

  TestSuccess(fs->WriteData(handle, buffer4, sizeof(buffer4) - 1, &ret), ret);
  ULONG filesize;
  TestSuccess(fs->GetFileSize(handle, &filesize, &ret), ret);
  std::wcout << _T("Expected filesize is 10. Real is ") << filesize << std::endl;
  TestSuccess(fs->CloseFile(handle, &ret), ret);

  TestSuccess(fs->OpenFile(_T("\\Video\\readme.txt"), _T("r"), &handle), handle);

  ULONG totalread = 0;
  char* totalbuffer = new char[1024];

  const int READ_BUFFER_SIZE = 8;

  char* buf = new char[READ_BUFFER_SIZE];
  ULONG bytesread;

  do 
  {
    TestSuccess(fs->ReadData(handle, buf, READ_BUFFER_SIZE, &bytesread, &ret), ret);
    memcpy(totalbuffer + totalread, buf, bytesread);
    totalread += bytesread;
  } while (bytesread > 0);

  TestSuccess(fs->CloseFile(handle, &ret), ret);

  std::wcout << _T("Expected bytes read is 10. Real is ") << totalread << std::endl;

  fs->PrintOut(&str);
  std::wcout << _T("**** object content ****") << std::endl << _bstr_t(str) << _T("************************") << std::endl;
  ::SysFreeString(str);

  TestSuccess(fs->OpenFile(_T("\\Video\\readme.txt"), _T("w"), &handle), handle);

  char buffer5[] =
    "abcd";

  TestFail(fs->SetFilePos(handle, 1000, &ret), ret);
  TestFail(fs->SetFilePos(1000, 1000, &ret), ret);
  TestSuccess(fs->SetFilePos(handle, 8, &ret), ret);
  TestSuccess(fs->WriteData(handle, buffer5, sizeof(buffer5) - 1, &ret), ret);
  TestSuccess(fs->GetFileSize(handle, &filesize, &ret), ret);
  std::wcout << _T("Expected filesize is 12. Real is ") << filesize << std::endl;
  TestSuccess(fs->CloseFile(handle, &ret), ret);

  fs->PrintOut(&str);
  std::wcout << _T("**** object content ****") << std::endl << _bstr_t(str) << _T("************************") << std::endl;
  ::SysFreeString(str);


  // closing object
  std::wcout << _T("Releasing instance") << std::endl;
  fs->Release();

  // loading second object
  fs2->LoadFromFile(DATA_FILE_NAME, &ret);

  fs2->PrintOut(&str);
  std::wcout << _T("**** object 2 content ****") << std::endl << _bstr_t(str) << _T("************************") << std::endl;
  ::SysFreeString(str);

  std::wcout << _T("----------------------") << std::endl;
  std::wcout << _T("Unit test end") << std::endl;
  std::wcout << _T("----------------------") << std::endl;

  CoUninitialize();

  return 0;
}

