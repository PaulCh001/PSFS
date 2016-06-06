// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <olectl.h>
#include <tchar.h>
#include "PSFScomp.h"

HINSTANCE h_instance;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	  case DLL_PROCESS_ATTACH:
      h_instance = (HINSTANCE)hModule;
      break;
	  case DLL_THREAD_ATTACH:
	  case DLL_THREAD_DETACH:
	  case DLL_PROCESS_DETACH:
		  break;
	}
	return TRUE;
}

long g_objs_count = 0;
long g_locks = 0;

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
	if (rclsid != CLSID_PSFileSystem)
		return(E_FAIL);

  auto class_factory = new CPSFileSystemClassFactory;

  if (! class_factory)
		return(E_OUTOFMEMORY);

  HRESULT hr = class_factory->QueryInterface(riid, ppv);

	if (FAILED(hr))
	{
		delete class_factory;
    class_factory = nullptr;
	}

	return hr;
} // DllGetClassObject

STDAPI DllCanUnloadNow(void)
{
  if (g_objs_count || g_locks)
		return(S_FALSE);
	else
		return(S_OK);
} // DllCanUnloadNow

const size_t GUID_STRLEN = 39;

#define _CRT_SECURE_NO_WARNINGS

STDAPI DllUnregisterServer(VOID)
{
  OLECHAR szCLSID[GUID_STRLEN];
  StringFromGUID2(CLSID_PSFileSystem, szCLSID, sizeof(szCLSID));

  TCHAR key_name[1024];

  // class id

  _stprintf(key_name, _T("CLSID\\%s\\InprocServer32"), szCLSID);
  ::RegDeleteKey(HKEY_CLASSES_ROOT, key_name);

  _stprintf(key_name, _T("CLSID\\%s\\ProgID"), szCLSID);
  ::RegDeleteKey(HKEY_CLASSES_ROOT, key_name);

  _stprintf(key_name, _T("CLSID\\%s"), szCLSID);
  ::RegDeleteKey(HKEY_CLASSES_ROOT, key_name);

  // progid

  TCHAR* prog_id_ver = _T("PSFS.1");

  _stprintf(key_name, _T("%s\\CurVer"), prog_id_ver);
  ::RegDeleteKey(HKEY_CLASSES_ROOT, key_name);

  _stprintf(key_name, _T("%s\\CLSID"), prog_id_ver);
  ::RegDeleteKey(HKEY_CLASSES_ROOT, key_name);

  _stprintf(key_name, _T("%s"), prog_id_ver);
  ::RegDeleteKey(HKEY_CLASSES_ROOT, key_name);


  return S_OK;
} // DllUnregisterServer

STDAPI DllRegisterServer(VOID)
{
  DllUnregisterServer();

  OLECHAR szCLSID[GUID_STRLEN];
  StringFromGUID2(CLSID_PSFileSystem, szCLSID, sizeof(szCLSID));

  HKEY hKey;

  TCHAR key_name[1024];

  TCHAR* prog_id_ver = _T("PSFS.1");
  TCHAR* com_title = _T("PSFileSystem object");

  // class id

  _stprintf(key_name, _T("CLSID\\%s"), szCLSID);

  if (::RegCreateKey(HKEY_CLASSES_ROOT, key_name, &hKey) != ERROR_SUCCESS)
    return SELFREG_E_CLASS; 

  if (::RegSetValue(hKey, NULL, REG_SZ, com_title, sizeof(com_title)) != ERROR_SUCCESS)
    return SELFREG_E_CLASS;

  TCHAR filename[MAX_PATH];
  DWORD len = ::GetModuleFileName(h_instance, filename, MAX_PATH);

  if (::RegSetValue(hKey, _T("InprocServer32"), REG_SZ, filename, len) != ERROR_SUCCESS)
    return SELFREG_E_CLASS;

  if (::RegSetValue(hKey, _T("ProgID"), REG_SZ, prog_id_ver, sizeof(prog_id_ver)) != ERROR_SUCCESS)
    return SELFREG_E_CLASS;

  // progid

  _stprintf(key_name, _T("%s"), prog_id_ver);

  if (::RegCreateKey(HKEY_CLASSES_ROOT, key_name, &hKey) != ERROR_SUCCESS)
    return SELFREG_E_CLASS;

  if (::RegSetValue(hKey, NULL, REG_SZ, com_title, sizeof(com_title)) != ERROR_SUCCESS)
    return SELFREG_E_CLASS;

  if (::RegSetValue(hKey, _T("CurVer"), REG_SZ, prog_id_ver, sizeof(prog_id_ver)) != ERROR_SUCCESS)
    return SELFREG_E_CLASS;

  if (::RegSetValue(hKey, _T("CLSID"), REG_SZ, szCLSID, sizeof(szCLSID)) != ERROR_SUCCESS)
    return SELFREG_E_CLASS;

  ::RegCloseKey(hKey);

  return S_OK;
} // DllRegisterServer



