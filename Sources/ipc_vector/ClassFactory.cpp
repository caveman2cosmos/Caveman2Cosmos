#include "StdAfx.h"
//#include "ClassFactory.h"
//#include "BVAA.h"
//#include "BVAB.h"
//#include "COMDLL.h"
//
//ClassFactory::ClassFactory(void) : m_lRef (1)
//{
//}
//
//
//ClassFactory::~ClassFactory(void)
//{
//}
//
////IUnknown
//HRESULT STDMETHODCALLTYPE ClassFactory::QueryInterface(REFIID riid,void  **ppv)
//{
//	HRESULT rc = S_OK;
//
//	if (riid == IID_IUnknown) *ppv = (IUnknown*)this;
//	else if (riid == IID_IClassFactory) *ppv = (IClassFactory*)this;
//	else rc = E_NOINTERFACE;
//
//	if (rc == S_OK)
//		this->AddRef();
//
//	return rc;
//}
//
//ULONG   STDMETHODCALLTYPE ClassFactory::AddRef(void)
//{
//	InterlockedIncrement(&m_lRef);
//
//	return this->m_lRef;
//}
//
//ULONG   STDMETHODCALLTYPE ClassFactory::Release(void)
//{
//	InterlockedDecrement(&m_lRef);
//
//	if ( this->m_lRef == 0)
//	{
//		delete this;
//		return 0;
//	}
//	else
//		return this->m_lRef;
//}
//
//HRESULT STDMETHODCALLTYPE ClassFactory::CreateInstance(LPUNKNOWN pUnk, const IID& id, void** ppv)
//{
//	//SEQ;
//	HRESULT rc = E_UNEXPECTED;
//
//	if (pUnk != NULL) rc = CLASS_E_NOAGGREGATION;
//	else if (id == IID_IBVAA_summer || id == IID_IBVAA_multiplier)
//	{
//		BVAA* pA;
//		if ((pA = new BVAA()) == NULL)
//			rc = E_OUTOFMEMORY;
//		else
//		{
//			rc = pA->QueryInterface(id,ppv);
//			pA->Release();
//		}
//	}
//	else if (id == IID_IBVAB_power || id == IID_IUnknown)
//	{
//		BVAB* pB;
//		if ((pB = new BVAB()) == NULL)
//			rc = E_OUTOFMEMORY;
//		else
//		{
//			rc = pB->QueryInterface(id,ppv);
//			pB->Release();
//		}
//	}
//
//
//	//LOG("CFactory::CreateInstance rc = ",rc);
//	// LogCOM<<CSeq<<":CFactory::CreateInstance rc = "<<rc<<std::endl;
//	return rc;
//}
//
//HRESULT STDMETHODCALLTYPE ClassFactory::LockServer (BOOL fLock)
//{
//	//SEQ;
//	if (fLock)  InterlockedIncrement ((LONG*)&(g_ServerLocks));
//	else    InterlockedDecrement ((LONG*)&(g_ServerLocks));
//	//LOG("CFactory::LockServer  rc = ",S_OK);
//	// LogCOM<<CSeq<<":CFactory::LockServer rc = "<<S_OK<<std::endl;
//	return S_OK;
//}