//------------------------------------------------------------------------------------------------
//  FILE:    CvGameOptionInfo.cpp
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvHeritageInfo.h"
#include "CvGameAI.h"
#include "CvGameTextMgr.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvInfoUtil.h"
#include "CvPlayerAI.h"
#include "CvPython.h"
#include "CvXMLLoadUtility.h"
#include "CvXMLLoadUtilityModTools.h"
#include "CheckSum.h"
#include "CvImprovementInfo.h"
#include "CvBonusInfo.h"
#include "CvGameOptionInfo.h"


//////////////////////////////////////////////////////////////////////////
//
//	CvGameOptionInfo
//	Game options and their default values
//
//
CvGameOptionInfo::CvGameOptionInfo() :
m_bDefault(false),
m_bVisible(true)
{
}


CvGameOptionInfo::~CvGameOptionInfo()
{
	GC.removeDelayedResolutionVector(m_aEnforcesGameOptionOnTypes);
	GC.removeDelayedResolutionVector(m_aEnforcesGameOptionOffTypes);
}


bool CvGameOptionInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(&m_bDefault, L"bDefault");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible", true);

	pXML->SetOptionalVectorWithDelayedResolution(m_aEnforcesGameOptionOnTypes, L"EnforcesGameOptionOnTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aEnforcesGameOptionOffTypes, L"EnforcesGameOptionOffTypes");

	return true;
}


void CvGameOptionInfo::copyNonDefaults(const CvGameOptionInfo* pClassInfo)
{
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
	if (getVisible()) m_bVisible = pClassInfo->getVisible();

	//TB's Tags
	GC.copyNonDefaultDelayedResolutionVector(m_aEnforcesGameOptionOnTypes, pClassInfo->m_aEnforcesGameOptionOnTypes);
	GC.copyNonDefaultDelayedResolutionVector(m_aEnforcesGameOptionOffTypes, pClassInfo->m_aEnforcesGameOptionOffTypes);
}


void CvGameOptionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bDefault);
	CheckSum(iSum, m_bVisible);

	//TB's Tags
	CheckSumC(iSum, m_aEnforcesGameOptionOnTypes);
	CheckSumC(iSum, m_aEnforcesGameOptionOffTypes);
}

