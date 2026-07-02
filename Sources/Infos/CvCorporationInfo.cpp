//------------------------------------------------------------------------------------------------
//  FILE:    CvCorporationInfo.cpp
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
#include "CvCorporationInfo.h"


//======================================================================================================
//					CvCorporationInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCorporationInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCorporationInfo::CvCorporationInfo() :
 m_iChar(0)
,m_iTGAIndex(-1)
,m_iHeadquarterChar(0)
,m_iTechPrereq(NO_TECH)
,m_iFreeUnit(NO_UNIT)
,m_iSpreadFactor(0)
,m_iSpreadCost(0)
,m_iMaintenance(0)
,m_iMissionType(NO_MISSION)
,m_iBonusProduced(NO_BONUS)
,m_paiHeadquarterCommerce(NULL)
,m_paiCommerceProduced(NULL)
,m_paiYieldProduced(NULL)
,m_iObsoleteTech(NO_TECH)
,m_iPrereqGameOption(NO_GAMEOPTION)
,m_iHealth(0)
,m_iHappiness(0)
,m_iMilitaryProductionModifier(0)
,m_iFreeXP(0)
,m_iSpread(0)
,m_paiPrereqBuilding(NULL)
,m_pabCompetingCorporation(NULL)
,m_piYieldChange(NULL)
,m_piCommerceChange(NULL)
,m_PropertyManipulators()
{
	reset();
}


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvReligionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCorporationInfo::~CvCorporationInfo()
{
	SAFE_DELETE_ARRAY(m_paiHeadquarterCommerce);
	SAFE_DELETE_ARRAY(m_paiCommerceProduced);
	SAFE_DELETE_ARRAY(m_paiYieldProduced);
	SAFE_DELETE_ARRAY(m_paiPrereqBuilding);
	SAFE_DELETE_ARRAY(m_pabCompetingCorporation);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piCommerceChange);

}


int CvCorporationInfo::getChar() const
{
	return m_iChar;
}



int CvCorporationInfo::getTGAIndex() const
{
	return m_iTGAIndex;
}


void CvCorporationInfo::setChar(int i)
{
	m_iChar = 8550 + (GC.getGAMEFONT_TGA_RELIGIONS() + m_iTGAIndex) * 2;
}


int CvCorporationInfo::getHeadquarterChar() const
{
	return m_iHeadquarterChar;
}


void CvCorporationInfo::setHeadquarterChar(int i)
{
	m_iHeadquarterChar = 8551 + (GC.getGAMEFONT_TGA_RELIGIONS() + m_iTGAIndex) * 2;
}


int CvCorporationInfo::getFreeUnit() const
{
	return m_iFreeUnit;
}


int CvCorporationInfo::getSpreadFactor() const
{
	return m_iSpreadFactor;
}


int CvCorporationInfo::getSpreadCost() const
{
	return m_iSpreadCost;
}


int CvCorporationInfo::getMaintenance() const
{
	return m_iMaintenance;
}


int CvCorporationInfo::getMissionType() const
{
	return m_iMissionType;
}


void CvCorporationInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}


int CvCorporationInfo::getBonusProduced() const
{
	return m_iBonusProduced;
}


const char* CvCorporationInfo::getMovieFile() const
{
	return m_szMovieFile;
}


const char* CvCorporationInfo::getMovieSound() const
{
	return m_szMovieSound;
}


const char* CvCorporationInfo::getSound() const
{
	return m_szSound;
}


TechTypes CvCorporationInfo::getObsoleteTech() const
{
	return m_iObsoleteTech;
}


int CvCorporationInfo::getSpread() const
{
	return m_iSpread;
}


int CvCorporationInfo::getHealth() const
{
	return m_iHealth;
}


int CvCorporationInfo::getHappiness() const
{
	return m_iHappiness;
}


int CvCorporationInfo::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}


int CvCorporationInfo::getFreeXP() const
{
	return m_iFreeXP;
}


int CvCorporationInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}


int CvCorporationInfo::getPrereqBuilding(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	return m_paiPrereqBuilding ? m_paiPrereqBuilding[i] : false;
}


int CvCorporationInfo::getPrereqBuildingVectorSize() const					{return m_aszPrereqBuildingforPass3.size();}

CvString CvCorporationInfo::getPrereqBuildingNamesVectorElement(const int i) const	{return m_aszPrereqBuildingforPass3[i];}

int CvCorporationInfo::getPrereqBuildingValuesVectorElement(const int i) const		{return m_aiPrereqBuildingforPass3[i];}


bool CvCorporationInfo::isCompetingCorporation(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCorporationInfos(), i);
	return m_pabCompetingCorporation ? m_pabCompetingCorporation[i] : false;
}


int CvCorporationInfo::getCompetingCorporationVectorSize() const					{return m_aszCompetingCorporationforPass3.size();}

CvString CvCorporationInfo::getCompetingCorporationNamesVectorElement(const int i) const	{return m_aszCompetingCorporationforPass3[i];}

bool CvCorporationInfo::getCompetingCorporationValuesVectorElement(const int i) const		{return m_abCompetingCorporationforPass3[i];}


int CvCorporationInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}


int* CvCorporationInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}


int CvCorporationInfo::getCommerceChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceChange ? m_piCommerceChange[i] : 0;
}


int* CvCorporationInfo::getCommerceChangeArray() const
{
	return m_piCommerceChange;
}


const std::vector<BonusTypes>& CvCorporationInfo::getPrereqBonuses() const
{
	return m_vPrereqBonuses;
}


int CvCorporationInfo::getHeadquarterCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiHeadquarterCommerce ? m_paiHeadquarterCommerce[i] : 0;
}


int* CvCorporationInfo::getHeadquarterCommerceArray() const
{
	return m_paiHeadquarterCommerce;
}


int CvCorporationInfo::getCommerceProduced(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_paiCommerceProduced ? m_paiCommerceProduced[i] : 0;
}


int* CvCorporationInfo::getCommerceProducedArray() const
{
	return m_paiCommerceProduced;
}


int CvCorporationInfo::getYieldProduced(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_paiYieldProduced ? m_paiYieldProduced[i] : 0;
}


int* CvCorporationInfo::getYieldProducedArray() const
{
	return m_paiYieldProduced;
}


int CvCorporationInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvCorporationInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvCorporationInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


//
// read from xml
//
bool CvCorporationInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnit");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iSpreadFactor, L"iSpreadFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSpreadCost, L"iSpreadCost");
	pXML->GetOptionalChildXmlValByName(&m_iMaintenance, L"iMaintenance");
	pXML->GetOptionalChildXmlValByName(&m_iTGAIndex, L"iTGAIndex");

	if (pXML->TryMoveToXmlFirstChild(L"HeadquarterCommerces"))
	{
		pXML->SetCommerce(&m_paiHeadquarterCommerce);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiHeadquarterCommerce);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommercesProduced"))
	{
		pXML->SetCommerce(&m_paiCommerceProduced);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceProduced);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldsProduced"))
	{
		pXML->SetYields(&m_paiYieldProduced);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiYieldProduced);
	}

	pXML->SetOptionalVector(&m_vPrereqBonuses, L"PrereqBonuses");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusProduced");
	m_iBonusProduced = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(m_szMovieFile, L"MovieFile");
	pXML->GetOptionalChildXmlValByName(m_szMovieSound, L"MovieSound");
	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = static_cast<TechTypes>(pXML->GetInfoClass(szTextVal));

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iSpread, L"iSpread");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryProductionModifier, L"iMilitaryProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeXP, L"iFreeXP");

	if (pXML->TryMoveToXmlFirstChild(L"PrereqBuildings"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = 0;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszPrereqBuildingforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiPrereqBuildingforPass3.push_back(iTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"CompetingCorporations"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		bool bTemp = false;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i=0;i<iNumSibs;i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszCompetingCorporationforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&bTemp);
						m_abCompetingCorporationforPass3.push_back(bTemp);
						pXML->MoveToXmlParent();
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
	{
		pXML->SetCommerce(&m_piCommerceChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceChange);
	}
	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		pXML->CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
	}


	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	m_PropertyManipulators.read(pXML);

	return true;
}


void CvCorporationInfo::copyNonDefaults(const CvCorporationInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (m_iFreeUnit == iTextDefault) m_iFreeUnit = pClassInfo->getFreeUnit();
	if (getSpreadFactor() == iDefault) m_iSpreadFactor = pClassInfo->getSpreadFactor();
	if (getSpreadCost() == iDefault) m_iSpreadCost = pClassInfo->getSpreadCost();
	if (getMaintenance() == iDefault) m_iMaintenance = pClassInfo->getMaintenance();

	if (getTGAIndex() == iDefault) m_iTGAIndex = pClassInfo->getTGAIndex();

	if (!m_paiHeadquarterCommerce) CvXMLLoadUtility::InitList(&m_paiHeadquarterCommerce, NUM_COMMERCE_TYPES);
	if (!m_paiCommerceProduced) CvXMLLoadUtility::InitList(&m_paiCommerceProduced, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_paiHeadquarterCommerce[i] == iDefault )
		{
			m_paiHeadquarterCommerce[i] = pClassInfo->getHeadquarterCommerce(i);
		}

		if ( m_paiCommerceProduced[i] == iDefault )
		{
			m_paiCommerceProduced[i] = pClassInfo->getCommerceProduced(i);
		}
	}

	if (!m_paiYieldProduced) CvXMLLoadUtility::InitList(&m_paiYieldProduced, NUM_YIELD_TYPES);
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( m_paiYieldProduced[i] == iDefault )
		{
			m_paiYieldProduced[i] = pClassInfo->getYieldProduced(i);
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_vPrereqBonuses, pClassInfo->m_vPrereqBonuses);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	if (getBonusProduced() == iTextDefault) m_iBonusProduced = pClassInfo->getBonusProduced();

	if (getMovieFile() == cDefault) m_szMovieFile = pClassInfo->getMovieFile();
	if (getMovieSound() == cDefault) m_szMovieSound = pClassInfo->getMovieSound();
	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();

	for ( int i = 0; i < pClassInfo->getPrereqBuildingVectorSize(); i++ )
	{
		m_aiPrereqBuildingforPass3.push_back(pClassInfo->getPrereqBuildingValuesVectorElement(i));
		m_aszPrereqBuildingforPass3.push_back(pClassInfo->getPrereqBuildingNamesVectorElement(i));
	}
	if (getObsoleteTech() == iTextDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getSpread() == iDefault) m_iSpread = pClassInfo->getSpread();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getMilitaryProductionModifier() == iDefault) m_iMilitaryProductionModifier = pClassInfo->getMilitaryProductionModifier();
	if (getFreeXP() == iDefault) m_iFreeXP = pClassInfo->getFreeXP();
	if (getPrereqGameOption() == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();

	for ( int i = 0; i < pClassInfo->getCompetingCorporationVectorSize(); i++ )
	{
		m_abCompetingCorporationforPass3.push_back(pClassInfo->getCompetingCorporationValuesVectorElement(i));
		m_aszCompetingCorporationforPass3.push_back(pClassInfo->getCompetingCorporationNamesVectorElement(i));
	}

	if (!m_piYieldChange) CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
	for ( int j = 0; j < NUM_YIELD_TYPES; j++)
	{
		if ( getYieldChange(j) == iDefault && pClassInfo->getYieldChange(j) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[j] = pClassInfo->getYieldChange(j);
		}
	}
	if (!m_piCommerceChange) CvXMLLoadUtility::InitList(&m_piCommerceChange, NUM_COMMERCE_TYPES);
	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( m_piCommerceChange[j] == iDefault )
		{
			m_piCommerceChange[j] = pClassInfo->getCommerceChange(j);
		}
	}
	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}


void CvCorporationInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iFreeUnit);
	CheckSum(iSum, m_iSpreadFactor);
	CheckSum(iSum, m_iSpreadCost);
	CheckSum(iSum, m_iMaintenance);
	CheckSum(iSum, m_iMissionType);
	CheckSum(iSum, m_iBonusProduced);

	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iSpread);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iFreeXP);
	CheckSum(iSum, m_iMilitaryProductionModifier);
	CheckSum(iSum, m_iPrereqGameOption);

	CheckSum(iSum, m_paiPrereqBuilding, GC.getNumBuildingInfos());

	CheckSum(iSum, m_pabCompetingCorporation, GC.getNumCorporationInfos());

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piCommerceChange, NUM_COMMERCE_TYPES);

	// Arrays

	CheckSumC(iSum, m_vPrereqBonuses);
	CheckSum(iSum, m_paiHeadquarterCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiCommerceProduced, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiYieldProduced, NUM_YIELD_TYPES);

	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}



bool CvCorporationInfo::readPass3()
{
	PROFILE_EXTRA_FUNC();
	m_paiPrereqBuilding = new int[GC.getNumBuildingInfos()];
	for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		m_paiPrereqBuilding[iI] = 0;
	}
	if (!m_aiPrereqBuildingforPass3.empty() && !m_aszPrereqBuildingforPass3.empty())
	{
		const int iNumLoad = m_aiPrereqBuildingforPass3.size();
		for (int iI = 0; iI < iNumLoad; iI++)
		{
			const int iTempIndex = GC.getInfoTypeForString(m_aszPrereqBuildingforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumBuildingInfos())
				m_paiPrereqBuilding[iTempIndex] = m_aiPrereqBuildingforPass3[iI];
		}
		m_aszPrereqBuildingforPass3.clear();
		m_aiPrereqBuildingforPass3.clear();
	}

	m_pabCompetingCorporation = new bool[GC.getNumCorporationInfos()];
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		m_pabCompetingCorporation[iI] = 0;
	}
	if (!m_abCompetingCorporationforPass3.empty() && !m_aszCompetingCorporationforPass3.empty())
	{
		const int iNumLoad = m_abCompetingCorporationforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			const int iTempIndex = GC.getInfoTypeForString(m_aszCompetingCorporationforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCorporationInfos())
				m_pabCompetingCorporation[iTempIndex] = m_abCompetingCorporationforPass3[iI];
		}
		m_aszCompetingCorporationforPass3.clear();
		m_abCompetingCorporationforPass3.clear();
	}

	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FErrorMsg("error");
		return false;
	}

	m_iFreeUnit = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);

	m_aszExtraXMLforPass3.clear();

	return true;
}

