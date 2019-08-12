#include "CvGameCoreDLL.h"

//======================================================================================================
//					CvDiplomacyResponse
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvDiplomacyResponse()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyResponse::CvDiplomacyResponse() :
m_iNumDiplomacyText(0),
m_pbCivilizationTypes(NULL),
m_pbLeaderHeadTypes(NULL),
m_pbAttitudeTypes(NULL),
m_pbDiplomacyPowerTypes(NULL),
m_paszDiplomacyText(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvDiplomacyResponse()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyResponse::~CvDiplomacyResponse()
{
	SAFE_DELETE_ARRAY(m_pbCivilizationTypes);
	SAFE_DELETE_ARRAY(m_pbLeaderHeadTypes);
	SAFE_DELETE_ARRAY(m_pbAttitudeTypes);
	SAFE_DELETE_ARRAY(m_pbDiplomacyPowerTypes);
	SAFE_DELETE_ARRAY(m_paszDiplomacyText);
}

int CvDiplomacyResponse::getNumDiplomacyText()
{
	return m_iNumDiplomacyText;
}

void CvDiplomacyResponse::setNumDiplomacyText(int i)
{
	m_iNumDiplomacyText = i;
}

bool CvDiplomacyResponse::getCivilizationTypes(int i)
{
	FAssertMsg(i < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbCivilizationTypes ? m_pbCivilizationTypes[i] : false;
}

bool* CvDiplomacyResponse::getCivilizationTypes() const
{
	return m_pbCivilizationTypes;
}

void CvDiplomacyResponse::setCivilizationTypes(int i, bool bVal)
{
	FAssertMsg(i < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if ( NULL == m_pbCivilizationTypes )
	{
		CvXMLLoadUtility::InitList(&m_pbCivilizationTypes,GC.getNumCivilizationInfos(),false);
	}
	m_pbCivilizationTypes[i] = bVal;
}

bool CvDiplomacyResponse::getLeaderHeadTypes(int i)
{
	FAssertMsg(i < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbLeaderHeadTypes ? m_pbLeaderHeadTypes[i] : false;
}

bool* CvDiplomacyResponse::getLeaderHeadTypes() const
{
	return m_pbLeaderHeadTypes;
}

void CvDiplomacyResponse::setLeaderHeadTypes(int i, bool bVal)
{
	FAssertMsg(i < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if ( NULL == m_pbLeaderHeadTypes )
	{
		CvXMLLoadUtility::InitList(&m_pbLeaderHeadTypes,GC.getNumLeaderHeadInfos(),false);
	}
	m_pbLeaderHeadTypes[i] = bVal;
}

bool CvDiplomacyResponse::getAttitudeTypes(int i) const
{
	FAssertMsg(i < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbAttitudeTypes ? m_pbAttitudeTypes[i] : false;
}

bool* CvDiplomacyResponse::getAttitudeTypes() const
{
	return m_pbAttitudeTypes;
}

void CvDiplomacyResponse::setAttitudeTypes(int i, bool bVal)
{
	FAssertMsg(i < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if ( NULL == m_pbAttitudeTypes )
	{
		CvXMLLoadUtility::InitList(&m_pbAttitudeTypes,NUM_ATTITUDE_TYPES,false);
	}
	m_pbAttitudeTypes[i] = bVal;
}

bool CvDiplomacyResponse::getDiplomacyPowerTypes(int i)
{
	FAssertMsg(i < NUM_DIPLOMACYPOWER_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbDiplomacyPowerTypes ? m_pbDiplomacyPowerTypes[i] : false;
}

bool* CvDiplomacyResponse::getDiplomacyPowerTypes() const
{
	return m_pbDiplomacyPowerTypes;
}

void CvDiplomacyResponse::setDiplomacyPowerTypes(int i, bool bVal)
{
	FAssertMsg(i < NUM_DIPLOMACYPOWER_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if ( NULL == m_pbDiplomacyPowerTypes )
	{
		CvXMLLoadUtility::InitList(&m_pbDiplomacyPowerTypes,NUM_DIPLOMACYPOWER_TYPES,false);
	}
	m_pbDiplomacyPowerTypes[i] = bVal;
}

const TCHAR* CvDiplomacyResponse::getDiplomacyText(int i) const
{
	return m_paszDiplomacyText[i];
}

const CvString* CvDiplomacyResponse::getDiplomacyText() const
{
	return m_paszDiplomacyText;
}

void CvDiplomacyResponse::setDiplomacyText(int i, CvString szText)
{
	FAssertMsg(i < getNumDiplomacyText(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	m_paszDiplomacyText[i] = szText;
}

bool CvDiplomacyResponse::read(CvXMLLoadUtility* pXML)
{
	pXML->SetVariableListTagPair(&m_pbCivilizationTypes, L"Civilizations", GC.getNumCivilizationInfos());
	// Leaders
	pXML->SetVariableListTagPair(&m_pbLeaderHeadTypes, L"Leaders", GC.getNumLeaderHeadInfos());
	// AttitudeTypes
	pXML->SetVariableListTagPair(&m_pbAttitudeTypes, L"Attitudes", NUM_ATTITUDE_TYPES);
	// PowerTypes
	pXML->SetVariableListTagPair(&m_pbDiplomacyPowerTypes, L"DiplomacyPowers", GC.getDiplomacyPowerTypes(), NUM_DIPLOMACYPOWER_TYPES);
	// DiplomacyText
	if (pXML->TryMoveToXmlFirstChild(L"DiplomacyText"))
	{
		pXML->SetStringList(&m_paszDiplomacyText, &m_iNumDiplomacyText);
		pXML->MoveToXmlParent();
	}

	return true;
}
void CvDiplomacyResponse::UpdateDiplomacies(CvDiplomacyInfo* pDiplomacyInfo, int iIndex)
{
	bool bDefault = false;
	CvXMLLoadUtilityModTools* pModTools = new CvXMLLoadUtilityModTools;

	// We use the String append mechanism from WOC default = assume the modder added his strings
	// purposly, so those are on the first place, the strings previously are appended after them
	CvString* m_paszNewNames = new CvString[pDiplomacyInfo->getNumDiplomacyText(iIndex)];
	for ( int i = 0; i < pDiplomacyInfo->getNumDiplomacyText(iIndex); i++)
	{
		m_paszNewNames[i] = pDiplomacyInfo->getDiplomacyText(iIndex, i);
	}

	pModTools->StringArrayExtend(&m_paszDiplomacyText, &m_iNumDiplomacyText, &m_paszNewNames, pDiplomacyInfo->getNumDiplomacyText(iIndex));
	SAFE_DELETE_ARRAY(m_paszNewNames);
	SAFE_DELETE(pModTools);

	// if anything is true, we don't overwrite(assuming the modder did set it true purposly
	for ( int i = 0; i < GC.getNumLeaderHeadInfos(); ++i)
	{
		if ((m_pbLeaderHeadTypes == NULL || m_pbLeaderHeadTypes[i] == bDefault) && pDiplomacyInfo->getLeaderHeadTypes(iIndex, i) != bDefault)
		{
			if ( NULL == m_pbLeaderHeadTypes )
			{
				CvXMLLoadUtility::InitList(&m_pbLeaderHeadTypes,GC.getNumLeaderHeadInfos(),bDefault);
			}
			m_pbLeaderHeadTypes[i] = pDiplomacyInfo->getLeaderHeadTypes(iIndex, i);
		}
	}
	for ( int i = 0; i < NUM_ATTITUDE_TYPES; ++i)
	{
		if ((m_pbAttitudeTypes == NULL || m_pbAttitudeTypes[i] == bDefault) && pDiplomacyInfo->getAttitudeTypes(iIndex, i) != bDefault)
		{
			if ( NULL == m_pbAttitudeTypes )
			{
				CvXMLLoadUtility::InitList(&m_pbAttitudeTypes,NUM_ATTITUDE_TYPES,bDefault);
			}
			m_pbAttitudeTypes[i] = pDiplomacyInfo->getAttitudeTypes(iIndex, i);
		}
	}
	for ( int i = 0; i < NUM_DIPLOMACYPOWER_TYPES; ++i)
	{
		if ((m_pbDiplomacyPowerTypes == NULL || m_pbDiplomacyPowerTypes[i] == bDefault) && pDiplomacyInfo->getDiplomacyPowerTypes(iIndex, i) != bDefault)
		{
			if ( NULL == m_pbDiplomacyPowerTypes )
			{
				CvXMLLoadUtility::InitList(&m_pbDiplomacyPowerTypes,NUM_DIPLOMACYPOWER_TYPES,bDefault);
			}
			m_pbDiplomacyPowerTypes[i] = pDiplomacyInfo->getDiplomacyPowerTypes(iIndex, i);
		}
	}
}

//======================================================================================================
//					CvTechInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTechInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTechInfo::CvTechInfo() :
m_iAdvisorType(NO_ADVISOR),
m_iAIWeight(0),
m_iAITradeModifier(0),
m_iResearchCost(0),
m_iAdvancedStartCost(100),
m_iAdvancedStartCostIncrease(0),
m_iEra(NO_ERA),
m_iTradeRoutes(0),
m_iFeatureProductionModifier(0),
m_iWorkerSpeedModifier(0),
//DPII < Maintenance Modifier >
m_iMaintenanceModifier(0),
m_iDistanceMaintenanceModifier(0),
m_iNumCitiesMaintenanceModifier(0),
m_iCoastalDistanceMaintenanceModifier(0),
//DPII < Maintenance Modifier >
m_iFirstFreeUnitClass(NO_UNITCLASS),
m_iFirstFreeProphet(NO_UNITCLASS),
m_iHealth(0),
m_iHappiness(0),
m_iFirstFreeTechs(0),
m_iAssetValue(0),
m_iPowerValue(0),
m_iGridX(0),
m_iGridY(0),
m_bRepeat(false),
m_bTrade(false),
m_bDisable(false),
m_bGoodyTech(false),
m_bExtraWaterSeeFrom(false),
m_bMapCentering(false),
m_bMapVisible(false),
m_bMapTrading(false),
m_bTechTrading(false),
m_bGoldTrading(false),
m_bOpenBordersTrading(false),
m_bDefensivePactTrading(false),
m_bPermanentAllianceTrading(false),
m_bVassalStateTrading(false),
m_bBridgeBuilding(false),
m_bIrrigation(false),
m_bIgnoreIrrigation(false),
m_bWaterWork(false),
m_bRiverTrade(false),
/************************************************************************************************/
/* DCM									 04/19/09								Johny Smith  */
/************************************************************************************************/
// Dale - AB: Bombing START
m_bDCMAirBombTech1(0),
m_bDCMAirBombTech2(0),
// Dale - AB: Bombing END
/************************************************************************************************/
/* DCM									 END												  */
/************************************************************************************************/
m_piDomainExtraMoves(NULL),
m_piFlavorValue(NULL),
m_piPrereqOrTechs(NULL),
m_piPrereqAndTechs(NULL),
m_pbCommerceFlexible(NULL),
m_pbTerrainTrade(NULL),
//ls612: Tech Commerce Modifiers
m_piCommerceModifier(NULL)
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_bEmbassyTrading(false)
,m_bCanPassPeaks(false)
,m_bMoveFastPeaks(false)
,m_bCanFoundOnPeaks(false)
,m_bEnableDarkAges(false)
,m_bRebaseAnywhere(false)
,m_bEnablesDesertFarming(false)
,m_iInflationModifier(0)
,m_iGlobalTradeModifier(0)
,m_iGlobalForeignTradeModifier(0)
,m_iTradeMissionModifier(0)
,m_iCorporationRevenueModifier(0)
,m_iCorporationMaintenanceModifier(0)
,m_iPrereqGameOption(NO_GAMEOPTION)
,m_piFreeSpecialistCount(NULL)
//,m_paiPrereqBuildingClass(NULL)
//,m_paiPrereqOrBuildingClass(NULL)
,m_piOriginalPrereqOrTechs(NULL)
,m_piOriginalPrereqAndTechs(NULL)
,m_piUnitClassStrengthChange(NULL)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
//TB Tech Tags
,m_bGlobal(false)
//TB Tech Tags end
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTechInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTechInfo::~CvTechInfo()
{
	SAFE_DELETE_ARRAY(m_piDomainExtraMoves);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piPrereqOrTechs);
	SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_pbCommerceFlexible);
	SAFE_DELETE_ARRAY(m_pbTerrainTrade);
	//ls612: Tech Commerce Modifiers
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
/************************************************************************************************/
/* Afforess					  Start		 04/01/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	//SAFE_DELETE_ARRAY(m_paiPrereqBuildingClass);
	//SAFE_DELETE_ARRAY(m_paiPrereqOrBuildingClass);
	SAFE_DELETE_ARRAY(m_piOriginalPrereqOrTechs);
	SAFE_DELETE_ARRAY(m_piOriginalPrereqAndTechs);
	SAFE_DELETE_ARRAY(m_piUnitClassStrengthChange);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	for (int i=0; i<(int)m_aPrereqBuildingClass.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPrereqBuildingClass[i]));
	}
	for (int i=0; i<(int)m_aPrereqOrBuildingClass.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aPrereqOrBuildingClass[i]));
	}
}

//	Validate tech info data after load (debug only)
void CvTechInfo::validate()
{
#ifdef _DEBUG
	for(int iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		CvTechInfo&	info = GC.getTechInfo((TechTypes)iI);

		//	Arbitrary tests - add cases as suspected bugs crop up to pre-detect
		FAssert(info.getEra() >= 0);
		FAssert(info.getEra() < GC.getNumEraInfos());
	}
#endif
}

int CvTechInfo::getAdvisorType() const
{
	return m_iAdvisorType;
}

int CvTechInfo::getAIWeight() const
{
	return m_iAIWeight;
}

int CvTechInfo::getAITradeModifier() const
{
	return m_iAITradeModifier;
}

int CvTechInfo::getResearchCost() const
{
	return m_iResearchCost;
}

int CvTechInfo::getAdvancedStartCost() const
{
	return m_iAdvancedStartCost;
}

int CvTechInfo::getAdvancedStartCostIncrease() const
{
	return m_iAdvancedStartCostIncrease;
}

int CvTechInfo::getEra() const
{
	return m_iEra;
}

int CvTechInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}

int CvTechInfo::getFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}

int CvTechInfo::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}
//DPII < Maintenance Modifier >
int CvTechInfo::getMaintenanceModifier() const
{
	return m_iMaintenanceModifier;
}

int CvTechInfo::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}

int CvTechInfo::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;
}

int CvTechInfo::getCoastalDistanceMaintenanceModifier() const
{
	return m_iCoastalDistanceMaintenanceModifier;
}
//DPII < Maintenance Modifier >
int CvTechInfo::getFirstFreeUnitClass() const
{
	return m_iFirstFreeUnitClass;
}

int CvTechInfo::getFirstFreeProphet() const
{
	return m_iFirstFreeProphet;
}

int CvTechInfo::getHealth() const
{
	return m_iHealth;
}

int CvTechInfo::getHappiness() const
{
	return m_iHappiness;
}

int CvTechInfo::getFirstFreeTechs() const
{
	return m_iFirstFreeTechs;
}

int CvTechInfo::getAssetValue(bool bForLoad) const
{
	if (!bForLoad)
	{
		return m_iAssetValue * 100;
	}
	return m_iAssetValue;
}

int CvTechInfo::getPowerValue(bool bForLoad) const
{
	if (!bForLoad)
	{
		return m_iPowerValue * 100;
	}
	return m_iPowerValue;
}

int CvTechInfo::getGridX() const
{
	return m_iGridX;
}

int CvTechInfo::getGridY() const
{
	return m_iGridY;
}

bool CvTechInfo::isRepeat() const
{
	return m_bRepeat;
}

bool CvTechInfo::isTrade() const
{
	return m_bTrade;
}

bool CvTechInfo::isDisable() const
{
	return m_bDisable;
}

bool CvTechInfo::isGoodyTech() const
{
	return m_bGoodyTech;
}

bool CvTechInfo::isExtraWaterSeeFrom() const
{
	return m_bExtraWaterSeeFrom;
}

bool CvTechInfo::isMapCentering() const
{
	return m_bMapCentering;
}

bool CvTechInfo::isMapVisible() const
{
	return m_bMapVisible;
}

bool CvTechInfo::isMapTrading() const
{
	return m_bMapTrading;
}

bool CvTechInfo::isTechTrading() const
{
	return m_bTechTrading;
}

bool CvTechInfo::isGoldTrading() const
{
	return m_bGoldTrading;
}

bool CvTechInfo::isOpenBordersTrading() const
{
	return m_bOpenBordersTrading;
}

bool CvTechInfo::isDefensivePactTrading() const
{
	return m_bDefensivePactTrading;
}

bool CvTechInfo::isPermanentAllianceTrading() const
{
	return m_bPermanentAllianceTrading;
}

bool CvTechInfo::isVassalStateTrading() const
{
	return m_bVassalStateTrading;
}

bool CvTechInfo::isBridgeBuilding() const
{
	return m_bBridgeBuilding;
}

bool CvTechInfo::isIrrigation() const
{
	return m_bIrrigation;
}

bool CvTechInfo::isIgnoreIrrigation() const
{
	return m_bIgnoreIrrigation;
}

bool CvTechInfo::isWaterWork() const
{
	return m_bWaterWork;
}

bool CvTechInfo::isRiverTrade() const
{
	return m_bRiverTrade;
}

/************************************************************************************************/
/* DCM									 04/19/09								Johny Smith  */
/************************************************************************************************/
bool CvTechInfo::getDCMAirBombTech1() const
{
	return m_bDCMAirBombTech1;
}

bool CvTechInfo::getDCMAirBombTech2() const
{
	return m_bDCMAirBombTech2;
}
/************************************************************************************************/
/* DCM									 END												  */
/************************************************************************************************/
std::wstring CvTechInfo::getQuote()	const
{
	CvWString text = gDLL->getText(m_szQuoteKey);
	FAssert(!text.empty());
	return text;
}

void CvTechInfo::setQuoteKey(const TCHAR* szVal)
{
	m_szQuoteKey = szVal;
}
const TCHAR* CvTechInfo::getQuoteKey()
{
	return m_szQuoteKey;
}

const TCHAR* CvTechInfo::getSound() const
{
	return m_szSound;
}

void CvTechInfo::setSound(const TCHAR* szVal)
{
	m_szSound = szVal;
}

const TCHAR* CvTechInfo::getSoundMP() const
{
	return m_szSoundMP;
}

void CvTechInfo::setSoundMP(const TCHAR* szVal)
{
	m_szSoundMP = szVal;
}

// Arrays

int CvTechInfo::getDomainExtraMoves(int i) const
{
	return m_piDomainExtraMoves ? m_piDomainExtraMoves[i] : 0;
}

int CvTechInfo::getFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvTechInfo::getPrereqOrTechs(int i) const
{
	return m_piPrereqOrTechs ? m_piPrereqOrTechs[i] : -1;
}

int CvTechInfo::getPrereqAndTechs(int i) const
{
	return m_piPrereqAndTechs ? m_piPrereqAndTechs[i] : -1;
}

bool CvTechInfo::isCommerceFlexible(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbCommerceFlexible ? m_pbCommerceFlexible[i] : false;
}

bool CvTechInfo::isTerrainTrade(int i) const
{
	return m_pbTerrainTrade ? m_pbTerrainTrade[i] : false;
}

//ls612: Tech Commerce Modifiers
int CvTechInfo::getCommerceModifier(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int* CvTechInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvTechInfo::isEmbassyTrading() const
{
	return m_bEmbassyTrading;
}

bool CvTechInfo::isEnablesDesertFarming() const
{
	return m_bEnablesDesertFarming;
}

bool CvTechInfo::isCanPassPeaks() const
{
	return m_bCanPassPeaks;
}

bool CvTechInfo::isMoveFastPeaks() const
{
	return m_bMoveFastPeaks;
}

bool CvTechInfo::isCanFoundOnPeaks() const
{
	return m_bCanFoundOnPeaks;
}
bool CvTechInfo::isEnableDarkAges() const
{
	return m_bEnableDarkAges;
}
bool CvTechInfo::isRebaseAnywhere() const
{
	return m_bRebaseAnywhere;
}
int CvTechInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}
int CvTechInfo::getGlobalTradeModifier() const
{
	return m_iGlobalTradeModifier;
}
int CvTechInfo::getGlobalForeignTradeModifier() const
{
	return  m_iGlobalForeignTradeModifier;
}
int CvTechInfo::getTradeMissionModifier() const
{
	return m_iTradeMissionModifier;
}
int CvTechInfo::getCorporationRevenueModifier() const
{
	return m_iCorporationRevenueModifier;
}
int CvTechInfo::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}
int CvTechInfo::getPrereqGameOption() const
{
	return m_iPrereqGameOption;
}
int CvTechInfo::getFreeSpecialistCount(int i) const
{
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}

int CvTechInfo::getNumPrereqBuildingClasses() const
{
	return (int)m_aPrereqBuildingClass.size();
}

PrereqBuildingClass& CvTechInfo::getPrereqBuildingClass(int iIndex)
{
	return m_aPrereqBuildingClass[iIndex];
}

int CvTechInfo::getPrereqBuildingClassType(int iIndex)
{
	return (int)m_aPrereqBuildingClass[iIndex].eBuildingClass;
}

int CvTechInfo::getPrereqBuildingClassMinimumRequired(int iIndex)
{
	return (int)m_aPrereqBuildingClass[iIndex].iMinimumRequired;
}

int CvTechInfo::getNumPrereqOrBuildingClasses() const
{
	return (int)m_aPrereqOrBuildingClass.size();
}

PrereqBuildingClass& CvTechInfo::getPrereqOrBuildingClass(int iIndex)
{
	return m_aPrereqOrBuildingClass[iIndex];
}

int CvTechInfo::getPrereqOrBuildingClassType(int iIndex)
{
	return (int)m_aPrereqOrBuildingClass[iIndex].eBuildingClass;
}

int CvTechInfo::getPrereqOrBuildingClassMinimumRequired(int iIndex)
{
	return (int)m_aPrereqOrBuildingClass[iIndex].iMinimumRequired;
}

//int CvTechInfo::getPrereqBuildingClass(int i) const
//{
//	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_paiPrereqBuildingClass ? m_paiPrereqBuildingClass[i] : false;
//}
//
//int CvTechInfo::getPrereqBuildingVectorSize()					{return m_aszPrereqBuildingClassforPass3.size();}
//CvString CvTechInfo::getPrereqBuildingNamesVectorElement(int i)	{return m_aszPrereqBuildingClassforPass3[i];}
//int CvTechInfo::getPrereqBuildingClassValuesVectorElement(int i)		{return m_aiPrereqBuildingClassforPass3[i];}
//
//int CvTechInfo::getPrereqOrBuildingClass(int i) const
//{
//	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_paiPrereqOrBuildingClass ? m_paiPrereqOrBuildingClass[i] : false;
//}
//
//int CvTechInfo::getPrereqOrBuildingVectorSize()					{return m_aszPrereqOrBuildingClassforPass3.size();}
//CvString CvTechInfo::getPrereqOrBuildingNamesVectorElement(int i)	{return m_aszPrereqOrBuildingClassforPass3[i];}
//int CvTechInfo::getPrereqOrBuildingClassValuesVectorElement(int i)		{return m_aiPrereqOrBuildingClassforPass3[i];}

void CvTechInfo::setPrereqOrTech(int i, int iTech)
{
	if ( getPrereqOrTechs(i) != iTech )
	{
		if ( NULL == m_piPrereqOrTechs )
		{
			CvXMLLoadUtility::InitList(&m_piPrereqOrTechs,GC.getNUM_OR_TECH_PREREQS(),-1);
		}
		m_piPrereqOrTechs[i] = iTech;
	}
}

void CvTechInfo::setPrereqAndTech(int i, int iTech)
{
	if ( getPrereqAndTechs(i) != iTech )
	{
		if ( NULL == m_piPrereqAndTechs )
		{
			CvXMLLoadUtility::InitList(&m_piPrereqAndTechs,GC.getNUM_AND_TECH_PREREQS(),-1);
		}
		m_piPrereqAndTechs[i] = iTech;
	}
}

void CvTechInfo::setGridX(int i)
{
	m_iGridX = i;
}


int CvTechInfo::getOriginalPrereqOrTechs(int i) const
{
	return m_piOriginalPrereqOrTechs ? m_piOriginalPrereqOrTechs[i] : -1;
}

void CvTechInfo::setOriginalPrereqOrTech(int i, int iTech)
{
	if ( getOriginalPrereqOrTechs(i) != iTech )
	{
		if ( NULL == m_piOriginalPrereqOrTechs )
		{
			CvXMLLoadUtility::InitList(&m_piOriginalPrereqOrTechs,GC.getNUM_OR_TECH_PREREQS(),-1);
		}
		m_piOriginalPrereqOrTechs[i] = iTech;
	}
}

void CvTechInfo::setOriginalPrereqAndTech(int i, int iTech)
{
	if ( getOriginalPrereqAndTechs(i) != iTech )
	{
		if ( NULL == m_piOriginalPrereqAndTechs )
		{
			CvXMLLoadUtility::InitList(&m_piOriginalPrereqAndTechs,GC.getNUM_AND_TECH_PREREQS(),-1);
		}
		m_piOriginalPrereqAndTechs[i] = iTech;
	}
}

int CvTechInfo::getOriginalPrereqAndTechs(int i) const
{
	return m_piOriginalPrereqAndTechs ? m_piOriginalPrereqAndTechs[i] : -1;
}

int CvTechInfo::getUnitClassStrengthChange(int iUnit, bool bForLoad) const
{
	//if (!bForLoad && GC.getGameINLINE().isOption(GAMEOPTION_SIZE_MATTERS))
	//{
	//	int iTotal = m_piUnitClassStrengthChange[iUnit] * 100;
	//	return m_piUnitClassStrengthChange ? iTotal : 0;
	//}
	return m_piUnitClassStrengthChange ? m_piUnitClassStrengthChange[iUnit] : 0;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
//TB Tech Tags

bool CvTechInfo::isGlobal() const
{
	return m_bGlobal;
}
//TB Tech Tags end

bool CvTechInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Advisor");
	m_iAdvisorType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iAIWeight, L"iAIWeight");
	pXML->GetOptionalChildXmlValByName(&m_iAITradeModifier, L"iAITradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iResearchCost, L"iCost");
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCost, L"iAdvancedStartCost", 100);
	pXML->GetOptionalChildXmlValByName(&m_iAdvancedStartCostIncrease, L"iAdvancedStartCostIncrease");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Era");
	m_iEra = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FirstFreeUnitClass");
	m_iFirstFreeUnitClass = pXML->GetInfoClass(szTextVal);

#ifdef C2C_BUILD
	pXML->GetOptionalChildXmlValByName(szTextVal, L"FirstFreeProphet");
	m_iFirstFreeProphet = pXML->GetInfoClass(szTextVal);
#endif

	pXML->GetOptionalChildXmlValByName(&m_iFeatureProductionModifier, L"iFeatureProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorkerSpeedModifier, L"iWorkerSpeedModifier");
	//DPII < Maintenance Modifiers >
	pXML->GetOptionalChildXmlValByName(&m_iMaintenanceModifier, L"iMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCoastalDistanceMaintenanceModifier, L"iCoastalDistanceMaintenanceModifier");
	//DPII < Maintenance Modifiers >
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iFirstFreeTechs, L"iFirstFreeTechs");
	pXML->GetOptionalChildXmlValByName(&m_iAssetValue, L"iAsset");
	pXML->GetOptionalChildXmlValByName(&m_iPowerValue, L"iPower");
	pXML->GetOptionalChildXmlValByName(&m_bRepeat, L"bRepeat");
	pXML->GetOptionalChildXmlValByName(&m_bTrade, L"bTrade");
	pXML->GetOptionalChildXmlValByName(&m_bDisable, L"bDisable");
	pXML->GetOptionalChildXmlValByName(&m_bGoodyTech, L"bGoodyTech");
	pXML->GetOptionalChildXmlValByName(&m_bExtraWaterSeeFrom, L"bExtraWaterSeeFrom");
	pXML->GetOptionalChildXmlValByName(&m_bMapCentering, L"bMapCentering");
	pXML->GetOptionalChildXmlValByName(&m_bMapVisible, L"bMapVisible");
	pXML->GetOptionalChildXmlValByName(&m_bMapTrading, L"bMapTrading");
	pXML->GetOptionalChildXmlValByName(&m_bTechTrading, L"bTechTrading");
	pXML->GetOptionalChildXmlValByName(&m_bGoldTrading, L"bGoldTrading");
	pXML->GetOptionalChildXmlValByName(&m_bOpenBordersTrading, L"bOpenBordersTrading");
	pXML->GetOptionalChildXmlValByName(&m_bDefensivePactTrading, L"bDefensivePactTrading");
	pXML->GetOptionalChildXmlValByName(&m_bPermanentAllianceTrading, L"bPermanentAllianceTrading");
	pXML->GetOptionalChildXmlValByName(&m_bVassalStateTrading, L"bVassalTrading");
	pXML->GetOptionalChildXmlValByName(&m_bBridgeBuilding, L"bBridgeBuilding");
	pXML->GetOptionalChildXmlValByName(&m_bIrrigation, L"bIrrigation");
	pXML->GetOptionalChildXmlValByName(&m_bIgnoreIrrigation, L"bIgnoreIrrigation");
	pXML->GetOptionalChildXmlValByName(&m_bWaterWork, L"bWaterWork");
	pXML->GetOptionalChildXmlValByName(&m_bRiverTrade, L"bRiverTrade");
	pXML->GetOptionalChildXmlValByName(&m_iGridX, L"iGridX");
	pXML->GetOptionalChildXmlValByName(&m_iGridY, L"iGridY");
/************************************************************************************************/
/* DCM									 04/19/09								Johny Smith  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBombTech1, L"bDCMAirBombTech1");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBombTech2, L"bDCMAirBombTech2");
/************************************************************************************************/
/* DCM									 END												  */
/************************************************************************************************/
	if (pXML->TryMoveToXmlFirstChild(L"CommerceFlexible"))
	{
		pXML->SetCommerce(&m_pbCommerceFlexible);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_pbCommerceFlexible);
	}

	pXML->SetVariableListTagPair(&m_piDomainExtraMoves, L"DomainExtraMoves", NUM_DOMAIN_TYPES);
	pXML->SetVariableListTagPair(&m_pbTerrainTrade, L"TerrainTrades", GC.getNumTerrainInfos(), false);
	//ls612: Tech Commerce Modifiers
	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	OutputDebugString("I'm here #1");

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getFlavorTypes(), GC.getNumFlavorTypes());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Quote");
	setQuoteKey(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Sound");
	setSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SoundMP");
	setSoundMP(szTextVal);

/************************************************************************************************/
/* XMLCOPY								 10/13/07								MRGENIE	  */
/*																							  */
/* Need to create these arrays here for the CopyNonDefaults Comparison						  */
/************************************************************************************************/
	//pXML->CvXMLLoadUtility::InitList(&m_piPrereqOrTechs, GC.getNUM_OR_TECH_PREREQS(), -1);
	//pXML->CvXMLLoadUtility::InitList(&m_piPrereqAndTechs, GC.getNUM_AND_TECH_PREREQS(), -1);
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_bEmbassyTrading, L"bEmbassyTrading");
	pXML->GetOptionalChildXmlValByName(&m_bCanPassPeaks, L"bCanPassPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bMoveFastPeaks, L"bMoveFastPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bCanFoundOnPeaks, L"bCanFoundOnPeaks");
	pXML->GetOptionalChildXmlValByName(&m_bEnableDarkAges, L"bEnableDarkAges");
	pXML->GetOptionalChildXmlValByName(&m_bRebaseAnywhere, L"bRebaseAnywhere");
	pXML->GetOptionalChildXmlValByName(&m_bEnablesDesertFarming, L"bAllowsDesertFarming");

	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalTradeModifier, L"iGlobalTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalForeignTradeModifier, L"iGlobalForeignTradeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTradeMissionModifier, L"iTradeMissionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationRevenueModifier, L"iCorporationRevenueModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenanceModifier, L"iCorporationMaintenanceModifier");

	pXML->SetVariableListTagPair(&m_piUnitClassStrengthChange, L"UnitClassStrengthChanges", GC.getNumUnitClassInfos());


	//pXML->CvXMLLoadUtility::InitList(&m_piOriginalPrereqOrTechs, GC.getNUM_OR_TECH_PREREQS(), -1);
	//pXML->CvXMLLoadUtility::InitList(&m_piOriginalPrereqAndTechs, GC.getNUM_AND_TECH_PREREQS(), -1);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, L"FreeSpecialistCounts", GC.getNumSpecialistInfos());


	if(pXML->TryMoveToXmlFirstChild(L"PrereqBuildingClasses"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"PrereqBuildingClass" );
		m_aPrereqBuildingClass.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"PrereqBuildingClass"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingClassType");
					pXML->GetChildXmlValByName(&(m_aPrereqBuildingClass[i].iMinimumRequired), L"iNumBuildingNeeded");
					GC.addDelayedResolution((int*)&(m_aPrereqBuildingClass[i].eBuildingClass), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PrereqBuildingClass"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PrereqOrBuildingClasses"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"PrereqOrBuildingClass" );
		m_aPrereqOrBuildingClass.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"PrereqOrBuildingClass"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingClassType");
					pXML->GetChildXmlValByName(&(m_aPrereqOrBuildingClass[i].iMinimumRequired), L"iNumBuildingNeeded");
					GC.addDelayedResolution((int*)&(m_aPrereqOrBuildingClass[i].eBuildingClass), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PrereqOrBuildingClass"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//if (pXML->TryMoveToXmlFirstChild(L"PrereqBuildingClasses"))
	//{
	//	int iNumChildren = pXML->GetXmlChildrenNumber();
	//	int iTemp = 0;
	//	if (iNumChildren > 0)
	//	{
	//		if (pXML->TryMoveToXmlFirstChild())
	//		{
	//			for (int i=0;i<iNumChildren;i++)
	//			{
	//				if (pXML->GetChildXmlVal(szTextVal))
	//				{
	//					m_aszPrereqBuildingClassforPass3.push_back(szTextVal);
	//					pXML->GetNextXmlVal(&iTemp);
	//					m_aiPrereqBuildingClassforPass3.push_back(iTemp);
	//					pXML->MoveToXmlParent();
	//				}

	//				if (!pXML->TryMoveToXmlNextSibling())
	//				{
	//					break;
	//				}
	//			}

	//			pXML->MoveToXmlParent();
	//		}
	//	}

	//	pXML->MoveToXmlParent();
	//}

	//if (pXML->TryMoveToXmlFirstChild(L"PrereqOrBuildingClasses"))
	//{
	//	int iNumChildren = pXML->GetXmlChildrenNumber();
	//	int iTemp = 0;
	//	if (iNumChildren > 0)
	//	{
	//		if (pXML->TryMoveToXmlFirstChild())
	//		{
	//			for (int i=0;i<iNumChildren;i++)
	//			{
	//				if (pXML->GetChildXmlVal(szTextVal))
	//				{
 //					   m_aszPrereqOrBuildingClassforPass3.push_back(szTextVal);
 //					   pXML->GetNextXmlVal(&iTemp);
 //					   m_aiPrereqOrBuildingClassforPass3.push_back(iTemp);
	//					pXML->MoveToXmlParent();
	//				}

	//				if (!pXML->TryMoveToXmlNextSibling())
	//				{
	//					break;
	//				}
	//			}

	//			pXML->MoveToXmlParent();
	//		}
	//	}

	//}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	//OutputDebugString("I'm here #2");
	//this->getCivilopedia();
	//OutputDebugString("I'm here #3");
	//TB Tech Tags
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	//TB Tech Tags end

	return true;
}

bool CvTechInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;
/************************************************************************************************/
/* XMLCOPY								 10/13/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvString szDebugBuffer;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	if (pXML->TryMoveToXmlFirstChild(L"OrPreReqs"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getNUM_OR_TECH_PREREQS()) ,"Allocating zero or less memory in SetGlobalUnitInfo");

/************************************************************************************************/
/* XMLCOPY								 10/13/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
		if (GC.isXMLLogging())
		{
			szDebugBuffer.Format("OrPrereqs for tech %s: %i entries", getType(), iNumSibs);
			gDLL->logMsg("CvTechInfo_readPass2.log", szDebugBuffer.c_str());
		}

		if (0 < iNumSibs)
		{
			pXML->CvXMLLoadUtility::InitList(&m_piPrereqOrTechs, GC.getNUM_OR_TECH_PREREQS(), -1);
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumSibs <= GC.getNUM_OR_TECH_PREREQS()) ,"There are more siblings than memory allocated for them in SetGlobalUnitInfo");
				for (int j = 0; j < iNumSibs; ++j)
				{
					m_piPrereqOrTechs[j] = GC.getInfoTypeForString(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_piPrereqOrTechs);
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"AndPreReqs"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		FAssertMsg((0 < GC.getNUM_AND_TECH_PREREQS()) ,"Allocating zero or less memory in SetGlobalUnitInfo");
/************************************************************************************************/
/* XMLCOPY								 10/13/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
		if (GC.isXMLLogging())
		{
			szDebugBuffer.Format("AndPrereqs for tech %s: %i entries", getType(), iNumSibs);
			gDLL->logMsg("CvTechInfo_readPass2.log", szDebugBuffer.c_str());
		}

		if (0 < iNumSibs)
		{
			pXML->CvXMLLoadUtility::InitList(&m_piPrereqAndTechs, GC.getNUM_AND_TECH_PREREQS(), -1);
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumSibs <= GC.getNUM_AND_TECH_PREREQS()) ,"There are more siblings than memory allocated for them in SetGlobalUnitInfo");
				for (int j = 0; j < iNumSibs; ++j)
				{
					m_piPrereqAndTechs[j] = GC.getInfoTypeForString(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_piPrereqAndTechs);
		}

		pXML->MoveToXmlParent();
	}

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 10/14/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvTechInfo::copyNonDefaults(CvTechInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (m_iAdvisorType == NO_ADVISOR) m_iAdvisorType = pClassInfo->getAdvisorType();
	if (getAIWeight() == iDefault) m_iAIWeight = pClassInfo->getAIWeight();
	if (getAITradeModifier() == iDefault) m_iAITradeModifier = pClassInfo->getAITradeModifier();
	if (m_iResearchCost == iDefault) m_iResearchCost = pClassInfo->m_iResearchCost;
	if (getAdvancedStartCost() == 100) m_iAdvancedStartCost = pClassInfo->getAdvancedStartCost();
	if (getAdvancedStartCostIncrease() == iDefault) m_iAdvancedStartCostIncrease = pClassInfo->getAdvancedStartCostIncrease();

	if (m_iEra == NO_ERA) m_iEra = pClassInfo->getEra();
	if (m_iFirstFreeUnitClass == NO_UNITCLASS) m_iFirstFreeUnitClass = pClassInfo->getFirstFreeUnitClass();
	if (m_iFirstFreeProphet == NO_UNITCLASS) m_iFirstFreeProphet = pClassInfo->getFirstFreeProphet();

	if (getFeatureProductionModifier() == iDefault) m_iFeatureProductionModifier = pClassInfo->getFeatureProductionModifier();
	if (getWorkerSpeedModifier() == iDefault) m_iWorkerSpeedModifier = pClassInfo->getWorkerSpeedModifier();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getFirstFreeTechs() == iDefault) m_iFirstFreeTechs = pClassInfo->getFirstFreeTechs();
	if (getAssetValue(true) == iDefault) m_iAssetValue = pClassInfo->getAssetValue(true);
	if (getPowerValue(true) == iDefault) m_iPowerValue = pClassInfo->getPowerValue(true);

	if (isRepeat() == bDefault) m_bRepeat = pClassInfo->isRepeat();
	if (isTrade() == bDefault) m_bTrade = pClassInfo->isTrade();
	if (isDisable() == bDefault) m_bDisable = pClassInfo->isDisable();
	if (isGoodyTech() == bDefault) m_bGoodyTech = pClassInfo->isGoodyTech();
	if (isExtraWaterSeeFrom() == bDefault) m_bExtraWaterSeeFrom = pClassInfo->isExtraWaterSeeFrom();
	if (isMapCentering() == bDefault) m_bMapCentering = pClassInfo->isMapCentering();
	if (isMapVisible() == bDefault) m_bMapVisible = pClassInfo->isMapVisible();
	if (isMapTrading() == bDefault) m_bMapTrading = pClassInfo->isMapTrading();
	if (isTechTrading() == bDefault) m_bTechTrading = pClassInfo->isTechTrading();
	if (isGoldTrading() == bDefault) m_bGoldTrading = pClassInfo->isGoldTrading();
	if (isOpenBordersTrading() == bDefault) m_bOpenBordersTrading = pClassInfo->isOpenBordersTrading();
	if (isDefensivePactTrading() == bDefault) m_bDefensivePactTrading = pClassInfo->isDefensivePactTrading();
	if (isPermanentAllianceTrading() == bDefault) m_bPermanentAllianceTrading = pClassInfo->isPermanentAllianceTrading();
	if (isVassalStateTrading() == bDefault) m_bVassalStateTrading = pClassInfo->isVassalStateTrading();
	if (isBridgeBuilding() == bDefault) m_bBridgeBuilding = pClassInfo->isBridgeBuilding();
	if (isIrrigation() == bDefault) m_bIrrigation = pClassInfo->isIrrigation();
	if (isIgnoreIrrigation() == bDefault) m_bIgnoreIrrigation = pClassInfo->isIgnoreIrrigation();
	if (isWaterWork() == bDefault) m_bWaterWork = pClassInfo->isWaterWork();
	if (isRiverTrade() == bDefault) m_bRiverTrade = pClassInfo->isRiverTrade();

	if (getGridX() == iDefault) m_iGridX = pClassInfo->getGridX();
	if (getGridY() == iDefault) m_iGridY = pClassInfo->getGridY();

	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if (isCommerceFlexible(j) == bDefault && pClassInfo->isCommerceFlexible(j) != bDefault)
		{
			if ( NULL == m_pbCommerceFlexible )
			{
				CvXMLLoadUtility::InitList(&m_pbCommerceFlexible,NUM_COMMERCE_TYPES,bDefault);
			}
			m_pbCommerceFlexible[j] = pClassInfo->isCommerceFlexible(j);
		}
	}
	for ( int j = 0; j < NUM_DOMAIN_TYPES; j++)
	{
		if ((m_piDomainExtraMoves == NULL || m_piDomainExtraMoves[j] == iDefault) &&
			pClassInfo->getDomainExtraMoves(j) != iDefault)
		{
			if ( m_piDomainExtraMoves == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piDomainExtraMoves,NUM_DOMAIN_TYPES,iDefault);
			}
			m_piDomainExtraMoves[j] = pClassInfo->getDomainExtraMoves(j);
		}
	}
	for ( int j = 0; j < GC.getNumTerrainInfos(); j++)
	{
		if ((m_pbTerrainTrade == NULL || m_pbTerrainTrade[j] == bDefault) &&
			pClassInfo->isTerrainTrade(j) != bDefault)
		{
			if ( m_pbTerrainTrade == NULL )
			{
				CvXMLLoadUtility::InitList(&m_pbTerrainTrade,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbTerrainTrade[j] = pClassInfo->isTerrainTrade(j);
		}
	}
	//ls612: Tech Commerce Modifiers
	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( getCommerceModifier(j) == iDefault && pClassInfo->getCommerceModifier(j) != iDefault)
		{
			if ( NULL == m_piCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
	}
	for ( int j = 0; j < GC.getNumFlavorTypes(); j++)
	{
		if ((m_piFlavorValue == NULL || m_piFlavorValue[j] == iDefault) &&
			pClassInfo->getFlavorValue(j) != iDefault)
		{
			if ( m_piFlavorValue == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[j] = pClassInfo->getFlavorValue(j);
		}
	}

	if (getQuoteKey() == cDefault) setQuoteKey( pClassInfo->getQuoteKey() );
	if (getSound() == cDefault) setSound( pClassInfo->getSound() );
	if (getSoundMP() == cDefault) setSoundMP( pClassInfo->getSoundMP() );
	//TB Tech Tags
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();
	//TB Tech Tags end

	// Readpass2 stuff
	for ( int j = 0; j < GC.getNUM_OR_TECH_PREREQS(); j++)
	{
		if ( getPrereqOrTechs(j) != pClassInfo->getPrereqOrTechs(j) )
		{
			if ( NULL == m_piPrereqOrTechs )
			{
				CvXMLLoadUtility::InitList(&m_piPrereqOrTechs,GC.getNUM_OR_TECH_PREREQS(),-1);
			}
			m_piPrereqOrTechs[j] = pClassInfo->getPrereqOrTechs(j);
		}
	}
	for ( int j = 0; j < GC.getNUM_AND_TECH_PREREQS(); j++)
	{
		if ( getPrereqAndTechs(j) != pClassInfo->getPrereqAndTechs(j) )
		{
			if ( NULL == m_piPrereqAndTechs )
			{
				CvXMLLoadUtility::InitList(&m_piPrereqAndTechs,GC.getNUM_OR_TECH_PREREQS(),-1);
			}
			m_piPrereqAndTechs[j] = pClassInfo->getPrereqAndTechs(j);
		}
	}
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (!isEmbassyTrading()) m_bEmbassyTrading = pClassInfo->isEmbassyTrading();
	if (!isCanPassPeaks()) m_bCanPassPeaks = pClassInfo->isCanPassPeaks();
	if (!isMoveFastPeaks()) m_bMoveFastPeaks = pClassInfo->isMoveFastPeaks();
	if (!isCanFoundOnPeaks()) m_bCanFoundOnPeaks = pClassInfo->isCanFoundOnPeaks();
	if (!isEnableDarkAges()) m_bEnableDarkAges = pClassInfo->isEnableDarkAges();
	if (!isRebaseAnywhere()) m_bRebaseAnywhere = pClassInfo->isRebaseAnywhere();
	if (!isEnablesDesertFarming()) m_bEnablesDesertFarming = pClassInfo->isEnablesDesertFarming();

	if (m_iInflationModifier == 0) m_iInflationModifier = pClassInfo->getInflationModifier();
	if (m_iGlobalTradeModifier == 0) m_iGlobalTradeModifier = pClassInfo->getGlobalTradeModifier();
	if (m_iGlobalForeignTradeModifier == 0) m_iGlobalForeignTradeModifier = pClassInfo->getGlobalForeignTradeModifier();
	if (m_iTradeMissionModifier == 0) m_iTradeMissionModifier = pClassInfo->getTradeMissionModifier();
	if (m_iCorporationRevenueModifier == 0) m_iCorporationRevenueModifier = pClassInfo->getCorporationRevenueModifier();
	if (m_iCorporationMaintenanceModifier == 0) m_iCorporationMaintenanceModifier = pClassInfo->getCorporationMaintenanceModifier();
	if (m_iPrereqGameOption == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();

	if (!m_iMaintenanceModifier) m_iMaintenanceModifier = pClassInfo->getMaintenanceModifier();
	if (!m_iDistanceMaintenanceModifier) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (!m_iNumCitiesMaintenanceModifier) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (!m_iCoastalDistanceMaintenanceModifier) m_iCoastalDistanceMaintenanceModifier = pClassInfo->getCoastalDistanceMaintenanceModifier();

	for ( int j = 0; j < GC.getNumSpecialistInfos(); j++)
	{
		if ((m_piFreeSpecialistCount == NULL ||m_piFreeSpecialistCount[j] == 0) &&
			pClassInfo->getFreeSpecialistCount(j) != 0)
		{
			if ( m_piFreeSpecialistCount == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piFreeSpecialistCount,GC.getNumSpecialistInfos(),0);
			}
			m_piFreeSpecialistCount[j] = pClassInfo->getFreeSpecialistCount(j);
		}
	}

	for ( int j = 0; j < GC.getNumUnitClassInfos(); j++)
	{
		if ((m_piUnitClassStrengthChange == NULL || m_piUnitClassStrengthChange[j] == 0) &&
			pClassInfo->getUnitClassStrengthChange(j, true) != 0)
		{
			if ( m_piUnitClassStrengthChange == NULL )
			{
				CvXMLLoadUtility::InitList(&m_piUnitClassStrengthChange,GC.getNumUnitClassInfos(),0);
			}
			m_piUnitClassStrengthChange[j] = pClassInfo->getUnitClassStrengthChange(j, true);
		}
	}

	if (getNumPrereqBuildingClasses() == 0)
	{
		int iNum = pClassInfo->getNumPrereqBuildingClasses();
		m_aPrereqBuildingClass.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPrereqBuildingClass[i].iMinimumRequired = pClassInfo->m_aPrereqBuildingClass[i].iMinimumRequired;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPrereqBuildingClass[i].eBuildingClass), (int*)&(pClassInfo->m_aPrereqBuildingClass[i].eBuildingClass));
	}
	}

	if (getNumPrereqOrBuildingClasses() == 0)
	{
		int iNum = pClassInfo->getNumPrereqOrBuildingClasses();
		m_aPrereqOrBuildingClass.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPrereqOrBuildingClass[i].iMinimumRequired = pClassInfo->m_aPrereqOrBuildingClass[i].iMinimumRequired;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPrereqOrBuildingClass[i].eBuildingClass), (int*)&(pClassInfo->m_aPrereqOrBuildingClass[i].eBuildingClass));
	}
	}
	//for ( int i = 0; i < pClassInfo->getPrereqBuildingVectorSize(); i++ )
	//{
	//	m_aiPrereqBuildingClassforPass3.push_back(pClassInfo->getPrereqBuildingClassValuesVectorElement(i));
	//	m_aszPrereqBuildingClassforPass3.push_back(pClassInfo->getPrereqBuildingNamesVectorElement(i));
	//}
	//for ( int i = 0; i < pClassInfo->getPrereqBuildingVectorSize(); i++ )
	//{
	//	m_aiPrereqOrBuildingClassforPass3.push_back(pClassInfo->getPrereqOrBuildingClassValuesVectorElement(i));
	//	m_aszPrereqOrBuildingClassforPass3.push_back(pClassInfo->getPrereqOrBuildingNamesVectorElement(i));
	//}


/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
}

void CvTechInfo::copyNonDefaultsReadPass2(CvTechInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
	int iTextDefault = -1;
	bool bOverride = false;

	// If the modder has something here, we assume he wants to override the tech definitions
	for ( int i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++ )
	{
		if (bOver || pClassInfo->getPrereqOrTechs(i) != iTextDefault )
		{
			bOverride = true;
			break;
		}
	}
	if ( bOverride)
	{
		if ( NULL == m_piPrereqOrTechs )
		{
			m_piPrereqOrTechs = new int[GC.getNUM_OR_TECH_PREREQS()];
		}
		for ( int i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++ )
		{
			m_piPrereqOrTechs[i] = pClassInfo->getPrereqOrTechs(i);
		}
	}

	bOverride = false;
	// If the modder has something here, we assume he wants to override the tech definitions
	for ( int i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++ )
	{
		if (bOver || pClassInfo->getPrereqAndTechs(i) != iTextDefault )
		{
			bOverride = true;
			break;
		}
	}
	if ( bOverride)
	{
		if ( NULL == m_piPrereqAndTechs )
		{
			m_piPrereqAndTechs = new int[GC.getNUM_AND_TECH_PREREQS()];
		}
		for ( int i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++ )
		{
			m_piPrereqAndTechs[i] = pClassInfo->getPrereqAndTechs(i);
		}
	}
}
/************************************************************************************************/
/* Afforess					  Start		 04/01/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvTechInfo::readPass3()
{
	//m_paiPrereqBuildingClass = new int[GC.getNumBuildingClassInfos()];
	//m_paiPrereqOrBuildingClass = new int[GC.getNumBuildingClassInfos()];
	//for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	//{
	//	m_paiPrereqBuildingClass[iI] = 0;
	//	m_paiPrereqOrBuildingClass[iI] = 0;
	//}
	//if (!m_aiPrereqBuildingClassforPass3.empty() && !m_aszPrereqBuildingClassforPass3.empty())
	//{
	//	int iNumLoad = m_aiPrereqBuildingClassforPass3.size();
	//	for(int iI = 0; iI < iNumLoad; iI++)
	//	{
	//		int iTempIndex = GC.getInfoTypeForString(m_aszPrereqBuildingClassforPass3[iI]);
	//		if (iTempIndex >= 0 && iTempIndex < GC.getNumBuildingClassInfos())
	//			m_paiPrereqBuildingClass[iTempIndex] = m_aiPrereqBuildingClassforPass3[iI];
	//	}
	//	m_aszPrereqBuildingClassforPass3.clear();
	//	m_aiPrereqBuildingClassforPass3.clear();
	//}
	//if (!m_aiPrereqOrBuildingClassforPass3.empty() && !m_aszPrereqOrBuildingClassforPass3.empty())
	//{
	//	int iNumLoad = m_aiPrereqOrBuildingClassforPass3.size();
	//	for(int iI = 0; iI < iNumLoad; iI++)
	//	{
	//		int iTempIndex = GC.getInfoTypeForString(m_aszPrereqOrBuildingClassforPass3[iI]);
	//		if (iTempIndex >= 0 && iTempIndex < GC.getNumBuildingClassInfos())
	//			m_paiPrereqOrBuildingClass[iTempIndex] = m_aiPrereqOrBuildingClassforPass3[iI];
	//	}
	//	m_aszPrereqOrBuildingClassforPass3.clear();
	//	m_aiPrereqOrBuildingClassforPass3.clear();
	//}
	return true;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

void CvTechInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iAdvisorType);
	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_iAITradeModifier);
	CheckSum(iSum, m_iResearchCost);
	CheckSum(iSum, m_iAdvancedStartCost);
	CheckSum(iSum, m_iAdvancedStartCostIncrease);
	CheckSum(iSum, m_iEra);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iFeatureProductionModifier);
	CheckSum(iSum, m_iWorkerSpeedModifier);
	CheckSum(iSum, m_iMaintenanceModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iCoastalDistanceMaintenanceModifier);
	CheckSum(iSum, m_iFirstFreeUnitClass);
	CheckSum(iSum, m_iFirstFreeProphet);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iFirstFreeTechs);
	CheckSum(iSum, m_iAssetValue);
	CheckSum(iSum, m_iPowerValue);

	CheckSum(iSum, m_bRepeat);
	CheckSum(iSum, m_bTrade);
	CheckSum(iSum, m_bDisable);
	CheckSum(iSum, m_bGoodyTech);
	CheckSum(iSum, m_bExtraWaterSeeFrom);
	CheckSum(iSum, m_bMapCentering);
	CheckSum(iSum, m_bMapVisible);
	CheckSum(iSum, m_bMapTrading);
	CheckSum(iSum, m_bTechTrading);
	CheckSum(iSum, m_bGoldTrading);
	CheckSum(iSum, m_bOpenBordersTrading);
	CheckSum(iSum, m_bDefensivePactTrading);
	CheckSum(iSum, m_bPermanentAllianceTrading);
	CheckSum(iSum, m_bVassalStateTrading);
	CheckSum(iSum, m_bBridgeBuilding);
	CheckSum(iSum, m_bIrrigation);
	CheckSum(iSum, m_bIgnoreIrrigation);
	CheckSum(iSum, m_bWaterWork);
	CheckSum(iSum, m_bRiverTrade);

	CheckSum(iSum, m_piDomainExtraMoves, NUM_DOMAIN_TYPES);
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());
	CheckSum(iSum, m_piPrereqOrTechs, GC.getNUM_OR_TECH_PREREQS());
	CheckSum(iSum, m_piPrereqAndTechs, GC.getNUM_AND_TECH_PREREQS());
	CheckSum(iSum, m_pbCommerceFlexible, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_pbTerrainTrade, GC.getNumTerrainInfos());
	//ls612: Tech Commerce Modifiers
	CheckSum(iSum, m_piCommerceModifier, NUM_COMMERCE_TYPES);

	CheckSum(iSum, m_bCanPassPeaks);
	CheckSum(iSum, m_bMoveFastPeaks);
	CheckSum(iSum, m_bCanFoundOnPeaks);
	CheckSum(iSum, m_bEmbassyTrading);
	CheckSum(iSum, m_bEnableDarkAges);
	CheckSum(iSum, m_bRebaseAnywhere);
	CheckSum(iSum, m_bEnablesDesertFarming);

	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_iGlobalTradeModifier);
	CheckSum(iSum, m_iGlobalForeignTradeModifier);
	CheckSum(iSum, m_iTradeMissionModifier);
	CheckSum(iSum, m_iCorporationRevenueModifier);
	CheckSum(iSum, m_iCorporationMaintenanceModifier);
	CheckSum(iSum, m_iPrereqGameOption);

	CheckSum(iSum, m_piFreeSpecialistCount, GC.getNumSpecialistInfos());
	CheckSum(iSum, m_piOriginalPrereqOrTechs, GC.getNUM_OR_TECH_PREREQS());
	CheckSum(iSum, m_piOriginalPrereqAndTechs, GC.getNUM_AND_TECH_PREREQS());
	CheckSum(iSum, m_piUnitClassStrengthChange, GC.getNumUnitClassInfos());
	//CheckSum(iSum, m_paiPrereqBuildingClass, GC.getNumBuildingClassInfos());
	//CheckSum(iSum, m_paiPrereqOrBuildingClass, GC.getNumBuildingClassInfos());

	int iNumElements = m_aPrereqBuildingClass.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPrereqBuildingClass[i].eBuildingClass);
		CheckSum(iSum, m_aPrereqBuildingClass[i].iMinimumRequired);
	}
	//TB Tech Tags
	CheckSum(iSum, m_bGlobal);
	//TB Tech Tags end

}

//======================================================================================================
//					CvCivicOptionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivicOptionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivicOptionInfo::CvCivicOptionInfo() :
	m_bPolicy(false),
	m_pabTraitNoUpkeep(NULL)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivicOptionInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivicOptionInfo::~CvCivicOptionInfo()
{
	//SAFE_DELETE_ARRAY(m_pabTraitNoUpkeep);
}

bool CvCivicOptionInfo::isPolicy() const
{
	return m_bPolicy;
}
//bool CvCivicOptionInfo::getTraitNoUpkeep(int i) const
//{
//	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
//	FAssertMsg(i > -1, "Index out of bounds");
//	return m_pabTraitNoUpkeep ? m_pabTraitNoUpkeep[i] : false;
//}

bool CvCivicOptionInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(&m_bPolicy, L"bPolicy");
	//pXML->SetVariableListTagPair(&m_pabTraitNoUpkeep, L"TraitNoUpkeeps", GC.getNumTraitInfos());

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 10/22/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvCivicOptionInfo::copyNonDefaults(CvCivicOptionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
//	int iDefault = 0;
//	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
//	int iAudioDefault = -1;  //all audio is default -1
//	float fDefault = 0.0f;
//	CvString cDefault = CvString::format("").GetCString();
//	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (isPolicy() == bDefault) m_bPolicy = pClassInfo->isPolicy();
	//for ( int i = 0; i < GC.getNumTraitInfos(); i++)
	//{
	//	if ( getTraitNoUpkeep(i) == bDefault && pClassInfo->getTraitNoUpkeep(i) != bDefault)
	//	{
	//		if ( NULL == m_pabTraitNoUpkeep )
	//		{
	//			CvXMLLoadUtility::InitList(&m_pabTraitNoUpkeep,GC.getNumTraitInfos(),bDefault);
	//		}
	//		m_pabTraitNoUpkeep[i] = pClassInfo->getTraitNoUpkeep(i);
	//	}
	//}
}

//======================================================================================================
//					CvCivicInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivicInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivicInfo::CvCivicInfo() :
m_iCivicOptionType(NO_CIVICOPTION),
m_iAnarchyLength(0),
m_iUpkeep(NO_UPKEEP),
m_iAIWeight(0),
m_iGreatPeopleRateModifier(0),
m_iGreatGeneralRateModifier(0),
m_iDomesticGreatGeneralRateModifier(0),
m_iStateReligionGreatPeopleRateModifier(0),
m_iDistanceMaintenanceModifier(0),
m_iNumCitiesMaintenanceModifier(0),
//DPII < Maintenance Modifiers >
m_iHomeAreaMaintenanceModifier(0),
m_iOtherAreaMaintenanceModifier(0),
//DPII < Maintenance Modifiers >
m_iCorporationMaintenanceModifier(0),
m_iExtraHealth(0),
m_iFreeExperience(0),
m_iWorkerSpeedModifier(0),
m_iImprovementUpgradeRateModifier(0),
m_iMilitaryProductionModifier(0),
m_iBaseFreeUnits(0),
m_iBaseFreeMilitaryUnits(0),
m_iFreeUnitsPopulationPercent(0),
m_iFreeMilitaryUnitsPopulationPercent(0),
m_iGoldPerUnit(0),
m_iGoldPerMilitaryUnit(0),
m_iHappyPerMilitaryUnit(0),
m_iLargestCityHappiness(0),
m_iWarWearinessModifier(0),
m_iFreeSpecialist(0),
m_iTradeRoutes(0),
m_iTechPrereq(NO_TECH),
m_iCivicPercentAnger(0),
m_iMaxConscript(0),
m_iStateReligionHappiness(0),
m_iNonStateReligionHappiness(0),
m_iStateReligionUnitProductionModifier(0),
m_iStateReligionBuildingProductionModifier(0),
m_iStateReligionFreeExperience(0),
m_iExpInBorderModifier(0),
/************************************************************************************************/
/* REVOLUTION_MOD								 01/01/08						DPII		  */
/*																							  */
/* RevCivic Effects																			 */
/************************************************************************************************/
m_bUpgradeAnywhere(false),
m_bAllowInquisitions(false),
m_bDisallowInquisitions(false),
m_iRevIdxLocal(0),
m_iRevIdxNational(0),
m_iRevIdxDistanceModifier(0),
m_iRevIdxHolyCityGood(0),
m_iRevIdxHolyCityBad(0),
m_iRevIdxSwitchTo(0),
m_fRevIdxNationalityMod(0),
m_fRevIdxGoodReligionMod(0),
m_fRevIdxBadReligionMod(0),
m_fRevViolentMod(0),
m_iRevReligiousFreedom(0),
m_iRevLaborFreedom(0),
m_iRevEnvironmentalProtection(0),
m_iRevDemocracyLevel(0),
m_bCommunism(false),
m_bFreeSpeech(false),
m_bCanDoElection(false),
/************************************************************************************************/
/* REVOLUTION_MOD						  END												  */
/************************************************************************************************/
m_bMilitaryFoodProduction(false),
m_bNoUnhealthyPopulation(false),
m_bBuildingOnlyHealthy(false),
m_bNoForeignTrade(false),
m_bNoCorporations(false),
m_bNoForeignCorporations(false),
m_bStateReligion(false),
m_bNoNonStateReligionSpread(false),
m_piYieldModifier(NULL),
m_piCapitalYieldModifier(NULL),
m_piTradeYieldModifier(NULL),
m_piCommerceModifier(NULL),
m_piCapitalCommerceModifier(NULL),
m_piSpecialistExtraCommerce(NULL),
m_paiBuildingHappinessChanges(NULL),
m_paiBuildingHealthChanges(NULL),
m_paiFeatureHappinessChanges(NULL),
m_pabHurry(NULL),
m_pabSpecialBuildingNotRequired(NULL),
m_pabSpecialistValid(NULL),
/********************************************************************************/
/* 	New Civic AI						02.08.2010				Fuyu			*/
/********************************************************************************/
m_ppiImprovementYieldChanges(NULL),
m_bAnyImprovementYieldChange(false)
/********************************************************************************/
/* 	New Civic AI												END 			*/
/********************************************************************************/

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iAttitudeShareMod(0)
,m_iEnslavementChance(0)
,m_iPopulationgrowthratepercentage(0)
,m_iReligionSpreadRate(0)
,m_iCivicHappiness(0)
,m_iDistantUnitSupportCostModifier(0)
,m_iExtraCityDefense(0)
,m_iForeignTradeRouteModifier(0)
,m_iTaxRateUnhappiness(0)
,m_iInflationModifier(0)
,m_iHurryInflationModifier(0)
,m_iHurryCostModifier(0)
,m_iSharedCivicTradeRouteModifier(0)
,m_iLandmarkHappiness(0)
,m_iCorporationSpreadRate(0)
,m_iRealCorporationMaintenanceModifier(0)
,m_iForeignerUnhappyPercent(0)
,m_iCityLimit(0)
,m_iCityOverLimitUnhappy(0)
,m_bFixedBorders(false)
,m_bNoCapitalUnhappiness(false)
,m_bTaxationAnger(false)
,m_bNoLandmarkAnger(false)
,m_bAnySpecialistYieldChanges(false)
,m_bAnySpecialistCommerceChanges(false)
,m_bEnablesMAD(false)
,m_piBonusMintedPercent(NULL)
,m_piImprovementHappinessChanges(NULL)
,m_piImprovementHealthPercentChanges(NULL)
,m_ppiSpecialistYieldPercentChanges(NULL)
,m_ppiSpecialistCommercePercentChanges(NULL)
,m_piLandmarkYieldChanges(NULL)
,m_piFreeSpecialistCount(NULL)
,m_paiUnitCombatProductionModifier(NULL)
,m_paiBuildingClassProductionModifier(NULL)
,m_piUnitClassProductionModifier(NULL)
,m_ppiTerrainYieldChanges(NULL)
,m_piFlavorValue(NULL)
,m_piCivicAttitudeChanges(NULL)
,m_pszCivicAttitudeReason(NULL)
,m_ppiBuildingCommerceModifier(NULL)
,m_ppiBuildingClassCommerceChange(NULL)
,m_ppiBonusCommerceModifier(NULL)
,m_iNationalCaptureProbabilityModifier(0)
,m_iNationalCaptureResistanceModifier(0)
,m_iFreedomFighterChange(0)
,m_bAllReligionsActive(false)
,m_bBansNonStateReligions(false)
,m_bFreedomFighter(false)
,m_bPolicy(false)
,m_PropertyManipulators()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivicInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivicInfo::~CvCivicInfo()
{
	int iI;

	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_piTradeYieldModifier);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	SAFE_DELETE_ARRAY(m_piCapitalCommerceModifier);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_paiBuildingHappinessChanges);
	SAFE_DELETE_ARRAY(m_paiBuildingHealthChanges);
	SAFE_DELETE_ARRAY(m_paiFeatureHappinessChanges);
	SAFE_DELETE_ARRAY(m_pabHurry);
	SAFE_DELETE_ARRAY(m_pabSpecialBuildingNotRequired);
	SAFE_DELETE_ARRAY(m_pabSpecialistValid);
	if (m_ppiImprovementYieldChanges != NULL)
	{
		for (iI=0;iI<GC.getNumImprovementInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppiImprovementYieldChanges[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppiImprovementYieldChanges);
	}
/************************************************************************************************/
/* Afforess					  Start		 Last Update: 3/8/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piBonusMintedPercent);
	SAFE_DELETE_ARRAY(m_piCivicAttitudeChanges);
	SAFE_DELETE_ARRAY(m_pszCivicAttitudeReason);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifier);
	SAFE_DELETE_ARRAY(m_paiBuildingClassProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitClassProductionModifier);
	SAFE_DELETE_ARRAY(m_piLandmarkYieldChanges);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piImprovementHappinessChanges);
	SAFE_DELETE_ARRAY(m_piImprovementHealthPercentChanges);
	if (m_ppiBuildingCommerceModifier != NULL)
	{
		for (iI=0;iI<GC.getNumBuildingInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppiBuildingCommerceModifier[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppiBuildingCommerceModifier);
	}
	if (m_ppiBuildingClassCommerceChange != NULL)
		{
			for (iI=0;iI<GC.getNumBuildingClassInfos();iI++)
			{
				SAFE_DELETE_ARRAY(m_ppiBuildingClassCommerceChange[iI]);
			}
			SAFE_DELETE_ARRAY(m_ppiBuildingClassCommerceChange);
		}
	if (m_ppiBonusCommerceModifier != NULL)
	{
		for (iI=0;iI<GC.getNumBonusInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppiBonusCommerceModifier[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppiBonusCommerceModifier);
	}
	if (m_ppiTerrainYieldChanges != NULL)
	{
		for (iI=0;iI<GC.getNumTerrainInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppiTerrainYieldChanges[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppiTerrainYieldChanges);
	}
	if (m_ppiSpecialistYieldPercentChanges != NULL)
	{
		for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppiSpecialistYieldPercentChanges[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppiSpecialistYieldPercentChanges);
	}
	if (m_ppiSpecialistCommercePercentChanges != NULL)
	{
		for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppiSpecialistCommercePercentChanges[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppiSpecialistCommercePercentChanges);
	}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
}

const wchar* CvCivicInfo::getWeLoveTheKing()
{
	return m_szWeLoveTheKingKey;
}

void CvCivicInfo::setWeLoveTheKingKey(const TCHAR* szVal)
{
	m_szWeLoveTheKingKey = szVal;
}

const wchar* CvCivicInfo::getWeLoveTheKingKey()
{
	return m_szWeLoveTheKingKey;
}

int CvCivicInfo::getCivicOptionType() const
{
	return m_iCivicOptionType;
}

int CvCivicInfo::getAnarchyLength() const
{
	return m_iAnarchyLength;
}

int CvCivicInfo::getUpkeep() const
{
	return m_iUpkeep;
}

int CvCivicInfo::getAIWeight() const
{
	return m_iAIWeight;
}

int CvCivicInfo::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}

int CvCivicInfo::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}

int CvCivicInfo::getDomesticGreatGeneralRateModifier() const
{
	return m_iDomesticGreatGeneralRateModifier;
}

int CvCivicInfo::getStateReligionGreatPeopleRateModifier() const
{
	return m_iStateReligionGreatPeopleRateModifier;
}

int CvCivicInfo::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}

int CvCivicInfo::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;
}
//DPII < Maintenance Modifiers >
int CvCivicInfo::getHomeAreaMaintenanceModifier() const
{
	return m_iHomeAreaMaintenanceModifier;
}

int CvCivicInfo::getOtherAreaMaintenanceModifier() const
{
	return m_iOtherAreaMaintenanceModifier;
}
//DPII < Maintenance Modifiers >
int CvCivicInfo::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}

int CvCivicInfo::getExtraHealth() const
{
	return m_iExtraHealth;
}

int CvCivicInfo::getFreeExperience() const
{
	return m_iFreeExperience;
}

int CvCivicInfo::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}

int CvCivicInfo::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}

int CvCivicInfo::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}

int CvCivicInfo::getBaseFreeUnits() const
{
	return m_iBaseFreeUnits;
}

int CvCivicInfo::getBaseFreeMilitaryUnits() const
{
	return m_iBaseFreeMilitaryUnits;
}

int CvCivicInfo::getFreeUnitsPopulationPercent() const
{
	return m_iFreeUnitsPopulationPercent;
}

int CvCivicInfo::getFreeMilitaryUnitsPopulationPercent() const
{
	return m_iFreeMilitaryUnitsPopulationPercent;
}

int CvCivicInfo::getGoldPerUnit() const
{
	return m_iGoldPerUnit;
}

int CvCivicInfo::getGoldPerMilitaryUnit() const
{
	return m_iGoldPerMilitaryUnit;
}

int CvCivicInfo::getHappyPerMilitaryUnit() const
{
	return m_iHappyPerMilitaryUnit;
}

int CvCivicInfo::getLargestCityHappiness() const
{
	return m_iLargestCityHappiness;
}

int CvCivicInfo::getWarWearinessModifier() const
{
	return m_iWarWearinessModifier;
}

int CvCivicInfo::getFreeSpecialist() const
{
	return m_iFreeSpecialist;
}

int CvCivicInfo::getTradeRoutes() const
{
	return m_iTradeRoutes;
}

int CvCivicInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvCivicInfo::getCivicPercentAnger() const
{
	return m_iCivicPercentAnger;
}

int CvCivicInfo::getMaxConscript() const
{
	return m_iMaxConscript;
}

int CvCivicInfo::getStateReligionHappiness() const
{
	return m_iStateReligionHappiness;
}

int CvCivicInfo::getNonStateReligionHappiness() const
{
	return m_iNonStateReligionHappiness;
}

int CvCivicInfo::getStateReligionUnitProductionModifier() const
{
	return m_iStateReligionUnitProductionModifier;
}

int CvCivicInfo::getStateReligionBuildingProductionModifier() const
{
	return m_iStateReligionBuildingProductionModifier;
}

int CvCivicInfo::getStateReligionFreeExperience() const
{
	return m_iStateReligionFreeExperience;
}

int CvCivicInfo::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}

int CvCivicInfo::getRevIdxLocal() const
{
	return m_iRevIdxLocal;
}

int CvCivicInfo::getRevIdxNational() const
{
	return m_iRevIdxNational;
}

int CvCivicInfo::getRevIdxDistanceModifier() const
{
	return m_iRevIdxDistanceModifier;
}

int CvCivicInfo::getRevIdxHolyCityGood() const
{
	return m_iRevIdxHolyCityGood;
}

int CvCivicInfo::getRevIdxHolyCityBad() const
{
	return m_iRevIdxHolyCityBad;
}

int CvCivicInfo::getRevIdxSwitchTo() const
{
	return m_iRevIdxSwitchTo;
}

int CvCivicInfo::getRevReligiousFreedom() const
{
	return m_iRevReligiousFreedom;
}

int CvCivicInfo::getRevLaborFreedom() const
{
	return m_iRevLaborFreedom;
}

int CvCivicInfo::getRevEnvironmentalProtection() const
{
	return m_iRevEnvironmentalProtection;
}

int CvCivicInfo::getRevDemocracyLevel() const
{
	return m_iRevDemocracyLevel;
}

int CvCivicInfo::getAttitudeShareMod() const
{
	return m_iAttitudeShareMod;
}

int CvCivicInfo::getEnslavementChance() const
{
	return m_iEnslavementChance;
}

int CvCivicInfo::getForeignerUnhappyPercent() const
{
	return m_iForeignerUnhappyPercent;
}

int CvCivicInfo::getCityOverLimitUnhappy() const
{
	return m_iCityOverLimitUnhappy;
}

int CvCivicInfo::getLandmarkHappiness() const
{
	return m_iLandmarkHappiness;
}

int CvCivicInfo::getForeignTradeRouteModifier() const
{
	return m_iForeignTradeRouteModifier;
}

int CvCivicInfo::getTaxRateUnhappiness() const
{
	return m_iTaxRateUnhappiness;
}

int CvCivicInfo::getPopulationgrowthratepercentage() const
{
	return m_iPopulationgrowthratepercentage;
}

int CvCivicInfo::getReligionSpreadRate() const
{
	return m_iReligionSpreadRate;
}

int CvCivicInfo::getCivicHappiness() const
{
	return m_iCivicHappiness;
}

int CvCivicInfo::getDistantUnitSupportCostModifier() const
{
	return m_iDistantUnitSupportCostModifier;
}

int CvCivicInfo::getExtraCityDefense() const
{
	return m_iExtraCityDefense;
}

int CvCivicInfo::getNationalCaptureProbabilityModifier() const
{
	return m_iNationalCaptureProbabilityModifier;
}

int CvCivicInfo::getNationalCaptureResistanceModifier() const
{
	return m_iNationalCaptureResistanceModifier;
}

int CvCivicInfo::getInflationModifier() const
{
	return m_iInflationModifier;
}

int CvCivicInfo::getHurryInflationModifier() const
{
	return m_iHurryInflationModifier;
}

int CvCivicInfo::getHurryCostModifier() const
{
	return m_iHurryCostModifier;
}

int CvCivicInfo::getSharedCivicTradeRouteModifier() const
{
	return m_iSharedCivicTradeRouteModifier;
}

int CvCivicInfo::getCorporationSpreadRate() const
{
	return m_iCorporationSpreadRate;
}

int CvCivicInfo::getRealCorporationMaintenanceModifier() const
{
	return m_iRealCorporationMaintenanceModifier;
}

int CvCivicInfo::getFreedomFighterChange() const
{
	return m_iFreedomFighterChange;
}


float CvCivicInfo::getRevIdxNationalityMod() const
{
	return m_fRevIdxNationalityMod;
}

float CvCivicInfo::getRevIdxBadReligionMod() const
{
	return m_fRevIdxBadReligionMod;
}

float CvCivicInfo::getRevIdxGoodReligionMod() const
{
	return m_fRevIdxGoodReligionMod;
}

float CvCivicInfo::getRevViolentMod() const
{
	return m_fRevViolentMod;
}


bool CvCivicInfo::isUpgradeAnywhere() const
{
	return m_bUpgradeAnywhere;
}

bool CvCivicInfo::isAllowInquisitions() const
{
	return m_bAllowInquisitions;
}

bool CvCivicInfo::isDisallowInquisitions() const
{
	return m_bDisallowInquisitions;
}

bool CvCivicInfo::isCommunism() const
{
	return m_bCommunism;
}

bool CvCivicInfo::isFreeSpeech() const
{
	return m_bFreeSpeech;
}

bool CvCivicInfo::isCanDoElection() const
{
	return m_bCanDoElection;
}

bool CvCivicInfo::isMilitaryFoodProduction() const
{
	return m_bMilitaryFoodProduction;
}

bool CvCivicInfo::isNoUnhealthyPopulation() const
{
	return m_bNoUnhealthyPopulation;
}

bool CvCivicInfo::isBuildingOnlyHealthy() const
{
	return m_bBuildingOnlyHealthy;
}

bool CvCivicInfo::isNoForeignTrade() const
{
	return m_bNoForeignTrade;
}

bool CvCivicInfo::isNoCorporations() const
{
	return m_bNoCorporations;
}

bool CvCivicInfo::isNoForeignCorporations() const
{
	return m_bNoForeignCorporations;
}

bool CvCivicInfo::isStateReligion() const
{
	return m_bStateReligion;
}

bool CvCivicInfo::isNoNonStateReligionSpread() const
{
	return m_bNoNonStateReligionSpread;
}

bool CvCivicInfo::IsFixedBorders() const
{
	return m_bFixedBorders;
}

bool CvCivicInfo::isNoCapitalUnhappiness() const
{
	return m_bNoCapitalUnhappiness;
}

bool CvCivicInfo::isTaxationAnger() const
{
	return m_bTaxationAnger;
}

bool CvCivicInfo::isNoLandmarkAnger() const
{
	return m_bNoLandmarkAnger;
}

bool CvCivicInfo::isEnablesMAD() const
{
	return m_bEnablesMAD;
}

bool CvCivicInfo::isAllReligionsActive() const
{
	return m_bAllReligionsActive;
}

bool CvCivicInfo::isBansNonStateReligions() const
{
	return m_bBansNonStateReligions;
}

bool CvCivicInfo::isFreedomFighter() const
{
	return m_bFreedomFighter;
}

bool CvCivicInfo::isPolicy() const
{
	return m_bPolicy;
}


// Arrays
int CvCivicInfo::getYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldModifier ? m_piYieldModifier[i] : 0;
}

int* CvCivicInfo::getYieldModifierArray() const
{
	return m_piYieldModifier;
}

int CvCivicInfo::getCapitalYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
}

int* CvCivicInfo::getCapitalYieldModifierArray() const
{
	return m_piCapitalYieldModifier;
}

int CvCivicInfo::getTradeYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piTradeYieldModifier ? m_piTradeYieldModifier[i] : 0;
}

int* CvCivicInfo::getTradeYieldModifierArray() const
{
	return m_piTradeYieldModifier;
}

int CvCivicInfo::getCommerceModifier(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int* CvCivicInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}

int CvCivicInfo::getCapitalCommerceModifier(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
}

int* CvCivicInfo::getCapitalCommerceModifierArray() const
{
	return m_piCapitalCommerceModifier;
}

int CvCivicInfo::getSpecialistExtraCommerce(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
}

int* CvCivicInfo::getSpecialistExtraCommerceArray() const
{
	return m_piSpecialistExtraCommerce;
}

int CvCivicInfo::getCivicAttitudeChange(int i) const
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCivicAttitudeChanges ? m_piCivicAttitudeChanges[i] : 0;
}

int* CvCivicInfo::getCivicAttitudeChanges() const
{
	return m_piCivicAttitudeChanges;
}

int CvCivicInfo::getLandmarkYieldChanges(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piLandmarkYieldChanges ? m_piLandmarkYieldChanges[i] : 0;
}

int* CvCivicInfo::getLandmarkYieldChangesArray() const
{
	return m_piLandmarkYieldChanges;
}


int CvCivicInfo::getBonusCommerceModifier(int i, int j) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiBonusCommerceModifier && m_ppiBonusCommerceModifier[i]) ? m_ppiBonusCommerceModifier[i][j] : 0;
}

int* CvCivicInfo::getBonusCommerceModifierArray(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_ppiBonusCommerceModifier ? m_ppiBonusCommerceModifier[i] : NULL;
}


int CvCivicInfo::getBuildingHappinessChanges(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingHappinessChanges ? m_paiBuildingHappinessChanges[i] : 0;
}

int CvCivicInfo::getBuildingHealthChanges(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingHealthChanges ? m_paiBuildingHealthChanges[i] : 0;
}

int CvCivicInfo::getFeatureHappinessChanges(int i) const
{
	FAssertMsg(i < GC.getNumFeatureInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiFeatureHappinessChanges ? m_paiFeatureHappinessChanges[i] : 0;
}

int CvCivicInfo::getBonusMintedPercent(int i) const
{
	FAssertMsg(i < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piBonusMintedPercent ? m_piBonusMintedPercent[i] : 0;
}

int CvCivicInfo::getFreeSpecialistCount(int i) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}

int CvCivicInfo::getFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvCivicInfo::getUnitCombatProductionModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitCombatInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiUnitCombatProductionModifier ? m_paiUnitCombatProductionModifier[i] : 0;
}

int CvCivicInfo::getUnitClassProductionModifier(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitClassProductionModifier ? m_piUnitClassProductionModifier[i] : 0;
}

int CvCivicInfo::getBuildingClassProductionModifier(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiBuildingClassProductionModifier ? m_paiBuildingClassProductionModifier[i] : 0;
}

int CvCivicInfo::getImprovementHappinessChanges(int i) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piImprovementHappinessChanges ? m_piImprovementHappinessChanges[i] : 0;
}

int CvCivicInfo::getImprovementHealthPercentChanges(int i) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piImprovementHealthPercentChanges ? m_piImprovementHealthPercentChanges[i] : 0;
}


bool CvCivicInfo::isHurry(int i) const
{
	FAssertMsg(i < GC.getNumHurryInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabHurry ? m_pabHurry[i] : false;
}

bool CvCivicInfo::isSpecialBuildingNotRequired(int i) const
{
	FAssertMsg(i < GC.getNumSpecialBuildingInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabSpecialBuildingNotRequired ? m_pabSpecialBuildingNotRequired[i] : false;
}

bool CvCivicInfo::isSpecialistValid(int i) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabSpecialistValid ? m_pabSpecialistValid[i] : false;
}


int CvCivicInfo::getImprovementYieldChanges(int i, int j) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiImprovementYieldChanges && m_ppiImprovementYieldChanges[i]) ? m_ppiImprovementYieldChanges[i][j] : 0;
}

int CvCivicInfo::getTerrainYieldChanges(int i, int j) const
{
	FAssertMsg(i < GC.getNumTerrainInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiTerrainYieldChanges && m_ppiTerrainYieldChanges[i]) ? m_ppiTerrainYieldChanges[i][j] : 0;
}

int CvCivicInfo::getBuildingCommerceModifier(int i, int j) const
{
	FAssertMsg(i < GC.getNumBuildingInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiBuildingCommerceModifier && m_ppiBuildingCommerceModifier[i]) ? m_ppiBuildingCommerceModifier[i][j] : 0;
}

int CvCivicInfo::getBuildingClassCommerceChange(int i, int j) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiBuildingClassCommerceChange && m_ppiBuildingClassCommerceChange[i]) ? m_ppiBuildingClassCommerceChange[i][j] : 0;
}

int CvCivicInfo::getSpecialistYieldPercentChanges(int i, int j ) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiSpecialistYieldPercentChanges && m_ppiSpecialistYieldPercentChanges[i]) ? m_ppiSpecialistYieldPercentChanges[i][j] : 0;
}

int CvCivicInfo::getSpecialistCommercePercentChanges(int i, int j ) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return (m_ppiSpecialistCommercePercentChanges && m_ppiSpecialistCommercePercentChanges[i]) ? m_ppiSpecialistCommercePercentChanges[i][j] : 0;
}


int CvCivicInfo::getCityLimit(PlayerTypes ePlayer) const
{
	if (ePlayer > NO_PLAYER && GC.getGameINLINE().isOption(GAMEOPTION_NO_CITY_LIMITS))
	{
		return 0;
	}
	//ls612: Option to scale City Limits by Mapsize
	else if (ePlayer > NO_PLAYER && GC.getGameINLINE().isOption(GAMEOPTION_SCALE_CITY_LIMITS))
	{
		int iCityLimit;
		CvPlayer* pPlayer = &GET_PLAYER(ePlayer);
		int iAdaptID = GC.getInfoTypeForString("ADAPT_SCALE_CITY_LIMITS");

		iCityLimit = pPlayer->getGameObject()->adaptValueToGame(iAdaptID , m_iCityLimit);

		return iCityLimit;
	}
	else
	{
		return m_iCityLimit;
	}
}


bool CvCivicInfo::isAnyBuildingHappinessChange() const
{
	return (m_paiBuildingHappinessChanges != NULL);
}

bool CvCivicInfo::isAnyBuildingHealthChange() const
{
	return (m_paiBuildingHealthChanges != NULL);
}

bool CvCivicInfo::isAnyFeatureHappinessChange() const
{
	return (m_paiFeatureHappinessChanges != NULL);
}

bool CvCivicInfo::isAnySpecialistValid() const
{
	return (m_pabSpecialistValid != NULL);
}

bool CvCivicInfo::isAnyImprovementYieldChange() const
{
	return m_bAnyImprovementYieldChange;
}

bool CvCivicInfo::isAnySpecialistCommerceChanges() const
{
	return m_bAnySpecialistCommerceChanges;
}

bool CvCivicInfo::isAnySpecialistYieldChanges() const
{
	return m_bAnySpecialistYieldChanges;
}


CvString CvCivicInfo::getCivicAttitudeReason(int i) const
{
	FAssertMsg(i < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pszCivicAttitudeReason[i];
}

int CvCivicInfo::getCivicAttitudeVectorSize()					{return m_aszCivicAttitudeforPass3.size();}
CvString CvCivicInfo::getCivicAttitudeNamesVectorElement(int i)	{return m_aszCivicAttitudeforPass3[i];}
int CvCivicInfo::getCivicAttitudeValuesVectorElement(int i)		{return m_aiCivicAttitudeforPass3[i];}

int CvCivicInfo::getCivicAttitudeReasonVectorSize()					{return m_aszCivicAttitudeReasonforPass3.size();}
CvString CvCivicInfo::getCivicAttitudeReasonNamesVectorElement(int i)	{return m_aszCivicAttitudeReasonforPass3[i];}
CvString CvCivicInfo::getCivicAttitudeReasonValuesVectorElement(int i)		{return m_aszCivicAttitudeReasonValueforPass3[i];}


void CvCivicInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iCivicOptionType);
	CheckSum(iSum, m_iAnarchyLength);
	CheckSum(iSum, m_iUpkeep);
	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_iGreatPeopleRateModifier);
	CheckSum(iSum, m_iGreatGeneralRateModifier);
	CheckSum(iSum, m_iDomesticGreatGeneralRateModifier);
	CheckSum(iSum, m_iStateReligionGreatPeopleRateModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iHomeAreaMaintenanceModifier);
	CheckSum(iSum, m_iOtherAreaMaintenanceModifier);
	CheckSum(iSum, m_iCorporationMaintenanceModifier);
	CheckSum(iSum, m_iExtraHealth);
	CheckSum(iSum, m_iFreeExperience);
	CheckSum(iSum, m_iWorkerSpeedModifier);
	CheckSum(iSum, m_iImprovementUpgradeRateModifier);
	CheckSum(iSum, m_iMilitaryProductionModifier);
	CheckSum(iSum, m_iBaseFreeUnits);
	CheckSum(iSum, m_iBaseFreeMilitaryUnits);
	CheckSum(iSum, m_iFreeUnitsPopulationPercent);
	CheckSum(iSum, m_iFreeMilitaryUnitsPopulationPercent);
	CheckSum(iSum, m_iGoldPerUnit);
	CheckSum(iSum, m_iGoldPerMilitaryUnit);
	CheckSum(iSum, m_iHappyPerMilitaryUnit);
	CheckSum(iSum, m_iLargestCityHappiness);
	CheckSum(iSum, m_iWarWearinessModifier);
	CheckSum(iSum, m_iFreeSpecialist);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iCivicPercentAnger);
	CheckSum(iSum, m_iMaxConscript);
	CheckSum(iSum, m_iStateReligionHappiness);
	CheckSum(iSum, m_iNonStateReligionHappiness);
	CheckSum(iSum, m_iStateReligionUnitProductionModifier);
	CheckSum(iSum, m_iStateReligionBuildingProductionModifier);
	CheckSum(iSum, m_iStateReligionFreeExperience);
	CheckSum(iSum, m_iExpInBorderModifier);
	CheckSum(iSum, m_iRevIdxLocal);
	CheckSum(iSum, m_iRevIdxNational);
	CheckSum(iSum, m_iRevIdxDistanceModifier);
	CheckSum(iSum, m_iRevIdxHolyCityGood);
	CheckSum(iSum, m_iRevIdxHolyCityBad);
	CheckSum(iSum, m_iRevIdxSwitchTo);
	CheckSum(iSum, m_iRevReligiousFreedom);
	CheckSum(iSum, m_iRevLaborFreedom);
	CheckSum(iSum, m_iRevEnvironmentalProtection);
	CheckSum(iSum, m_iRevDemocracyLevel);
	CheckSum(iSum, m_iAttitudeShareMod);
	CheckSum(iSum, m_iEnslavementChance);
	CheckSum(iSum, m_iPopulationgrowthratepercentage);
	CheckSum(iSum, m_iReligionSpreadRate);
	CheckSum(iSum, m_iCivicHappiness);
	CheckSum(iSum, m_iDistantUnitSupportCostModifier);
	CheckSum(iSum, m_iExtraCityDefense);
	CheckSum(iSum, m_iForeignTradeRouteModifier);
	CheckSum(iSum, m_iTaxRateUnhappiness);
	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_iHurryInflationModifier);
	CheckSum(iSum, m_iHurryCostModifier);
	CheckSum(iSum, m_iSharedCivicTradeRouteModifier);
	CheckSum(iSum, m_iLandmarkHappiness);
	CheckSum(iSum, m_iCorporationSpreadRate);
	CheckSum(iSum, m_iRealCorporationMaintenanceModifier);
	CheckSum(iSum, m_iForeignerUnhappyPercent);
	CheckSum(iSum, m_iCityLimit);
	CheckSum(iSum, m_iNationalCaptureProbabilityModifier);
	CheckSum(iSum, m_iNationalCaptureResistanceModifier);
	CheckSum(iSum, m_iFreedomFighterChange);

	CheckSum(iSum, m_fRevIdxNationalityMod);
	CheckSum(iSum, m_fRevIdxBadReligionMod);
	CheckSum(iSum, m_fRevIdxGoodReligionMod);
	CheckSum(iSum, m_fRevViolentMod);

	CheckSum(iSum, m_bUpgradeAnywhere);
	CheckSum(iSum, m_bAllowInquisitions);
	CheckSum(iSum, m_bDisallowInquisitions);
	CheckSum(iSum, m_bCommunism);
	CheckSum(iSum, m_bFreeSpeech);
	CheckSum(iSum, m_bCanDoElection);
	CheckSum(iSum, m_bMilitaryFoodProduction);
	CheckSum(iSum, m_bNoUnhealthyPopulation);
	CheckSum(iSum, m_bBuildingOnlyHealthy);
	CheckSum(iSum, m_bNoForeignTrade);
	CheckSum(iSum, m_bNoCorporations);
	CheckSum(iSum, m_bNoForeignCorporations);
	CheckSum(iSum, m_bStateReligion);
	CheckSum(iSum, m_bNoNonStateReligionSpread);
	CheckSum(iSum, m_bFixedBorders);
	CheckSum(iSum, m_bNoCapitalUnhappiness);
	CheckSum(iSum, m_bTaxationAnger);
	CheckSum(iSum, m_bNoLandmarkAnger);
	CheckSum(iSum, m_bEnablesMAD);
	CheckSum(iSum, m_bAllReligionsActive);
	CheckSum(iSum, m_bBansNonStateReligions);
	CheckSum(iSum, m_bFreedomFighter);
	CheckSum(iSum, m_bPolicy);

	// Arrays

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piCapitalYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piTradeYieldModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCommerceModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCapitalCommerceModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerce);
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_paiBuildingHappinessChanges);
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_paiBuildingHealthChanges);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_paiFeatureHappinessChanges);
	CheckSumI(iSum, GC.getNumHurryInfos(), m_pabHurry);
	CheckSumI(iSum, GC.getNumSpecialBuildingInfos(), m_pabSpecialBuildingNotRequired);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_pabSpecialistValid);

	int i;
	if (m_ppiImprovementYieldChanges)
		for(i=0;i<GC.getNumImprovementInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiImprovementYieldChanges[i]);
		}


	CheckSumI(iSum, GC.getNumBonusInfos(), m_piBonusMintedPercent);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_paiUnitCombatProductionModifier);
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_paiBuildingClassProductionModifier);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_piUnitClassProductionModifier);
	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piFlavorValue);
	CheckSumI(iSum, GC.getNumCivicInfos(), m_piCivicAttitudeChanges);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piLandmarkYieldChanges);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_piFreeSpecialistCount);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementHappinessChanges);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementHealthPercentChanges);

	if (m_ppiSpecialistYieldPercentChanges)
	{
		for(i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiSpecialistYieldPercentChanges[i]);
		}
	}

	if (m_ppiSpecialistCommercePercentChanges)
	{
		for(i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiSpecialistCommercePercentChanges[i]);
		}
	}

	if (m_ppiTerrainYieldChanges)
	{
		for(i=0;i<GC.getNumTerrainInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiTerrainYieldChanges[i]);
		}
	}

	if (m_ppiBuildingCommerceModifier)
	{
		for(i=0;i<GC.getNumBuildingInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiBuildingCommerceModifier[i]);
		}
	}

	if (m_ppiBuildingClassCommerceChange)
	{
		for(i=0;i<GC.getNumBuildingClassInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiBuildingClassCommerceChange[i]);
		}
	}

	if (m_ppiBonusCommerceModifier)
	{
		for(i=0;i<GC.getNumBonusInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiBonusCommerceModifier[i]);
		}
	}

	m_PropertyManipulators.getCheckSum(iSum);
}

bool CvCivicInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int j;
	int iNumSibs=0;				// the number of siblings the current xml node has
	int iIndex;

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivicOptionType");
	m_iCivicOptionType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereq");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iAnarchyLength, L"iAnarchyLength");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Upkeep");
	m_iUpkeep = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iAIWeight, L"iAIWeight");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateModifier, L"iGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatGeneralRateModifier, L"iGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, L"iDomesticGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionGreatPeopleRateModifier, L"iStateReligionGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHomeAreaMaintenanceModifier, L"iHomeAreaMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iOtherAreaMaintenanceModifier, L"iOtherAreaMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenanceModifier, L"iCorporationMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iExtraHealth, L"iExtraHealth");
	pXML->GetOptionalChildXmlValByName(&m_iFreeExperience, L"iFreeExperience");
	pXML->GetOptionalChildXmlValByName(&m_iWorkerSpeedModifier, L"iWorkerSpeedModifier");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementUpgradeRateModifier, L"iImprovementUpgradeRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryProductionModifier, L"iMilitaryProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iBaseFreeUnits, L"iBaseFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_iBaseFreeMilitaryUnits, L"iBaseFreeMilitaryUnits");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitsPopulationPercent, L"iFreeUnitsPopulationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iFreeMilitaryUnitsPopulationPercent, L"iFreeMilitaryUnitsPopulationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iGoldPerUnit, L"iGoldPerUnit");
	pXML->GetOptionalChildXmlValByName(&m_iGoldPerMilitaryUnit, L"iGoldPerMilitaryUnit");
	pXML->GetOptionalChildXmlValByName(&m_iHappyPerMilitaryUnit, L"iHappyPerMilitaryUnit");
	pXML->GetOptionalChildXmlValByName(&m_bMilitaryFoodProduction, L"bMilitaryFoodProduction");
	pXML->GetOptionalChildXmlValByName(&m_iMaxConscript, L"iMaxConscript");
	pXML->GetOptionalChildXmlValByName(&m_bNoUnhealthyPopulation, L"bNoUnhealthyPopulation");
	pXML->GetOptionalChildXmlValByName(&m_bBuildingOnlyHealthy, L"bBuildingOnlyHealthy");
	pXML->GetOptionalChildXmlValByName(&m_iLargestCityHappiness, L"iLargestCityHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessModifier, L"iWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeSpecialist, L"iFreeSpecialist");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_bNoForeignTrade, L"bNoForeignTrade");
	pXML->GetOptionalChildXmlValByName(&m_bNoCorporations, L"bNoCorporations");
	pXML->GetOptionalChildXmlValByName(&m_bNoForeignCorporations, L"bNoForeignCorporations");
	pXML->GetOptionalChildXmlValByName(&m_iCivicPercentAnger, L"iCivicPercentAnger");
	pXML->GetOptionalChildXmlValByName(&m_bStateReligion, L"bStateReligion");
	pXML->GetOptionalChildXmlValByName(&m_bNoNonStateReligionSpread, L"bNoNonStateReligionSpread");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionHappiness, L"iStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iNonStateReligionHappiness, L"iNonStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionUnitProductionModifier, L"iStateReligionUnitProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionBuildingProductionModifier, L"iStateReligionBuildingProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionFreeExperience, L"iStateReligionFreeExperience");
	pXML->GetOptionalChildXmlValByName(&m_iExpInBorderModifier, L"iExpInBorderModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxLocal, L"iRevIdxLocal");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxNational, L"iRevIdxNational");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxDistanceModifier, L"iRevIdxDistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxHolyCityGood, L"iRevIdxHolyCityGood");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxHolyCityBad, L"iRevIdxHolyCityBad");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxSwitchTo, L"iRevIdxSwitchTo");
	pXML->GetOptionalChildXmlValByName(&m_iRevReligiousFreedom, L"iRevReligiousFreedom");
	pXML->GetOptionalChildXmlValByName(&m_iRevLaborFreedom, L"iRevLaborFreedom");
	pXML->GetOptionalChildXmlValByName(&m_iRevEnvironmentalProtection, L"iRevEnvironmentalProtection");
	pXML->GetOptionalChildXmlValByName(&m_iRevDemocracyLevel, L"iRevDemocracyLevel");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeShareMod, L"iAttitudeShareMod");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationgrowthratepercentage, L"iPopulationgrowthratepercentage");
	pXML->GetOptionalChildXmlValByName(&m_iReligionSpreadRate, L"iReligionSpreadRate");
	pXML->GetOptionalChildXmlValByName(&m_iCivicHappiness, L"iCivicHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iDistantUnitSupportCostModifier, L"iDistantUnitSupportCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iExtraCityDefense, L"iExtraCityDefense");
	pXML->GetOptionalChildXmlValByName(&m_iRealCorporationMaintenanceModifier, L"iRealCorporationMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iForeignTradeRouteModifier, L"iForeignTradeRouteModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTaxRateUnhappiness, L"iTaxRateUnhappiness");
	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflation");
	pXML->GetOptionalChildXmlValByName(&m_iHurryInflationModifier, L"iHurryInflationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHurryCostModifier, L"iHurryCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iSharedCivicTradeRouteModifier, L"iSharedCivicTradeRouteModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLandmarkHappiness, L"iLandmarkHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iLandmarkHappiness, L"iCorporationSpreadRate");
	pXML->GetOptionalChildXmlValByName(&m_iForeignerUnhappyPercent, L"iForeignerUnhappyPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCityLimit, L"iCityLimit");
	pXML->GetOptionalChildXmlValByName(&m_iCityOverLimitUnhappy, L"iCityOverLimitUnhappy");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureProbabilityModifier, L"iNationalCaptureProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureResistanceModifier, L"iNationalCaptureResistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreedomFighterChange, L"iFreedomFighterChange");

	pXML->GetOptionalChildXmlValByName(&m_fRevIdxNationalityMod, L"fRevIdxNationalityMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxBadReligionMod, L"fRevIdxBadReligionMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxGoodReligionMod, L"fRevIdxGoodReligionMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevViolentMod, L"fRevViolentMod");

	pXML->GetOptionalChildXmlValByName(&m_bUpgradeAnywhere, L"bUpgradeAnywhere");
	pXML->GetOptionalChildXmlValByName(&m_bAllowInquisitions, L"bAllowInquisitions");
	pXML->GetOptionalChildXmlValByName(&m_bDisallowInquisitions, L"bDisallowInquisitions");
	pXML->GetOptionalChildXmlValByName(&m_bCommunism, L"bCommunism");
	pXML->GetOptionalChildXmlValByName(&m_bFreeSpeech, L"bFreeSpeech");
	pXML->GetOptionalChildXmlValByName(&m_bCanDoElection, L"bCanDoElection");
	pXML->GetOptionalChildXmlValByName(&m_bFixedBorders, L"bFixedBorders");
	pXML->GetOptionalChildXmlValByName(&m_bNoCapitalUnhappiness, L"bNoCapitalUnhappiness");
	pXML->GetOptionalChildXmlValByName(&m_bTaxationAnger, L"bTaxationAnger");
	pXML->GetOptionalChildXmlValByName(&m_bNoLandmarkAnger, L"bNoLandmarkAnger");
	pXML->GetOptionalChildXmlValByName(&m_bEnablesMAD, L"bEnablesMADNukes");
	pXML->GetOptionalChildXmlValByName(&m_bAllReligionsActive, L"bAllReligionsActive");
	pXML->GetOptionalChildXmlValByName(&m_bBansNonStateReligions, L"bBansNonStateReligions");
	pXML->GetOptionalChildXmlValByName(&m_bFreedomFighter, L"bFreedomFighter");
	pXML->GetOptionalChildXmlValByName(&m_bPolicy, L"bPolicy");


	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalYieldModifiers"))
	{
		pXML->SetYields(&m_piCapitalYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"TradeYieldModifiers"))
	{
		pXML->SetYields(&m_piTradeYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piTradeYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalCommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCapitalCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraCommerces"))
	{
		pXML->SetCommerce(&m_piSpecialistExtraCommerce);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	}

	pXML->SetVariableListTagPair(&m_pabHurry, L"Hurrys", GC.getNumHurryInfos());
	pXML->SetVariableListTagPair(&m_pabSpecialBuildingNotRequired, L"SpecialBuildingNotRequireds",  GC.getNumSpecialBuildingInfos());

/********************************************************************************/
/* 	New Civic AI						02.08.2010				Fuyu			*/
/********************************************************************************/
	pXML->SetVariableListTagPair(&m_pabSpecialistValid, L"SpecialistValids", GC.getNumSpecialistInfos());

	pXML->SetVariableListTagPair(&m_paiBuildingHappinessChanges, L"BuildingHappinessChanges", GC.getNumBuildingClassInfos());
	pXML->SetVariableListTagPair(&m_paiBuildingHealthChanges, L"BuildingHealthChanges", GC.getNumBuildingClassInfos());

	pXML->SetVariableListTagPair(&m_paiFeatureHappinessChanges, L"FeatureHappinessChanges", GC.getNumFeatureInfos());

	// initialize the boolean list to the correct size and all the booleans to false
	FAssertMsg((GC.getNumImprovementInfos() > 0) && (NUM_YIELD_TYPES) > 0,"either the number of improvement infos is zero or less or the number of yield types is zero or less");
	m_bAnyImprovementYieldChange = false;
	if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DIntList(&m_ppiImprovementYieldChanges, GC.getNumImprovementInfos(), NUM_YIELD_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiImprovementYieldChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"ImprovementYields"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiImprovementYieldChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
						else
						{
							SAFE_DELETE_ARRAY(m_ppiImprovementYieldChanges[iIndex]);
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				for(int ii=0;(!m_bAnyImprovementYieldChange) && ii<GC.getNumImprovementInfos();ii++)
				{
					for(int ij=0; ij < NUM_YIELD_TYPES; ij++ )
					{
						if( getImprovementYieldChanges(ii, ij) != 0 )
						{
							m_bAnyImprovementYieldChange = true;
							break;
						}
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WeLoveTheKing");
	setWeLoveTheKingKey(szTextVal);




	pXML->SetVariableListTagPair(&m_piBonusMintedPercent, L"BonusMintedPercents", GC.getNumBonusInfos());

	pXML->SetVariableListTagPair(&m_piImprovementHappinessChanges, L"ImprovementHappinessChanges", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPair(&m_piImprovementHealthPercentChanges, L"ImprovementHealthPercentChanges", GC.getNumImprovementInfos());

	pXML->SetVariableListTagPair(&m_paiUnitCombatProductionModifier, L"UnitCombatProductionModifiers", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_paiBuildingClassProductionModifier, L"BuildingClassProductionModifiers",  GC.getNumBuildingClassInfos());
	pXML->SetVariableListTagPair(&m_piUnitClassProductionModifier, L"UnitClassProductionModifiers", GC.getNumUnitClassInfos());

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getFlavorTypes(), GC.getNumFlavorTypes());
	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, L"FreeSpecialistCounts", GC.getNumSpecialistInfos());
	pXML->GetOptionalChildXmlValByName(&m_iEnslavementChance, L"iEnslavementChance");

	if (pXML->TryMoveToXmlFirstChild(L"LandmarkYieldChanges"))
	{
		pXML->SetYields(&m_piLandmarkYieldChanges);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piLandmarkYieldChanges);
	}

	FAssertMsg((GC.getNumSpecialistInfos() > 0) && (NUM_YIELD_TYPES) > 0,"either the number of terrain infos is zero or less or the number of yield types is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldPercentChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DIntList(&m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos(), NUM_YIELD_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiSpecialistYieldPercentChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldPercents"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiSpecialistYieldPercentChanges[iIndex]);
							pXML->MoveToXmlParent();
							m_bAnySpecialistYieldChanges = true;
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	FAssertMsg((GC.getNumSpecialistInfos() > 0) && (NUM_COMMERCE_TYPES) > 0,"either the number of terrain infos is zero or less or the number of commerce types is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommercePercentChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DIntList(&m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiSpecialistCommercePercentChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommercePercents"))
						{
							// call the function that sets the commerce change variable
							pXML->SetCommerce(&m_ppiSpecialistCommercePercentChanges[iIndex]);
							pXML->MoveToXmlParent();
							m_bAnySpecialistCommerceChanges = true;
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	FAssertMsg((GC.getNumTerrainInfos() > 0) && (NUM_YIELD_TYPES) > 0,"either the number of terrain infos is zero or less or the number of yield types is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"TerrainYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DIntList(&m_ppiTerrainYieldChanges, GC.getNumTerrainInfos(), NUM_YIELD_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"TerrainType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiTerrainYieldChanges[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"TerrainYields"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiTerrainYieldChanges[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}
			else
			{
				SAFE_DELETE_ARRAY(m_ppiTerrainYieldChanges);
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"BuildingCommerceModifiers"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DIntList(&m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiBuildingCommerceModifier[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
						{
							// call the function that sets the yield change variable
							pXML->SetCommerce(&m_ppiBuildingCommerceModifier[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"BuildingClassCommerceChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DIntList(&m_ppiBuildingClassCommerceChange, GC.getNumBuildingClassInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingClassType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiBuildingClassCommerceChange[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
						{
							// call the function that sets the yield change variable
							pXML->SetCommerce(&m_ppiBuildingClassCommerceChange[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"BonusCommerceModifiers"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DIntList(&m_ppiBonusCommerceModifier, GC.getNumBonusInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"BonusType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiBonusCommerceModifier[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
						{
							// call the function that sets the yield change variable
							pXML->SetCommerce(&m_ppiBonusCommerceModifier[iIndex]);
							pXML->MoveToXmlParent();
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"CivicAttitudeChanges"))
	{
			int iTemp = 0;
			CvString szTemp;
			int iNumSibs = pXML->GetXmlChildrenNumber();
			if (pXML->TryMoveToXmlFirstChild())
			{
				if (iNumSibs > 0)
				{
					for (int i=0;i<iNumSibs;i++)
					{
						if (pXML->GetChildXmlVal(szTextVal))
						{
							m_aszCivicAttitudeforPass3.push_back(szTextVal);
							pXML->GetNextXmlVal(&iTemp);
							m_aiCivicAttitudeforPass3.push_back(iTemp);

							m_aszCivicAttitudeReasonforPass3.push_back(szTextVal);
							pXML->GetNextXmlVal(szTemp);
							m_aszCivicAttitudeReasonValueforPass3.push_back(szTemp);

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

	m_PropertyManipulators.read(pXML);
	return true;
}

bool CvCivicInfo::readPass3()
{
	m_piCivicAttitudeChanges = new int[GC.getNumCivicInfos()];
	m_pszCivicAttitudeReason = new CvString[GC.getNumCivicInfos()];
	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		m_piCivicAttitudeChanges[iI] = 0;
		m_pszCivicAttitudeReason[iI] = "";
	}
	if (!m_aiCivicAttitudeforPass3.empty() && !m_aszCivicAttitudeforPass3.empty())
	{
		int iNumLoad = m_aiCivicAttitudeforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszCivicAttitudeforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_piCivicAttitudeChanges[iTempIndex] = m_aiCivicAttitudeforPass3[iI];
		}
		m_aszCivicAttitudeforPass3.clear();
		m_aiCivicAttitudeforPass3.clear();
	}
	if (!m_aszCivicAttitudeReasonValueforPass3.empty() && !m_aszCivicAttitudeReasonforPass3.empty())
	{
		int iNumLoad = m_aszCivicAttitudeReasonValueforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszCivicAttitudeReasonforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_pszCivicAttitudeReason[iTempIndex] = m_aszCivicAttitudeReasonValueforPass3[iI];
		}
		m_aszCivicAttitudeReasonforPass3.clear();
		m_aszCivicAttitudeReasonValueforPass3.clear();
	}

	return true;
}

void CvCivicInfo::copyNonDefaults(CvCivicInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getCivicOptionType() == iTextDefault) m_iCivicOptionType = pClassInfo->getCivicOptionType();
	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getAnarchyLength() == iDefault) m_iAnarchyLength = pClassInfo->getAnarchyLength();
	if (getUpkeep() == iTextDefault) m_iUpkeep = pClassInfo->getUpkeep();

	if (getAIWeight() == iDefault) m_iAIWeight = pClassInfo->getAIWeight();
	if (getGreatPeopleRateModifier() == iDefault) m_iGreatPeopleRateModifier = pClassInfo->getGreatPeopleRateModifier();
	if (getGreatGeneralRateModifier() == iDefault) m_iGreatGeneralRateModifier = pClassInfo->getGreatGeneralRateModifier();
	if (getDomesticGreatGeneralRateModifier() == iDefault) m_iDomesticGreatGeneralRateModifier = pClassInfo->getDomesticGreatGeneralRateModifier();
	if (getStateReligionGreatPeopleRateModifier() == iDefault) m_iStateReligionGreatPeopleRateModifier = pClassInfo->getStateReligionGreatPeopleRateModifier();
	if (getDistanceMaintenanceModifier() == iDefault) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (getNumCitiesMaintenanceModifier() == iDefault) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (getCorporationMaintenanceModifier() == iDefault) m_iCorporationMaintenanceModifier = pClassInfo->getCorporationMaintenanceModifier();
	if (getExtraHealth() == iDefault) m_iExtraHealth = pClassInfo->getExtraHealth();
	if (getFreeExperience() == iDefault) m_iFreeExperience = pClassInfo->getFreeExperience();
	if (getWorkerSpeedModifier() == iDefault) m_iWorkerSpeedModifier = pClassInfo->getWorkerSpeedModifier();
	if (getImprovementUpgradeRateModifier() == iDefault) m_iImprovementUpgradeRateModifier = pClassInfo->getImprovementUpgradeRateModifier();
	if (getMilitaryProductionModifier() == iDefault) m_iMilitaryProductionModifier = pClassInfo->getMilitaryProductionModifier();
	if (getBaseFreeUnits() == iDefault) m_iBaseFreeUnits = pClassInfo->getBaseFreeUnits();
	if (getBaseFreeMilitaryUnits() == iDefault) m_iBaseFreeMilitaryUnits = pClassInfo->getBaseFreeMilitaryUnits();
	if (getFreeUnitsPopulationPercent() == iDefault) m_iFreeUnitsPopulationPercent = pClassInfo->getFreeUnitsPopulationPercent();
	if (getFreeMilitaryUnitsPopulationPercent() == iDefault) m_iFreeMilitaryUnitsPopulationPercent = pClassInfo->getFreeMilitaryUnitsPopulationPercent();
	if (getGoldPerUnit() == iDefault) m_iGoldPerUnit = pClassInfo->getGoldPerUnit();
	if (getGoldPerMilitaryUnit() == iDefault) m_iGoldPerMilitaryUnit = pClassInfo->getGoldPerMilitaryUnit();
	if (getHappyPerMilitaryUnit() == iDefault) m_iHappyPerMilitaryUnit = pClassInfo->getHappyPerMilitaryUnit();
	if (isMilitaryFoodProduction() == bDefault) m_bMilitaryFoodProduction = pClassInfo->isMilitaryFoodProduction();
	if (getMaxConscript() == iDefault) m_iMaxConscript = pClassInfo->getMaxConscript();
	if (isNoUnhealthyPopulation() == bDefault) m_bNoUnhealthyPopulation = pClassInfo->isNoUnhealthyPopulation();
	if (isBuildingOnlyHealthy() == bDefault) m_bBuildingOnlyHealthy = pClassInfo->isBuildingOnlyHealthy();
	if (getLargestCityHappiness() == iDefault) m_iLargestCityHappiness = pClassInfo->getLargestCityHappiness();
	if (getWarWearinessModifier() == iDefault) m_iWarWearinessModifier = pClassInfo->getWarWearinessModifier();
	if (getFreeSpecialist() == iDefault) m_iFreeSpecialist = pClassInfo->getFreeSpecialist();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (isNoForeignTrade() == bDefault) m_bNoForeignTrade = pClassInfo->isNoForeignTrade();
	if (isNoCorporations() == bDefault) m_bNoCorporations = pClassInfo->isNoCorporations();
	if (isNoForeignCorporations() == bDefault) m_bNoForeignCorporations = pClassInfo->isNoForeignCorporations();
	if (getCivicPercentAnger() == iDefault) m_iCivicPercentAnger = pClassInfo->getCivicPercentAnger();
	if (isStateReligion() == bDefault) m_bStateReligion = pClassInfo->isStateReligion();
	if (isNoNonStateReligionSpread() == bDefault) m_bNoNonStateReligionSpread = pClassInfo->isNoNonStateReligionSpread();
	if (getStateReligionHappiness() == iDefault) m_iStateReligionHappiness = pClassInfo->getStateReligionHappiness();
	if (getNonStateReligionHappiness() == iDefault) m_iNonStateReligionHappiness = pClassInfo->getNonStateReligionHappiness();
	if (getStateReligionUnitProductionModifier() == iDefault) m_iStateReligionUnitProductionModifier = pClassInfo->getStateReligionUnitProductionModifier();
	if (getStateReligionBuildingProductionModifier() == iDefault) m_iStateReligionBuildingProductionModifier = pClassInfo->getStateReligionBuildingProductionModifier();
	if (getStateReligionFreeExperience() == iDefault) m_iStateReligionFreeExperience = pClassInfo->getStateReligionFreeExperience();
	if (getExpInBorderModifier() == iDefault) m_iExpInBorderModifier = pClassInfo->getExpInBorderModifier();
	if (isUpgradeAnywhere() == bDefault) m_bUpgradeAnywhere = pClassInfo->isUpgradeAnywhere();
	if (isAllowInquisitions() == bDefault) m_bAllowInquisitions = pClassInfo->isAllowInquisitions();
	if (isDisallowInquisitions() == bDefault) m_bDisallowInquisitions = pClassInfo->isDisallowInquisitions();
	if (getRevIdxLocal() == iDefault) m_iRevIdxLocal = pClassInfo->getRevIdxLocal();
	if (getRevIdxNational() == iDefault) m_iRevIdxNational = pClassInfo->getRevIdxNational();
	if (getRevIdxDistanceModifier() == iDefault) m_iRevIdxDistanceModifier = pClassInfo->getRevIdxDistanceModifier();
	if (getRevIdxHolyCityGood() == iDefault) m_iRevIdxHolyCityGood = pClassInfo->getRevIdxHolyCityGood();
	if (getRevIdxHolyCityBad() == iDefault) m_iRevIdxHolyCityBad = pClassInfo->getRevIdxHolyCityBad();
	if (getRevIdxSwitchTo() == iDefault) m_iRevIdxSwitchTo = pClassInfo->getRevIdxSwitchTo();
	if (getRevIdxNationalityMod() == fDefault) m_fRevIdxNationalityMod = pClassInfo->getRevIdxNationalityMod();
	if (getRevIdxBadReligionMod() == fDefault) m_fRevIdxBadReligionMod = pClassInfo->getRevIdxBadReligionMod();
	if (getRevIdxGoodReligionMod() == fDefault) m_fRevIdxGoodReligionMod = pClassInfo->getRevIdxGoodReligionMod();
	if (getRevViolentMod() == fDefault) m_fRevViolentMod = pClassInfo->getRevViolentMod();
	if (getRevReligiousFreedom() == iDefault) m_iRevReligiousFreedom = pClassInfo->getRevReligiousFreedom();
	if (getRevLaborFreedom() == iDefault) m_iRevLaborFreedom = pClassInfo->getRevLaborFreedom();
	if (getRevEnvironmentalProtection() == iDefault) m_iRevEnvironmentalProtection = pClassInfo->getRevEnvironmentalProtection();
	if (getRevDemocracyLevel() == iDefault) m_iRevDemocracyLevel = pClassInfo->getRevDemocracyLevel();
	if (getFreedomFighterChange() == iDefault) m_iFreedomFighterChange = pClassInfo->getFreedomFighterChange();
	if (isCommunism() == bDefault) m_bCommunism = pClassInfo->isCommunism();
	if (isFreeSpeech() == bDefault) m_bFreeSpeech = pClassInfo->isFreeSpeech();
	if (isCanDoElection() == bDefault) m_bCanDoElection = pClassInfo->isCanDoElection();
	if (isFreedomFighter() == bDefault) m_bFreedomFighter = pClassInfo->isFreedomFighter();
	if (isPolicy() == bDefault) m_bPolicy = pClassInfo->isPolicy();


	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getYieldModifier(i) == iDefault && pClassInfo->getYieldModifier(i) != iDefault)
		{
			if ( NULL == m_piYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piYieldModifier[i] = pClassInfo->getYieldModifier(i);
		}
		if ( getCapitalYieldModifier(i) == iDefault && pClassInfo->getCapitalYieldModifier(i) != iDefault)
		{
			if ( NULL == m_piCapitalYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCapitalYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piCapitalYieldModifier[i] = pClassInfo->getCapitalYieldModifier(i);
		}
		if ( getTradeYieldModifier(i) == iDefault && pClassInfo->getTradeYieldModifier(i) != iDefault)
		{
			if ( NULL == m_piTradeYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piTradeYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piTradeYieldModifier[i] = pClassInfo->getTradeYieldModifier(i);
		}
	}

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( getCommerceModifier(i) == iDefault && pClassInfo->getCommerceModifier(i) != iDefault)
		{
			if ( NULL == m_piCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceModifier[i] = pClassInfo->getCommerceModifier(i);
		}
		if ( getCapitalCommerceModifier(i) == iDefault && pClassInfo->getCapitalCommerceModifier(i) != iDefault)
		{
			if ( NULL == m_piCapitalCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCapitalCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCapitalCommerceModifier[i] = pClassInfo->getCapitalCommerceModifier(i);
		}
		if ( getSpecialistExtraCommerce(i) == iDefault && pClassInfo->getSpecialistExtraCommerce(i) != iDefault)
		{
			if ( NULL == m_piSpecialistExtraCommerce )
			{
				CvXMLLoadUtility::InitList(&m_piSpecialistExtraCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piSpecialistExtraCommerce[i] = pClassInfo->getSpecialistExtraCommerce(i);
		}
	}

	for ( int i = 0; i < GC.getNumHurryInfos(); i++ )
	{
		if ( isHurry(i) == bDefault && pClassInfo->isHurry(i) != bDefault)
		{
			if ( NULL == m_pabHurry )
			{
				CvXMLLoadUtility::InitList(&m_pabHurry,GC.getNumHurryInfos(),bDefault);
			}
			m_pabHurry[i] = pClassInfo->isHurry(i);
		}
	}
	for ( int i = 0; i < GC.getNumSpecialBuildingInfos(); i++ )
	{
		if ( isSpecialBuildingNotRequired(i) == bDefault && pClassInfo->isSpecialBuildingNotRequired(i) != bDefault)
		{
			if ( NULL == m_pabSpecialBuildingNotRequired)
			{
				CvXMLLoadUtility::InitList(&m_pabSpecialBuildingNotRequired,GC.getNumSpecialBuildingInfos(),bDefault);
			}
			m_pabSpecialBuildingNotRequired[i] = pClassInfo->isSpecialBuildingNotRequired(i);
		}
	}
	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++ )
	{
		if ( isSpecialistValid(i) == bDefault && pClassInfo->isSpecialistValid(i) != bDefault)
		{
			if ( NULL == m_pabSpecialistValid )
			{
				CvXMLLoadUtility::InitList(&m_pabSpecialistValid,GC.getNumSpecialistInfos(),bDefault);
			}
			m_pabSpecialistValid[i] = pClassInfo->isSpecialistValid(i);
		}
	}

	for ( int i = 0; i < GC.getNumBuildingClassInfos(); i++ )
	{
		if ( getBuildingHappinessChanges(i) == iDefault && pClassInfo->getBuildingHappinessChanges(i) != iDefault)
		{
			if ( NULL == m_paiBuildingHappinessChanges )
			{
				CvXMLLoadUtility::InitList(&m_paiBuildingHappinessChanges,GC.getNumBuildingClassInfos(),iDefault);
			}
			m_paiBuildingHappinessChanges[i] = pClassInfo->getBuildingHappinessChanges(i);
		}
		if ( getBuildingHealthChanges(i) == iDefault && pClassInfo->getBuildingHealthChanges(i) != iDefault)
		{
			if ( NULL == m_paiBuildingHealthChanges )
			{
				CvXMLLoadUtility::InitList(&m_paiBuildingHealthChanges,GC.getNumBuildingClassInfos(),iDefault);
			}
			m_paiBuildingHealthChanges[i] = pClassInfo->getBuildingHealthChanges(i);
		}
	}
	for ( int i = 0; i < GC.getNumFeatureInfos(); i++ )
	{
		if ( getFeatureHappinessChanges(i) == iDefault && pClassInfo->getFeatureHappinessChanges(i) != iDefault)
		{
			if ( NULL == m_paiFeatureHappinessChanges )
			{
				CvXMLLoadUtility::InitList(&m_paiFeatureHappinessChanges,GC.getNumFeatureInfos(),iDefault);
			}
			m_paiFeatureHappinessChanges[i] = pClassInfo->getFeatureHappinessChanges(i);
		}
	}

	for ( int i = 0; i < GC.getNumImprovementInfos(); i++ )
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			if ( getImprovementYieldChanges(i, j) == iDefault && pClassInfo->getImprovementYieldChanges(i,j) != iDefault)
			{
				if ( NULL == m_ppiImprovementYieldChanges )
				{
					pXML->Init2DIntList(&m_ppiImprovementYieldChanges, GC.getNumImprovementInfos(), NUM_YIELD_TYPES);
				}
				else if ( NULL == m_ppiImprovementYieldChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiImprovementYieldChanges[i],NUM_YIELD_TYPES,iDefault);
				}
				m_ppiImprovementYieldChanges[i][j] = pClassInfo->getImprovementYieldChanges(i,j);
			}
		}
	}

	if (getWeLoveTheKingKey() == wDefault)
	{
		m_szWeLoveTheKingKey = pClassInfo->getWeLoveTheKingKey();
	}

/************************************************************************************************/
/* Afforess					  Start		 Last Update: 6/13/10									*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getAttitudeShareMod() == iDefault) m_iAttitudeShareMod = pClassInfo->getAttitudeShareMod();
	if (getEnslavementChance() == iDefault) m_iEnslavementChance = pClassInfo->getEnslavementChance();
	if (getForeignTradeRouteModifier() == iDefault) m_iForeignTradeRouteModifier = pClassInfo->getForeignTradeRouteModifier();
	if (getPopulationgrowthratepercentage() == iDefault) m_iPopulationgrowthratepercentage = pClassInfo->getPopulationgrowthratepercentage();
	if (getReligionSpreadRate() == iDefault) m_iReligionSpreadRate = pClassInfo->getReligionSpreadRate();
	if (getCivicHappiness() == iDefault) m_iCivicHappiness = pClassInfo->getCivicHappiness();
	if (getDistantUnitSupportCostModifier() == iDefault) m_iDistantUnitSupportCostModifier = pClassInfo->getDistantUnitSupportCostModifier();
	if (getExtraCityDefense() == iDefault) m_iExtraCityDefense = pClassInfo->getExtraCityDefense();
	if (getTaxRateUnhappiness() == iDefault) m_iTaxRateUnhappiness = pClassInfo->getTaxRateUnhappiness();
	if (IsFixedBorders() == bDefault) m_bFixedBorders = pClassInfo->IsFixedBorders();
	if (isNoCapitalUnhappiness() == bDefault) m_bNoCapitalUnhappiness = pClassInfo->isNoCapitalUnhappiness();
	if (isTaxationAnger() == bDefault) m_bTaxationAnger = pClassInfo->isTaxationAnger();
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();
	if (getHurryInflationModifier() == iDefault) m_iHurryInflationModifier = pClassInfo->getHurryInflationModifier();
	if (getHurryCostModifier() == iDefault) m_iHurryCostModifier = pClassInfo->getHurryCostModifier();
	if (getSharedCivicTradeRouteModifier() == iDefault) m_iSharedCivicTradeRouteModifier = pClassInfo->getSharedCivicTradeRouteModifier();
	if (getLandmarkHappiness() == iDefault) m_iLandmarkHappiness = pClassInfo->getLandmarkHappiness();
	if (getCorporationSpreadRate() == iDefault) m_iCorporationSpreadRate = pClassInfo->getCorporationSpreadRate();
	if (isNoLandmarkAnger() == bDefault) m_bNoLandmarkAnger = pClassInfo->isNoLandmarkAnger();
	if (isEnablesMAD() == bDefault) m_bEnablesMAD = pClassInfo->isEnablesMAD();
	if (getRealCorporationMaintenanceModifier() == iDefault) m_iRealCorporationMaintenanceModifier = pClassInfo->getRealCorporationMaintenanceModifier();
	if (getCityLimit(NO_PLAYER) == iDefault) m_iCityLimit = pClassInfo->getCityLimit(NO_PLAYER);
	if (getCityOverLimitUnhappy() == iDefault) m_iCityOverLimitUnhappy = pClassInfo->getCityOverLimitUnhappy();
	if (getForeignerUnhappyPercent() == iDefault) m_iForeignerUnhappyPercent = pClassInfo->getForeignerUnhappyPercent();

	if (!m_iHomeAreaMaintenanceModifier) m_iHomeAreaMaintenanceModifier = pClassInfo->getHomeAreaMaintenanceModifier();
	if (!m_iOtherAreaMaintenanceModifier) m_iOtherAreaMaintenanceModifier = pClassInfo->getOtherAreaMaintenanceModifier();

	for ( int i = 0; i < GC.getNumImprovementInfos(); i++ )
	{
		if ( getImprovementHealthPercentChanges(i) == iDefault && pClassInfo->getImprovementHealthPercentChanges(i) != iDefault)
		{
			if ( NULL == m_piImprovementHealthPercentChanges )
			{
				CvXMLLoadUtility::InitList(&m_piImprovementHealthPercentChanges,GC.getNumImprovementInfos(),iDefault);
			}
			m_piImprovementHealthPercentChanges[i] = pClassInfo->getImprovementHealthPercentChanges(i);
		}

		if ( getImprovementHappinessChanges(i) == iDefault && pClassInfo->getImprovementHappinessChanges(i) != iDefault)
		{
			if ( NULL == m_piImprovementHappinessChanges )
			{
				CvXMLLoadUtility::InitList(&m_piImprovementHappinessChanges,GC.getNumImprovementInfos(),iDefault);
			}
			m_piImprovementHappinessChanges[i] = pClassInfo->getImprovementHappinessChanges(i);
		}
	}


	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( getLandmarkYieldChanges(i) == iDefault && pClassInfo->getLandmarkYieldChanges(i) != iDefault)
		{
			if ( NULL == m_piLandmarkYieldChanges )
			{
				CvXMLLoadUtility::InitList(&m_piLandmarkYieldChanges,NUM_YIELD_TYPES,iDefault);
			}
			m_piLandmarkYieldChanges[i] = pClassInfo->getLandmarkYieldChanges(i);
		}
	}

	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++ )
	{
		if ( getFreeSpecialistCount(i) == iDefault && pClassInfo->getFreeSpecialistCount(i) != iDefault)
		{
			if ( NULL == m_piFreeSpecialistCount)
			{
				CvXMLLoadUtility::InitList(&m_piFreeSpecialistCount,GC.getNumSpecialistInfos(),iDefault);
			}
			m_piFreeSpecialistCount[i] = pClassInfo->getFreeSpecialistCount(i);
		}

		for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			if ( getSpecialistYieldPercentChanges(i,j) == iDefault && pClassInfo->getSpecialistYieldPercentChanges(i,j) != iDefault)
			{
				if ( NULL == m_ppiSpecialistYieldPercentChanges )
				{
					pXML->Init2DIntList(&m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos(), NUM_YIELD_TYPES);
				}
				else if ( NULL == m_ppiSpecialistYieldPercentChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiSpecialistYieldPercentChanges[i],NUM_YIELD_TYPES,iDefault);
				}
				m_ppiSpecialistYieldPercentChanges[i][j] = pClassInfo->getSpecialistYieldPercentChanges(i,j);
			}
		}

		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getSpecialistCommercePercentChanges(i,j) == iDefault && pClassInfo->getSpecialistCommercePercentChanges(i,j) != iDefault)
			{
				if ( NULL == m_ppiSpecialistCommercePercentChanges )
				{
					pXML->Init2DIntList(&m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiSpecialistCommercePercentChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiSpecialistCommercePercentChanges[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiSpecialistCommercePercentChanges[i][j] = pClassInfo->getSpecialistCommercePercentChanges(i,j);
			}
		}
	}

	for ( int i = 0; i < GC.getNumUnitCombatInfos(); i++ )
	{
		if ( getUnitCombatProductionModifier(i) == iDefault && pClassInfo->getUnitCombatProductionModifier(i) != iDefault)
		{
			if ( NULL == m_paiUnitCombatProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_paiUnitCombatProductionModifier,GC.getNumUnitCombatInfos(),iDefault);
			}
			m_paiUnitCombatProductionModifier[i] = pClassInfo->getUnitCombatProductionModifier(i);
		}
	}

	for ( int j = 0; j < GC.getNumUnitClassInfos(); j++)
	{
		if ( getUnitClassProductionModifier(j) == iDefault && pClassInfo->getUnitClassProductionModifier(j) != iDefault)
		{
			if ( NULL == m_piUnitClassProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_piUnitClassProductionModifier,GC.getNumUnitClassInfos(),iDefault);
			}
			m_piUnitClassProductionModifier[j] = pClassInfo->getUnitClassProductionModifier(j);
		}
	}

	for ( int i = 0; i < GC.getNumBuildingClassInfos(); i++ )
	{
		if ( getBuildingClassProductionModifier(i) == iDefault && pClassInfo->getBuildingClassProductionModifier(i) != iDefault)
		{
			if ( NULL == m_paiBuildingClassProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_paiBuildingClassProductionModifier,GC.getNumBuildingClassInfos(),iDefault);
			}
			m_paiBuildingClassProductionModifier[i] = pClassInfo->getBuildingClassProductionModifier(i);
		}
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getBuildingClassCommerceChange(i,j) == iDefault && pClassInfo->getBuildingClassCommerceChange(i,j) != iDefault)
			{
				if ( NULL == m_ppiBuildingClassCommerceChange )
				{
					pXML->Init2DIntList(&m_ppiBuildingClassCommerceChange, GC.getNumBuildingClassInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiBuildingClassCommerceChange[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiBuildingClassCommerceChange[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiBuildingClassCommerceChange[i][j] = pClassInfo->getBuildingClassCommerceChange(i,j);
			}
		}
	}

	for ( int i = 0; i < GC.getNumTerrainInfos(); i++ )
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			if ( getTerrainYieldChanges(i,j) == iDefault && pClassInfo->getTerrainYieldChanges(i,j) != iDefault)
			{
				if ( NULL == m_ppiTerrainYieldChanges)
				{
					pXML->Init2DIntList(&m_ppiTerrainYieldChanges, GC.getNumTerrainInfos(), NUM_YIELD_TYPES);
				}
				else if ( NULL == m_ppiTerrainYieldChanges[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiTerrainYieldChanges[i],NUM_YIELD_TYPES,iDefault);
				}
				m_ppiTerrainYieldChanges[i][j] = pClassInfo->getTerrainYieldChanges(i,j);
			}
		}
	}
	for ( int i = 0; i < GC.getNumBuildingInfos(); i++ )
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getBuildingCommerceModifier(i,j) == iDefault && pClassInfo->getBuildingCommerceModifier(i,j) != iDefault)
			{
				if ( NULL == m_ppiBuildingCommerceModifier)
				{
					pXML->Init2DIntList(&m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiBuildingCommerceModifier[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiBuildingCommerceModifier[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiBuildingCommerceModifier[i][j] = pClassInfo->getBuildingCommerceModifier(i,j);
			}
		}
	}
	for ( int i = 0; i < GC.getNumBonusInfos(); i++ )
	{
		if (getBonusMintedPercent(i) == iDefault && pClassInfo->getBonusMintedPercent(i) != iDefault)
		{
			if ( NULL == m_piBonusMintedPercent )
			{
				CvXMLLoadUtility::InitList(&m_piBonusMintedPercent,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusMintedPercent[i] = pClassInfo->getBonusMintedPercent(i);
		}
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getBonusCommerceModifier(i,j) == iDefault && pClassInfo->getBonusCommerceModifier(i,j) != iDefault)
			{
				if ( NULL == m_ppiBonusCommerceModifier )
				{
					pXML->Init2DIntList(&m_ppiBonusCommerceModifier, GC.getNumBonusInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiBonusCommerceModifier[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiBonusCommerceModifier[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiBonusCommerceModifier[i][j] = pClassInfo->getBonusCommerceModifier(i,j);
			}
		}
	}
	for ( int i = 0; i < pClassInfo->getCivicAttitudeVectorSize(); i++ )
	{
		m_aiCivicAttitudeforPass3.push_back(pClassInfo->getCivicAttitudeValuesVectorElement(i));
		m_aszCivicAttitudeforPass3.push_back(pClassInfo->getCivicAttitudeNamesVectorElement(i));
	}

/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
//Team Project (3)
	//TB Civic Tags
	//Integers
	if (getNationalCaptureProbabilityModifier() == iDefault) m_iNationalCaptureProbabilityModifier = pClassInfo->getNationalCaptureProbabilityModifier();
	if (getNationalCaptureResistanceModifier() == iDefault) m_iNationalCaptureResistanceModifier = pClassInfo->getNationalCaptureResistanceModifier();
//Team Project (5)
	//Booleans
	if (isAllReligionsActive() == bDefault) m_bAllReligionsActive = pClassInfo->isAllReligionsActive();
	if (isBansNonStateReligions() == bDefault) m_bBansNonStateReligions = pClassInfo->isBansNonStateReligions();
	//
	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

CvPropertyManipulators* CvCivicInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}


//======================================================================================================
//					CvDiplomacyInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvDiplomacyInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyInfo::CvDiplomacyInfo()
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvDiplomacyInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvDiplomacyInfo::~CvDiplomacyInfo()
{
	uninit();
}

// note - Response member vars allocated by CvXmlLoadUtility
void CvDiplomacyInfo::uninit()
{
	for (std::vector<CvDiplomacyResponse*>::iterator it = m_pResponses.begin(); it != m_pResponses.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	m_pResponses.clear();
}

const CvDiplomacyResponse& CvDiplomacyInfo::getResponse(int iNum) const
{
	return *(m_pResponses[iNum]);
}

int CvDiplomacyInfo::getNumResponses() const
{
	return m_pResponses.size();
}

bool CvDiplomacyInfo::getCivilizationTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumCivilizationInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getCivilizationTypes(j);
}

bool CvDiplomacyInfo::getLeaderHeadTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getLeaderHeadTypes(j);
}

bool CvDiplomacyInfo::getAttitudeTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getAttitudeTypes(j);
}

bool CvDiplomacyInfo::getDiplomacyPowerTypes(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_DIPLOMACYPOWER_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getDiplomacyPowerTypes(j);
}

int CvDiplomacyInfo::getNumDiplomacyText(int i) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pResponses[i]->getNumDiplomacyText();
}

const TCHAR* CvDiplomacyInfo::getDiplomacyText(int i, int j) const
{
	FAssertMsg(i < getNumResponses(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < getNumDiplomacyText(i), "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");
	return m_pResponses[i]->getDiplomacyText(j);
}

bool CvDiplomacyInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	int i;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	uninit();
	if ( pXML->TryMoveToXmlFirstChild(L"Responses") )
	{
		int iNewResponses = pXML->GetXmlChildrenNumber(L"Response");

		pXML->TryMoveToXmlFirstChild();

		for (i = 0; i < iNewResponses; i++)
		{
			CvDiplomacyResponse* pNewResponse = new CvDiplomacyResponse;
			pNewResponse->read(pXML);
			m_pResponses.push_back(pNewResponse);

			if (!pXML->TryMoveToXmlNextSibling())
			{
				break;
			}
		}
		pXML->MoveToXmlParent();
	}
	pXML->MoveToXmlParent();

	return true;
}
void CvDiplomacyInfo::copyNonDefaults(CvXMLLoadUtility* pXML)
{
	// We have 6 possibilities in civ what this might be
	// 1) Text
	// 2) Leadertype + Text
	// 3) Attitude + Text
	// 4) Power + Text
	// 5) Leadertype + Attitude + Text
	// 6) Leadertype + Power + Text
	// Civilizations aren't used thus far...No idea why firaxis added this to the XML
	int iCase = 0; //no Case

	// Read the New Responses
	if ( pXML->TryMoveToXmlFirstChild(L"Responses") )
	{
		int iNewResponses = pXML->GetXmlChildrenNumber(L"Response");

		pXML->TryMoveToXmlFirstChild();

		for (int i = 0; i < iNewResponses; i++)
		{
			CvDiplomacyResponse* pNewResponse = new CvDiplomacyResponse;
			pNewResponse->read(pXML);
			int iResponses = m_pResponses.size();

			// Check if the new
			bool bLeaderHeadTypes = false;
//			bool bLeaderHeadTypesExist = false;
			for ( int j = 0; j < GC.getNumLeaderHeadInfos(); j++ )
			{
				if (pNewResponse->getLeaderHeadTypes(j))
				{
					bLeaderHeadTypes = true;
/*					for ( int jj = 0; jj < iResponses; ++jj)
					{
						if ( getLeaderHeadTypes(jj, j) )
						{
							bLeaderHeadTypesExist = true;
							break;
						}
					}*/
					break;
				}
			}
			bool bAttitudeTypes = false;
//			bool bAttitudeTypesExist = false;
			for ( int j = 0; j < NUM_ATTITUDE_TYPES; j++ )
			{
				if (pNewResponse->getAttitudeTypes(j))
				{
					bAttitudeTypes = true;
/*					for ( int jj = 0; jj < iResponses; ++jj)
					{
						if ( getAttitudeTypes(jj, j) )
						{
							bAttitudeTypesExist = true;
							break;
						}
					}*/
					break;
				}
			}
			bool bPowerTypes = false;
//			bool bPowerTypesExist = false;
			for ( int j = 0; j < NUM_DIPLOMACYPOWER_TYPES; j++ )
			{
				if (pNewResponse->getDiplomacyPowerTypes(j))
				{
					bPowerTypes = true;
/*					for ( int jj = 0; jj < iResponses; ++jj)
					{
						if ( getDiplomacyPowerTypes(jj, j) )
						{
							bPowerTypesExist = true;
							break;
						}
					}*/
					break;
				}
			}

			// Check which case we have
//			if ( bLeaderHeadTypes && !bLeaderHeadTypesExist) FAssertMsg(false, L"Wrong XML format!");
//			if ( bAttitudeTypes && !bAttitudeTypesExist) FAssertMsg(false, L"Wrong XML format!");
//			if ( bPowerTypes && !bPowerTypesExist) FAssertMsg(false, L"Wrong XML format!");
			if ( !(bLeaderHeadTypes || bAttitudeTypes || bPowerTypes) ) iCase = 1;
			else if ( bLeaderHeadTypes && !( bAttitudeTypes || bPowerTypes) ) iCase = 2;
			else if ( bAttitudeTypes && !( bLeaderHeadTypes || bPowerTypes) ) iCase = 3;
			else if ( bPowerTypes && !( bLeaderHeadTypes || bAttitudeTypes) ) iCase = 4;
			else if ( bLeaderHeadTypes && bAttitudeTypes && !bPowerTypes ) iCase = 5;
			else if ( bLeaderHeadTypes && bPowerTypes && !bAttitudeTypes ) iCase = 6;
			else FAssertMsg(false, "A new case with an added Diplomacy XML but already exists?" );

			// Find ResponseIndex(if already exists)
			int iIndex = 0;
			bool bIsIndex = FindResponseIndex(pNewResponse, iCase, &iIndex);

			if (bIsIndex)
			{
				// Check the Previous Values of this class and copy them to the Diplomacies Response
				pNewResponse->UpdateDiplomacies(this, iIndex);

				//Delete old vector info(only for this response)
				//DeleteResponseOnly(m_pResponses, getResponse(iIndex));
				m_pResponses.erase(m_pResponses.begin() + iIndex);
			}

			//apply new vector info
			m_pResponses.push_back(pNewResponse);

			if (!pXML->TryMoveToXmlNextSibling())
			{
				break;
			}
		}
		pXML->MoveToXmlParent();
	}
	pXML->MoveToXmlParent();
}
bool CvDiplomacyInfo::FindResponseIndex(CvDiplomacyResponse* pNewResponse, int iCase, int* iIndex)
{
	// We have 6 possibilities in civ what this might be
	// 1) Text
	// 2) Leadertype + Text
	// 3) Attitude + Text
	// 4) Power + Text
	// 5) Leadertype + Attitude + Text
	// 6) Leadertype + Power + Text

	if ( getNumResponses() == 1 )
	{
		iIndex = 0;
		return true;
	}

	// Text
	if ( iCase == 1 )
	{
		bool bOnlyText = true;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) ) bOnlyText = false;
			}
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) ) bOnlyText = false;
			}
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) ) bOnlyText = false;
			}
			if ( bOnlyText )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Leaderheads + Text
	if ( iCase == 2 )
	{
		bool bOnlyLeaderheads = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) && pNewResponse->getLeaderHeadTypes(iElement) ) bOnlyLeaderheads = true;
			}
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) ) bOnlyLeaderheads = false;
			}
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) ) bOnlyLeaderheads = false;
			}
			if ( bOnlyLeaderheads )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Attitude + Text
	if ( iCase == 3 )
	{
		bool bOnlyAttitudeTypes = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) && pNewResponse->getAttitudeTypes(iElement)) bOnlyAttitudeTypes = true;
			}
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) ) bOnlyAttitudeTypes = false;
			}
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) ) bOnlyAttitudeTypes = false;
			}
			if ( bOnlyAttitudeTypes )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Power + Text
	if ( iCase == 4 )
	{
		bool bOnlyDiplomacyPowerTypes = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) && pNewResponse->getDiplomacyPowerTypes(iElement) ) bOnlyDiplomacyPowerTypes = true;
			}
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) ) bOnlyDiplomacyPowerTypes = false;
			}
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) ) bOnlyDiplomacyPowerTypes = false;
			}

			if ( bOnlyDiplomacyPowerTypes )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Leadertype + Attitude + Text
	if ( iCase == 5 )
	{
		bool bLeaderAttitudeTypes = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) && pNewResponse->getLeaderHeadTypes(iElement) )
				{
					for ( int iiElement = 0; iiElement < NUM_ATTITUDE_TYPES; ++iiElement )
					{
						if ( getAttitudeTypes(i, iiElement) && pNewResponse->getAttitudeTypes(iiElement) ) bLeaderAttitudeTypes = true;
					}
				}
			}
			for ( int iElement = 0; iElement < NUM_DIPLOMACYPOWER_TYPES; ++iElement )
			{
				if ( getDiplomacyPowerTypes(i, iElement) ) bLeaderAttitudeTypes = false;
			}

			if ( bLeaderAttitudeTypes )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	// Leadertype + Power + Text
	if ( iCase == 6 )
	{
		bool bLeaderDiplomacyPowerTypes = false;

		for ( int i = 0; i < getNumResponses(); ++i )
		{
			for ( int iElement = 0; iElement < GC.getNumLeaderHeadInfos(); ++iElement )
			{
				if ( getLeaderHeadTypes(i, iElement) && pNewResponse->getLeaderHeadTypes(iElement) )
				{
					for ( int iiElement = 0; iiElement < NUM_DIPLOMACYPOWER_TYPES; ++iiElement )
					{
						if ( getDiplomacyPowerTypes(i, iiElement) && pNewResponse->getDiplomacyPowerTypes(iiElement) ) bLeaderDiplomacyPowerTypes = true;
					}
				}
			}
			for ( int iElement = 0; iElement < NUM_ATTITUDE_TYPES; ++iElement )
			{
				if ( getAttitudeTypes(i, iElement) ) bLeaderDiplomacyPowerTypes = false;
			}

			if ( bLeaderDiplomacyPowerTypes )
			{
				*iIndex = i;
				return true;
			}
		}
	}

	return false;  //This means there was no index, hence it's a new one and needs to be added to the vector
}

//======================================================================================================
//					CvCivilizationInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivilizationInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivilizationInfo::CvCivilizationInfo():
m_iDefaultPlayerColor(NO_PLAYERCOLOR),
m_iArtStyleType(NO_ARTSTYLE),
m_iUnitArtStyleType(NO_UNIT_ARTSTYLE),
m_iNumCityNames(0),
m_iNumLeaders(0),
m_iSelectionSoundScriptId(0),
m_iActionSoundScriptId(0),
m_iDerivativeCiv(NO_CIVILIZATION),
m_bPlayable(false),
m_bAIPlayable(false),
m_piCivilizationBuildings(NULL),
m_piCivilizationUnits(NULL),
m_piCivilizationFreeUnitsClass(NULL),
m_piCivilizationInitialCivics(NULL),
m_pbLeaders(NULL),
m_pbCivilizationFreeBuildingClass(NULL),
m_pbCivilizationFreeTechs(NULL),
m_pbCivilizationDisableTechs(NULL),
m_paszCityNames(NULL),
m_iSpawnRateModifier(0),
m_iSpawnRateNPCPeaceModifier(0),
m_bStronglyRestricted(false)
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivilizationInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivilizationInfo::~CvCivilizationInfo()
{
	SAFE_DELETE_ARRAY(m_piCivilizationBuildings);
	SAFE_DELETE_ARRAY(m_piCivilizationUnits);
	SAFE_DELETE_ARRAY(m_piCivilizationFreeUnitsClass);
	SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
	SAFE_DELETE_ARRAY(m_pbLeaders);
	SAFE_DELETE_ARRAY(m_pbCivilizationFreeBuildingClass);
	SAFE_DELETE_ARRAY(m_pbCivilizationFreeTechs);
	SAFE_DELETE_ARRAY(m_pbCivilizationDisableTechs);
	SAFE_DELETE_ARRAY(m_paszCityNames);
}

void CvCivilizationInfo::reset()
{
	CvInfoBase::reset();
	m_aszAdjective.clear();
	m_aszShortDescription.clear();
}


int CvCivilizationInfo::getDefaultPlayerColor() const
{
	return m_iDefaultPlayerColor;
}

int CvCivilizationInfo::getArtStyleType() const
{
	return m_iArtStyleType;
}

int CvCivilizationInfo::getUnitArtStyleType() const
{
	return m_iUnitArtStyleType;
}

int CvCivilizationInfo::getNumCityNames() const
{
	return m_iNumCityNames;
}

int CvCivilizationInfo::getNumLeaders() const// the number of leaders the Civ has, this is needed so that random leaders can be generated easily
{
	return m_iNumLeaders;
}

int CvCivilizationInfo::getSelectionSoundScriptId() const
{
	return m_iSelectionSoundScriptId;
}

int CvCivilizationInfo::getActionSoundScriptId() const
{
	return m_iActionSoundScriptId;
}

bool CvCivilizationInfo::isAIPlayable() const
{
	return m_bAIPlayable;
}

bool CvCivilizationInfo::isPlayable() const
{
	return m_bPlayable;
}

const wchar* CvCivilizationInfo::getShortDescription(uint uiForm)
{
	while(m_aszShortDescription.size() <= uiForm)
	{
		m_aszShortDescription.push_back(gDLL->getObjectText(m_szShortDescriptionKey, m_aszShortDescription.size()));
	}

	return m_aszShortDescription[uiForm];
}

const wchar* CvCivilizationInfo::getShortDescriptionKey() const
{
	return m_szShortDescriptionKey;
}

const wchar* CvCivilizationInfo::getAdjective(uint uiForm)
{
	while(m_aszAdjective.size() <= uiForm)
	{
		m_aszAdjective.push_back(gDLL->getObjectText(m_szAdjectiveKey, m_aszAdjective.size()));
	}

	return m_aszAdjective[uiForm];
}

const wchar* CvCivilizationInfo::getAdjectiveKey() const
{
	return m_szAdjectiveKey;
}

const TCHAR* CvCivilizationInfo::getFlagTexture() const
{
	return ARTFILEMGR.getCivilizationArtInfo( getArtDefineTag() )->getPath();
}

const TCHAR* CvCivilizationInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

void CvCivilizationInfo::setArtDefineTag(const TCHAR* szVal)
{
	m_szArtDefineTag = szVal;
}

// Arrays

int CvCivilizationInfo::getCivilizationBuildings(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCivilizationBuildings ? m_piCivilizationBuildings[i] : -1;
}

int CvCivilizationInfo::getCivilizationUnits(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCivilizationUnits ? m_piCivilizationUnits[i] : -1;
}

int CvCivilizationInfo::getCivilizationFreeUnitsClass(int i) const
{
	FAssertMsg(i < GC.getNumUnitClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCivilizationFreeUnitsClass ? m_piCivilizationFreeUnitsClass[i] : 0;
}

int CvCivilizationInfo::getCivilizationInitialCivics(int i) const
{
	FAssertMsg(i < GC.getNumCivicOptionInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return (m_piCivilizationInitialCivics ? m_piCivilizationInitialCivics[i] : -1);
}
/************************************************************************************************/
/* Afforess					  Start		 04/05/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvCivilizationInfo::setCivilizationInitialCivics(int iCivicOption, int iCivic)
{
	FAssertMsg(iCivicOption < GC.getNumCivicOptionInfos(), "Index out of bounds");
	FAssertMsg(iCivicOption > -1, "Index out of bounds");
	FAssertMsg(iCivic < GC.getNumCivicInfos(), "Index out of bounds");
	FAssertMsg(iCivic > -1, "Index out of bounds");

	if ( NULL == m_piCivilizationInitialCivics )
	{
		CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics,GC.getNumCivicOptionInfos(),-1);
	}

	m_piCivilizationInitialCivics[iCivicOption] = iCivic;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
bool CvCivilizationInfo::isLeaders(int i) const
{
	FAssertMsg(i < GC.getNumLeaderHeadInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbLeaders ? m_pbLeaders[i] : false;
}

bool CvCivilizationInfo::isCivilizationFreeBuildingClass(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbCivilizationFreeBuildingClass ? m_pbCivilizationFreeBuildingClass[i] : false;
}

bool CvCivilizationInfo::isCivilizationFreeTechs(int i) const
{
	FAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbCivilizationFreeTechs ? m_pbCivilizationFreeTechs[i] : false;
}

bool CvCivilizationInfo::isCivilizationDisableTechs(int i) const
{
	FAssertMsg(i < GC.getNumTechInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbCivilizationDisableTechs ? m_pbCivilizationDisableTechs[i] : false;
}

const CvArtInfoCivilization* CvCivilizationInfo::getArtInfo() const
{
	return ARTFILEMGR.getCivilizationArtInfo( getArtDefineTag() );
}

/************************************************************************************************/
/* XMLCOPY								 10/25/07								MRGENIE	  */
/*																							  */
/* Catch non-existing tag																	   */
/************************************************************************************************/
/*
const TCHAR* CvCivilizationInfo::getButton() const
{
	return getArtInfo()->getButton();
}
*/
const TCHAR* CvCivilizationInfo::getButton() const
{
	CvString cDefault = CvString::format("").GetCString();
	if (getArtDefineTag() == cDefault)
	{
		return NULL;
	}
	const CvArtInfoCivilization * pArtInfoCivilization;
	pArtInfoCivilization = getArtInfo();
	if (pArtInfoCivilization != NULL)
	{
		return getArtInfo()->getButton();
	}
	return NULL;
}

std::string CvCivilizationInfo::getCityNames(int i) const
{
	FAssertMsg(i < getNumCityNames(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paszCityNames[i];
}

int CvCivilizationInfo::getDerivativeCiv() const
{
	return m_iDerivativeCiv;
}
void CvCivilizationInfo::setDerivativeCiv(int iCiv)
{
	m_iDerivativeCiv = iCiv;
}

//TB Tags

int CvCivilizationInfo::getSpawnRateModifier() const
{
	return m_iSpawnRateModifier;
}

int CvCivilizationInfo::getSpawnRateNPCPeaceModifier() const
{
	return m_iSpawnRateNPCPeaceModifier;
}

bool CvCivilizationInfo::isStronglyRestricted() const
{
	return m_bStronglyRestricted;
}

void CvCivilizationInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iDerivativeCiv);
	CheckSum(iSum, m_bAIPlayable);
	CheckSum(iSum, m_bPlayable);

	// Arrays
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_piCivilizationBuildings);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_piCivilizationUnits);
	CheckSumI(iSum, GC.getNumUnitClassInfos(), m_piCivilizationFreeUnitsClass);
	CheckSumI(iSum, GC.getNumCivicOptionInfos(), m_piCivilizationInitialCivics);
	CheckSumI(iSum, GC.getNumLeaderHeadInfos(), m_pbLeaders);
	CheckSumI(iSum, GC.getNumBuildingClassInfos(), m_pbCivilizationFreeBuildingClass);
	CheckSumI(iSum, GC.getNumTechInfos(), m_pbCivilizationFreeTechs);
	CheckSumI(iSum, GC.getNumTechInfos(), m_pbCivilizationDisableTechs);

	//TB Tags
	CheckSum(iSum, m_iSpawnRateModifier);
	CheckSum(iSum, m_iSpawnRateNPCPeaceModifier);
	CheckSum(iSum, m_bStronglyRestricted);
}

bool CvCivilizationInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	char szClassVal[256];					// holds the text value of the relevant classinfo

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	int j, iNumSibs;

	pXML->GetOptionalChildXmlValByName(m_szShortDescriptionKey, L"ShortDescription");
	// Get the Text from Text/Civ4GameTextXML.xml

	pXML->GetOptionalChildXmlValByName(m_szAdjectiveKey, L"Adjective");
	// Get the Text from Text/Civ4GameTextXML.xml

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DefaultPlayerColor");
	m_iDefaultPlayerColor = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ArtDefineTag");
	setArtDefineTag(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ArtStyleType");
	m_iArtStyleType = GC.getTypesEnum(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"UnitArtStyleType");
	m_iUnitArtStyleType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationSelectionSound");
	m_iSelectionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;
	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationActionSound");
	m_iActionSoundScriptId = (szTextVal.GetLength() > 0) ? gDLL->getAudioTagIndex( szTextVal.GetCString(), AUDIOTAG_3DSCRIPT ) : -1;

	// set the current xml node to it's next sibling and then
	pXML->GetOptionalChildXmlValByName(&m_bPlayable, L"bPlayable");
	pXML->GetOptionalChildXmlValByName(&m_bAIPlayable, L"bAIPlayable");

	if (pXML->TryMoveToXmlFirstChild(L"Cities"))
	{
		pXML->SetStringList(&m_paszCityNames, &m_iNumCityNames);
		pXML->MoveToXmlParent();
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"Buildings"))
	{
		// call the function that sets the default civilization buildings
		pXML->InitBuildingDefaults(&m_piCivilizationBuildings);
		// get the total number of children the current xml node has
		iNumSibs = pXML->GetXmlChildrenNumber();
		// if the call to the function that sets the current xml node to it's first non-comment
		// child and sets the parameter with the new node's value succeeds
		if ( (0 < iNumSibs) && (pXML->TryMoveToXmlFirstChild()) )
		{
			int iBuildingClassIndex;

			FAssertMsg((iNumSibs <= GC.getNumBuildingClassInfos()) ,"In SetGlobalCivilizationInfo iNumSibs is greater than GC.getNumBuildingClassInfos()");

			// loop through all the siblings
			for (j=0;j<iNumSibs;j++)
			{
				if (pXML->GetChildXmlVal(szClassVal))
				{
					// get the index into the array based on the building class type
					iBuildingClassIndex = pXML->GetInfoClass(szClassVal);
					if (-1 < iBuildingClassIndex)
					{
						// get the next value which should be the building type to set this civilization's version of this building class too
						pXML->GetNextXmlVal( szTextVal);
						// call the find in list function to return either -1 if no value is found
						// or the index in the list the match is found at
						m_piCivilizationBuildings[iBuildingClassIndex] = pXML->GetInfoClass(szTextVal);
					}
/************************************************************************************************/
/* Afforess					  Start		 03/21/10											   */
/*																							  */
/*  Hide Assert for Deleted Objects																	 */
/************************************************************************************************/
					else if (!GC.getDefineINT(szClassVal))
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

					{
						FAssertMsg(0,"BuildingClass index is -1 in SetGlobalCivilizationInfo function");
					}

					// set the current xml node to it's parent node
					pXML->MoveToXmlParent();
				}

				// if the call to the function that sets the current xml node to it's first non-comment
				// sibling and sets the parameter with the new node's value does not succeed
				// we will break out of this for loop
				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}
/************************************************************************************************/
/* XMLCOPY								 10/09/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	else
	{
		pXML->InitBuildingDefaults(&m_piCivilizationBuildings);
		for ( j = 0; j < GC.getNumBuildingClassInfos(); j++)
		{
			m_piCivilizationBuildings[j] = GC.getBuildingClassInfo((BuildingClassTypes) j).getDefaultBuildingIndex();
		}
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"Units"))
	{
			// call the function that sets the default civilization buildings
			pXML->InitUnitDefaults(&m_piCivilizationUnits);
			// get the total number of children the current xml node has
			iNumSibs = pXML->GetXmlChildrenNumber();
			// if the call to the function that sets the current xml node to it's first non-comment
			// child and sets the parameter with the new node's value succeeds
			if ( (0 < iNumSibs) && (pXML->TryMoveToXmlFirstChild()) )
			{
				int iUnitClassIndex;

				FAssertMsg((iNumSibs <= GC.getNumUnitClassInfos()),"In SetGlobalCivilizationInfo iNumSibs is greater than GC.getNumUnitClassInfos()");

				// loop through all the siblings
				for (j=0;j<iNumSibs;j++)
				{
					if (pXML->GetChildXmlVal(szClassVal))
					{
						// set the unit class index
						iUnitClassIndex = pXML->GetInfoClass(szClassVal);
						if (-1 < iUnitClassIndex)
						{
							// get the next value which should be the building type to set this civilization's version of this building class too
							pXML->GetNextXmlVal( szTextVal);
							// call the find in list function to return either -1 if no value is found
							// or the index in the list the match is found at
							m_piCivilizationUnits[iUnitClassIndex] = pXML->GetInfoClass(szTextVal);
						}
						else
						{
							if (!GC.getDefineINT(szClassVal))
							{
								FAssertMsg(0, "UnitClass index is -1 in SetGlobalCivilizationInfo function");
							}
						}

						// set the current xml node to it's parent node
						pXML->MoveToXmlParent();
					}

					// if the call to the function that sets the current xml node to it's first non-comment
					// sibling and sets the parameter with the new node's value does not succeed
					// we will break out of this for loop
					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}

				// set the current xml node to it's parent node
				pXML->MoveToXmlParent();
			}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}
/************************************************************************************************/
/* XMLCOPY								 10/09/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	else
	{
		pXML->InitUnitDefaults(&m_piCivilizationUnits);
		for ( j = 0; j < GC.getNumUnitClassInfos(); j++)
		{
			m_piCivilizationUnits[j] = GC.getUnitClassInfo((UnitClassTypes) j).getDefaultUnitIndex();
		}
	}

	pXML->SetVariableListTagPair(&m_piCivilizationFreeUnitsClass, L"FreeUnitClasses", GC.getNumUnitClassInfos());

	pXML->SetVariableListTagPair(&m_pbCivilizationFreeBuildingClass, L"FreeBuildingClasses",  GC.getNumBuildingClassInfos());

	pXML->SetVariableListTagPair(&m_pbCivilizationFreeTechs, L"FreeTechs", GC.getNumTechInfos());
	pXML->SetVariableListTagPair(&m_pbCivilizationDisableTechs, L"DisableTechs", GC.getNumTechInfos());

	if (pXML->TryMoveToXmlFirstChild(L"InitialCivics"))
	{
			iNumSibs = pXML->GetXmlChildrenNumber();

			if (0 < iNumSibs)
			{
				pXML->CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics, GC.getNumCivicOptionInfos());
				if (pXML->GetChildXmlVal(szTextVal))
				{
					for (j=0;j<iNumSibs;j++)
					{
						CivicTypes eCivic = (CivicTypes)pXML->GetInfoClass(szTextVal);//, true);
						if ( eCivic != NO_CIVIC )
						{
							CvCivicInfo& kCivic = GC.getCivicInfo(eCivic);
							CivicOptionTypes eCivicOption = (CivicOptionTypes)kCivic.getCivicOptionType();

							if ( eCivicOption != NO_CIVICOPTION )
							{
								FAssertMsg((eCivicOption < GC.getNumCivicOptionInfos()),"Bad default civic");
								m_piCivilizationInitialCivics[eCivicOption] = eCivic;
							}
						}

						if (!pXML->GetNextXmlVal(szTextVal))
						{
							break;
						}
					}

					pXML->MoveToXmlParent();
				}
			}
			else
			{
				SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
			}

		pXML->MoveToXmlParent();
	}
/************************************************************************************************/
/* XMLCOPY								 10/09/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	else
	{
		SAFE_DELETE_ARRAY(m_piCivilizationInitialCivics);
	}

	pXML->SetVariableListTagPair(&m_pbLeaders, L"Leaders", GC.getNumLeaderHeadInfos());

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivilizationSelectionSound");

	//TB Tags
	pXML->GetOptionalChildXmlValByName(&m_iSpawnRateModifier, L"iSpawnRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iSpawnRateNPCPeaceModifier, L"iSpawnRateNPCPeaceModifier");
	pXML->GetOptionalChildXmlValByName(&m_bStronglyRestricted, L"bStronglyRestricted");

	return true;
}

bool CvCivilizationInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

/************************************************************************************************/
/* XMLCOPY								 10/13/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}
	pXML->GetOptionalChildXmlValByName(szTextVal, L"DerivativeCiv");
	m_iDerivativeCiv = GC.getInfoTypeForString(szTextVal);

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 10/12/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvCivilizationInfo::copyNonDefaults(CvCivilizationInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	int iDefault = 0;
	int iTextDefault = -1;
	bool bDefault = false;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	// must be before we set the InfoBaseClass else it can't find the button to to corresponding arttag
	if ( getArtDefineTag() == cDefault ) // "ArtDefineTag"
	{
		setArtDefineTag(pClassInfo->getArtDefineTag());
	}

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if ( isPlayable() == bDefault ) // "bPlayable"
	{
		m_bPlayable = (pClassInfo->isPlayable());
	}
	if ( isAIPlayable() == bDefault ) // "bAIPlayable"
	{
		m_bAIPlayable = (pClassInfo->isAIPlayable());
	}

	if ( getShortDescriptionKey() == wDefault )
	{
		m_szShortDescriptionKey = pClassInfo->getShortDescriptionKey();
	}

	if ( getAdjectiveKey() == wDefault ) // "Adjective"
	{
		m_szAdjectiveKey = pClassInfo->getAdjectiveKey();
	}

	if ( getDefaultPlayerColor() == NO_PLAYERCOLOR ) // "DefaultPlayerColor"
	{
		m_iDefaultPlayerColor = (pClassInfo->getDefaultPlayerColor());
	}
	if ( getArtStyleType() == NO_ARTSTYLE ) // "ArtStyleType"
	{
		m_iArtStyleType = (pClassInfo->getArtStyleType());
	}
	if ( getUnitArtStyleType() == NO_UNIT_ARTSTYLE ) // "UnitArtStyleType"
	{
		m_iUnitArtStyleType = (pClassInfo->getUnitArtStyleType());
	}
	if ( getSelectionSoundScriptId() == AUDIOTAG_NONE ) // "CivilizationSelectionSound"
	{
		m_iSelectionSoundScriptId = (pClassInfo->getSelectionSoundScriptId());
	}
	if ( getActionSoundScriptId() == AUDIOTAG_NONE ) // "CivilizationActionSound"
	{
		m_iActionSoundScriptId = (pClassInfo->getActionSoundScriptId());
	}

	for ( int i = 0; i < GC.getNumBuildingClassInfos(); i++)
	{
		if ( getCivilizationBuildings(i) == GC.getBuildingClassInfo((BuildingClassTypes) i).getDefaultBuildingIndex())	// "Buildings"
		{
			m_piCivilizationBuildings[i] = pClassInfo->getCivilizationBuildings(i);
		}
		if ( isCivilizationFreeBuildingClass(i) == bDefault && pClassInfo->isCivilizationFreeBuildingClass(i) != bDefault)
		{
			if ( NULL == m_pbCivilizationFreeBuildingClass )
			{
				CvXMLLoadUtility::InitList(&m_pbCivilizationFreeBuildingClass,GC.getNumBuildingClassInfos(),bDefault);
			}
			m_pbCivilizationFreeBuildingClass[i] = pClassInfo->isCivilizationFreeBuildingClass(i);
		}
	}

	for ( int i = 0; i < GC.getNumUnitClassInfos(); i++)
	{
		if ( getCivilizationUnits(i) == GC.getUnitClassInfo((UnitClassTypes) i).getDefaultUnitIndex())	// "Units"
		{
			m_piCivilizationUnits[i] = pClassInfo->getCivilizationUnits(i);
		}
		if ( getCivilizationFreeUnitsClass(i) == iDefault && pClassInfo->getCivilizationFreeUnitsClass(i) != iDefault)
		{
			if ( NULL == m_piCivilizationFreeUnitsClass )
			{
				CvXMLLoadUtility::InitList(&m_piCivilizationFreeUnitsClass,GC.getNumUnitClassInfos(),iDefault);
			}
			m_piCivilizationFreeUnitsClass[i] = pClassInfo->getCivilizationFreeUnitsClass(i);
		}
	}

	for ( int i = 0; i < GC.getNumTechInfos(); i++)
	{
		if ( isCivilizationFreeTechs(i) == bDefault && pClassInfo->isCivilizationFreeTechs(i) != bDefault)
		{
			if ( NULL == m_pbCivilizationFreeTechs )
			{
				CvXMLLoadUtility::InitList(&m_pbCivilizationFreeTechs,GC.getNumTechInfos(),bDefault);
			}
			m_pbCivilizationFreeTechs[i] = pClassInfo->isCivilizationFreeTechs(i);
		}
		if ( isCivilizationDisableTechs(i) == bDefault && pClassInfo->isCivilizationDisableTechs(i) != bDefault)
		{
			if ( NULL == m_pbCivilizationDisableTechs )
			{
				CvXMLLoadUtility::InitList(&m_pbCivilizationDisableTechs,GC.getNumTechInfos(),bDefault);
			}
			m_pbCivilizationDisableTechs[i] = pClassInfo->isCivilizationDisableTechs(i);
		}
	}

	for ( int i = 0; i < GC.getNumCivicOptionInfos(); i++)
	{
		if ( getCivilizationInitialCivics(i) == -1 && pClassInfo->getCivilizationInitialCivics(i) != -1 )
		{
			if ( NULL == m_pbCivilizationDisableTechs )
			{
				CvXMLLoadUtility::InitList(&m_piCivilizationInitialCivics,GC.getNumCivicOptionInfos(),-1);
			}
			m_piCivilizationInitialCivics[i] = pClassInfo->getCivilizationInitialCivics(i);
		}
	}

	for ( int i = 0; i < GC.getNumLeaderHeadInfos(); i++)
	{
		if ( isLeaders(i) == bDefault && pClassInfo->isLeaders(i) != bDefault)
		{
			if ( NULL == m_pbLeaders )
			{
				CvXMLLoadUtility::InitList(&m_pbLeaders,GC.getNumLeaderHeadInfos(),bDefault);
			}
			m_pbLeaders[i] = pClassInfo->isLeaders(i);
		}
	}

	//TB Tags
	//int
	if ( getSpawnRateModifier() == iDefault )
	{
		m_iSpawnRateModifier = (pClassInfo->getSpawnRateModifier());
	}
	if ( getSpawnRateNPCPeaceModifier() == iDefault )
	{
		m_iSpawnRateNPCPeaceModifier = (pClassInfo->getSpawnRateNPCPeaceModifier());
	}
	//bool
	if ( isStronglyRestricted() == bDefault )
	{
		m_bStronglyRestricted = (pClassInfo->isStronglyRestricted());
	}

	// Readpass2 stuff
	if (getDerivativeCiv() == iTextDefault) m_iDerivativeCiv = pClassInfo->getDerivativeCiv();
	// First we check if there are different Unique Names in the Modules(we want to keep all of them)
	// So we have to set the Arraysize properly, knowing the amount of Unique Names
	if ( pClassInfo->getNumCityNames() != 0 )
	{
		CvString* m_paszOldNames = new CvString[pClassInfo->getNumCityNames()];
		for ( int i = 0; i < pClassInfo->getNumCityNames(); i++)
		{
			m_paszOldNames[i] = pClassInfo->getCityNames(i);
		}

		CvXMLLoadUtilityModTools* pCurrentUnitClass = new CvXMLLoadUtilityModTools;
		pCurrentUnitClass->StringArrayExtend(&m_paszCityNames, &m_iNumCityNames,
										 &m_paszOldNames, pClassInfo->getNumCityNames());
		delete pCurrentUnitClass;
		SAFE_DELETE_ARRAY(m_paszOldNames)
	}
}

void CvCivilizationInfo::copyNonDefaultsReadPass2(CvCivilizationInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
	int iTextDefault = -1;
	if (bOver || getDerivativeCiv() == iTextDefault) m_iDerivativeCiv = pClassInfo->getDerivativeCiv();
}

//======================================================================================================
//					CvLeaderHeadInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvLeaderHeadInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvLeaderHeadInfo::CvLeaderHeadInfo() :
m_bNPC(false),
m_iWonderConstructRand(0),
m_iBaseAttitude(0),
m_iBasePeaceWeight(0),
m_iPeaceWeightRand(0),
m_iWarmongerRespect(0),
m_iEspionageWeight(0),
m_iRefuseToTalkWarThreshold(0),
m_iNoTechTradeThreshold(0),
m_iTechTradeKnownPercent(0),
m_iMaxGoldTradePercent(0),
m_iMaxGoldPerTurnTradePercent(0),
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  03/21/10								jdog5000	  */
/*																							  */
/* Victory Strategy AI																		  */
/************************************************************************************************/
m_iCultureVictoryWeight(0),
m_iSpaceVictoryWeight(0),
m_iConquestVictoryWeight(0),
m_iDominationVictoryWeight(0),
m_iDiplomacyVictoryWeight(0),
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/
m_iMaxWarRand(0),
m_iMaxWarNearbyPowerRatio(0),
m_iMaxWarDistantPowerRatio(0),
m_iMaxWarMinAdjacentLandPercent(0),
m_iLimitedWarRand(0),
m_iLimitedWarPowerRatio(0),
m_iDogpileWarRand(0),
m_iMakePeaceRand(0),
m_iDeclareWarTradeRand(0),
m_iDemandRebukedSneakProb(0),
m_iDemandRebukedWarProb(0),
m_iRazeCityProb(0),
m_iBuildUnitProb(0),
m_iBaseAttackOddsChange(0),
m_iAttackOddsChangeRand(0),
m_iWorseRankDifferenceAttitudeChange(0),
m_iBetterRankDifferenceAttitudeChange(0),
m_iCloseBordersAttitudeChange(0),
m_iLostWarAttitudeChange(0),
m_iAtWarAttitudeDivisor(0),
m_iAtWarAttitudeChangeLimit(0),
m_iAtPeaceAttitudeDivisor(0),
m_iAtPeaceAttitudeChangeLimit(0),
m_iSameReligionAttitudeChange(0),
m_iSameReligionAttitudeDivisor(0),
m_iSameReligionAttitudeChangeLimit(0),
m_iDifferentReligionAttitudeChange(0),
m_iDifferentReligionAttitudeDivisor(0),
m_iDifferentReligionAttitudeChangeLimit(0),
m_iBonusTradeAttitudeDivisor(0),
m_iBonusTradeAttitudeChangeLimit(0),
m_iOpenBordersAttitudeDivisor(0),
m_iOpenBordersAttitudeChangeLimit(0),
m_iDefensivePactAttitudeDivisor(0),
m_iDefensivePactAttitudeChangeLimit(0),
m_iShareWarAttitudeChange(0),
m_iShareWarAttitudeDivisor(0),
m_iShareWarAttitudeChangeLimit(0),
m_iFavoriteCivicAttitudeChange(0),
m_iFavoriteCivicAttitudeDivisor(0),
m_iFavoriteCivicAttitudeChangeLimit(0),
m_iDemandTributeAttitudeThreshold(NO_ATTITUDE),
m_iNoGiveHelpAttitudeThreshold(NO_ATTITUDE),
m_iTechRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStrategicBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iHappinessBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iHealthBonusRefuseAttitudeThreshold(NO_ATTITUDE),
m_iMapRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDeclareWarRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDeclareWarThemRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStopTradingRefuseAttitudeThreshold(NO_ATTITUDE),
m_iStopTradingThemRefuseAttitudeThreshold(NO_ATTITUDE),
m_iAdoptCivicRefuseAttitudeThreshold(NO_ATTITUDE),
m_iConvertReligionRefuseAttitudeThreshold(NO_ATTITUDE),
m_iOpenBordersRefuseAttitudeThreshold(NO_ATTITUDE),
m_iDefensivePactRefuseAttitudeThreshold(NO_ATTITUDE),
m_iPermanentAllianceRefuseAttitudeThreshold(NO_ATTITUDE),
m_iVassalRefuseAttitudeThreshold(NO_ATTITUDE),
m_iVassalPowerModifier(0),
m_iFreedomAppreciation(0),
m_iFavoriteCivic(NO_CIVIC),
m_iFavoriteReligion(NO_RELIGION),
m_pbTraits(NULL),
m_piFlavorValue(NULL),
m_piContactRand(NULL),
m_piContactDelay(NULL),
m_piMemoryDecayRand(NULL),
m_piMemoryAttitudePercent(NULL),
m_piNoWarAttitudeProb(NULL),
m_piUnitAIWeightModifier(NULL),
m_piImprovementWeightModifier(NULL),
m_piDiploPeaceIntroMusicScriptIds(NULL),
m_piDiploPeaceMusicScriptIds(NULL),
m_piDiploWarIntroMusicScriptIds(NULL),
m_piDiploWarMusicScriptIds(NULL)
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iMilitaryUnitRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
,m_iWorkerRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
,m_iCorporationRefuseAttitudeThreshold(ATTITUDE_CAUTIOUS)
,m_iSecretaryGeneralVoteRefuseAttitudeThreshold(ATTITUDE_ANNOYED)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvLeaderHeadInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvLeaderHeadInfo::~CvLeaderHeadInfo()
{
	SAFE_DELETE_ARRAY(m_pbTraits);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piContactRand);
	SAFE_DELETE_ARRAY(m_piContactDelay);
	SAFE_DELETE_ARRAY(m_piMemoryDecayRand);
	SAFE_DELETE_ARRAY(m_piMemoryAttitudePercent);
	SAFE_DELETE_ARRAY(m_piNoWarAttitudeProb);
	SAFE_DELETE_ARRAY(m_piUnitAIWeightModifier);
	SAFE_DELETE_ARRAY(m_piImprovementWeightModifier);
	SAFE_DELETE_ARRAY(m_piDiploPeaceIntroMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploPeaceMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploWarIntroMusicScriptIds);
	SAFE_DELETE_ARRAY(m_piDiploWarMusicScriptIds);
}

const TCHAR* CvLeaderHeadInfo::getButton() const
{
/************************************************************************************************/
/* XMLCOPY								 10/25/07								MRGENIE	  */
/*																							  */
/* Catch non-existing tag																	   */
/************************************************************************************************/
	CvString cDefault = CvString::format("").GetCString();
	if (getArtDefineTag() == cDefault)
	{
		return NULL;
	}
	const CvArtInfoLeaderhead * pLeaderheadArtInfo;
	pLeaderheadArtInfo = getArtInfo();
	if (pLeaderheadArtInfo != NULL)
	{
		return pLeaderheadArtInfo->getButton();
	}
	else
	{
		return NULL;
	}
}

bool CvLeaderHeadInfo::isNPC() const
{
	return m_bNPC;
}

int CvLeaderHeadInfo::getWonderConstructRand() const
{
	return m_iWonderConstructRand;
}

int CvLeaderHeadInfo::getBaseAttitude() const
{
	return m_iBaseAttitude;
}

int CvLeaderHeadInfo::getBasePeaceWeight() const
{
	return m_iBasePeaceWeight;
}

int CvLeaderHeadInfo::getPeaceWeightRand() const
{
	return m_iPeaceWeightRand;
}

int CvLeaderHeadInfo::getWarmongerRespect() const
{
	return m_iWarmongerRespect;
}

int CvLeaderHeadInfo::getEspionageWeight() const
{
	return m_iEspionageWeight;
}

int CvLeaderHeadInfo::getRefuseToTalkWarThreshold() const
{
	return m_iRefuseToTalkWarThreshold;
}

int CvLeaderHeadInfo::getNoTechTradeThreshold() const
{
	return m_iNoTechTradeThreshold;
}

int CvLeaderHeadInfo::getTechTradeKnownPercent() const
{
	return m_iTechTradeKnownPercent;
}

int CvLeaderHeadInfo::getMaxGoldTradePercent() const
{
	return m_iMaxGoldTradePercent;
}

int CvLeaderHeadInfo::getMaxGoldPerTurnTradePercent() const
{
	return m_iMaxGoldPerTurnTradePercent;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  03/21/10								jdog5000	  */
/*																							  */
/* Victory Strategy AI																		  */
/************************************************************************************************/
int CvLeaderHeadInfo::getCultureVictoryWeight() const
{
	return m_iCultureVictoryWeight;
}

int CvLeaderHeadInfo::getSpaceVictoryWeight() const
{
	return m_iSpaceVictoryWeight;
}

int CvLeaderHeadInfo::getConquestVictoryWeight() const
{
	return m_iConquestVictoryWeight;
}

int CvLeaderHeadInfo::getDominationVictoryWeight() const
{
	return m_iDominationVictoryWeight;
}

int CvLeaderHeadInfo::getDiplomacyVictoryWeight() const
{
	return m_iDiplomacyVictoryWeight;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/

int CvLeaderHeadInfo::getMaxWarRand() const
{
	return m_iMaxWarRand;
}

int CvLeaderHeadInfo::getMaxWarNearbyPowerRatio() const
{
	return m_iMaxWarNearbyPowerRatio;
}

int CvLeaderHeadInfo::getMaxWarDistantPowerRatio() const
{
	return m_iMaxWarDistantPowerRatio;
}

int CvLeaderHeadInfo::getMaxWarMinAdjacentLandPercent() const
{
	return m_iMaxWarMinAdjacentLandPercent;
}

int CvLeaderHeadInfo::getLimitedWarRand() const
{
	return m_iLimitedWarRand;
}

int CvLeaderHeadInfo::getLimitedWarPowerRatio() const
{
	return m_iLimitedWarPowerRatio;
}

int CvLeaderHeadInfo::getDogpileWarRand() const
{
	return m_iDogpileWarRand;
}

int CvLeaderHeadInfo::getMakePeaceRand() const
{
	return m_iMakePeaceRand;
}

int CvLeaderHeadInfo::getDeclareWarTradeRand() const
{
	return m_iDeclareWarTradeRand;
}

int CvLeaderHeadInfo::getDemandRebukedSneakProb() const
{
	return m_iDemandRebukedSneakProb;
}

int CvLeaderHeadInfo::getDemandRebukedWarProb() const
{
	return m_iDemandRebukedWarProb;
}

int CvLeaderHeadInfo::getRazeCityProb() const
{
	return m_iRazeCityProb;
}

int CvLeaderHeadInfo::getBuildUnitProb() const
{
	return m_iBuildUnitProb;
}

int CvLeaderHeadInfo::getBaseAttackOddsChange() const
{
	return m_iBaseAttackOddsChange;
}

int CvLeaderHeadInfo::getAttackOddsChangeRand() const
{
	return m_iAttackOddsChangeRand;
}

int CvLeaderHeadInfo::getWorseRankDifferenceAttitudeChange() const
{
	return m_iWorseRankDifferenceAttitudeChange;
}

int CvLeaderHeadInfo::getBetterRankDifferenceAttitudeChange() const
{
	return m_iBetterRankDifferenceAttitudeChange;
}

int CvLeaderHeadInfo::getCloseBordersAttitudeChange() const
{
	return m_iCloseBordersAttitudeChange;
}

int CvLeaderHeadInfo::getLostWarAttitudeChange() const
{
	return m_iLostWarAttitudeChange;
}

int CvLeaderHeadInfo::getAtWarAttitudeDivisor() const
{
	return m_iAtWarAttitudeDivisor;
}

int CvLeaderHeadInfo::getAtWarAttitudeChangeLimit() const
{
	return m_iAtWarAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getAtPeaceAttitudeDivisor() const
{
	return m_iAtPeaceAttitudeDivisor;
}

int CvLeaderHeadInfo::getAtPeaceAttitudeChangeLimit() const
{
	return m_iAtPeaceAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getSameReligionAttitudeChange() const
{
	return m_iSameReligionAttitudeChange;
}

int CvLeaderHeadInfo::getSameReligionAttitudeDivisor() const
{
	return m_iSameReligionAttitudeDivisor;
}

int CvLeaderHeadInfo::getSameReligionAttitudeChangeLimit() const
{
	return m_iSameReligionAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getDifferentReligionAttitudeChange() const
{
	return m_iDifferentReligionAttitudeChange;
}

int CvLeaderHeadInfo::getDifferentReligionAttitudeDivisor() const
{
	return m_iDifferentReligionAttitudeDivisor;
}

int CvLeaderHeadInfo::getDifferentReligionAttitudeChangeLimit() const
{
	return m_iDifferentReligionAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getBonusTradeAttitudeDivisor() const
{
	return m_iBonusTradeAttitudeDivisor;
}

int CvLeaderHeadInfo::getBonusTradeAttitudeChangeLimit() const
{
	return m_iBonusTradeAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getOpenBordersAttitudeDivisor() const
{
	return m_iOpenBordersAttitudeDivisor;
}

int CvLeaderHeadInfo::getOpenBordersAttitudeChangeLimit() const
{
	return m_iOpenBordersAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getDefensivePactAttitudeDivisor() const
{
	return m_iDefensivePactAttitudeDivisor;
}

int CvLeaderHeadInfo::getDefensivePactAttitudeChangeLimit() const
{
	return m_iDefensivePactAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getShareWarAttitudeChange() const
{
	return m_iShareWarAttitudeChange;
}

int CvLeaderHeadInfo::getShareWarAttitudeDivisor() const
{
	return m_iShareWarAttitudeDivisor;
}

int CvLeaderHeadInfo::getShareWarAttitudeChangeLimit() const
{
	return m_iShareWarAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getFavoriteCivicAttitudeChange() const
{
	return m_iFavoriteCivicAttitudeChange;
}

int CvLeaderHeadInfo::getFavoriteCivicAttitudeDivisor() const
{
	return m_iFavoriteCivicAttitudeDivisor;
}

int CvLeaderHeadInfo::getFavoriteCivicAttitudeChangeLimit() const
{
	return m_iFavoriteCivicAttitudeChangeLimit;
}

int CvLeaderHeadInfo::getDemandTributeAttitudeThreshold() const
{
	return m_iDemandTributeAttitudeThreshold;
}

int CvLeaderHeadInfo::getNoGiveHelpAttitudeThreshold() const
{
	return m_iNoGiveHelpAttitudeThreshold;
}

int CvLeaderHeadInfo::getTechRefuseAttitudeThreshold() const
{
	return m_iTechRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getStrategicBonusRefuseAttitudeThreshold() const
{
	return m_iStrategicBonusRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getHappinessBonusRefuseAttitudeThreshold() const
{
	return m_iHappinessBonusRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getHealthBonusRefuseAttitudeThreshold() const
{
	return m_iHealthBonusRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getMapRefuseAttitudeThreshold() const
{
	return m_iMapRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getDeclareWarRefuseAttitudeThreshold() const
{
	return m_iDeclareWarRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getDeclareWarThemRefuseAttitudeThreshold() const
{
	return m_iDeclareWarThemRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getStopTradingRefuseAttitudeThreshold() const
{
	return m_iStopTradingRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getStopTradingThemRefuseAttitudeThreshold() const
{
	return m_iStopTradingThemRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getAdoptCivicRefuseAttitudeThreshold() const
{
	return m_iAdoptCivicRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getConvertReligionRefuseAttitudeThreshold() const
{
	return m_iConvertReligionRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getOpenBordersRefuseAttitudeThreshold() const
{
	return m_iOpenBordersRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getDefensivePactRefuseAttitudeThreshold() const
{
	return m_iDefensivePactRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getPermanentAllianceRefuseAttitudeThreshold() const
{
	return m_iPermanentAllianceRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getVassalRefuseAttitudeThreshold() const
{
	return m_iVassalRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getVassalPowerModifier() const
{
	return m_iVassalPowerModifier;
}

int CvLeaderHeadInfo::getFavoriteCivic() const
{
	return m_iFavoriteCivic;
}

int CvLeaderHeadInfo::getFavoriteReligion() const
{
	return m_iFavoriteReligion;
}

int CvLeaderHeadInfo::getFreedomAppreciation() const
{
	return m_iFreedomAppreciation;
}

const TCHAR* CvLeaderHeadInfo::getArtDefineTag() const
{
	return m_szArtDefineTag;
}

void CvLeaderHeadInfo::setArtDefineTag(const TCHAR* szVal)
{
	m_szArtDefineTag = szVal;
}

// Arrays

bool CvLeaderHeadInfo::hasTrait(int i) const
{
	FAssertMsg(i < GC.getNumTraitInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pbTraits ? m_pbTraits[i] : false;
}

int CvLeaderHeadInfo::getFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvLeaderHeadInfo::getContactRand(int i) const
{
	FAssertMsg(i < NUM_CONTACT_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piContactRand ? m_piContactRand[i] : 0;
}

int CvLeaderHeadInfo::getContactDelay(int i) const
{
	FAssertMsg(i < NUM_CONTACT_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piContactDelay ? m_piContactDelay[i] : 0;
}

int CvLeaderHeadInfo::getMemoryDecayRand(int i) const
{
	FAssertMsg(i < NUM_MEMORY_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piMemoryDecayRand ? m_piMemoryDecayRand[i] : 0;
}

int CvLeaderHeadInfo::getMemoryAttitudePercent(int i) const
{
	FAssertMsg(i < NUM_MEMORY_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piMemoryAttitudePercent ? m_piMemoryAttitudePercent[i] : 0;
}

int CvLeaderHeadInfo::getNoWarAttitudeProb(int i) const
{
	FAssertMsg(i < NUM_ATTITUDE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piNoWarAttitudeProb ? m_piNoWarAttitudeProb[i] : 0;
}

int CvLeaderHeadInfo::getUnitAIWeightModifier(int i) const
{
	FAssertMsg(i < NUM_UNITAI_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piUnitAIWeightModifier ? m_piUnitAIWeightModifier[i] : 0;
}

int CvLeaderHeadInfo::getImprovementWeightModifier(int i) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piImprovementWeightModifier ? m_piImprovementWeightModifier[i] : 0;
}

int CvLeaderHeadInfo::getDiploPeaceIntroMusicScriptIds(int i) const
{
	FAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piDiploPeaceIntroMusicScriptIds ? m_piDiploPeaceIntroMusicScriptIds[i] : -1;
}

int CvLeaderHeadInfo::getDiploPeaceMusicScriptIds(int i) const
{
	FAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piDiploPeaceMusicScriptIds ? m_piDiploPeaceMusicScriptIds[i] : -1;
}

int CvLeaderHeadInfo::getDiploWarIntroMusicScriptIds(int i) const
{
	FAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piDiploWarIntroMusicScriptIds ? m_piDiploWarIntroMusicScriptIds[i] : -1;
}

int CvLeaderHeadInfo::getDiploWarMusicScriptIds(int i) const
{
	FAssertMsg(i < GC.getNumEraInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piDiploWarMusicScriptIds ? m_piDiploWarMusicScriptIds[i] : -1;
}

const TCHAR* CvLeaderHeadInfo::getLeaderHead() const
{
	const CvArtInfoLeaderhead * pLeaderheadArtInfo;
	pLeaderheadArtInfo = getArtInfo();
	if (pLeaderheadArtInfo != NULL)
	{
		return pLeaderheadArtInfo->getNIF();
	}
	else
	{
		return NULL;
	}
}

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
int CvLeaderHeadInfo::getMilitaryUnitRefuseAttitudeThreshold() const
{
	return m_iMilitaryUnitRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getWorkerRefuseAttitudeThreshold() const
{
	return m_iWorkerRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getCorporationRefuseAttitudeThreshold() const
{
	return m_iCorporationRefuseAttitudeThreshold;
}

int CvLeaderHeadInfo::getSecretaryGeneralVoteRefuseAttitudeThreshold() const
{
	return m_iSecretaryGeneralVoteRefuseAttitudeThreshold;
}

void CvLeaderHeadInfo::setCultureVictoryWeight(int i)
{
	m_iCultureVictoryWeight = i;
}

void CvLeaderHeadInfo::setSpaceVictoryWeight(int i)
{
	m_iSpaceVictoryWeight = i;
}

void CvLeaderHeadInfo::setConquestVictoryWeight(int i)
{
	m_iConquestVictoryWeight = i;
}

void CvLeaderHeadInfo::setDominationVictoryWeight(int i)
{
	m_iDominationVictoryWeight = i;
}

void CvLeaderHeadInfo::setDiplomacyVictoryWeight(int i)
{
	m_iDiplomacyVictoryWeight = i;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

void CvLeaderHeadInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_bNPC);
	CheckSum(iSum, m_iWonderConstructRand);
	CheckSum(iSum, m_iBaseAttitude);
	CheckSum(iSum, m_iBasePeaceWeight);
	CheckSum(iSum, m_iPeaceWeightRand);
	CheckSum(iSum, m_iWarmongerRespect);
	CheckSum(iSum, m_iEspionageWeight);
	CheckSum(iSum, m_iRefuseToTalkWarThreshold);
	CheckSum(iSum, m_iNoTechTradeThreshold);
	CheckSum(iSum, m_iTechTradeKnownPercent);
	CheckSum(iSum, m_iMaxGoldTradePercent);
	CheckSum(iSum, m_iMaxGoldPerTurnTradePercent);

	CheckSum(iSum, m_iCultureVictoryWeight);
	CheckSum(iSum, m_iSpaceVictoryWeight);
	CheckSum(iSum, m_iConquestVictoryWeight);
	CheckSum(iSum, m_iDominationVictoryWeight);
	CheckSum(iSum, m_iDiplomacyVictoryWeight);

	CheckSum(iSum, m_iMaxWarRand);
	CheckSum(iSum, m_iMaxWarNearbyPowerRatio);
	CheckSum(iSum, m_iMaxWarDistantPowerRatio);
	CheckSum(iSum, m_iMaxWarMinAdjacentLandPercent);
	CheckSum(iSum, m_iLimitedWarRand);
	CheckSum(iSum, m_iLimitedWarPowerRatio);
	CheckSum(iSum, m_iDogpileWarRand);
	CheckSum(iSum, m_iMakePeaceRand);
	CheckSum(iSum, m_iDeclareWarTradeRand);
	CheckSum(iSum, m_iDemandRebukedSneakProb);
	CheckSum(iSum, m_iDemandRebukedWarProb);
	CheckSum(iSum, m_iRazeCityProb);
	CheckSum(iSum, m_iBuildUnitProb);
	CheckSum(iSum, m_iBaseAttackOddsChange);
	CheckSum(iSum, m_iAttackOddsChangeRand);
	CheckSum(iSum, m_iWorseRankDifferenceAttitudeChange);
	CheckSum(iSum, m_iBetterRankDifferenceAttitudeChange);
	CheckSum(iSum, m_iCloseBordersAttitudeChange);
	CheckSum(iSum, m_iLostWarAttitudeChange);
	CheckSum(iSum, m_iAtWarAttitudeDivisor);
	CheckSum(iSum, m_iAtWarAttitudeChangeLimit);
	CheckSum(iSum, m_iAtPeaceAttitudeDivisor);
	CheckSum(iSum, m_iAtPeaceAttitudeChangeLimit);
	CheckSum(iSum, m_iSameReligionAttitudeChange);
	CheckSum(iSum, m_iSameReligionAttitudeDivisor);
	CheckSum(iSum, m_iSameReligionAttitudeChangeLimit);
	CheckSum(iSum, m_iDifferentReligionAttitudeChange);
	CheckSum(iSum, m_iDifferentReligionAttitudeDivisor);
	CheckSum(iSum, m_iDifferentReligionAttitudeChangeLimit);
	CheckSum(iSum, m_iBonusTradeAttitudeDivisor);
	CheckSum(iSum, m_iBonusTradeAttitudeChangeLimit);
	CheckSum(iSum, m_iOpenBordersAttitudeDivisor);
	CheckSum(iSum, m_iOpenBordersAttitudeChangeLimit);
	CheckSum(iSum, m_iDefensivePactAttitudeDivisor);
	CheckSum(iSum, m_iDefensivePactAttitudeChangeLimit);
	CheckSum(iSum, m_iShareWarAttitudeChange);
	CheckSum(iSum, m_iShareWarAttitudeDivisor);
	CheckSum(iSum, m_iShareWarAttitudeChangeLimit);
	CheckSum(iSum, m_iFavoriteCivicAttitudeChange);
	CheckSum(iSum, m_iFavoriteCivicAttitudeDivisor);
	CheckSum(iSum, m_iFavoriteCivicAttitudeChangeLimit);
	CheckSum(iSum, m_iDemandTributeAttitudeThreshold);
	CheckSum(iSum, m_iNoGiveHelpAttitudeThreshold);
	CheckSum(iSum, m_iTechRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStrategicBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iHappinessBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iHealthBonusRefuseAttitudeThreshold);
	CheckSum(iSum, m_iMapRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDeclareWarRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDeclareWarThemRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStopTradingRefuseAttitudeThreshold);
	CheckSum(iSum, m_iStopTradingThemRefuseAttitudeThreshold);
	CheckSum(iSum, m_iAdoptCivicRefuseAttitudeThreshold);
	CheckSum(iSum, m_iConvertReligionRefuseAttitudeThreshold);
	CheckSum(iSum, m_iOpenBordersRefuseAttitudeThreshold);
	CheckSum(iSum, m_iDefensivePactRefuseAttitudeThreshold);
	CheckSum(iSum, m_iPermanentAllianceRefuseAttitudeThreshold);
	CheckSum(iSum, m_iVassalRefuseAttitudeThreshold);
	CheckSum(iSum, m_iVassalPowerModifier);
	CheckSum(iSum, m_iFreedomAppreciation);
	CheckSum(iSum, m_iFavoriteCivic);
	CheckSum(iSum, m_iFavoriteReligion);

	// Arrays

	CheckSumI(iSum, GC.getNumTraitInfos(), m_pbTraits);

	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piFlavorValue);
	CheckSumI(iSum, NUM_CONTACT_TYPES, m_piContactRand);
	CheckSumI(iSum, NUM_CONTACT_TYPES, m_piContactDelay);
	CheckSumI(iSum, NUM_MEMORY_TYPES, m_piMemoryDecayRand);
	CheckSumI(iSum, NUM_MEMORY_TYPES, m_piMemoryAttitudePercent);
	CheckSumI(iSum, NUM_ATTITUDE_TYPES, m_piNoWarAttitudeProb);
	CheckSumI(iSum, NUM_UNITAI_TYPES, m_piUnitAIWeightModifier);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementWeightModifier);

	CheckSum(iSum, m_iMilitaryUnitRefuseAttitudeThreshold);
	CheckSum(iSum, m_iWorkerRefuseAttitudeThreshold);
	CheckSum(iSum, m_iCorporationRefuseAttitudeThreshold);
	CheckSum(iSum, m_iSecretaryGeneralVoteRefuseAttitudeThreshold);
}

const CvArtInfoLeaderhead* CvLeaderHeadInfo::getArtInfo() const
{
	return ARTFILEMGR.getLeaderheadArtInfo( getArtDefineTag());
}

bool CvLeaderHeadInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ArtDefineTag");
	setArtDefineTag(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bNPC, L"bNPC");
	pXML->GetOptionalChildXmlValByName(&m_iWonderConstructRand, L"iWonderConstructRand");
	pXML->GetOptionalChildXmlValByName(&m_iBaseAttitude, L"iBaseAttitude");
	pXML->GetOptionalChildXmlValByName(&m_iBasePeaceWeight, L"iBasePeaceWeight");
	pXML->GetOptionalChildXmlValByName(&m_iPeaceWeightRand, L"iPeaceWeightRand");
	pXML->GetOptionalChildXmlValByName(&m_iWarmongerRespect, L"iWarmongerRespect");
	pXML->GetOptionalChildXmlValByName(&m_iEspionageWeight, L"iEspionageWeight");
	pXML->GetOptionalChildXmlValByName(&m_iRefuseToTalkWarThreshold, L"iRefuseToTalkWarThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iNoTechTradeThreshold, L"iNoTechTradeThreshold");
	pXML->GetOptionalChildXmlValByName(&m_iTechTradeKnownPercent, L"iTechTradeKnownPercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGoldTradePercent, L"iMaxGoldTradePercent");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGoldPerTurnTradePercent, L"iMaxGoldPerTurnTradePercent");
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					  03/21/10								jdog5000	  */
/*																							  */
/* Victory Strategy AI																		  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iCultureVictoryWeight, L"iCultureVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iSpaceVictoryWeight, L"iSpaceVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iConquestVictoryWeight, L"iConquestVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iDominationVictoryWeight, L"iDominationVictoryWeight");
	pXML->GetOptionalChildXmlValByName(&m_iDiplomacyVictoryWeight, L"iDiplomacyVictoryWeight");
/************************************************************************************************/
/* BETTER_BTS_AI_MOD					   END												  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarRand, L"iMaxWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarNearbyPowerRatio, L"iMaxWarNearbyPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarDistantPowerRatio, L"iMaxWarDistantPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iMaxWarMinAdjacentLandPercent, L"iMaxWarMinAdjacentLandPercent");
	pXML->GetOptionalChildXmlValByName(&m_iLimitedWarRand, L"iLimitedWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iLimitedWarPowerRatio, L"iLimitedWarPowerRatio");
	pXML->GetOptionalChildXmlValByName(&m_iDogpileWarRand, L"iDogpileWarRand");
	pXML->GetOptionalChildXmlValByName(&m_iMakePeaceRand, L"iMakePeaceRand");
	pXML->GetOptionalChildXmlValByName(&m_iDeclareWarTradeRand, L"iDeclareWarTradeRand");
	pXML->GetOptionalChildXmlValByName(&m_iDemandRebukedSneakProb, L"iDemandRebukedSneakProb");
	pXML->GetOptionalChildXmlValByName(&m_iDemandRebukedWarProb, L"iDemandRebukedWarProb");
	pXML->GetOptionalChildXmlValByName(&m_iRazeCityProb, L"iRazeCityProb");
	pXML->GetOptionalChildXmlValByName(&m_iBuildUnitProb, L"iBuildUnitProb");
	pXML->GetOptionalChildXmlValByName(&m_iBaseAttackOddsChange, L"iBaseAttackOddsChange");
	pXML->GetOptionalChildXmlValByName(&m_iAttackOddsChangeRand, L"iAttackOddsChangeRand");
	pXML->GetOptionalChildXmlValByName(&m_iWorseRankDifferenceAttitudeChange, L"iWorseRankDifferenceAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iBetterRankDifferenceAttitudeChange, L"iBetterRankDifferenceAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iCloseBordersAttitudeChange, L"iCloseBordersAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iLostWarAttitudeChange, L"iLostWarAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iAtWarAttitudeDivisor, L"iAtWarAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iAtWarAttitudeChangeLimit, L"iAtWarAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iAtPeaceAttitudeDivisor, L"iAtPeaceAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iAtPeaceAttitudeChangeLimit, L"iAtPeaceAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeChange, L"iSameReligionAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeDivisor, L"iSameReligionAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iSameReligionAttitudeChangeLimit, L"iSameReligionAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeChange, L"iDifferentReligionAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeDivisor, L"iDifferentReligionAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iDifferentReligionAttitudeChangeLimit, L"iDifferentReligionAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iBonusTradeAttitudeDivisor, L"iBonusTradeAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iBonusTradeAttitudeChangeLimit, L"iBonusTradeAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iOpenBordersAttitudeDivisor, L"iOpenBordersAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iOpenBordersAttitudeChangeLimit, L"iOpenBordersAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iDefensivePactAttitudeDivisor, L"iDefensivePactAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iDefensivePactAttitudeChangeLimit, L"iDefensivePactAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeChange, L"iShareWarAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeDivisor, L"iShareWarAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iShareWarAttitudeChangeLimit, L"iShareWarAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeChange, L"iFavoriteCivicAttitudeChange");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeDivisor, L"iFavoriteCivicAttitudeDivisor");
	pXML->GetOptionalChildXmlValByName(&m_iFavoriteCivicAttitudeChangeLimit, L"iFavoriteCivicAttitudeChangeLimit");
	pXML->GetOptionalChildXmlValByName(&m_iVassalPowerModifier, L"iVassalPowerModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreedomAppreciation, L"iFreedomAppreciation");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DemandTributeAttitudeThreshold");
	m_iDemandTributeAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NoGiveHelpAttitudeThreshold");
	m_iNoGiveHelpAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechRefuseAttitudeThreshold");
	m_iTechRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StrategicBonusRefuseAttitudeThreshold");
	m_iStrategicBonusRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HappinessBonusRefuseAttitudeThreshold");
	m_iHappinessBonusRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HealthBonusRefuseAttitudeThreshold");
	m_iHealthBonusRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MapRefuseAttitudeThreshold");
	m_iMapRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DeclareWarRefuseAttitudeThreshold");
	m_iDeclareWarRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DeclareWarThemRefuseAttitudeThreshold");
	m_iDeclareWarThemRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StopTradingRefuseAttitudeThreshold");
	m_iStopTradingRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StopTradingThemRefuseAttitudeThreshold");
	m_iStopTradingThemRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"AdoptCivicRefuseAttitudeThreshold");
	m_iAdoptCivicRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ConvertReligionRefuseAttitudeThreshold");
	m_iConvertReligionRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"OpenBordersRefuseAttitudeThreshold");
	m_iOpenBordersRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DefensivePactRefuseAttitudeThreshold");
	m_iDefensivePactRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PermanentAllianceRefuseAttitudeThreshold");
	m_iPermanentAllianceRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"VassalRefuseAttitudeThreshold");
	m_iVassalRefuseAttitudeThreshold = pXML->GetInfoClass( szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FavoriteCivic");
	m_iFavoriteCivic = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FavoriteReligion");
	m_iFavoriteReligion = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_pbTraits, L"Traits", GC.getNumTraitInfos());

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getFlavorTypes(), GC.getNumFlavorTypes());
	pXML->SetVariableListTagPair(&m_piContactRand, L"ContactRands", GC.getContactTypes(), NUM_CONTACT_TYPES);
	pXML->SetVariableListTagPair(&m_piContactDelay, L"ContactDelays", GC.getContactTypes(), NUM_CONTACT_TYPES);
	pXML->SetVariableListTagPair(&m_piMemoryDecayRand, L"MemoryDecays", NUM_MEMORY_TYPES);
	pXML->SetVariableListTagPair(&m_piMemoryAttitudePercent, L"MemoryAttitudePercents", NUM_MEMORY_TYPES);
	pXML->SetVariableListTagPair(&m_piNoWarAttitudeProb, L"NoWarAttitudeProbs", NUM_ATTITUDE_TYPES);
	pXML->SetVariableListTagPair(&m_piUnitAIWeightModifier, L"UnitAIWeightModifiers", NUM_UNITAI_TYPES);
	pXML->SetVariableListTagPair(&m_piImprovementWeightModifier, L"ImprovementWeightModifiers", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploPeaceIntroMusicScriptIds, L"DiplomacyIntroMusicPeace", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploPeaceMusicScriptIds, L"DiplomacyMusicPeace", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploWarIntroMusicScriptIds, L"DiplomacyIntroMusicWar", GC.getNumEraInfos());
	pXML->SetVariableListTagPairForAudioScripts(&m_piDiploWarMusicScriptIds, L"DiplomacyMusicWar", GC.getNumEraInfos());
/************************************************************************************************/
/* Afforess					  Start		 07/29/10											   */
/*																							  */
/* Advanced Diplomacy																		   */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(szTextVal, L"MilitaryUnitRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iMilitaryUnitRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"WorkerRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iWorkerRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CorporationRefuseAttitudeThreshold", "ATTITUDE_CAUTIOUS");
	m_iCorporationRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SecretaryGeneralVoteRefuseAttitudeThreshold", "ATTITUDE_ANNOYED");
	m_iSecretaryGeneralVoteRefuseAttitudeThreshold = pXML->GetInfoClass(szTextVal);

	setDefaultMemoryInfo();
	setDefaultContactInfo();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	return true;
}
/************************************************************************************************/
/* XMLCOPY								 10/12/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
void CvLeaderHeadInfo::copyNonDefaults(CvLeaderHeadInfo* pClassInfo, CvXMLLoadUtility* pXML)
{

	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;
	int iAudioDefault = -1;
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	//Art files must be reread first!
	if (getArtDefineTag() == cDefault) setArtDefineTag(pClassInfo->getArtDefineTag());

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (isNPC() == bDefault) m_bNPC = pClassInfo->isNPC();
	if (getWonderConstructRand() == iDefault) m_iWonderConstructRand = pClassInfo->getWonderConstructRand();
	if (getBaseAttitude() == iDefault) m_iBaseAttitude = pClassInfo->getBaseAttitude();
	if (getBasePeaceWeight() == iDefault) m_iBasePeaceWeight = pClassInfo->getBasePeaceWeight();
	if (getPeaceWeightRand() == iDefault) m_iPeaceWeightRand = pClassInfo->getPeaceWeightRand();
	if (getWarmongerRespect() == iDefault) m_iWarmongerRespect = pClassInfo->getWarmongerRespect();
	if (getEspionageWeight() == iDefault) m_iEspionageWeight = pClassInfo->getEspionageWeight();
	if (getRefuseToTalkWarThreshold() == iDefault) m_iRefuseToTalkWarThreshold = pClassInfo->getRefuseToTalkWarThreshold();
	if (getNoTechTradeThreshold() == iDefault) m_iNoTechTradeThreshold = pClassInfo->getNoTechTradeThreshold();
	if (getTechTradeKnownPercent() == iDefault) m_iTechTradeKnownPercent = pClassInfo->getTechTradeKnownPercent();
	if (getMaxGoldTradePercent() == iDefault) m_iMaxGoldTradePercent = pClassInfo->getMaxGoldTradePercent();
	if (getMaxGoldPerTurnTradePercent() == iDefault) m_iMaxGoldPerTurnTradePercent = pClassInfo->getMaxGoldPerTurnTradePercent();
	if (getMaxWarRand() == iDefault) m_iMaxWarRand = pClassInfo->getMaxWarRand();
	if (getMaxWarNearbyPowerRatio() == iDefault) m_iMaxWarNearbyPowerRatio = pClassInfo->getMaxWarNearbyPowerRatio();
	if (getMaxWarDistantPowerRatio() == iDefault) m_iMaxWarDistantPowerRatio = pClassInfo->getMaxWarDistantPowerRatio();
	if (getMaxWarMinAdjacentLandPercent() == iDefault) m_iMaxWarMinAdjacentLandPercent = pClassInfo->getMaxWarMinAdjacentLandPercent();
	if (getLimitedWarRand() == iDefault) m_iLimitedWarRand = pClassInfo->getLimitedWarRand();
	if (getLimitedWarPowerRatio() == iDefault) m_iLimitedWarPowerRatio = pClassInfo->getLimitedWarPowerRatio();
	if (getDogpileWarRand() == iDefault) m_iDogpileWarRand = pClassInfo->getDogpileWarRand();
	if (getMakePeaceRand() == iDefault) m_iMakePeaceRand = pClassInfo->getMakePeaceRand();
	if (getDeclareWarTradeRand() == iDefault) m_iDeclareWarTradeRand = pClassInfo->getDeclareWarTradeRand();
	if (getDemandRebukedSneakProb() == iDefault) m_iDemandRebukedSneakProb = pClassInfo->getDemandRebukedSneakProb();
	if (getDemandRebukedWarProb() == iDefault) m_iDemandRebukedWarProb = pClassInfo->getDemandRebukedWarProb();
	if (getRazeCityProb() == iDefault) m_iRazeCityProb = pClassInfo->getRazeCityProb();
	if (getBuildUnitProb() == iDefault) m_iBuildUnitProb = pClassInfo->getBuildUnitProb();
	if (getBaseAttackOddsChange() == iDefault) m_iBaseAttackOddsChange = pClassInfo->getBaseAttackOddsChange();
	if (getAttackOddsChangeRand() == iDefault) m_iAttackOddsChangeRand = pClassInfo->getAttackOddsChangeRand();
	if (getWorseRankDifferenceAttitudeChange() == iDefault) m_iWorseRankDifferenceAttitudeChange = pClassInfo->getWorseRankDifferenceAttitudeChange();
	if (getBetterRankDifferenceAttitudeChange() == iDefault) m_iBetterRankDifferenceAttitudeChange = pClassInfo->getBetterRankDifferenceAttitudeChange();
	if (getCloseBordersAttitudeChange() == iDefault) m_iCloseBordersAttitudeChange = pClassInfo->getCloseBordersAttitudeChange();
	if (getLostWarAttitudeChange() == iDefault) m_iLostWarAttitudeChange = pClassInfo->getLostWarAttitudeChange();
	if (getAtWarAttitudeDivisor() == iDefault) m_iAtWarAttitudeDivisor = pClassInfo->getAtWarAttitudeDivisor();
	if (getAtWarAttitudeChangeLimit() == iDefault) m_iAtWarAttitudeChangeLimit = pClassInfo->getAtWarAttitudeChangeLimit();
	if (getAtPeaceAttitudeDivisor() == iDefault) m_iAtPeaceAttitudeDivisor = pClassInfo->getAtPeaceAttitudeDivisor();
	if (getAtPeaceAttitudeChangeLimit() == iDefault) m_iAtPeaceAttitudeChangeLimit = pClassInfo->getAtPeaceAttitudeChangeLimit();
	if (getSameReligionAttitudeChange() == iDefault) m_iSameReligionAttitudeChange = pClassInfo->getSameReligionAttitudeChange();
	if (getSameReligionAttitudeDivisor() == iDefault) m_iSameReligionAttitudeDivisor = pClassInfo->getSameReligionAttitudeDivisor();
	if (getSameReligionAttitudeChangeLimit() == iDefault) m_iSameReligionAttitudeChangeLimit = pClassInfo->getSameReligionAttitudeChangeLimit();
	if (getDifferentReligionAttitudeChange() == iDefault) m_iDifferentReligionAttitudeChange = pClassInfo->getDifferentReligionAttitudeChange();
	if (getDifferentReligionAttitudeDivisor() == iDefault) m_iDifferentReligionAttitudeDivisor = pClassInfo->getDifferentReligionAttitudeDivisor();
	if (getDifferentReligionAttitudeChangeLimit() == iDefault) m_iDifferentReligionAttitudeChangeLimit = pClassInfo->getDifferentReligionAttitudeChangeLimit();
	if (getBonusTradeAttitudeDivisor() == iDefault) m_iBonusTradeAttitudeDivisor = pClassInfo->getBonusTradeAttitudeDivisor();
	if (getBonusTradeAttitudeChangeLimit() == iDefault) m_iBonusTradeAttitudeChangeLimit = pClassInfo->getBonusTradeAttitudeChangeLimit();
	if (getOpenBordersAttitudeDivisor() == iDefault) m_iOpenBordersAttitudeDivisor = pClassInfo->getOpenBordersAttitudeDivisor();
	if (getOpenBordersAttitudeChangeLimit() == iDefault) m_iOpenBordersAttitudeChangeLimit = pClassInfo->getOpenBordersAttitudeChangeLimit();
	if (getDefensivePactAttitudeDivisor() == iDefault) m_iDefensivePactAttitudeDivisor = pClassInfo->getDefensivePactAttitudeDivisor();
	if (getDefensivePactAttitudeChangeLimit() == iDefault) m_iDefensivePactAttitudeChangeLimit = pClassInfo->getDefensivePactAttitudeChangeLimit();
	if (getShareWarAttitudeChange() == iDefault) m_iShareWarAttitudeChange = pClassInfo->getShareWarAttitudeChange();
	if (getShareWarAttitudeDivisor() == iDefault) m_iShareWarAttitudeDivisor = pClassInfo->getShareWarAttitudeDivisor();
	if (getShareWarAttitudeChangeLimit() == iDefault) m_iShareWarAttitudeChangeLimit = pClassInfo->getShareWarAttitudeChangeLimit();
	if (getFavoriteCivicAttitudeChange() == iDefault) m_iFavoriteCivicAttitudeChange = pClassInfo->getFavoriteCivicAttitudeChange();
	if (getFavoriteCivicAttitudeDivisor() == iDefault) m_iFavoriteCivicAttitudeDivisor = pClassInfo->getFavoriteCivicAttitudeDivisor();
	if (getFavoriteCivicAttitudeChangeLimit() == iDefault) m_iFavoriteCivicAttitudeChangeLimit = pClassInfo->getFavoriteCivicAttitudeChangeLimit();
	if (getVassalPowerModifier() == iDefault) m_iVassalPowerModifier = pClassInfo->getVassalPowerModifier();
	if (getFreedomAppreciation() == iDefault) m_iFreedomAppreciation = pClassInfo->getFreedomAppreciation();

	if (getDemandTributeAttitudeThreshold() == iTextDefault) m_iDemandTributeAttitudeThreshold = pClassInfo->getDemandTributeAttitudeThreshold();
	if (getNoGiveHelpAttitudeThreshold() == iTextDefault) m_iNoGiveHelpAttitudeThreshold = pClassInfo->getNoGiveHelpAttitudeThreshold();
	if (getTechRefuseAttitudeThreshold() == iTextDefault) m_iTechRefuseAttitudeThreshold = pClassInfo->getTechRefuseAttitudeThreshold();
	if (getStrategicBonusRefuseAttitudeThreshold() == iTextDefault) m_iStrategicBonusRefuseAttitudeThreshold = pClassInfo->getStrategicBonusRefuseAttitudeThreshold();
	if (getHappinessBonusRefuseAttitudeThreshold() == iTextDefault) m_iHappinessBonusRefuseAttitudeThreshold = pClassInfo->getHappinessBonusRefuseAttitudeThreshold();
	if (getHealthBonusRefuseAttitudeThreshold() == iTextDefault) m_iHealthBonusRefuseAttitudeThreshold = pClassInfo->getHealthBonusRefuseAttitudeThreshold();
	if (getMapRefuseAttitudeThreshold() == iTextDefault) m_iMapRefuseAttitudeThreshold = pClassInfo->getMapRefuseAttitudeThreshold();
	if (getDeclareWarRefuseAttitudeThreshold() == iTextDefault) m_iDeclareWarRefuseAttitudeThreshold = pClassInfo->getDeclareWarRefuseAttitudeThreshold();
	if (getDeclareWarThemRefuseAttitudeThreshold() == iTextDefault) m_iDeclareWarThemRefuseAttitudeThreshold = pClassInfo->getDeclareWarThemRefuseAttitudeThreshold();
	if (getStopTradingRefuseAttitudeThreshold() == iTextDefault) m_iStopTradingRefuseAttitudeThreshold = pClassInfo->getStopTradingRefuseAttitudeThreshold();
	if (getStopTradingThemRefuseAttitudeThreshold() == iTextDefault) m_iStopTradingThemRefuseAttitudeThreshold = pClassInfo->getStopTradingThemRefuseAttitudeThreshold();
	if (getAdoptCivicRefuseAttitudeThreshold() == iTextDefault) m_iAdoptCivicRefuseAttitudeThreshold = pClassInfo->getAdoptCivicRefuseAttitudeThreshold();
	if (getConvertReligionRefuseAttitudeThreshold() == iTextDefault) m_iConvertReligionRefuseAttitudeThreshold = pClassInfo->getConvertReligionRefuseAttitudeThreshold();
	if (getOpenBordersRefuseAttitudeThreshold() == iTextDefault) m_iOpenBordersRefuseAttitudeThreshold = pClassInfo->getOpenBordersRefuseAttitudeThreshold();
	if (getDefensivePactRefuseAttitudeThreshold() == iTextDefault) m_iDefensivePactRefuseAttitudeThreshold = pClassInfo->getDefensivePactRefuseAttitudeThreshold();
	if (getPermanentAllianceRefuseAttitudeThreshold() == iTextDefault) m_iPermanentAllianceRefuseAttitudeThreshold = pClassInfo->getPermanentAllianceRefuseAttitudeThreshold();
	if (getVassalRefuseAttitudeThreshold() == iTextDefault) m_iVassalRefuseAttitudeThreshold = pClassInfo->getVassalRefuseAttitudeThreshold();
	if (getFavoriteCivic() == iTextDefault) m_iFavoriteCivic = pClassInfo->getFavoriteCivic();
	if (getFavoriteReligion() == iTextDefault) m_iFavoriteReligion = pClassInfo->getFavoriteReligion();

	for ( int j = 0; j < GC.getNumTraitInfos(); j++)
	{
		if ( hasTrait(j) == bDefault && pClassInfo->hasTrait(j) != bDefault)
		{
			if ( NULL == m_pbTraits )
			{
				CvXMLLoadUtility::InitList(&m_pbTraits,GC.getNumTraitInfos(),bDefault);
			}
			m_pbTraits[j] = pClassInfo->hasTrait(j);
		}
	}

	for ( int j = 0; j < GC.getNumFlavorTypes(); j++)
	{
		if ( getFlavorValue(j) == iDefault && pClassInfo->getFlavorValue(j) != iDefault)
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[j] = pClassInfo->getFlavorValue(j);
		}
	}
	for ( int j = 0; j < NUM_CONTACT_TYPES; j++)
	{
		if ( getContactRand(j) == iDefault && pClassInfo->getContactRand(j) != iDefault)
		{
			if ( NULL == m_piContactRand )
			{
				CvXMLLoadUtility::InitList(&m_piContactRand,NUM_CONTACT_TYPES,iDefault);
			}
			m_piContactRand[j] = pClassInfo->getContactRand(j);
		}
		if ( getContactDelay(j) == iDefault && pClassInfo->getContactDelay(j) != iDefault)
		{
			if ( NULL == m_piContactDelay )
			{
				CvXMLLoadUtility::InitList(&m_piContactDelay,NUM_CONTACT_TYPES,iDefault);
			}
			m_piContactDelay[j] = pClassInfo->getContactDelay(j);
		}
	}
	for ( int j = 0; j < NUM_MEMORY_TYPES; j++)
	{
		if ( getMemoryDecayRand(j) == iDefault && pClassInfo->getMemoryDecayRand(j) != iDefault)
		{
			if ( NULL == m_piMemoryDecayRand )
			{
				CvXMLLoadUtility::InitList(&m_piMemoryDecayRand,NUM_MEMORY_TYPES,iDefault);
			}
			m_piMemoryDecayRand[j] = pClassInfo->getMemoryDecayRand(j);
		}
		if ( getMemoryAttitudePercent(j) == iDefault && pClassInfo->getMemoryAttitudePercent(j) != iDefault )
		{
			if ( NULL == m_piMemoryAttitudePercent )
			{
				CvXMLLoadUtility::InitList(&m_piMemoryAttitudePercent,NUM_MEMORY_TYPES,iDefault);
			}
			m_piMemoryAttitudePercent[j] = pClassInfo->getMemoryAttitudePercent(j);
		}
	}
	for ( int j = 0; j < NUM_ATTITUDE_TYPES; j++)
	{
		if ( getNoWarAttitudeProb(j) == iDefault && pClassInfo->getNoWarAttitudeProb(j) != iDefault)
		{
			if ( NULL == m_piNoWarAttitudeProb )
			{
				CvXMLLoadUtility::InitList(&m_piNoWarAttitudeProb,NUM_ATTITUDE_TYPES,iDefault);
			}
			m_piNoWarAttitudeProb[j] = pClassInfo->getNoWarAttitudeProb(j);
		}
	}
	for ( int j = 0; j < NUM_UNITAI_TYPES; j++)
	{
		if ( getUnitAIWeightModifier(j) == iDefault && pClassInfo->getUnitAIWeightModifier(j) != iDefault)
		{
			if ( NULL == m_piUnitAIWeightModifier )
			{
				CvXMLLoadUtility::InitList(&m_piUnitAIWeightModifier,NUM_UNITAI_TYPES,iDefault);
			}
			m_piUnitAIWeightModifier[j] = pClassInfo->getUnitAIWeightModifier(j);
		}
	}
	for ( int j = 0; j < GC.getNumImprovementInfos(); j++)
	{
		if ( getImprovementWeightModifier(j) == iDefault && pClassInfo->getImprovementWeightModifier(j) != iDefault)
		{
			if ( NULL == m_piImprovementWeightModifier )
			{
				CvXMLLoadUtility::InitList(&m_piImprovementWeightModifier, GC.getNumImprovementInfos(),iDefault);
			}
			m_piImprovementWeightModifier[j] = pClassInfo->getImprovementWeightModifier(j);
		}
	}

	for ( int j = 0; j < GC.getNumEraInfos(); j++)
	{
		if ( getDiploPeaceIntroMusicScriptIds(j) == iAudioDefault && pClassInfo->getDiploPeaceIntroMusicScriptIds(j) != iAudioDefault)
		{
			if ( NULL == m_piDiploPeaceIntroMusicScriptIds )
			{
				CvXMLLoadUtility::InitList(&m_piDiploPeaceIntroMusicScriptIds,GC.getNumEraInfos(),iAudioDefault);
			}
			m_piDiploPeaceIntroMusicScriptIds[j] = pClassInfo->getDiploPeaceIntroMusicScriptIds(j);
		}
		if ( getDiploPeaceMusicScriptIds(j) == iAudioDefault && pClassInfo->getDiploPeaceMusicScriptIds(j) != iAudioDefault)
		{
			if ( NULL == m_piDiploPeaceMusicScriptIds )
			{
				CvXMLLoadUtility::InitList(&m_piDiploPeaceMusicScriptIds,GC.getNumEraInfos(),iAudioDefault);
			}
			m_piDiploPeaceMusicScriptIds[j] = pClassInfo->getDiploPeaceMusicScriptIds(j);
		}
		if ( getDiploWarIntroMusicScriptIds(j) == iAudioDefault && pClassInfo->getDiploWarIntroMusicScriptIds(j) != iAudioDefault)
		{
			if ( NULL == m_piDiploWarIntroMusicScriptIds )
			{
				CvXMLLoadUtility::InitList(&m_piDiploWarIntroMusicScriptIds,GC.getNumEraInfos(),iAudioDefault);
			}
			m_piDiploWarIntroMusicScriptIds[j] = pClassInfo->getDiploWarIntroMusicScriptIds(j);
		}
		if ( getDiploWarMusicScriptIds(j) == iAudioDefault && pClassInfo->getDiploWarMusicScriptIds(j) != iAudioDefault)
		{
			if ( NULL == m_piDiploWarMusicScriptIds )
			{
				CvXMLLoadUtility::InitList(&m_piDiploWarMusicScriptIds,GC.getNumEraInfos(),iAudioDefault);
			}
			m_piDiploWarMusicScriptIds[j] = pClassInfo->getDiploWarMusicScriptIds(j);
		}
	}
/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getMilitaryUnitRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iMilitaryUnitRefuseAttitudeThreshold = pClassInfo->getMilitaryUnitRefuseAttitudeThreshold();
	if (getWorkerRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iWorkerRefuseAttitudeThreshold = pClassInfo->getWorkerRefuseAttitudeThreshold();
	if (getCorporationRefuseAttitudeThreshold() == ATTITUDE_CAUTIOUS) m_iCorporationRefuseAttitudeThreshold = pClassInfo->getCorporationRefuseAttitudeThreshold();
	if (getSecretaryGeneralVoteRefuseAttitudeThreshold() == ATTITUDE_ANNOYED) m_iSecretaryGeneralVoteRefuseAttitudeThreshold = pClassInfo->getSecretaryGeneralVoteRefuseAttitudeThreshold();
	if (getCultureVictoryWeight() == 0) m_iCultureVictoryWeight = pClassInfo->getCultureVictoryWeight();
	if (getSpaceVictoryWeight() == 0) m_iSpaceVictoryWeight = pClassInfo->getSpaceVictoryWeight();
	if (getConquestVictoryWeight() == 0) m_iConquestVictoryWeight = pClassInfo->getConquestVictoryWeight();
	if (getDominationVictoryWeight() == 0) m_iDominationVictoryWeight = pClassInfo->getDominationVictoryWeight();
	if (getDiplomacyVictoryWeight() == 0) m_iDiplomacyVictoryWeight = pClassInfo->getDiplomacyVictoryWeight();
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
}
/************************************************************************************************/
/* Afforess					  Start		 05/23/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
//I'm lazy, so sue me. The XML still overrides this, so no worries.
void CvLeaderHeadInfo::setDefaultMemoryInfo()
{
	if ( NULL == m_piMemoryDecayRand )
	{
		CvXMLLoadUtility::InitList(&m_piMemoryDecayRand,NUM_MEMORY_TYPES,0);
	}
	if ( NULL == m_piMemoryAttitudePercent )
	{
		CvXMLLoadUtility::InitList(&m_piMemoryAttitudePercent,NUM_MEMORY_TYPES,0);
	}

	for (int iJ = 0; iJ < NUM_MEMORY_TYPES; iJ++)
	{
		if (getMemoryDecayRand(iJ) == 0)
		{
			if (iJ == MEMORY_INQUISITION)
			{
				m_piMemoryDecayRand[iJ] = 75;
			}
			else if (iJ == MEMORY_RECALLED_AMBASSADOR)
			{
				m_piMemoryDecayRand[iJ] = 25;
			}
			else if (iJ == MEMORY_WARMONGER)
			{
				m_piMemoryDecayRand[iJ] = 1;
			}
			else if (iJ == MEMORY_MADE_PEACE)
			{
				m_piMemoryDecayRand[iJ] = 1;
			}
			else if (iJ == MEMORY_SACKED_CITY)
			{
				m_piMemoryDecayRand[iJ] = 125;
			}
			else if (iJ == MEMORY_SACKED_HOLY_CITY)
			{
				m_piMemoryDecayRand[iJ] = 200;
			}
			else if (iJ == MEMORY_ENSLAVED_CITIZENS)
			{
				m_piMemoryDecayRand[iJ] = 100;
			}
			else if (iJ == MEMORY_BACKSTAB)
			{
				m_piMemoryDecayRand[iJ] = 250;
			}
			else if (iJ == MEMORY_BACKSTAB_FRIEND)
			{
				m_piMemoryDecayRand[iJ] = 250;
			}
		}
		if (getMemoryAttitudePercent(iJ) == 0)
		{
			if (iJ == MEMORY_INQUISITION)
			{
				m_piMemoryAttitudePercent[iJ] = -100;
			}
			else if (iJ == MEMORY_RECALLED_AMBASSADOR)
			{
				m_piMemoryAttitudePercent[iJ] = -0;
			}
			else if (iJ == MEMORY_WARMONGER)
			{
				m_piMemoryAttitudePercent[iJ] = 0;
			}
			else if (iJ == MEMORY_MADE_PEACE)
			{
				m_piMemoryAttitudePercent[iJ] = 0;
			}
			else if (iJ == MEMORY_SACKED_CITY)
			{
				m_piMemoryAttitudePercent[iJ] = -200;
			}
			else if (iJ == MEMORY_SACKED_HOLY_CITY)
			{
				m_piMemoryAttitudePercent[iJ] = -400;
			}
			else if (iJ == MEMORY_ENSLAVED_CITIZENS)
			{
				m_piMemoryAttitudePercent[iJ] = -200;
			}
			else if (iJ == MEMORY_BACKSTAB)
			{
				m_piMemoryAttitudePercent[iJ] = -400;
			}
			else if (iJ == MEMORY_BACKSTAB_FRIEND)
			{
				m_piMemoryAttitudePercent[iJ] = -150;
			}
		}
	}
}

void CvLeaderHeadInfo::setDefaultContactInfo()
{
	if ( NULL == m_piContactRand )
	{
		CvXMLLoadUtility::InitList(&m_piContactRand,NUM_CONTACT_TYPES,0);
	}

	if ( NULL == m_piContactDelay )
	{
		CvXMLLoadUtility::InitList(&m_piContactDelay,NUM_CONTACT_TYPES,0);
	}

	for (int iJ = 0; iJ < NUM_CONTACT_TYPES; iJ++)
	{
		if (getContactRand(iJ) == 0)
		{
			if (iJ == CONTACT_PEACE_PRESSURE)
			{
				m_piContactRand[iJ] = 50;
			}
			else if (iJ == CONTACT_EMBASSY)
			{
				m_piContactRand[iJ] = 25;
			}
			else if (iJ == CONTACT_TRADE_STOP_TRADING)
			{
				m_piContactRand[iJ] = 20;
			}
			else if (iJ == CONTACT_TRADE_JOIN_WAR)
			{
				m_piContactRand[iJ] = 10;
			}
			else if (iJ == CONTACT_SECRETARY_GENERAL_VOTE)
			{
				m_piContactRand[iJ] = 25;
			}
			else if (iJ == CONTACT_TRADE_MILITARY_UNITS)
			{
				m_piContactRand[iJ] = 20;
			}
			else if (iJ == CONTACT_TRADE_WORKERS)
			{
				m_piContactRand[iJ] = 25;
			}
			else if (iJ == CONTACT_TRADE_CONTACTS)
			{
				m_piContactRand[iJ] = 15;
			}
			else if (iJ == CONTACT_TRADE_CORPORATION)
			{
				m_piContactRand[iJ] = 35;
			}
			else if (iJ == CONTACT_TRADE_BUY_WAR)
			{
				m_piContactRand[iJ] = 10;
			}
		}
		if (getContactDelay(iJ) == 0)
		{
			if (iJ == CONTACT_PEACE_PRESSURE)
			{
				m_piContactDelay[iJ] = 30;
			}
			else if (iJ == CONTACT_EMBASSY)
			{
				m_piContactDelay[iJ] = 20;
			}
			else if (iJ == CONTACT_TRADE_STOP_TRADING)
			{
				m_piContactDelay[iJ] = 20;
			}
			else if (iJ == CONTACT_TRADE_JOIN_WAR)
			{
				m_piContactDelay[iJ] = 20;
			}
			else if (iJ == CONTACT_SECRETARY_GENERAL_VOTE)
			{
				m_piContactDelay[iJ] = 25;
			}
			else if (iJ == CONTACT_TRADE_MILITARY_UNITS)
			{
				m_piContactDelay[iJ] = 25;
			}
			else if (iJ == CONTACT_TRADE_WORKERS)
			{
				m_piContactDelay[iJ] = 30;
			}
			else if (iJ == CONTACT_TRADE_CONTACTS)
			{
				m_piContactDelay[iJ] = 20;
			}
			else if (iJ == CONTACT_TRADE_CORPORATION)
			{
				m_piContactDelay[iJ] = 50;
			}
			else if (iJ == CONTACT_TRADE_BUY_WAR)
			{
				m_piContactDelay[iJ] = 10;
			}
		}
	}
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

//======================================================================================================
//					CvReligionInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvReligionInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvReligionInfo::CvReligionInfo() :
m_iChar(0),
/************************************************************************************************/
/* TGA_INDEXATION						  03/17/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
m_iTGAIndex(-1),
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/
m_iHolyCityChar(0),
m_iTechPrereq(NO_TECH),
m_iFreeUnitClass(NO_UNITCLASS),
m_iNumFreeUnits(0),
m_iSpreadFactor(0),
m_iMissionType(NO_MISSION),
m_paiGlobalReligionCommerce(NULL),
m_paiHolyCityCommerce(NULL),
m_paiStateReligionCommerce(NULL),
m_piFlavorValue(NULL),
m_PropertyManipulators()
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
CvReligionInfo::~CvReligionInfo()
{
	SAFE_DELETE_ARRAY(m_paiGlobalReligionCommerce);
	SAFE_DELETE_ARRAY(m_paiHolyCityCommerce);
	SAFE_DELETE_ARRAY(m_paiStateReligionCommerce);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
}

int CvReligionInfo::getChar() const
{
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
/*	return m_iChar;
*/
	return m_iChar;
}

/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvReligionInfo::getTGAIndex() const
{
	return m_iTGAIndex;
}

void CvReligionInfo::setTGAIndex(int i)
{
	m_iTGAIndex = i;
}
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/
void CvReligionInfo::setChar(int i)
{
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	m_iChar = i;
*/
	m_iChar = 8550 + m_iTGAIndex * 2;
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/
}

int CvReligionInfo::getHolyCityChar() const
{
	return m_iHolyCityChar;
}

void CvReligionInfo::setHolyCityChar(int i)
{
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	m_iHolyCityChar = i;
*/
	m_iHolyCityChar = 8551 + m_iTGAIndex * 2;
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/
}

int CvReligionInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvReligionInfo::getFreeUnitClass() const
{
	return m_iFreeUnitClass;
}

int CvReligionInfo::getNumFreeUnits() const
{
	return m_iNumFreeUnits;
}

int CvReligionInfo::getSpreadFactor() const
{
	return m_iSpreadFactor;
}

int CvReligionInfo::getMissionType() const
{
	return m_iMissionType;
}

void CvReligionInfo::setMissionType(int iNewType)
{
	m_iMissionType = iNewType;
}

const TCHAR* CvReligionInfo::getTechButton() const
{
	return m_szTechButton;
}

void CvReligionInfo::setTechButton(const TCHAR* szVal)
{
	m_szTechButton=szVal;
}

const TCHAR* CvReligionInfo::getGenericTechButton() const
{
	return m_szGenericTechButton;
}

void CvReligionInfo::setGenericTechButton(const TCHAR* szVal)
{
	m_szGenericTechButton = szVal;
}

const TCHAR* CvReligionInfo::getMovieFile() const
{
	return m_szMovieFile;
}

void CvReligionInfo::setMovieFile(const TCHAR* szVal)
{
	m_szMovieFile = szVal;
}

const TCHAR* CvReligionInfo::getMovieSound() const
{
	return m_szMovieSound;
}

void CvReligionInfo::setMovieSound(const TCHAR* szVal)
{
	m_szMovieSound = szVal;
}

const TCHAR* CvReligionInfo::getButtonDisabled( void ) const
{
	static TCHAR szDisabled[512];

	szDisabled[0] = '\0';

	if ( getButton() && strlen(getButton()) > 4 )
	{
		strncpy( szDisabled, getButton(), strlen(getButton()) - 4 );
		szDisabled[strlen(getButton()) - 4] = '\0';
		strcat( szDisabled, "_D.dds" );
	}

	return szDisabled;
}

const TCHAR* CvReligionInfo::getSound() const
{
	return m_szSound;
}

void CvReligionInfo::setSound(const TCHAR* szVal)
{
	m_szSound=szVal;
}

void CvReligionInfo::setAdjectiveKey(const TCHAR* szVal)
{
	m_szAdjectiveKey = szVal;
}

const wchar* CvReligionInfo::getAdjectiveKey() const
{
	return m_szAdjectiveKey;
}

// Arrays

int CvReligionInfo::getGlobalReligionCommerce(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiGlobalReligionCommerce ? m_paiGlobalReligionCommerce[i] : 0;
}

int* CvReligionInfo::getGlobalReligionCommerceArray() const
{
	return m_paiGlobalReligionCommerce;
}

int CvReligionInfo::getHolyCityCommerce(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiHolyCityCommerce ? m_paiHolyCityCommerce[i] : 0;
}

int* CvReligionInfo::getHolyCityCommerceArray() const
{
	return m_paiHolyCityCommerce;
}

int CvReligionInfo::getStateReligionCommerce(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiStateReligionCommerce ? m_paiStateReligionCommerce[i] : 0;
}

int* CvReligionInfo::getStateReligionCommerceArray() const
{
	return m_paiStateReligionCommerce;
}

int CvReligionInfo::getFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

//
// read from xml
//
bool CvReligionInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereq");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnitClass");
	m_iFreeUnitClass = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iNumFreeUnits, L"iFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_iSpreadFactor, L"iSpreadFactor");
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iTGAIndex, L"iTGAIndex");
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/

	if (pXML->TryMoveToXmlFirstChild(L"GlobalReligionCommerces"))
	{
		pXML->SetCommerce(&m_paiGlobalReligionCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiGlobalReligionCommerce);

	if (pXML->TryMoveToXmlFirstChild(L"HolyCityCommerces"))
	{
		pXML->SetCommerce(&m_paiHolyCityCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiHolyCityCommerce);

	if (pXML->TryMoveToXmlFirstChild(L"StateReligionCommerces"))
	{
		pXML->SetCommerce(&m_paiStateReligionCommerce);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_paiStateReligionCommerce);

	pXML->GetChildXmlValByName(szTextVal, L"TechButton");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 01/16/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	CvXMLLoadUtilityModTools* p_szDirName = new CvXMLLoadUtilityModTools;
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setTechButton(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"GenericTechButton");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 01/16/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setGenericTechButton(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MovieFile");
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 01/16/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	p_szDirName->setLocationName( &szTextVal, (GC.getModDir()).c_str());
	SAFE_DELETE(p_szDirName);
/************************************************************************************************/
/* XML_MODULAR_ART_LOADING				 END												  */
/************************************************************************************************/
	setMovieFile(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MovieSound");
	setMovieSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Sound");
	setSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Adjective");
	setAdjectiveKey(szTextVal);

#ifdef C2C_BUILD
	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getFlavorTypes(), GC.getNumFlavorTypes());
#endif

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvReligionInfo::copyNonDefaults(CvReligionInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getFreeUnitClass() == iTextDefault) m_iFreeUnitClass = pClassInfo->getFreeUnitClass();

	if (getNumFreeUnits() == iDefault) m_iNumFreeUnits = pClassInfo->getNumFreeUnits();
	if (getSpreadFactor() == iDefault) m_iSpreadFactor = pClassInfo->getSpreadFactor();
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getTGAIndex() == iDefault) m_iTGAIndex = pClassInfo->getTGAIndex();
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/

	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++)
	{
		if ( getGlobalReligionCommerce(i) == iDefault && pClassInfo->getGlobalReligionCommerce(i) != iDefault)
		{
			if ( NULL == m_paiGlobalReligionCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiGlobalReligionCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiGlobalReligionCommerce[i] = pClassInfo->getGlobalReligionCommerce(i);
		}

		if ( getHolyCityCommerce(i) == iDefault && pClassInfo->getHolyCityCommerce(i) != iDefault)
		{
			if ( NULL == m_paiHolyCityCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiHolyCityCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiHolyCityCommerce[i] = pClassInfo->getHolyCityCommerce(i);
		}

		if ( getStateReligionCommerce(i) == iDefault && pClassInfo->getStateReligionCommerce(i) != iDefault)
		{
			if ( NULL == m_paiStateReligionCommerce )
			{
				CvXMLLoadUtility::InitList(&m_paiStateReligionCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_paiStateReligionCommerce[i] = pClassInfo->getStateReligionCommerce(i);
		}
	}
	if (getTechButton() == cDefault) setTechButton(pClassInfo->getTechButton());
	if (getGenericTechButton() == cDefault) setGenericTechButton(pClassInfo->getGenericTechButton());
	if (getMovieFile() == cDefault) setMovieFile(pClassInfo->getMovieFile());
	if (getMovieSound() == cDefault) setMovieSound(pClassInfo->getMovieSound());
	if (getSound() == cDefault) setSound(pClassInfo->getSound());
	if (getAdjectiveKey() == wDefault) setAdjectiveKey(CvString::format("%s",pClassInfo->getAdjectiveKey()).GetCString());

#ifdef C2C_BUILD
	for ( int i = 0; i < GC.getNumFlavorTypes(); i++ )
	{
		if ( getFlavorValue(i) == iDefault && pClassInfo->getFlavorValue(i) != iDefault)
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[i] = pClassInfo->getFlavorValue(i);
		}
	}
#endif

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

void CvReligionInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iFreeUnitClass);
	CheckSum(iSum, m_iNumFreeUnits);
	CheckSum(iSum, m_iSpreadFactor);
	CheckSum(iSum, m_iMissionType);

	// Arrays

	CheckSum(iSum, m_paiGlobalReligionCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiHolyCityCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiStateReligionCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());

	m_PropertyManipulators.getCheckSum(iSum);
}

CvPropertyManipulators* CvReligionInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}


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
m_iChar(0),
/************************************************************************************************/
/* TGA_INDEXATION						  03/17/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
m_iTGAIndex(-1),
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/
m_iHeadquarterChar(0),
m_iTechPrereq(NO_TECH),
m_iFreeUnitClass(NO_UNITCLASS),
m_iSpreadFactor(0),
m_iSpreadCost(0),
m_iMaintenance(0),
m_iMissionType(NO_MISSION),
m_iBonusProduced(NO_BONUS),
m_paiPrereqBonuses(NULL),
m_paiHeadquarterCommerce(NULL),
m_paiCommerceProduced(NULL),
m_paiYieldProduced(NULL)
/************************************************************************************************/
/* Afforess					  Start		 01/14/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
,m_iObsoleteTech(NO_TECH)
,m_iPrereqGameOption(NO_GAMEOPTION)
,m_iHealth(0)
,m_iHappiness(0)
,m_iMilitaryProductionModifier(0)
,m_iFreeXP(0)
,m_iSpread(0)
,m_paiPrereqBuildingClass(NULL)
,m_pabCompetingCorporation(NULL)
,m_piYieldChange(NULL)
,m_piCommerceChange(NULL)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
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
	SAFE_DELETE_ARRAY(m_paiPrereqBonuses);
	SAFE_DELETE_ARRAY(m_paiHeadquarterCommerce);
	SAFE_DELETE_ARRAY(m_paiCommerceProduced);
	SAFE_DELETE_ARRAY(m_paiYieldProduced);
/************************************************************************************************/
/* Afforess					  Start		 02/05/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	SAFE_DELETE_ARRAY(m_paiPrereqBuildingClass);
	SAFE_DELETE_ARRAY(m_pabCompetingCorporation);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piCommerceChange);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

}

int CvCorporationInfo::getChar() const
{
	return m_iChar;
}

/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvCorporationInfo::getTGAIndex() const
{
	return m_iTGAIndex;
}

void CvCorporationInfo::setTGAIndex(int i)
{
	m_iTGAIndex = i;
}
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/
void CvCorporationInfo::setChar(int i)
{
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	m_iChar = i;
*/
	m_iChar = 8550 + (TGA_RELIGIONS + m_iTGAIndex) * 2;
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/
}

int CvCorporationInfo::getHeadquarterChar() const
{
	return m_iHeadquarterChar;
}

void CvCorporationInfo::setHeadquarterChar(int i)
{
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	m_iHeadquarterChar = i;
*/
	m_iHeadquarterChar = 8551 + (TGA_RELIGIONS + m_iTGAIndex) * 2;
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/
}

int CvCorporationInfo::getTechPrereq() const
{
	return m_iTechPrereq;
}

int CvCorporationInfo::getFreeUnitClass() const
{
	return m_iFreeUnitClass;
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

const TCHAR* CvCorporationInfo::getMovieFile() const
{
	return m_szMovieFile;
}

void CvCorporationInfo::setMovieFile(const TCHAR* szVal)
{
	m_szMovieFile = szVal;
}

const TCHAR* CvCorporationInfo::getMovieSound() const
{
	return m_szMovieSound;
}

void CvCorporationInfo::setMovieSound(const TCHAR* szVal)
{
	m_szMovieSound = szVal;
}


const TCHAR* CvCorporationInfo::getSound() const
{
	return m_szSound;
}

void CvCorporationInfo::setSound(const TCHAR* szVal)
{
	m_szSound =szVal;
}

/************************************************************************************************/
/* Afforess					  Start		 01/14/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
int CvCorporationInfo::getObsoleteTech() const
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

int CvCorporationInfo::getPrereqBuildingClass(int i) const
{
	FAssertMsg(i < GC.getNumBuildingClassInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiPrereqBuildingClass ? m_paiPrereqBuildingClass[i] : false;
}

int CvCorporationInfo::getPrereqBuildingVectorSize()					{return m_aszPrereqBuildingClassforPass3.size();}
CvString CvCorporationInfo::getPrereqBuildingNamesVectorElement(int i)	{return m_aszPrereqBuildingClassforPass3[i];}
int CvCorporationInfo::getPrereqBuildingClassValuesVectorElement(int i)		{return m_aiPrereqBuildingClassforPass3[i];}

bool CvCorporationInfo::isCompetingCorporation(int i) const
{
	FAssertMsg(i < GC.getNumCorporationInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_pabCompetingCorporation ? m_pabCompetingCorporation[i] : false;
}

int CvCorporationInfo::getCompetingCorporationVectorSize()					{return m_aszCompetingCorporationforPass3.size();}
CvString CvCorporationInfo::getCompetingCorporationNamesVectorElement(int i)	{return m_aszCompetingCorporationforPass3[i];}
bool CvCorporationInfo::getCompetingCorporationValuesVectorElement(int i)		{return m_abCompetingCorporationforPass3[i];}

int CvCorporationInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvCorporationInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}

int CvCorporationInfo::getCommerceChange(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_piCommerceChange ? m_piCommerceChange[i] : 0;
}

int* CvCorporationInfo::getCommerceChangeArray() const
{
	return m_piCommerceChange;
}

/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

// Arrays

int CvCorporationInfo::getPrereqBonus(int i) const
{
	FAssertMsg(i < GC.getNUM_CORPORATION_PREREQ_BONUSES(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiPrereqBonuses ? m_paiPrereqBonuses[i] : NO_BONUS;
}

int CvCorporationInfo::getHeadquarterCommerce(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiHeadquarterCommerce ? m_paiHeadquarterCommerce[i] : 0;
}

int* CvCorporationInfo::getHeadquarterCommerceArray() const
{
	return m_paiHeadquarterCommerce;
}

int CvCorporationInfo::getCommerceProduced(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiCommerceProduced ? m_paiCommerceProduced[i] : 0;
}

int* CvCorporationInfo::getCommerceProducedArray() const
{
	return m_paiCommerceProduced;
}

int CvCorporationInfo::getYieldProduced(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	return m_paiYieldProduced ? m_paiYieldProduced[i] : 0;
}

int* CvCorporationInfo::getYieldProducedArray() const
{
	return m_paiYieldProduced;
}

//
// read from xml
//
bool CvCorporationInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TechPrereq");
	m_iTechPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeUnitClass");
	m_iFreeUnitClass = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iSpreadFactor, L"iSpreadFactor");
	pXML->GetOptionalChildXmlValByName(&m_iSpreadCost, L"iSpreadCost");
	pXML->GetOptionalChildXmlValByName(&m_iMaintenance, L"iMaintenance");
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iTGAIndex, L"iTGAIndex");
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/

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


	if (pXML->TryMoveToXmlFirstChild(L"PrereqBonuses"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		FAssertMsg(0 < GC.getNUM_CORPORATION_PREREQ_BONUSES(),"Allocating zero or less memory in CvCorporationInfo::read");

		if (0 < iNumSibs)
		{
			pXML->CvXMLLoadUtility::InitList(&m_paiPrereqBonuses, GC.getNUM_CORPORATION_PREREQ_BONUSES(), -1);
			if (pXML->GetChildXmlVal(szTextVal))
			{
				FAssertMsg((iNumSibs <= GC.getNUM_CORPORATION_PREREQ_BONUSES()) , "There are more siblings than memory allocated for them in CvCorporationInfo::read");
				for (int j=0; j<iNumSibs; ++j)
				{
					m_paiPrereqBonuses[j] = pXML->GetInfoClass(szTextVal);
					if (!pXML->GetNextXmlVal(szTextVal))
					{
						break;
					}
				}

				pXML->MoveToXmlParent();
			}
		}
		else
		{
			SAFE_DELETE_ARRAY(m_paiPrereqBonuses);
		}

		pXML->MoveToXmlParent();
	}
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	else
	{
		SAFE_DELETE_ARRAY(m_paiPrereqBonuses);
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"BonusProduced");
	m_iBonusProduced = pXML->GetInfoClass(szTextVal);
/************************************************************************************************/
/* XMLCOPY								 11/19/07								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	pXML->GetChildXmlValByName(szTextVal, L"MovieFile");
	setMovieFile(szTextVal);

	pXML->GetChildXmlValByName(szTextVal, L"MovieSound");
	setMovieSound(szTextVal);
*/
	pXML->GetOptionalChildXmlValByName(szTextVal, L"MovieFile");
	setMovieFile(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MovieSound");
	setMovieSound(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Sound");
	setSound(szTextVal);

/************************************************************************************************/
/* Afforess					  Start		 01/14/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iSpread, L"iSpread");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryProductionModifier, L"iMilitaryProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeXP, L"iFreeXP");

	if (pXML->TryMoveToXmlFirstChild(L"PrereqBuildingClasses"))
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
						m_aszPrereqBuildingClassforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiPrereqBuildingClassforPass3.push_back(iTemp);
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
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	m_PropertyManipulators.read(pXML);

	return true;
}

void CvCorporationInfo::copyNonDefaults(CvCorporationInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvHotkeyInfo::copyNonDefaults(pClassInfo, pXML);

	if (getTechPrereq() == iTextDefault) m_iTechPrereq = pClassInfo->getTechPrereq();
	if (getFreeUnitClass() == iTextDefault) m_iFreeUnitClass = pClassInfo->getFreeUnitClass();
	if (getSpreadFactor() == iDefault) m_iSpreadFactor = pClassInfo->getSpreadFactor();
	if (getSpreadCost() == iDefault) m_iSpreadCost = pClassInfo->getSpreadCost();
	if (getMaintenance() == iDefault) m_iMaintenance = pClassInfo->getMaintenance();
/************************************************************************************************/
/* TGA_INDEXATION						  01/21/08								MRGENIE	  */
/*																							  */
/*																							  */
/************************************************************************************************/
	if (getTGAIndex() == iDefault) m_iTGAIndex = pClassInfo->getTGAIndex();
/************************************************************************************************/
/* TGA_INDEXATION						  END												  */
/************************************************************************************************/

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

	if (!m_paiPrereqBonuses) CvXMLLoadUtility::InitList(&m_paiPrereqBonuses, GC.getNUM_CORPORATION_PREREQ_BONUSES());
	for ( int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); i++ )
	{
		if ( m_paiPrereqBonuses[i] == iTextDefault )
		{
			m_paiPrereqBonuses[i] = pClassInfo->getPrereqBonus(i);
		}
	}

	if (getBonusProduced() == iTextDefault) m_iBonusProduced = pClassInfo->getBonusProduced();

	if (getMovieFile() == cDefault) setMovieFile(pClassInfo->getMovieFile());
	if (getMovieSound() == cDefault) setMovieSound(pClassInfo->getMovieSound());
	if (getSound() == cDefault) setSound(pClassInfo->getSound());

/************************************************************************************************/
/* Afforess					  Start		 02/05/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/

	for ( int i = 0; i < pClassInfo->getPrereqBuildingVectorSize(); i++ )
	{
		m_aiPrereqBuildingClassforPass3.push_back(pClassInfo->getPrereqBuildingClassValuesVectorElement(i));
		m_aszPrereqBuildingClassforPass3.push_back(pClassInfo->getPrereqBuildingNamesVectorElement(i));
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
				pXML->InitList(&m_piYieldChange, NUM_YIELD_TYPES);
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
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);
}

void CvCorporationInfo::getCheckSum(unsigned int& iSum)
{
	CheckSum(iSum, m_iTechPrereq);
	CheckSum(iSum, m_iFreeUnitClass);
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

	CheckSum(iSum, m_paiPrereqBuildingClass, GC.getNumBuildingClassInfos());

	CheckSum(iSum, m_pabCompetingCorporation, GC.getNumCorporationInfos());

	CheckSum(iSum, m_piYieldChange, NUM_YIELD_TYPES);
	CheckSum(iSum, m_piCommerceChange, NUM_COMMERCE_TYPES);

	// Arrays

	CheckSum(iSum, m_paiPrereqBonuses, GC.getNUM_CORPORATION_PREREQ_BONUSES());
	CheckSum(iSum, m_paiHeadquarterCommerce, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiCommerceProduced, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiYieldProduced, NUM_YIELD_TYPES);

	m_PropertyManipulators.getCheckSum(iSum);
}

/************************************************************************************************/
/* Afforess					  Start		 02/05/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvCorporationInfo::readPass3()
{
	m_paiPrereqBuildingClass = new int[GC.getNumBuildingClassInfos()];
	for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		m_paiPrereqBuildingClass[iI] = 0;
	}
	if (!m_aiPrereqBuildingClassforPass3.empty() && !m_aszPrereqBuildingClassforPass3.empty())
	{
		int iNumLoad = m_aiPrereqBuildingClassforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszPrereqBuildingClassforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumBuildingClassInfos())
				m_paiPrereqBuildingClass[iTempIndex] = m_aiPrereqBuildingClassforPass3[iI];
		}
		m_aszPrereqBuildingClassforPass3.clear();
		m_aiPrereqBuildingClassforPass3.clear();
	}

	m_pabCompetingCorporation = new bool[GC.getNumCorporationInfos()];
	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		m_pabCompetingCorporation[iI] = 0;
	}
	if (!m_abCompetingCorporationforPass3.empty() && !m_aszCompetingCorporationforPass3.empty())
	{
		int iNumLoad = m_abCompetingCorporationforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszCompetingCorporationforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCorporationInfos())
				m_pabCompetingCorporation[iTempIndex] = m_abCompetingCorporationforPass3[iI];
		}
		m_aszCompetingCorporationforPass3.clear();
		m_abCompetingCorporationforPass3.clear();
	}

	return true;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

CvPropertyManipulators* CvCorporationInfo::getPropertyManipulators()
{
	return &m_PropertyManipulators;
}


//======================================================================================================
//					CvTraitInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvTraitInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvTraitInfo::CvTraitInfo() :
m_iHealth(0),
m_iHappiness(0),
m_iMaxAnarchy(-1),
m_iUpkeepModifier(0),
m_iLevelExperienceModifier(0),
m_iGreatPeopleRateModifier(0),
m_iGreatGeneralRateModifier(0),
m_iDomesticGreatGeneralRateModifier(0),
m_iMaxGlobalBuildingProductionModifier(0),
m_iMaxTeamBuildingProductionModifier(0),
m_iMaxPlayerBuildingProductionModifier(0),

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTrait Effects														*/
/********************************************************************************/
m_iRevIdxLocal(0),
m_iRevIdxNational(0),
m_iRevIdxDistanceModifier(0),
m_iRevIdxHolyCityGood(0),
m_iRevIdxHolyCityBad(0),
m_fRevIdxNationalityMod(0),
m_fRevIdxGoodReligionMod(0),
m_fRevIdxBadReligionMod(0),
m_bNonStateReligionCommerce(false),
m_bUpgradeAnywhere(false),
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

m_paiExtraYieldThreshold(NULL),
m_paiTradeYieldModifier(NULL),
m_paiCommerceChange(NULL),
m_paiCommerceModifier(NULL)
/************************************************************************************************/
/* Afforess					  Start		 08/26/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
m_pabFreePromotionUnitCombat(NULL),
m_pabFreePromotion(NULL)
*/
,m_ppbFreePromotionUnitCombats(NULL)
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
,m_PropertyManipulators()
//TB Traits Mods begin
,m_PropertyManipulatorsNull()
//Textual References
,m_iPrereqTrait(NO_TRAIT)
,m_iPrereqOrTrait1(NO_TRAIT)
,m_iPrereqOrTrait2(NO_TRAIT)
,m_ePromotionLine(NO_PROMOTIONLINE)
,m_iGreatPeopleUnitClass(NO_UNITCLASS)
,m_ePrereqTech(NO_TECH)
//Team Project (6)
,m_eEraAdvanceFreeSpecialistType(NO_SPECIALIST)
,m_iGoldenAgeonBirthofGreatPeopleType(NO_UNITCLASS)
//integers
,m_iWarWearinessAccumulationModifier(0)
,m_iCivicAnarchyTimeModifier(0)
,m_iReligiousAnarchyTimeModifier(0)
,m_iImprovementUpgradeRateModifier(0)
,m_iWorkerSpeedModifier(0)
,m_iMaxConscript(0)
,m_iDistanceMaintenanceModifier(0)
,m_iNumCitiesMaintenanceModifier(0)
,m_iCorporationMaintenanceModifier(0)
,m_iStateReligionGreatPeopleRateModifier(0)
,m_iFreeExperience(0)
,m_iBaseFreeUnits(0)
,m_iBaseFreeMilitaryUnits(0)
,m_iFreeUnitsPopulationPercent(0)
,m_iFreeMilitaryUnitsPopulationPercent(0)
,m_iGoldPerUnit(0)
,m_iGoldPerMilitaryUnit(0)
,m_iHappyPerMilitaryUnit(0)
,m_iLargestCityHappiness(0)
,m_iFreeSpecialist(0)
,m_iTradeRoutes(0)
,m_iStateReligionHappiness(0)
,m_iNonStateReligionHappiness(0)
,m_iStateReligionUnitProductionModifier(0)
,m_iStateReligionBuildingProductionModifier(0)
,m_iStateReligionFreeExperience(0)
,m_iExpInBorderModifier(0)
,m_iCityDefenseBonus(0)
,m_iMilitaryProductionModifier(0)
,m_iAttitudeModifier(0)
,m_iLinePriority(0)
,m_iEspionageDefense(0)
,m_iMinAnarchy(0)
,m_iMaxTradeRoutesChange(0)
,m_iGoldenAgeDurationModifier(0)
,m_iGreatPeopleRateChange(0)
,m_iHurryAngerModifier(0)
,m_iHurryCostModifier(0)
,m_iEnemyWarWearinessModifier(0)
,m_iForeignTradeRouteModifier(0)
,m_iBombardDefense(0)
,m_iUnitUpgradePriceModifier(0)
,m_iCoastalTradeRoutes(0)
//Team Project (6)
,m_iGlobalPopulationgrowthratepercentage(0)
,m_iCityStartCulture(0)
,m_iGlobalAirUnitCapacity(0)
,m_iCapitalXPModifier(0)
,m_iHolyCityofStateReligionXPModifier(0)
,m_iHolyCityofNonStateReligionXPModifier(0)
,m_iBonusPopulationinNewCities(0)
,m_iMissileRange(0)
,m_iFlightOperationRange(0)
,m_iNavalCargoSpace(0)
,m_iMissileCargoSpace(0)
,m_iNationalCaptureProbabilityModifier(0)
,m_iNationalCaptureResistanceModifier(0)
,m_iStateReligionSpreadProbabilityModifier(0)
,m_iNonStateReligionSpreadProbabilityModifier(0)
,m_iFreedomFighterChange(0)
//booleans
,m_bMilitaryFoodProduction(false)
,m_bNegativeTrait(false)
,m_bImpurePropertyManipulators(false)
,m_bImpurePromotions(false)
,m_bCivilizationTrait(false)
,m_bAllowsInquisitions(false)
,m_bCoastalAIInfluence(false)
,m_bBarbarianSelectionOnly(false)
,m_bCitiesStartwithStateReligion(false)
,m_bDraftsOnCityCapture(false)
,m_bFreeSpecialistperWorldWonder(false)
,m_bFreeSpecialistperNationalWonder(false)
,m_bFreeSpecialistperTeamProject(false)
,m_bExtraGoody(false)
,m_bAllReligionsActive(false)
,m_bBansNonStateReligions(false)
,m_bFreedomFighter(false)
//arrays
,m_ppaiSpecialistYieldChange(NULL)
,m_bAnySpecialistYieldChanges(false)
,m_piYieldModifier(NULL)
,m_piCapitalYieldModifier(NULL)
,m_piCapitalCommerceModifier(NULL)
,m_piSpecialistExtraCommerce(NULL)
,m_piSpecialistExtraYield(NULL)
,m_piYieldChange(NULL)
,m_ppaiSpecialistCommerceChange(NULL)
,m_bAnySpecialistCommerceChanges(false)
,m_piFlavorValue(NULL)
,m_paiLessYieldThreshold(NULL)
,m_piSeaPlotYieldChanges(NULL)
,m_ppaiImprovementYieldChange(NULL)
,m_bAnyImprovementYieldChanges(false)
//Team Project (7)
,m_piGoldenAgeYieldChanges(NULL)
,m_piGoldenAgeCommerceChanges(NULL)
//For Pure Traits
,m_paiExtraYieldThresholdFiltered(NULL)
,m_paiTradeYieldModifierFiltered(NULL)
,m_paiCommerceChangeFiltered(NULL)
,m_paiCommerceModifierFiltered(NULL)
,m_ppaiSpecialistYieldChangeFiltered(NULL)
,m_piYieldModifierFiltered(NULL)
,m_piCapitalYieldModifierFiltered(NULL)
,m_piCapitalCommerceModifierFiltered(NULL)
,m_piSpecialistExtraCommerceFiltered(NULL)
,m_piSpecialistExtraYieldFiltered(NULL)
,m_piYieldChangeFiltered(NULL)
,m_ppaiSpecialistCommerceChangeFiltered(NULL)
,m_paiLessYieldThresholdFiltered(NULL)
,m_piSeaPlotYieldChangesFiltered(NULL)
,m_ppaiImprovementYieldChangeFiltered(NULL)
//Team Project (7)
,m_piGoldenAgeYieldChangesFiltered(NULL)
,m_piGoldenAgeCommerceChangesFiltered(NULL)
//TB Traits Mods end
{
}

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvTraitInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvTraitInfo::~CvTraitInfo()
{

	SAFE_DELETE_ARRAY(m_paiExtraYieldThreshold);
	SAFE_DELETE_ARRAY(m_paiTradeYieldModifier);
	SAFE_DELETE_ARRAY(m_paiCommerceChange);
	SAFE_DELETE_ARRAY(m_paiCommerceModifier);

/************************************************************************************************/
/* Afforess					  Start		 08/26/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	SAFE_DELETE_ARRAY(m_pabFreePromotionUnitCombat);
	SAFE_DELETE_ARRAY(m_pabFreePromotion);
*/
	if (m_ppbFreePromotionUnitCombats != NULL)
	{
		for (int iI=0;iI<GC.getNumPromotionInfos();iI++)
		{
			SAFE_DELETE_ARRAY(m_ppbFreePromotionUnitCombats[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppbFreePromotionUnitCombats);
	}
	//TB Traits begin
	//Arrays
	if (m_ppaiSpecialistYieldChange != NULL)
	{
		for(int i=0;i<GC.getNumSpecialistInfos();i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChange[i]);
		}
		SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChange);
	}
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	SAFE_DELETE_ARRAY(m_piCapitalCommerceModifier);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraYield);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_paiLessYieldThreshold);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChanges);
//Team Project (7)
	SAFE_DELETE_ARRAY(m_piGoldenAgeYieldChanges);
	SAFE_DELETE_ARRAY(m_piGoldenAgeCommerceChanges);
	if (m_ppaiSpecialistCommerceChange != NULL)
	{
		for(int i=0;i<GC.getNumSpecialistInfos();i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiSpecialistCommerceChange[i]);
		}
		SAFE_DELETE_ARRAY(m_ppaiSpecialistCommerceChange);
	}
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	if (m_ppaiImprovementYieldChange != NULL)
	{
		for(int i=0;i<GC.getNumImprovementInfos();i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiImprovementYieldChange[i]);
		}
		SAFE_DELETE_ARRAY(m_ppaiImprovementYieldChange);
	}
//	 int vector utilizing struct with delayed resolution
	for (int i=0; i<(int)m_aBuildWorkerSpeedModifierTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aBuildWorkerSpeedModifierTypes[i]));
	}

	for (int i=0; i<(int)m_aDisallowedTraitTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aDisallowedTraitTypes[i]));
	}

	for (int i=0; i<(int)m_aBuildingProductionModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aBuildingProductionModifiers[i]));
	}

	for (int i=0; i<(int)m_aSpecialBuildingProductionModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aSpecialBuildingProductionModifiers[i]));
	}

	for (int i=0; i<(int)m_aBuildingHappinessModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aBuildingHappinessModifiers[i]));
	}

	for (int i=0; i<(int)m_aUnitProductionModifiers.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aUnitProductionModifiers[i]));
	}

	for (int i=0; i<(int)m_aCivicOptionNoUpkeepTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aCivicOptionNoUpkeepTypes[i]));
	}

//Team Project (8)
	//for Pure Traits
	SAFE_DELETE_ARRAY(m_paiExtraYieldThresholdFiltered);
	SAFE_DELETE_ARRAY(m_paiTradeYieldModifierFiltered);
	SAFE_DELETE_ARRAY(m_paiCommerceChangeFiltered);
	SAFE_DELETE_ARRAY(m_paiCommerceModifierFiltered);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChangesFiltered);
//Team Project (7)
	SAFE_DELETE_ARRAY(m_piGoldenAgeYieldChangesFiltered);
	SAFE_DELETE_ARRAY(m_piGoldenAgeCommerceChangesFiltered);
	if (m_ppaiSpecialistYieldChangeFiltered != NULL)
	{
		for(int i=0;i<GC.getNumSpecialistInfos();i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChangeFiltered[i]);
		}
		SAFE_DELETE_ARRAY(m_ppaiSpecialistYieldChangeFiltered);
	}
	SAFE_DELETE_ARRAY(m_piYieldModifierFiltered);
	SAFE_DELETE_ARRAY(m_piCapitalYieldModifierFiltered);
	SAFE_DELETE_ARRAY(m_piCapitalCommerceModifierFiltered);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerceFiltered);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraYieldFiltered);
	SAFE_DELETE_ARRAY(m_piYieldChangeFiltered);
	SAFE_DELETE_ARRAY(m_paiLessYieldThresholdFiltered);
	if (m_ppaiSpecialistCommerceChangeFiltered != NULL)
	{
		for(int i=0;i<GC.getNumSpecialistInfos();i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiSpecialistCommerceChangeFiltered[i]);
		}
		SAFE_DELETE_ARRAY(m_ppaiSpecialistCommerceChangeFiltered);
	}
	if (m_ppaiImprovementYieldChangeFiltered != NULL)
	{
		for(int i=0;i<GC.getNumImprovementInfos();i++)
		{
			SAFE_DELETE_ARRAY(m_ppaiImprovementYieldChangeFiltered[i]);
		}
		SAFE_DELETE_ARRAY(m_ppaiImprovementYieldChangeFiltered);
	}
	//TB Traits end
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
}

int CvTraitInfo::getHealth() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHealth > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iHealth < 0)
		{
			return 0;
		}
	}
	return m_iHealth;
}

int CvTraitInfo::getHappiness() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHappiness > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iHappiness < 0)
		{
			return 0;
		}
	}
	return m_iHappiness;
}

int CvTraitInfo::getMaxAnarchy() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxAnarchy > -1)
		{
				return -1;
		}
	}
	return m_iMaxAnarchy;
}

int CvTraitInfo::getUpkeepModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iUpkeepModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iUpkeepModifier > 0)
		{
			return 0;
		}
	}
	return m_iUpkeepModifier;
}

int CvTraitInfo::getLevelExperienceModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iLevelExperienceModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iLevelExperienceModifier > 0)
		{
			return 0;
		}
	}
	return m_iLevelExperienceModifier;
}

int CvTraitInfo::getGreatPeopleRateModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGreatPeopleRateModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iGreatPeopleRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iGreatPeopleRateModifier;
}

int CvTraitInfo::getGreatGeneralRateModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGreatGeneralRateModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iGreatGeneralRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iGreatGeneralRateModifier;
}

int CvTraitInfo::getDomesticGreatGeneralRateModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iDomesticGreatGeneralRateModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iDomesticGreatGeneralRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iDomesticGreatGeneralRateModifier;
}

int CvTraitInfo::getMaxGlobalBuildingProductionModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxGlobalBuildingProductionModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iMaxGlobalBuildingProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iMaxGlobalBuildingProductionModifier;
}

int CvTraitInfo::getMaxTeamBuildingProductionModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxTeamBuildingProductionModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iMaxTeamBuildingProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iMaxTeamBuildingProductionModifier;
}

int CvTraitInfo::getMaxPlayerBuildingProductionModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxPlayerBuildingProductionModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iMaxPlayerBuildingProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iMaxPlayerBuildingProductionModifier;
}

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTrait Effects														*/
/********************************************************************************/

int CvTraitInfo::getRevIdxLocal() const
//Note: Positive is penalty
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxLocal < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxLocal > 0)
		{
			return 0;
		}
	}
	return m_iRevIdxLocal;
}

int CvTraitInfo::getRevIdxNational() const
{
//Note: Positive is Penalty
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxNational < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxNational > 0)
		{
			return 0;
		}
	}
	return m_iRevIdxNational;
}

int CvTraitInfo::getRevIdxDistanceModifier() const
{
//Note: Positive is penalty
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxDistanceModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxDistanceModifier > 0)
		{
			return 0;
		}
	}
	return m_iRevIdxDistanceModifier;
}

int CvTraitInfo::getRevIdxHolyCityGood() const
{
//Note: Positive is bonus
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxHolyCityGood > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxHolyCityGood < 0)
		{
			return 0;
		}
	}
	return m_iRevIdxHolyCityGood;
}

int CvTraitInfo::getRevIdxHolyCityBad() const
{
//Note: Positive is Penalty
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iRevIdxHolyCityBad < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iRevIdxHolyCityBad > 0)
		{
			return 0;
		}
	}
	return m_iRevIdxHolyCityBad;
}

float CvTraitInfo::getRevIdxNationalityMod() const
{
//Note: Positive is Penalty
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_fRevIdxNationalityMod < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_fRevIdxNationalityMod > 0)
		{
			return 0;
		}
	}
	return m_fRevIdxNationalityMod;
}

float CvTraitInfo::getRevIdxBadReligionMod() const
{
//Note: Positive is Penalty
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_fRevIdxBadReligionMod < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_fRevIdxBadReligionMod > 0)
		{
			return 0;
		}
	}
	return m_fRevIdxBadReligionMod;
}

float CvTraitInfo::getRevIdxGoodReligionMod() const
{
//Note: Positive is Bonus
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_fRevIdxGoodReligionMod > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_fRevIdxGoodReligionMod < 0)
		{
			return 0;
		}
	}
	return m_fRevIdxGoodReligionMod;
}

bool CvTraitInfo::isNonStateReligionCommerce() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bNonStateReligionCommerce)
		{
				return false;
		}
	}
	return m_bNonStateReligionCommerce;
}

bool CvTraitInfo::isUpgradeAnywhere() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bUpgradeAnywhere)
		{
				return false;
		}
	}
	return m_bUpgradeAnywhere;
}
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

const TCHAR* CvTraitInfo::getShortDescription() const
{
	return m_szShortDescription;
}

void CvTraitInfo::setShortDescription(const TCHAR* szVal)
{
	m_szShortDescription = szVal;
}

// Arrays

int CvTraitInfo::getExtraYieldThreshold(int i) const
{
	if (m_paiExtraYieldThreshold)
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS) && m_paiExtraYieldThresholdFiltered)
		{
			if (isNegativeTrait() && m_paiExtraYieldThreshold[i] > 0)
			{
					m_paiExtraYieldThresholdFiltered[i] = -1;
			}
			else
			{
				m_paiExtraYieldThresholdFiltered[i] = -1;
				m_paiExtraYieldThresholdFiltered[i] = m_paiExtraYieldThreshold[i];
			}
			return m_paiExtraYieldThresholdFiltered ? m_paiExtraYieldThresholdFiltered[i] : 0;
		}
	}
	return m_paiExtraYieldThreshold ? m_paiExtraYieldThreshold[i] : 0;
}

int CvTraitInfo::getTradeYieldModifier(int i) const
{
	if (m_paiTradeYieldModifier)
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS) && m_paiTradeYieldModifierFiltered)
		{
			if (isNegativeTrait() && m_paiTradeYieldModifier[i] > 0)
			{
					m_paiTradeYieldModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_paiTradeYieldModifier[i] < 0)
			{
				m_paiTradeYieldModifierFiltered[i] = 0;
			}
			else
			{
				m_paiTradeYieldModifierFiltered[i] = m_paiTradeYieldModifier[i];
			}
			return m_paiTradeYieldModifierFiltered ? m_paiTradeYieldModifierFiltered[i] : 0;
		}
	}
	return m_paiTradeYieldModifier ? m_paiTradeYieldModifier[i] : 0;
}

int CvTraitInfo::getCommerceChange(int i) const
{
	if (m_paiCommerceChange)
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS) && m_paiCommerceChangeFiltered)
		{
			if (isNegativeTrait() && m_paiCommerceChange[i] > 0)
			{
					m_paiCommerceChangeFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_paiCommerceChange[i] < 0)
			{
				m_paiCommerceChangeFiltered[i] = 0;
			}
			else
			{
				m_paiCommerceChangeFiltered[i] = m_paiCommerceChange[i];
			}
			return m_paiCommerceChangeFiltered ? m_paiCommerceChangeFiltered[i] : 0;
		}
	}
	return m_paiCommerceChange ? m_paiCommerceChange[i] : 0;
}

int CvTraitInfo::getCommerceModifier(int i) const
{
	if (m_paiCommerceModifier)
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS) && m_paiCommerceModifierFiltered)
		{
			if (isNegativeTrait() && m_paiCommerceModifier[i] > 0)
			{
					m_paiCommerceModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_paiCommerceModifier[i] < 0)
			{
				m_paiCommerceModifierFiltered[i] = 0;
			}
			else
			{
				m_paiCommerceModifierFiltered[i] = m_paiCommerceModifier[i];
			}
			return m_paiCommerceModifierFiltered ? m_paiCommerceModifierFiltered[i] : 0;
		}
	}
	return m_paiCommerceModifier ? m_paiCommerceModifier[i] : 0;
}


/************************************************************************************************/
/* Afforess					  Start		 08/26/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
int CvTraitInfo::isFreePromotion(int i) const
{
	return false;//m_pabFreePromotion ? m_pabFreePromotion[i] : -1;
}

int CvTraitInfo::isFreePromotionUnitCombat(int i) const
{
	return false;//m_pabFreePromotionUnitCombat ? m_pabFreePromotionUnitCombat[i] : -1;
}
*/

bool CvTraitInfo::isFreePromotionUnitCombats(int i, int j) const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS) && isImpurePromotions())
	{
		return false;
	}
	return (m_ppbFreePromotionUnitCombats && m_ppbFreePromotionUnitCombats[i]) ? m_ppbFreePromotionUnitCombats[i][j] : false;
}

//TB Traits Mods begin

//Textual References

int CvTraitInfo::getPrereqTrait() const
{
	return m_iPrereqTrait;
}

int CvTraitInfo::getPrereqOrTrait1() const
{
	return m_iPrereqOrTrait1;
}

int CvTraitInfo::getPrereqOrTrait2() const
{
	return m_iPrereqOrTrait2;
}

PromotionLineTypes CvTraitInfo::getPromotionLine() const
{
	return m_ePromotionLine;
}

int CvTraitInfo::getGreatPeopleUnitClass() const
{
	return m_iGreatPeopleUnitClass;
}

TechTypes CvTraitInfo::getPrereqTech() const
{
	return m_ePrereqTech;
}


//Team Project (6)
SpecialistTypes CvTraitInfo::getEraAdvanceFreeSpecialistType() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_eEraAdvanceFreeSpecialistType != NO_SPECIALIST)
		{
				return NO_SPECIALIST;
		}
	}
	return m_eEraAdvanceFreeSpecialistType;
}

int CvTraitInfo::getGoldenAgeonBirthofGreatPeopleType() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGoldenAgeonBirthofGreatPeopleType != NO_UNITCLASS)
		{
				return NO_UNITCLASS;
		}
	}
	return m_iGoldenAgeonBirthofGreatPeopleType;
}

//integers
int CvTraitInfo::getWarWearinessAccumulationModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iWarWearinessAccumulationModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iWarWearinessAccumulationModifier > 0)
		{
			return 0;
		}
	}
	return m_iWarWearinessAccumulationModifier;
}

int CvTraitInfo::getCivicAnarchyTimeModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCivicAnarchyTimeModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iCivicAnarchyTimeModifier > 0)
		{
			return 0;
		}
	}
	return m_iCivicAnarchyTimeModifier;
}

int CvTraitInfo::getReligiousAnarchyTimeModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iReligiousAnarchyTimeModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iReligiousAnarchyTimeModifier > 0)
		{
			return 0;
		}
	}
	return m_iReligiousAnarchyTimeModifier;
}

int CvTraitInfo::getImprovementUpgradeRateModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iImprovementUpgradeRateModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iImprovementUpgradeRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iImprovementUpgradeRateModifier;
}

int CvTraitInfo::getWorkerSpeedModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iWorkerSpeedModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iWorkerSpeedModifier < 0)
		{
			return 0;
		}
	}
	return m_iWorkerSpeedModifier;
}

int CvTraitInfo::getMaxConscript() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxConscript > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iMaxConscript < 0)
		{
			return 0;
		}
	}
	return m_iMaxConscript;
}

int CvTraitInfo::getDistanceMaintenanceModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iDistanceMaintenanceModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iDistanceMaintenanceModifier > 0)
		{
			return 0;
		}
	}
	return m_iDistanceMaintenanceModifier;
}

int CvTraitInfo::getNumCitiesMaintenanceModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNumCitiesMaintenanceModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iNumCitiesMaintenanceModifier > 0)
		{
			return 0;
		}
	}
	return m_iNumCitiesMaintenanceModifier;
}

int CvTraitInfo::getCorporationMaintenanceModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCorporationMaintenanceModifier < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iCorporationMaintenanceModifier > 0)
		{
			return 0;
		}
	}
	return m_iCorporationMaintenanceModifier;
}

int CvTraitInfo::getStateReligionGreatPeopleRateModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionGreatPeopleRateModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionGreatPeopleRateModifier < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionGreatPeopleRateModifier;
}

int CvTraitInfo::getFreeExperience() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFreeExperience > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iFreeExperience < 0)
		{
			return 0;
		}
	}
	return m_iFreeExperience;
}

int CvTraitInfo::getBaseFreeUnits() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iBaseFreeUnits > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iBaseFreeUnits < 0)
		{
			return 0;
		}
	}
	return m_iBaseFreeUnits;
}

int CvTraitInfo::getBaseFreeMilitaryUnits() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iBaseFreeMilitaryUnits > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iBaseFreeMilitaryUnits < 0)
		{
			return 0;
		}
	}
	return m_iBaseFreeMilitaryUnits;
}

int CvTraitInfo::getFreeUnitsPopulationPercent() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFreeUnitsPopulationPercent > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iFreeUnitsPopulationPercent < 0)
		{
			return 0;
		}
	}
	return m_iFreeUnitsPopulationPercent;
}

int CvTraitInfo::getFreeMilitaryUnitsPopulationPercent() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFreeMilitaryUnitsPopulationPercent > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iFreeMilitaryUnitsPopulationPercent < 0)
		{
			return 0;
		}
	}
	return m_iFreeMilitaryUnitsPopulationPercent;
}

int CvTraitInfo::getGoldPerUnit() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGoldPerUnit < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iGoldPerUnit > 0)
		{
			return 0;
		}
	}
	return m_iGoldPerUnit;
}

int CvTraitInfo::getGoldPerMilitaryUnit() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGoldPerMilitaryUnit < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iGoldPerMilitaryUnit > 0)
		{
			return 0;
		}
	}
	return m_iGoldPerMilitaryUnit;
}

int CvTraitInfo::getHappyPerMilitaryUnit() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHappyPerMilitaryUnit > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iHappyPerMilitaryUnit < 0)
		{
			return 0;
		}
	}
	return m_iHappyPerMilitaryUnit;
}

int CvTraitInfo::getLargestCityHappiness() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iLargestCityHappiness > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iLargestCityHappiness < 0)
		{
			return 0;
		}
	}
	return m_iLargestCityHappiness;
}

int CvTraitInfo::getFreeSpecialist() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFreeSpecialist > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iFreeSpecialist < 0)
		{
			return 0;
		}
	}
	return m_iFreeSpecialist;
}

int CvTraitInfo::getTradeRoutes() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iTradeRoutes > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iTradeRoutes < 0)
		{
			return 0;
		}
	}
	return m_iTradeRoutes;
}

int CvTraitInfo::getStateReligionHappiness() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionHappiness > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionHappiness < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionHappiness;
}

int CvTraitInfo::getNonStateReligionHappiness() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNonStateReligionHappiness > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iNonStateReligionHappiness < 0)
		{
			return 0;
		}
	}
	return m_iNonStateReligionHappiness;
}

int CvTraitInfo::getStateReligionUnitProductionModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionUnitProductionModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionUnitProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionUnitProductionModifier;
}

int CvTraitInfo::getStateReligionBuildingProductionModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionBuildingProductionModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionBuildingProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionBuildingProductionModifier;
}

int CvTraitInfo::getStateReligionFreeExperience() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionFreeExperience > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionFreeExperience < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionFreeExperience;
}

int CvTraitInfo::getExpInBorderModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iExpInBorderModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iExpInBorderModifier < 0)
		{
			return 0;
		}
	}
	return m_iExpInBorderModifier;
}

int CvTraitInfo::getCityDefenseBonus() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCityDefenseBonus > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iCityDefenseBonus < 0)
		{
			return 0;
		}
	}
	return m_iCityDefenseBonus;
}

int CvTraitInfo::getMilitaryProductionModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMilitaryProductionModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iMilitaryProductionModifier < 0)
		{
			return 0;
		}
	}
	return m_iMilitaryProductionModifier;
}

int CvTraitInfo::getAttitudeModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iAttitudeModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iAttitudeModifier < 0)
		{
			return 0;
		}
	}
	return m_iAttitudeModifier;
}

int CvTraitInfo::getLinePriority() const
{
	return m_iLinePriority;
}

int CvTraitInfo::getEspionageDefense() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iEspionageDefense > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iEspionageDefense < 0)
		{
			return 0;
		}
	}
	return m_iEspionageDefense;
}

int CvTraitInfo::getMinAnarchy() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMinAnarchy < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iMinAnarchy > 0)
		{
			return 0;
		}
	}
	return m_iMinAnarchy;
}

int CvTraitInfo::getMaxTradeRoutesChange() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMaxTradeRoutesChange > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iMaxTradeRoutesChange < 0)
		{
			return 0;
		}
	}
	return m_iMaxTradeRoutesChange;
}

int CvTraitInfo::getGoldenAgeDurationModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGoldenAgeDurationModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iGoldenAgeDurationModifier < 0)
		{
			return 0;
		}
	}
	return m_iGoldenAgeDurationModifier;
}

int CvTraitInfo::getGreatPeopleRateChange() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGreatPeopleRateChange > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iGreatPeopleRateChange < 0)
		{
			return 0;
		}
	}
	return m_iGreatPeopleRateChange;
}

int CvTraitInfo::getHurryAngerModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHurryAngerModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHurryAngerModifier > 0)
		{
			return 0;
		}
	}
	return m_iHurryAngerModifier;
}

int CvTraitInfo::getHurryCostModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHurryCostModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iHurryCostModifier > 0)
		{
			return 0;
		}
	}
	return m_iHurryCostModifier;
}

int CvTraitInfo::getEnemyWarWearinessModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iEnemyWarWearinessModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iEnemyWarWearinessModifier < 0)
		{
			return 0;
		}
	}
	return m_iEnemyWarWearinessModifier;
}

int CvTraitInfo::getForeignTradeRouteModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iForeignTradeRouteModifier > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iForeignTradeRouteModifier < 0)
		{
			return 0;
		}
	}
	return m_iForeignTradeRouteModifier;
}

int CvTraitInfo::getBombardDefense() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iBombardDefense > 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iBombardDefense < 0)
		{
			return 0;
		}
	}
	return m_iBombardDefense;
}

int CvTraitInfo::getUnitUpgradePriceModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iUnitUpgradePriceModifier < 0)
		{
			return 0;
		}
		else if (!isNegativeTrait() && m_iUnitUpgradePriceModifier > 0)
		{
			return 0;
		}
	}
	return m_iUnitUpgradePriceModifier;
}

int CvTraitInfo::getCoastalTradeRoutes() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCoastalTradeRoutes > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iCoastalTradeRoutes < 0)
		{
			return 0;
		}
	}
	return m_iCoastalTradeRoutes;
}

//Team Project (6)
int CvTraitInfo::getGlobalPopulationgrowthratepercentage() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGlobalPopulationgrowthratepercentage < 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iGlobalPopulationgrowthratepercentage > 0)
		{
			return 0;
		}
	}
	return m_iGlobalPopulationgrowthratepercentage;
}

int CvTraitInfo::getCityStartCulture(bool bForLoad) const
{
	if (!bForLoad && GC.getGameINLINE().isOption(GAMEOPTION_1_CITY_TILE_FOUNDING))
	{
		return 0;
	}
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCityStartCulture > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iCityStartCulture < 0)
		{
			return 0;
		}
	}
	return m_iCityStartCulture;
}

int CvTraitInfo::getGlobalAirUnitCapacity() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iGlobalAirUnitCapacity > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iGlobalAirUnitCapacity < 0)
		{
			return 0;
		}
	}
	return m_iGlobalAirUnitCapacity;
}

int CvTraitInfo::getCapitalXPModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iCapitalXPModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iCapitalXPModifier < 0)
		{
			return 0;
		}
	}
	return m_iCapitalXPModifier;
}

int CvTraitInfo::getHolyCityofStateReligionXPModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHolyCityofStateReligionXPModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iHolyCityofStateReligionXPModifier < 0)
		{
			return 0;
		}
	}
	return m_iHolyCityofStateReligionXPModifier;
}

int CvTraitInfo::getHolyCityofNonStateReligionXPModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iHolyCityofNonStateReligionXPModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iHolyCityofNonStateReligionXPModifier < 0)
		{
			return 0;
		}
	}
	return m_iHolyCityofNonStateReligionXPModifier;
}

int CvTraitInfo::getBonusPopulationinNewCities() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iBonusPopulationinNewCities > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iBonusPopulationinNewCities < 0)
		{
			return 0;
		}
	}
	return m_iBonusPopulationinNewCities;
}

int CvTraitInfo::getMissileRange() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMissileRange > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iMissileRange < 0)
		{
			return 0;
		}
	}
	return m_iMissileRange;
}

int CvTraitInfo::getFlightOperationRange() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFlightOperationRange > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iFlightOperationRange < 0)
		{
			return 0;
		}
	}
	return m_iFlightOperationRange;
}

int CvTraitInfo::getNavalCargoSpace() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNavalCargoSpace > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iNavalCargoSpace < 0)
		{
			return 0;
		}
	}
	return m_iNavalCargoSpace;
}

int CvTraitInfo::getMissileCargoSpace() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iMissileCargoSpace > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iMissileCargoSpace < 0)
		{
			return 0;
		}
	}
	return m_iMissileCargoSpace;
}

//Team Project (3)
int CvTraitInfo::getNationalCaptureProbabilityModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNationalCaptureProbabilityModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iNationalCaptureProbabilityModifier < 0)
		{
			return 0;
		}
	}
	return m_iNationalCaptureProbabilityModifier;
}

int CvTraitInfo::getNationalCaptureResistanceModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNationalCaptureResistanceModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iNationalCaptureResistanceModifier < 0)
		{
			return 0;
		}
	}
	return m_iNationalCaptureResistanceModifier;
}

//Team Project (6)
int CvTraitInfo::getStateReligionSpreadProbabilityModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iStateReligionSpreadProbabilityModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iStateReligionSpreadProbabilityModifier < 0)
		{
			return 0;
		}
	}
	return m_iStateReligionSpreadProbabilityModifier;
}

int CvTraitInfo::getNonStateReligionSpreadProbabilityModifier() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iNonStateReligionSpreadProbabilityModifier > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iNonStateReligionSpreadProbabilityModifier < 0)
		{
			return 0;
		}
	}
	return m_iNonStateReligionSpreadProbabilityModifier;
}

int CvTraitInfo::getFreedomFighterChange() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_iFreedomFighterChange > 0)
		{
				return 0;
		}
		else if (!isNegativeTrait() && m_iFreedomFighterChange < 0)
		{
			return 0;
		}
	}
	return m_iFreedomFighterChange;
}
//booleans

bool CvTraitInfo::isMilitaryFoodProduction() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bMilitaryFoodProduction)
		{
				return false;
		}
	}
	return m_bMilitaryFoodProduction;
}

bool CvTraitInfo::isNegativeTrait() const
{
	return m_bNegativeTrait;
}

bool CvTraitInfo::isImpurePropertyManipulators() const
{
	return m_bImpurePropertyManipulators;
}

bool CvTraitInfo::isImpurePromotions() const
{
	return m_bImpurePromotions;
}

bool CvTraitInfo::isCivilizationTrait() const
{
	return m_bCivilizationTrait;
}

bool CvTraitInfo::isAllowsInquisitions() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bAllowsInquisitions)
		{
				return false;
		}
	}
	return m_bAllowsInquisitions;
}

bool CvTraitInfo::isCoastalAIInfluence() const
{
	return m_bCoastalAIInfluence;
}

bool CvTraitInfo::isBarbarianSelectionOnly() const
{
	return m_bBarbarianSelectionOnly;
}

//Team Project (6)
bool CvTraitInfo::isCitiesStartwithStateReligion() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bCitiesStartwithStateReligion)
		{
				return false;
		}
	}

	return m_bCitiesStartwithStateReligion;
}

bool CvTraitInfo::isDraftsOnCityCapture() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bDraftsOnCityCapture)
		{
				return false;
		}
	}

	return m_bDraftsOnCityCapture;
}

bool CvTraitInfo::isFreeSpecialistperWorldWonder() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bFreeSpecialistperWorldWonder)
		{
				return false;
		}
	}

	return m_bFreeSpecialistperWorldWonder;
}

bool CvTraitInfo::isFreeSpecialistperNationalWonder() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bFreeSpecialistperNationalWonder)
		{
				return false;
		}
	}

	return m_bFreeSpecialistperNationalWonder;
}

bool CvTraitInfo::isFreeSpecialistperTeamProject() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bFreeSpecialistperTeamProject)
		{
				return false;
		}
	}

	return m_bFreeSpecialistperTeamProject;
}

bool CvTraitInfo::isExtraGoody() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bExtraGoody)
		{
				return false;
		}
	}

	return m_bExtraGoody;
}

//Team Project (5)
bool CvTraitInfo::isAllReligionsActive() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bAllReligionsActive)
		{
				return false;
		}
	}
	return m_bAllReligionsActive;
}

bool CvTraitInfo::isBansNonStateReligions() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (!isNegativeTrait() && m_bBansNonStateReligions)
		{
				return false;
		}
	}
	return m_bBansNonStateReligions;
}

bool CvTraitInfo::isFreedomFighter() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (isNegativeTrait() && m_bFreedomFighter)
		{
				return false;
		}
	}
	return m_bFreedomFighter;
}

// bool vector without delayed resolution
int CvTraitInfo::getNotOnGameOption(int i) const
{
	return m_aiNotOnGameOptions[i];
}

int CvTraitInfo::getNumNotOnGameOptions() const
{
	return (int)m_aiNotOnGameOptions.size();
}

bool CvTraitInfo::isNotOnGameOption(int i)
{
	FAssert (i > -1 && i < GC.getNumGameOptions());
	if (find(m_aiNotOnGameOptions.begin(), m_aiNotOnGameOptions.end(), i) == m_aiNotOnGameOptions.end())
	{
		return false;
	}
	return true;
}

int CvTraitInfo::getOnGameOption(int i) const
{
	return m_aiOnGameOptions[i];
}

int CvTraitInfo::getNumOnGameOptions() const
{
	return (int)m_aiOnGameOptions.size();
}

bool CvTraitInfo::isOnGameOption(int i)
{
	FAssert (i > -1 && i < GC.getNumGameOptions());
	if (find(m_aiOnGameOptions.begin(), m_aiOnGameOptions.end(), i) == m_aiOnGameOptions.end())
	{
		return false;
	}
	return true;
}
//Arrays
int CvTraitInfo::getSpecialistYieldChange(int i, int j) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i] && m_ppaiSpecialistYieldChangeFiltered && m_ppaiSpecialistYieldChangeFiltered[i])
		{
			if (isNegativeTrait() && m_ppaiSpecialistYieldChange[i][j] > 0)
			{
				m_ppaiSpecialistYieldChangeFiltered[i][j] = 0;
			}
			else if (!isNegativeTrait() && m_ppaiSpecialistYieldChange[i][j] < 0)
			{
				m_ppaiSpecialistYieldChangeFiltered[i][j] = 0;
			}
			else
			{
				m_ppaiSpecialistYieldChangeFiltered[i][j] = (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i]) ? m_ppaiSpecialistYieldChange[i][j] : 0;
			}
			return (m_ppaiSpecialistYieldChangeFiltered && m_ppaiSpecialistYieldChangeFiltered[i]) ? m_ppaiSpecialistYieldChangeFiltered[i][j] : 0;
		}
	}
	return (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i]) ? m_ppaiSpecialistYieldChange[i][j] : 0;
}

int* CvTraitInfo::getSpecialistYieldChangeArray(int i) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i] && m_ppaiSpecialistYieldChangeFiltered)
			{
				if (isNegativeTrait() && m_ppaiSpecialistYieldChange[i][j] > 0)
				{
					m_ppaiSpecialistYieldChangeFiltered[i][j] = 0;
				}
				else if (!isNegativeTrait() && m_ppaiSpecialistYieldChange[i][j] < 0)
				{
					m_ppaiSpecialistYieldChangeFiltered[i][j] = 0;
				}
				else
				{
					m_ppaiSpecialistYieldChangeFiltered[i][j] = (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i]) ? m_ppaiSpecialistYieldChange[i][j] : 0;
				}
			}
		}
		return (m_ppaiSpecialistYieldChangeFiltered) ? m_ppaiSpecialistYieldChangeFiltered[i] : 0;
	}
	return (m_ppaiSpecialistYieldChange) ? m_ppaiSpecialistYieldChange[i] : 0;
}

int CvTraitInfo::getYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piYieldModifier && m_piYieldModifierFiltered)
		{
			if (isNegativeTrait() && m_piYieldModifier[i] > 0)
			{
				m_piYieldModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piYieldModifier[i] < 0)
			{
				m_piYieldModifierFiltered[i] = 0;
			}
			else
			{
				m_piYieldModifierFiltered[i] = m_piYieldModifier ? m_piYieldModifier[i] : 0;
			}
			return m_piYieldModifierFiltered ? m_piYieldModifierFiltered[i] : 0;
		}
	}
	return m_piYieldModifier ? m_piYieldModifier[i] : 0;
}

int* CvTraitInfo::getYieldModifierArray() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piYieldModifier && m_piYieldModifier[i])
			{
				if (isNegativeTrait() && m_piYieldModifier[i] > 0)
				{
					m_piYieldModifierFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piYieldModifier[i] < 0)
				{
					m_piYieldModifierFiltered[i] = 0;
				}
				else
				{
					m_piYieldModifierFiltered[i] = m_piYieldModifier ? m_piYieldModifier[i] : 0;
				}
			}
		}
		return m_piYieldModifierFiltered;
	}
	return m_piYieldModifier;
}

int CvTraitInfo::getCapitalYieldModifier(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piCapitalYieldModifier && m_piCapitalYieldModifierFiltered)
		{
			if (isNegativeTrait() && m_piCapitalYieldModifier[i] > 0)
			{
				m_piCapitalYieldModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piCapitalYieldModifier[i] < 0)
			{
				m_piCapitalYieldModifierFiltered[i] = 0;
			}
			else
			{
				m_piCapitalYieldModifierFiltered[i] = m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
			}
			return m_piCapitalYieldModifierFiltered ? m_piCapitalYieldModifierFiltered[i] : 0;
		}
	}
	return m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
}

int* CvTraitInfo::getCapitalYieldModifierArray() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piCapitalYieldModifier && m_piCapitalYieldModifier[i])
			{
				if (isNegativeTrait() && m_piCapitalYieldModifier[i] > 0)
				{
					m_piCapitalYieldModifierFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piCapitalYieldModifier[i] < 0)
				{
					m_piCapitalYieldModifierFiltered[i] = 0;
				}
				else
				{
					m_piCapitalYieldModifierFiltered[i] = m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
				}
			}
		}
		return m_piCapitalYieldModifierFiltered;
	}
	return m_piCapitalYieldModifier;
}

int CvTraitInfo::getCapitalCommerceModifier(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piCapitalCommerceModifier && m_piCapitalCommerceModifierFiltered)
		{
			if (isNegativeTrait() && m_piCapitalCommerceModifier[i] > 0)
			{
				m_piCapitalCommerceModifierFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piCapitalCommerceModifier[i] < 0)
			{
				m_piCapitalCommerceModifierFiltered[i] = 0;
			}
			else
			{
				m_piCapitalCommerceModifierFiltered[i] = m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
			}
			return m_piCapitalCommerceModifierFiltered ? m_piCapitalCommerceModifierFiltered[i] : 0;
		}
	}
	return m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
}

int* CvTraitInfo::getCapitalCommerceModifierArray() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_COMMERCE_TYPES; i++)
		{
			if (m_piCapitalCommerceModifier && m_piCapitalCommerceModifier[i])
			{
				if (isNegativeTrait() && m_piCapitalCommerceModifier[i] > 0)
				{
					m_piCapitalCommerceModifierFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piCapitalCommerceModifier[i] < 0)
				{
					m_piCapitalCommerceModifierFiltered[i] = 0;
				}
				else
				{
					m_piCapitalCommerceModifierFiltered[i] = m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
				}
			}
		}
		return m_piCapitalCommerceModifierFiltered;
	}
	return m_piCapitalCommerceModifier;
}

int CvTraitInfo::getSpecialistExtraCommerce(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piSpecialistExtraCommerce && m_piSpecialistExtraCommerceFiltered)
		{
			if (isNegativeTrait() && m_piSpecialistExtraCommerce[i] > 0)
			{
				m_piSpecialistExtraCommerceFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piSpecialistExtraCommerce[i] < 0)
			{
				m_piSpecialistExtraCommerceFiltered[i] = 0;
			}
			else
			{
				m_piSpecialistExtraCommerceFiltered[i] = m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
			}
			return m_piSpecialistExtraCommerceFiltered ? m_piSpecialistExtraCommerceFiltered[i] : 0;
		}
	}
	return m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
}

int* CvTraitInfo::getSpecialistExtraCommerceArray() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_COMMERCE_TYPES; i++)
		{
			if (m_piSpecialistExtraCommerce && m_piSpecialistExtraCommerce[i])
			{
				if (isNegativeTrait() && m_piSpecialistExtraCommerce[i] > 0)
				{
					m_piSpecialistExtraCommerceFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piSpecialistExtraCommerce[i] < 0)
				{
					m_piSpecialistExtraCommerceFiltered[i] = 0;
				}
				else
				{
					m_piSpecialistExtraCommerceFiltered[i] = m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
				}
			}
		}
		return m_piSpecialistExtraCommerceFiltered;
	}
	return m_piSpecialistExtraCommerce;
}

int CvTraitInfo::getSpecialistExtraYield(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piSpecialistExtraYield && m_piSpecialistExtraYieldFiltered)
		{
			if (isNegativeTrait() && m_piSpecialistExtraYield[i] > 0)
			{
				m_piSpecialistExtraYieldFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piSpecialistExtraYield[i] < 0)
			{
				m_piSpecialistExtraYieldFiltered[i] = 0;
			}
			else
			{
				m_piSpecialistExtraYieldFiltered[i] = m_piSpecialistExtraYield ? m_piSpecialistExtraYield[i] : 0;
			}
			return m_piSpecialistExtraYieldFiltered ? m_piSpecialistExtraYieldFiltered[i] : 0;
		}
	}
	return m_piSpecialistExtraYield ? m_piSpecialistExtraYield[i] : 0;
}

int* CvTraitInfo::getSpecialistExtraYieldArray() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piSpecialistExtraYield && m_piSpecialistExtraYield[i])
			{
				if (isNegativeTrait() && m_piSpecialistExtraYield[i] > 0)
				{
					m_piSpecialistExtraYieldFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piSpecialistExtraYield[i] < 0)
				{
					m_piSpecialistExtraYieldFiltered[i] = 0;
				}
				else
				{
					m_piSpecialistExtraYieldFiltered[i] = m_piSpecialistExtraYield ? m_piSpecialistExtraYield[i] : 0;
				}
			}
		}
		return m_piSpecialistExtraYieldFiltered;
	}
	return m_piSpecialistExtraYield;
}

int CvTraitInfo::getFlavorValue(int i) const
{
	FAssertMsg(i < GC.getNumFlavorTypes(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvTraitInfo::getYieldChange(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piYieldChange && m_piYieldChangeFiltered)
		{
			if (isNegativeTrait() && m_piYieldChange[i] > 0)
			{
				m_piYieldChangeFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piYieldChange[i] < 0)
			{
				m_piYieldChangeFiltered[i] = 0;
			}
			else
			{
				m_piYieldChangeFiltered[i] = m_piYieldChange ? m_piYieldChange[i] : 0;
			}
			return m_piYieldChangeFiltered ? m_piYieldChangeFiltered[i] : 0;
		}
	}
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int CvTraitInfo::getSpecialistCommerceChange(int i, int j) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i] && m_ppaiSpecialistCommerceChangeFiltered && m_ppaiSpecialistCommerceChangeFiltered[i])
		{
			if (isNegativeTrait() && m_ppaiSpecialistCommerceChange[i][j] > 0)
			{
				m_ppaiSpecialistCommerceChangeFiltered[i][j] = 0;
			}
			else if (!isNegativeTrait() && m_ppaiSpecialistCommerceChange[i][j] < 0)
			{
				m_ppaiSpecialistCommerceChangeFiltered[i][j] = 0;
			}
			else
			{
				m_ppaiSpecialistCommerceChangeFiltered[i][j] = (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i]) ? m_ppaiSpecialistCommerceChange[i][j] : 0;
			}
			return (m_ppaiSpecialistCommerceChangeFiltered && m_ppaiSpecialistCommerceChangeFiltered[i]) ? m_ppaiSpecialistCommerceChangeFiltered[i][j] : 0;
		}
	}
	return (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i]) ? m_ppaiSpecialistCommerceChange[i][j] : 0;
}

int* CvTraitInfo::getSpecialistCommerceChangeArray(int i) const
{
	FAssertMsg(i < GC.getNumSpecialistInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i])
			{
				if (isNegativeTrait() && m_ppaiSpecialistCommerceChange[i][j] > 0)
				{
					m_ppaiSpecialistCommerceChangeFiltered[i][j] = 0;
				}
				else if (!isNegativeTrait() && m_ppaiSpecialistCommerceChange[i][j] < 0)
				{
					m_ppaiSpecialistCommerceChangeFiltered[i][j] = 0;
				}
				else
				{
					m_ppaiSpecialistCommerceChangeFiltered[i][j] = (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i]) ? m_ppaiSpecialistCommerceChange[i][j] : 0;
				}
			}
		}
		return m_ppaiSpecialistCommerceChangeFiltered[i];
	}
	return m_ppaiSpecialistCommerceChange[i];
}

int CvTraitInfo::getLessYieldThreshold(int i) const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_paiLessYieldThreshold && m_paiLessYieldThresholdFiltered)
		{
			// AIAndy: There is something wrong with this logic, both branches are identical
			// TB: Thanks for pointing that out there!
			if (!isNegativeTrait() && m_paiLessYieldThreshold[i] > 0)
			{
				m_paiLessYieldThresholdFiltered[i] = -1;
			}
			else
			{
				m_paiLessYieldThresholdFiltered[i] = m_paiLessYieldThreshold ? m_paiLessYieldThreshold[i] : 0;
			}
			return m_paiLessYieldThresholdFiltered ? m_paiLessYieldThresholdFiltered[i] : 0;
		}
	}
	return m_paiLessYieldThreshold ? m_paiLessYieldThreshold[i] : 0;
}

int CvTraitInfo::getSeaPlotYieldChanges(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piSeaPlotYieldChanges && m_piSeaPlotYieldChangesFiltered)
		{
			if (isNegativeTrait() && m_piSeaPlotYieldChanges[i] > 0)
			{
				m_piSeaPlotYieldChangesFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piSeaPlotYieldChanges[i] < 0)
			{
				m_piSeaPlotYieldChangesFiltered[i] = 0;
			}
			else
			{
				m_piSeaPlotYieldChangesFiltered[i] = m_piSeaPlotYieldChanges ? m_piSeaPlotYieldChanges[i] : 0;
			}
			return m_piSeaPlotYieldChangesFiltered ? m_piSeaPlotYieldChangesFiltered[i] : 0;
		}
	}
	return m_piSeaPlotYieldChanges ? m_piSeaPlotYieldChanges[i] : 0;
}

int* CvTraitInfo::getSeaPlotYieldChangesArray() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piSeaPlotYieldChanges && m_piSeaPlotYieldChanges[i])
			{
				if (isNegativeTrait() && m_piSeaPlotYieldChanges[i] > 0)
				{
					m_piSeaPlotYieldChangesFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piSeaPlotYieldChanges[i] < 0)
				{
					m_piSeaPlotYieldChangesFiltered[i] = 0;
				}
				else
				{
					m_piSeaPlotYieldChangesFiltered[i] = m_piSeaPlotYieldChanges ? m_piSeaPlotYieldChanges[i] : 0;
				}
			}
		}
		return m_piSeaPlotYieldChangesFiltered;
	}
	return m_piSeaPlotYieldChanges;
}

int CvTraitInfo::getImprovementYieldChange(int i, int j) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");
	FAssertMsg(j < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(j > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i] && m_ppaiImprovementYieldChangeFiltered && m_ppaiImprovementYieldChangeFiltered[i])
		{
			if (isNegativeTrait() && m_ppaiImprovementYieldChange[i][j] > 0)
			{
				m_ppaiImprovementYieldChangeFiltered[i][j] = 0;
			}
			else if (!isNegativeTrait() && m_ppaiImprovementYieldChange[i][j] < 0)
			{
				m_ppaiImprovementYieldChangeFiltered[i][j] = 0;
			}
			else
			{
				m_ppaiImprovementYieldChangeFiltered[i][j] = (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i]) ? m_ppaiImprovementYieldChange[i][j] : 0;
			}
			return (m_ppaiImprovementYieldChangeFiltered && m_ppaiImprovementYieldChangeFiltered[i]) ? m_ppaiImprovementYieldChangeFiltered[i][j] : 0;
		}
	}
	return (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i]) ? m_ppaiImprovementYieldChange[i][j] : 0;
}

int* CvTraitInfo::getImprovementYieldChangeArray(int i) const
{
	FAssertMsg(i < GC.getNumImprovementInfos(), "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i] && m_ppaiImprovementYieldChangeFiltered)
			{
				if (isNegativeTrait() && m_ppaiImprovementYieldChange[i][j] > 0)
				{
					m_ppaiImprovementYieldChangeFiltered[i][j] = 0;
				}
				else if (!isNegativeTrait() && m_ppaiImprovementYieldChange[i][j] < 0)
				{
					m_ppaiImprovementYieldChangeFiltered[i][j] = 0;
				}
				else
				{
					m_ppaiImprovementYieldChangeFiltered[i][j] = (m_ppaiImprovementYieldChange && m_ppaiImprovementYieldChange[i]) ? m_ppaiImprovementYieldChange[i][j] : 0;
				}
			}
		}
		return (m_ppaiImprovementYieldChangeFiltered) ? m_ppaiImprovementYieldChangeFiltered[i] : 0;
	}
	return (m_ppaiImprovementYieldChange) ? m_ppaiImprovementYieldChange[i] : 0;
}

//Team Project (7)
int CvTraitInfo::getGoldenAgeYieldChanges(int i) const
{
	FAssertMsg(i < NUM_YIELD_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piGoldenAgeYieldChanges && m_piGoldenAgeYieldChangesFiltered)
		{
			if (isNegativeTrait() && m_piGoldenAgeYieldChanges[i] > 0)
			{
				m_piGoldenAgeYieldChangesFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piGoldenAgeYieldChanges[i] < 0)
			{
				m_piGoldenAgeYieldChangesFiltered[i] = 0;
			}
			else
			{
				m_piGoldenAgeYieldChangesFiltered[i] = m_piGoldenAgeYieldChanges ? m_piGoldenAgeYieldChanges[i] : 0;
			}
			return m_piGoldenAgeYieldChangesFiltered ? m_piGoldenAgeYieldChangesFiltered[i] : 0;
		}
	}
	return m_piGoldenAgeYieldChanges ? m_piGoldenAgeYieldChanges[i] : 0;
}

int* CvTraitInfo::getGoldenAgeYieldChangesArray() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			if (m_piGoldenAgeYieldChanges && m_piGoldenAgeYieldChanges[i])
			{
				if (isNegativeTrait() && m_piGoldenAgeYieldChanges[i] > 0)
				{
					m_piGoldenAgeYieldChangesFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piGoldenAgeYieldChanges[i] < 0)
				{
					m_piGoldenAgeYieldChangesFiltered[i] = 0;
				}
				else
				{
					m_piGoldenAgeYieldChangesFiltered[i] = m_piGoldenAgeYieldChanges ? m_piGoldenAgeYieldChanges[i] : 0;
				}
			}
		}
		return m_piGoldenAgeYieldChangesFiltered;
	}
	return m_piGoldenAgeYieldChanges;
}

int CvTraitInfo::getGoldenAgeCommerceChanges(int i) const
{
	FAssertMsg(i < NUM_COMMERCE_TYPES, "Index out of bounds");
	FAssertMsg(i > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		if (m_piGoldenAgeCommerceChanges && m_piGoldenAgeCommerceChangesFiltered)
		{
			if (isNegativeTrait() && m_piGoldenAgeCommerceChanges[i] > 0)
			{
				m_piGoldenAgeCommerceChangesFiltered[i] = 0;
			}
			else if (!isNegativeTrait() && m_piGoldenAgeCommerceChanges[i] < 0)
			{
				m_piGoldenAgeCommerceChangesFiltered[i] = 0;
			}
			else
			{
				m_piGoldenAgeCommerceChangesFiltered[i] = m_piGoldenAgeCommerceChanges ? m_piGoldenAgeCommerceChanges[i] : 0;
			}
			return m_piGoldenAgeCommerceChangesFiltered ? m_piGoldenAgeCommerceChangesFiltered[i] : 0;
		}
	}
	return m_piGoldenAgeCommerceChanges ? m_piGoldenAgeCommerceChanges[i] : 0;
}

int* CvTraitInfo::getGoldenAgeCommerceChangesArray() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		for (int i = 0; i < NUM_COMMERCE_TYPES; i++)
		{
			if (m_piGoldenAgeCommerceChanges && m_piGoldenAgeCommerceChanges[i])
			{
				if (isNegativeTrait() && m_piGoldenAgeCommerceChanges[i] > 0)
				{
					m_piGoldenAgeCommerceChangesFiltered[i] = 0;
				}
				else if (!isNegativeTrait() && m_piGoldenAgeCommerceChanges[i] < 0)
				{
					m_piGoldenAgeCommerceChangesFiltered[i] = 0;
				}
				else
				{
					m_piGoldenAgeCommerceChangesFiltered[i] = m_piGoldenAgeCommerceChanges ? m_piGoldenAgeCommerceChanges[i] : 0;
				}
			}
		}
		return m_piGoldenAgeCommerceChangesFiltered;
	}
	return m_piGoldenAgeCommerceChanges;
}

// int vector utilizing struct with delayed resolution
int CvTraitInfo::getNumImprovementUpgradeModifierTypes() const
{
	return (int)m_aImprovementUpgradeModifierTypes.size();
}

ImprovementModifier CvTraitInfo::getImprovementUpgradeModifier(int iImprovement)
{
	FAssertMsg(iImprovement < (int)m_aImprovementUpgradeModifierTypes.size(), "Index out of bounds");
	FAssertMsg(iImprovement > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		ImprovementModifier kMod = m_aImprovementUpgradeModifierTypes[iImprovement];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}

	return m_aImprovementUpgradeModifierTypes[iImprovement];
}

int CvTraitInfo::getNumBuildWorkerSpeedModifierTypes() const
{
	return (int)m_aBuildWorkerSpeedModifierTypes.size();
}

BuildModifier CvTraitInfo::getBuildWorkerSpeedModifier(int iBuild)
{
	FAssertMsg(iBuild < (int)m_aBuildWorkerSpeedModifierTypes.size(), "Index out of bounds");
	FAssertMsg(iBuild > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		BuildModifier kMod = m_aBuildWorkerSpeedModifierTypes[iBuild];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aBuildWorkerSpeedModifierTypes[iBuild];
}

int CvTraitInfo::getNumDisallowedTraitTypes() const
{
	return (int)m_aDisallowedTraitTypes.size();
}

DisallowedTraitType CvTraitInfo::isDisallowedTraitType(int iTrait)
{
	FAssertMsg(iTrait < (int)m_aDisallowedTraitTypes.size(), "Index out of bounds");
	FAssertMsg(iTrait > -1, "Index out of bounds");

	return m_aDisallowedTraitTypes[iTrait];
}

int CvTraitInfo::getNumDomainFreeExperiences() const
{
	return (int)m_aDomainFreeExperiences.size();
}

DomainModifier CvTraitInfo::getDomainFreeExperience(int iDomain)
{
	FAssertMsg(iDomain < (int)m_aDomainFreeExperiences.size(), "Index out of bounds");
	FAssertMsg(iDomain > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		DomainModifier kMod = m_aDomainFreeExperiences[iDomain];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aDomainFreeExperiences[iDomain];
}

int CvTraitInfo::getNumDomainProductionModifiers() const
{
	return (int)m_aDomainProductionModifiers.size();
}

DomainModifier CvTraitInfo::getDomainProductionModifier(int iDomain)
{
	FAssertMsg(iDomain < (int)m_aDomainProductionModifiers.size(), "Index out of bounds");
	FAssertMsg(iDomain > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		DomainModifier kMod = m_aDomainProductionModifiers[iDomain];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aDomainProductionModifiers[iDomain];
}

int CvTraitInfo::getNumTechResearchModifiers() const
{
	return (int)m_aTechResearchModifiers.size();
}

TechModifier CvTraitInfo::getTechResearchModifier(int iTech)
{
	FAssertMsg(iTech < (int)m_aTechResearchModifiers.size(), "Index out of bounds");
	FAssertMsg(iTech > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		TechModifier kMod = m_aTechResearchModifiers[iTech];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aTechResearchModifiers[iTech];
}

int CvTraitInfo::getNumBuildingProductionModifiers() const
{
	return (int)m_aBuildingProductionModifiers.size();
}

BuildingModifier CvTraitInfo::getBuildingProductionModifier(int iBuilding)
{
	FAssertMsg(iBuilding < (int)m_aBuildingProductionModifiers.size(), "Index out of bounds");
	FAssertMsg(iBuilding > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		BuildingModifier kMod = m_aBuildingProductionModifiers[iBuilding];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aBuildingProductionModifiers[iBuilding];
}

int CvTraitInfo::getNumSpecialBuildingProductionModifiers() const
{
	return (int)m_aSpecialBuildingProductionModifiers.size();
}

SpecialBuildingModifier CvTraitInfo::getSpecialBuildingProductionModifier(int iSpecialBuilding)
{
	FAssertMsg(iSpecialBuilding < (int)m_aSpecialBuildingProductionModifiers.size(), "Index out of bounds");
	FAssertMsg(iSpecialBuilding > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		SpecialBuildingModifier kMod = m_aSpecialBuildingProductionModifiers[iSpecialBuilding];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aSpecialBuildingProductionModifiers[iSpecialBuilding];
}

int CvTraitInfo::getNumBuildingHappinessModifiers() const
{
	return (int)m_aBuildingHappinessModifiers.size();
}

BuildingModifier CvTraitInfo::getBuildingHappinessModifier(int iBuilding)
{
	FAssertMsg(iBuilding < (int)m_aBuildingHappinessModifiers.size(), "Index out of bounds");
	FAssertMsg(iBuilding > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		BuildingModifier kMod = m_aBuildingHappinessModifiers[iBuilding];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aBuildingHappinessModifiers[iBuilding];
}

int CvTraitInfo::getNumUnitProductionModifiers() const
{
	return (int)m_aUnitProductionModifiers.size();
}

UnitModifier CvTraitInfo::getUnitProductionModifier(int iUnit)
{
	FAssertMsg(iUnit < (int)m_aUnitProductionModifiers.size(), "Index out of bounds");
	FAssertMsg(iUnit > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		UnitModifier kMod = m_aUnitProductionModifiers[iUnit];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aUnitProductionModifiers[iUnit];
}

int CvTraitInfo::getNumSpecialUnitProductionModifiers() const
{
	return (int)m_aSpecialUnitProductionModifiers.size();
}

SpecialUnitModifier CvTraitInfo::getSpecialUnitProductionModifier(int iSpecialUnit)
{
	FAssertMsg(iSpecialUnit < (int)m_aSpecialUnitProductionModifiers.size(), "Index out of bounds");
	FAssertMsg(iSpecialUnit > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		SpecialUnitModifier kMod = m_aSpecialUnitProductionModifiers[iSpecialUnit];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aSpecialUnitProductionModifiers[iSpecialUnit];
}

int CvTraitInfo::getNumCivicOptionNoUpkeepTypes() const
{
	return (int)m_aCivicOptionNoUpkeepTypes.size();
}

CivicOptionTypeBool CvTraitInfo::isCivicOptionNoUpkeepType(int iCivicOption)
{
	FAssertMsg(iCivicOption < (int)m_aCivicOptionNoUpkeepTypes.size(), "Index out of bounds");
	FAssertMsg(iCivicOption > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		CivicOptionTypeBool kMod = m_aCivicOptionNoUpkeepTypes[iCivicOption];
		if (isNegativeTrait() && kMod.bBool)
		{
			kMod.bBool = 0;
		}
		else if (!isNegativeTrait() && !kMod.bBool)
		{
			kMod.bBool = 0;
		}
		return kMod;
	}
	return m_aCivicOptionNoUpkeepTypes[iCivicOption];
}

//Team Project (8)
int CvTraitInfo::getNumUnitCombatFreeExperiences() const
{
	return (int)m_aUnitCombatFreeExperiences.size();
}

UnitCombatModifier CvTraitInfo::getUnitCombatFreeExperience(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aUnitCombatFreeExperiences.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		UnitCombatModifier kMod = m_aUnitCombatFreeExperiences[iUnitCombat];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aUnitCombatFreeExperiences[iUnitCombat];
}

int CvTraitInfo::getNumUnitCombatProductionModifiers() const
{
	return (int)m_aUnitCombatProductionModifiers.size();
}

UnitCombatModifier CvTraitInfo::getUnitCombatProductionModifier(int iUnitCombat)
{
	FAssertMsg(iUnitCombat < (int)m_aUnitCombatProductionModifiers.size(), "Index out of bounds");
	FAssertMsg(iUnitCombat > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		UnitCombatModifier kMod = m_aUnitCombatProductionModifiers[iUnitCombat];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aUnitCombatProductionModifiers[iUnitCombat];
}

int CvTraitInfo::getNumBonusHappinessChanges() const
{
	return (int)m_aBonusHappinessChanges.size();
}

BonusModifier CvTraitInfo::getBonusHappinessChange(int iBonus)
{
	FAssertMsg((int)m_aBonusHappinessChanges[iBonus].eBonus < GC.getNumBonusInfos(), "Index out of bounds");
	FAssertMsg((int)m_aBonusHappinessChanges[iBonus].eBonus > -1, "Index out of bounds");

	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS))
	{
		BonusModifier kMod = m_aBonusHappinessChanges[iBonus];
		if (isNegativeTrait() && kMod.iModifier > 0)
		{
			kMod.iModifier = 0;
		}
		else if (!isNegativeTrait() && kMod.iModifier < 0)
		{
			kMod.iModifier = 0;
		}
		return kMod;
	}
	return m_aBonusHappinessChanges[iBonus];
}
//TB Traits Mods end

/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
bool CvTraitInfo::read(CvXMLLoadUtility* pXML)
{
	MEMORY_TRACE_FUNCTION();

	CvString szTextVal;
	CvString szTextVal2;
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ShortDescription");
	setShortDescription(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iMaxAnarchy, L"iMaxAnarchy",-1);
	pXML->GetOptionalChildXmlValByName(&m_iUpkeepModifier, L"iUpkeepModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLevelExperienceModifier, L"iLevelExperienceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateModifier, L"iGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatGeneralRateModifier, L"iGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, L"iDomesticGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxGlobalBuildingProductionModifier, L"iMaxGlobalBuildingProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxTeamBuildingProductionModifier, L"iMaxTeamBuildingProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxPlayerBuildingProductionModifier, L"iMaxPlayerBuildingProductionModifier");

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		RevTrait Effects														*/
/********************************************************************************/
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxLocal, L"iRevIdxLocal");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxNational, L"iRevIdxNational");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxDistanceModifier, L"iRevIdxDistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxHolyCityGood, L"iRevIdxHolyCityGood");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxHolyCityBad, L"iRevIdxHolyCityBad");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxNationalityMod, L"fRevIdxNationalityMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxBadReligionMod, L"fRevIdxBadReligionMod");
	pXML->GetOptionalChildXmlValByName(&m_fRevIdxGoodReligionMod, L"fRevIdxGoodReligionMod");
	pXML->GetOptionalChildXmlValByName(&m_bNonStateReligionCommerce, L"bNonStateReligionCommerce");
	pXML->GetOptionalChildXmlValByName(&m_bUpgradeAnywhere, L"bUpgradeAnywhere");
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	if (pXML->TryMoveToXmlFirstChild(L"ExtraYieldThresholds"))
	{
		pXML->SetYields(&m_paiExtraYieldThreshold);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiExtraYieldThreshold);
	}

	if (pXML->TryMoveToXmlFirstChild(L"TradeYieldModifiers"))
	{
		pXML->SetYields(&m_paiTradeYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiTradeYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
	{
		pXML->SetCommerce(&m_paiCommerceChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_paiCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceModifier);
	}



/************************************************************************************************/
/* Afforess					  Start		 08/26/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	pXML->SetVariableListTagPair(&m_pabFreePromotion, L"FreePromotions", sizeof(GC.getPromotionInfo((PromotionTypes)0)), GC.getNumPromotionInfos());

	pXML->SetVariableListTagPair(&m_pabFreePromotionUnitCombat, L"FreePromotionUnitCombats", GC.getNumUnitCombatInfos());
*/
	FAssertMsg((GC.getNumPromotionInfos() > 0) && (GC.getNumUnitCombatInfos()) > 0,"either the number of promotion infos is zero or less or the number of unit combat types is zero or less");
	if (pXML->TryMoveToXmlFirstChild(L"FreePromotionUnitCombatTypes"))
	{
		int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DBoolList(&m_ppbFreePromotionUnitCombats, GC.getNumPromotionInfos(), -1);
				for (int j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionType");
					int iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppbFreePromotionUnitCombats[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"UnitCombatTypes"))
						{
							// get the total number of children the current xml node has
							int iNumChildren = pXML->GetXmlChildrenNumber();

							if (0 < iNumChildren)
							{
								pXML->CvXMLLoadUtility::InitList(&m_ppbFreePromotionUnitCombats[iIndex], GC.getNumUnitCombatInfos());

								// if the call to the function that sets the current xml node to it's first non-comment
								// child and sets the parameter with the new node's value succeeds
								CvString szTag;
								if (pXML->GetChildXmlVal(szTag))
								{
									int iIndex2 = pXML->GetInfoClass(szTag);
									if (iIndex2 > -1)
									{
										m_ppbFreePromotionUnitCombats[iIndex][iIndex2] = true;
									}
									if(!(iNumChildren <= GC.getNumUnitCombatInfos()))
									{
										char	szMessage[1024];
										sprintf( szMessage, "For loop iterator is greater than array size \n Current XML file is: %s", GC.getCurrentXMLFile().GetCString());
										gDLL->MessageBox(szMessage, "XML Error");
									}
									// loop through all the siblings, we start at 1 since we already have the first value
									for (int i=1;i<iNumChildren;i++)
									{
										// if the call to the function that sets the current xml node to it's first non-comment
										// sibling and sets the parameter with the new node's value does not succeed
										// we will break out of this for loop

										if (!pXML->GetNextXmlVal(szTag))
										{
											break;
										}
										iIndex2 = pXML->GetInfoClass(szTag);
										if (iIndex2 > -1)
										{
											m_ppbFreePromotionUnitCombats[iIndex][iIndex2] = true;
										}

									}
									// set the current xml node to it's parent node
									pXML->MoveToXmlParent();
								}
							}
							else
							{
								SAFE_DELETE_ARRAY(m_ppbFreePromotionUnitCombats[iIndex]);
							}

							pXML->MoveToXmlParent();
						}
						else
						{
							SAFE_DELETE_ARRAY(m_ppbFreePromotionUnitCombats[iIndex]);
						}
					}

					if (!pXML->TryMoveToXmlNextSibling())
					{
						break;
					}
				}
			}

			pXML->MoveToXmlParent();
		}

		pXML->MoveToXmlParent();
	}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	m_PropertyManipulators.read(pXML);

	//TB Traits Mods begin
	//Textual References
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionLine");
	m_ePromotionLine = (PromotionLineTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GreatPeopleUnitClass");
	m_iGreatPeopleUnitClass = pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_ePrereqTech = (TechTypes) pXML->GetInfoClass(szTextVal);
//Team Project (6)
	pXML->GetOptionalChildXmlValByName(szTextVal, L"EraAdvanceFreeSpecialistType");
	m_eEraAdvanceFreeSpecialistType = (SpecialistTypes) pXML->GetInfoClass(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"GoldenAgeonBirthofGreatPersonType");
	m_iGoldenAgeonBirthofGreatPeopleType = pXML->GetInfoClass(szTextVal);
	//integers
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessAccumulationModifier, L"iWarWearinessAccumulationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCivicAnarchyTimeModifier, L"iCivicAnarchyTimeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iReligiousAnarchyTimeModifier, L"iReligiousAnarchyTimeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iImprovementUpgradeRateModifier, L"iImprovementUpgradeRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorkerSpeedModifier, L"iWorkerSpeedModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMaxConscript, L"iMaxConscript");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCorporationMaintenanceModifier, L"iCorporationMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionGreatPeopleRateModifier, L"iStateReligionGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeExperience, L"iFreeExperience");
	pXML->GetOptionalChildXmlValByName(&m_iBaseFreeUnits, L"iBaseFreeUnits");
	pXML->GetOptionalChildXmlValByName(&m_iBaseFreeMilitaryUnits, L"iBaseFreeMilitaryUnits");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitsPopulationPercent, L"iFreeUnitsPopulationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iFreeMilitaryUnitsPopulationPercent, L"iFreeMilitaryUnitsPopulationPercent");
	pXML->GetOptionalChildXmlValByName(&m_iGoldPerUnit, L"iGoldPerUnit");
	pXML->GetOptionalChildXmlValByName(&m_iGoldPerMilitaryUnit, L"iGoldPerMilitaryUnit");
	pXML->GetOptionalChildXmlValByName(&m_iHappyPerMilitaryUnit, L"iHappyPerMilitaryUnit");
	pXML->GetOptionalChildXmlValByName(&m_iLargestCityHappiness, L"iLargestCityHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iFreeSpecialist, L"iFreeSpecialist");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionHappiness, L"iStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iNonStateReligionHappiness, L"iNonStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionUnitProductionModifier, L"iStateReligionUnitProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionBuildingProductionModifier, L"iStateReligionBuildingProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionFreeExperience, L"iStateReligionFreeExperience");
	pXML->GetOptionalChildXmlValByName(&m_iExpInBorderModifier, L"iExpInBorderModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCityDefenseBonus, L"iCityDefenseBonus");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryProductionModifier, L"iMilitaryProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAttitudeModifier, L"iAttitudeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLinePriority, L"iLinePriority");
	pXML->GetOptionalChildXmlValByName(&m_iEspionageDefense, L"iEspionageDefense");
	pXML->GetOptionalChildXmlValByName(&m_iMinAnarchy, L"iMinAnarchy");
	pXML->GetOptionalChildXmlValByName(&m_iMaxTradeRoutesChange, L"iMaxTradeRoutesChange");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeDurationModifier, L"iGoldenAgeDurationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateChange, L"iGreatPeopleRateChange");
	pXML->GetOptionalChildXmlValByName(&m_iHurryAngerModifier, L"iHurryAngerModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHurryCostModifier, L"iHurryCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iEnemyWarWearinessModifier, L"iEnemyWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iForeignTradeRouteModifier, L"iForeignTradeRouteModifier");
	pXML->GetOptionalChildXmlValByName(&m_iBombardDefense, L"iBombardDefense");
	pXML->GetOptionalChildXmlValByName(&m_iUnitUpgradePriceModifier, L"iUnitUpgradePriceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCoastalTradeRoutes, L"iCoastalTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalPopulationgrowthratepercentage, L"iGlobalPopulationgrowthratepercentage");
	pXML->GetOptionalChildXmlValByName(&m_iCityStartCulture, L"iCityStartCulture");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalAirUnitCapacity, L"iGlobalAirUnitCapacity");
	pXML->GetOptionalChildXmlValByName(&m_iCapitalXPModifier, L"iCapitalXPModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHolyCityofStateReligionXPModifier, L"iHolyCityofStateReligionXPModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHolyCityofNonStateReligionXPModifier, L"iHolyCityofNonStateReligionXPModifier");
	pXML->GetOptionalChildXmlValByName(&m_iBonusPopulationinNewCities, L"iBonusPopulationinNewCities");
	pXML->GetOptionalChildXmlValByName(&m_iMissileRange, L"iMissileRange");
	pXML->GetOptionalChildXmlValByName(&m_iFlightOperationRange, L"iFlightOperationRange");
	pXML->GetOptionalChildXmlValByName(&m_iNavalCargoSpace, L"iNavalCargoSpace");
	pXML->GetOptionalChildXmlValByName(&m_iMissileCargoSpace, L"iMissileCargoSpace");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureProbabilityModifier, L"iNationalCaptureProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureResistanceModifier, L"iNationalCaptureResistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionSpreadProbabilityModifier, L"iStateReligionSpreadProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNonStateReligionSpreadProbabilityModifier, L"iNonStateReligionSpreadProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreedomFighterChange, L"iFreedomFighterChange");
	//booleans
	pXML->GetOptionalChildXmlValByName(&m_bMilitaryFoodProduction, L"bMilitaryFoodProduction");
	pXML->GetOptionalChildXmlValByName(&m_bNegativeTrait, L"bNegativeTrait");
	pXML->GetOptionalChildXmlValByName(&m_bImpurePropertyManipulators, L"bImpurePropertyManipulators");
	pXML->GetOptionalChildXmlValByName(&m_bImpurePromotions, L"bImpurePromotions");
	pXML->GetOptionalChildXmlValByName(&m_bCivilizationTrait, L"bCivilizationTrait");
	pXML->GetOptionalChildXmlValByName(&m_bAllowsInquisitions, L"bAllowsInquisitions");
	pXML->GetOptionalChildXmlValByName(&m_bCoastalAIInfluence, L"bCoastalAIInfluence");
	pXML->GetOptionalChildXmlValByName(&m_bBarbarianSelectionOnly, L"bBarbarianSelectionOnly");
	pXML->GetOptionalChildXmlValByName(&m_bCitiesStartwithStateReligion, L"bCitiesStartwithStateReligion");
	pXML->GetOptionalChildXmlValByName(&m_bDraftsOnCityCapture, L"bDraftsOnCityCapture");
	pXML->GetOptionalChildXmlValByName(&m_bFreeSpecialistperWorldWonder, L"bFreeSpecialistperWorldWonder");
	pXML->GetOptionalChildXmlValByName(&m_bFreeSpecialistperNationalWonder, L"bFreeSpecialistperNationalWonder");
	pXML->GetOptionalChildXmlValByName(&m_bFreeSpecialistperTeamProject, L"bFreeSpecialistperTeamProject");
	pXML->GetOptionalChildXmlValByName(&m_bExtraGoody, L"bExtraGoody");
	pXML->GetOptionalChildXmlValByName(&m_bAllReligionsActive, L"bAllReligionsActive");
	pXML->GetOptionalChildXmlValByName(&m_bBansNonStateReligions, L"bBansNonStateReligions");
	pXML->GetOptionalChildXmlValByName(&m_bFreedomFighter, L"bFreedomFighter");

	// bool vector without delayed resolution
	pXML->SetOptionalIntVector(&m_aiOnGameOptions, L"OnGameOptions");
	pXML->SetOptionalIntVector(&m_aiNotOnGameOptions, L"NotOnGameOptions");
	//arrays
	int j;
	int k;
	int iNumChildren;
	if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiSpecialistYieldChange == NULL )
					{
						m_ppaiSpecialistYieldChange = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiSpecialistYieldChange[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiSpecialistYieldChange[k]);
						pXML->MoveToXmlParent();
						m_bAnySpecialistYieldChanges = true;
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalYieldModifiers"))
	{
		pXML->SetYields(&m_piCapitalYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalYieldModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalCommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCapitalCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalCommerceModifier);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraCommerces"))
	{
		pXML->SetCommerce(&m_piSpecialistExtraCommerce);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraYields"))
	{
		pXML->SetYields(&m_piSpecialistExtraYield);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraYield);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"LessYieldThresholds"))
	{
		pXML->SetYields(&m_paiLessYieldThreshold);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiLessYieldThreshold);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiSpecialistCommerceChange == NULL )
					{
						m_ppaiSpecialistCommerceChange = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiSpecialistCommerceChange[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetCommerce(&m_ppaiSpecialistCommerceChange[k]);
						pXML->MoveToXmlParent();
						m_bAnySpecialistCommerceChanges = true;
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}
	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getFlavorTypes(), GC.getNumFlavorTypes());

	if (pXML->TryMoveToXmlFirstChild(L"SeaPlotYieldChanges"))
	{
		pXML->SetYields(&m_piSeaPlotYieldChanges);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSeaPlotYieldChanges);
	}

	if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiImprovementYieldChange == NULL )
					{
						m_ppaiImprovementYieldChange = new int*[GC.getNumImprovementInfos()];

						for(int i = 0; i < GC.getNumImprovementInfos(); i++)
						{
							m_ppaiImprovementYieldChange[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiImprovementYieldChange[k]);
						pXML->MoveToXmlParent();
						m_bAnyImprovementYieldChanges = true;
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

//Team Project (7)
	if (pXML->TryMoveToXmlFirstChild(L"GoldenAgeYieldChanges"))
	{
		pXML->SetYields(&m_piGoldenAgeYieldChanges);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGoldenAgeYieldChanges);
	}

	if (pXML->TryMoveToXmlFirstChild(L"GoldenAgeCommerceChanges"))
	{
		pXML->SetCommerce(&m_piGoldenAgeCommerceChanges);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGoldenAgeCommerceChanges);
	}

	// int vector utilizing struct with delayed resolution
	if(pXML->TryMoveToXmlFirstChild(L"ImprovementUpgradeModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"ImprovementUpgradeModifierType" );
		m_aImprovementUpgradeModifierTypes.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"ImprovementUpgradeModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					m_aImprovementUpgradeModifierTypes[i].eImprovement = (ImprovementTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aImprovementUpgradeModifierTypes[i].iModifier), L"iImprovementUpgradeModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"ImprovementUpgradeModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"BuildWorkerSpeedModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"BuildWorkerSpeedModifierType" );
		m_aBuildWorkerSpeedModifierTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"BuildWorkerSpeedModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildType");
					pXML->GetChildXmlValByName(&(m_aBuildWorkerSpeedModifierTypes[i].iModifier), L"iBuildWorkerSpeedModifier");
					GC.addDelayedResolution((int*)&(m_aBuildWorkerSpeedModifierTypes[i].eBuild), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"BuildWorkerSpeedModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"DisallowedTraitTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"DisallowedTraitType" );
		m_aDisallowedTraitTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"DisallowedTraitType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TraitType");
					GC.addDelayedResolution((int*)&(m_aDisallowedTraitTypes[i].eTrait), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"DisallowedTraitType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"DomainFreeExperiences"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"DomainFreeExperience" );
		m_aDomainFreeExperiences.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"DomainFreeExperience"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"DomainType");
					m_aDomainFreeExperiences[i].eDomain = (DomainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aDomainFreeExperiences[i].iModifier), L"iExperience");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"DomainFreeExperience"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"DomainProductionModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"DomainProductionModifier" );
		m_aDomainProductionModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"DomainProductionModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"DomainType");
					m_aDomainProductionModifiers[i].eDomain = (DomainTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aDomainProductionModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"DomainProductionModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"TechResearchModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"TechResearchModifier" );
		m_aTechResearchModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"TechResearchModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"TechType");
					m_aTechResearchModifiers[i].eTech = (TechTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aTechResearchModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"TechResearchModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"BuildingProductionModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"BuildingProductionModifierType" );
		m_aBuildingProductionModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"BuildingProductionModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					pXML->GetChildXmlValByName(&(m_aBuildingProductionModifiers[i].iModifier), L"iBuildingProductionModifier");
					GC.addDelayedResolution((int*)&(m_aBuildingProductionModifiers[i].eBuilding), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"BuildingProductionModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"SpecialBuildingProductionModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"SpecialBuildingProductionModifierType" );
		m_aSpecialBuildingProductionModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"SpecialBuildingProductionModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"SpecialBuildingType");
					pXML->GetChildXmlValByName(&(m_aSpecialBuildingProductionModifiers[i].iModifier), L"iSpecialBuildingProductionModifier");
					GC.addDelayedResolution((int*)&(m_aSpecialBuildingProductionModifiers[i].eSpecialBuilding), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"SpecialBuildingProductionModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"BuildingHappinessModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"BuildingHappinessModifierType" );
		m_aBuildingHappinessModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"BuildingHappinessModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					pXML->GetChildXmlValByName(&(m_aBuildingHappinessModifiers[i].iModifier), L"iBuildingHappinessModifier");
					GC.addDelayedResolution((int*)&(m_aBuildingHappinessModifiers[i].eBuilding), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"BuildingHappinessModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"UnitProductionModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitProductionModifierType" );
		m_aUnitProductionModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitProductionModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitType");
					pXML->GetChildXmlValByName(&(m_aUnitProductionModifiers[i].iModifier), L"iUnitProductionModifier");
					GC.addDelayedResolution((int*)&(m_aUnitProductionModifiers[i].eUnit), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitProductionModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"SpecialUnitProductionModifierTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"SpecialUnitProductionModifierType" );
		m_aSpecialUnitProductionModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"SpecialUnitProductionModifierType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"SpecialUnitType");
					m_aSpecialUnitProductionModifiers[i].eSpecialUnit = (SpecialUnitTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aSpecialUnitProductionModifiers[i].iModifier), L"iSpecialUnitProductionModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"SpecialUnitProductionModifierType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"CivicOptionNoUpkeepTypes"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"CivicOptionNoUpkeepType" );
		m_aCivicOptionNoUpkeepTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"CivicOptionNoUpkeepType"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"CivicOptionType");
					pXML->GetChildXmlValByName(&(m_aCivicOptionNoUpkeepTypes[i].bBool), L"bCivicOptionNoUpkeep");
					GC.addDelayedResolution((int*)&(m_aCivicOptionNoUpkeepTypes[i].eCivicOption), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"CivicOptionNoUpkeepType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

//Team Project (8)
	if(pXML->TryMoveToXmlFirstChild(L"UnitCombatFreeExperiences"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitCombatFreeExperience" );
		m_aUnitCombatFreeExperiences.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitCombatFreeExperience"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aUnitCombatFreeExperiences[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aUnitCombatFreeExperiences[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitCombatFreeExperience"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"UnitCombatProductionModifiers"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"UnitCombatProductionModifier" );
		m_aUnitCombatProductionModifiers.resize(iNum);
		if(pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"UnitCombatProductionModifier"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aUnitCombatProductionModifiers[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aUnitCombatProductionModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"UnitCombatProductionModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"BonusHappinessChanges"))
	{
		int i = 0;
		int iNum = pXML->GetXmlChildrenNumber(L"BonusHappinessChange");
		m_aBonusHappinessChanges.resize(iNum);
		if (pXML->TryMoveToXmlFirstChild())
		{

			if (pXML->TryMoveToXmlFirstOfSiblings(L"BonusHappinessChange"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BonusType");
					m_aBonusHappinessChanges[i].eBonus = (BonusTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aBonusHappinessChanges[i].iModifier), L"iHappinessChange");
					i++;
				} while (pXML->TryMoveToXmlNextSibling(L"BonusHappinessChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//For Pure Traits

	if (pXML->TryMoveToXmlFirstChild(L"ExtraYieldThresholds"))
	{
		pXML->SetYields(&m_paiExtraYieldThresholdFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiExtraYieldThresholdFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"TradeYieldModifiers"))
	{
		pXML->SetYields(&m_paiTradeYieldModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiTradeYieldModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
	{
		pXML->SetCommerce(&m_paiCommerceChangeFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceChangeFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
	{
		pXML->SetCommerce(&m_paiCommerceModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiCommerceModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiSpecialistYieldChangeFiltered == NULL )
					{
						m_ppaiSpecialistYieldChangeFiltered = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiSpecialistYieldChangeFiltered[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiSpecialistYieldChangeFiltered[k]);
						pXML->MoveToXmlParent();
						m_bAnySpecialistYieldChanges = true;
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		pXML->SetYields(&m_piYieldModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalYieldModifiers"))
	{
		pXML->SetYields(&m_piCapitalYieldModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalYieldModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CapitalCommerceModifiers"))
	{
		pXML->SetCommerce(&m_piCapitalCommerceModifierFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCapitalCommerceModifierFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraCommerces"))
	{
		pXML->SetCommerce(&m_piSpecialistExtraCommerceFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerceFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistExtraYields"))
	{
		pXML->SetYields(&m_piSpecialistExtraYieldFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSpecialistExtraYieldFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
	{
		pXML->SetYields(&m_piYieldChangeFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldChangeFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiSpecialistCommerceChangeFiltered == NULL )
					{
						m_ppaiSpecialistCommerceChangeFiltered = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiSpecialistCommerceChangeFiltered[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetCommerce(&m_ppaiSpecialistCommerceChangeFiltered[k]);
						pXML->MoveToXmlParent();
						m_bAnySpecialistCommerceChanges = true;
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"LessYieldThresholds"))
	{
		pXML->SetYields(&m_paiLessYieldThresholdFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_paiLessYieldThresholdFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"SeaPlotYieldChanges"))
	{
		pXML->SetYields(&m_piSeaPlotYieldChangesFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSeaPlotYieldChangesFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChanges"))
	{
		iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChange"))
		{
			for(j=0;j<iNumChildren;j++)
			{
				pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
				k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiImprovementYieldChangeFiltered == NULL )
					{
						m_ppaiImprovementYieldChangeFiltered = new int*[GC.getNumImprovementInfos()];

						for(int i = 0; i < GC.getNumImprovementInfos(); i++)
						{
							m_ppaiImprovementYieldChangeFiltered[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiImprovementYieldChangeFiltered[k]);
						pXML->MoveToXmlParent();
						m_bAnyImprovementYieldChanges = true;
					}
				}

				if (!pXML->TryMoveToXmlNextSibling())
				{
					break;
				}
			}

			// set the current xml node to it's parent node
			pXML->MoveToXmlParent();
		}

		// set the current xml node to it's parent node
		pXML->MoveToXmlParent();
	}

//Team Project (7)
	if (pXML->TryMoveToXmlFirstChild(L"GoldenAgeYieldChanges"))
	{
		pXML->SetYields(&m_piGoldenAgeYieldChangesFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGoldenAgeYieldChangesFiltered);
	}

	if (pXML->TryMoveToXmlFirstChild(L"GoldenAgeCommerceChanges"))
	{
		pXML->SetCommerce(&m_piGoldenAgeCommerceChangesFiltered);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGoldenAgeCommerceChangesFiltered);
	}

	//TB Traits Mods end

	return true;
}

bool CvTraitInfo::readPass2(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"TraitPrereq");
	m_iPrereqTrait = GC.getInfoTypeForString(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"TraitPrereqOr1");
	m_iPrereqOrTrait1 = GC.getInfoTypeForString(szTextVal);
	pXML->GetOptionalChildXmlValByName(szTextVal, L"TraitPrereqOr2");
	m_iPrereqOrTrait2 = GC.getInfoTypeForString(szTextVal);
	return true;
}

void CvTraitInfo::copyNonDefaults(CvTraitInfo* pClassInfo, CvXMLLoadUtility* pXML)
{
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	int iAudioDefault = -1;  //all audio is default -1
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	if (getShortDescription() == cDefault) setShortDescription(pClassInfo->getShortDescription());

	CvInfoBase::copyNonDefaults(pClassInfo, pXML);

	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getMaxAnarchy() == iDefault) m_iMaxAnarchy = pClassInfo->getMaxAnarchy();
	if (getUpkeepModifier() == iDefault) m_iUpkeepModifier = pClassInfo->getUpkeepModifier();
	if (getLevelExperienceModifier() == iDefault) m_iLevelExperienceModifier = pClassInfo->getLevelExperienceModifier();
	if (getGreatPeopleRateModifier() == iDefault) m_iGreatPeopleRateModifier = pClassInfo->getGreatPeopleRateModifier();
	if (getGreatGeneralRateModifier() == iDefault) m_iGreatGeneralRateModifier = pClassInfo->getGreatGeneralRateModifier();
	if (getDomesticGreatGeneralRateModifier() == iDefault) m_iDomesticGreatGeneralRateModifier = pClassInfo->getDomesticGreatGeneralRateModifier();
	if (getMaxGlobalBuildingProductionModifier() == iDefault) m_iMaxGlobalBuildingProductionModifier = pClassInfo->getMaxGlobalBuildingProductionModifier();
	if (getMaxTeamBuildingProductionModifier() == iDefault) m_iMaxTeamBuildingProductionModifier = pClassInfo->getMaxTeamBuildingProductionModifier();
	if (getMaxPlayerBuildingProductionModifier() == iDefault) m_iMaxPlayerBuildingProductionModifier = pClassInfo->getMaxPlayerBuildingProductionModifier();

	if (!m_paiExtraYieldThreshold) CvXMLLoadUtility::InitList(&m_paiExtraYieldThreshold, NUM_YIELD_TYPES);
	if (!m_paiTradeYieldModifier) CvXMLLoadUtility::InitList(&m_paiTradeYieldModifier, NUM_YIELD_TYPES);
	if (!m_paiExtraYieldThresholdFiltered) CvXMLLoadUtility::InitList(&m_paiExtraYieldThresholdFiltered, NUM_YIELD_TYPES);
	if (!m_paiTradeYieldModifierFiltered) CvXMLLoadUtility::InitList(&m_paiTradeYieldModifierFiltered, NUM_YIELD_TYPES);
	if (!m_paiLessYieldThreshold) CvXMLLoadUtility::InitList(&m_paiLessYieldThreshold, NUM_YIELD_TYPES);
	if (!m_paiLessYieldThresholdFiltered) CvXMLLoadUtility::InitList(&m_paiLessYieldThresholdFiltered, NUM_YIELD_TYPES);
	for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
	{
		if (m_paiExtraYieldThreshold[j] == iDefault)
		{
			m_paiExtraYieldThreshold[j] = pClassInfo->getExtraYieldThreshold(j);
		}
		if (m_paiTradeYieldModifier[j] == iDefault)
		{
			m_paiTradeYieldModifier[j] = pClassInfo->getTradeYieldModifier(j);
		}
		if (m_paiExtraYieldThresholdFiltered[j] == iDefault)
		{
			m_paiExtraYieldThresholdFiltered[j] = pClassInfo->getExtraYieldThreshold(j);
		}
		if (m_paiTradeYieldModifierFiltered[j] == iDefault)
		{
			m_paiTradeYieldModifierFiltered[j] = pClassInfo->getTradeYieldModifier(j);
		}
		if (m_paiLessYieldThreshold[j] == iDefault)
		{
			m_paiLessYieldThreshold[j] = pClassInfo->getLessYieldThreshold(j);
		}
		if (m_paiLessYieldThresholdFiltered[j] == iDefault)
		{
			m_paiLessYieldThresholdFiltered[j] = pClassInfo->getLessYieldThreshold(j);
		}
	}

	if (!m_paiCommerceChange) CvXMLLoadUtility::InitList(&m_paiCommerceChange, NUM_COMMERCE_TYPES);
	if (!m_paiCommerceModifier) CvXMLLoadUtility::InitList(&m_paiCommerceModifier, NUM_COMMERCE_TYPES);
	if (!m_paiCommerceChangeFiltered) CvXMLLoadUtility::InitList(&m_paiCommerceChangeFiltered, NUM_COMMERCE_TYPES);
	if (!m_paiCommerceModifierFiltered) CvXMLLoadUtility::InitList(&m_paiCommerceModifierFiltered, NUM_COMMERCE_TYPES);
	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
	{
		if (m_paiCommerceChange[j] == iDefault)
		{
			m_paiCommerceChange[j] = pClassInfo->getCommerceChange(j);
		}
		if (m_paiCommerceModifier[j] == iDefault)
		{
			m_paiCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
		if (m_paiCommerceChangeFiltered[j] == iDefault)
		{
			m_paiCommerceChangeFiltered[j] = pClassInfo->getCommerceChange(j);
		}
		if (m_paiCommerceModifierFiltered[j] == iDefault)
		{
			m_paiCommerceModifierFiltered[j] = pClassInfo->getCommerceModifier(j);
		}
	}

/************************************************************************************************/
/* Afforess					  Start		 08/26/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
/*
	for ( int j = 0; j < GC.getNumPromotionInfos(); j++ )
	{
		if (m_pabFreePromotion[j] == bDefault)
		{
			m_pabFreePromotion[j] = pClassInfo->isFreePromotion(j);
		}
	}
	for ( int j = 0; j < GC.getNumUnitCombatInfos(); j++ )
	{
		if (m_pabFreePromotionUnitCombat[j] == bDefault)
		{
			m_pabFreePromotionUnitCombat[j] = pClassInfo->isFreePromotionUnitCombat(j);
		}
	}
*/
	for ( int j = 0; j < GC.getNumPromotionInfos(); j++ )
	{
		for ( int i = 0; i < GC.getNumUnitCombatInfos(); i++ )
		{
			if (!isFreePromotionUnitCombats(j, i) && pClassInfo->isFreePromotionUnitCombats(j, i))
			{
				if ( NULL == m_ppbFreePromotionUnitCombats )
				{
					pXML->Init2DBoolList(&m_ppbFreePromotionUnitCombats, GC.getNumPromotionInfos(), GC.getNumUnitCombatInfos());
				}
				else if ( NULL == m_ppbFreePromotionUnitCombats[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppbFreePromotionUnitCombats[i],GC.getNumUnitCombatInfos());
				}
				m_ppbFreePromotionUnitCombats[j][i] = pClassInfo->isFreePromotionUnitCombats(j, i);
			}
		}
	}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	m_PropertyManipulators.copyNonDefaults(pClassInfo->getPropertyManipulators(), pXML);

	//TB Traits Mods begin
	//Textual References
	if (getPromotionLine() == NO_PROMOTIONLINE) m_ePromotionLine = pClassInfo->getPromotionLine();
	if (getGreatPeopleUnitClass() == NO_UNITCLASS) m_iGreatPeopleUnitClass = pClassInfo->getGreatPeopleUnitClass();
	if (getPrereqTech() == NO_TECH) m_ePrereqTech = pClassInfo->getPrereqTech();
//Team Project (6)
	if (getEraAdvanceFreeSpecialistType() == NO_SPECIALIST) m_eEraAdvanceFreeSpecialistType = pClassInfo->getEraAdvanceFreeSpecialistType();
	if (getGoldenAgeonBirthofGreatPeopleType() == NO_UNITCLASS) m_iGoldenAgeonBirthofGreatPeopleType = pClassInfo->getGoldenAgeonBirthofGreatPeopleType();
	//integers
	if (getWarWearinessAccumulationModifier() == iDefault) m_iWarWearinessAccumulationModifier = pClassInfo->getWarWearinessAccumulationModifier();
	if (getCivicAnarchyTimeModifier() == iDefault) m_iCivicAnarchyTimeModifier = pClassInfo->getCivicAnarchyTimeModifier();
	if (getReligiousAnarchyTimeModifier() == iDefault) m_iReligiousAnarchyTimeModifier = pClassInfo->getReligiousAnarchyTimeModifier();
	if (getImprovementUpgradeRateModifier() == iDefault) m_iImprovementUpgradeRateModifier = pClassInfo->getImprovementUpgradeRateModifier();
	if (getWorkerSpeedModifier() == iDefault) m_iWorkerSpeedModifier = pClassInfo->getWorkerSpeedModifier();
	if (getMaxConscript() == iDefault) m_iMaxConscript = pClassInfo->getMaxConscript();
	if (getDistanceMaintenanceModifier() == iDefault) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (getNumCitiesMaintenanceModifier() == iDefault) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (getCorporationMaintenanceModifier() == iDefault) m_iCorporationMaintenanceModifier = pClassInfo->getCorporationMaintenanceModifier();
	if (getStateReligionGreatPeopleRateModifier() == iDefault) m_iStateReligionGreatPeopleRateModifier = pClassInfo->getStateReligionGreatPeopleRateModifier();
	if (getFreeExperience() == iDefault) m_iFreeExperience = pClassInfo->getFreeExperience();
	if (getBaseFreeUnits() == iDefault) m_iBaseFreeUnits = pClassInfo->getBaseFreeUnits();
	if (getBaseFreeMilitaryUnits() == iDefault) m_iBaseFreeMilitaryUnits = pClassInfo->getBaseFreeMilitaryUnits();
	if (getFreeUnitsPopulationPercent() == iDefault) m_iFreeUnitsPopulationPercent = pClassInfo->getFreeUnitsPopulationPercent();
	if (getFreeMilitaryUnitsPopulationPercent() == iDefault) m_iFreeMilitaryUnitsPopulationPercent = pClassInfo->getFreeMilitaryUnitsPopulationPercent();
	if (getGoldPerUnit() == iDefault) m_iGoldPerUnit = pClassInfo->getGoldPerUnit();
	if (getGoldPerMilitaryUnit() == iDefault) m_iGoldPerMilitaryUnit = pClassInfo->getGoldPerMilitaryUnit();
	if (getHappyPerMilitaryUnit() == iDefault) m_iHappyPerMilitaryUnit = pClassInfo->getHappyPerMilitaryUnit();
	if (getLargestCityHappiness() == iDefault) m_iLargestCityHappiness = pClassInfo->getLargestCityHappiness();
	if (getFreeSpecialist() == iDefault) m_iFreeSpecialist = pClassInfo->getFreeSpecialist();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getStateReligionHappiness() == iDefault) m_iStateReligionHappiness = pClassInfo->getStateReligionHappiness();
	if (getNonStateReligionHappiness() == iDefault) m_iNonStateReligionHappiness = pClassInfo->getNonStateReligionHappiness();
	if (getStateReligionUnitProductionModifier() == iDefault) m_iStateReligionUnitProductionModifier = pClassInfo->getStateReligionUnitProductionModifier();
	if (getStateReligionBuildingProductionModifier() == iDefault) m_iStateReligionBuildingProductionModifier = pClassInfo->getStateReligionBuildingProductionModifier();
	if (getStateReligionFreeExperience() == iDefault) m_iStateReligionFreeExperience = pClassInfo->getStateReligionFreeExperience();
	if (getExpInBorderModifier() == iDefault) m_iExpInBorderModifier = pClassInfo->getExpInBorderModifier();
	if (getCityDefenseBonus() == iDefault) m_iCityDefenseBonus = pClassInfo->getCityDefenseBonus();
	if (getMilitaryProductionModifier() == iDefault) m_iMilitaryProductionModifier = pClassInfo->getMilitaryProductionModifier();
	if (getAttitudeModifier() == iDefault) m_iAttitudeModifier = pClassInfo->getAttitudeModifier();
	if (getLinePriority() == iDefault) m_iLinePriority = pClassInfo->getLinePriority();
	if (getEspionageDefense() == iDefault) m_iEspionageDefense = pClassInfo->getEspionageDefense();
	if (getMinAnarchy() == iDefault) m_iMinAnarchy = pClassInfo->getMinAnarchy();
	if (getMaxTradeRoutesChange() == iDefault) m_iMaxTradeRoutesChange = pClassInfo->getMaxTradeRoutesChange();
	if (getGoldenAgeDurationModifier() == iDefault) m_iGoldenAgeDurationModifier = pClassInfo->getGoldenAgeDurationModifier();
	if (getGreatPeopleRateChange() == iDefault) m_iGreatPeopleRateChange = pClassInfo->getGreatPeopleRateChange();
	if (getHurryAngerModifier() == iDefault) m_iHurryAngerModifier = pClassInfo->getHurryAngerModifier();
	if (getHurryCostModifier() == iDefault) m_iHurryCostModifier = pClassInfo->getHurryCostModifier();
	if (getEnemyWarWearinessModifier() == iDefault) m_iEnemyWarWearinessModifier = pClassInfo->getEnemyWarWearinessModifier();
	if (getForeignTradeRouteModifier() == iDefault) m_iForeignTradeRouteModifier = pClassInfo->getForeignTradeRouteModifier();
	if (getBombardDefense() == iDefault) m_iBombardDefense = pClassInfo->getBombardDefense();
	if (getUnitUpgradePriceModifier() == iDefault) m_iUnitUpgradePriceModifier = pClassInfo->getUnitUpgradePriceModifier();
	if (getCoastalTradeRoutes() == iDefault) m_iCoastalTradeRoutes = pClassInfo->getCoastalTradeRoutes();
//Team Project (6)
	if (getGlobalPopulationgrowthratepercentage() == iDefault) m_iGlobalPopulationgrowthratepercentage = pClassInfo->getGlobalPopulationgrowthratepercentage();
	if (getCityStartCulture(true) == iDefault) m_iCityStartCulture = pClassInfo->getCityStartCulture(true);
	if (getGlobalAirUnitCapacity() == iDefault) m_iGlobalAirUnitCapacity = pClassInfo->getGlobalAirUnitCapacity();
	if (getCapitalXPModifier() == iDefault) m_iCapitalXPModifier = pClassInfo->getCapitalXPModifier();
	if (getHolyCityofStateReligionXPModifier() == iDefault) m_iHolyCityofStateReligionXPModifier = pClassInfo->getHolyCityofStateReligionXPModifier();
	if (getHolyCityofNonStateReligionXPModifier() == iDefault) m_iHolyCityofNonStateReligionXPModifier = pClassInfo->getHolyCityofNonStateReligionXPModifier();
	if (getBonusPopulationinNewCities() == iDefault) m_iBonusPopulationinNewCities = pClassInfo->getBonusPopulationinNewCities();
	if (getMissileRange() == iDefault) m_iMissileRange = pClassInfo->getMissileRange();
	if (getFlightOperationRange() == iDefault) m_iFlightOperationRange = pClassInfo->getFlightOperationRange();
	if (getNavalCargoSpace() == iDefault) m_iNavalCargoSpace = pClassInfo->getNavalCargoSpace();
	if (getMissileCargoSpace() == iDefault) m_iMissileCargoSpace = pClassInfo->getMissileCargoSpace();
	if (getNationalCaptureProbabilityModifier() == iDefault) m_iNationalCaptureProbabilityModifier = pClassInfo->getNationalCaptureProbabilityModifier();
	if (getNationalCaptureResistanceModifier() == iDefault) m_iNationalCaptureResistanceModifier = pClassInfo->getNationalCaptureResistanceModifier();
	if (getStateReligionSpreadProbabilityModifier() == iDefault) m_iStateReligionSpreadProbabilityModifier = pClassInfo->getStateReligionSpreadProbabilityModifier();
	if (getNonStateReligionSpreadProbabilityModifier() == iDefault) m_iNonStateReligionSpreadProbabilityModifier = pClassInfo->getNonStateReligionSpreadProbabilityModifier();
	if (getFreedomFighterChange() == iDefault) m_iFreedomFighterChange = pClassInfo->getFreedomFighterChange();
	//booleans
	if (isMilitaryFoodProduction() == bDefault) m_bMilitaryFoodProduction = pClassInfo->isMilitaryFoodProduction();
	if (isNegativeTrait() == bDefault) m_bNegativeTrait = pClassInfo->isNegativeTrait();
	if (isImpurePropertyManipulators() == bDefault) m_bImpurePropertyManipulators = pClassInfo->isImpurePropertyManipulators();
	if (isImpurePromotions() == bDefault) m_bImpurePromotions = pClassInfo->isImpurePromotions();
	if (isCivilizationTrait() == bDefault) m_bCivilizationTrait = pClassInfo->isCivilizationTrait();
	if (isAllowsInquisitions() == bDefault) m_bAllowsInquisitions = pClassInfo->isAllowsInquisitions();
	if (isCoastalAIInfluence() == bDefault) m_bCoastalAIInfluence = pClassInfo->isCoastalAIInfluence();
	if (isBarbarianSelectionOnly() == bDefault) m_bBarbarianSelectionOnly = pClassInfo->isBarbarianSelectionOnly();
	if (isCitiesStartwithStateReligion() == bDefault) m_bCitiesStartwithStateReligion = pClassInfo->isCitiesStartwithStateReligion();
	if (isDraftsOnCityCapture() == bDefault) m_bDraftsOnCityCapture = pClassInfo->isDraftsOnCityCapture();
	if (isFreeSpecialistperWorldWonder() == bDefault) m_bFreeSpecialistperWorldWonder = pClassInfo->isFreeSpecialistperWorldWonder();
	if (isFreeSpecialistperNationalWonder() == bDefault) m_bFreeSpecialistperNationalWonder = pClassInfo->isFreeSpecialistperNationalWonder();
	if (isFreeSpecialistperTeamProject() == bDefault) m_bFreeSpecialistperTeamProject = pClassInfo->isFreeSpecialistperTeamProject();
	if (isExtraGoody() == bDefault) m_bExtraGoody = pClassInfo->isExtraGoody();
	if (isAllReligionsActive() == bDefault) m_bAllReligionsActive = pClassInfo->isAllReligionsActive();
	if (isBansNonStateReligions() == bDefault) m_bBansNonStateReligions = pClassInfo->isBansNonStateReligions();
	if (isFreedomFighter() == bDefault) m_bFreedomFighter = pClassInfo->isFreedomFighter();

	// bool vector without delayed resolution
	if (getNumNotOnGameOptions() == 0)
	{
		m_aiNotOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumNotOnGameOptions(); i++)
		{
			m_aiNotOnGameOptions.push_back(pClassInfo->getNotOnGameOption(i));
		}
	}
	if (getNumOnGameOptions() == 0)
	{
		m_aiOnGameOptions.clear();
		for ( int i = 0; i < pClassInfo->getNumOnGameOptions(); i++)
		{
			m_aiOnGameOptions.push_back(pClassInfo->getOnGameOption(i));
		}
	}
	//Arrays
	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getSpecialistYieldChange(i,j) == iDefault )
			{
				int iChange = pClassInfo->getSpecialistYieldChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiSpecialistYieldChange == NULL )
					{
						m_ppaiSpecialistYieldChange = new int*[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiSpecialistYieldChange[k] = NULL;
						}
					}
					if ( m_ppaiSpecialistYieldChange[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistYieldChange[i], NUM_YIELD_TYPES);
					}

					if ( m_ppaiSpecialistYieldChangeFiltered == NULL )
					{
						m_ppaiSpecialistYieldChangeFiltered = new int*[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiSpecialistYieldChangeFiltered[k] = NULL;
						}
					}
					if ( m_ppaiSpecialistYieldChangeFiltered[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistYieldChangeFiltered[i], NUM_YIELD_TYPES);
					}

					m_ppaiSpecialistYieldChange[i][j] = iChange;
					m_ppaiSpecialistYieldChangeFiltered[i][j] = iChange;
					m_bAnySpecialistYieldChanges = true;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumImprovementInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getImprovementYieldChange(i,j) == iDefault )
			{
				int iChange = pClassInfo->getImprovementYieldChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiImprovementYieldChange == NULL )
					{
						m_ppaiImprovementYieldChange = new int*[GC.getNumImprovementInfos()];
						for(int k = 0; k < GC.getNumImprovementInfos(); k++)
						{
							m_ppaiImprovementYieldChange[k] = NULL;
						}
					}
					if ( m_ppaiImprovementYieldChange[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiImprovementYieldChange[i], NUM_YIELD_TYPES);
					}

					if ( m_ppaiImprovementYieldChangeFiltered == NULL )
					{
						m_ppaiImprovementYieldChangeFiltered = new int*[GC.getNumImprovementInfos()];
						for(int k = 0; k < GC.getNumImprovementInfos(); k++)
						{
							m_ppaiImprovementYieldChangeFiltered[k] = NULL;
						}
					}
					if ( m_ppaiImprovementYieldChangeFiltered[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiImprovementYieldChangeFiltered[i], NUM_YIELD_TYPES);
					}

					m_ppaiImprovementYieldChange[i][j] = iChange;
					m_ppaiImprovementYieldChangeFiltered[i][j] = iChange;
					m_bAnyImprovementYieldChanges = true;
				}
			}
		}
	}

	if (!m_piYieldModifier) CvXMLLoadUtility::InitList(&m_piYieldModifier, NUM_YIELD_TYPES);
	if (!m_piYieldModifierFiltered) CvXMLLoadUtility::InitList(&m_piYieldModifierFiltered, NUM_YIELD_TYPES);
	if (!m_piCapitalYieldModifier) CvXMLLoadUtility::InitList(&m_piCapitalYieldModifier, NUM_YIELD_TYPES);
	if (!m_piCapitalYieldModifierFiltered) CvXMLLoadUtility::InitList(&m_piCapitalYieldModifierFiltered, NUM_YIELD_TYPES);
	if (!m_piSpecialistExtraYield) CvXMLLoadUtility::InitList(&m_piSpecialistExtraYield, NUM_YIELD_TYPES);
	if (!m_piSpecialistExtraYieldFiltered) CvXMLLoadUtility::InitList(&m_piSpecialistExtraYieldFiltered, NUM_YIELD_TYPES);
	if (!m_piYieldChange) CvXMLLoadUtility::InitList(&m_piYieldChange, NUM_YIELD_TYPES);
	if (!m_piYieldChangeFiltered) CvXMLLoadUtility::InitList(&m_piYieldChangeFiltered, NUM_YIELD_TYPES);
	if (!m_piSeaPlotYieldChanges) CvXMLLoadUtility::InitList(&m_piSeaPlotYieldChanges, NUM_YIELD_TYPES);
	if (!m_piSeaPlotYieldChangesFiltered) CvXMLLoadUtility::InitList(&m_piSeaPlotYieldChangesFiltered, NUM_YIELD_TYPES);
	if (!m_piGoldenAgeYieldChanges) CvXMLLoadUtility::InitList(&m_piGoldenAgeYieldChanges, NUM_YIELD_TYPES);
	if (!m_piGoldenAgeYieldChangesFiltered) CvXMLLoadUtility::InitList(&m_piGoldenAgeYieldChangesFiltered, NUM_YIELD_TYPES);
	for ( int i = 0; i < NUM_YIELD_TYPES; i++ )
	{
		if ( m_piYieldModifier[i] == iDefault )
		{
			m_piYieldModifier[i] = pClassInfo->getYieldModifier(i);
		}
		if ( m_piYieldModifierFiltered[i] == iDefault )
		{
			m_piYieldModifierFiltered[i] = pClassInfo->getYieldModifier(i);
		}

		if ( m_piCapitalYieldModifier[i] == iDefault )
		{
			m_piCapitalYieldModifier[i] = pClassInfo->getCapitalYieldModifier(i);
		}
		if ( m_piCapitalYieldModifierFiltered[i] == iDefault )
		{
			m_piCapitalYieldModifierFiltered[i] = pClassInfo->getCapitalYieldModifier(i);
		}

		if ( m_piSpecialistExtraYield[i] == iDefault )
		{
			m_piSpecialistExtraYield[i] = pClassInfo->getSpecialistExtraYield(i);
		}
		if ( m_piSpecialistExtraYieldFiltered[i] == iDefault )
		{
			m_piSpecialistExtraYieldFiltered[i] = pClassInfo->getSpecialistExtraYield(i);
		}

		if ( m_piYieldChange[i] == iDefault )
		{
			m_piYieldChange[i] = pClassInfo->getYieldChange(i);
		}
		if ( m_piYieldChangeFiltered[i] == iDefault )
		{
			m_piYieldChangeFiltered[i] = pClassInfo->getYieldChange(i);
		}

		if ( m_piSeaPlotYieldChanges[i] == iDefault )
		{
			m_piSeaPlotYieldChanges[i] = pClassInfo->getSeaPlotYieldChanges(i);
		}
		if ( m_piSeaPlotYieldChangesFiltered[i] == iDefault )
		{
			m_piSeaPlotYieldChangesFiltered[i] = pClassInfo->getSeaPlotYieldChanges(i);
		}

//Team Project (7)
		if ( m_piGoldenAgeYieldChanges[i] == iDefault )
		{
			m_piGoldenAgeYieldChanges[i] = pClassInfo->getGoldenAgeYieldChanges(i);
		}
		if ( m_piGoldenAgeYieldChangesFiltered[i] == iDefault )
		{
			m_piGoldenAgeYieldChangesFiltered[i] = pClassInfo->getGoldenAgeYieldChanges(i);
		}
	}

	if (!m_piCapitalCommerceModifier) CvXMLLoadUtility::InitList(&m_piCapitalCommerceModifier, NUM_COMMERCE_TYPES);
	if (!m_piCapitalCommerceModifierFiltered) CvXMLLoadUtility::InitList(&m_piCapitalCommerceModifierFiltered, NUM_COMMERCE_TYPES);
	if (!m_piSpecialistExtraCommerce) CvXMLLoadUtility::InitList(&m_piSpecialistExtraCommerce, NUM_COMMERCE_TYPES);
	if (!m_piSpecialistExtraCommerceFiltered) CvXMLLoadUtility::InitList(&m_piSpecialistExtraCommerceFiltered, NUM_COMMERCE_TYPES);
	if (!m_piGoldenAgeCommerceChanges) CvXMLLoadUtility::InitList(&m_piGoldenAgeCommerceChanges, NUM_COMMERCE_TYPES);
	if (!m_piGoldenAgeCommerceChangesFiltered) CvXMLLoadUtility::InitList(&m_piGoldenAgeCommerceChangesFiltered, NUM_COMMERCE_TYPES);
	for ( int i = 0; i < NUM_COMMERCE_TYPES; i++ )
	{
		if ( m_piCapitalCommerceModifier[i] == iDefault )
		{
			m_piCapitalCommerceModifier[i] = pClassInfo->getCapitalCommerceModifier(i);
		}
		if ( m_piCapitalCommerceModifierFiltered[i] == iDefault )
		{
			m_piCapitalCommerceModifierFiltered[i] = pClassInfo->getCapitalCommerceModifier(i);
		}

		if ( m_piSpecialistExtraCommerce[i] == iDefault )
		{
			m_piSpecialistExtraCommerce[i] = pClassInfo->getSpecialistExtraCommerce(i);
		}
		if ( m_piSpecialistExtraCommerceFiltered[i] == iDefault )
		{
			m_piSpecialistExtraCommerceFiltered[i] = pClassInfo->getSpecialistExtraCommerce(i);
		}

//Team Project (7)
		if ( m_piGoldenAgeCommerceChanges[i] == iDefault )
		{
			m_piGoldenAgeCommerceChanges[i] = pClassInfo->getGoldenAgeCommerceChanges(i);
		}
		if ( m_piGoldenAgeCommerceChangesFiltered[i] == iDefault )
		{
			m_piGoldenAgeCommerceChangesFiltered[i] = pClassInfo->getGoldenAgeCommerceChanges(i);
		}
	}

	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if ( getSpecialistCommerceChange(i,j) == iDefault )
			{
				int iChange = pClassInfo->getSpecialistCommerceChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiSpecialistCommerceChange == NULL )
					{
						m_ppaiSpecialistCommerceChange = new int*[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiSpecialistCommerceChange[k] = NULL;
						}
					}
					if ( m_ppaiSpecialistCommerceChange[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistCommerceChange[i], NUM_COMMERCE_TYPES);
					}

					if ( m_ppaiSpecialistCommerceChangeFiltered == NULL )
					{
						m_ppaiSpecialistCommerceChangeFiltered = new int*[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiSpecialistCommerceChangeFiltered[k] = NULL;
						}
					}
					if ( m_ppaiSpecialistCommerceChangeFiltered[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistCommerceChangeFiltered[i], NUM_COMMERCE_TYPES);
					}

					m_ppaiSpecialistCommerceChange[i][j] = iChange;
					m_ppaiSpecialistCommerceChangeFiltered[i][j] = iChange;
					m_bAnySpecialistCommerceChanges = true;
				}
			}
		}
	}

	// int vector utilizing struct with delayed resolution
	if (getNumImprovementUpgradeModifierTypes() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aImprovementUpgradeModifierTypes, pClassInfo->m_aImprovementUpgradeModifierTypes);
	}

	if (getNumBuildWorkerSpeedModifierTypes() == 0)
	{
		int iNum = pClassInfo->getNumBuildWorkerSpeedModifierTypes();
		m_aBuildWorkerSpeedModifierTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aBuildWorkerSpeedModifierTypes[i].iModifier = pClassInfo->m_aBuildWorkerSpeedModifierTypes[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aBuildWorkerSpeedModifierTypes[i].eBuild), (int*)&(pClassInfo->m_aBuildWorkerSpeedModifierTypes[i].eBuild));
		}
	}

	if (getNumDisallowedTraitTypes() == 0)
	{
		int iNum = pClassInfo->getNumDisallowedTraitTypes();
		m_aDisallowedTraitTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aDisallowedTraitTypes[i].eTrait), (int*)&(pClassInfo->m_aDisallowedTraitTypes[i].eTrait));
		}
	}

	if (getNumCivicOptionNoUpkeepTypes() == 0)
	{
		int iNum = pClassInfo->getNumCivicOptionNoUpkeepTypes();
		m_aCivicOptionNoUpkeepTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aCivicOptionNoUpkeepTypes[i].bBool = pClassInfo->m_aCivicOptionNoUpkeepTypes[i].bBool;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aCivicOptionNoUpkeepTypes[i].eCivicOption), (int*)&(pClassInfo->m_aCivicOptionNoUpkeepTypes[i].eCivicOption));
		}
	}

	for ( int i = 0; i < GC.getNumFlavorTypes(); i++ )
	{
		if ( getFlavorValue(i) == iDefault && pClassInfo->getFlavorValue(i) != iDefault)
		{
			if ( NULL == m_piFlavorValue )
			{
				CvXMLLoadUtility::InitList(&m_piFlavorValue,GC.getNumFlavorTypes(),iDefault);
			}
			m_piFlavorValue[i] = pClassInfo->getFlavorValue(i);
		}
	}

	if (getNumDomainFreeExperiences() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aDomainFreeExperiences, pClassInfo->m_aDomainFreeExperiences);
	}

	if (getNumDomainProductionModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aDomainProductionModifiers, pClassInfo->m_aDomainProductionModifiers);
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aTechResearchModifiers, pClassInfo->m_aTechResearchModifiers);

	if (getNumBuildingProductionModifiers() == 0)
	{
		int iNum = pClassInfo->getNumBuildingProductionModifiers();
		m_aBuildingProductionModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aBuildingProductionModifiers[i].iModifier = pClassInfo->m_aBuildingProductionModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aBuildingProductionModifiers[i].eBuilding), (int*)&(pClassInfo->m_aBuildingProductionModifiers[i].eBuilding));
		}
	}

	if (getNumSpecialBuildingProductionModifiers() == 0)
	{
		int iNum = pClassInfo->getNumSpecialBuildingProductionModifiers();
		m_aSpecialBuildingProductionModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aSpecialBuildingProductionModifiers[i].iModifier = pClassInfo->m_aSpecialBuildingProductionModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aSpecialBuildingProductionModifiers[i].eSpecialBuilding), (int*)&(pClassInfo->m_aSpecialBuildingProductionModifiers[i].eSpecialBuilding));
		}
	}

	if (getNumBuildingHappinessModifiers() == 0)
	{
		int iNum = pClassInfo->getNumBuildingHappinessModifiers();
		m_aBuildingHappinessModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aBuildingHappinessModifiers[i].iModifier = pClassInfo->m_aBuildingHappinessModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aBuildingHappinessModifiers[i].eBuilding), (int*)&(pClassInfo->m_aBuildingHappinessModifiers[i].eBuilding));
		}
	}

	if (getNumUnitProductionModifiers() == 0)
	{
		int iNum = pClassInfo->getNumUnitProductionModifiers();
		m_aUnitProductionModifiers.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aUnitProductionModifiers[i].iModifier = pClassInfo->m_aUnitProductionModifiers[i].iModifier;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aUnitProductionModifiers[i].eUnit), (int*)&(pClassInfo->m_aUnitProductionModifiers[i].eUnit));
		}
	}

	if (getNumSpecialUnitProductionModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aSpecialUnitProductionModifiers, pClassInfo->m_aSpecialUnitProductionModifiers);
	}

//Team Project (8)
	if (getNumUnitCombatFreeExperiences() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aUnitCombatFreeExperiences, pClassInfo->m_aUnitCombatFreeExperiences);
	}

	if (getNumUnitCombatProductionModifiers() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aUnitCombatProductionModifiers, pClassInfo->m_aUnitCombatProductionModifiers);
	}

	if (getNumBonusHappinessChanges() == 0)
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aBonusHappinessChanges, pClassInfo->m_aBonusHappinessChanges);
	}

	// Readpass2 stuff
	if (getPrereqTrait() == iTextDefault) m_iPrereqTrait = pClassInfo->getPrereqTrait();
	if (getPrereqOrTrait1() == iTextDefault) m_iPrereqOrTrait1 = pClassInfo->getPrereqOrTrait1();
	if (getPrereqOrTrait2() == iTextDefault) m_iPrereqOrTrait2 = pClassInfo->getPrereqOrTrait2();

	//TB Traits Mods end
}

void CvTraitInfo::copyNonDefaultsReadPass2(CvTraitInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default

	if (bOver || getPrereqTrait() == iTextDefault) m_iPrereqTrait = pClassInfo->getPrereqTrait();
	if (bOver || getPrereqOrTrait1() == iTextDefault) m_iPrereqOrTrait1 = pClassInfo->getPrereqOrTrait1();
	if (bOver || getPrereqOrTrait2() == iTextDefault) m_iPrereqOrTrait2 = pClassInfo->getPrereqOrTrait2();
}

void CvTraitInfo::getCheckSum(unsigned int& iSum)
{
	for ( int j = 0; j < GC.getNumPromotionInfos(); j++ )
	{
		for ( int i = 0; i < GC.getNumUnitCombatInfos(); i++ )
		{
			CheckSum(iSum, isFreePromotionUnitCombats(j,i));
		}
	}

	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iMaxAnarchy);
	CheckSum(iSum, m_iUpkeepModifier);
	CheckSum(iSum, m_iLevelExperienceModifier);
	CheckSum(iSum, m_iGreatPeopleRateModifier);
	CheckSum(iSum, m_iGreatGeneralRateModifier);
	CheckSum(iSum, m_iDomesticGreatGeneralRateModifier);
	CheckSum(iSum, m_iMaxGlobalBuildingProductionModifier);
	CheckSum(iSum, m_iMaxTeamBuildingProductionModifier);
	CheckSum(iSum, m_iMaxPlayerBuildingProductionModifier);

	CheckSum(iSum, m_iRevIdxLocal);
	CheckSum(iSum, m_iRevIdxNational);
	CheckSum(iSum, m_iRevIdxDistanceModifier);
	CheckSum(iSum, m_iRevIdxHolyCityGood);
	CheckSum(iSum, m_iRevIdxHolyCityBad);
	CheckSum(iSum, m_fRevIdxNationalityMod);
	CheckSum(iSum, m_fRevIdxGoodReligionMod);
	CheckSum(iSum, m_fRevIdxBadReligionMod);
	CheckSum(iSum, m_bNonStateReligionCommerce);
	CheckSum(iSum, m_bUpgradeAnywhere);

	// Arrays

	CheckSum(iSum, m_paiExtraYieldThreshold, NUM_YIELD_TYPES);
	CheckSum(iSum, m_paiTradeYieldModifier, NUM_YIELD_TYPES);
	CheckSum(iSum, m_paiCommerceChange, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiCommerceModifier, NUM_COMMERCE_TYPES);

	m_PropertyManipulators.getCheckSum(iSum);

	//TB Traits Mods begin
	//Textual References
	CheckSum(iSum, m_iPrereqTrait);
	CheckSum(iSum, m_iPrereqOrTrait1);
	CheckSum(iSum, m_iPrereqOrTrait2);
	CheckSum(iSum, m_ePromotionLine);
	CheckSum(iSum, m_iGreatPeopleUnitClass);
	CheckSum(iSum, m_ePrereqTech);
	CheckSum(iSum, m_eEraAdvanceFreeSpecialistType);
	CheckSum(iSum, m_iGoldenAgeonBirthofGreatPeopleType);
	//integers
	CheckSum(iSum, m_iWarWearinessAccumulationModifier);
	CheckSum(iSum, m_iCivicAnarchyTimeModifier);
	CheckSum(iSum, m_iReligiousAnarchyTimeModifier);
	CheckSum(iSum, m_iImprovementUpgradeRateModifier);
	CheckSum(iSum, m_iWorkerSpeedModifier);
	CheckSum(iSum, m_iMaxConscript);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iCorporationMaintenanceModifier);
	CheckSum(iSum, m_iStateReligionGreatPeopleRateModifier);
	CheckSum(iSum, m_iFreeExperience);
	CheckSum(iSum, m_iBaseFreeUnits);
	CheckSum(iSum, m_iBaseFreeMilitaryUnits);
	CheckSum(iSum, m_iFreeUnitsPopulationPercent);
	CheckSum(iSum, m_iFreeMilitaryUnitsPopulationPercent);
	CheckSum(iSum, m_iGoldPerUnit);
	CheckSum(iSum, m_iGoldPerMilitaryUnit);
	CheckSum(iSum, m_iHappyPerMilitaryUnit);
	CheckSum(iSum, m_iLargestCityHappiness);
	CheckSum(iSum, m_iFreeSpecialist);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iStateReligionHappiness);
	CheckSum(iSum, m_iNonStateReligionHappiness);
	CheckSum(iSum, m_iStateReligionUnitProductionModifier);
	CheckSum(iSum, m_iStateReligionBuildingProductionModifier);
	CheckSum(iSum, m_iStateReligionFreeExperience);
	CheckSum(iSum, m_iExpInBorderModifier);
	CheckSum(iSum, m_iCityDefenseBonus);
	CheckSum(iSum, m_iMilitaryProductionModifier);
	CheckSum(iSum, m_iAttitudeModifier);
	CheckSum(iSum, m_iLinePriority);
	CheckSum(iSum, m_iEspionageDefense);
	CheckSum(iSum, m_iMinAnarchy);
	CheckSum(iSum, m_iMaxTradeRoutesChange);
	CheckSum(iSum, m_iGoldenAgeDurationModifier);
	CheckSum(iSum, m_iGreatPeopleRateChange);
	CheckSum(iSum, m_iHurryAngerModifier);
	CheckSum(iSum, m_iHurryCostModifier);
	CheckSum(iSum, m_iEnemyWarWearinessModifier);
	CheckSum(iSum, m_iForeignTradeRouteModifier);
	CheckSum(iSum, m_iBombardDefense);
	CheckSum(iSum, m_iUnitUpgradePriceModifier);
	CheckSum(iSum, m_iCoastalTradeRoutes);
	CheckSum(iSum, m_iGlobalPopulationgrowthratepercentage);
	CheckSum(iSum, m_iCityStartCulture);
	CheckSum(iSum, m_iGlobalAirUnitCapacity);
	CheckSum(iSum, m_iCapitalXPModifier);
	CheckSum(iSum, m_iHolyCityofStateReligionXPModifier);
	CheckSum(iSum, m_iHolyCityofNonStateReligionXPModifier);
	CheckSum(iSum, m_iBonusPopulationinNewCities);
	CheckSum(iSum, m_iMissileRange);
	CheckSum(iSum, m_iFlightOperationRange);
	CheckSum(iSum, m_iNavalCargoSpace);
	CheckSum(iSum, m_iMissileCargoSpace);
	CheckSum(iSum, m_iNationalCaptureProbabilityModifier);
	CheckSum(iSum, m_iNationalCaptureResistanceModifier);
	CheckSum(iSum, m_iStateReligionSpreadProbabilityModifier);
	CheckSum(iSum, m_iNonStateReligionSpreadProbabilityModifier);
	CheckSum(iSum, m_iFreedomFighterChange);
	//booleans
	CheckSum(iSum, m_bMilitaryFoodProduction);
	CheckSum(iSum, m_bNegativeTrait);
	CheckSum(iSum, m_bImpurePropertyManipulators);
	CheckSum(iSum, m_bImpurePromotions);
	CheckSum(iSum, m_bCivilizationTrait);
	CheckSum(iSum, m_bAllowsInquisitions);
	CheckSum(iSum, m_bCoastalAIInfluence);
	CheckSum(iSum, m_bBarbarianSelectionOnly);
	CheckSum(iSum, m_bCitiesStartwithStateReligion);
	CheckSum(iSum, m_bDraftsOnCityCapture);
	CheckSum(iSum, m_bFreeSpecialistperWorldWonder);
	CheckSum(iSum, m_bFreeSpecialistperNationalWonder);
	CheckSum(iSum, m_bFreeSpecialistperTeamProject);
	CheckSum(iSum, m_bExtraGoody);
	CheckSum(iSum, m_bAllReligionsActive);
	CheckSum(iSum, m_bBansNonStateReligions);
	CheckSum(iSum, m_bFreedomFighter);
	// bool vector without delayed resolution
	CheckSumC(iSum, m_aiNotOnGameOptions);
	CheckSumC(iSum, m_aiOnGameOptions);
	//Arrays
	if (m_ppaiSpecialistYieldChange)
	{
		for(int i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiSpecialistYieldChange[i]);
		}
	}

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piCapitalYieldModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCapitalCommerceModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerce);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSpecialistExtraYield);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSeaPlotYieldChanges);
//Team Project (7)
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piGoldenAgeYieldChanges);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piGoldenAgeCommerceChanges);

	if (m_ppaiSpecialistCommerceChange)
	{
		for(int i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiSpecialistCommerceChange[i]);
		}
	}

	CheckSum(iSum, m_paiLessYieldThreshold, NUM_YIELD_TYPES);
	if (m_ppaiImprovementYieldChange)
	{
		for(int i=0;i<GC.getNumImprovementInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiImprovementYieldChange[i]);
		}
	}
	// int vectors utilizing struct with delayed resolution
	int iNumElements;
	int i;

	iNumElements = m_aImprovementUpgradeModifierTypes.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aImprovementUpgradeModifierTypes[i].eImprovement);
		CheckSum(iSum, m_aImprovementUpgradeModifierTypes[i].iModifier);
	}

	iNumElements = m_aBuildWorkerSpeedModifierTypes.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aBuildWorkerSpeedModifierTypes[i].eBuild);
		CheckSum(iSum, m_aBuildWorkerSpeedModifierTypes[i].iModifier);
	}

	iNumElements = m_aDisallowedTraitTypes.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aDisallowedTraitTypes[i].eTrait);
	}

	iNumElements = m_aDomainFreeExperiences.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aDomainFreeExperiences[i].eDomain);
		CheckSum(iSum, m_aDomainFreeExperiences[i].iModifier);
	}

	iNumElements = m_aDomainProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aDomainProductionModifiers[i].eDomain);
		CheckSum(iSum, m_aDomainProductionModifiers[i].iModifier);
	}

	iNumElements = m_aTechResearchModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aTechResearchModifiers[i].eTech);
		CheckSum(iSum, m_aTechResearchModifiers[i].iModifier);
	}
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());

	iNumElements = m_aBuildingProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aBuildingProductionModifiers[i].eBuilding);
		CheckSum(iSum, m_aBuildingProductionModifiers[i].iModifier);
	}

	iNumElements = m_aSpecialBuildingProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aSpecialBuildingProductionModifiers[i].eSpecialBuilding);
		CheckSum(iSum, m_aSpecialBuildingProductionModifiers[i].iModifier);
	}

	iNumElements = m_aBuildingHappinessModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aBuildingHappinessModifiers[i].eBuilding);
		CheckSum(iSum, m_aBuildingHappinessModifiers[i].iModifier);
	}

	iNumElements = m_aUnitProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitProductionModifiers[i].eUnit);
		CheckSum(iSum, m_aUnitProductionModifiers[i].iModifier);
	}

	iNumElements = m_aSpecialUnitProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aSpecialUnitProductionModifiers[i].eSpecialUnit);
		CheckSum(iSum, m_aSpecialUnitProductionModifiers[i].iModifier);
	}

	iNumElements = m_aCivicOptionNoUpkeepTypes.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aCivicOptionNoUpkeepTypes[i].eCivicOption);
		CheckSum(iSum, m_aCivicOptionNoUpkeepTypes[i].bBool);
	}

//Team Project (8)
	iNumElements = m_aUnitCombatFreeExperiences.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatFreeExperiences[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatFreeExperiences[i].iModifier);
	}

	iNumElements = m_aUnitCombatProductionModifiers.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aUnitCombatProductionModifiers[i].eUnitCombat);
		CheckSum(iSum, m_aUnitCombatProductionModifiers[i].iModifier);
	}

	iNumElements = m_aBonusHappinessChanges.size();
	for (i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aBonusHappinessChanges[i].eBonus);
		CheckSum(iSum, m_aBonusHappinessChanges[i].iModifier);
	}

	//For Pure Traits
	CheckSum(iSum, m_paiExtraYieldThresholdFiltered, NUM_YIELD_TYPES);
	CheckSum(iSum, m_paiTradeYieldModifierFiltered, NUM_YIELD_TYPES);
	CheckSum(iSum, m_paiCommerceChangeFiltered, NUM_COMMERCE_TYPES);
	CheckSum(iSum, m_paiCommerceModifierFiltered, NUM_COMMERCE_TYPES);
	if (m_ppaiSpecialistYieldChangeFiltered)
	{
		for(i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiSpecialistYieldChangeFiltered[i]);
		}
	}

	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldModifierFiltered);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piCapitalYieldModifierFiltered);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCapitalCommerceModifierFiltered);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerceFiltered);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSpecialistExtraYieldFiltered);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChangeFiltered);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSeaPlotYieldChangesFiltered);
//Team Project (7)
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piGoldenAgeYieldChangesFiltered);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piGoldenAgeCommerceChangesFiltered);
	CheckSum(iSum, m_paiLessYieldThresholdFiltered, NUM_YIELD_TYPES);

	if (m_ppaiSpecialistCommerceChangeFiltered)
	{
		for(i=0;i<GC.getNumSpecialistInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiSpecialistCommerceChangeFiltered[i]);
		}
	}

	if (m_ppaiImprovementYieldChangeFiltered)
	{
		for(i=0;i<GC.getNumImprovementInfos();i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiImprovementYieldChangeFiltered[i]);
		}
	}

	//TB Traits Mods end
}

/************************************************************************************************/
/* Afforess					  Start		 08/26/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
bool CvTraitInfo::readPass3()
{
	return true;
}
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

CvPropertyManipulators* CvTraitInfo::getPropertyManipulators()
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_PURE_TRAITS) && isImpurePropertyManipulators())
	{
		return &m_PropertyManipulatorsNull;
	}

	return &m_PropertyManipulators;
}

