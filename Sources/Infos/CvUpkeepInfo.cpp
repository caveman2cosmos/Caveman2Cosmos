//------------------------------------------------------------------------------------------------
//  FILE:    CvUpkeepInfo.cpp
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
#include "CvUpkeepInfo.h"


//------------------------------------------------------------------------------------------------------
//
//  CvUpkeepInfo
//

CvUpkeepInfo::CvUpkeepInfo() :
m_iPopulationPercent(0),
m_iCityPercent(0)
{
}


CvUpkeepInfo::~CvUpkeepInfo()
{
}


int CvUpkeepInfo::getPopulationPercent() const
{
	return m_iPopulationPercent;
}


int CvUpkeepInfo::getCityPercent() const
{
	return m_iCityPercent;
}


bool CvUpkeepInfo::read(CvXMLLoadUtility* pXml)
{
	if (!CvInfoBase::read(pXml))
	{
		return false;
	}

	pXml->GetChildXmlValByName(&m_iPopulationPercent, L"iPopulationPercent");
	pXml->GetChildXmlValByName(&m_iCityPercent, L"iCityPercent");

	return true;
}


void CvUpkeepInfo::copyNonDefaults(const CvUpkeepInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getPopulationPercent() == iDefault) m_iPopulationPercent = pClassInfo->getPopulationPercent();
	if (getCityPercent() == iDefault) m_iCityPercent = pClassInfo->getCityPercent();
}


void CvUpkeepInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iPopulationPercent);
	CheckSum(iSum, m_iCityPercent);
}

