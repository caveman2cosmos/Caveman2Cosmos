//------------------------------------------------------------------------------------------------
//  FILE:    CvCivicInfo.cpp
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
#include "CvCivicInfo.h"



//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvCivicInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvCivicInfo::CvCivicInfo()

	: m_iCivicOptionType(NO_CIVICOPTION)
	, m_iAnarchyLength(0)
	, m_iUpkeep(NO_UPKEEP)
	, m_iAIWeight(0)
	, m_iGreatPeopleRateModifier(0)
	, m_iGreatGeneralRateModifier(0)
	, m_iDomesticGreatGeneralRateModifier(0)
	, m_iStateReligionGreatPeopleRateModifier(0)
	, m_iDistanceMaintenanceModifier(0)
	, m_iNumCitiesMaintenanceModifier(0)
	, m_iHomeAreaMaintenanceModifier(0)
	, m_iOtherAreaMaintenanceModifier(0)
	, m_iCorporationMaintenanceModifier(0)
	, m_iExtraHealth(0)
	, m_iFreeExperience(0)
	, m_iWorkerSpeedModifier(0)
	, m_iImprovementUpgradeRateModifier(0)
	, m_iMilitaryProductionModifier(0)
	, m_iFreeUnitUpkeepCivilian(0)
	, m_iFreeUnitUpkeepMilitary(0)
	, m_iFreeUnitUpkeepCivilianPopPercent(0)
	, m_iFreeUnitUpkeepMilitaryPopPercent(0)

	, m_iCivilianUnitUpkeepMod(0)
	, m_iMilitaryUnitUpkeepMod(0)

	, m_iHappyPerMilitaryUnit(0)
	, m_iLargestCityHappiness(0)
	, m_iWarWearinessModifier(0)
	, m_iFreeSpecialist(0)
	, m_iTradeRoutes(0)
	, m_iTechPrereq(NO_TECH)
	, m_iCivicPercentAnger(0)
	, m_iMaxConscript(0)
	, m_iStateReligionHappiness(0)
	, m_iNonStateReligionHappiness(0)
	, m_iStateReligionUnitProductionModifier(0)
	, m_iStateReligionBuildingProductionModifier(0)
	, m_iStateReligionFreeExperience(0)
	, m_iExpInBorderModifier(0)

	, m_bUpgradeAnywhere(false)
	, m_bAllowInquisitions(false)
	, m_bDisallowInquisitions(false)
	, m_iRevIdxLocal(0)
	, m_iRevIdxNational(0)
	, m_iRevIdxDistanceModifier(0)
	, m_iRevIdxHolyCityGood(0)
	, m_iRevIdxHolyCityBad(0)
	, m_iRevIdxSwitchTo(0)
	, m_fRevIdxNationalityMod(0)
	, m_fRevIdxGoodReligionMod(0)
	, m_fRevIdxBadReligionMod(0)
	, m_fRevViolentMod(0)
	, m_iRevReligiousFreedom(0)
	, m_iRevLaborFreedom(0)
	, m_iRevEnvironmentalProtection(0)
	, m_iRevDemocracyLevel(0)
	, m_bCommunism(false)
	, m_bFreeSpeech(false)
	, m_bCanDoElection(false)

	, m_bMilitaryFoodProduction(false)
	, m_bNoUnhealthyPopulation(false)
	, m_bBuildingOnlyHealthy(false)
	, m_bNoForeignTrade(false)
	, m_bNoCorporations(false)
	, m_bNoForeignCorporations(false)
	, m_bStateReligion(false)
	, m_bNoNonStateReligionSpread(false)
	, m_piYieldModifier(NULL)
	, m_piCapitalYieldModifier(NULL)
	, m_piTradeYieldModifier(NULL)
	, m_piCommerceModifier(NULL)
	, m_piCapitalCommerceModifier(NULL)
	, m_piSpecialistExtraCommerce(NULL)
	, m_paiBuildingHappinessChanges(NULL)
	, m_paiBuildingHealthChanges(NULL)
	, m_bSparseListsCached(false)
	, m_paiFeatureHappinessChanges(NULL)
	, m_pabHurry(NULL)
	, m_pabSpecialBuildingNotRequired(NULL)
	, m_pabSpecialistValid(NULL)

	, m_ppiImprovementYieldChanges(NULL)
	, m_bAnyImprovementYieldChange(false)

	, m_iAttitudeShareMod(0)
	, m_iEnslavementChance(0)
	, m_iPopulationgrowthratepercentage(0)
	, m_iReligionSpreadRate(0)
	, m_iCivicHappiness(0)
	, m_iDistantUnitSupportCostModifier(0)
	, m_iExtraCityDefense(0)
	, m_iForeignTradeRouteModifier(0)
	, m_iTaxRateUnhappiness(0)
	, m_iInflationModifier(0)
	, m_iHurryInflationModifier(0)
	, m_iHurryCostModifier(0)
	, m_iSharedCivicTradeRouteModifier(0)
	, m_iLandmarkHappiness(0)
	, m_iCorporationSpreadRate(0)
	, m_iForeignerUnhappyPercent(0)
	, m_iCityLimit(0)
	, m_iCityOverLimitUnhappy(0)
	, m_bFixedBorders(false)
	, m_bNoCapitalUnhappiness(false)
	, m_bNoLandmarkAnger(false)
	, m_piBonusMintedPercent(NULL)
	, m_piImprovementHappinessChanges(NULL)
	, m_piImprovementHealthPercentChanges(NULL)
	, m_ppiSpecialistYieldPercentChanges(NULL)
	, m_ppiSpecialistCommercePercentChanges(NULL)
	, m_piLandmarkYieldChanges(NULL)
	, m_piFreeSpecialistCount(NULL)
	, m_paiUnitCombatProductionModifier(NULL)
	, m_piUnitProductionModifier(NULL)
	, m_ppiTerrainYieldChanges(NULL)
	, m_piFlavorValue(NULL)
	, m_piCivicAttitudeChanges(NULL)
	, m_pszCivicAttitudeReason(NULL)
	, m_ppiBuildingCommerceModifier(NULL)
	, m_ppiBuildingCommerceChange(NULL)
	, m_ppiBonusCommerceModifier(NULL)
	, m_iNationalCaptureProbabilityModifier(0)
	, m_iNationalCaptureResistanceModifier(0)
	, m_iFreedomFighterChange(0)
	, m_bAllReligionsActive(false)
	, m_bBansNonStateReligions(false)
	, m_bFreedomFighter(false)
	, m_bPolicy(false)
	, m_PropertyManipulators()
	{ }


//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvCivicInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvCivicInfo::~CvCivicInfo()
{
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
	SAFE_DELETE_ARRAY2(m_ppiImprovementYieldChanges, GC.getNumImprovementInfos());
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piBonusMintedPercent);
	SAFE_DELETE_ARRAY(m_piCivicAttitudeChanges);
	SAFE_DELETE_ARRAY(m_pszCivicAttitudeReason);
	SAFE_DELETE_ARRAY(m_paiUnitCombatProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitProductionModifier);
	SAFE_DELETE_ARRAY(m_piLandmarkYieldChanges);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piImprovementHappinessChanges);
	SAFE_DELETE_ARRAY(m_piImprovementHealthPercentChanges);
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBuildingCommerceChange, GC.getNumBuildingInfos());
	SAFE_DELETE_ARRAY2(m_ppiBonusCommerceModifier, GC.getNumBonusInfos());
	SAFE_DELETE_ARRAY2(m_ppiTerrainYieldChanges, GC.getNumTerrainInfos());
	SAFE_DELETE_ARRAY2(m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos());
}


const wchar_t* CvCivicInfo::getWeLoveTheKing() const
{
	return m_szWeLoveTheKingKey;
}


const wchar_t* CvCivicInfo::getWeLoveTheKingKey() const
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


int CvCivicInfo::getFreeUnitUpkeepCivilian() const
{
	return m_iFreeUnitUpkeepCivilian;
}


int CvCivicInfo::getFreeUnitUpkeepMilitary() const
{
	return m_iFreeUnitUpkeepMilitary;
}


int CvCivicInfo::getFreeUnitUpkeepCivilianPopPercent() const
{
	return m_iFreeUnitUpkeepCivilianPopPercent;
}


int CvCivicInfo::getFreeUnitUpkeepMilitaryPopPercent() const
{
	return m_iFreeUnitUpkeepMilitaryPopPercent;
}


int CvCivicInfo::getCivilianUnitUpkeepMod() const
{
	return m_iCivilianUnitUpkeepMod;
}


int CvCivicInfo::getMilitaryUnitUpkeepMod() const
{
	return m_iMilitaryUnitUpkeepMod;
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


bool CvCivicInfo::isNoLandmarkAnger() const
{
	return m_bNoLandmarkAnger;
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
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piYieldModifier ? m_piYieldModifier[i] : 0;
}


int* CvCivicInfo::getYieldModifierArray() const
{
	return m_piYieldModifier;
}


int CvCivicInfo::getCapitalYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piCapitalYieldModifier ? m_piCapitalYieldModifier[i] : 0;
}


int* CvCivicInfo::getCapitalYieldModifierArray() const
{
	return m_piCapitalYieldModifier;
}


int CvCivicInfo::getTradeYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piTradeYieldModifier ? m_piTradeYieldModifier[i] : 0;
}


int* CvCivicInfo::getTradeYieldModifierArray() const
{
	return m_piTradeYieldModifier;
}


int CvCivicInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}


int* CvCivicInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}


int CvCivicInfo::getCapitalCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piCapitalCommerceModifier ? m_piCapitalCommerceModifier[i] : 0;
}


int* CvCivicInfo::getCapitalCommerceModifierArray() const
{
	return m_piCapitalCommerceModifier;
}


int CvCivicInfo::getSpecialistExtraCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i);
	return m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
}


int* CvCivicInfo::getSpecialistExtraCommerceArray() const
{
	return m_piSpecialistExtraCommerce;
}


int CvCivicInfo::getCivicAttitudeChange(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), i);
	return m_piCivicAttitudeChanges ? m_piCivicAttitudeChanges[i] : 0;
}


int* CvCivicInfo::getCivicAttitudeChanges() const
{
	return m_piCivicAttitudeChanges;
}


int CvCivicInfo::getLandmarkYieldChanges(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i);
	return m_piLandmarkYieldChanges ? m_piLandmarkYieldChanges[i] : 0;
}


int* CvCivicInfo::getLandmarkYieldChangesArray() const
{
	return m_piLandmarkYieldChanges;
}



int CvCivicInfo::getBonusCommerceModifier(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);
	return (m_ppiBonusCommerceModifier && m_ppiBonusCommerceModifier[i]) ? m_ppiBonusCommerceModifier[i][j] : 0;
}


int* CvCivicInfo::getBonusCommerceModifierArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_ppiBonusCommerceModifier ? m_ppiBonusCommerceModifier[i] : NULL;
}



int CvCivicInfo::getBuildingHappinessChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	return m_paiBuildingHappinessChanges ? m_paiBuildingHappinessChanges[i] : 0;
}


int CvCivicInfo::getBuildingHealthChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	return m_paiBuildingHealthChanges ? m_paiBuildingHealthChanges[i] : 0;
}

// Walk the dense arrays once and record (building id, value) pairs for the
// non-zero entries. The dense arrays may be NULL when no modifier was loaded
// for this civic, in which case the corresponding sparse list stays empty.
// Called only from the public sparse accessors below; safe to call from a
// const context because the cache fields are mutable.
//
// Invariant: cache is valid until invalidateSparseLists() is called, which
// happens whenever copyNonDefaults() writes to one of the dense arrays.
void CvCivicInfo::cacheSparseListsIfNeeded() const
{
	PROFILE_EXTRA_FUNC();
	if (m_bSparseListsCached)
	{
		return;
	}
	m_vBuildingHappinessChangesSparse.clear();
	m_vBuildingHealthChangesSparse.clear();
	m_vFeatureHappinessChangesSparse.clear();

	const int iNumBuildings = GC.getNumBuildingInfos();
	for (int i = 0; i < iNumBuildings; ++i)
	{
		const int iHappy = m_paiBuildingHappinessChanges ? m_paiBuildingHappinessChanges[i] : 0;
		if (iHappy != 0)
		{
			m_vBuildingHappinessChangesSparse.push_back(BuildingModifier2((BuildingTypes)i, iHappy));
		}
		const int iHealth = m_paiBuildingHealthChanges ? m_paiBuildingHealthChanges[i] : 0;
		if (iHealth != 0)
		{
			m_vBuildingHealthChangesSparse.push_back(BuildingModifier2((BuildingTypes)i, iHealth));
		}
	}

	const int iNumFeatures = GC.getNumFeatureInfos();
	for (int i = 0; i < iNumFeatures; ++i)
	{
		const int iHappy = m_paiFeatureHappinessChanges ? m_paiFeatureHappinessChanges[i] : 0;
		if (iHappy != 0)
		{
			m_vFeatureHappinessChangesSparse.push_back(std::pair<FeatureTypes, int>((FeatureTypes)i, iHappy));
		}
	}

	m_bSparseListsCached = true;
}


const std::vector<BuildingModifier2>& CvCivicInfo::getBuildingHappinessChangesSparse() const
{
	cacheSparseListsIfNeeded();
	return m_vBuildingHappinessChangesSparse;
}


const std::vector<BuildingModifier2>& CvCivicInfo::getBuildingHealthChangesSparse() const
{
	cacheSparseListsIfNeeded();
	return m_vBuildingHealthChangesSparse;
}


const std::vector<std::pair<FeatureTypes, int> >& CvCivicInfo::getFeatureHappinessChangesSparse() const
{
	cacheSparseListsIfNeeded();
	return m_vFeatureHappinessChangesSparse;
}


void CvCivicInfo::invalidateSparseLists()
{
	m_bSparseListsCached = false;
}

int CvCivicInfo::getFeatureHappinessChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i);
	return m_paiFeatureHappinessChanges ? m_paiFeatureHappinessChanges[i] : 0;
}


int CvCivicInfo::getBonusMintedPercent(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i);
	return m_piBonusMintedPercent ? m_piBonusMintedPercent[i] : 0;
}


int CvCivicInfo::getFreeSpecialistCount(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}


int CvCivicInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i);
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}


int CvCivicInfo::getUnitCombatProductionModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitCombatInfos(), i);
	return m_paiUnitCombatProductionModifier ? m_paiUnitCombatProductionModifier[i] : 0;
}


int CvCivicInfo::getUnitProductionModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumUnitInfos(), i);
	return m_piUnitProductionModifier ? m_piUnitProductionModifier[i] : 0;
}


int CvCivicInfo::getBuildingProductionModifier(BuildingTypes e) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), e);
	return m_aBuildingProductionModifier.getValue(e);
}


int CvCivicInfo::getImprovementHappinessChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	return m_piImprovementHappinessChanges ? m_piImprovementHappinessChanges[i] : 0;
}


int CvCivicInfo::getImprovementHealthPercentChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	return m_piImprovementHealthPercentChanges ? m_piImprovementHealthPercentChanges[i] : 0;
}



bool CvCivicInfo::isHurry(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumHurryInfos(), i);
	return m_pabHurry ? m_pabHurry[i] : false;
}


bool CvCivicInfo::isSpecialBuildingNotRequired(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialBuildingInfos(), i);
	return m_pabSpecialBuildingNotRequired ? m_pabSpecialBuildingNotRequired[i] : false;
}


bool CvCivicInfo::isSpecialistValid(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	return m_pabSpecialistValid ? m_pabSpecialistValid[i] : false;
}



int CvCivicInfo::getImprovementYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return (m_ppiImprovementYieldChanges && m_ppiImprovementYieldChanges[i]) ? m_ppiImprovementYieldChanges[i][j] : 0;
}


int CvCivicInfo::getTerrainYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return (m_ppiTerrainYieldChanges && m_ppiTerrainYieldChanges[i]) ? m_ppiTerrainYieldChanges[i][j] : 0;
}


int CvCivicInfo::getBuildingCommerceModifier(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);
	return (m_ppiBuildingCommerceModifier && m_ppiBuildingCommerceModifier[i]) ? m_ppiBuildingCommerceModifier[i][j] : 0;
}


int CvCivicInfo::getBuildingCommerceChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBuildingInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);
	return (m_ppiBuildingCommerceChange && m_ppiBuildingCommerceChange[i]) ? m_ppiBuildingCommerceChange[i][j] : 0;
}


int CvCivicInfo::getSpecialistYieldPercentChanges(int i, int j ) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j);
	return (m_ppiSpecialistYieldPercentChanges && m_ppiSpecialistYieldPercentChanges[i]) ? m_ppiSpecialistYieldPercentChanges[i][j] : 0;
}


int CvCivicInfo::getSpecialistCommercePercentChanges(int i, int j ) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i);
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j);
	return (m_ppiSpecialistCommercePercentChanges && m_ppiSpecialistCommercePercentChanges[i]) ? m_ppiSpecialistCommercePercentChanges[i][j] : 0;
}



int CvCivicInfo::getCityLimit(PlayerTypes ePlayer) const
{
	if (ePlayer > NO_PLAYER && GC.getGame().isOption(GAMEOPTION_EXP_OVEREXPANSION_PENALTIES))
	{
		const int iAdaptID = GC.getInfoTypeForString("ADAPT_SCALE_CITY_LIMITS");
		return GET_PLAYER(ePlayer).getGameObject()->adaptValueToGame(iAdaptID , m_iCityLimit);
	}
	return 0;
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


CvString CvCivicInfo::getCivicAttitudeReason(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), i);
	return m_pszCivicAttitudeReason[i];
}


int CvCivicInfo::getCategory(int i) const
{
	return m_aiCategories[i];
}


int CvCivicInfo::getNumCategories() const
{
	return (int)m_aiCategories.size();
}


bool CvCivicInfo::isCategory(int i) const
{
	return algo::any_of_equal(m_aiCategories, i);
}


int CvCivicInfo::getCivicAttitudeVectorSize() const						{ return m_aszCivicAttitudeforPass3.size(); }

CvString CvCivicInfo::getCivicAttitudeNamesVectorElement(int i) const	{ return m_aszCivicAttitudeforPass3[i]; }

int CvCivicInfo::getCivicAttitudeValuesVectorElement(int i) const		{ return m_aiCivicAttitudeforPass3[i]; }


int CvCivicInfo::getCivicAttitudeReasonVectorSize() const						{ return m_aszCivicAttitudeReasonforPass3.size(); }

CvString CvCivicInfo::getCivicAttitudeReasonNamesVectorElement(int i) const		{ return m_aszCivicAttitudeReasonforPass3[i]; }

CvString CvCivicInfo::getCivicAttitudeReasonValuesVectorElement(int i) const	{ return m_aszCivicAttitudeReasonValueforPass3[i]; }



void CvCivicInfo::getCheckSum(uint32_t& iSum) const
{
	PROFILE_EXTRA_FUNC();
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
	CheckSum(iSum, m_iFreeUnitUpkeepCivilian);
	CheckSum(iSum, m_iFreeUnitUpkeepMilitary);
	CheckSum(iSum, m_iFreeUnitUpkeepCivilianPopPercent);
	CheckSum(iSum, m_iFreeUnitUpkeepMilitaryPopPercent);
	CheckSum(iSum, m_iCivilianUnitUpkeepMod);
	CheckSum(iSum, m_iMilitaryUnitUpkeepMod);
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
	CheckSum(iSum, m_bNoLandmarkAnger);
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
	CheckSumI(iSum, GC.getNumBuildingInfos(), m_paiBuildingHappinessChanges);
	CheckSumI(iSum, GC.getNumBuildingInfos(), m_paiBuildingHealthChanges);
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
	CheckSumI(iSum, GC.getNumUnitInfos(), m_piUnitProductionModifier);
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

	if (m_ppiBuildingCommerceChange)
	{
		for(i=0;i<GC.getNumBuildingInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiBuildingCommerceChange[i]);
		}
	}

	if (m_ppiBonusCommerceModifier)
	{
		for(i=0;i<GC.getNumBonusInfos();i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppiBonusCommerceModifier[i]);
		}
	}

	CheckSumC(iSum, m_aBuildingProductionModifier);
	CheckSumC(iSum, m_aiCategories);

	m_PropertyManipulators.getCheckSum(iSum);
}


bool CvCivicInfo::read(CvXMLLoadUtility* pXML)
{

	PROFILE_EXTRA_FUNC();
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

	pXML->GetOptionalTypeEnum(m_iTechPrereq, L"TechPrereq");

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
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepCivilian, L"iFreeUnitUpkeepCivilian");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepMilitary, L"iFreeUnitUpkeepMilitary");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepCivilianPopPercent, L"iFreeUnitUpkeepCivilianPopPercent");
	pXML->GetOptionalChildXmlValByName(&m_iFreeUnitUpkeepMilitaryPopPercent, L"iFreeUnitUpkeepMilitaryPopPercent");
	pXML->GetOptionalChildXmlValByName(&m_iCivilianUnitUpkeepMod, L"iCivilianUnitUpkeepMod");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryUnitUpkeepMod, L"iMilitaryUnitUpkeepMod");
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
	pXML->GetOptionalChildXmlValByName(&m_bNoLandmarkAnger, L"bNoLandmarkAnger");
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
	pXML->SetVariableListTagPair(&m_pabSpecialistValid, L"SpecialistValids", GC.getNumSpecialistInfos());
	pXML->SetVariableListTagPair(&m_paiBuildingHappinessChanges, L"BuildingHappinessChanges", GC.getNumBuildingInfos());
	pXML->SetVariableListTagPair(&m_paiBuildingHealthChanges, L"BuildingHealthChanges", GC.getNumBuildingInfos());
	pXML->SetVariableListTagPair(&m_paiFeatureHappinessChanges, L"FeatureHappinessChanges", GC.getNumFeatureInfos());

	pXML->SetOptionalVector(&m_aiCategories, L"Categories");

	m_bAnyImprovementYieldChange = false;
	if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiImprovementYieldChanges, GC.getNumImprovementInfos(), NUM_YIELD_TYPES);
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

	pXML->GetOptionalChildXmlValByName(m_szWeLoveTheKingKey, L"WeLoveTheKing");

	pXML->SetVariableListTagPair(&m_piBonusMintedPercent, L"BonusMintedPercents", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piImprovementHappinessChanges, L"ImprovementHappinessChanges", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPair(&m_piImprovementHealthPercentChanges, L"ImprovementHealthPercentChanges", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPair(&m_paiUnitCombatProductionModifier, L"UnitCombatProductionModifiers", GC.getNumUnitCombatInfos());

	m_aBuildingProductionModifier.read(pXML, L"BuildingProductionModifiers");

	if (pXML->TryMoveToXmlFirstChild(L"UnitProductionModifiers"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		int iTemp = false;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i = 0; i < iNumSibs; i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszUnitProdModforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&iTemp);
						m_aiUnitProdModforPass3.push_back(iTemp);
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

	pXML->SetVariableListTagPair(&m_piFlavorValue, L"Flavors", GC.getNumFlavorTypes());
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

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldPercentChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos(), NUM_YIELD_TYPES);
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

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommercePercentChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos(), NUM_COMMERCE_TYPES);
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

	if (pXML->TryMoveToXmlFirstChild(L"TerrainYieldChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiTerrainYieldChanges, GC.getNumTerrainInfos(), NUM_YIELD_TYPES);
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
				pXML->Init2DList(&m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
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

	if (pXML->TryMoveToXmlFirstChild(L"BuildingCommerceChanges"))
	{
		iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				pXML->Init2DList(&m_ppiBuildingCommerceChange, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
				for (j=0;j<iNumSibs;j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"BuildingType");
					iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppiBuildingCommerceChange[iIndex]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
						{
							// call the function that sets the yield change variable
							pXML->SetCommerce(&m_ppiBuildingCommerceChange[iIndex]);
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
				pXML->Init2DList(&m_ppiBonusCommerceModifier, GC.getNumBonusInfos(), NUM_COMMERCE_TYPES);
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
	PROFILE_EXTRA_FUNC();
	m_piCivicAttitudeChanges = new int[GC.getNumCivicInfos()];
	m_pszCivicAttitudeReason = new CvString[GC.getNumCivicInfos()];
	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		m_piCivicAttitudeChanges[iI] = 0;
		m_pszCivicAttitudeReason[iI] = "";
	}
	if (!m_aiCivicAttitudeforPass3.empty() && !m_aszCivicAttitudeforPass3.empty())
	{
		const int iNumLoad = m_aiCivicAttitudeforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			const int iTempIndex = GC.getInfoTypeForString(m_aszCivicAttitudeforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_piCivicAttitudeChanges[iTempIndex] = m_aiCivicAttitudeforPass3[iI];
		}
		m_aszCivicAttitudeforPass3.clear();
		m_aiCivicAttitudeforPass3.clear();
	}
	if (!m_aszCivicAttitudeReasonValueforPass3.empty() && !m_aszCivicAttitudeReasonforPass3.empty())
	{
		const int iNumLoad = m_aszCivicAttitudeReasonValueforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			const int iTempIndex = GC.getInfoTypeForString(m_aszCivicAttitudeReasonforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_pszCivicAttitudeReason[iTempIndex] = m_aszCivicAttitudeReasonValueforPass3[iI];
		}
		m_aszCivicAttitudeReasonforPass3.clear();
		m_aszCivicAttitudeReasonValueforPass3.clear();
	}

	m_piUnitProductionModifier = new int[GC.getNumUnitInfos()];
    for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		m_piUnitProductionModifier[iI] = 0;
	}
	if (!m_aiUnitProdModforPass3.empty() && !m_aszUnitProdModforPass3.empty())
	{
		int iNumLoad = m_aiUnitProdModforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			int iTempIndex = GC.getInfoTypeForString(m_aszUnitProdModforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumUnitInfos())
			{
				m_piUnitProductionModifier[iTempIndex] = m_aiUnitProdModforPass3[iI];
			}
		}
		m_aszUnitProdModforPass3.clear();
		m_aiUnitProdModforPass3.clear();
	}

	return true;
}


void CvCivicInfo::copyNonDefaults(const CvCivicInfo* pClassInfo)
{
	PROFILE_EXTRA_FUNC();
	bool bDefault = false;
	int iDefault = 0;
	int iTextDefault = -1;  //all integers which are TEXT_KEYS in the xml are -1 by default
	float fDefault = 0.0f;
	CvString cDefault = CvString::format("").GetCString();
	CvWString wDefault = CvWString::format(L"").GetCString();

	CvInfoBase::copyNonDefaults(pClassInfo);

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
	if (m_iFreeUnitUpkeepCivilian == iDefault) m_iFreeUnitUpkeepCivilian = pClassInfo->getFreeUnitUpkeepCivilian();
	if (m_iFreeUnitUpkeepMilitary == iDefault) m_iFreeUnitUpkeepMilitary = pClassInfo->getFreeUnitUpkeepMilitary();
	if (m_iFreeUnitUpkeepCivilianPopPercent == iDefault) m_iFreeUnitUpkeepCivilianPopPercent = pClassInfo->getFreeUnitUpkeepCivilianPopPercent();
	if (m_iFreeUnitUpkeepMilitaryPopPercent == iDefault) m_iFreeUnitUpkeepMilitaryPopPercent = pClassInfo->getFreeUnitUpkeepMilitaryPopPercent();
	if (m_iCivilianUnitUpkeepMod == iDefault) m_iCivilianUnitUpkeepMod = pClassInfo->getCivilianUnitUpkeepMod();
	if (m_iMilitaryUnitUpkeepMod == iDefault) m_iMilitaryUnitUpkeepMod = pClassInfo->getMilitaryUnitUpkeepMod();
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

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiCategories, pClassInfo->m_aiCategories);

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

    // Note: any write into m_paiBuildingHappinessChanges /
    // m_paiBuildingHealthChanges below must be followed by
    // invalidateSparseLists() so the next sparse-view access rebuilds from the
    // updated dense storage. The two single calls below cover the only mutating
    // branches in this block.
	for ( int i = 0; i < GC.getNumBuildingInfos(); i++ )
	{
		if ( getBuildingHappinessChanges(i) == iDefault && pClassInfo->getBuildingHappinessChanges(i) != iDefault)
		{
			if ( NULL == m_paiBuildingHappinessChanges )
			{
				CvXMLLoadUtility::InitList(&m_paiBuildingHappinessChanges,GC.getNumBuildingInfos(),iDefault);
			}
			m_paiBuildingHappinessChanges[i] = pClassInfo->getBuildingHappinessChanges(i);
			invalidateSparseLists();
		}
		if ( getBuildingHealthChanges(i) == iDefault && pClassInfo->getBuildingHealthChanges(i) != iDefault)
		{
			if ( NULL == m_paiBuildingHealthChanges )
			{
				CvXMLLoadUtility::InitList(&m_paiBuildingHealthChanges,GC.getNumBuildingInfos(),iDefault);
			}
			m_paiBuildingHealthChanges[i] = pClassInfo->getBuildingHealthChanges(i);
			invalidateSparseLists();
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
			invalidateSparseLists();
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
					CvXMLLoadUtility::Init2DList(&m_ppiImprovementYieldChanges, GC.getNumImprovementInfos(), NUM_YIELD_TYPES);
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
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();
	if (getHurryInflationModifier() == iDefault) m_iHurryInflationModifier = pClassInfo->getHurryInflationModifier();
	if (getHurryCostModifier() == iDefault) m_iHurryCostModifier = pClassInfo->getHurryCostModifier();
	if (getSharedCivicTradeRouteModifier() == iDefault) m_iSharedCivicTradeRouteModifier = pClassInfo->getSharedCivicTradeRouteModifier();
	if (getLandmarkHappiness() == iDefault) m_iLandmarkHappiness = pClassInfo->getLandmarkHappiness();
	if (getCorporationSpreadRate() == iDefault) m_iCorporationSpreadRate = pClassInfo->getCorporationSpreadRate();
	if (isNoLandmarkAnger() == bDefault) m_bNoLandmarkAnger = pClassInfo->isNoLandmarkAnger();
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
					CvXMLLoadUtility::Init2DList(&m_ppiSpecialistYieldPercentChanges, GC.getNumSpecialistInfos(), NUM_YIELD_TYPES);
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
					CvXMLLoadUtility::Init2DList(&m_ppiSpecialistCommercePercentChanges, GC.getNumSpecialistInfos(), NUM_COMMERCE_TYPES);
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

	m_aBuildingProductionModifier.copyNonDefaults(pClassInfo->getBuildingProductionModifiers());

	for ( int i = 0; i < GC.getNumBuildingInfos(); i++ )
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++ )
		{
			if ( getBuildingCommerceChange(i,j) == iDefault && pClassInfo->getBuildingCommerceChange(i,j) != iDefault)
			{
				if ( NULL == m_ppiBuildingCommerceChange )
				{
					CvXMLLoadUtility::Init2DList(&m_ppiBuildingCommerceChange, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
				}
				else if ( NULL == m_ppiBuildingCommerceChange[i] )
				{
					CvXMLLoadUtility::InitList(&m_ppiBuildingCommerceChange[i],NUM_COMMERCE_TYPES,iDefault);
				}
				m_ppiBuildingCommerceChange[i][j] = pClassInfo->getBuildingCommerceChange(i,j);
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
					CvXMLLoadUtility::Init2DList(&m_ppiTerrainYieldChanges, GC.getNumTerrainInfos(), NUM_YIELD_TYPES);
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
					CvXMLLoadUtility::Init2DList(&m_ppiBuildingCommerceModifier, GC.getNumBuildingInfos(), NUM_COMMERCE_TYPES);
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
					CvXMLLoadUtility::Init2DList(&m_ppiBonusCommerceModifier, GC.getNumBonusInfos(), NUM_COMMERCE_TYPES);
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

	//TB Civic Tags
	//Integers
	if (getNationalCaptureProbabilityModifier() == iDefault) m_iNationalCaptureProbabilityModifier = pClassInfo->getNationalCaptureProbabilityModifier();
	if (getNationalCaptureResistanceModifier() == iDefault) m_iNationalCaptureResistanceModifier = pClassInfo->getNationalCaptureResistanceModifier();

	//Booleans
	if (isAllReligionsActive() == bDefault) m_bAllReligionsActive = pClassInfo->isAllReligionsActive();
	if (isBansNonStateReligions() == bDefault) m_bBansNonStateReligions = pClassInfo->isBansNonStateReligions();

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);
}

