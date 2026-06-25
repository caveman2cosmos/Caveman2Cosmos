//------------------------------------------------------------------------------------------------
//  FILE:    CvTechInfo.cpp
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
#include "CvTechInfo.h"


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
m_iResearchCost(-1),
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
m_iFirstFreeUnit(NO_UNIT),
m_iFirstFreeProphet(NO_UNIT),
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
m_bLanguage(false),
// Dale - AB: Bombing START
m_bDCMAirBombTech1(0),
m_bDCMAirBombTech2(0),
// Dale - AB: Bombing END
m_piDomainExtraMoves(NULL),
m_piFlavorValue(NULL),
m_pbCommerceFlexible(NULL),
m_pbTerrainTrade(NULL),
//ls612: Tech Commerce Modifiers
m_piCommerceModifier(NULL)
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
,m_bGlobal(false)
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
	SAFE_DELETE_ARRAY(m_pbCommerceFlexible);
	SAFE_DELETE_ARRAY(m_pbTerrainTrade);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	GC.removeDelayedResolutionVector(m_piPrereqOrTechs);
	GC.removeDelayedResolutionVector(m_piPrereqAndTechs);
	GC.removeDelayedResolutionVector(m_aPrereqBuilding);
	GC.removeDelayedResolutionVector(m_aPrereqOrBuilding);
	GC.removeDelayedResolution((int*)&m_iFirstFreeUnit);
	GC.removeDelayedResolution((int*)&m_iFirstFreeProphet);
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
int CvTechInfo::getFirstFreeUnit() const
{
	return m_iFirstFreeUnit;
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


int CvTechInfo::getAssetValue() const
{
	return m_iAssetValue * 100;
}


int CvTechInfo::getPowerValue() const
{
	return m_iPowerValue * 100;
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


bool CvTechInfo::getDCMAirBombTech1() const
{
	return m_bDCMAirBombTech1;
}


bool CvTechInfo::getDCMAirBombTech2() const
{
	return m_bDCMAirBombTech2;
}


std::wstring CvTechInfo::getQuote()	const
{
	CvWString text = gDLL->getText(m_szQuoteKey);
	FAssert(!text.empty());
	return text;
}


const char* CvTechInfo::getQuoteKey() const
{
	return m_szQuoteKey;
}


const char* CvTechInfo::getSound() const
{
	return m_szSound;
}


const char* CvTechInfo::getSoundMP() const
{
	return m_szSoundMP;
}


// Arrays

int CvTechInfo::getDomainExtraMoves(int i) const
{
	return m_piDomainExtraMoves ? m_piDomainExtraMoves[i] : 0;
}


int CvTechInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}


const std::vector<TechTypes>& CvTechInfo::getPrereqOrTechs() const
{
	return m_piPrereqOrTechs;
}


const std::vector<TechTypes>& CvTechInfo::getPrereqAndTechs() const
{
	return m_piPrereqAndTechs;
}


bool CvTechInfo::isCommerceFlexible(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_pbCommerceFlexible ? m_pbCommerceFlexible[i] : false;
}


bool CvTechInfo::isTerrainTrade(int i) const
{
	return m_pbTerrainTrade ? m_pbTerrainTrade[i] : false;
}


//ls612: Tech Commerce Modifiers
int CvTechInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}


int* CvTechInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}


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


int CvTechInfo::getNumPrereqBuildings() const
{
	return m_aPrereqBuilding.size();
}


const PrereqBuilding& CvTechInfo::getPrereqBuilding(int iIndex) const
{
	return m_aPrereqBuilding[iIndex];
}


int CvTechInfo::getPrereqBuildingType(int iIndex) const
{
	return (int)m_aPrereqBuilding[iIndex].eBuilding;
}


int CvTechInfo::getPrereqBuildingMinimumRequired(int iIndex) const
{
	return m_aPrereqBuilding[iIndex].iMinimumRequired;
}


int CvTechInfo::getNumPrereqOrBuildings() const
{
	return m_aPrereqOrBuilding.size();
}


const PrereqBuilding& CvTechInfo::getPrereqOrBuilding(int iIndex) const
{
	return m_aPrereqOrBuilding[iIndex];
}


int CvTechInfo::getPrereqOrBuildingType(int iIndex) const
{
	return (int)m_aPrereqOrBuilding[iIndex].eBuilding;
}


int CvTechInfo::getPrereqOrBuildingMinimumRequired(int iIndex) const
{
	return m_aPrereqOrBuilding[iIndex].iMinimumRequired;
}


int CvTechInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvTechInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvTechInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


bool CvTechInfo::isGlobal() const
{
	return m_bGlobal;
}


bool CvTechInfo::read(CvXMLLoadUtility* pXML)
{
	PROFILE_EXTRA_FUNC();
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

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Era");
	m_iEra = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FirstFreeUnit");
	GC.addDelayedResolution((int*)&m_iFirstFreeUnit, szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FirstFreeProphet");
	GC.addDelayedResolution((int*)&m_iFirstFreeProphet, szTextVal);

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
	pXML->GetOptionalChildXmlValByName(&m_bLanguage, L"bLanguage");
	pXML->GetOptionalChildXmlValByName(&m_iGridX, L"iGridX");
	pXML->GetOptionalChildXmlValByName(&m_iGridY, L"iGridY");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBombTech1, L"bDCMAirBombTech1");
	pXML->GetOptionalChildXmlValByName(&m_bDCMAirBombTech2, L"bDCMAirBombTech2");

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

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());

	pXML->GetOptionalChildXmlValByName(m_szQuoteKey, L"Quote");
	pXML->GetOptionalChildXmlValByName(m_szSound, L"Sound");
	pXML->GetOptionalChildXmlValByName(m_szSoundMP, L"SoundMP");

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

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, L"FreeSpecialistCounts", GC.getNumSpecialistInfos());

	if(pXML->TryMoveToXmlFirstChild(L"PrereqBuildings"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"PrereqBuilding" );
		m_aPrereqBuilding.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PrereqBuilding"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					pXML->GetChildXmlValByName(&(m_aPrereqBuilding[i].iMinimumRequired), L"iNumBuildingNeeded");
					GC.addDelayedResolution((int*)&(m_aPrereqBuilding[i].eBuilding), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PrereqBuilding"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"PrereqOrBuildings"))
	{
		int i = 0;
		const int iNum = pXML->GetXmlChildrenNumber(L"PrereqOrBuilding" );
		m_aPrereqOrBuilding.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"PrereqOrBuilding"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					pXML->GetChildXmlValByName(&(m_aPrereqOrBuilding[i].iMinimumRequired), L"iNumBuildingNeeded");
					GC.addDelayedResolution((int*)&(m_aPrereqOrBuilding[i].eBuilding), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"PrereqOrBuilding"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	//TB Tech Tags
	pXML->GetOptionalChildXmlValByName(&m_bGlobal, L"bGlobal");
	//TB Tech Tags end

	pXML->SetOptionalVectorWithDelayedResolution(m_piPrereqOrTechs, L"OrPreReqs");
	pXML->SetOptionalVectorWithDelayedResolution(m_piPrereqAndTechs, L"AndPreReqs");
	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	return true;
}


void CvTechInfo::copyNonDefaults(const CvTechInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	CvInfoBase::copyNonDefaults(pClassInfo);

	const bool bDefault = false;
	const int iDefault = 0;
	const int iTextDefault = -1;
	const CvString cDefault = CvString::format("").GetCString();

	if (m_iAdvisorType == NO_ADVISOR) m_iAdvisorType = pClassInfo->getAdvisorType();
	if (getAIWeight() == iDefault) m_iAIWeight = pClassInfo->getAIWeight();
	if (getAITradeModifier() == iDefault) m_iAITradeModifier = pClassInfo->getAITradeModifier();
	if (m_iResearchCost == iDefault) m_iResearchCost = pClassInfo->m_iResearchCost;
	if (m_iEra == NO_ERA) m_iEra = pClassInfo->getEra();
	GC.copyNonDefaultDelayedResolution((int*)&m_iFirstFreeUnit, (int*)&pClassInfo->m_iFirstFreeUnit);
	GC.copyNonDefaultDelayedResolution((int*)&m_iFirstFreeProphet, (int*)&pClassInfo->m_iFirstFreeProphet);
	if (getFeatureProductionModifier() == iDefault) m_iFeatureProductionModifier = pClassInfo->getFeatureProductionModifier();
	if (getWorkerSpeedModifier() == iDefault) m_iWorkerSpeedModifier = pClassInfo->getWorkerSpeedModifier();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getFirstFreeTechs() == iDefault) m_iFirstFreeTechs = pClassInfo->getFirstFreeTechs();
	if (m_iAssetValue == iDefault) m_iAssetValue = pClassInfo->m_iAssetValue;
	if (m_iPowerValue == iDefault) m_iPowerValue = pClassInfo->m_iPowerValue;

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
	if (m_bLanguage == bDefault) m_bLanguage = pClassInfo->isLanguage();

	if (getGridX() == iDefault) m_iGridX = pClassInfo->getGridX();
	if (getGridY() == iDefault) m_iGridY = pClassInfo->getGridY();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

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

	if (getQuoteKey() == cDefault) m_szQuoteKey = pClassInfo->getQuoteKey();
	if (getSound() == cDefault) m_szSound = pClassInfo->getSound();
	if (getSoundMP() == cDefault) m_szSoundMP = pClassInfo->getSoundMP();
	//TB Tech Tags
	if (isGlobal() == bDefault) m_bGlobal = pClassInfo->isGlobal();
	//TB Tech Tags end
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

	if (getNumPrereqBuildings() == 0)
	{
		const int iNum = pClassInfo->getNumPrereqBuildings();
		m_aPrereqBuilding.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPrereqBuilding[i].iMinimumRequired = pClassInfo->m_aPrereqBuilding[i].iMinimumRequired;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPrereqBuilding[i].eBuilding), (int*)&(pClassInfo->m_aPrereqBuilding[i].eBuilding));
		}
	}

	if (getNumPrereqOrBuildings() == 0)
	{
		const int iNum = pClassInfo->getNumPrereqOrBuildings();
		m_aPrereqOrBuilding.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			m_aPrereqOrBuilding[i].iMinimumRequired = pClassInfo->m_aPrereqOrBuilding[i].iMinimumRequired;
			GC.copyNonDefaultDelayedResolution((int*)&(m_aPrereqOrBuilding[i].eBuilding), (int*)&(pClassInfo->m_aPrereqOrBuilding[i].eBuilding));
		}
	}
	GC.copyNonDefaultDelayedResolutionVector(m_piPrereqOrTechs, pClassInfo->getPrereqOrTechs());
	GC.copyNonDefaultDelayedResolutionVector(m_piPrereqAndTechs, pClassInfo->getPrereqAndTechs());
}


void CvTechInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
	CheckSum(iSum, m_iAdvisorType);
	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_iAITradeModifier);
	CheckSum(iSum, m_iResearchCost);
	CheckSum(iSum, m_iEra);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iFeatureProductionModifier);
	CheckSum(iSum, m_iWorkerSpeedModifier);
	CheckSum(iSum, m_iMaintenanceModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iCoastalDistanceMaintenanceModifier);
	CheckSum(iSum, m_iFirstFreeUnit);
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
	CheckSum(iSum, m_bLanguage);

	CheckSum(iSum, m_piDomainExtraMoves, NUM_DOMAIN_TYPES);
	CheckSum(iSum, m_piFlavorValue, GC.getNumFlavorTypes());
	CheckSumC(iSum, m_piPrereqOrTechs);
	CheckSumC(iSum, m_piPrereqAndTechs);
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
	CheckSumC(iSum, m_aiCategories);

	const int iNumElements = m_aPrereqBuilding.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aPrereqBuilding[i].eBuilding);
		CheckSum(iSum, m_aPrereqBuilding[i].iMinimumRequired);
	}
	//TB Tech Tags
	CheckSum(iSum, m_bGlobal);
	//TB Tech Tags end
}


// Toffer - Derived tech cache
void CvTechInfo::setLeadsTo(const TechTypes eTech)
{
	m_leadsTo.insert(eTech);
}


void CvTechInfo::doPostLoadCaching(uint32_t iThis)
{
	PROFILE_EXTRA_FUNC();
	foreach_(const TechTypes ePrereq, getPrereqOrTechs())
	{
		GC.getTechInfo(ePrereq).setLeadsTo((TechTypes)iThis);
	}
	foreach_(const TechTypes ePrereq, getPrereqAndTechs())
	{
		GC.getTechInfo(ePrereq).setLeadsTo((TechTypes)iThis);
	}
}

