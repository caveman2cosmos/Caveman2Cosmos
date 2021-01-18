#pragma once
#include <ObjBase.h>

HMODULE hIPCVector;

// {6942E971-6F95-44BC-B3A9-EFD270EB39C9}
GUID CLSID_IPCVector = { 0x6942e971, 0x6f95, 0x44bc, { 0xb3, 0xa9, 0xef, 0xd2, 0x70, 0xeb, 0x39, 0xc9 } };

const WCHAR FName[]  = L"IPCVector COM";
const WCHAR VerInd[] = L"IPCVector.Component";
const WCHAR ProgId[] = L"IPCVector.Component.1";

//extern "C"  __declspec(dllexport) HRESULT DllInstall(char* s);
//extern "C"  __declspec(dllexport) HRESULT DllRegisterServer();
//extern "C"  __declspec(dllexport) HRESULT DllUnregisterServer();
STDAPI DllInstall(char* s);
STDAPI DllRegisterServer();
STDAPI DllUnregisterServer();
// This function will register a component in the Registry.
// The component calls this function from its DllRegisterServer function.
HRESULT RegisterServer(HMODULE hModule, 
                       const CLSID& clsid, 
                       const WCHAR* szFriendlyName,
                       const WCHAR* szVerIndProgID,
                       const WCHAR* szProgID);

// This function will unregister a component.  Components
// call this function from their DllUnregisterServer function.
HRESULT UnregisterServer(const CLSID& clsid,
                         const WCHAR* szVerIndProgID,
                         const WCHAR* szProgID);