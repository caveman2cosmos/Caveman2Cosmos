//------------------------------------------------------------------------------------------------
//  FILE:    CvCultureLevelInfo.cpp
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
#include "CvCultureLevelInfo.h"


//------------------------------------------------------------------------------------------------------
//
//  CvCultureLevelInfo
//

CvCultureLevelInfo::CvCultureLevelInfo() :
m_iLevel(-1),
m_iCityDefenseModifier(0),

m_iCityRadius(1),
m_iMaxWorldWonders(1),
m_iMaxTeamWonders(1),
m_iMaxNationalWonders(1),
m_iMaxNationalWondersOCC(1),
m_iPrereqGameOption(NO_GAMEOPTION),

m_paiSpeedThreshold(NULL)
{
}


CvCultureLevelInfo::~CvCultureLevelInfo()
{
	SAFE_DELETE_ARRAY(m_paiSpeedThreshold);
}


int CvCultureLevelInfo::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}


int CvCultureLevelInfo::getSpeedThreshold(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumGameSpeedInfos(), i);
	return m_paiSpeedThreshold ? m_paiSpeedThreshold[i] : 0;
}



int CvCultureLevelInfo::getCityRadius() const
{
	return m_iCityRadius;
}


int CvCultureLevelInfo::getMaxWorldWonders() const
{
	return m_iMaxWorldWonders;
}


int CvCultureLevelInfo::getMaxTeamWonders() const
{
	return m_iMaxTeamWonders;
}


int CvCultureLevelInfo::getMaxNationalWonders() const
{
	return m_iMaxNationalWonders;
}


int CvCultureLevelInfo::getMaxNationalWondersOCC() const
{
	return m_iMaxNationalWondersOCC;
}


int CvCultureLevelInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}



bool CvCultureLevelInfo::read(CvXMLLoadUtility* pXml)
{
	if (!CvInfoBase::read(pXml))
	{
		return false;
	}

	pXml->GetOptionalChildXmlValByName(&m_iCityDefenseModifier, L"iCityDefenseModifier");

	pXml->GetOptionalChildXmlValByName(&m_iCityRadius, L"iCityRadius", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxWorldWonders, L"iMaxWorldWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxTeamWonders, L"iMaxTeamWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxNationalWonders, L"iMaxNationalWonders", 1);
	pXml->GetOptionalChildXmlValByName(&m_iMaxNationalWondersOCC, L"iMaxNationalWondersOCC", 1);
	CvString szTextVal;
	pXml->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXml->GetInfoClass(szTextVal);

	pXml->SetVariableListTagPair(&m_paiSpeedThreshold, L"SpeedThresholds", GC.getNumGameSpeedInfos());

	return true;
}


void CvCultureLevelInfo::copyNonDefaults(const CvCultureLevelInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getCityDefenseModifier() == iDefault) m_iCityDefenseModifier = pClassInfo->getCityDefenseModifier();
	if (getCityRadius() == 1) m_iCityRadius = pClassInfo->getCityRadius();
	if (getMaxWorldWonders() == 1) m_iMaxWorldWonders = pClassInfo->getMaxWorldWonders();
	if (getMaxTeamWonders() == 1) m_iMaxTeamWonders = pClassInfo->getMaxTeamWonders();
	if (getMaxNationalWonders() == 1) m_iMaxNationalWonders = pClassInfo->getMaxNationalWonders();
	if (getMaxNationalWondersOCC() == 1) m_iMaxNationalWondersOCC = pClassInfo->getMaxNationalWondersOCC();
	if (getPrereqGameOption() == NO_GAMEOPTION) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();

	for ( int i = 0; i < GC.getNumGameSpeedInfos(); i++ )
	{
		if ( getSpeedThreshold(i) == iDefault && pClassInfo->getSpeedThreshold(i) != iDefault)
		{
			if ( NULL == m_paiSpeedThreshold )
			{
				CvXMLLoadUtility::InitList(&m_paiSpeedThreshold,GC.getNumGameSpeedInfos(),iDefault);
			}
			m_paiSpeedThreshold[i] = pClassInfo->getSpeedThreshold(i);
		}
	}
}


void CvCultureLevelInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iCityDefenseModifier);
	CheckSum(iSum, m_iCityRadius);
	CheckSum(iSum, m_iMaxWorldWonders);
	CheckSum(iSum, m_iMaxTeamWonders);
	CheckSum(iSum, m_iMaxNationalWonders);
	CheckSum(iSum, m_iMaxNationalWondersOCC);
	CheckSum(iSum, m_iPrereqGameOption);

	CheckSum(iSum, m_paiSpeedThreshold, GC.getNumGameSpeedInfos());
}

