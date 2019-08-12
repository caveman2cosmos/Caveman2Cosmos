#include "CvGameCoreDLL.h"

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
m_iGreatPeopleUnitClass(NO_UNITCLASS),
m_iGreatPeopleRateChange(0),
m_iMissionType(NO_MISSION),
m_bVisible(false),
m_piYieldChange(NULL),
m_piCommerceChange(NULL),
m_piFlavorValue(NULL),
m_iExperience(0)
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iHealthPercent(0)
,m_iHappinessPercent(0)
,m_iInsidiousness(0)
,m_iInvestigation(0)
,m_bSlave(false)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialistInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialistInfo::~CvSpecialistInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piCommerceChange);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	//TB Specialist Tags
	//Team Project (1)
	// int vector utilizing struct with delayed resolution
	for (int i=0; i<(int)m_aTechHappinessTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aTechHappinessTypes[i]));
	}

	for (int i=0; i<(int)m_aTechHealthTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aTechHealthTypes[i]));
	}

	for (int i=0; i<(int)m_aUnitCombatExperienceTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitCombatExperienceTypes[i]));
	}

	for (int i=0; i<(int)m_aUnitCombatExperienceTypesNull.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitCombatExperienceTypesNull[i]));
	}
}

int CvSpecialistInfo::getGreatPeopleUnitClass() const
{
	return m_iGreatPeopleUnitClass;
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

CvPropertyManipulators* CvSpecialistInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}

// Arrays

int CvSpecialistInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

const int* CvSpecialistInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}

int CvSpecialistInfo::getCommerceChange(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCommerceChange ? m_piCommerceChange[i] : 0;
}

int CvSpecialistInfo::getFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

const TCHAR* CvSpecialistInfo::getTexture() const
{
	return m_szTexture;
}

void CvSpecialistInfo::setTexture(const TCHAR* szVal)
{
	m_szTexture = szVal;
}

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
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
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
//Team Project (1)
//TB Specialist Tags
// int
int CvSpecialistInfo::getInsidiousness() const
{
	return m_iInsidiousness;
}

int CvSpecialistInfo::getInvestigation() const
{
	return m_iInvestigation;
}

// int vector utilizing struct with delayed resolution
int CvSpecialistInfo::getNumTechHappinessTypes() const
{
	return (int)m_aTechHappinessTypes.size();
}

TechModifier& CvSpecialistInfo::getTechHappinessType(int iTech)
{
	FAssertMsg(iTech < (int)m_aTechHappinessTypes.size(), "Index out of bounds");
	FAssertMsg(iTech > -1, "Index out of bounds");
	return m_aTechHappinessTypes[iTech];
}

int CvSpecialistInfo::getNumTechHealthTypes() const
{
	return (int)m_aTechHealthTypes.size();
}

TechModifier& CvSpecialistInfo::getTechHealthType(int iTech)
{
	FAssertMsg(iTech < (int)m_aTechHealthTypes.size(), "Index out of bounds");
	FAssertMsg(iTech > -1, "Index out of bounds");
	return m_aTechHealthTypes[iTech];
}

int CvSpecialistInfo::getNumUnitCombatExperienceTypes() const
{
	return (int)m_aUnitCombatExperienceTypes.size();
}

UnitCombatModifier& CvSpecialistInfo::getUnitCombatExperienceType(int iUnitCombat, bool bForLoad)
{
	FAssertMsg(iUnitCombat < (int)m_aUnitCombatExperienceTypes.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat < (int)m_aUnitCombatExperienceTypesNull.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");
	if (!bForLoad)
	{
		if (!GC.getGameINLINE().isOption(GAMEOPTION_XP_FROM_ASSIGNED_SPECIALISTS) && isVisible())
		{
			return m_aUnitCombatExperienceTypesNull[iUnitCombat];
		}
	}
	return m_aUnitCombatExperienceTypes[iUnitCombat];
}


// read from xml
//
bool CvSpecialistInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Texture");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 10/26/07							MRGENIE		  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setTexture(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bVisible, L"bVisible");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"GreatPeopleUnitClass");
	m_iGreatPeopleUnitClass = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateChange, L"iGreatPeopleRateChange");

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

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getFlavorTypes(), GC.getNumFlavorTypes());

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercent, L"iHealthPercent");
	pXML->GetOptionalChildXmlValByName(&m_iHappinessPercent, L"iHappinessPercent");
	pXML->GetOptionalChildXmlValByName(&m_bSlave, L"bSlave");
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	//Team Project (1)
	//TB Specialist Tags
	// int
	pXML->GetOptionalChildXmlValByName(&m_iInsidiousness, L"iInsidiousness");
	pXML->GetOptionalChildXmlValByName(&m_iInvestigation, L"iInvestigation");
	// int vector utilizing struct with delayed resolution
	if(pXML->TryMoveToXmlFirstChild(L"TechHappinessTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"TechHappinessType" );
		m_aTechHappinessTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TechHappinessType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TechType");
					pXML->GetChildXmlValByName(&(m_aTechHappinessTypes[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aTechHappinessTypes[i].eTech), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TechHappinessType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TechHealthTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"TechHealthType" );
		m_aTechHealthTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TechHealthType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TechType");
					pXML->GetChildXmlValByName(&(m_aTechHealthTypes[i].iModifier), L"iModifier");
					GC.addDelayedResolution((int*)&(m_aTechHealthTypes[i].eTech), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TechHealthType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

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

/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvSpecialistInfo::copyNonDefaults(CvSpecialistInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	if (getTexture() == cDefault) setTexture(pClassInfo->getTexture());
	if (isVisible() == bDefault) m_bVisible = pClassInfo->isVisible();
	if (getGreatPeopleUnitClass() == iTextDefault) m_iGreatPeopleUnitClass = pClassInfo->getGreatPeopleUnitClass();
	if (getGreatPeopleRateChange() == iDefault) m_iGreatPeopleRateChange = pClassInfo->getGreatPeopleRateChange();

	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getYieldChange(i) == iDefault && pClassInfo->getYieldChange(i) != iDefault)
		{
			if ( m_piYieldChange == NULL )
			{
				pXML->InitList(&m_piYieldChange, NUM_YIELD_TYPES);
			}
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
	}

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( getCommerceChange(i) == iDefault ) m_piCommerceChange[i] = pClassInfo->getCommerceChange(i);
	}

	if (getExperience() == iDefault) m_iExperience = pClassInfo->getExperience();

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);

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
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getHealthPercent() == iDefault) m_iHealthPercent = pClassInfo->getHealthPercent();
	if (getHappinessPercent() == iDefault) m_iHappinessPercent = pClassInfo->getHappinessPercent();
	if (isSlave() == bDefault) m_bSlave = pClassInfo->isSlave();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	//Team Project (1)
	//TB Specialist Tags
	// int
	if (getInsidiousness() == iDefault) m_iInsidiousness = pClassInfo->getInsidiousness();
	if (getInvestigation() == iDefault) m_iInvestigation = pClassInfo->getInvestigation();

	// int vectors utilizing struct with delayed resolution
	if (getNumTechHappinessTypes() == 0)
	{
		int iNum = pClassInfo->getNumTechHappinessTypes();
		m_aTechHappinessTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aTechHappinessTypes[i].iModifier = pClassInfo->m_aTechHappinessTypes[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aTechHappinessTypes[i].eTech), (int*)&(pClassInfo->m_aTechHappinessTypes[i].eTech));
		}
	}

	if (getNumTechHealthTypes() == 0)
	{
		int iNum = pClassInfo->getNumTechHealthTypes();
		m_aTechHealthTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aTechHealthTypes[i].iModifier = pClassInfo->m_aTechHealthTypes[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aTechHealthTypes[i].eTech), (int*)&(pClassInfo->m_aTechHealthTypes[i].eTech));
		}
	}

	if (getNumUnitCombatExperienceTypes() == 0)
	{
		int iNum = pClassInfo->getNumUnitCombatExperienceTypes();
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
void CvSpecialistInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iGreatPeopleUnitClass);
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

	// int vectors utilizing struct with delayed resolution
	int iNumElements;

	iNumElements = m_aTechHappinessTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTechHappinessTypes[i].eTech);
		CheckSum(iSum, m_aTechHappinessTypes[i].iModifier);
	}

	iNumElements = m_aTechHealthTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTechHealthTypes[i].eTech);
		CheckSum(iSum, m_aTechHealthTypes[i].iModifier);
	}

	iNumElements = m_aUnitCombatExperienceTypes.size();
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

//======================================================================================================
//					CvSpecialBuildingInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvSpecialBuildingInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialBuildingInfo::CvSpecialBuildingInfo() :
m_iObsoleteTech(NO_TECH),
m_iTechPrereq(NO_TECH),
m_iTechPrereqAnyone(NO_TECH),
m_iMaxPlayerInstances(-1),
m_bValid(false)/*,
m_piProductionTraits(NULL)*/
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvSpecialBuildingInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvSpecialBuildingInfo::~CvSpecialBuildingInfo()
{
	//SAFE_DELETE_ARRAY(m_piProductionTraits);
}

int CvSpecialBuildingInfo::getObsoleteTech( void ) const
{
	return m_iObsoleteTech;
}

int CvSpecialBuildingInfo::getTechPrereq( void ) const
{
	return m_iTechPrereq;
}

int CvSpecialBuildingInfo::getTechPrereqAnyone( void ) const
{
	return m_iTechPrereqAnyone;
}

int CvSpecialBuildingInfo::getMaxPlayerInstances() const
{
	return m_iMaxPlayerInstances;
}

bool CvSpecialBuildingInfo::isValid( void ) const
{
	return m_bValid;
}

// Arrays
//
//int CvSpecialBuildingInfo::getProductionTraits(int i) const
//{
//	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_piProductionTraits ? m_piProductionTraits[i] : 0;
//}

bool CvSpecialBuildingInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereq");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereqAnyone");
	m_iTechPrereqAnyone = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iMaxPlayerInstances, L"iMaxPlayerInstances", -1);

	pXML->GetOptionalChildXmlValByName(&m_bValid, L"bValid");

	//pXML->SetVariableListTagPair(&m_piProductionTraits, L"ProductionTraits", GC.getNumTraitInfos());

	return true;
}

void CvSpecialBuildingInfo::copyNonDefaults(CvSpecialBuildingInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	bool bDefault = false;
	int iDefault = 0;
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getObsoleteTech() == iDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getTechPrereq() == iDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getTechPrereqAnyone() == iDefault) m_iTechPrereqAnyone = pClassInfo->getTechPrereqAnyone();
	if (getMaxPlayerInstances() == -1) m_iMaxPlayerInstances = pClassInfo->getMaxPlayerInstances();

	if (isValid() == bDefault) m_bValid = pClassInfo->isValid();

	//for (int i = 0; i < GC.getNumTraitInfos(); i++)
	//{
	//	if (getProductionTraits(i) == iDefault && pClassInfo->getProductionTraits(i) != iDefault)
	//	{
	//		if ( NULL == m_piProductionTraits )
	//		{
	//			CvXMLLoadUtility::InitList(&m_piProductionTraits,GC.getNumTraitInfos(),iDefault);
	//		}
	//		m_piProductionTraits[i] = pClassInfo->getProductionTraits(i);
	//	}
	//}
}

void CvSpecialBuildingInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iTechPrereqAnyone);
	CheckSum(iSum, m_iMaxPlayerInstances);

	CheckSum(iSum, m_bValid);

	// Arrays

	//CheckSum(iSum, m_piProductionTraits, GC.getNumTraitInfos());
}

//======================================================================================================
//					CvBuildingClassInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBuildingClassInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBuildingClassInfo::CvBuildingClassInfo() :
m_iMaxGlobalInstances(0),
m_iMaxTeamInstances(0),
m_iMaxPlayerInstances(0),
m_iExtraPlayerInstances(0),
m_iDefaultBuildingIndex(NO_BUILDING),
m_bNoLimit(false),
m_bMonument(false),
m_piVictoryThreshold(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBuildingClassInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBuildingClassInfo::~CvBuildingClassInfo()
{
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
}

int CvBuildingClassInfo::getMaxGlobalInstances() const
{
	return m_iMaxGlobalInstances;
}

int CvBuildingClassInfo::getMaxTeamInstances() const
{
	return m_iMaxTeamInstances;
}

int CvBuildingClassInfo::getMaxPlayerInstances() const
{
	return m_iMaxPlayerInstances;
}

int CvBuildingClassInfo::getExtraPlayerInstances() const
{
	return m_iExtraPlayerInstances;
}

int CvBuildingClassInfo::getDefaultBuildingIndex() const
{
	return m_iDefaultBuildingIndex;
}

void CvBuildingClassInfo::setDefaultBuildingIndex(int i)
{
	m_iDefaultBuildingIndex = i;
}

bool CvBuildingClassInfo::isNoLimit() const
{
	return m_bNoLimit;
}

bool CvBuildingClassInfo::isMonument() const
{
	return m_bMonument;
}

// Arrays

int CvBuildingClassInfo::getVictoryThreshold(int i) const
{
	FAssertMsg(i < GC.getNumVictoryInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piVictoryThreshold ? m_piVictoryThreshold[i] : 0;
}

bool CvBuildingClassInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iMaxGlobalInstances, L"iMaxGlobalInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxTeamInstances, L"iMaxTeamInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxPlayerInstances, L"iMaxPlayerInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iExtraPlayerInstances, L"iExtraPlayerInstances");

	pXML->GetOptionalChildXmlValByName(&m_bNoLimit, L"bNoLimit");
	pXML->GetOptionalChildXmlValByName(&m_bMonument, L"bMonument");

	pXML->SetVariableListTagPair(&m_piVictoryThreshold, L"VictoryThresholds",  GC.getNumVictoryInfos());

	CvString szTextVal;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"DefaultBuilding");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 10/12/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvBuildingClassInfo::copyNonDefaults(CvBuildingClassInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	bool bDefault = false;
	int iDefault = 0;
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getMaxGlobalInstances() == -1) m_iMaxGlobalInstances = pClassInfo->getMaxGlobalInstances();
	if (getMaxTeamInstances() == -1) m_iMaxTeamInstances = pClassInfo->getMaxTeamInstances();
	if (getMaxPlayerInstances() == -1) m_iMaxPlayerInstances = pClassInfo->getMaxPlayerInstances();
	if (getExtraPlayerInstances() == iDefault) m_iExtraPlayerInstances = pClassInfo->getExtraPlayerInstances();

	if (isNoLimit() == bDefault) m_bNoLimit = pClassInfo->isNoLimit();
	if (isMonument() == bDefault) m_bMonument = pClassInfo->isMonument();

	for ( int i = 0; i < GC.getNumVictoryInfos(); i++ )
	{
		if (getVictoryThreshold(i) == iDefault && pClassInfo->getVictoryThreshold(i) != iDefault)
		{
			if ( NULL == m_piVictoryThreshold )
			{
				CvXMLLoadUtility::InitList(&m_piVictoryThreshold,GC.getNumVictoryInfos(),iDefault);
			}
			m_piVictoryThreshold[i] = pClassInfo->getVictoryThreshold(i);
		}
	}
}

bool CvBuildingClassInfo::readPass3()
{
	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}

/************************************************************************************************/
/* XMLCOPY								 10/12/07								MRGENIE	  */
/*																							  */
/* Assuming the modder purposly added an entry to this tag, we want to take the last enty set   */
/* by the modder and not the first as set by firaxis											*/
/************************************************************************************************/
/*
	m_iDefaultBuildingIndex = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);
*/
	int iTextDefault = -1;
	int iSize = m_aszExtraXMLforPass3.size();
	for ( int i = 0; i < iSize; i++ )
	{
		if ( GC.getInfoTypeForString(m_aszExtraXMLforPass3[iSize - ( i + 1)]) != iTextDefault)
		{
			m_iDefaultBuildingIndex = GC.getInfoTypeForString(m_aszExtraXMLforPass3[iSize - ( i + 1)]);
			break;
		}
	}
	m_aszExtraXMLforPass3.clear();

	return true;
}

void CvBuildingClassInfo::getCheckSum(unsigned int &iSum)
{
	CheckSum(iSum, m_iMaxGlobalInstances);
	CheckSum(iSum, m_iMaxTeamInstances);
	CheckSum(iSum, m_iMaxPlayerInstances);
	CheckSum(iSum, m_iExtraPlayerInstances);
	CheckSum(iSum, m_iDefaultBuildingIndex);

	CheckSum(iSum, m_bNoLimit);
	CheckSum(iSum, m_bMonument);

	// Arrays

	CheckSum(iSum, m_piVictoryThreshold, GC.getNumVictoryInfos());
}

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
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_iGoldPerProduction, L"iGoldPerProduction");
	pXML->GetOptionalChildXmlValByName(&m_iProductionPerPopulation, L"iProductionPerPopulation");

	pXML->GetOptionalChildXmlValByName(&m_bAnger, L"bAnger");

	return true;
}

void CvHurryInfo::copyNonDefaults(CvHurryInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getGoldPerProduction() == iDefault) m_iGoldPerProduction = pClassInfo->getGoldPerProduction();
	if (getProductionPerPopulation() == iDefault) m_iProductionPerPopulation = pClassInfo->getProductionPerPopulation();

	if (isAnger() == bDefault) m_bAnger = pClassInfo->isAnger();
}

void CvHurryInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iGoldPerProduction);
	CheckSum(iSum, m_iProductionPerPopulation);

	CheckSum(iSum, m_bAnger);
}

//======================================================================================================
//					CvProcessInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvProcessInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvProcessInfo::CvProcessInfo() :
m_iTechPrereq(NO_TECH),
m_paiProductionToCommerceModifier(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvProcessInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvProcessInfo::~CvProcessInfo()
{
	SAFE_DELETE_ARRAY(m_paiProductionToCommerceModifier);
}

int CvProcessInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

// Arrays

int CvProcessInfo::getProductionToCommerceModifier(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiProductionToCommerceModifier ? m_paiProductionToCommerceModifier[i] : 0;
}

bool CvProcessInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereq");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);

	if (pXML->TryMoveToXmlFirstChild(L"ProductionToCommerceModifiers"))
	{
		pXML->SetCommerce(&m_paiProductionToCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiProductionToCommerceModifier);
	}

	return true;
}

void CvProcessInfo::copyNonDefaults(CvProcessInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();

	if (!m_paiProductionToCommerceModifier) CvXMLLoadUtility::InitList(&m_paiProductionToCommerceModifier, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_paiProductionToCommerceModifier[i] == iDefault )
		{
			m_paiProductionToCommerceModifier[i] = pClassInfo->getProductionToCommerceModifier(i);
		}
	}
}

void CvProcessInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iTechPrereq);

	// Arrays

	CheckSum(iSum, m_paiProductionToCommerceModifier, NUM_COMMERCE_TYPES);
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvEmphasizeInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvEmphasizeInfo::CvEmphasizeInfo() :
m_bAvoidGrowth(false),
m_bGreatPeople(false),
m_piYieldModifiers(NULL),
m_piCommerceModifiers(NULL)
/************************************************************************************************/
/* Afforess					  Start		 02/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
,m_bAvoidAngryCitizens(false)
,m_bAvoidUnhealthyCitizens(false)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvEmphasizeInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvEmphasizeInfo::~CvEmphasizeInfo()
{
	SAFE_DELETE_ARRAY(m_piYieldModifiers);
	SAFE_DELETE_ARRAY(m_piCommerceModifiers);
}

bool CvEmphasizeInfo::isAvoidGrowth() const
{
	return m_bAvoidGrowth;
}

bool CvEmphasizeInfo::isGreatPeople() const
{
	return m_bGreatPeople;
}

/************************************************************************************************/
/* Afforess					  Start		 02/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvEmphasizeInfo::isAvoidAngryCitizens() const
{
	return m_bAvoidAngryCitizens;
}

bool CvEmphasizeInfo::isAvoidUnhealthyCitizens() const
{
	return m_bAvoidUnhealthyCitizens;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/


// Arrays

int CvEmphasizeInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifiers ? m_piYieldModifiers[i] : 0;
}

int CvEmphasizeInfo::getCommerceChange(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCommerceModifiers ? m_piCommerceModifiers[i] : 0;
}

//
// read from XML
//
bool CvEmphasizeInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetChildXmlValByName(&m_bAvoidGrowth, L"bAvoidGrowth");
	pXML->GetChildXmlValByName(&m_bGreatPeople, L"bGreatPeople");

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifiers);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifiers);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifiers);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifiers);
	}
/************************************************************************************************/
/* Afforess					  Start		 02/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetChildXmlValByName(&m_bAvoidAngryCitizens, L"bAvoidAngryCitizens");
	pXML->GetChildXmlValByName(&m_bAvoidUnhealthyCitizens, L"bAvoidUnhealthyCitizens");
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	return true;
}
void CvEmphasizeInfo::copyNonDefaults(CvEmphasizeInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (isAvoidGrowth() == bDefault) m_bAvoidGrowth = pClassInfo->isAvoidGrowth();
	if (isGreatPeople() == bDefault) m_bGreatPeople = pClassInfo->isGreatPeople();

	if (!m_piYieldModifiers) CvXMLLoadUtility::InitList(&m_piYieldModifiers, NUM_YIELD_TYPES);
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( m_piYieldModifiers[i] == iDefault )
		{
			m_piYieldModifiers[i] = pClassInfo->getYieldChange(i);
		}
	}

	if (!m_piCommerceModifiers) CvXMLLoadUtility::InitList(&m_piCommerceModifiers, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_piCommerceModifiers[i] == iDefault )
		{
			m_piCommerceModifiers[i] = pClassInfo->getCommerceChange(i);
		}
	}
/************************************************************************************************/
/* Afforess					  Start		 02/10/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (isAvoidAngryCitizens() == false) m_bAvoidAngryCitizens = pClassInfo->isAvoidAngryCitizens();
	if (isAvoidUnhealthyCitizens() == false) m_bAvoidUnhealthyCitizens = pClassInfo->isAvoidUnhealthyCitizens();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

}

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
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXml))
	{
		return false;
	}

	pXml->GetChildXmlValByName(&m_iPopulationPercent, L"iPopulationPercent");
	pXml->GetChildXmlValByName(&m_iCityPercent, L"iCityPercent");

	return true;
}

void CvUpkeepInfo::copyNonDefaults(CvUpkeepInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getPopulationPercent() == iDefault) m_iPopulationPercent = pClassInfo->getPopulationPercent();
	if (getCityPercent() == iDefault) m_iCityPercent = pClassInfo->getCityPercent();
}

void CvUpkeepInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iPopulationPercent);
	CheckSum(iSum, m_iCityPercent);
}

