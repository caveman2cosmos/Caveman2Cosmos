#pragma once

#include <ObjBase.h>

extern ULONG g_ServerLocks;

template <typename T>
class ClassFactory :
	public IClassFactory // standart interface IClassFactory, provides creating components for outter requests
{
protected:
   long m_lRef; // Reference count

public:
	ClassFactory();
	~ClassFactory();

	//IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv);
	virtual ULONG   STDMETHODCALLTYPE AddRef();
	virtual ULONG   STDMETHODCALLTYPE Release();

	/*Basic interface IClassFactory contains 2 methods:*/
	virtual HRESULT STDMETHODCALLTYPE CreateInstance(LPUNKNOWN pUnk, const IID& id, void** ppv); // creates component instance
	virtual HRESULT STDMETHODCALLTYPE LockServer (BOOL fLock);
};

template <typename T>
ClassFactory<T>::ClassFactory() : m_lRef(1) {
}

template <typename T>
ClassFactory<T>::~ClassFactory() {
}

//IUnknown
template <typename T>
HRESULT STDMETHODCALLTYPE ClassFactory<T>::QueryInterface(REFIID riid, void** ppv) {
	HRESULT rc = S_OK;

	if (riid == IID_IUnknown) *ppv = (IUnknown*)this;
	else if (riid == IID_IClassFactory) *ppv = (IClassFactory*)this;
	else rc = E_NOINTERFACE;

	if (rc == S_OK)
		this->AddRef();

	return rc;
}

template <typename T>
ULONG   STDMETHODCALLTYPE ClassFactory<T>::AddRef() {
	InterlockedIncrement(&m_lRef);

	return this->m_lRef;
}

template <typename T>
ULONG STDMETHODCALLTYPE ClassFactory<T>::Release() {
	InterlockedDecrement(&m_lRef);

	if (this->m_lRef == 0) {
		delete this;
		return 0;
	}
	else
		return this->m_lRef;
}

template <typename T>
HRESULT STDMETHODCALLTYPE ClassFactory<T>::CreateInstance(LPUNKNOWN pUnk, //basic interface IUnknown
															const IID& id,	// id of required interface
															void** ppv)	//required interface
{
	//SEQ;
	HRESULT rc = E_UNEXPECTED;

	if (pUnk != NULL) rc = CLASS_E_NOAGGREGATION;
	else {
		T* p;
		if ((p = new T()) == NULL) {
			rc = E_OUTOFMEMORY;
		}
		else {
			rc = p->QueryInterface(id,ppv);
			p->Release();
		}
	}
	return rc;
}

template <typename T>
HRESULT STDMETHODCALLTYPE ClassFactory<T>::LockServer(BOOL fLock) {
	if (fLock)  InterlockedIncrement ((LONG*)&(g_ServerLocks));
	else    InterlockedDecrement ((LONG*)&(g_ServerLocks));
	
	return S_OK;
}
