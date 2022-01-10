#include "CvGameCoreDLL.h"
#include "CvMapExternal.h"
#include "CvMapInterfaceBase.h"

CvMapInterfaceBase::CvMapInterfaceBase()
{
	m_proxy = new CvMapExternal(this);
}

CvMapInterfaceBase::~CvMapInterfaceBase()
{
	SAFE_DELETE(m_proxy);
}

CvMapExternal& CvMapInterfaceBase::getProxy() const
{
	return *m_proxy;
}