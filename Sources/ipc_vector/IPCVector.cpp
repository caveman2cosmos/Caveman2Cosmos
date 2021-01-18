#include "StdAfx.h"
#include "IPCVector.h"

extern ULONG g_Components;

IPCVector::IPCVector() : m_lRef(1) {
	InterlockedIncrement((LONG*)&g_Components);
}


IPCVector::~IPCVector() {
	InterlockedDecrement((LONG*)&g_Components);
}

// IUnknown
HRESULT STDMETHODCALLTYPE IPCVector::QueryInterface(REFIID riid, void** ppv) {
	HRESULT rc = S_OK;                                    // COM
	*ppv = NULL;                                          // COM
	if       (riid == IID_IUnknown)   *ppv = (IIPCVector*)this;
	else if  (riid == IID_IIPCVector) *ppv = (IIPCVector*)this;
	else rc = E_NOINTERFACE;                             // COM

	if (rc == S_OK) this->AddRef();
	//LOG("CA::QueryInterface rc = ",rc);
    return rc;
}

ULONG STDMETHODCALLTYPE IPCVector::AddRef() {
	InterlockedIncrement((LONG*)&(this->m_lRef));
	return this->m_lRef;
}

ULONG STDMETHODCALLTYPE IPCVector::Release() {
	InterlockedDecrement((LONG*)&(this->m_lRef));
	if (m_lRef == 0) {
		delete this;
		return 0;
	}
	else
		return m_lRef;
	//LOG("CA::AddRef rc = ",this->m_Ref);
	// LogCOM<<"CA::AddRef rc = "<<this->m_Ref<<std::endl;
	//return this->m_lRef;
}

// IPCVector
void STDMETHODCALLTYPE IPCVector::getAt(const int index, int& value) const {
	value = m_vector[index];
}

int STDMETHODCALLTYPE IPCVector::size() const {
	return m_vector.size();
}

void STDMETHODCALLTYPE IPCVector::push_back(int value) {
	m_vector.push_back(value);
}
