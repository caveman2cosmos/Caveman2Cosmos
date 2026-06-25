//------------------------------------------------------------------------------------------------
//  FILE:    CvTurnTimerInfo.cpp
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
#include "CvTurnTimerInfo.h"



//======================================================================================================
//					CvTurnTimerInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTurnTimerInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTurnTimerInfo::CvTurnTimerInfo() :
m_iBaseTime(0),
m_iCityBonus(0),
m_iUnitBonus(0),
m_iFirstTurnMultiplier(0)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTurnTimerInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTurnTimerInfo::~CvTurnTimerInfo()
{
}


int CvTurnTimerInfo::getBaseTime() const
{
	return m_iBaseTime;
}


int CvTurnTimerInfo::getCityBonus() const
{
	return m_iCityBonus;
}


int CvTurnTimerInfo::getUnitBonus() const
{
	return m_iUnitBonus;
}


int CvTurnTimerInfo::getFirstTurnMultiplier() const
{
	return m_iFirstTurnMultiplier;
}


bool CvTurnTimerInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iBaseTime, L"iBaseTime");
	pXML->GetOptionalChildXmlValByName(&m_iCityBonus, L"iCityBonus");
	pXML->GetOptionalChildXmlValByName(&m_iUnitBonus, L"iUnitBonus");
	pXML->GetOptionalChildXmlValByName(&m_iFirstTurnMultiplier, L"iFirstTurnMultiplier");

	return true;
}


void CvTurnTimerInfo::copyNonDefaults(const CvTurnTimerInfo* pClassInfo)
{
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getBaseTime() == iDefault) m_iBaseTime = pClassInfo->getBaseTime();
	if (getCityBonus() == iDefault) m_iCityBonus = pClassInfo->getCityBonus();
	if (getUnitBonus() == iDefault) m_iUnitBonus = pClassInfo->getUnitBonus();
	if (getFirstTurnMultiplier() == iDefault) m_iFirstTurnMultiplier = pClassInfo->getFirstTurnMultiplier();
}


void CvTurnTimerInfo::getCheckSum(uint32_t &iSum) const
{
	CheckSum(iSum, m_iBaseTime);
	CheckSum(iSum, m_iCityBonus);
	CheckSum(iSum, m_iUnitBonus);
	CheckSum(iSum, m_iFirstTurnMultiplier);
}

