#include "CvGameCoreDLL.h"
#include "FProfiler.h"

#include "CvDefines.h"
#include "CvHeritageInfo.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

CvHeritageInfo::CvHeritageInfo()
	:
	 m_PropertyManipulators()
	,m_iPrereqTech(NO_TECH)
{ }

CvHeritageInfo::~CvHeritageInfo() 
{
	GC.removeDelayedResolutionVector(m_prereqOrHeritage);
}


void CvHeritageInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();

	m_PropertyManipulators.getCheckSum(iSum);

	CheckSum(iSum, m_iPrereqTech);

	CheckSumC(iSum, m_prereqOrHeritage);
	CheckSumC(iSum, m_eraCommerceChanges);
}

bool CvHeritageInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	CvString szTextVal;

	m_PropertyManipulators.read(pXML);

	pXML->GetOptionalChildXmlValByName(&m_bNeedLanguage, L"bNeedLanguage");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_iPrereqTech = pXML->GetInfoClass(szTextVal);

	pXML->SetOptionalVectorWithDelayedResolution(m_prereqOrHeritage, L"PrereqOrHeritage");

	m_eraCommerceChanges.readPairedArrays(pXML, L"EraCommerceChanges", L"EraType", L"CentiCommerce");

	return true;
}

void CvHeritageInfo::copyNonDefaults(const CvHeritageInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();

	CvInfoBase::copyNonDefaults(pClassInfo);
	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators());

	if (m_iPrereqTech == -1) m_iPrereqTech = pClassInfo->getPrereqTech();

	GC.copyNonDefaultDelayedResolutionVector(m_prereqOrHeritage, pClassInfo->getPrereqOrHeritage());

	m_eraCommerceChanges.copyNonDefaults(pClassInfo->getEraCommerceChanges100());
}

void CvHeritageInfo::doPostLoadCaching(uint32_t iThis)
{
}