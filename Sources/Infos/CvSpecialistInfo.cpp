//------------------------------------------------------------------------------------------------
//  FILE:    CvSpecialistInfo.cpp
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
#include "CvSpecialistInfo.h"


//======================================================================================================
//					CvSpecialistInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialistInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialistInfo::CvSpecialistInfo() :
 m_iGreatPeopleUnitType(NO_UNIT)
,m_iGreatPeopleRateChange(0)
,m_iMissionType(NO_MISSION)
,m_bVisible(false)
,m_piYieldChange(NULL)
,m_piCommerceChange(NULL)
,m_piFlavorValue(NULL)
,m_iExperience(0)
,m_iHealthPercent(0)
,m_iHappinessPercent(0)
,m_iInsidiousness(0)
,m_iInvestigation(0)
,m_bSlave(false)
{ }


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialistInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialistInfo::~CvSpecialistInfo()
{
	PROFILE_EXTRA_FUNC();
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piCommerceChange);
	SAFE_DELETE_ARRAY(m_piFlavorValue);

	m_aTechHappinessTypes.removeDelayedResolution();
	m_aTechHealthTypes.removeDelayedResolution();

	for (int i=0; i<(int)m_aUnitCombatExperienceTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitCombatExperienceTypes[i]));
	}

	for (int i=0; i<(int)m_aUnitCombatExperienceTypesNull.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitCombatExperienceTypesNull[i]));
	}

	GC.removeDelayedResolution((int*)&m_iGreatPeopleUnitType);
}


int CvSpecialistInfo::getGreatPeopleUnitType() const
{
	return m_iGreatPeopleUnitType;
}


int CvSpecialistInfo::getGreatPeopleRateChange() const
{
	return m_iGreatPeopleRateChange;
}


int CvSpecialistInfo::getMissionType() const
{
	return m_iMissionType;
}


void CvSpecialistInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}


bool CvSpecialistInfo::isVisible() const
{
	return m_bVisible;
}


int CvSpecialistInfo::getExperience() const
{
	return m_iExperience;
}


// Arrays

int CvSpecialistInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}


const int* CvSpecialistInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}


int CvSpecialistInfo::getCommerceChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceChange ? m_piCommerceChange[i] : 0;
}


int CvSpecialistInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}


const char* CvSpecialistInfo::getTexture() const
{
	return m_szTexture;
}


int CvSpecialistInfo::getHealthPercent() const
{
	return m_iHealthPercent;
}

int CvSpecialistInfo::getHappinessPercent() const
{
	return m_iHappinessPercent;
}


bool CvSpecialistInfo::isSlave() const
{
	return m_bSlave;
}


//TB Specialist Tags
int CvSpecialistInfo::getInsidiousness() const
{
	return m_iInsidiousness;
}


int CvSpecialistInfo::getInvestigation() const
{
	return m_iInvestigation;
}


//int CvSpecialistInfo::getNumTechHappinessTypes() const
//{
//	return (int)m_aTechHappinessTypes.size();
//}

int CvSpecialistInfo::getTechHappiness(TechTypes eTech) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);
	return m_aTechHappinessTypes.getValue(eTech);
}


//int CvSpecialistInfo::getNumTechHealthTypes() const
//{
//	return (int)m_aTechHealthTypes.size();
//}

int CvSpecialistInfo::getTechHealth(TechTypes eTech) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), eTech);
	return m_aTechHealthTypes.getValue(eTech);
}


int CvSpecialistInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvSpecialistInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvSpecialistInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


int CvSpecialistInfo::getNumUnitCombatExperienceTypes() const
{
	return (int)m_aUnitCombatExperienceTypes.size();
}


const UnitCombatModifier& CvSpecialistInfo::getUnitCombatExperienceType(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, (int)m_aUnitCombatExperienceTypes.size(), iUnitCombat);
	FASSERT_BOUNDS(0, (int)m_aUnitCombatExperienceTypesNull.size(), iUnitCombat);

	if (!GC.getGame().isOption(GAMEOPTION_UNIT_XP_FROM_SPECIALISTS) && isVisible())
	{
		return m_aUnitCombatExperienceTypesNull[iUnitCombat];
	}
	return m_aUnitCombatExperienceTypes[iUnitCombat];
}



// read from xml
//
bool CvSpecialistInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(m_szTexture, L"Texture");
	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GreatPeopleUnitType");
	GC.addDelayedResolution((int*)&m_iGreatPeopleUnitType, szTextVal);
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateChange, L"iGreatPeopleRateChange");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	if (pXML->TryMoveToXmlFirstChild(L"Yields"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"Commerces"))
	{
		pXML->SetCommerce(&m_piCommerceChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceChange);
	}

	pXML->GetOptionalChildXmlValByName(&m_iExperience, L"iExperience");

	m_PropertyManipulators.read(pXML);

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());

	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iHappinessPercent, L"iHappinessPercent");
	pXML->GetOptionalChildXmlValByName(&m_bSlave, L"bSlave");

	//TB Specialist Tags
	pXML->GetOptionalChildXmlValByName(&m_iInsidiousness, L"iInsidiousness");
	pXML->GetOptionalChildXmlValByName(&m_iInvestigation, L"iInvestigation");

	m_aTechHappinessTypes.readWithDelayedResolution(pXML, L"TechHappinessTypes");
	m_aTechHealthTypes.readWithDelayedResolution(pXML, L"TechHealthTypes");

	if(pXML->TryMoveToXmlFirstChild(L"UnitCombatExperienceTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitCombatExperienceType" );
		m_aUnitCombatExperienceTypes.resize(iNum); // Important to keep the delayed resolution pointers correct
		m_aUnitCombatExperienceTypesNull.resize(iNum);

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitCombatExperienceType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					pXML->GetChildXmlValByName(&(m_aUnitCombatExperienceTypes[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aUnitCombatExperienceTypes[i].eUnitCombat), szTextVal);
					GC.addDelayedResolution((int*)&(m_aUnitCombatExperienceTypesNull[i].eUnitCombat), szTextVal);
					m_aUnitCombatExperienceTypesNull[i].iModifier = 0;
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitCombatExperienceType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}
	return true;
}


void CvSpecialistInfo::copyNonDefaults(const CvSpecialistInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	CvString cDefault = CvString::format("").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getTexture() == cDefault) m_szTexture = pClassInfo->getTexture();
	if (isVisible() == bDefault) m_bVisible = pClassInfo->isVisible();
	GC.copyNonDefaultDelayedResolution((int*)&m_iGreatPeopleUnitType, (int*)&pClassInfo->m_iGreatPeopleUnitType);
	if (getGreatPeopleRateChange() == iDefault) m_iGreatPeopleRateChange = pClassInfo->getGreatPeopleRateChange();

	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
	}

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( getCommerceChange(i) == iDefault ) m_piCommerceChange[i] = pClassInfo->getCommerceChange(i);
	}

	if (getExperience() == iDefault) m_iExperience = pClassInfo->getExperience();

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++ )
	{
		if ( getFlavorValue(i) == iDefault && pClassInfo->getFlavorValue(i) != iDefault )
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[i] = pClassInfo->getFlavorValue(i);
		}
	}

	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
	if (getHappinessPercent() == iDefault) m_iHappinessPercent = pClassInfo->getHappinessPercent();
	if (isSlave() == bDefault) m_bSlave = pClassInfo->isSlave();

	//TB Specialist Tags
	if (getInsidiousness() == iDefault) m_iInsidiousness = pClassInfo->getInsidiousness();
	if (getInvestigation() == iDefault) m_iInvestigation = pClassInfo->getInvestigation();

	m_aTechHappinessTypes.copyNonDefaultDelayedResolution(pClassInfo->getTechHappinessTypes());
	m_aTechHealthTypes.copyNonDefaultDelayedResolution(pClassInfo->getTechHealthTypes());

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

	if (getNumUnitCombatExperienceTypes() == 0)
	{
		const int iNum = pClassInfo->getNumUnitCombatExperienceTypes();
		m_aUnitCombatExperienceTypes.resize(iNum);
		m_aUnitCombatExperienceTypesNull.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aUnitCombatExperienceTypes[i].iModifier = pClassInfo->m_aUnitCombatExperienceTypes[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aUnitCombatExperienceTypes[i].eUnitCombat), (int*)&(pClassInfo->m_aUnitCombatExperienceTypes[i].eUnitCombat));
			m_aUnitCombatExperienceTypesNull[i].iModifier = 0;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aUnitCombatExperienceTypesNull[i].eUnitCombat), (int*)&(pClassInfo->m_aUnitCombatExperienceTypesNull[i].eUnitCombat));
		}
	}
}


void CvSpecialistInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iGreatPeopleUnitType);
	CheckSum(iSum, m_iGreatPeopleRateChange);
	CheckSum(iSum, m_iMissionType);
	CheckSum(iSum, m_iHealthPercent);
	CheckSum(iSum, m_iHappinessPercent);
	CheckSum(iSum, m_bSlave);
	CheckSum(iSum, m_iExperience);
	m_PropertyManipulators.getCheckSum(iSum);
	CheckSum(iSum, m_bVisible);
	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piCommerceChange, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());
	//Team Project (1)
	//TB Specialist Tags
	// int
	CheckSum(iSum, m_iInsidiousness);
	CheckSum(iSum, m_iInvestigation);
	CheckSumC(iSum, m_aTechHappinessTypes);
	CheckSumC(iSum, m_aTechHealthTypes);
	CheckSumC(iSum, m_aiCategories);

	int iNumElements = m_aUnitCombatExperienceTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatExperienceTypes[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatExperienceTypes[i].iModifier);
	}

	iNumElements = m_aUnitCombatExperienceTypesNull.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatExperienceTypesNull[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatExperienceTypesNull[i].iModifier);
	}

}

