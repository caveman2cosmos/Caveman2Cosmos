#include "CvGameCoreDLL.h"
#include "FProfiler.h"

#include "CvDefines.h"
#include "CvHeritageInfo.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvHeritageInfo::CvHeritageInfo()
:
 m_PropertyManipulators()
{ }

CvHeritageInfo::~CvHeritageInfo() 
{
}


void CvHeritageInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();

	m_PropertyManipulators.getCheckSum(iSum);
	CheckSumC(iSum, m_techCommerceChanges);
}

bool CvHeritageInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	m_PropertyManipulators.read(pXML);

	m_techCommerceChanges.readPairedArrays(pXML, L"TechCommerceChanges", L"TechType", L"CommercePercents");

	return true;
}

void CvHeritageInfo::copyNonDefaults(const CvHeritageInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();

	CvInfoBase::copyNonDefaults(pClassInfo);
	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);

	m_techCommerceChanges.copyNonDefaults(pClassInfo->getTechCommerceChanges100());
}

void CvHeritageInfo::doPostLoadCaching(uint32_t iThis)
{
}