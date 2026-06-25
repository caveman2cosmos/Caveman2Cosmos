//------------------------------------------------------------------------------------------------
//  FILE:    CvForceControlInfo.cpp
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
#include "CvForceControlInfo.h"


//////////////////////////////////////////////////////////////////////////
//
//	CvForceControlInfo
//	Forced Controls and their default values
//
//
CvForceControlInfo::CvForceControlInfo() :
m_bDefault(false)
{
}


CvForceControlInfo::~CvForceControlInfo()
{
}


bool CvForceControlInfo::getDefault() const
{
	return m_bDefault;
}


bool CvForceControlInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}


void CvForceControlInfo::copyNonDefaults(const CvForceControlInfo* pClassInfo)
{
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}


void CvForceControlInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bDefault);
}

