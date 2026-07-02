//------------------------------------------------------------------------------------------------
//  FILE:    CvHurryInfo.cpp
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
#include "CvHurryInfo.h"


//======================================================================================================
//					CvHurryInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvHurryInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvHurryInfo::CvHurryInfo() :
m_iGoldPerProduction(0),
m_iProductionPerPopulation(0),
m_bAnger(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvHurryInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvHurryInfo::~CvHurryInfo()
{
}


int CvHurryInfo::getGoldPerProduction() const
{
	return m_iGoldPerProduction;
}


int CvHurryInfo::getProductionPerPopulation() const
{
	return m_iProductionPerPopulation;
}


bool CvHurryInfo::isAnger() const
{
	return m_bAnger;
}


bool CvHurryInfo::read(CvXMLLoadUtility* pXML)
{

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iGoldPerProduction, L"iGoldPerProduction");
	pXML->GetOptionalChildXmlValByName(&m_iProductionPerPopulation, L"iProductionPerPopulation");

	pXML->GetOptionalChildXmlValByName(&m_bAnger, L"bAnger");

	return true;
}


void CvHurryInfo::copyNonDefaults(const CvHurryInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iDefault = 0;

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getGoldPerProduction() == iDefault) m_iGoldPerProduction = pClassInfo->getGoldPerProduction();
	if (getProductionPerPopulation() == iDefault) m_iProductionPerPopulation = pClassInfo->getProductionPerPopulation();

	if (isAnger() == bDefault) m_bAnger = pClassInfo->isAnger();
}


void CvHurryInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iGoldPerProduction);
	CheckSum(iSum, m_iProductionPerPopulation);

	CheckSum(iSum, m_bAnger);
}

