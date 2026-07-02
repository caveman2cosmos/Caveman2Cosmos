//------------------------------------------------------------------------------------------------
//  FILE:    CvGoodyInfo.cpp
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
#include "CvGoodyInfo.h"


//======================================================================================================
//					CvGoodyInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvGoodyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvGoodyInfo::CvGoodyInfo() :
m_iGold(0),
m_iGoldRand1(0),
m_iGoldRand2(0),
m_iMapOffset(0),
m_iMapRange(0),
m_iMapProb(0),
m_iExperience(0),
m_iHealing(0),
m_iDamagePrereq(0),
m_iBarbarianUnitProb(0),
m_iMinBarbarians(0),
m_iGoodyUnit(NO_UNIT),
m_iBarbarianUnit(NO_UNIT),
m_iEraType(NO_ERA),
m_iNotEraType(NO_ERA),
m_iResearch(0),
m_bTech(false),
m_bBad(false),
m_bNaval(false)
{
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvGoodyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvGoodyInfo::~CvGoodyInfo()
{
}


int CvGoodyInfo::getGold() const
{
	return m_iGold;
}


int CvGoodyInfo::getGoldRand1() const
{
	return m_iGoldRand1;
}


int CvGoodyInfo::getGoldRand2() const
{
	return m_iGoldRand2;
}


int CvGoodyInfo::getMapOffset() const
{
	return m_iMapOffset;
}


int CvGoodyInfo::getMapRange() const
{
	return m_iMapRange;
}


int CvGoodyInfo::getMapProb() const
{
	return m_iMapProb;
}


int CvGoodyInfo::getExperience() const
{
	return m_iExperience;
}


int CvGoodyInfo::getHealing() const
{
	return m_iHealing;
}


int CvGoodyInfo::getDamagePrereq() const
{
	return m_iDamagePrereq;
}


int CvGoodyInfo::getBarbarianUnitProb() const
{
	return m_iBarbarianUnitProb;
}


int CvGoodyInfo::getMinBarbarians() const
{
	return m_iMinBarbarians;
}


int CvGoodyInfo::getGoodyUnit() const
{
	return m_iGoodyUnit;
}


int CvGoodyInfo::getBarbarianUnit() const
{
	return m_iBarbarianUnit;
}


int CvGoodyInfo::getEraType() const
{
	return m_iEraType;
}


int CvGoodyInfo::getNotEraType() const
{
	return m_iNotEraType;
}


int CvGoodyInfo::getResearch() const
{
	return m_iResearch;
}


bool CvGoodyInfo::isTech() const
{
	return m_bTech;
}


bool CvGoodyInfo::isBad() const
{
	return m_bBad;
}


bool CvGoodyInfo::isNaval() const
{
	return m_bNaval;
}


const char* CvGoodyInfo::getSound() const
{
	return m_szSound;
}


bool CvGoodyInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");
	pXML->GetOptionalChildXmlValByName(&m_iGold, L"iGold");
	pXML->GetOptionalChildXmlValByName(&m_iGoldRand1, L"iGoldRand1");
	pXML->GetOptionalChildXmlValByName(&m_iGoldRand2, L"iGoldRand2");
	pXML->GetOptionalChildXmlValByName(&m_iMapOffset, L"iMapOffset");
	pXML->GetOptionalChildXmlValByName(&m_iMapRange, L"iMapRange");
	pXML->GetOptionalChildXmlValByName(&m_iMapProb, L"iMapProb");
	pXML->GetOptionalChildXmlValByName(&m_iExperience, L"iExperience");
	pXML->GetOptionalChildXmlValByName(&m_iHealing, L"iHealing");
	pXML->GetOptionalChildXmlValByName(&m_iDamagePrereq, L"iDamagePrereq");
	pXML->GetOptionalChildXmlValByName(&m_iResearch, L"iResearch");
	pXML->GetOptionalChildXmlValByName(&m_bTech, L"bTech");
	pXML->GetOptionalChildXmlValByName(&m_bBad, L"bBad");
	pXML->GetOptionalChildXmlValByName(&m_bNaval, L"bNaval");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnit");
	m_iGoodyUnit = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BarbarianUnit");
	m_iBarbarianUnit = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"EraType");
	m_iEraType = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"NotEraType");
	m_iNotEraType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iBarbarianUnitProb, L"iBarbarianUnitProb");
	pXML->GetOptionalChildXmlValByName(&m_iMinBarbarians, L"iMinBarbarians");

	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	return true;
}


void CvGoodyInfo::copyNonDefaults(const CvGoodyInfo* pClassInfo)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();
	if (getGold() == iDefault) m_iGold = pClassInfo->getGold();
	if (getGoldRand1() == iDefault) m_iGoldRand1 = pClassInfo->getGoldRand1();
	if (getGoldRand2() == iDefault) m_iGoldRand2 = pClassInfo->getGoldRand2();
	if (getMapOffset() == iDefault) m_iMapOffset = pClassInfo->getMapOffset();
	if (getMapRange() == iDefault) m_iMapRange = pClassInfo->getMapRange();
	if (getMapProb() == iDefault) m_iMapProb = pClassInfo->getMapProb();
	if (getExperience() == iDefault) m_iExperience = pClassInfo->getExperience();
	if (getHealing() == iDefault) m_iHealing = pClassInfo->getHealing();
	if (getDamagePrereq() == iDefault) m_iDamagePrereq = pClassInfo->getDamagePrereq();
	if (isTech() == bDefault) m_bTech = pClassInfo->isTech();
	if (isBad() == bDefault) m_bBad = pClassInfo->isBad();
	if (isNaval() == bDefault) m_bNaval = pClassInfo->isNaval();
	if (m_iGoodyUnit == iTextDefault) m_iGoodyUnit = pClassInfo->getGoodyUnit();
	if (m_iBarbarianUnit == iTextDefault) m_iBarbarianUnit = pClassInfo->getBarbarianUnit();
	if (getEraType() == iTextDefault) m_iEraType = pClassInfo->getEraType();
	if (getNotEraType() == iTextDefault) m_iNotEraType = pClassInfo->getNotEraType();
	if (getResearch() == iDefault) m_iResearch = pClassInfo->getResearch();
	if (getBarbarianUnitProb() == iDefault) m_iBarbarianUnitProb = pClassInfo->getBarbarianUnitProb();
	if (getMinBarbarians() == iDefault) m_iMinBarbarians = pClassInfo->getMinBarbarians();
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
}


void CvGoodyInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iGold);
	CheckSum(iSum, m_iGoldRand1);
	CheckSum(iSum, m_iGoldRand2);
	CheckSum(iSum, m_iMapOffset);
	CheckSum(iSum, m_iMapRange);
	CheckSum(iSum, m_iMapProb);
	CheckSum(iSum, m_iExperience);
	CheckSum(iSum, m_iHealing);
	CheckSum(iSum, m_iDamagePrereq);
	CheckSum(iSum, m_iBarbarianUnitProb);
	CheckSum(iSum, m_iMinBarbarians);
	CheckSum(iSum, m_iGoodyUnit);
	CheckSum(iSum, m_iBarbarianUnit);
	CheckSum(iSum, m_iEraType);
	CheckSum(iSum, m_iNotEraType);
	CheckSum(iSum, m_iResearch);
	CheckSum(iSum, m_bTech);
	CheckSum(iSum, m_bBad);
	CheckSum(iSum, m_bNaval);
	CheckSumC(iSum, m_aeMapCategoryTypes);
}

