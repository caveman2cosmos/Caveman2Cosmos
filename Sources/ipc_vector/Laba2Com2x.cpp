// Laba2Com2x.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ClassFactory.h"
#include "IPCVector.h"
//#include "Registry.h"

ULONG g_ServerLocks = 0;
ULONG g_Components  = 0;

extern GUID CLSID_IPCVector;

STDAPI DllCanUnloadNow() {
	//SEQ;
	HRESULT rc = E_UNEXPECTED;
	if (g_ServerLocks == 0 && g_Components  == 0) rc = S_OK;
	else rc = S_FALSE;
	//LOG("DllCanUnloadNow: rc =",rc);
	//LogCOM<<"DllCanUnloadNow: rc = "<<rc<<std::endl;
	return rc;
};

STDAPI DllGetClassObject(const CLSID& clsid, const IID& iid, void** ppv) {
	//SEQ;
	HRESULT rc = E_UNEXPECTED;

	if (clsid == CLSID_IPCVector) {
		ClassFactory<IPCVector> *cf = new ClassFactory<IPCVector>();
		rc = cf->QueryInterface(iid, ppv);
		cf->Release();
	} 
	else  
		rc = CLASS_E_CLASSNOTAVAILABLE;

	return rc;
};