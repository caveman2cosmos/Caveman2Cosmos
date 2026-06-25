//------------------------------------------------------------------------------------------------
//  FILE:    CvGraphicOptionInfo.cpp
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
#include "CvGraphicOptionInfo.h"


//////////////////////////////////////////////////////////////////////////
//
//	CvPlayerOptionInfo
//	Player options and their default values
//
//

//////////////////////////////////////////////////////////////////////////
//
//	CvGraphicOptionInfo
//	Graphic options and their default values
//
//
CvGraphicOptionInfo::CvGraphicOptionInfo() :
m_bDefault(false)
{
}


CvGraphicOptionInfo::~CvGraphicOptionInfo()
{
}


bool CvGraphicOptionInfo::getDefault() const
{
	return m_bDefault;
}


bool CvGraphicOptionInfo::read(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bDefault, L"bDefault");

	return true;
}


void CvGraphicOptionInfo::copyNonDefaults(const CvGraphicOptionInfo* pClassInfo)
{
	const bool bDefault = false;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getDefault() == bDefault) m_bDefault = pClassInfo->getDefault();
}


void CvGraphicOptionInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bDefault);
}

