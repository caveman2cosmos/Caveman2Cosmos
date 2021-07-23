//  $Header:
//------------------------------------------------------------------------------------------------
//
//  FILE:	CvBuildingInfos.cpp
//
//  PURPOSE: Info class for buildings
//
//------------------------------------------------------------------------------------------------
//  Copyright (c) 2003 Firaxis Games, Inc. All rights reserved.
//------------------------------------------------------------------------------------------------
#include "CvGameCoreDLL.h"
#include "CvArtFileMgr.h"
#include "CvBuildingInfo.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvXMLLoadUtility.h"
#include "CheckSum.h"

//======================================================================================================
//					CvBuildingInfo
//======================================================================================================

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   CvBuildingInfo()
//
//  PURPOSE :   Default constructor
//
//------------------------------------------------------------------------------------------------------
CvBuildingInfo::CvBuildingInfo() :
m_iMaxGlobalInstances(-1),
m_iMaxTeamInstances(-1),
m_iMaxPlayerInstances(-1),
m_iExtraPlayerInstances(0),
m_piVictoryThreshold(NULL),
m_iDCMAirbombMission(0),
m_bNoLimit(false),
m_iVictoryPrereq(NO_VICTORY),
m_iFreeStartEra(NO_ERA),
m_iMaxStartEra(NO_ERA),
m_iObsoleteTech(NO_TECH),
m_iPrereqAndTech(NO_TECH),
m_iNoBonus(NO_BONUS),
m_iPowerBonus(NO_BONUS),
m_iFreeBonus(NO_BONUS),
m_iNumFreeBonuses(0),
m_iFreeBuilding(NO_BUILDING),
m_iFreeAreaBuilding(NO_BUILDING),
m_iFreePromotion(NO_PROMOTION),
m_iCivicOption(NO_CIVICOPTION),
m_iAIWeight(0),
m_iProductionCost(-1),
m_iHurryCostModifier(0),
m_iHurryAngerModifier(0),
m_iMinAreaSize(0),
m_iNumCitiesPrereq(0),
m_iNumTeamsPrereq(0),
m_iUnitLevelPrereq(0),
m_iMinLatitude(0),
m_iMaxLatitude(90),
m_iGreatPeopleRateModifier(0),
m_iGreatGeneralRateModifier(0),
m_iDomesticGreatGeneralRateModifier(0),
m_iGlobalGreatPeopleRateModifier(0),
m_iAnarchyModifier(0),
m_iGoldenAgeModifier(0),
m_iGlobalHurryModifier(0),
m_iFreeExperience(0),
m_iGlobalFreeExperience(0),
m_iFoodKept(0),
m_iAirlift(0),
m_iAirModifier(0),
m_iAirUnitCapacity(0),
m_iNukeModifier(0),
m_iNukeExplosionRand(0),
m_iFreeSpecialist(0),
m_iAreaFreeSpecialist(0),
m_iGlobalFreeSpecialist(0),
m_iHappiness(0),
m_iAreaHappiness(0),
m_iGlobalHappiness(0),
m_iStateReligionHappiness(0),
m_iWorkerSpeedModifier(0),
m_iMilitaryProductionModifier(0),
m_iSpaceProductionModifier(0),
m_iGlobalSpaceProductionModifier(0),
m_iTradeRoutes(0),
m_iCoastalTradeRoutes(0),
m_iGlobalTradeRoutes(0),
m_iTradeRouteModifier(0),
m_iForeignTradeRouteModifier(0),
m_iAssetValue(0),
m_iPowerValue(0),
m_eSpecialBuilding(NO_SPECIALBUILDING),
m_iAdvisorType(NO_ADVISOR),

m_iPrereqGameOption(NO_GAMEOPTION),
m_iNotGameOption(NO_GAMEOPTION),

m_iHolyCity(NO_RELIGION),
m_iReligionType(NO_RELIGION),
m_iStateReligion(NO_RELIGION),
m_iPrereqReligion(NO_RELIGION),
m_iPrereqCorporation(NO_CORPORATION),
m_iFoundsCorporation(NO_CORPORATION),
m_iGlobalReligionCommerce(NO_RELIGION),
m_iGlobalCorporationCommerce(NO_CORPORATION),
m_iPrereqAndBonus(NO_BONUS),
m_iGreatPeopleUnitType(NO_UNIT),
m_iGreatPeopleRateChange(0),
m_iConquestProbability(50),
m_iMaintenanceModifier(0),
//DPII < Maintenance Modifier >
m_iGlobalMaintenanceModifier(0),
m_iAreaMaintenanceModifier(0),
m_iOtherAreaMaintenanceModifier(0),
m_iDistanceMaintenanceModifier(0),
m_iNumCitiesMaintenanceModifier(0),
m_iCoastalDistanceMaintenanceModifier(0),
m_iConnectedCityMaintenanceModifier(0),
//DPII < Maintenance Modifier >
m_iWarWearinessModifier(0),
m_iGlobalWarWearinessModifier(0),
m_iEnemyWarWearinessModifier(0),
m_iHealRateChange(0),
m_iHealth(0),
m_iAreaHealth(0),
m_iGlobalHealth(0),
m_iGlobalPopulationChange(0),
m_iFreeTechs(0),
m_eFreeSpecialTech(NO_TECH),
m_iDefenseModifier(0),
m_iBombardDefenseModifier(0),
m_iAllCityDefenseModifier(0),
m_iEspionageDefenseModifier(0),

m_iUnitUpgradePriceModifier(0),
m_iRevIdxLocal(0),
m_iRevIdxNational(0),
m_iRevIdxDistanceModifier(0),

m_iMissionType(NO_MISSION),
m_iVoteSourceType(NO_VOTESOURCE),
m_fVisibilityPriority(0.0f),
m_bTeamShare(false),
m_bWater(false),
m_bRiver(false),
m_bFreshWater(false),
m_bPower(false),
m_bOrbital(false),
m_bOrbitalInfrastructure(false),
m_bNoHolyCity(false),
m_bAreaBorderObstacle(false),
m_bForceTeamVoteEligible(false),
m_bCapital(false),
m_bGovernmentCenter(false),
m_bGoldenAge(false),
m_bMapCentering(false),
m_bNoUnhappiness(false),
m_bNoUnhealthyPopulation(false),
m_bBuildingOnlyHealthy(false),
m_bNeverCapture(false),
m_bNukeImmune(false),
m_bCenterInCity(false),
m_bStateReligionInCity(false),
m_bAllowsNukes(false),
m_piSeaPlotYieldChange(NULL),
m_piRiverPlotYieldChange(NULL),
m_piGlobalSeaPlotYieldChange(NULL),
m_piYieldChange(NULL),
m_piYieldPerPopChange(NULL),
m_piYieldModifier(NULL),
m_piPowerYieldModifier(NULL),
m_piAreaYieldModifier(NULL),
m_piGlobalYieldModifier(NULL),
m_piCommerceChange(NULL),
m_piCommercePerPopChange(NULL),
m_piCommerceChangeDoubleTime(NULL),
m_piCommerceModifier(NULL),
m_piGlobalCommerceModifier(NULL),
m_piSpecialistExtraCommerce(NULL),
m_piStateReligionCommerce(NULL),
m_piCommerceHappiness(NULL),
m_piReligionChange(NULL),
m_piSpecialistCount(NULL),
m_piFreeSpecialistCount(NULL),
m_piBonusHealthChanges(NULL),
m_piBonusHappinessChanges(NULL),
m_piBonusProductionModifier(NULL),
m_piUnitCombatFreeExperience(NULL),
m_piDomainFreeExperience(NULL),
m_piDomainProductionModifier(NULL),
m_piFlavorValue(NULL),
m_piImprovementFreeSpecialist(NULL),
m_pbCommerceFlexible(NULL),
m_pbCommerceChangeOriginalOwner(NULL),
m_ppaiSpecialistYieldChange(NULL),
m_ppaiSpecialistCommerceChange(NULL),
m_ppaiBonusYieldModifier(NULL)

//New Integers
,m_iGlobalPopulationgrowthratepercentage(0)
,m_iPopulationgrowthratepercentage(0)
,m_iPrereqVicinityBonus(NO_BONUS)
,m_iPrereqRawVicinityBonus(NO_BONUS)
,m_iPillageGoldModifier(0)
,m_iFreePromotion_2(NO_PROMOTION)
,m_iFreePromotion_3(NO_PROMOTION)
,m_iWorldTradeRoutes(0)
,m_iLineOfSight(0)
,m_iInflationModifier(0)
,m_iInvasionChance(0)
,m_iAdjacentDamagePercent(0)
,m_iProductionContinueBuilding(NO_BUILDING)
,m_iPrereqPopulation(0)
,m_iPrereqCultureLevel(NO_CULTURELEVEL)
,m_iWorkableRadius(0)
,m_iPrereqAnyoneBuilding(NO_BUILDING)
,m_iOccupationTimeModifier(0)
,m_iNoEntryDefenseLevel(0)
,m_iNumUnitFullHeal(0)
,m_iExtendsBuilding(NO_BUILDING)
,m_iObsoletesToBuilding(NO_BUILDING)
,m_iNumPopulationEmployed(0)
,m_iHealthPercentPerPopulation(0)
,m_iHappinessPercentPerPopulation(0)
//New Booleans
,m_bApplyFreePromotionOnMove(false)
,m_bPrereqPower(false)
,m_bNoEnemyPillagingIncome(false)
,m_bProvidesFreshWater(false)
,m_bForceAllTradeRoutes(false)
,m_bForceNoPrereqScaling(false)
,m_bPrereqWar(false)
,m_bRequiresActiveCivics(false)
,m_bZoneOfControl(false)
,m_bProtectedCulture(false)
//New Boolean Arrays
,m_pbPrereqOrCivics(NULL)
,m_pbPrereqAndCivics(NULL)
,m_pbPrereqOrTerrain(NULL)
,m_pbPrereqAndTerrain(NULL)
,m_pbPrereqOrImprovement(NULL)
,m_pbPrereqOrFeature(NULL)
//New Integer Arrays
,m_piBonusDefenseChanges(NULL)
,m_piCommerceAttacks(NULL)
//New Multidimensional Integer Arrays
,m_ppaiTechCommerceChange(NULL)
,m_ppaiTechYieldChange(NULL)
,m_ppaiTechSpecialistChange(NULL)
,m_ppaiTechCommerceModifier(NULL)
,m_ppaiTechYieldModifier(NULL)
,m_ppaiBonusCommerceModifier(NULL)
,m_ppaiBonusYieldChanges(NULL)
,m_ppaiVicinityBonusYieldChanges(NULL)
,m_ppaiBonusCommercePercentChanges(NULL)
,m_ppiImprovementYieldChanges(NULL)
,m_PropertyManipulators()
,m_pExprNewCityFree(NULL)
,m_pExprConstructCondition(NULL)
//TB Combat Mods (Buildings) begin
,m_iLinePriority(0)
,m_iOutbreakBase(0)
,m_iOvercomeBase(0)
,m_iTradeCommunicability(0)
#ifdef STRENGTH_IN_NUMBERS
,m_iFrontSupportPercentModifier(0)
,m_iShortRangeSupportPercentModifier(0)
,m_iMediumRangeSupportPercentModifier(0)
,m_iLongRangeSupportPercentModifier(0)
,m_iFlankSupportPercentModifier(0)
#endif
//TB Combat Mods (Buildings) end
,m_bAutoBuild(false)
,m_bEnablesOtherBuildingsCalculated(false)
,m_bEnablesOtherBuildingsValue(false)
,m_bEnablesUnitsCalculated(false)
,m_bEnablesUnits(false)
//Team Project (3)
//TB Building Tags
,m_iNationalCaptureProbabilityModifier(0)
,m_iNationalCaptureResistanceModifier(0)
,m_iLocalCaptureProbabilityModifier(0)
,m_iLocalCaptureResistanceModifier(0)
,m_iLocalDynamicDefense(0)
,m_iRiverDefensePenalty(0)
,m_iLocalRepel(0)
,m_iMinDefense(0)
,m_iBuildingDefenseRecoverySpeedModifier(0)
,m_iCityDefenseRecoverySpeedModifier(0)
,m_iDamageAttackerChance(0)
,m_iDamageToAttacker(0)
,m_iMaxPopulationAllowed(-1)
,m_iMaxPopulationChange(0)
,m_iInsidiousness(0)
,m_iInvestigation(0)
,m_iPopulationChange(0)
,m_iOneTimePopulationPercentLoss(0)
// booleans
,m_bDamageToAttackerIgnoresArmor(false)
,m_bDamageAllAttackers(false)
,m_bDamageAttackerCapable(false)
,m_bQuarantine(false)
,m_ppaiLocalSpecialistYieldChange(NULL)
,m_ppaiLocalSpecialistCommerceChange(NULL)
,m_pabHurry(NULL)
, m_iMaxPopAllowed(0)
//,m_pExprFreePromotionCondition(NULL)
,m_bNotShowInCity(false)
,m_ePropertySpawnUnit(NO_UNIT)
,m_ePropertySpawnProperty(NO_PROPERTY)
,m_ePromotionLineType(NO_PROMOTIONLINE)
{ }

//------------------------------------------------------------------------------------------------------
//
//  FUNCTION:   ~CvBuildingInfo()
//
//  PURPOSE :   Default destructor
//
//------------------------------------------------------------------------------------------------------
CvBuildingInfo::~CvBuildingInfo()
{
	SAFE_DELETE_ARRAY(m_piVictoryThreshold);
	SAFE_DELETE_ARRAY(m_piSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piRiverPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piGlobalSeaPlotYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldChange);
	SAFE_DELETE_ARRAY(m_piYieldPerPopChange);
	SAFE_DELETE_ARRAY(m_piYieldModifier);
	SAFE_DELETE_ARRAY(m_piPowerYieldModifier);
	SAFE_DELETE_ARRAY(m_piAreaYieldModifier);
	SAFE_DELETE_ARRAY(m_piGlobalYieldModifier);
	SAFE_DELETE_ARRAY(m_piCommerceChange);
	SAFE_DELETE_ARRAY(m_piCommercePerPopChange);
	SAFE_DELETE_ARRAY(m_piCommerceChangeDoubleTime);
	SAFE_DELETE_ARRAY(m_piCommerceModifier);
	SAFE_DELETE_ARRAY(m_piGlobalCommerceModifier);
	SAFE_DELETE_ARRAY(m_piSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_piStateReligionCommerce);
	SAFE_DELETE_ARRAY(m_piCommerceHappiness);
	SAFE_DELETE_ARRAY(m_piReligionChange);
	SAFE_DELETE_ARRAY(m_piSpecialistCount);
	SAFE_DELETE_ARRAY(m_piFreeSpecialistCount);
	SAFE_DELETE_ARRAY(m_piBonusHealthChanges);
	SAFE_DELETE_ARRAY(m_piBonusHappinessChanges);
	SAFE_DELETE_ARRAY(m_piBonusProductionModifier);
	SAFE_DELETE_ARRAY(m_piUnitCombatFreeExperience);
	SAFE_DELETE_ARRAY(m_piDomainFreeExperience);
	SAFE_DELETE_ARRAY(m_piDomainProductionModifier);
	SAFE_DELETE_ARRAY(m_piFlavorValue);
	SAFE_DELETE_ARRAY(m_piImprovementFreeSpecialist);
	SAFE_DELETE_ARRAY(m_pbCommerceFlexible);
	SAFE_DELETE_ARRAY(m_pbCommerceChangeOriginalOwner);
	SAFE_DELETE_ARRAY2(m_ppaiSpecialistYieldChange, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaiSpecialistCommerceChange, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaiLocalSpecialistYieldChange, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaiLocalSpecialistCommerceChange, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY2(m_ppaiBonusYieldModifier, GC.getNumSpecialistInfos());
	SAFE_DELETE_ARRAY(m_pbPrereqOrCivics);
	SAFE_DELETE_ARRAY(m_pbPrereqAndCivics);
	SAFE_DELETE_ARRAY(m_pbPrereqOrTerrain);
	SAFE_DELETE_ARRAY(m_pbPrereqAndTerrain);
	SAFE_DELETE_ARRAY(m_pbPrereqOrImprovement);
	SAFE_DELETE_ARRAY(m_pbPrereqOrFeature);
	SAFE_DELETE_ARRAY(m_piBonusDefenseChanges);
	SAFE_DELETE_ARRAY(m_piCommerceAttacks);
	SAFE_DELETE_ARRAY2(m_ppaiBonusCommerceModifier, GC.getNumBonusInfos());
	SAFE_DELETE_ARRAY2(m_ppaiBonusYieldChanges, GC.getNumBonusInfos());
	SAFE_DELETE_ARRAY2(m_ppaiBonusCommercePercentChanges, GC.getNumBonusInfos());
	SAFE_DELETE_ARRAY2(m_ppaiVicinityBonusYieldChanges, GC.getNumBonusInfos());
	SAFE_DELETE_ARRAY2(m_ppaiTechCommerceChange, GC.getNumTechInfos());
	SAFE_DELETE_ARRAY2(m_ppaiTechYieldChange, GC.getNumTechInfos());
	SAFE_DELETE_ARRAY2(m_ppaiTechSpecialistChange, GC.getNumTechInfos());
	SAFE_DELETE_ARRAY2(m_ppaiTechCommerceModifier, GC.getNumTechInfos());
	SAFE_DELETE_ARRAY2(m_ppaiTechYieldModifier, GC.getNumTechInfos());
	SAFE_DELETE_ARRAY2(m_ppiImprovementYieldChanges, GC.getNumImprovementInfos());
	SAFE_DELETE(m_pExprNewCityFree);
	SAFE_DELETE(m_pExprConstructCondition);

	//TB Building Tags
	SAFE_DELETE_ARRAY(m_pabHurry);
	//SAFE_DELETE(m_pExprFreePromotionCondition);

	for (int i = 0; i < (int)m_aEnabledCivilizationTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aEnabledCivilizationTypes[i]));
	}

	for (int i = 0; i < (int)m_aAidRateChanges.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aAidRateChanges[i]));
	}

	for (int i = 0; i < (int)m_aiFreeTraitTypes.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiFreeTraitTypes[i]));
	}

	for (int i = 0; i < (int)m_aiPrereqInCityBuildings.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_aiPrereqInCityBuildings[i]));
	}

	for (int i = 0; i < (int)m_vPrereqNotInCityBuildings.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_vPrereqNotInCityBuildings[i]));
	}

	for (int i=0; i<(int)m_vPrereqOrBuilding.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_vPrereqOrBuilding[i]));
	}

	for (int i=0; i<(int)m_vReplacementBuilding.size(); i++)
	{
		GC.removeDelayedResolution((int*)&(m_vReplacementBuilding[i]));
	}

	GC.removeDelayedResolution((int*)&m_iFreeBuilding);
	GC.removeDelayedResolution((int*)&m_iFreeAreaBuilding);
	GC.removeDelayedResolution((int*)&m_iProductionContinueBuilding);
	GC.removeDelayedResolution((int*)&m_iPrereqAnyoneBuilding);
	GC.removeDelayedResolution((int*)&m_iExtendsBuilding);
	GC.removeDelayedResolution((int*)&m_iObsoletesToBuilding);

	m_aBuildingHappinessChanges.removeDelayedResolution();
	m_aUnitProductionModifier.removeDelayedResolution();
	m_aBuildingProductionModifier.removeDelayedResolution();
	m_aGlobalBuildingProductionModifier.removeDelayedResolution();
	m_aPrereqNumOfBuilding.removeDelayedResolution();
	m_aGlobalBuildingCostModifier.removeDelayedResolution();
}

int CvBuildingInfo::getVictoryThreshold(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumVictoryInfos(), i)
	return m_piVictoryThreshold ? m_piVictoryThreshold[i] : 0;
}

BonusTypes CvBuildingInfo::getExtraFreeBonus(int i) const
{
	FASSERT_BOUNDS(0, (int)m_aExtraFreeBonuses.size(), i)
	return m_aExtraFreeBonuses[i].first;
}

int CvBuildingInfo::getExtraFreeBonusNum(int i) const
{
	FASSERT_BOUNDS(0, (int)m_aExtraFreeBonuses.size(), i)
	return m_aExtraFreeBonuses[i].second;
}

bool CvBuildingInfo::hasExtraFreeBonus(BonusTypes eBonus) const
{
	for(unsigned int i=0; i<m_aExtraFreeBonuses.size(); i++)
		if (m_aExtraFreeBonuses[i].first == eBonus)
			return true;
	return false;
}

int CvBuildingInfo::getYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piYieldChange ? m_piYieldChange[i] : 0;
}

int* CvBuildingInfo::getYieldChangeArray() const
{
	return m_piYieldChange;
}

int CvBuildingInfo::getYieldPerPopChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piYieldPerPopChange ? m_piYieldPerPopChange[i] : 0;
}

int* CvBuildingInfo::getYieldPerPopChangeArray() const
{
	return m_piYieldPerPopChange;
}

int CvBuildingInfo::getYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piYieldModifier ? m_piYieldModifier[i] : 0;
}

int* CvBuildingInfo::getYieldModifierArray() const
{
	return m_piYieldModifier;
}

int CvBuildingInfo::getPowerYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piPowerYieldModifier ? m_piPowerYieldModifier[i] : 0;
}

int* CvBuildingInfo::getPowerYieldModifierArray() const
{
	return m_piPowerYieldModifier;
}

int CvBuildingInfo::getAreaYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piAreaYieldModifier ? m_piAreaYieldModifier[i] : 0;
}

int* CvBuildingInfo::getAreaYieldModifierArray() const
{
	return m_piAreaYieldModifier;
}

int CvBuildingInfo::getGlobalYieldModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piGlobalYieldModifier ? m_piGlobalYieldModifier[i] : 0;
}

int* CvBuildingInfo::getGlobalYieldModifierArray() const
{
	return m_piGlobalYieldModifier;
}

int CvBuildingInfo::getSeaPlotYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piSeaPlotYieldChange ? m_piSeaPlotYieldChange[i] : 0;
}

int* CvBuildingInfo::getSeaPlotYieldChangeArray() const
{
	return m_piSeaPlotYieldChange;
}

int CvBuildingInfo::getRiverPlotYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piRiverPlotYieldChange ? m_piRiverPlotYieldChange[i] : 0;
}

int* CvBuildingInfo::getRiverPlotYieldChangeArray() const
{
	return m_piRiverPlotYieldChange;
}

int CvBuildingInfo::getGlobalSeaPlotYieldChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, i)
	return m_piGlobalSeaPlotYieldChange ? m_piGlobalSeaPlotYieldChange[i] : 0;
}

int* CvBuildingInfo::getGlobalSeaPlotYieldChangeArray() const
{
	return m_piGlobalSeaPlotYieldChange;
}

int CvBuildingInfo::getCommerceChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_piCommerceChange ? m_piCommerceChange[i] : 0;
}

int* CvBuildingInfo::getCommerceChangeArray() const
{
	return m_piCommerceChange;
}

int CvBuildingInfo::getCommercePerPopChange(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_piCommercePerPopChange ? m_piCommercePerPopChange[i] : 0;
}

int* CvBuildingInfo::getCommercePerPopChangeArray() const
{
	return m_piCommercePerPopChange;
}

int CvBuildingInfo::getCommerceChangeDoubleTime(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_piCommerceChangeDoubleTime ? m_piCommerceChangeDoubleTime[i] : 0;
}

int CvBuildingInfo::getCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_piCommerceModifier ? m_piCommerceModifier[i] : 0;
}

int* CvBuildingInfo::getCommerceModifierArray() const
{
	return m_piCommerceModifier;
}

int CvBuildingInfo::getGlobalCommerceModifier(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_piGlobalCommerceModifier ? m_piGlobalCommerceModifier[i] : 0;
}

int* CvBuildingInfo::getGlobalCommerceModifierArray() const
{
	return m_piGlobalCommerceModifier;
}

int CvBuildingInfo::getSpecialistExtraCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_piSpecialistExtraCommerce ? m_piSpecialistExtraCommerce[i] : 0;
}

int* CvBuildingInfo::getSpecialistExtraCommerceArray() const
{
	return m_piSpecialistExtraCommerce;
}

int CvBuildingInfo::getStateReligionCommerce(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_piStateReligionCommerce ? m_piStateReligionCommerce[i] : 0;
}

int* CvBuildingInfo::getStateReligionCommerceArray() const
{
	return m_piStateReligionCommerce;
}

int CvBuildingInfo::getCommerceHappiness(int i) const
{
	FASSERT_BOUNDS(NO_COMMERCE, NUM_COMMERCE_TYPES, i)

	if (i == NO_COMMERCE)
	{
		return m_piCommerceHappiness ? 1 : 0;
	}
	else
	{
		return m_piCommerceHappiness ? m_piCommerceHappiness[i] : 0;
	}
}

int CvBuildingInfo::getReligionChange(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumReligionInfos(), i)
	return m_piReligionChange ? m_piReligionChange[i] : 0;
}

int CvBuildingInfo::getSpecialistCount(int i) const
{
	FASSERT_BOUNDS(NO_SPECIALIST, GC.getNumSpecialistInfos(), i)

	if (i == NO_SPECIALIST)
	{
		return m_piSpecialistCount ? 1 : 0;
	}
	return m_piSpecialistCount ? m_piSpecialistCount[i] : 0;
}

int CvBuildingInfo::getFreeSpecialistCount(int i) const
{
	FASSERT_BOUNDS(NO_SPECIALIST, GC.getNumSpecialistInfos(), i)

	if (i == NO_SPECIALIST)
	{
		return m_piFreeSpecialistCount ? 1 : 0;
	}
	return m_piFreeSpecialistCount ? m_piFreeSpecialistCount[i] : 0;
}

int CvBuildingInfo::getBonusHealthChanges(int i) const
{
	FASSERT_BOUNDS(NO_BONUS, GC.getNumBonusInfos(), i)

	if (i == NO_BONUS)
	{
		return m_piBonusHealthChanges ? 1 : 0;
	}
	return m_piBonusHealthChanges ? m_piBonusHealthChanges[i] : 0;
}

int CvBuildingInfo::getBonusHappinessChanges(int i) const
{
	FASSERT_BOUNDS(NO_BONUS, GC.getNumBonusInfos(), i)

	if (i == NO_BONUS)
	{
		return m_piBonusHappinessChanges ? 1 : 0;
	}
	return m_piBonusHappinessChanges ? m_piBonusHappinessChanges[i] : 0;
}

int CvBuildingInfo::getBonusProductionModifier(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	return m_piBonusProductionModifier ? m_piBonusProductionModifier[i] : 0;
}

int CvBuildingInfo::getUnitCombatFreeExperience(int i) const
{
	FASSERT_BOUNDS(NO_UNIT, GC.getNumUnitCombatInfos(), i)

	if (i == NO_UNIT)
	{
		return m_piUnitCombatFreeExperience ? 1 : 0;
	}
	return m_piUnitCombatFreeExperience ? m_piUnitCombatFreeExperience[i] : 0;
}

int CvBuildingInfo::getDomainFreeExperience(int i) const
{
	FASSERT_BOUNDS(NO_DOMAIN, NUM_DOMAIN_TYPES, i)

	if (i == NO_DOMAIN)
	{
		return m_piDomainFreeExperience ? 1 : 0;
	}
	return m_piDomainFreeExperience ? m_piDomainFreeExperience[i] : 0;
}

int CvBuildingInfo::getDomainProductionModifier(int i) const
{
	FASSERT_BOUNDS(NO_DOMAIN, NUM_DOMAIN_TYPES, i)

	if (i == NO_DOMAIN)
	{
		return m_piDomainProductionModifier ? 1 : 0;
	}
	return m_piDomainProductionModifier ? m_piDomainProductionModifier[i] : 0;
}

const std::vector<TechTypes>& CvBuildingInfo::getPrereqAndTechs() const
{
	return m_piPrereqAndTechs;
}

const python::list CvBuildingInfo::cyGetPrereqAndTechs() const
{
	return Cy::makeList(m_piPrereqAndTechs);
}

int CvBuildingInfo::getPrereqNumOfBuilding(BuildingTypes e) const
{
	return m_aPrereqNumOfBuilding.getValue(e);
}

int CvBuildingInfo::getFlavorValue(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFlavorTypes(), i)
	return m_piFlavorValue ? m_piFlavorValue[i] : 0;
}

int CvBuildingInfo::getImprovementFreeSpecialist(int i) const
{
	FASSERT_BOUNDS(NO_IMPROVEMENT, GC.getNumImprovementInfos(), i)

	if (i == NO_IMPROVEMENT)
	{
		return m_piImprovementFreeSpecialist ? 1 : 0;
	}
	return m_piImprovementFreeSpecialist ? m_piImprovementFreeSpecialist[i] : 0;
}

bool CvBuildingInfo::isCommerceFlexible(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_pbCommerceFlexible ? m_pbCommerceFlexible[i] : false;
}

bool CvBuildingInfo::isCommerceChangeOriginalOwner(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_pbCommerceChangeOriginalOwner ? m_pbCommerceChangeOriginalOwner[i] : false;
}


int CvBuildingInfo::getPrereqInCityBuilding(const int i) const
{
	return m_aiPrereqInCityBuildings[i];
}

short CvBuildingInfo::getNumPrereqInCityBuildings() const
{
	return m_aiPrereqInCityBuildings.size();
}

bool CvBuildingInfo::isPrereqInCityBuilding(const int i) const
{
	return find(m_aiPrereqInCityBuildings.begin(), m_aiPrereqInCityBuildings.end(), i) != m_aiPrereqInCityBuildings.end();
}


int CvBuildingInfo::getPrereqNotInCityBuilding(const int i) const
{
	return m_vPrereqNotInCityBuildings[i];
}

short CvBuildingInfo::getNumPrereqNotInCityBuildings() const
{
	return m_vPrereqNotInCityBuildings.size();
}


int CvBuildingInfo::getSpecialistYieldChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i)
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j)

	return (m_ppaiSpecialistYieldChange && m_ppaiSpecialistYieldChange[i]) ? m_ppaiSpecialistYieldChange[i][j] : 0;
}

int* CvBuildingInfo::getSpecialistYieldChangeArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i)
	return m_ppaiSpecialistYieldChange[i];
}

int CvBuildingInfo::getSpecialistCommerceChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i)
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j)

	return (m_ppaiSpecialistCommerceChange && m_ppaiSpecialistCommerceChange[i]) ? m_ppaiSpecialistCommerceChange[i][j] : 0;
}

int* CvBuildingInfo::getSpecialistCommerceChangeArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i)
	return m_ppaiSpecialistCommerceChange[i];
}

//Team Project (1)
int CvBuildingInfo::getLocalSpecialistYieldChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i)
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j)

	return (m_ppaiLocalSpecialistYieldChange && m_ppaiLocalSpecialistYieldChange[i]) ? m_ppaiLocalSpecialistYieldChange[i][j] : 0;
}

int* CvBuildingInfo::getLocalSpecialistYieldChangeArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i)
	return m_ppaiLocalSpecialistYieldChange[i];
}

int CvBuildingInfo::getLocalSpecialistCommerceChange(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i)
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j)

	return (m_ppaiLocalSpecialistCommerceChange && m_ppaiLocalSpecialistCommerceChange[i]) ? m_ppaiLocalSpecialistCommerceChange[i][j] : 0;
}

int* CvBuildingInfo::getLocalSpecialistCommerceChangeArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), i)
	return m_ppaiLocalSpecialistCommerceChange[i];
}

int CvBuildingInfo::getBonusYieldModifier(int i, int j) const
{
	FASSERT_BOUNDS(NO_BONUS, GC.getNumBonusInfos(), i)

	if (i == NO_BONUS)
	{
		return m_ppaiBonusYieldModifier ? 1 : 0;
	}
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j)
	return (m_ppaiBonusYieldModifier && m_ppaiBonusYieldModifier[i]) ? m_ppaiBonusYieldModifier[i][j] : 0;
}

int* CvBuildingInfo::getBonusYieldModifierArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	return m_ppaiBonusYieldModifier == NULL ? NULL : m_ppaiBonusYieldModifier[i];
}

int CvBuildingInfo::getGlobalBuildingCommerceChange(int iBuilding, int iCommerce) const
{
	foreach_(const BuildingCommerceChange& pChange, m_aGlobalBuildingCommerceChanges)
	{
		if (pChange.eBuilding == (BuildingTypes)iBuilding && pChange.eCommerce == (CommerceTypes)iCommerce)
		{
			return pChange.iChange;
		}
	}

	return 0;
}

const std::vector<BuildingCommerceChange>& CvBuildingInfo::getGlobalBuildingCommerceChanges() const
{
	return m_aGlobalBuildingCommerceChanges;
}

const python::list CvBuildingInfo::cyGetGlobalBuildingCommerceChanges() const
{
	python::list pyList = python::list();
	foreach_(const BuildingCommerceChange& pChange, m_aGlobalBuildingCommerceChanges)
		pyList.append(pChange);
	return pyList;
}

const TCHAR* CvBuildingInfo::getButton() const
{
	const CvString cDefault = CvString::format("").GetCString();
	if (getArtDefineTag() == cDefault)	// MRGENIE: Catch non-existing tag
	{
		return NULL;
	}
	const CvArtInfoBuilding* pBuildingArtInfo = getArtInfo();
	return pBuildingArtInfo ? pBuildingArtInfo->getButton() : NULL;
}

const CvArtInfoBuilding* CvBuildingInfo::getArtInfo() const
{
	return ARTFILEMGR.getBuildingArtInfo(getArtDefineTag());
}

const CvArtInfoMovie* CvBuildingInfo::getMovieInfo() const
{
	const TCHAR* pcTag = getMovieDefineTag();
	if (NULL != pcTag && 0 != _tcscmp(pcTag, "") && 0 != _tcscmp(pcTag, "NONE"))
	{
		return ARTFILEMGR.getMovieArtInfo(pcTag);
	}
	else
	{
		return NULL;
	}
}

const TCHAR* CvBuildingInfo::getMovie() const
{
	const CvArtInfoMovie* pArt = getMovieInfo();
	return pArt ? pArt->getPath() : NULL;
}

int CvBuildingInfo::getNoEntryDefenseLevel() const
{
	if (!GC.getGame().isOption(GAMEOPTION_REALISTIC_SIEGE))
	{
		return 0;
	}
	return m_iNoEntryDefenseLevel;
}


bool CvBuildingInfo::isPrereqOrCivics(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), i)
	return m_pbPrereqOrCivics ? m_pbPrereqOrCivics[i] : false;
}

bool CvBuildingInfo::isPrereqAndCivics(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumCivicInfos(), i)
	return m_pbPrereqAndCivics ? m_pbPrereqAndCivics[i] : false;
}

//This is for the readpass3
int CvBuildingInfo::isPrereqOrCivicsVectorSize() const						{return m_aszPrereqOrCivicsforPass3.size();}
CvString CvBuildingInfo::isPrereqOrCivicsNamesVectorElement(int i) const	{return m_aszPrereqOrCivicsforPass3[i];}
int CvBuildingInfo::isPrereqOrCivicsValuesVectorElement(int i) const		{return m_abPrereqOrCivicsforPass3[i];}
int CvBuildingInfo::isPrereqAndCivicsVectorSize() const						{return m_aszPrereqAndCivicsforPass3.size();}
CvString CvBuildingInfo::isPrereqAndCivicsNamesVectorElement(int i) const	{return m_aszPrereqAndCivicsforPass3[i];}
int CvBuildingInfo::isPrereqAndCivicsValuesVectorElement(int i) const		{return m_abPrereqAndCivicsforPass3[i];}

bool CvBuildingInfo::isPrereqOrTerrain(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i)
	return m_pbPrereqOrTerrain ? m_pbPrereqOrTerrain[i] : false;
}

bool CvBuildingInfo::isPrereqAndTerrain(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumTerrainInfos(), i)
	return m_pbPrereqAndTerrain ? m_pbPrereqAndTerrain[i] : false;
}

bool CvBuildingInfo::isPrereqOrImprovement(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i)
	return m_pbPrereqOrImprovement ? m_pbPrereqOrImprovement[i] : false;
}

bool CvBuildingInfo::isPrereqOrFeature(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumFeatureInfos(), i)
	return m_pbPrereqOrFeature ? m_pbPrereqOrFeature[i] : false;
}

int CvBuildingInfo::getBonusDefenseChanges(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	return m_piBonusDefenseChanges ? m_piBonusDefenseChanges[i] : 0;
}

int CvBuildingInfo::getCommerceAttacks(int i) const
{
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, i)
	return m_piCommerceAttacks ? m_piCommerceAttacks[i] : 0;
}

int* CvBuildingInfo::getCommerceAttacksArray() const
{
	return m_piCommerceAttacks;
}

int CvBuildingInfo::getBonusCommerceModifier(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j)
	return (m_ppaiBonusCommerceModifier && m_ppaiBonusCommerceModifier[i]) ? m_ppaiBonusCommerceModifier[i][j] : 0;
}

int* CvBuildingInfo::getBonusCommerceModifierArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	return m_ppaiBonusCommerceModifier[i];
}

int CvBuildingInfo::getBonusYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(NO_BONUS, GC.getNumBonusInfos(), i)

	if (i == NO_BONUS)
	{
		return (m_ppaiBonusYieldChanges ? 1 : 0);
	}
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j)
	return (m_ppaiBonusYieldChanges && m_ppaiBonusYieldChanges[i]) ? m_ppaiBonusYieldChanges[i][j] : 0;
}

int* CvBuildingInfo::getBonusYieldChangesArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	return m_ppaiBonusYieldChanges[i];
}

int CvBuildingInfo::getBonusCommercePercentChanges(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j)
	return (m_ppaiBonusCommercePercentChanges && m_ppaiBonusCommercePercentChanges[i]) ? m_ppaiBonusCommercePercentChanges[i][j] : 0;
}

int* CvBuildingInfo::getBonusCommercePercentChangesArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	return m_ppaiBonusCommercePercentChanges[i];
}

int CvBuildingInfo::getVicinityBonusYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(NO_BONUS, GC.getNumBonusInfos(), i)

	if (i == NO_BONUS)
	{
		return m_ppaiVicinityBonusYieldChanges ? 1 : 0;
	}
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j)
	return (m_ppaiVicinityBonusYieldChanges && m_ppaiVicinityBonusYieldChanges[i]) ? m_ppaiVicinityBonusYieldChanges[i][j] : 0;
}

int* CvBuildingInfo::getVicinityBonusYieldChangesArray(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), i)
	return m_ppaiVicinityBonusYieldChanges[i];
}

int CvBuildingInfo::getTechCommerceChange(int i, int j) const
{
	FASSERT_BOUNDS(NO_TECH, GC.getNumTechInfos(), i)

	if (i == NO_TECH)
	{
		return m_ppaiTechCommerceChange ? 1 : 0;
	}
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j)
	return (m_ppaiTechCommerceChange && m_ppaiTechCommerceChange[i]) ? m_ppaiTechCommerceChange[i][j] : 0;
}

int* CvBuildingInfo::getTechCommerceChangeArray(int i) const
{
	return m_ppaiTechCommerceChange[i];
}

int CvBuildingInfo::getTechYieldChange(int i, int j) const
{
	FASSERT_BOUNDS(NO_TECH, GC.getNumTechInfos(), i)

	if (i == NO_TECH)
	{
		return (m_ppaiTechYieldChange ? 1 : 0);
	}
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j)
	return (m_ppaiTechYieldChange && m_ppaiTechYieldChange[i]) ? m_ppaiTechYieldChange[i][j] : 0;
}

int* CvBuildingInfo::getTechYieldChangeArray(int i) const
{
	return (m_ppaiTechYieldChange ? m_ppaiTechYieldChange[i] : NULL);
}

int CvBuildingInfo::getTechSpecialistChange(int i, int j) const
{
	FASSERT_BOUNDS(NO_TECH, GC.getNumTechInfos(), i)

	if (i == NO_TECH)
	{
		return (m_ppaiTechSpecialistChange ? 1 : 0);
	}
	FASSERT_BOUNDS(0, GC.getNumSpecialistInfos(), j)
	return (m_ppaiTechSpecialistChange && m_ppaiTechSpecialistChange[i]) ? m_ppaiTechSpecialistChange[i][j] : 0;
}

int* CvBuildingInfo::getTechSpecialistChangeArray(int i) const
{
	return m_ppaiTechSpecialistChange[i];
}

int CvBuildingInfo::getTechCommerceModifier(int i, int j) const
{
	FASSERT_BOUNDS(NO_TECH, GC.getNumTechInfos(), i)

	if (i == NO_TECH)
	{
		return (m_ppaiTechCommerceModifier ? 1 : 0);
	}
	FASSERT_BOUNDS(0, NUM_COMMERCE_TYPES, j)
	return (m_ppaiTechCommerceModifier && m_ppaiTechCommerceModifier[i]) ? m_ppaiTechCommerceModifier[i][j] : 0;
}

int* CvBuildingInfo::getTechCommerceModifierArray(int i) const
{
	return m_ppaiTechCommerceModifier[i];
}

int CvBuildingInfo::getTechYieldModifier(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumTechInfos(), i)
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j)
	return (m_ppaiTechYieldModifier && m_ppaiTechYieldModifier[i]) ? m_ppaiTechYieldModifier[i][j] : 0;
}

int* CvBuildingInfo::getTechYieldModifierArray(int i) const
{
	return m_ppaiTechYieldModifier[i];
}

int CvBuildingInfo::getImprovementYieldChanges(int i, int j) const
{
	FASSERT_BOUNDS(0, GC.getNumImprovementInfos(), i)
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, j)
	return (m_ppiImprovementYieldChanges && m_ppiImprovementYieldChanges[i]) ? m_ppiImprovementYieldChanges[i][j] : 0;
}
/*
int CvBuildingInfo::getUnitCombatExtraStrength(int i) const
{
	FASSERT_BOUNDS(NO_UNITCOMBAT, GC.getNumUnitCombatInfos(), i)

	if (i == NO_UNITCOMBAT)
	{
		return m_piUnitCombatExtraStrength ? 1 : 0;
	}
	return m_piUnitCombatExtraStrength ? m_piUnitCombatExtraStrength[i] : 0;
}
*/
//TB Combat Mods (Buildings) begin

UnitTypes CvBuildingInfo::getPropertySpawnUnit() const
{
	return m_ePropertySpawnUnit;
}

PropertyTypes CvBuildingInfo::getPropertySpawnProperty() const
{
	return m_ePropertySpawnProperty;
}

PromotionLineTypes CvBuildingInfo::getPromotionLineType() const
{
	return m_ePromotionLineType;
}
//integers
int CvBuildingInfo::getLinePriority() const
{
	return m_iLinePriority;
}

int CvBuildingInfo::getOutbreakBase() const
{
	return m_iOutbreakBase;
}

int CvBuildingInfo::getOvercomeBase() const
{
	return m_iOvercomeBase;
}

int CvBuildingInfo::getTradeCommunicability() const
{
	return m_iTradeCommunicability;
}

int CvBuildingInfo::getNationalCaptureProbabilityModifier() const
{
	return m_iNationalCaptureProbabilityModifier;
}

int CvBuildingInfo::getNationalCaptureResistanceModifier() const
{
	return m_iNationalCaptureResistanceModifier;
}

int CvBuildingInfo::getLocalCaptureProbabilityModifier() const
{
	return m_iLocalCaptureProbabilityModifier;
}

int CvBuildingInfo::getLocalCaptureResistanceModifier() const
{
	return m_iLocalCaptureResistanceModifier;
}

int CvBuildingInfo::getLocalDynamicDefense() const
{
	return m_iLocalDynamicDefense;
}

int CvBuildingInfo::getRiverDefensePenalty() const
{
	return m_iRiverDefensePenalty;
}

int CvBuildingInfo::getLocalRepel() const
{
	if (!GC.getGame().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	return m_iLocalRepel;
}

int CvBuildingInfo::getMinDefense() const
{
	if (!GC.getGame().isOption(GAMEOPTION_REALISTIC_SIEGE))
	{
		return 0;
	}
	return m_iMinDefense;
}

int CvBuildingInfo::getBuildingDefenseRecoverySpeedModifier() const
{
	return m_iBuildingDefenseRecoverySpeedModifier;
}

int CvBuildingInfo::getCityDefenseRecoverySpeedModifier() const
{
	return m_iCityDefenseRecoverySpeedModifier;
}

int CvBuildingInfo::getDamageAttackerChance() const
{
	return m_iDamageAttackerChance;
}

int CvBuildingInfo::getDamageToAttacker() const
{
	return m_iDamageToAttacker;
}

int CvBuildingInfo::getMaxPopulationAllowed() const
{
	if (!GC.getGame().isOption(GAMEOPTION_MAXIMUM_POPULATION))
	{
		return -1;
	}
	return m_iMaxPopulationAllowed;
}

int CvBuildingInfo::getMaxPopulationChange() const
{
	if (!GC.getGame().isOption(GAMEOPTION_MAXIMUM_POPULATION))
	{
		return 0;
	}
	return m_iMaxPopulationChange;
}

int CvBuildingInfo::getInsidiousness() const
{
	return m_iInsidiousness;
}

int CvBuildingInfo::getInvestigation() const
{
	return m_iInvestigation;
}

int CvBuildingInfo::getPopulationChange() const
{
	return m_iPopulationChange;
}

int CvBuildingInfo::getOneTimePopulationPercentLoss() const
{
	return m_iOneTimePopulationPercentLoss;
}

//Booleans
bool CvBuildingInfo::isDamageToAttackerIgnoresArmor() const
{
	return m_bDamageToAttackerIgnoresArmor;
}

bool CvBuildingInfo::isDamageAllAttackers() const
{
	return m_bDamageAllAttackers;
}

bool CvBuildingInfo::isDamageAttackerCapable() const
{
	return m_bDamageAttackerCapable;
}

bool CvBuildingInfo::isQuarantine() const
{
	return m_bQuarantine;
}

int CvBuildingInfo::getNumHealUnitCombatTypes() const
{
	return (int)m_aHealUnitCombatTypes.size();
}

const HealUnitCombat& CvBuildingInfo::getHealUnitCombatType(int iUnitCombat) const
{
	FASSERT_BOUNDS(0, getNumHealUnitCombatTypes(), iUnitCombat)
	return m_aHealUnitCombatTypes[iUnitCombat];
}

int CvBuildingInfo::getNumBonusAidModifiers() const
{
	return (int)m_aBonusAidModifiers.size();
}

const BonusAidModifiers& CvBuildingInfo::getBonusAidModifier(int iIndex) const
{
	return m_aBonusAidModifiers[iIndex];
}

int CvBuildingInfo::getNumAidRateChanges() const
{
	return (int)m_aAidRateChanges.size();
}

const AidRateChanges& CvBuildingInfo::getAidRateChange(int iIndex) const
{
	return m_aAidRateChanges[iIndex];
}

int CvBuildingInfo::getNumEnabledCivilizationTypes() const
{
	return (int)m_aEnabledCivilizationTypes.size();
}

const EnabledCivilizations& CvBuildingInfo::getEnabledCivilizationType(int iIndex) const
{
	return m_aEnabledCivilizationTypes[iIndex];
}


int CvBuildingInfo::getUnitCombatRetrainType(int i) const
{
	return m_aiUnitCombatRetrainTypes[i];
}

int CvBuildingInfo::getNumUnitCombatRetrainTypes() const
{
	return (int)m_aiUnitCombatRetrainTypes.size();
}

bool CvBuildingInfo::isUnitCombatRetrainType(int i) const
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos()); // do not include this line if for delayed resolution
	if (find(m_aiUnitCombatRetrainTypes.begin(), m_aiUnitCombatRetrainTypes.end(), i) == m_aiUnitCombatRetrainTypes.end())
	{
		return false;
	}
	return true;
}

int CvBuildingInfo::getMayDamageAttackingUnitCombatType(int i) const
{
	return m_aiMayDamageAttackingUnitCombatTypes[i];
}

int CvBuildingInfo::getNumMayDamageAttackingUnitCombatTypes() const
{
	return (int)m_aiMayDamageAttackingUnitCombatTypes.size();
}

bool CvBuildingInfo::isMayDamageAttackingUnitCombatType(int i) const
{
	FAssert (i > -1 && i < GC.getNumUnitCombatInfos()); // do not include this line if for delayed resolution
	if (find(m_aiMayDamageAttackingUnitCombatTypes.begin(), m_aiMayDamageAttackingUnitCombatTypes.end(), i) == m_aiMayDamageAttackingUnitCombatTypes.end())
	{
		return false;
	}
	return true;
}

int CvBuildingInfo::getNumUnitCombatRepelModifiers() const
{
	return m_aUnitCombatRepelModifiers.size();
}

int CvBuildingInfo::getUnitCombatRepelModifier(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatRepelModifiers.begin(); it != m_aUnitCombatRepelModifiers.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}

	return 0;
}

int CvBuildingInfo::getNumUnitCombatRepelAgainstModifiers() const
{
	return m_aUnitCombatRepelAgainstModifiers.size();
}

int CvBuildingInfo::getUnitCombatRepelAgainstModifier(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_HEART_OF_WAR))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatRepelAgainstModifiers.begin(); it != m_aUnitCombatRepelAgainstModifiers.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}

	return 0;
}

int CvBuildingInfo::getNumUnitCombatDefenseAgainstModifiers() const
{
	return m_aUnitCombatDefenseAgainstModifiers.size();
}

int CvBuildingInfo::getUnitCombatDefenseAgainstModifier(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatDefenseAgainstModifiers.begin(); it != m_aUnitCombatDefenseAgainstModifiers.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}

	return 0;
}

int CvBuildingInfo::getNumUnitCombatProdModifiers() const
{
	return m_aUnitCombatProdModifiers.size();
}

int CvBuildingInfo::getUnitCombatProdModifier(int iUnitCombat) const
{
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatProdModifiers.begin(); it != m_aUnitCombatProdModifiers.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}

	return 0;
}

int CvBuildingInfo::getNumUnitCombatOngoingTrainingDurations() const
{
	return m_aUnitCombatOngoingTrainingDurations.size();
}

int CvBuildingInfo::getUnitCombatOngoingTrainingDuration(int iUnitCombat, bool bForLoad) const
{
	if (!bForLoad && !GC.getGame().isOption(GAMEOPTION_ONGOING_TRAINING))
	{
		return 0;
	}
	for (UnitCombatModifierArray::const_iterator it = m_aUnitCombatOngoingTrainingDurations.begin(); it != m_aUnitCombatOngoingTrainingDurations.end(); ++it)
	{
		if ((*it).first == (UnitCombatTypes)iUnitCombat)
		{
			return (*it).second;
		}
	}

	return 0;
}

int CvBuildingInfo::getNumAfflictionOutbreakLevelChanges() const
{
	return m_aAfflictionOutbreakLevelChanges.size();
}

int CvBuildingInfo::getAfflictionOutbreakLevelChange(int iAfflictionLine) const
{
	for (PromotionLineModifierArray::const_iterator it = m_aAfflictionOutbreakLevelChanges.begin(); it != m_aAfflictionOutbreakLevelChanges.end(); ++it)
	{
		if ((*it).first == (PromotionLineTypes)iAfflictionLine)
		{
			return (*it).second;
		}
	}

	return 0;
}

int CvBuildingInfo::getNumTechOutbreakLevelChanges() const
{
	return m_aTechOutbreakLevelChanges.size();
}

int CvBuildingInfo::getTechOutbreakLevelChange(int iTech) const
{
	for (TechModifierArray::const_iterator it = m_aTechOutbreakLevelChanges.begin(); it != m_aTechOutbreakLevelChanges.end(); ++it)
	{
		if ((*it).first == (TechTypes)iTech)
		{
			return (*it).second;
		}
	}
	return 0;
}

int CvBuildingInfo::getTechHappiness(TechTypes eTech) const
{
	return m_aTechHappinessChanges.getValue(eTech);
}

int CvBuildingInfo::getTechHealth(TechTypes eTech) const
{
	return m_aTechHealthChanges.getValue(eTech);
}

bool CvBuildingInfo::isHurry(int i) const
{
	FASSERT_BOUNDS(0, GC.getNumHurryInfos(), i)
	return m_pabHurry ? m_pabHurry[i] : false;
}
//TB Combat Mods (Buildings) end


int CvBuildingInfo::getMaxPopAllowed() const
{
	return m_iMaxPopAllowed;
}


bool CvBuildingInfo::EnablesOtherBuildings() const
{
	if (!m_bEnablesOtherBuildingsCalculated)
	{
		const int iId = GC.getInfoTypeForString(getType());

		m_bEnablesOtherBuildingsCalculated = true;
		m_bEnablesOtherBuildingsValue = false;

		// add the building and its bonuses to the query to see if they influence the construct condition of a building
		std::vector<GOMQuery> queries;
		GOMQuery query;
		query.GOM = GOM_BUILDING;
		query.id = iId;
		queries.push_back(query);
		query.GOM = GOM_BONUS;
		query.id = getFreeBonus();
		if (query.id != NO_BONUS)
		{
			queries.push_back(query);
		}
		for (int iJ = 0; iJ < getNumExtraFreeBonuses(); iJ++)
		{
			query.id = getExtraFreeBonus(iJ);
			queries.push_back(query);
		}

		for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
		{
			if(GC.getBuildingInfo((BuildingTypes)iJ).isPrereqInCityBuilding(iId)
			|| GC.getBuildingInfo((BuildingTypes)iJ).isPrereqOrBuilding(iId))
			{
				m_bEnablesOtherBuildingsValue = true;
				break;
			}
		}

		if ( !m_bEnablesOtherBuildingsValue )
		{
			const BonusTypes eFreeBonus = (BonusTypes)getFreeBonus();

			if ( eFreeBonus != NO_BONUS )
			{
				for (int iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
				{
					if (GC.getBuildingInfo((BuildingTypes)iJ).getPrereqAndBonus() == eFreeBonus
					|| algo::contains(GC.getBuildingInfo((BuildingTypes)iJ).getPrereqOrBonuses(), eFreeBonus))
					{
						m_bEnablesOtherBuildingsValue = true;
						break;
					}
				}
			}
		}
	}

	return m_bEnablesOtherBuildingsValue;
}

bool CvBuildingInfo::isFreeBonusOfBuilding(BonusTypes eBonus) const
{
	if ( getFreeBonus() == eBonus )
	{
		return true;
	}

	for( int iJ = 0; iJ < getNumExtraFreeBonuses(); iJ++ )
	{
		if ( getExtraFreeBonus(iJ) == eBonus )
		{
			return true;
		}
	}

	return false;
}

bool CvBuildingInfo::EnablesUnits() const
{
	if ( !m_bEnablesUnitsCalculated )
	{
		const int eBuilding = GC.getInfoTypeForString(m_szType);

		m_bEnablesUnitsCalculated = true;
		m_bEnablesUnits = false;

		// add the building and its bonuses to the query to see if they influence the construct condition of a building
		std::vector<GOMQuery> queries;
		GOMQuery query;
		query.GOM = GOM_BUILDING;
		query.id = eBuilding;
		queries.push_back(query);
		query.GOM = GOM_BONUS;
		query.id = getFreeBonus();
		if (query.id != NO_BONUS)
		{
			queries.push_back(query);
		}
		for (int iJ = 0; iJ < getNumExtraFreeBonuses(); iJ++)
		{
			query.id = getExtraFreeBonus(iJ);
			queries.push_back(query);
		}

		for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
		{
			const CvUnitInfo& kUnit = GC.getUnitInfo((UnitTypes)iI);

			const BoolExpr* condition = kUnit.getTrainCondition();
			if (condition != NULL)
			{
				if (condition->getInvolvesGOM(queries))
				{
					m_bEnablesUnits = true;
					return m_bEnablesUnits;
				}
			}

			if (kUnit.isPrereqAndBuilding(eBuilding))
			{
				m_bEnablesUnits = true;
				return m_bEnablesUnits;
			}

			for (int iK = 0; iK < kUnit.getPrereqOrBuildingsNum(); iK++)
			{
				if (kUnit.getPrereqOrBuilding((BuildingTypes)iK) == eBuilding)
				{
					m_bEnablesUnits = true;
					return m_bEnablesUnits;
				}
			}

			if ( kUnit.getPrereqAndBonus() != NO_BONUS )
			{
				if ( isFreeBonusOfBuilding( (BonusTypes)kUnit.getPrereqAndBonus()) )
				{
					m_bEnablesUnits = true;
					return m_bEnablesUnits;
				}
			}

			foreach_(const BonusTypes eXtraFreeBonus, kUnit.getPrereqOrBonuses())
			{
				if (isFreeBonusOfBuilding(eXtraFreeBonus))
				{
					m_bEnablesUnits = true;
					return m_bEnablesUnits;
				}
			}
		}
	}

	return m_bEnablesUnits;
}

void CvBuildingInfo::getCheckSum(uint32_t& iSum) const
{
	CheckSum(iSum, m_bNoLimit);
	CheckSum(iSum, m_iVictoryPrereq);
	CheckSum(iSum, m_iFreeStartEra);
	CheckSum(iSum, m_iMaxStartEra);
	CheckSum(iSum, m_iObsoleteTech);
	CheckSum(iSum, m_iPrereqAndTech);
	CheckSum(iSum, m_iNoBonus);
	CheckSum(iSum, m_iPowerBonus);
	CheckSum(iSum, m_iFreeBonus);
	CheckSum(iSum, m_iNumFreeBonuses);
	CheckSum(iSum, m_iFreeBuilding);
	CheckSum(iSum, m_iFreeAreaBuilding);
	CheckSum(iSum, m_iFreePromotion);
	CheckSum(iSum, m_iCivicOption);
	CheckSum(iSum, m_iAIWeight);
	CheckSum(iSum, m_iProductionCost);
	CheckSum(iSum, m_iHurryCostModifier);
	CheckSum(iSum, m_iHurryAngerModifier);
	CheckSum(iSum, m_iMinAreaSize);
	CheckSum(iSum, m_iNumCitiesPrereq);
	CheckSum(iSum, m_iNumTeamsPrereq);
	CheckSum(iSum, m_iUnitLevelPrereq);
	CheckSum(iSum, m_iMinLatitude);
	CheckSum(iSum, m_iMaxLatitude);
	CheckSum(iSum, m_iGreatPeopleRateModifier);
	CheckSum(iSum, m_iGreatGeneralRateModifier);
	CheckSum(iSum, m_iDomesticGreatGeneralRateModifier);
	CheckSum(iSum, m_iGlobalGreatPeopleRateModifier);
	CheckSum(iSum, m_iAnarchyModifier);
	CheckSum(iSum, m_iGoldenAgeModifier);
	CheckSum(iSum, m_iGlobalHurryModifier);
	CheckSum(iSum, m_iFreeExperience);
	CheckSum(iSum, m_iGlobalFreeExperience);
	CheckSum(iSum, m_iFoodKept);
	CheckSum(iSum, m_iAirlift);
	CheckSum(iSum, m_iAirModifier);
	CheckSum(iSum, m_iAirUnitCapacity);
	CheckSum(iSum, m_iNukeModifier);
	CheckSum(iSum, m_iNukeExplosionRand);
	CheckSum(iSum, m_iFreeSpecialist);
	CheckSum(iSum, m_iAreaFreeSpecialist);
	CheckSum(iSum, m_iGlobalFreeSpecialist);
	CheckSum(iSum, m_iHappiness);
	CheckSum(iSum, m_iAreaHappiness);
	CheckSum(iSum, m_iGlobalHappiness);
	CheckSum(iSum, m_iStateReligionHappiness);
	CheckSum(iSum, m_iWorkerSpeedModifier);
	CheckSum(iSum, m_iMilitaryProductionModifier);
	CheckSum(iSum, m_iSpaceProductionModifier);
	CheckSum(iSum, m_iGlobalSpaceProductionModifier);
	CheckSum(iSum, m_iTradeRoutes);
	CheckSum(iSum, m_iCoastalTradeRoutes);
	CheckSum(iSum, m_iGlobalTradeRoutes);
	CheckSum(iSum, m_iTradeRouteModifier);
	CheckSum(iSum, m_iForeignTradeRouteModifier);
	CheckSum(iSum, m_iAssetValue);
	CheckSum(iSum, m_iPowerValue);
	CheckSum(iSum, m_eSpecialBuilding);
	CheckSum(iSum, m_iAdvisorType);
	CheckSum(iSum, m_iPrereqGameOption);
	CheckSum(iSum, m_iNotGameOption);
	CheckSum(iSum, m_iHolyCity);
	CheckSum(iSum, m_iReligionType);
	CheckSum(iSum, m_iStateReligion);
	CheckSum(iSum, m_iPrereqReligion);
	CheckSum(iSum, m_iPrereqCorporation);
	CheckSum(iSum, m_iFoundsCorporation);
	CheckSum(iSum, m_iGlobalReligionCommerce);
	CheckSum(iSum, m_iGlobalCorporationCommerce);
	CheckSum(iSum, m_iPrereqAndBonus);
	CheckSum(iSum, m_iGreatPeopleUnitType);
	CheckSum(iSum, m_iGreatPeopleRateChange);
	CheckSum(iSum, m_iConquestProbability);
	CheckSum(iSum, m_iMaintenanceModifier);
	CheckSum(iSum, m_iGlobalMaintenanceModifier);
	CheckSum(iSum, m_iAreaMaintenanceModifier);
	CheckSum(iSum, m_iOtherAreaMaintenanceModifier);
	CheckSum(iSum, m_iDistanceMaintenanceModifier);
	CheckSum(iSum, m_iNumCitiesMaintenanceModifier);
	CheckSum(iSum, m_iCoastalDistanceMaintenanceModifier);
	CheckSum(iSum, m_iConnectedCityMaintenanceModifier);
	CheckSum(iSum, m_iWarWearinessModifier);
	CheckSum(iSum, m_iGlobalWarWearinessModifier);
	CheckSum(iSum, m_iEnemyWarWearinessModifier);
	CheckSum(iSum, m_iHealRateChange);
	CheckSum(iSum, m_iHealth);
	CheckSum(iSum, m_iAreaHealth);
	CheckSum(iSum, m_iGlobalHealth);
	CheckSum(iSum, m_iGlobalPopulationChange);
	CheckSum(iSum, m_iFreeTechs);
	CheckSum(iSum, m_eFreeSpecialTech);
	CheckSum(iSum, m_iDefenseModifier);
	CheckSum(iSum, m_iBombardDefenseModifier);
	CheckSum(iSum, m_iAllCityDefenseModifier);
	CheckSum(iSum, m_iEspionageDefenseModifier);
	CheckSum(iSum, m_iUnitUpgradePriceModifier);
	CheckSum(iSum, m_iRevIdxLocal);
	CheckSum(iSum, m_iRevIdxNational);
	CheckSum(iSum, m_iRevIdxDistanceModifier);
	CheckSum(iSum, m_iMissionType);
	CheckSum(iSum, m_iVoteSourceType);
	//CheckSum(iSum, m_fVisibilityPriority);
	CheckSum(iSum, m_bTeamShare);
	CheckSum(iSum, m_bAutoBuild);
	CheckSum(iSum, m_bWater);
	CheckSum(iSum, m_bRiver);
	CheckSum(iSum, m_bFreshWater);
	CheckSum(iSum, m_bPower);
	CheckSum(iSum, m_bOrbital);
	CheckSum(iSum, m_bOrbitalInfrastructure);
	CheckSum(iSum, m_bNoHolyCity);
	CheckSum(iSum, m_bAreaBorderObstacle);
	CheckSum(iSum, m_bForceTeamVoteEligible);
	CheckSum(iSum, m_bCapital);
	CheckSum(iSum, m_iDCMAirbombMission);
	CheckSum(iSum, m_bGovernmentCenter);
	CheckSum(iSum, m_bGoldenAge);
	CheckSum(iSum, m_bMapCentering);
	CheckSum(iSum, m_bNoUnhappiness);
	CheckSum(iSum, m_bNoUnhealthyPopulation);
	CheckSum(iSum, m_bBuildingOnlyHealthy);
	CheckSum(iSum, m_bNeverCapture);
	CheckSum(iSum, m_bNukeImmune);
	CheckSum(iSum, m_bCenterInCity);
	CheckSum(iSum, m_bStateReligionInCity);
	CheckSum(iSum, m_bAllowsNukes);

	CheckSumC(iSum, m_piPrereqAndTechs);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piSeaPlotYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piRiverPlotYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piGlobalSeaPlotYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldPerPopChange);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piPowerYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piAreaYieldModifier);
	CheckSumI(iSum, NUM_YIELD_TYPES, m_piGlobalYieldModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCommerceChange);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCommercePerPopChange);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCommerceChangeDoubleTime);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCommerceModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piGlobalCommerceModifier);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piSpecialistExtraCommerce);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piStateReligionCommerce);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCommerceHappiness);
	CheckSumI(iSum, GC.getNumReligionInfos(), m_piReligionChange);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_piSpecialistCount);
	CheckSumI(iSum, GC.getNumSpecialistInfos(), m_piFreeSpecialistCount);
	CheckSumI(iSum, GC.getNumBonusInfos(), m_piBonusHealthChanges);
	CheckSumI(iSum, GC.getNumBonusInfos(), m_piBonusHappinessChanges);
	CheckSumI(iSum, GC.getNumBonusInfos(), m_piBonusProductionModifier);
	CheckSumI(iSum, GC.getNumUnitCombatInfos(), m_piUnitCombatFreeExperience);
	CheckSumI(iSum, NUM_DOMAIN_TYPES, m_piDomainFreeExperience);
	CheckSumI(iSum, NUM_DOMAIN_TYPES, m_piDomainProductionModifier);
	CheckSumC(iSum, m_aBuildingHappinessChanges);
	CheckSumC(iSum, m_aPrereqNumOfBuilding);
	CheckSumI(iSum, GC.getNumFlavorTypes(), m_piFlavorValue);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_piImprovementFreeSpecialist);

	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_pbCommerceFlexible);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_pbCommerceChangeOriginalOwner);

	if (m_ppaiSpecialistYieldChange)
	{
		for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiSpecialistYieldChange[i]);
		}
	}
	if (m_ppaiSpecialistCommerceChange)
	{
		for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiSpecialistCommerceChange[i]);
		}
	}

	if (m_ppaiLocalSpecialistYieldChange)
	{
		for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiLocalSpecialistYieldChange[i]);
		}
	}

	if (m_ppaiLocalSpecialistCommerceChange)
	{
		for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
		{
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiLocalSpecialistCommerceChange[i]);
		}
	}

	if (m_ppaiBonusYieldModifier)
	{
		for(int i = 0; i < GC.getNumBonusInfos(); i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiBonusYieldModifier[i]);
		}
	}

	CheckSum(iSum, m_iGlobalPopulationgrowthratepercentage);
	CheckSum(iSum, m_iPopulationgrowthratepercentage);
	CheckSum(iSum, m_iPrereqVicinityBonus);
	CheckSum(iSum, m_iPrereqRawVicinityBonus);
	CheckSum(iSum, m_iPillageGoldModifier);
	CheckSum(iSum, m_iFreePromotion_2);
	CheckSum(iSum, m_iFreePromotion_3);
	CheckSum(iSum, m_iWorldTradeRoutes);
	CheckSum(iSum, m_iLineOfSight);
	CheckSum(iSum, m_iInflationModifier);
	CheckSum(iSum, m_iInvasionChance);
	CheckSum(iSum, m_iAdjacentDamagePercent);
	CheckSum(iSum, m_iProductionContinueBuilding);
	CheckSum(iSum, m_iPrereqPopulation);
	CheckSum(iSum, m_iPrereqCultureLevel);
	CheckSum(iSum, m_iWorkableRadius);
	CheckSum(iSum, m_iPrereqAnyoneBuilding);
	CheckSum(iSum, m_iExtendsBuilding);
	CheckSum(iSum, m_iObsoletesToBuilding);
	CheckSum(iSum, m_iOccupationTimeModifier);
	CheckSum(iSum, m_iNoEntryDefenseLevel);
	CheckSum(iSum, m_iNumUnitFullHeal);
	CheckSum(iSum, m_iNumPopulationEmployed);
	CheckSum(iSum, m_iHealthPercentPerPopulation);
	CheckSum(iSum, m_iHappinessPercentPerPopulation);

	CheckSum(iSum, m_bApplyFreePromotionOnMove);
	CheckSum(iSum, m_bProtectedCulture);
	CheckSum(iSum, m_bPrereqPower);
	CheckSum(iSum, m_bNoEnemyPillagingIncome);
	CheckSum(iSum, m_bProvidesFreshWater);
	CheckSum(iSum, m_bForceAllTradeRoutes);
	CheckSum(iSum, m_bForceNoPrereqScaling);
	CheckSum(iSum, m_bPrereqWar);
	CheckSum(iSum, m_bRequiresActiveCivics);
	CheckSum(iSum, m_bZoneOfControl);

	CheckSumI(iSum, GC.getNumCivicInfos(), m_pbPrereqOrCivics);
	CheckSumI(iSum, GC.getNumCivicInfos(), m_pbPrereqAndCivics);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbPrereqOrTerrain);
	CheckSumI(iSum, GC.getNumTerrainInfos(), m_pbPrereqAndTerrain);
	CheckSumI(iSum, GC.getNumImprovementInfos(), m_pbPrereqOrImprovement);
	CheckSumI(iSum, GC.getNumFeatureInfos(), m_pbPrereqOrFeature);
	CheckSumC(iSum, m_aBuildingProductionModifier);
	CheckSumC(iSum, m_aGlobalBuildingProductionModifier);
	CheckSumC(iSum, m_aGlobalBuildingCostModifier);

	CheckSumC(iSum, m_aUnitProductionModifier);
	CheckSumC(iSum, m_piPrereqOrVicinityBonuses);
	CheckSumI(iSum, GC.getNumBonusInfos(), m_piBonusDefenseChanges);
	CheckSumC(iSum, m_aUnitCombatExtraStrength);
	CheckSumI(iSum, NUM_COMMERCE_TYPES, m_piCommerceAttacks);

	for(int i = 0; i < GC.getNumTechInfos(); i++)
	{
		if (m_ppaiTechCommerceChange)
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiTechCommerceChange[i]);
		if (m_ppaiTechYieldChange)
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiTechYieldChange[i]);
		if (m_ppaiTechSpecialistChange)
			CheckSumI(iSum, GC.getNumSpecialistInfos(), m_ppaiTechSpecialistChange[i]);
		if (m_ppaiTechCommerceModifier)
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiTechCommerceModifier[i]);
		if (m_ppaiTechYieldModifier)
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiTechYieldModifier[i]);
	}

	for(int i = 0; i < GC.getNumBonusInfos(); i++)
	{
		if (m_ppaiBonusCommerceModifier)
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiBonusCommerceModifier[i]);
		if (m_ppaiBonusCommercePercentChanges)
			CheckSumI(iSum, NUM_COMMERCE_TYPES, m_ppaiBonusCommercePercentChanges[i]);
		if (m_ppaiBonusYieldChanges)
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiBonusYieldChanges[i]);
		if (m_ppaiVicinityBonusYieldChanges)
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppaiVicinityBonusYieldChanges[i]);
	}

	if (m_ppiImprovementYieldChanges)
	{
		for (int i = 0; i < GC.getNumImprovementInfos(); i++)
		{
			CheckSumI(iSum, NUM_YIELD_TYPES, m_ppiImprovementYieldChanges[i]);
		}
	}

	m_Properties.getCheckSum(iSum);
	m_PropertiesAllCities.getCheckSum(iSum);
	m_PrereqMinProperties.getCheckSum(iSum);
	m_PrereqMaxProperties.getCheckSum(iSum);
	m_PrereqPlayerMinProperties.getCheckSum(iSum);
	m_PrereqPlayerMaxProperties.getCheckSum(iSum);

	m_PropertyManipulators.getCheckSum(iSum);

	if (m_pExprNewCityFree)
		m_pExprNewCityFree->getCheckSum(iSum);

	if (m_pExprConstructCondition)
		m_pExprConstructCondition->getCheckSum(iSum);
	//TB Combat Mods (Buildings) begin
	CheckSum(iSum, m_ePropertySpawnUnit);
	CheckSum(iSum, m_ePropertySpawnProperty);
	CheckSum(iSum, (int)m_ePromotionLineType);
	//integers
	CheckSum(iSum, m_iLinePriority);
	CheckSum(iSum, m_iOutbreakBase);
	CheckSum(iSum, m_iOvercomeBase);
	CheckSum(iSum, m_iTradeCommunicability);
#ifdef STRENGTH_IN_NUMBERS
	CheckSum(iSum, m_iFrontSupportPercentModifier);
	CheckSum(iSum, m_iShortRangeSupportPercentModifier);
	CheckSum(iSum, m_iMediumRangeSupportPercentModifier);
	CheckSum(iSum, m_iLongRangeSupportPercentModifier);
	CheckSum(iSum, m_iFlankSupportPercentModifier);
#endif
	CheckSum(iSum, m_iNationalCaptureProbabilityModifier);
	CheckSum(iSum, m_iNationalCaptureResistanceModifier);
	CheckSum(iSum, m_iLocalCaptureProbabilityModifier);
	CheckSum(iSum, m_iLocalCaptureResistanceModifier);
	CheckSum(iSum, m_iLocalDynamicDefense);
	CheckSum(iSum, m_iRiverDefensePenalty);
	CheckSum(iSum, m_iLocalRepel);
	CheckSum(iSum, m_iMinDefense);
	CheckSum(iSum, m_iBuildingDefenseRecoverySpeedModifier);
	CheckSum(iSum, m_iCityDefenseRecoverySpeedModifier);
	CheckSum(iSum, m_iDamageAttackerChance);
	CheckSum(iSum, m_iDamageToAttacker);
	CheckSum(iSum, m_iMaxPopulationAllowed);
	CheckSum(iSum, m_iMaxPopulationChange);
	CheckSum(iSum, m_iInsidiousness);
	CheckSum(iSum, m_iInvestigation);
	CheckSum(iSum, m_iPopulationChange);
	CheckSum(iSum, m_iOneTimePopulationPercentLoss);
	CheckSum(iSum, m_bDamageToAttackerIgnoresArmor);
	CheckSum(iSum, m_bDamageAllAttackers);
	CheckSum(iSum, m_bDamageAttackerCapable);
	CheckSum(iSum, m_bQuarantine);
	CheckSum(iSum, m_iMaxPopAllowed);

	//Structs
	int iNumElements = m_aFreePromoTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aFreePromoTypes[i].ePromotion);
		if (m_aFreePromoTypes[i].m_pExprFreePromotionCondition)
			m_aFreePromoTypes[i].m_pExprFreePromotionCondition->getCheckSum(iSum);
	}

	iNumElements = m_aHealUnitCombatTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aHealUnitCombatTypes[i].eUnitCombat);
		CheckSum(iSum, m_aHealUnitCombatTypes[i].iHeal);
	}

	iNumElements = m_aBonusAidModifiers.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aBonusAidModifiers[i].eBonusType);
		CheckSum(iSum, m_aBonusAidModifiers[i].ePropertyType);
		CheckSum(iSum, m_aBonusAidModifiers[i].iModifier);
	}

	iNumElements = m_aAidRateChanges.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aAidRateChanges[i].ePropertyType);
		CheckSum(iSum, m_aAidRateChanges[i].iChange);
	}

	iNumElements = m_aEnabledCivilizationTypes.size();
	for (int i = 0; i < iNumElements; ++i)
	{
		CheckSum(iSum, m_aEnabledCivilizationTypes[i].eCivilization);
	}

	CheckSumC(iSum, m_aiUnitCombatRetrainTypes);
	CheckSumC(iSum, m_aiMayDamageAttackingUnitCombatTypes);
	CheckSumC(iSum, m_aeMapCategoryTypes);
	CheckSumC(iSum, m_aUnitCombatRepelModifiers);
	CheckSumC(iSum, m_aUnitCombatRepelAgainstModifiers);
	CheckSumC(iSum, m_aUnitCombatDefenseAgainstModifiers);
	CheckSumC(iSum, m_aUnitCombatProdModifiers);
	CheckSumC(iSum, m_aUnitCombatOngoingTrainingDurations);
	CheckSumC(iSum, m_aAfflictionOutbreakLevelChanges);
	CheckSumC(iSum, m_aTechOutbreakLevelChanges);
	CheckSumC(iSum, m_aTechHappinessChanges);
	CheckSumC(iSum, m_aTechHealthChanges);
	CheckSumC(iSum, m_aiFreeTraitTypes);
	CheckSumC(iSum, m_aExtraFreeBonuses);
	CheckSumC(iSum, m_aePrereqOrRawVicinityBonuses);
	CheckSumC(iSum, m_aePrereqOrBonuses);
	CheckSumC(iSum, m_aiPrereqInCityBuildings);
	CheckSumC(iSum, m_vPrereqNotInCityBuildings);
	CheckSumC(iSum, m_vPrereqOrBuilding);
	CheckSumC(iSum, m_vReplacementBuilding);
	CheckSumC(iSum, m_vReplacedBuilding);

	// Arrays
	CheckSumI(iSum, GC.getNumHurryInfos(), m_pabHurry);

	//if (m_pExprFreePromotionCondition)
	//	m_pExprFreePromotionCondition->getCheckSum(iSum);
	//TB Combat Mods (Buildings) end

	foreach_(const BuildingCommerceChange& it, m_aGlobalBuildingCommerceChanges)
	{
		CheckSum(iSum, it.eBuilding);
		CheckSum(iSum, it.eCommerce);
		CheckSum(iSum, it.iChange);
	}

	CheckSum(iSum, m_iMaxGlobalInstances);
	CheckSum(iSum, m_iMaxTeamInstances);
	CheckSum(iSum, m_iMaxPlayerInstances);
	CheckSum(iSum, m_iExtraPlayerInstances);
	CheckSum(iSum, m_piVictoryThreshold, GC.getNumVictoryInfos());
}

//
// read from XML
//
bool CvBuildingInfo::read(CvXMLLoadUtility* pXML)
{
	CvString szTextVal;

	if (!CvHotkeyInfo::read(pXML))
	{
		return false;
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"SpecialBuildingType");
	m_eSpecialBuilding = static_cast<SpecialBuildingTypes>(pXML->GetInfoClass(szTextVal));

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Advisor");
	m_iAdvisorType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_bNoLimit, L"bNoLimit");

	pXML->GetOptionalChildXmlValByName(m_szArtDefineTag, L"ArtDefineTag");

	setNotShowInCity();

	pXML->GetOptionalChildXmlValByName(m_szMovieDefineTag, L"MovieDefineTag");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqGameOption");
	m_iPrereqGameOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NotGameOption");
	m_iNotGameOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"HolyCity");
	m_iHolyCity = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ReligionType");
	m_iReligionType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"StateReligion");
	m_iStateReligion = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqReligion");
	m_iPrereqReligion = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqCorporation");
	m_iPrereqCorporation = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FoundsCorporation");
	m_iFoundsCorporation = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"GlobalReligionCommerce");
	m_iGlobalReligionCommerce = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"GlobalCorporationCommerce");
	m_iGlobalCorporationCommerce = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"VictoryPrereq");
	m_iVictoryPrereq = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeStartEra");
	m_iFreeStartEra = static_cast<EraTypes>(pXML->GetInfoClass(szTextVal));

	pXML->GetOptionalChildXmlValByName(szTextVal, L"MaxStartEra");
	m_iMaxStartEra = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"ObsoleteTech");
	m_iObsoleteTech = static_cast<TechTypes>(pXML->GetInfoClass(szTextVal));

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqTech");
	m_iPrereqAndTech = pXML->GetInfoClass(szTextVal);

	pXML->SetOptionalVector(&m_piPrereqAndTechs, L"TechTypes");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"Bonus");
	m_iPrereqAndBonus = pXML->GetInfoClass(szTextVal);

	//Alberts2 PrereqBonuses
	pXML->SetOptionalVector(&m_aePrereqOrBonuses, L"PrereqBonuses");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"NoBonus");
	m_iNoBonus = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PowerBonus");
	m_iPowerBonus = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeBonus");
	m_iFreeBonus = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iNumFreeBonuses, L"iNumFreeBonuses");

	if(pXML->TryMoveToXmlFirstChild(L"ExtraFreeBonuses"))
	{
		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"ExtraFreeBonus"))
			{
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"FreeBonus");
					BonusTypes eBonus = (BonusTypes) pXML->GetInfoClass(szTextVal);
					int iNum;
					pXML->GetChildXmlValByName(&iNum, L"iNumFreeBonuses");
					m_aExtraFreeBonuses.push_back(std::pair<BonusTypes,int>(eBonus, iNum));
				} while(pXML->TryMoveToXmlNextSibling());
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreePromotion");
	m_iFreePromotion = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"CivicOption");
	m_iCivicOption = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"GreatPeopleUnitType");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"DiploVoteType");
	m_iVoteSourceType = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateChange, L"iGreatPeopleRateChange");
	pXML->GetOptionalChildXmlValByName(&m_bTeamShare, L"bTeamShare");
	pXML->GetOptionalChildXmlValByName(&m_bAutoBuild, L"bAutoBuild");
	pXML->GetOptionalChildXmlValByName(&m_bWater, L"bWater");
	pXML->GetOptionalChildXmlValByName(&m_bRiver, L"bRiver");
	pXML->GetOptionalChildXmlValByName(&m_bFreshWater, L"bFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bPower, L"bPower");
	pXML->GetOptionalChildXmlValByName(&m_bOrbital, L"bOrbital");
	pXML->GetOptionalChildXmlValByName(&m_bOrbitalInfrastructure, L"bOrbitalInfrastructure");
	pXML->GetOptionalChildXmlValByName(&m_bNoHolyCity, L"bNoHolyCity");
	pXML->GetOptionalChildXmlValByName(&m_bAreaBorderObstacle, L"bBorderObstacle");
	pXML->GetOptionalChildXmlValByName(&m_bForceTeamVoteEligible, L"bForceTeamVoteEligible");
	pXML->GetOptionalChildXmlValByName(&m_bCapital, L"bCapital");
	pXML->GetOptionalChildXmlValByName(&m_iDCMAirbombMission, L"iDCMAirbombMission");
	pXML->GetOptionalChildXmlValByName(&m_bGovernmentCenter, L"bGovernmentCenter");
	pXML->GetOptionalChildXmlValByName(&m_bGoldenAge, L"bGoldenAge");
	pXML->GetOptionalChildXmlValByName(&m_bAllowsNukes, L"bAllowsNukes");
	pXML->GetOptionalChildXmlValByName(&m_bMapCentering, L"bMapCentering");
	pXML->GetOptionalChildXmlValByName(&m_bNoUnhappiness, L"bNoUnhappiness");
	pXML->GetOptionalChildXmlValByName(&m_bNoUnhealthyPopulation, L"bNoUnhealthyPopulation");
	pXML->GetOptionalChildXmlValByName(&m_bBuildingOnlyHealthy, L"bBuildingOnlyHealthy");
	pXML->GetOptionalChildXmlValByName(&m_bNeverCapture, L"bNeverCapture");
	pXML->GetOptionalChildXmlValByName(&m_bNukeImmune, L"bNukeImmune");
	pXML->GetOptionalChildXmlValByName(&m_bCenterInCity, L"bCenterInCity");
	pXML->GetOptionalChildXmlValByName(&m_bStateReligionInCity, L"bNeedStateReligionInCity");
	pXML->GetOptionalChildXmlValByName(&m_iAIWeight, L"iAIWeight");
	pXML->GetOptionalChildXmlValByName(&m_iProductionCost, L"iCost", -1);
	pXML->GetOptionalChildXmlValByName(&m_iHurryCostModifier, L"iHurryCostModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHurryAngerModifier, L"iHurryAngerModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMinAreaSize, L"iMinAreaSize");
	pXML->GetOptionalChildXmlValByName(&m_iConquestProbability, L"iConquestProb", 50);
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesPrereq, L"iCitiesPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iNumTeamsPrereq, L"iTeamsPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iUnitLevelPrereq, L"iLevelPrereq");
	pXML->GetOptionalChildXmlValByName(&m_iMinLatitude, L"iMinLatitude");
	pXML->GetOptionalChildXmlValByName(&m_iMaxLatitude, L"iMaxLatitude", 90);
	pXML->GetOptionalChildXmlValByName(&m_iGreatPeopleRateModifier, L"iGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGreatGeneralRateModifier, L"iGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDomesticGreatGeneralRateModifier, L"iDomesticGreatGeneralRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalGreatPeopleRateModifier, L"iGlobalGreatPeopleRateModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAnarchyModifier, L"iAnarchyModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGoldenAgeModifier, L"iGoldenAgeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHurryModifier, L"iGlobalHurryModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFreeExperience, L"iExperience");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalFreeExperience, L"iGlobalExperience");
	pXML->GetOptionalChildXmlValByName(&m_iFoodKept, L"iFoodKept");
	pXML->GetOptionalChildXmlValByName(&m_iAirlift, L"iAirlift");
	pXML->GetOptionalChildXmlValByName(&m_iAirModifier, L"iAirModifier");
	pXML->GetOptionalChildXmlValByName(&m_iAirUnitCapacity, L"iAirUnitCapacity");
	pXML->GetOptionalChildXmlValByName(&m_iNukeModifier, L"iNukeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNukeExplosionRand, L"iNukeExplosionRand");
	pXML->GetOptionalChildXmlValByName(&m_iFreeSpecialist, L"iFreeSpecialist");
	pXML->GetOptionalChildXmlValByName(&m_iAreaFreeSpecialist, L"iAreaFreeSpecialist");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalFreeSpecialist, L"iGlobalFreeSpecialist");
	pXML->GetOptionalChildXmlValByName(&m_iMaintenanceModifier, L"iMaintenanceModifier");
    pXML->GetOptionalChildXmlValByName(&m_iGlobalMaintenanceModifier, L"iGlobalMaintenanceModifier");
    pXML->GetOptionalChildXmlValByName(&m_iAreaMaintenanceModifier, L"iAreaMaintenanceModifier");
    pXML->GetOptionalChildXmlValByName(&m_iOtherAreaMaintenanceModifier, L"iOtherAreaMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDistanceMaintenanceModifier, L"iDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNumCitiesMaintenanceModifier, L"iNumCitiesMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCoastalDistanceMaintenanceModifier, L"iCoastalDistanceMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iConnectedCityMaintenanceModifier, L"iConnectedCityMaintenanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWarWearinessModifier, L"iWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalWarWearinessModifier, L"iGlobalWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iEnemyWarWearinessModifier, L"iEnemyWarWearinessModifier");
	pXML->GetOptionalChildXmlValByName(&m_iHealRateChange, L"iHealRateChange");
	pXML->GetOptionalChildXmlValByName(&m_iHealth, L"iHealth");
	pXML->GetOptionalChildXmlValByName(&m_iAreaHealth, L"iAreaHealth");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHealth, L"iGlobalHealth");
	pXML->GetOptionalChildXmlValByName(&m_iHappiness, L"iHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iAreaHappiness, L"iAreaHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalHappiness, L"iGlobalHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iStateReligionHappiness, L"iStateReligionHappiness");
	pXML->GetOptionalChildXmlValByName(&m_iWorkerSpeedModifier, L"iWorkerSpeedModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMilitaryProductionModifier, L"iMilitaryProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iSpaceProductionModifier, L"iSpaceProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalSpaceProductionModifier, L"iGlobalSpaceProductionModifier");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRoutes, L"iTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iCoastalTradeRoutes, L"iCoastalTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalTradeRoutes, L"iGlobalTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iTradeRouteModifier, L"iTradeRouteModifier");
	pXML->GetOptionalChildXmlValByName(&m_iForeignTradeRouteModifier, L"iForeignTradeRouteModifier");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalPopulationChange, L"iGlobalPopulationChange");
	pXML->GetOptionalChildXmlValByName(&m_iFreeTechs, L"iFreeTechs");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreeSpecialTech");
	m_eFreeSpecialTech = (TechTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(&m_iDefenseModifier, L"iDefense");
	pXML->GetOptionalChildXmlValByName(&m_iBombardDefenseModifier, L"iBombardDefense");
	pXML->GetOptionalChildXmlValByName(&m_iAllCityDefenseModifier, L"iAllCityDefense");
	pXML->GetOptionalChildXmlValByName(&m_iEspionageDefenseModifier, L"iEspionageDefense");
	pXML->GetOptionalChildXmlValByName(&m_iUnitUpgradePriceModifier, L"iUnitUpgradePriceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxLocal, L"iRevIdxLocal");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxNational, L"iRevIdxNational");
	pXML->GetOptionalChildXmlValByName(&m_iRevIdxDistanceModifier, L"iRevIdxDistanceModifier");

	pXML->GetOptionalChildXmlValByName(&m_iMaxPopAllowed, L"iObsoletePopulation", -1);

	pXML->GetOptionalChildXmlValByName(&m_iAssetValue, L"iAsset");
	pXML->GetOptionalChildXmlValByName(&m_iPowerValue, L"iPower");
	pXML->GetOptionalChildXmlValByName(&m_fVisibilityPriority, L"fVisibilityPriority");

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"SeaPlotYieldChanges"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piSeaPlotYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piSeaPlotYieldChange);
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"RiverPlotYieldChanges"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piRiverPlotYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piRiverPlotYieldChange);
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"GlobalSeaPlotYieldChanges"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piGlobalSeaPlotYieldChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGlobalSeaPlotYieldChange);
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
		SAFE_DELETE_ARRAY(m_piYieldChange);
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"YieldPerPopChanges"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldPerPopChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldPerPopChange);
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piYieldModifier);
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"PowerYieldModifiers"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piPowerYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piPowerYieldModifier);
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"AreaYieldModifiers"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piAreaYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piAreaYieldModifier);
	}

	// if we can set the current xml node to it's next sibling
	if (pXML->TryMoveToXmlFirstChild(L"GlobalYieldModifiers"))
	{
		// call the function that sets the yield change variable
		pXML->SetYields(&m_piGlobalYieldModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGlobalYieldModifier);
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

	if (pXML->TryMoveToXmlFirstChild(L"CommercePerPopChanges"))
	{
		pXML->SetCommerce(&m_piCommercePerPopChange);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommercePerPopChange);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceChangeDoubleTimes"))
	{
		pXML->SetCommerce(&m_piCommerceChangeDoubleTime);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceChangeDoubleTime);
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

	if (pXML->TryMoveToXmlFirstChild(L"GlobalCommerceModifiers"))
	{
		pXML->SetCommerce(&m_piGlobalCommerceModifier);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piGlobalCommerceModifier);
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

	if (pXML->TryMoveToXmlFirstChild(L"StateReligionCommerces"))
	{
		pXML->SetCommerce(&m_piStateReligionCommerce);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piStateReligionCommerce);
	}

	if (pXML->TryMoveToXmlFirstChild(L"CommerceHappinesses"))
	{
		pXML->SetCommerce(&m_piCommerceHappiness);
		pXML->MoveToXmlParent();
	}
	else
	{
		SAFE_DELETE_ARRAY(m_piCommerceHappiness);
	}

	pXML->SetVariableListTagPair(&m_piReligionChange, L"ReligionChanges", GC.getNumReligionInfos());

	pXML->SetVariableListTagPair(&m_piSpecialistCount, L"SpecialistCounts", GC.getNumSpecialistInfos());
	pXML->SetVariableListTagPair(&m_piFreeSpecialistCount, L"FreeSpecialistCounts", GC.getNumSpecialistInfos());

	if (pXML->TryMoveToXmlFirstChild(L"CommerceFlexibles"))
	{
		pXML->SetCommerce(&m_pbCommerceFlexible);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_pbCommerceFlexible);

	if (pXML->TryMoveToXmlFirstChild(L"CommerceChangeOriginalOwners"))
	{
		pXML->SetCommerce(&m_pbCommerceChangeOriginalOwner);
		pXML->MoveToXmlParent();
	}
	else
		SAFE_DELETE_ARRAY(m_pbCommerceChangeOriginalOwner);

	pXML->GetOptionalChildXmlValByName(m_szConstructSound, L"ConstructSound");

	pXML->SetVariableListTagPair(&m_piBonusHealthChanges, L"BonusHealthChanges", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piBonusHappinessChanges, L"BonusHappinessChanges", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piBonusProductionModifier, L"BonusProductionModifiers", GC.getNumBonusInfos());
	pXML->SetVariableListTagPair(&m_piUnitCombatFreeExperience, L"UnitCombatFreeExperiences", GC.getNumUnitCombatInfos());
	pXML->SetVariableListTagPair(&m_piDomainFreeExperience, L"DomainFreeExperiences", NUM_DOMAIN_TYPES);
	pXML->SetVariableListTagPair(&m_piDomainProductionModifier, L"DomainProductionModifiers", NUM_DOMAIN_TYPES);

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChanges"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistYieldChange"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				const int k = pXML->GetInfoClass(szTextVal);
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

	if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChanges"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"SpecialistCommerceChange"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				const int k = pXML->GetInfoClass(szTextVal);
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

//Team Project (1)
	if (pXML->TryMoveToXmlFirstChild(L"LocalSpecialistYieldChanges"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"LocalSpecialistYieldChange"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				const int k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiLocalSpecialistYieldChange == NULL )
					{
						m_ppaiLocalSpecialistYieldChange = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiLocalSpecialistYieldChange[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiLocalSpecialistYieldChange[k]);
						pXML->MoveToXmlParent();
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

	if (pXML->TryMoveToXmlFirstChild(L"LocalSpecialistCommerceChanges"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"LocalSpecialistCommerceChange"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"SpecialistType");
				const int k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiLocalSpecialistCommerceChange == NULL )
					{
						m_ppaiLocalSpecialistCommerceChange = new int*[GC.getNumSpecialistInfos()];

						for(int i = 0; i < GC.getNumSpecialistInfos(); i++)
						{
							m_ppaiLocalSpecialistCommerceChange[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"CommerceChanges"))
					{
						// call the function that sets the yield change variable
						pXML->SetCommerce(&m_ppaiLocalSpecialistCommerceChange[k]);
						pXML->MoveToXmlParent();
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

	if (pXML->TryMoveToXmlFirstChild(L"BonusYieldModifiers"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"BonusYieldModifier"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"BonusType");
				const int k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiBonusYieldModifier == NULL )
					{
						m_ppaiBonusYieldModifier = new int*[GC.getNumBonusInfos()];

						for(int i = 0; i < GC.getNumBonusInfos(); i++)
						{
							m_ppaiBonusYieldModifier[i] = 0;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldModifiers"))
					{
						// call the function that sets the yield change variable
						pXML->SetYields(&m_ppaiBonusYieldModifier[k]);
						pXML->MoveToXmlParent();
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
	pXML->SetVariableListTagPair(&m_piImprovementFreeSpecialist, L"ImprovementFreeSpecialists", GC.getNumImprovementInfos());

	//pXML->GetOptionalChildXmlValByName(&m_iRevIdxLocal, L"iRevolutionIndexModifier");
	pXML->GetOptionalChildXmlValByName(&m_bApplyFreePromotionOnMove, L"bApplyFreePromotionOnMove");
	pXML->GetOptionalChildXmlValByName(&m_bNoEnemyPillagingIncome, L"bNoEnemyPillagingIncome");
	pXML->GetOptionalChildXmlValByName(&m_bProvidesFreshWater, L"bProvidesFreshWater");
	pXML->GetOptionalChildXmlValByName(&m_bForceAllTradeRoutes, L"bForceAllTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_iPillageGoldModifier, L"iPillageGoldModifier");
	pXML->GetOptionalChildXmlValByName(&m_iWorldTradeRoutes, L"iWorldTradeRoutes");
	pXML->GetOptionalChildXmlValByName(&m_bPrereqPower, L"bPrereqPower");
	pXML->GetOptionalChildXmlValByName(&m_bForceNoPrereqScaling, L"bForceNoPrereqScaling");
	pXML->GetOptionalChildXmlValByName(&m_bPrereqWar, L"bPrereqWar");
	pXML->GetOptionalChildXmlValByName(&m_bRequiresActiveCivics, L"bRequiresActiveCivics");
	pXML->GetOptionalChildXmlValByName(&m_bZoneOfControl, L"bZoneOfControl");
	pXML->GetOptionalChildXmlValByName(&m_iGlobalPopulationgrowthratepercentage, L"iGlobalPopulationgrowthratepercentage");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationgrowthratepercentage, L"iPopulationgrowthratepercentage");
	pXML->GetOptionalChildXmlValByName(&m_iLineOfSight, L"iLineOfSight");
	pXML->GetOptionalChildXmlValByName(&m_iInflationModifier, L"iInflationModifier");
	pXML->GetOptionalChildXmlValByName(&m_iInvasionChance, L"iInvasionChance");
	pXML->GetOptionalChildXmlValByName(&m_iAdjacentDamagePercent, L"iAdjacentDamagePercent");
	pXML->GetOptionalChildXmlValByName(&m_iPrereqPopulation, L"iPrereqPopulation");
	pXML->GetOptionalChildXmlValByName(&m_iWorkableRadius, L"iWorkableRadius");
	pXML->GetOptionalChildXmlValByName(&m_iOccupationTimeModifier, L"iOccupationTimeModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNoEntryDefenseLevel, L"iNoEntryDefenseLevel");
	pXML->GetOptionalChildXmlValByName(&m_iNumUnitFullHeal, L"iNumUnitFullHeal");
	pXML->GetOptionalChildXmlValByName(&m_iNumPopulationEmployed, L"iNumPopulationEmployed");
	pXML->GetOptionalChildXmlValByName(&m_iHealthPercentPerPopulation, L"iHealthPercentPerPopulation");
	pXML->GetOptionalChildXmlValByName(&m_iHappinessPercentPerPopulation, L"iHappinessPercentPerPopulation");
	pXML->GetOptionalChildXmlValByName(&m_bProtectedCulture, L"bProtectedCulture");

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PrereqCultureLevel");
	m_iPrereqCultureLevel = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreePromotion_2");
	m_iFreePromotion_2 = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"FreePromotion_3");
	m_iFreePromotion_3 = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"VicinityBonus");
	m_iPrereqVicinityBonus = pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"RawVicinityBonus");
	m_iPrereqRawVicinityBonus = pXML->GetInfoClass(szTextVal);

	pXML->SetVariableListTagPair(&m_pbPrereqOrTerrain, L"PrereqOrTerrain", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbPrereqAndTerrain, L"PrereqAndTerrain", GC.getNumTerrainInfos());
	pXML->SetVariableListTagPair(&m_pbPrereqOrImprovement, L"PrereqOrImprovement", GC.getNumImprovementInfos());
	pXML->SetVariableListTagPair(&m_pbPrereqOrFeature, L"PrereqOrFeature", GC.getNumFeatureInfos());
	pXML->SetVariableListTagPair(&m_piBonusDefenseChanges, L"BonusDefenseChanges", GC.getNumBonusInfos());
	m_aUnitCombatExtraStrength.read(pXML, L"UnitCombatExtraStrengths");

	if (pXML->TryMoveToXmlFirstChild(L"CommerceAttacks"))
	{
		pXML->SetCommerce(&m_piCommerceAttacks);
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"BonusCommerceModifiers"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"BonusCommerceModifier"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"BonusType");
				const int k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiBonusCommerceModifier == NULL )
					{
						m_ppaiBonusCommerceModifier = new int*[GC.getNumBonusInfos()];

						for(int i = 0; i < GC.getNumBonusInfos(); i++)
						{
							m_ppaiBonusCommerceModifier[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"CommerceModifiers"))
					{
						// call the function that sets the commerce change variable
						pXML->SetCommerce(&m_ppaiBonusCommerceModifier[k]);
						pXML->MoveToXmlParent();
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

	if (pXML->TryMoveToXmlFirstChild(L"BonusCommercePercentChanges"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"BonusCommercePercentChange"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"BonusType");
				const int k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiBonusCommercePercentChanges == NULL )
					{
						m_ppaiBonusCommercePercentChanges = new int*[GC.getNumBonusInfos()];

						for(int i = 0; i < GC.getNumBonusInfos(); i++)
						{
							m_ppaiBonusCommercePercentChanges[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"CommercePercents"))
					{
						// call the function that sets the commerce change variable
						pXML->SetCommerce(&m_ppaiBonusCommercePercentChanges[k]);
						pXML->MoveToXmlParent();
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

	if (pXML->TryMoveToXmlFirstChild(L"BonusYieldChanges"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"BonusYieldChange"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"BonusType");
				const int k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiBonusYieldChanges == NULL )
					{
						m_ppaiBonusYieldChanges = new int*[GC.getNumBonusInfos()];

						for(int i = 0; i < GC.getNumBonusInfos(); i++)
						{
							m_ppaiBonusYieldChanges[i] = NULL;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the commerce change variable
						pXML->SetYields(&m_ppaiBonusYieldChanges[k]);
						pXML->MoveToXmlParent();
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

	if (pXML->TryMoveToXmlFirstChild(L"VicinityBonusYieldChanges"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (pXML->TryMoveToXmlFirstChild(L"BonusYieldChange"))
		{
			for (int j = 0; j < iNumChildren; ++j)
			{
				pXML->GetChildXmlValByName(szTextVal, L"BonusType");
				const int k = pXML->GetInfoClass(szTextVal);
				if (k > -1)
				{
					if ( m_ppaiVicinityBonusYieldChanges == NULL )
					{
						m_ppaiVicinityBonusYieldChanges = new int*[GC.getNumBonusInfos()];

						for(int i = 0; i < GC.getNumBonusInfos(); i++)
						{
							m_ppaiVicinityBonusYieldChanges[i] = 0;
						}
					}
					if (pXML->TryMoveToXmlFirstChild(L"YieldChanges"))
					{
						// call the function that sets the commerce change variable
						pXML->SetYields(&m_ppaiVicinityBonusYieldChanges[k]);
						pXML->MoveToXmlParent();
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

	if (pXML->TryMoveToXmlFirstChild(L"PrereqOrCivics"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		bool bTemp = false;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i = 0; i < iNumSibs; i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
                        m_aszPrereqOrCivicsforPass3.push_back(szTextVal);
                        pXML->GetNextXmlVal(&bTemp);
                        m_abPrereqOrCivicsforPass3.push_back(bTemp);
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

	if (pXML->TryMoveToXmlFirstChild(L"PrereqAndCivics"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		bool bTemp = false;
		if (iNumSibs > 0)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int i = 0; i < iNumSibs; i++)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						m_aszPrereqAndCivicsforPass3.push_back(szTextVal);
						pXML->GetNextXmlVal(&bTemp);
						m_abPrereqAndCivicsforPass3.push_back(bTemp);
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

	pXML->SetOptionalVector(&m_piPrereqOrVicinityBonuses, L"PrereqVicinityBonuses");
	pXML->SetOptionalVector(&m_aePrereqOrRawVicinityBonuses, L"PrereqRawVicinityBonuses");

	if (pXML->TryMoveToXmlFirstChild(L"TechCommerceChanges"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				for (int j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					const int k = pXML->GetInfoClass(szTextVal);

					if (k > -1)
					{
						if ( m_ppaiTechCommerceChange == NULL )
						{
							m_ppaiTechCommerceChange = new int*[GC.getNumTechInfos()];
							for(int i = 0; i < GC.getNumTechInfos(); i++)
							{
								m_ppaiTechCommerceChange[i] = NULL;
							}
						}
						// delete the array since it will be reallocated
						SAFE_DELETE_ARRAY(m_ppaiTechCommerceChange[k]);
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"TechCommerce"))
						{
							// call the function that sets the commerce change variable
							pXML->SetCommerce(&m_ppaiTechCommerceChange[k]);
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

	if (pXML->TryMoveToXmlFirstChild(L"TechYieldChanges"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				for (int j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					const int k = pXML->GetInfoClass(szTextVal);

					if (k > -1)
					{
						if ( m_ppaiTechYieldChange == NULL )
						{
							m_ppaiTechYieldChange = new int*[GC.getNumTechInfos()];

							for(int i = 0; i < GC.getNumTechInfos(); i++)
							{
								m_ppaiTechYieldChange[i] = NULL;
							}
						}
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"TechYield"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppaiTechYieldChange[k]);
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

	if (pXML->TryMoveToXmlFirstChild(L"TechSpecialistChanges"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				for (int j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					const int k = pXML->GetInfoClass(szTextVal);

					if (k > -1)
					{
						if ( m_ppaiTechSpecialistChange == NULL )
						{
							m_ppaiTechSpecialistChange = new int*[GC.getNumTechInfos()];

							for(int i = 0; i < GC.getNumTechInfos(); i++)
							{
								m_ppaiTechSpecialistChange[i] = NULL;
							}
						}
						pXML->SetVariableListTagPair(&m_ppaiTechSpecialistChange[k], L"SpecialistCounts", GC.getNumSpecialistInfos());
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

	if (pXML->TryMoveToXmlFirstChild(L"TechCommerceModifiers"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				for (int j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					const int k = pXML->GetInfoClass(szTextVal);

					if (k > -1)
					{
						if ( m_ppaiTechCommerceModifier == NULL )
						{
							m_ppaiTechCommerceModifier = new int*[GC.getNumTechInfos()];

							for(int i = 0; i < GC.getNumTechInfos(); i++ )
							{
								m_ppaiTechCommerceModifier[i] = NULL;
							}
						}
						if (pXML->TryMoveToXmlFirstChild(L"TechCommerce"))
						{
							// call the function that sets the commerce change variable
							pXML->SetCommerce(&m_ppaiTechCommerceModifier[k]);
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

	if (pXML->TryMoveToXmlFirstChild(L"TechYieldModifiers"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				for (int j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"PrereqTech");
					const int k = pXML->GetInfoClass(szTextVal);

					if (k > -1)
					{
						if ( m_ppaiTechYieldModifier == NULL )
						{
							m_ppaiTechYieldModifier = new int*[GC.getNumTechInfos()];

							for(int i = 0; i < GC.getNumTechInfos(); i++ )
							{
								m_ppaiTechYieldModifier[i] = NULL;
							}
						}
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"TechYield"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppaiTechYieldModifier[k]);
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

	if (pXML->TryMoveToXmlFirstChild(L"ImprovementYieldChanges"))
	{
		const int iNumSibs = pXML->GetXmlChildrenNumber();
		if (pXML->TryMoveToXmlFirstChild())
		{
			if (0 < iNumSibs)
			{
				for (int j = 0; j < iNumSibs; j++)
				{
					pXML->GetChildXmlValByName(szTextVal, L"ImprovementType");
					const int iIndex = pXML->GetInfoClass(szTextVal);

					if (iIndex > -1)
					{
						if ( m_ppiImprovementYieldChanges == NULL )
						{
							m_ppiImprovementYieldChanges = new int*[GC.getNumImprovementInfos()];

							for(int i = 0; i < GC.getNumImprovementInfos(); i++ )
							{
								m_ppiImprovementYieldChanges[i] = NULL;
							}
						}
						// if we can set the current xml node to it's next sibling
						if (pXML->TryMoveToXmlFirstChild(L"ImprovementYields"))
						{
							// call the function that sets the yield change variable
							pXML->SetYields(&m_ppiImprovementYieldChanges[iIndex]);
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

	m_Properties.read(pXML);
	m_PropertiesAllCities.read(pXML, L"PropertiesAllCities");
	m_PrereqMinProperties.read(pXML, L"PrereqMinProperties");
	m_PrereqMaxProperties.read(pXML, L"PrereqMaxProperties");
	m_PrereqPlayerMinProperties.read(pXML, L"PrereqPlayerMinProperties");
	m_PrereqPlayerMaxProperties.read(pXML, L"PrereqPlayerMaxProperties");

	m_PropertyManipulators.read(pXML);

	if (pXML->TryMoveToXmlFirstChild(L"NewCityFree"))
	{
		m_pExprNewCityFree = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}

	if (pXML->TryMoveToXmlFirstChild(L"ConstructCondition"))
	{
		m_pExprConstructCondition = BoolExpr::read(pXML);
		pXML->MoveToXmlParent();
	}
	//TB Combat Mods (Buildings) begin
	pXML->GetOptionalChildXmlValByName(szTextVal, L"PropertySpawnUnit");
	m_aszExtraXMLforPass3.push_back(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PropertySpawnProperty");
	m_ePropertySpawnProperty = (PropertyTypes) pXML->GetInfoClass(szTextVal);

	pXML->GetOptionalChildXmlValByName(szTextVal, L"PromotionLineType");
	m_ePromotionLineType = (PromotionLineTypes) pXML->GetInfoClass(szTextVal);
	//integers
	pXML->GetOptionalChildXmlValByName(&m_iLinePriority, L"iLinePriority");
	pXML->GetOptionalChildXmlValByName(&m_iOutbreakBase, L"iOutbreakBase");
	pXML->GetOptionalChildXmlValByName(&m_iOvercomeBase, L"iOvercomeBase");
	pXML->GetOptionalChildXmlValByName(&m_iTradeCommunicability, L"iTradeCommunicability");
#ifdef STRENGTH_IN_NUMBERS
	pXML->GetOptionalChildXmlValByName(&m_iFrontSupportPercentModifier, L"iFrontSupportPercentModifier");
	pXML->GetOptionalChildXmlValByName(&m_iShortRangeSupportPercentModifier, L"iShortRangeSupportPercentModifier");
	pXML->GetOptionalChildXmlValByName(&m_iMediumRangeSupportPercentModifier, L"iMediumRangeSupportPercentModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLongRangeSupportPercentModifier, L"iLongRangeSupportPercentModifier");
	pXML->GetOptionalChildXmlValByName(&m_iFlankSupportPercentModifier, L"iFlankSupportPercentModifier");
#endif
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureProbabilityModifier, L"iNationalCaptureProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iNationalCaptureResistanceModifier, L"iNationalCaptureResistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLocalCaptureProbabilityModifier, L"iLocalCaptureProbabilityModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLocalCaptureResistanceModifier, L"iLocalCaptureResistanceModifier");
	pXML->GetOptionalChildXmlValByName(&m_iLocalDynamicDefense, L"iLocalDynamicDefense");
	pXML->GetOptionalChildXmlValByName(&m_iRiverDefensePenalty, L"iRiverDefensePenalty");
	pXML->GetOptionalChildXmlValByName(&m_iLocalRepel, L"iLocalRepel");
	pXML->GetOptionalChildXmlValByName(&m_iMinDefense, L"iMinDefense");
	pXML->GetOptionalChildXmlValByName(&m_iBuildingDefenseRecoverySpeedModifier, L"iBuildingDefenseRecoverySpeedModifier");
	pXML->GetOptionalChildXmlValByName(&m_iCityDefenseRecoverySpeedModifier, L"iCityDefenseRecoverySpeedModifier");
	pXML->GetOptionalChildXmlValByName(&m_iDamageAttackerChance, L"iDamageAttackerChance");
	pXML->GetOptionalChildXmlValByName(&m_iDamageToAttacker, L"iDamageToAttacker");
	pXML->GetOptionalChildXmlValByName(&m_iMaxPopulationAllowed, L"iMaxPopulationAllowed", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxPopulationChange, L"iMaxPopulationChange");
	pXML->GetOptionalChildXmlValByName(&m_iInsidiousness, L"iInsidiousness");
	pXML->GetOptionalChildXmlValByName(&m_iInvestigation, L"iInvestigation");
	pXML->GetOptionalChildXmlValByName(&m_iPopulationChange, L"iPopulationChange");
	pXML->GetOptionalChildXmlValByName(&m_iOneTimePopulationPercentLoss, L"iOneTimePopulationPercentLoss");
	pXML->GetOptionalChildXmlValByName(&m_bDamageToAttackerIgnoresArmor, L"bDamageToAttackerIgnoresArmor");
	pXML->GetOptionalChildXmlValByName(&m_bDamageAllAttackers, L"bDamageAllAttackers");
	if (m_bDamageAllAttackers)
	{
		m_bDamageAttackerCapable = true;
	}
	pXML->GetOptionalChildXmlValByName(&m_bQuarantine, L"bQuarantine");

	//Structs

	if(pXML->TryMoveToXmlFirstChild(L"FreePromoTypes"))
	{
		const int iNum = pXML->GetXmlChildrenNumber(L"FreePromoType" );
		m_aFreePromoTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"FreePromoType"))
			{
				int i = 0;
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PromotionType");
					m_aFreePromoTypes[i].ePromotion = (PromotionTypes)pXML->GetInfoClass(szTextVal);
					if (pXML->TryMoveToXmlFirstChild(L"FreePromotionCondition"))
					{
						m_aFreePromoTypes[i].m_pExprFreePromotionCondition = BoolExpr::read(pXML);
						pXML->MoveToXmlParent();
					}
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"FreePromoType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	pXML->SetOptionalVectorWithDelayedResolution(m_aiFreeTraitTypes, L"FreeTraitTypes");
	pXML->SetOptionalVectorWithDelayedResolution(m_aiPrereqInCityBuildings, L"PrereqInCityBuildings");
	pXML->SetOptionalVectorWithDelayedResolution(m_vPrereqNotInCityBuildings, L"PrereqNotInCityBuildings");
	pXML->SetOptionalVectorWithDelayedResolution(m_vPrereqOrBuilding, L"PrereqOrBuildings");
	pXML->SetOptionalVectorWithDelayedResolution(m_vReplacementBuilding, L"ReplacementBuildings");

	if(pXML->TryMoveToXmlFirstChild(L"HealUnitCombatTypes"))
	{
		const int iNum = pXML->GetXmlChildrenNumber(L"HealUnitCombatType" );
		m_aHealUnitCombatTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"HealUnitCombatType"))
			{
				int i = 0;
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"UnitCombatType");
					m_aHealUnitCombatTypes[i].eUnitCombat = (UnitCombatTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aHealUnitCombatTypes[i].iHeal), L"iHeal");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"HealUnitCombatType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"BonusAidModifiers"))
	{
		const int iNum = pXML->GetXmlChildrenNumber(L"BonusAidModifier" );
		m_aBonusAidModifiers.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"BonusAidModifier"))
			{
				int i = 0;
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"BonusType");
					m_aBonusAidModifiers[i].eBonusType = (BonusTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(szTextVal, L"PropertyType");
					m_aBonusAidModifiers[i].ePropertyType = (PropertyTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aBonusAidModifiers[i].iModifier), L"iModifier");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"BonusAidModifier"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"AidRateChanges"))
	{
		const int iNum = pXML->GetXmlChildrenNumber(L"AidRateChange" );
		m_aAidRateChanges.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"AidRateChange"))
			{
				int i = 0;
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"PropertyType");
					m_aAidRateChanges[i].ePropertyType = (PropertyTypes)pXML->GetInfoClass(szTextVal);
					pXML->GetChildXmlValByName(&(m_aAidRateChanges[i].iChange), L"iChange");
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"AidRateChange"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	if(pXML->TryMoveToXmlFirstChild(L"EnabledCivilizationTypes"))
	{
		const int iNum = pXML->GetXmlChildrenNumber(L"EnabledCivilizationType" );
		m_aEnabledCivilizationTypes.resize(iNum); // Important to keep the delayed resolution pointers correct

		if(pXML->TryMoveToXmlFirstChild())
		{
			if (pXML->TryMoveToXmlFirstOfSiblings(L"EnabledCivilizationType"))
			{
				int i = 0;
				do
				{
					pXML->GetChildXmlValByName(szTextVal, L"CivilizationType");
					GC.addDelayedResolution((int*)&(m_aEnabledCivilizationTypes[i].eCivilization), szTextVal);
					i++;
				} while(pXML->TryMoveToXmlNextSibling(L"EnabledCivilizationType"));
			}
			pXML->MoveToXmlParent();
		}
		pXML->MoveToXmlParent();
	}

	pXML->SetOptionalVector(&m_aiUnitCombatRetrainTypes, L"UnitCombatRetrainTypes");
	pXML->SetOptionalVector(&m_aiMayDamageAttackingUnitCombatTypes, L"MayDamageAttackingUnitCombatTypes");
	pXML->SetOptionalVector(&m_aeMapCategoryTypes, L"MapCategoryTypes");

	// int vector utilizing pairing without delayed resolution
	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatRepelModifiers, L"UnitCombatRepelModifiers");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatRepelAgainstModifiers, L"UnitCombatRepelAgainstModifiers");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatDefenseAgainstModifiers, L"UnitCombatDefenseAgainstModifiers");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatProdModifiers, L"UnitCombatProdModifiers");

	pXML->SetOptionalPairVector<UnitCombatModifierArray, UnitCombatTypes, int>(&m_aUnitCombatOngoingTrainingDurations, L"UnitCombatOngoingTrainingDurations");

	pXML->SetOptionalPairVector<PromotionLineModifierArray, PromotionLineTypes, int>(&m_aAfflictionOutbreakLevelChanges, L"AfflictionOutbreakLevelChanges");

	pXML->SetOptionalPairVector<TechModifierArray, TechTypes, int>(&m_aTechOutbreakLevelChanges, L"TechOutbreakLevelChanges");

	m_aTechHappinessChanges.read(pXML, L"TechHappinessChanges");
	m_aTechHealthChanges.read(pXML, L"TechHealthChanges");

	//Arrays
	pXML->SetVariableListTagPair(&m_pabHurry, L"Hurrys", GC.getNumHurryInfos());
	//TB Combat Mods (Buildings) end

	pXML->GetOptionalChildXmlValByName(&m_iMaxGlobalInstances, L"iMaxGlobalInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxTeamInstances, L"iMaxTeamInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iMaxPlayerInstances, L"iMaxPlayerInstances", -1);
	pXML->GetOptionalChildXmlValByName(&m_iExtraPlayerInstances, L"iExtraPlayerInstances", 0);

	pXML->SetVariableListTagPair(&m_piVictoryThreshold, L"VictoryThresholds",  GC.getNumVictoryInfos());

	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iFreeBuilding, L"FreeBuilding");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iFreeAreaBuilding, L"FreeAreaBuilding");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iProductionContinueBuilding, L"ProductionContinueBuilding");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iPrereqAnyoneBuilding, L"PrereqAnyoneBuilding");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iExtendsBuilding, L"ExtendsBuilding");
	pXML->GetOptionalTypeEnumWithDelayedResolution(m_iObsoletesToBuilding, L"ObsoletesToBuilding");

	m_aBuildingHappinessChanges.readWithDelayedResolution(pXML, L"BuildingHappinessChanges");
	m_aUnitProductionModifier.readWithDelayedResolution(pXML, L"UnitProductionModifiers");
	m_aBuildingProductionModifier.readWithDelayedResolution(pXML, L"BuildingProductionModifiers");
	m_aGlobalBuildingProductionModifier.readWithDelayedResolution(pXML, L"GlobalBuildingProductionModifiers");
	m_aPrereqNumOfBuilding.readWithDelayedResolution(pXML, L"PrereqAmountBuildings");
	m_aGlobalBuildingCostModifier.readWithDelayedResolution(pXML, L"GlobalBuildingCostModifiers");

	return true;
}

bool CvBuildingInfo::readPass2(CvXMLLoadUtility* pXML)
{
	if (!CvInfoBase::read(pXML))
	{
		return false;
	}

	CvString szTextVal;

	m_aGlobalBuildingCommerceChanges.clear();
	if (pXML->TryMoveToXmlFirstChild(L"GlobalBuildingExtraCommerces"))
	{
		const int iNumChildren = pXML->GetXmlChildrenNumber();

		if (0 < iNumChildren)
		{
			if (pXML->TryMoveToXmlFirstChild())
			{
				for (int j = 0; j < iNumChildren; ++j)
				{
					if (pXML->GetChildXmlVal(szTextVal))
					{
						BuildingCommerceChange kChange;
						kChange.eBuilding = (BuildingTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(szTextVal);
						kChange.eCommerce = (CommerceTypes)pXML->GetInfoClass(szTextVal);
						pXML->GetNextXmlVal(&kChange.iChange);
						m_aGlobalBuildingCommerceChanges.push_back(kChange);

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

	return true;
}

bool CvBuildingInfo::readPass3()
{
	m_pbPrereqOrCivics = new bool[GC.getNumCivicInfos()];
	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		m_pbPrereqOrCivics[iI] = false;
	}
	if (!m_abPrereqOrCivicsforPass3.empty() && !m_aszPrereqOrCivicsforPass3.empty())
	{
		const int iNumLoad = m_abPrereqOrCivicsforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			//FAssertMsg(GC.getInfoTypeForString(m_aszPrereqOrCivicsforPass3[iI]) >= 0, L"Warning, about to leak memory in CvBuildingInfo::readPass3");
			const int iTempIndex = GC.getInfoTypeForString(m_aszPrereqOrCivicsforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_pbPrereqOrCivics[iTempIndex] = m_abPrereqOrCivicsforPass3[iI];
		}
		m_aszPrereqOrCivicsforPass3.clear();
		m_abPrereqOrCivicsforPass3.clear();
	}

	m_pbPrereqAndCivics = new bool[GC.getNumCivicInfos()];
    for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		m_pbPrereqAndCivics[iI] = false;
	}
	if (!m_abPrereqAndCivicsforPass3.empty() && !m_aszPrereqAndCivicsforPass3.empty())
	{
		const int iNumLoad = m_abPrereqAndCivicsforPass3.size();
		for(int iI = 0; iI < iNumLoad; iI++)
		{
			//FAssertMsg(GC.getInfoTypeForString(m_aszPrereqAndCivicsforPass3[iI]) >= 0, L"Warning, about to leak memory in CvBuildingInfo::readPass3");
			const int iTempIndex = GC.getInfoTypeForString(m_aszPrereqAndCivicsforPass3[iI]);
			if (iTempIndex >= 0 && iTempIndex < GC.getNumCivicInfos())
				m_pbPrereqAndCivics[iTempIndex] = m_abPrereqAndCivicsforPass3[iI];
		}
		m_aszPrereqAndCivicsforPass3.clear();
		m_abPrereqAndCivicsforPass3.clear();
	}

	if (m_aszExtraXMLforPass3.size() < 1)
	{
		FAssert(false);
		return false;
	}
	m_iGreatPeopleUnitType = GC.getInfoTypeForString(m_aszExtraXMLforPass3[0]);

	m_ePropertySpawnUnit = (UnitTypes) GC.getInfoTypeForString(m_aszExtraXMLforPass3[1]);

	m_aszExtraXMLforPass3.clear();

	int iCount = getNumReplacementBuilding();
	if (iCount > 0)
	{
		// Toffer - Prune self reference, to make the code XML idiot proof.
		//	A building was once set to replace itself, it caused an infinite loop in the canBuild logic used for MODDEROPTION_HIDE_REPLACED_BUILDINGS.
		//	Instead of doing a self reference check in all loops for this vector I thought it more clean to just prune it here.
		const int iId = GC.getInfoTypeForString(getType());

		std::vector<int>::iterator itr = find(m_vReplacementBuilding.begin(), m_vReplacementBuilding.end(), iId);
		while (itr != m_vReplacementBuilding.end())
		{
			FErrorMsg(CvString::format("%s is set to replace itself!!", getType()).c_str())
			m_vReplacementBuilding.erase(itr);
			iCount--;
			itr = find(m_vReplacementBuilding.begin(), m_vReplacementBuilding.end(), iId);
		}

		// Toffer - As good a place as any to make this derived cache
		for (int i = 0; i < iCount; i++)
		{
			GC.getBuildingInfo((BuildingTypes)getReplacementBuilding(i)).setReplacedBuilding(iId);
		}
	}
	return true;
}

void CvBuildingInfo::copyNonDefaults(CvBuildingInfo* pClassInfo)
{
	const bool bDefault = false;
	const int iDefault = 0;
	const int iTextDefault = -1;
	const float fDefault = 0.0f;
	const CvString cDefault = CvString::format("").GetCString();

	if ( getArtDefineTag() == cDefault ) // "ArtDefineTag"
	{
		m_szArtDefineTag = pClassInfo->getArtDefineTag();
		setNotShowInCity();
	}
	if ( getMovieDefineTag() == cDefault ) // "ArtDefineTag"
	{
		m_szMovieDefineTag = pClassInfo->getMovieDefineTag();
	}

	CvHotkeyInfo::copyNonDefaults(pClassInfo);

	if (getSpecialBuilding() == iTextDefault) m_eSpecialBuilding = pClassInfo->getSpecialBuilding();
	if (getAdvisorType() == iTextDefault) m_iAdvisorType = pClassInfo->getAdvisorType();

	if (isNoLimit() == bDefault) m_bNoLimit = pClassInfo->isNoLimit();

	if (getPrereqGameOption() == iTextDefault) m_iPrereqGameOption = pClassInfo->getPrereqGameOption();
	if (getNotGameOption() == iTextDefault) m_iNotGameOption = pClassInfo->getNotGameOption();
	if (getHolyCity() == iTextDefault) m_iHolyCity = pClassInfo->getHolyCity();
	if (getReligionType() == iTextDefault) m_iReligionType = pClassInfo->getReligionType();
	if (getPrereqStateReligion() == iTextDefault) m_iStateReligion = pClassInfo->getPrereqStateReligion();
	if (getPrereqReligion() == iTextDefault) m_iPrereqReligion = pClassInfo->getPrereqReligion();
	if (getPrereqCorporation() == iTextDefault) m_iPrereqCorporation = pClassInfo->getPrereqCorporation();
	if (getFoundsCorporation() == iTextDefault) m_iFoundsCorporation = pClassInfo->getFoundsCorporation();
	if (getGlobalReligionCommerce() == iTextDefault) m_iGlobalReligionCommerce = pClassInfo->getGlobalReligionCommerce();
	if (getGlobalCorporationCommerce() == iTextDefault) m_iGlobalCorporationCommerce = pClassInfo->getGlobalCorporationCommerce();
	if (getVictoryPrereq() == iTextDefault) m_iVictoryPrereq = pClassInfo->getVictoryPrereq();
	if (getFreeStartEra() == iTextDefault) m_iFreeStartEra = pClassInfo->getFreeStartEra();
	if (getMaxStartEra() == iTextDefault) m_iMaxStartEra = pClassInfo->getMaxStartEra();
	if (getObsoleteTech() == iTextDefault) m_iObsoleteTech = pClassInfo->getObsoleteTech();
	if (getPrereqAndTech() == iTextDefault) m_iPrereqAndTech = pClassInfo->getPrereqAndTech();
	if (getPrereqVicinityBonus() == iTextDefault) m_iPrereqVicinityBonus = pClassInfo->getPrereqVicinityBonus();
	if (getPrereqRawVicinityBonus() == iTextDefault) m_iPrereqRawVicinityBonus = pClassInfo->getPrereqRawVicinityBonus();

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_piPrereqAndTechs, pClassInfo->getPrereqAndTechs());

	if (getPrereqAndBonus() == NO_BONUS) m_iPrereqAndBonus = pClassInfo->getPrereqAndBonus();

	if (getNoBonus() == iTextDefault) m_iNoBonus = pClassInfo->getNoBonus();
	if (getPowerBonus() == iTextDefault) m_iPowerBonus = pClassInfo->getPowerBonus();
	if (getFreeBonus() == iTextDefault) m_iFreeBonus = pClassInfo->getFreeBonus();
	if (getNumFreeBonuses() == iDefault) m_iNumFreeBonuses = pClassInfo->getNumFreeBonuses();

	if (m_aExtraFreeBonuses.empty())
	{
		int iNum = pClassInfo->getNumExtraFreeBonuses();
		for (int i = 0; i < iNum; i++)
		{
			m_aExtraFreeBonuses.push_back(std::pair<BonusTypes,int>(pClassInfo->getExtraFreeBonus(i),pClassInfo->getExtraFreeBonusNum(i)));
		}
	}

	if (getFreePromotion() == iTextDefault) m_iFreePromotion = pClassInfo->getFreePromotion();
	if (getCivicOption() == iTextDefault) m_iCivicOption = pClassInfo->getCivicOption();
	if (getGreatPeopleUnitType() == iTextDefault) m_iGreatPeopleUnitType = pClassInfo->getGreatPeopleUnitType();
	if (getVoteSourceType() == iTextDefault) m_iVoteSourceType = pClassInfo->getVoteSourceType();

	if (getGreatPeopleRateChange() == iDefault) m_iGreatPeopleRateChange = pClassInfo->getGreatPeopleRateChange();
	if (isTeamShare() == bDefault) m_bTeamShare = pClassInfo->isTeamShare();
	if (isAutoBuild() == bDefault) m_bAutoBuild = pClassInfo->isAutoBuild();
	if (isWater() == bDefault) m_bWater = pClassInfo->isWater();
	if (isRiver() == bDefault) m_bRiver = pClassInfo->isRiver();
	if (isPower() == bDefault) m_bPower = pClassInfo->isPower();
	if (isOrbital() == bDefault) m_bOrbital = pClassInfo->isOrbital();
	if (isOrbitalInfrastructure() == bDefault) m_bOrbitalInfrastructure = pClassInfo->isOrbitalInfrastructure();
	if (isNoHolyCity() == bDefault) m_bNoHolyCity = pClassInfo->isNoHolyCity();
	if (isAreaBorderObstacle() == bDefault) m_bAreaBorderObstacle = pClassInfo->isAreaBorderObstacle();
	if (isForceTeamVoteEligible() == bDefault) m_bForceTeamVoteEligible = pClassInfo->isForceTeamVoteEligible();
	if (isCapital() == bDefault) m_bCapital = pClassInfo->isCapital();
	if (isGovernmentCenter() == bDefault) m_bGovernmentCenter = pClassInfo->isGovernmentCenter();
	if (isGoldenAge () == bDefault) m_bGoldenAge = pClassInfo->isGoldenAge ();
	if (isAllowsNukes() == bDefault) m_bAllowsNukes = pClassInfo->isAllowsNukes();
	if (isMapCentering() == bDefault) m_bMapCentering = pClassInfo->isMapCentering();
	if (isNoUnhappiness() == bDefault) m_bNoUnhappiness = pClassInfo->isNoUnhappiness();
	if (isNoUnhealthyPopulation() == bDefault) m_bNoUnhealthyPopulation = pClassInfo->isNoUnhealthyPopulation();
	if (isBuildingOnlyHealthy() == bDefault) m_bBuildingOnlyHealthy = pClassInfo->isBuildingOnlyHealthy();
	if (isNeverCapture() == bDefault) m_bNeverCapture = pClassInfo->isNeverCapture();
	if (isNukeImmune() == bDefault) m_bNukeImmune = pClassInfo->isNukeImmune();
	if (isCenterInCity() == bDefault) m_bCenterInCity = pClassInfo->isCenterInCity();
	if (needStateReligionInCity() == bDefault) m_bStateReligionInCity = pClassInfo->needStateReligionInCity();

	if (getAIWeight() == iDefault) m_iAIWeight = pClassInfo->getAIWeight();
	if (m_iProductionCost == iTextDefault) m_iProductionCost = pClassInfo->getProductionCost();
	if (getHurryCostModifier() == iDefault) m_iHurryCostModifier = pClassInfo->getHurryCostModifier();
	if (getHurryAngerModifier() == iDefault) m_iHurryAngerModifier = pClassInfo->getHurryAngerModifier();
	if (getMinAreaSize() == iDefault) m_iMinAreaSize = pClassInfo->getMinAreaSize();
	if (getConquestProbability() == 50) m_iConquestProbability = pClassInfo->getConquestProbability();
	if (getNumCitiesPrereq() == iDefault) m_iNumCitiesPrereq = pClassInfo->getNumCitiesPrereq();
	if (getNumTeamsPrereq() == iDefault) m_iNumTeamsPrereq = pClassInfo->getNumTeamsPrereq();
	if (getUnitLevelPrereq() == iDefault) m_iUnitLevelPrereq = pClassInfo->getUnitLevelPrereq();
	if (getMinLatitude() == iDefault) m_iMinLatitude = pClassInfo->getMinLatitude();
	if (getMaxLatitude() == 90) m_iMaxLatitude = pClassInfo->getMaxLatitude();
	if (getGreatPeopleRateModifier() == iDefault) m_iGreatPeopleRateModifier = pClassInfo->getGreatPeopleRateModifier();
	if (getGreatGeneralRateModifier() == iDefault) m_iGreatGeneralRateModifier = pClassInfo->getGreatGeneralRateModifier();
	if (getDomesticGreatGeneralRateModifier() == iDefault) m_iDomesticGreatGeneralRateModifier = pClassInfo->getDomesticGreatGeneralRateModifier();
	if (getGlobalGreatPeopleRateModifier() == iDefault) m_iGlobalGreatPeopleRateModifier = pClassInfo->getGlobalGreatPeopleRateModifier();
	if (getAnarchyModifier() == iDefault) m_iAnarchyModifier = pClassInfo->getAnarchyModifier();
	if (getGoldenAgeModifier() == iDefault) m_iGoldenAgeModifier = pClassInfo->getGoldenAgeModifier();
	if (getGlobalHurryModifier() == iDefault) m_iGlobalHurryModifier = pClassInfo->getGlobalHurryModifier();
	if (getFreeExperience() == iDefault) m_iFreeExperience = pClassInfo->getFreeExperience();
	if (getGlobalFreeExperience() == iDefault) m_iGlobalFreeExperience = pClassInfo->getGlobalFreeExperience();
	if (getFoodKept() == iDefault) m_iFoodKept = pClassInfo->getFoodKept();
	if (getAirlift() == iDefault) m_iAirlift = pClassInfo->getAirlift();
	if (getAirModifier() == iDefault) m_iAirModifier = pClassInfo->getAirModifier();
	if (getAirUnitCapacity() == iDefault) m_iAirUnitCapacity = pClassInfo->getAirUnitCapacity();
	if (getNukeModifier() == iDefault) m_iNukeModifier = pClassInfo->getNukeModifier();
	if (getNukeExplosionRand() == iDefault) m_iNukeExplosionRand = pClassInfo->getNukeExplosionRand();
	if (getFreeSpecialist() == iDefault) m_iFreeSpecialist = pClassInfo->getFreeSpecialist();
	if (getAreaFreeSpecialist() == iDefault) m_iAreaFreeSpecialist = pClassInfo->getAreaFreeSpecialist();
	if (getGlobalFreeSpecialist() == iDefault) m_iGlobalFreeSpecialist = pClassInfo->getGlobalFreeSpecialist();
	if (getMaintenanceModifier() == iDefault) m_iMaintenanceModifier = pClassInfo->getMaintenanceModifier();
	if (getWarWearinessModifier() == iDefault) m_iWarWearinessModifier = pClassInfo->getWarWearinessModifier();
	if (getGlobalWarWearinessModifier() == iDefault) m_iGlobalWarWearinessModifier = pClassInfo->getGlobalWarWearinessModifier();
	if (getEnemyWarWearinessModifier() == iDefault) m_iEnemyWarWearinessModifier = pClassInfo->getEnemyWarWearinessModifier();
	if (getHealRateChange() == iDefault) m_iHealRateChange = pClassInfo->getHealRateChange();
	if (getHealth() == iDefault) m_iHealth = pClassInfo->getHealth();
	if (getAreaHealth() == iDefault) m_iAreaHealth = pClassInfo->getAreaHealth();
	if (getGlobalHealth() == iDefault) m_iGlobalHealth = pClassInfo->getGlobalHealth();
	if (getHappiness() == iDefault) m_iHappiness = pClassInfo->getHappiness();
	if (getAreaHappiness() == iDefault) m_iAreaHappiness = pClassInfo->getAreaHappiness();
	if (getGlobalHappiness() == iDefault) m_iGlobalHappiness = pClassInfo->getGlobalHappiness();
	if (getStateReligionHappiness() == iDefault) m_iStateReligionHappiness = pClassInfo->getStateReligionHappiness();
	if (getWorkerSpeedModifier() == iDefault) m_iWorkerSpeedModifier = pClassInfo->getWorkerSpeedModifier();
	if (getMilitaryProductionModifier() == iDefault) m_iMilitaryProductionModifier = pClassInfo->getMilitaryProductionModifier();
	if (getSpaceProductionModifier() == iDefault) m_iSpaceProductionModifier = pClassInfo->getSpaceProductionModifier();
	if (getGlobalSpaceProductionModifier() == iDefault) m_iGlobalSpaceProductionModifier = pClassInfo->getGlobalSpaceProductionModifier();
	if (getTradeRoutes() == iDefault) m_iTradeRoutes = pClassInfo->getTradeRoutes();
	if (getCoastalTradeRoutes() == iDefault) m_iCoastalTradeRoutes = pClassInfo->getCoastalTradeRoutes();
	if (getGlobalTradeRoutes() == iDefault) m_iGlobalTradeRoutes = pClassInfo->getGlobalTradeRoutes();
	if (getTradeRouteModifier() == iDefault) m_iTradeRouteModifier = pClassInfo->getTradeRouteModifier();
	if (getForeignTradeRouteModifier() == iDefault) m_iForeignTradeRouteModifier = pClassInfo->getForeignTradeRouteModifier();
	if (getGlobalPopulationChange() == iDefault) m_iGlobalPopulationChange = pClassInfo->getGlobalPopulationChange();
	if (getFreeTechs() == iDefault) m_iFreeTechs = pClassInfo->getFreeTechs();
	if (getFreeSpecialTech() == NO_TECH) m_eFreeSpecialTech = pClassInfo->getFreeSpecialTech();
	if (getDefenseModifier() == iDefault) m_iDefenseModifier = pClassInfo->getDefenseModifier();
	if (getBombardDefenseModifier() == iDefault) m_iBombardDefenseModifier = pClassInfo->getBombardDefenseModifier();
	if (getAllCityDefenseModifier() == iDefault) m_iAllCityDefenseModifier = pClassInfo->getAllCityDefenseModifier();
	if (getEspionageDefenseModifier() == iDefault) m_iEspionageDefenseModifier = pClassInfo->getEspionageDefenseModifier();
	if (m_iAssetValue == iDefault) m_iAssetValue = pClassInfo->m_iAssetValue;
	if (m_iPowerValue == iDefault) m_iPowerValue = pClassInfo->m_iPowerValue;

	if (getUnitUpgradePriceModifier() == iDefault) m_iUnitUpgradePriceModifier = pClassInfo->getUnitUpgradePriceModifier();
	if (getRevIdxLocal() == iDefault) m_iRevIdxLocal = pClassInfo->getRevIdxLocal();
	if (getRevIdxNational() == iDefault) m_iRevIdxNational = pClassInfo->getRevIdxNational();
	if (getRevIdxDistanceModifier() == iDefault) m_iRevIdxDistanceModifier = pClassInfo->getRevIdxDistanceModifier();

	if (getMaxPopAllowed() == -1) m_iMaxPopAllowed = pClassInfo->getMaxPopAllowed();

	if (getVisibilityPriority() == fDefault) m_fVisibilityPriority = pClassInfo->getVisibilityPriority();

	for ( int j = 0; j < NUM_YIELD_TYPES; j++)
	{
		if ( getSeaPlotYieldChange(j) == iDefault && pClassInfo->getSeaPlotYieldChange(j) != iDefault)
		{
			if ( NULL == m_piSeaPlotYieldChange )
			{
				CvXMLLoadUtility::InitList(&m_piSeaPlotYieldChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piSeaPlotYieldChange[j] = pClassInfo->getSeaPlotYieldChange(j);
		}
		if ( getRiverPlotYieldChange(j) == iDefault && pClassInfo->getRiverPlotYieldChange(j) != iDefault)
		{
			if ( NULL == m_piRiverPlotYieldChange )
			{
				CvXMLLoadUtility::InitList(&m_piRiverPlotYieldChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piRiverPlotYieldChange[j] = pClassInfo->getRiverPlotYieldChange(j);
		}
		if ( getGlobalSeaPlotYieldChange(j) == iDefault && pClassInfo->getGlobalSeaPlotYieldChange(j) != iDefault)
		{
			if ( NULL == m_piGlobalSeaPlotYieldChange )
			{
				CvXMLLoadUtility::InitList(&m_piGlobalSeaPlotYieldChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piGlobalSeaPlotYieldChange[j] = pClassInfo->getGlobalSeaPlotYieldChange(j);
		}
		if ( getYieldChange(j) == iDefault && pClassInfo->getYieldChange(j) != iDefault)
		{
			if ( NULL == m_piYieldChange )
			{
				CvXMLLoadUtility::InitList(&m_piYieldChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piYieldChange[j] = pClassInfo->getYieldChange(j);
		}
		if ( getYieldPerPopChange(j) == iDefault && pClassInfo->getYieldPerPopChange(j) != iDefault)
		{
			if ( NULL == m_piYieldPerPopChange )
			{
				CvXMLLoadUtility::InitList(&m_piYieldPerPopChange,NUM_YIELD_TYPES,iDefault);
			}
			m_piYieldPerPopChange[j] = pClassInfo->getYieldPerPopChange(j);
		}
		if ( getYieldModifier(j) == iDefault && pClassInfo->getYieldModifier(j) != iDefault)
		{
			if ( NULL == m_piYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piYieldModifier[j] = pClassInfo->getYieldModifier(j);
		}
		if ( getPowerYieldModifier(j) == iDefault && pClassInfo->getPowerYieldModifier(j) != iDefault)
		{
			if ( NULL == m_piPowerYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piPowerYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piPowerYieldModifier[j] = pClassInfo->getPowerYieldModifier(j);
		}
		if ( getAreaYieldModifier(j) == iDefault && pClassInfo->getAreaYieldModifier(j) != iDefault)
		{
			if ( NULL == m_piAreaYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piAreaYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piAreaYieldModifier[j] = pClassInfo->getAreaYieldModifier(j);
		}
		if ( getGlobalYieldModifier(j) == iDefault && pClassInfo->getGlobalYieldModifier(j) != iDefault)
		{
			if ( NULL == m_piGlobalYieldModifier )
			{
				CvXMLLoadUtility::InitList(&m_piGlobalYieldModifier,NUM_YIELD_TYPES,iDefault);
			}
			m_piGlobalYieldModifier[j] = pClassInfo->getGlobalYieldModifier(j);
		}
	}

	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( getCommerceChange(j) == iDefault && pClassInfo->getCommerceChange(j) != iDefault)
		{
			if ( NULL == m_piCommerceChange )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceChange,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceChange[j] = pClassInfo->getCommerceChange(j);
		}
		if ( getCommercePerPopChange(j) == iDefault && pClassInfo->getCommercePerPopChange(j) != iDefault)
		{
			if ( NULL == m_piCommercePerPopChange )
			{
				CvXMLLoadUtility::InitList(&m_piCommercePerPopChange,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommercePerPopChange[j] = pClassInfo->getCommercePerPopChange(j);
		}
		if ( getCommerceChangeDoubleTime(j) == iDefault && pClassInfo->getCommerceChangeDoubleTime(j) != iDefault)
		{
			if ( NULL == m_piCommerceChangeDoubleTime )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceChangeDoubleTime,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceChangeDoubleTime[j] = pClassInfo->getCommerceChangeDoubleTime(j);
		}
		if ( getCommerceModifier(j) == iDefault && pClassInfo->getCommerceModifier(j) != iDefault)
		{
			if ( NULL == m_piCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceModifier[j] = pClassInfo->getCommerceModifier(j);
		}
		if ( getGlobalCommerceModifier(j) == iDefault && pClassInfo->getGlobalCommerceModifier(j) != iDefault)
		{
			if ( NULL == m_piGlobalCommerceModifier )
			{
				CvXMLLoadUtility::InitList(&m_piGlobalCommerceModifier,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piGlobalCommerceModifier[j] = pClassInfo->getGlobalCommerceModifier(j);
		}
		if ( getSpecialistExtraCommerce(j) == iDefault && pClassInfo->getSpecialistExtraCommerce(j) != iDefault)
		{
			if ( NULL == m_piSpecialistExtraCommerce )
			{
				CvXMLLoadUtility::InitList(&m_piSpecialistExtraCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piSpecialistExtraCommerce[j] = pClassInfo->getSpecialistExtraCommerce(j);
		}
		if ( getStateReligionCommerce(j) == iDefault && pClassInfo->getStateReligionCommerce(j) != iDefault)
		{
			if ( NULL == m_piStateReligionCommerce )
			{
				CvXMLLoadUtility::InitList(&m_piStateReligionCommerce,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piStateReligionCommerce[j] = pClassInfo->getStateReligionCommerce(j);
		}
		if ( getCommerceHappiness(j) == iDefault && pClassInfo->getCommerceHappiness(j) != iDefault)
		{
			if ( NULL == m_piCommerceHappiness )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceHappiness,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceHappiness[j] = pClassInfo->getCommerceHappiness(j);
		}
		if ( isCommerceFlexible(j) == bDefault && pClassInfo->isCommerceFlexible(j) != bDefault)
		{
			if ( NULL == m_pbCommerceFlexible )
			{
				CvXMLLoadUtility::InitList(&m_pbCommerceFlexible,NUM_COMMERCE_TYPES,bDefault);
			}
			m_pbCommerceFlexible[j] = pClassInfo->isCommerceFlexible(j);
		}
		if ( isCommerceChangeOriginalOwner(j) == bDefault && pClassInfo->isCommerceChangeOriginalOwner(j) != bDefault)
		{
			if ( NULL == m_pbCommerceChangeOriginalOwner )
			{
				CvXMLLoadUtility::InitList(&m_pbCommerceChangeOriginalOwner,NUM_COMMERCE_TYPES,bDefault);
			}
			m_pbCommerceChangeOriginalOwner[j] = pClassInfo->isCommerceChangeOriginalOwner(j);
		}
	}

	for ( int j = 0; j < GC.getNumReligionInfos(); j++)
	{
		if ( getReligionChange(j) == iDefault && pClassInfo->getReligionChange(j) != iDefault)
		{
			if ( NULL == m_piReligionChange )
			{
				CvXMLLoadUtility::InitList(&m_piReligionChange,GC.getNumReligionInfos(),iDefault);
			}
			m_piReligionChange[j] = pClassInfo->getReligionChange(j);
		}
	}

	for ( int j = 0; j < GC.getNumSpecialistInfos(); j++)
	{
		if ( getSpecialistCount(j) == iDefault && pClassInfo->getSpecialistCount(j) != iDefault)
		{
			if ( NULL == m_piSpecialistCount )
			{
				CvXMLLoadUtility::InitList(&m_piSpecialistCount,GC.getNumSpecialistInfos(),iDefault);
			}
			m_piSpecialistCount[j] = pClassInfo->getSpecialistCount(j);
		}
		if ( getFreeSpecialistCount(j) == iDefault && pClassInfo->getFreeSpecialistCount(j) != iDefault)
		{
			if ( NULL == m_piFreeSpecialistCount )
			{
				CvXMLLoadUtility::InitList(&m_piFreeSpecialistCount,GC.getNumSpecialistInfos(),iDefault);
			}
			m_piFreeSpecialistCount[j] = pClassInfo->getFreeSpecialistCount(j);
		}
	}

	if (getConstructSound() == cDefault) m_szConstructSound = pClassInfo->getConstructSound();

	for ( int j = 0; j < GC.getNumBonusInfos(); j++)
	{
		if ( getBonusHealthChanges(j) == iDefault && pClassInfo->getBonusHealthChanges(j) != iDefault)
		{
			if ( NULL == m_piBonusHealthChanges )
			{
				CvXMLLoadUtility::InitList(&m_piBonusHealthChanges,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusHealthChanges[j] = pClassInfo->getBonusHealthChanges(j);
		}
		if ( getBonusHappinessChanges(j) == iDefault && pClassInfo->getBonusHappinessChanges(j) != iDefault)
		{
			if ( NULL == m_piBonusHappinessChanges )
			{
				CvXMLLoadUtility::InitList(&m_piBonusHappinessChanges,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusHappinessChanges[j] = pClassInfo->getBonusHappinessChanges(j);
		}
		if ( getBonusProductionModifier(j) == iDefault && pClassInfo->getBonusProductionModifier(j) != iDefault )
		{
			if ( NULL == m_piBonusProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_piBonusProductionModifier,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusProductionModifier[j] = pClassInfo->getBonusProductionModifier(j);
		}
	}
	for ( int j = 0; j < GC.getNumUnitCombatInfos(); j++)
	{
		if ( getUnitCombatFreeExperience(j) == iDefault && pClassInfo->getUnitCombatFreeExperience(j) != iDefault)
		{
			if ( NULL == m_piUnitCombatFreeExperience)
			{
				CvXMLLoadUtility::InitList(&m_piUnitCombatFreeExperience,GC.getNumUnitCombatInfos(),iDefault);
			}
			m_piUnitCombatFreeExperience[j] = pClassInfo->getUnitCombatFreeExperience(j);
		}
	}
	for ( int j = 0; j < NUM_DOMAIN_TYPES; j++)
	{
		if ( getDomainFreeExperience(j) == iDefault && pClassInfo->getDomainFreeExperience(j) != iDefault)
		{
			if ( NULL == m_piDomainFreeExperience )
			{
				CvXMLLoadUtility::InitList(&m_piDomainFreeExperience,NUM_DOMAIN_TYPES,iDefault);
			}
			m_piDomainFreeExperience[j] = pClassInfo->getDomainFreeExperience(j);
		}
		if ( getDomainProductionModifier(j) == iDefault && pClassInfo->getDomainProductionModifier(j) != iDefault)
		{
			if ( NULL == m_piDomainProductionModifier )
			{
				CvXMLLoadUtility::InitList(&m_piDomainProductionModifier,NUM_DOMAIN_TYPES,iDefault);
			}
			m_piDomainProductionModifier[j] = pClassInfo->getDomainProductionModifier(j);
		}
	}

	for ( int i = 0; i < GC.getNumSpecialistInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getSpecialistYieldChange(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getSpecialistYieldChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiSpecialistYieldChange == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistYieldChange, GC.getNumSpecialistInfos());
					}
					if ( m_ppaiSpecialistYieldChange[i] == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistYieldChange[i], NUM_YIELD_TYPES);
					}

					m_ppaiSpecialistYieldChange[i][j] = iChange;
				}
			}

			if ( getLocalSpecialistYieldChange(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getLocalSpecialistYieldChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiLocalSpecialistYieldChange == NULL )
					{
						CvXMLLoadUtility::InitList(&m_ppaiLocalSpecialistYieldChange, GC.getNumSpecialistInfos());
					}
					if (m_ppaiLocalSpecialistYieldChange[i] == NULL)
					{
						CvXMLLoadUtility::InitList(&m_ppaiLocalSpecialistYieldChange[i], NUM_YIELD_TYPES);
					}

					m_ppaiLocalSpecialistYieldChange[i][j] = iChange;
				}
			}
		}
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if ( getSpecialistCommerceChange(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getSpecialistCommerceChange(i, j);

				if ( iChange != iDefault )
				{
					if (m_ppaiSpecialistCommerceChange == NULL)
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistCommerceChange, GC.getNumSpecialistInfos());
					}
					if (m_ppaiSpecialistCommerceChange[i] == NULL)
					{
						CvXMLLoadUtility::InitList(&m_ppaiSpecialistCommerceChange[i], NUM_COMMERCE_TYPES);
					}

					m_ppaiSpecialistCommerceChange[i][j] = iChange;
				}
			}

			if ( getLocalSpecialistCommerceChange(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getLocalSpecialistCommerceChange(i, j);

				if ( iChange != iDefault )
				{
					if (m_ppaiLocalSpecialistCommerceChange == NULL)
					{
						CvXMLLoadUtility::InitList(&m_ppaiLocalSpecialistCommerceChange, GC.getNumSpecialistInfos());
					}
					if (m_ppaiLocalSpecialistCommerceChange[i] == NULL)
					{
						CvXMLLoadUtility::InitList(&m_ppaiLocalSpecialistCommerceChange[i], NUM_COMMERCE_TYPES);
					}

					m_ppaiLocalSpecialistCommerceChange[i][j] = iChange;
				}
			}
		}
	}
	for ( int i = 0; i < GC.getNumBonusInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getBonusYieldModifier(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getBonusYieldModifier(i, j);

				if ( iChange != iDefault )
				{
					if (m_ppaiBonusYieldModifier == NULL)
					{
						CvXMLLoadUtility::InitList(&m_ppaiBonusYieldModifier, GC.getNumBonusInfos());
					}
					if (m_ppaiBonusYieldModifier[i] == NULL)
					{
						CvXMLLoadUtility::InitList(&m_ppaiBonusYieldModifier[i], NUM_YIELD_TYPES);
					}

					m_ppaiBonusYieldModifier[i][j] = iChange;
				}
			}
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
	for ( int j = 0; j < GC.getNumImprovementInfos(); j++)
	{
		if ( getImprovementFreeSpecialist(j) == iDefault && pClassInfo->getImprovementFreeSpecialist(j) != iDefault)
		{
			if ( NULL == m_piImprovementFreeSpecialist )
			{
				CvXMLLoadUtility::InitList(&m_piImprovementFreeSpecialist,GC.getNumImprovementInfos(),iDefault);
			}
			m_piImprovementFreeSpecialist[j] = pClassInfo->getImprovementFreeSpecialist(j);
		}
	}

	if (isApplyFreePromotionOnMove() == bDefault) m_bApplyFreePromotionOnMove = pClassInfo->isApplyFreePromotionOnMove();
	if (isNoEnemyPillagingIncome() == bDefault) m_bNoEnemyPillagingIncome = pClassInfo->isNoEnemyPillagingIncome();
	if (isProvidesFreshWater() == bDefault) m_bProvidesFreshWater = pClassInfo->isProvidesFreshWater();
	if (isForceAllTradeRoutes() == bDefault) m_bForceAllTradeRoutes = pClassInfo->isForceAllTradeRoutes();
	if (getPillageGoldModifier() == iDefault) m_iPillageGoldModifier = pClassInfo->getPillageGoldModifier();
	if (getWorldTradeRoutes() == iDefault) m_iWorldTradeRoutes = pClassInfo->getWorldTradeRoutes();
	if (isForceNoPrereqScaling() == bDefault) m_bForceNoPrereqScaling = pClassInfo->isForceNoPrereqScaling();
	if (isPrereqWar() == bDefault) m_bPrereqWar = pClassInfo->isPrereqWar();
	if (isRequiresActiveCivics() == bDefault) m_bRequiresActiveCivics = pClassInfo->isRequiresActiveCivics();
	if (isZoneOfControl() == bDefault) m_bZoneOfControl = pClassInfo->isZoneOfControl();
	if (isPrereqPower() == bDefault) m_bPrereqPower = pClassInfo->isPrereqPower();
	if (getGlobalPopulationgrowthratepercentage() == iDefault) m_iGlobalPopulationgrowthratepercentage = pClassInfo->getGlobalPopulationgrowthratepercentage();
	if (getPopulationgrowthratepercentage() == iDefault) m_iPopulationgrowthratepercentage = pClassInfo->getPopulationgrowthratepercentage();
	if (getFreePromotion_2() == iTextDefault) m_iFreePromotion_2 = pClassInfo->getFreePromotion_2();
	if (getFreePromotion_3() == iTextDefault) m_iFreePromotion_3 = pClassInfo->getFreePromotion_3();
	if (getLineOfSight() == iDefault) m_iLineOfSight = pClassInfo->getLineOfSight();
	if (getInflationModifier() == iDefault) m_iInflationModifier = pClassInfo->getInflationModifier();
	if (getInvasionChance() == iDefault) m_iInvasionChance = pClassInfo->getInvasionChance();
	if (getAdjacentDamagePercent() == iDefault) m_iAdjacentDamagePercent = pClassInfo->getAdjacentDamagePercent();
	if (getPrereqPopulation() == iDefault) m_iPrereqPopulation = pClassInfo->getPrereqPopulation();
	if (getPrereqCultureLevel() == NO_CULTURELEVEL) m_iPrereqCultureLevel = pClassInfo->getPrereqCultureLevel();
	if (getWorkableRadius() == iDefault) m_iWorkableRadius = pClassInfo->getWorkableRadius();
	if (getOccupationTimeModifier() == iDefault) m_iOccupationTimeModifier = pClassInfo->getOccupationTimeModifier();
	if (m_iNoEntryDefenseLevel == iDefault) m_iNoEntryDefenseLevel = pClassInfo->m_iNoEntryDefenseLevel;
	if (getNumUnitFullHeal() == iDefault) m_iNumUnitFullHeal = pClassInfo->getNumUnitFullHeal();
	if (getNumPopulationEmployed() == iDefault) m_iNumPopulationEmployed = pClassInfo->getNumPopulationEmployed();
	if (getHealthPercentPerPopulation() == iDefault) m_iHealthPercentPerPopulation = pClassInfo->getHealthPercentPerPopulation();
	if (getHappinessPercentPerPopulation() == iDefault) m_iHappinessPercentPerPopulation = pClassInfo->getHappinessPercentPerPopulation();
	if (isProtectedCulture() == bDefault) m_bProtectedCulture = pClassInfo->isProtectedCulture();

	if (!m_iGlobalMaintenanceModifier) m_iGlobalMaintenanceModifier = pClassInfo->getGlobalMaintenanceModifier();
	if (!m_iAreaMaintenanceModifier) m_iAreaMaintenanceModifier = pClassInfo->getAreaMaintenanceModifier();
	if (!m_iOtherAreaMaintenanceModifier) m_iOtherAreaMaintenanceModifier = pClassInfo->getOtherAreaMaintenanceModifier();
	if (!m_iDistanceMaintenanceModifier) m_iDistanceMaintenanceModifier = pClassInfo->getDistanceMaintenanceModifier();
	if (!m_iNumCitiesMaintenanceModifier) m_iNumCitiesMaintenanceModifier = pClassInfo->getNumCitiesMaintenanceModifier();
	if (!m_iCoastalDistanceMaintenanceModifier) m_iCoastalDistanceMaintenanceModifier = pClassInfo->getCoastalDistanceMaintenanceModifier();
	if (!m_iConnectedCityMaintenanceModifier) m_iConnectedCityMaintenanceModifier = pClassInfo->getConnectedCityMaintenanceModifier();

	m_aUnitCombatExtraStrength.copyNonDefaults(pClassInfo->getUnitCombatExtraStrength());

	for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
	{
		if ( getCommerceAttacks(j) == iDefault && pClassInfo->getCommerceAttacks(j) != iDefault)
		{
			if ( NULL == m_piCommerceAttacks )
			{
				CvXMLLoadUtility::InitList(&m_piCommerceAttacks,NUM_COMMERCE_TYPES,iDefault);
			}
			m_piCommerceAttacks[j] = pClassInfo->getCommerceAttacks(j);
		}
	}

	for ( int j = 0; j < GC.getNumTerrainInfos(); j++)
	{
		if ( isPrereqOrTerrain(j) == bDefault && pClassInfo->isPrereqOrTerrain(j) != bDefault)
		{
			if ( NULL == m_pbPrereqOrTerrain )
			{
				CvXMLLoadUtility::InitList(&m_pbPrereqOrTerrain,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbPrereqOrTerrain[j] = pClassInfo->isPrereqOrTerrain(j);
		}
	}
	for ( int j = 0; j < GC.getNumTerrainInfos(); j++)
	{
		if ( isPrereqAndTerrain(j) == bDefault && pClassInfo->isPrereqAndTerrain(j) != bDefault)
		{
			if ( NULL == m_pbPrereqAndTerrain)
			{
				CvXMLLoadUtility::InitList(&m_pbPrereqAndTerrain,GC.getNumTerrainInfos(),bDefault);
			}
			m_pbPrereqAndTerrain[j] = pClassInfo->isPrereqAndTerrain(j);
		}
	}
	for ( int j = 0; j < GC.getNumImprovementInfos(); j++)
	{
		if ( isPrereqOrImprovement(j) == bDefault && pClassInfo->isPrereqOrImprovement(j) != bDefault)
		{
			if ( NULL == m_pbPrereqOrImprovement )
			{
				CvXMLLoadUtility::InitList(&m_pbPrereqOrImprovement,GC.getNumImprovementInfos(),bDefault);
			}
			m_pbPrereqOrImprovement[j] = pClassInfo->isPrereqOrImprovement(j);
		}
	}
	for ( int j = 0; j < GC.getNumFeatureInfos(); j++)
	{
		if ( isPrereqOrFeature(j) == bDefault && pClassInfo->isPrereqOrFeature(j) != bDefault)
		{
			if ( NULL == m_pbPrereqOrFeature )
			{
				CvXMLLoadUtility::InitList(&m_pbPrereqOrFeature,GC.getNumFeatureInfos(),bDefault);
			}
			m_pbPrereqOrFeature[j] = pClassInfo->isPrereqOrFeature(j);
		}
	}
	for ( int j = 0; j < GC.getNumBonusInfos(); j++)
	{
		if ( getBonusDefenseChanges(j) == iDefault && pClassInfo->getBonusDefenseChanges(j) != iDefault)
		{
			if ( NULL == m_piBonusDefenseChanges )
			{
				CvXMLLoadUtility::InitList(&m_piBonusDefenseChanges,GC.getNumBonusInfos(),iDefault);
			}
			m_piBonusDefenseChanges[j] = pClassInfo->getBonusDefenseChanges(j);
		}
	}
	for ( int i = 0; i < GC.getNumBonusInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if ( getBonusCommerceModifier(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getBonusCommerceModifier(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiBonusCommerceModifier == NULL )
					{
						m_ppaiBonusCommerceModifier = new int*[GC.getNumBonusInfos()];
						for(int k = 0; k < GC.getNumBonusInfos(); k++)
						{
							m_ppaiBonusCommerceModifier[k] = NULL;
						}
					}
					if ( m_ppaiBonusCommerceModifier[i] == NULL )
					{
						m_ppaiBonusCommerceModifier[i] = new int[NUM_COMMERCE_TYPES];
						for(int k = 0; k < NUM_COMMERCE_TYPES; k++)
						{
							m_ppaiBonusCommerceModifier[i][k] = NULL;
						}
					}

					m_ppaiBonusCommerceModifier[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumBonusInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getBonusYieldChanges(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getBonusYieldChanges(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiBonusYieldChanges == NULL )
					{
						m_ppaiBonusYieldChanges = new int*[GC.getNumBonusInfos()];
						for(int k = 0; k < GC.getNumBonusInfos(); k++)
						{
							m_ppaiBonusYieldChanges[k] = NULL;
						}
					}
					if ( m_ppaiBonusYieldChanges[i] == NULL )
					{
						m_ppaiBonusYieldChanges[i] = new int[NUM_YIELD_TYPES];
						for(int k = 0; k < NUM_YIELD_TYPES; k++)
						{
							m_ppaiBonusYieldChanges[i][k] = NULL;
						}
					}

					m_ppaiBonusYieldChanges[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumBonusInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if ( getBonusCommercePercentChanges(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getBonusCommercePercentChanges(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiBonusCommercePercentChanges == NULL )
					{
						m_ppaiBonusCommercePercentChanges = new int*[GC.getNumBonusInfos()];
						for(int k = 0; k < GC.getNumBonusInfos(); k++)
						{
							m_ppaiBonusCommercePercentChanges[k] = NULL;
						}
					}
					if ( m_ppaiBonusCommercePercentChanges[i] == NULL )
					{
						m_ppaiBonusCommercePercentChanges[i] = new int[NUM_COMMERCE_TYPES];
						for(int k = 0; k < NUM_COMMERCE_TYPES; k++)
						{
							m_ppaiBonusCommercePercentChanges[i][k] = NULL;
						}
					}

					m_ppaiBonusCommercePercentChanges[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumBonusInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getVicinityBonusYieldChanges(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getVicinityBonusYieldChanges(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiVicinityBonusYieldChanges == NULL )
					{
						m_ppaiVicinityBonusYieldChanges = new int*[GC.getNumBonusInfos()];
						for(int k = 0; k < GC.getNumBonusInfos(); k++)
						{
							m_ppaiVicinityBonusYieldChanges[k] = NULL;
						}
					}
					if ( m_ppaiVicinityBonusYieldChanges[i] == NULL )
					{
						m_ppaiVicinityBonusYieldChanges[i] = new int[NUM_YIELD_TYPES];
						for(int k = 0; k < NUM_YIELD_TYPES; k++)
						{
							m_ppaiVicinityBonusYieldChanges[i][k] = NULL;
						}
					}

					m_ppaiVicinityBonusYieldChanges[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumImprovementInfos(); i++ )
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++ )
		{
			if ( getImprovementYieldChanges(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getImprovementYieldChanges(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppiImprovementYieldChanges == NULL )
					{
						m_ppiImprovementYieldChanges = new int*[GC.getNumImprovementInfos()];
						for(int k = 0; k < GC.getNumImprovementInfos(); k++)
						{
							m_ppiImprovementYieldChanges[k] = NULL;
						}
					}
					if ( m_ppiImprovementYieldChanges[i] == NULL )
					{
						m_ppiImprovementYieldChanges[i] = new int[NUM_YIELD_TYPES];
						for(int k = 0; k < NUM_YIELD_TYPES; k++)
						{
							m_ppiImprovementYieldChanges[i][k] = NULL;
						}
					}

					m_ppiImprovementYieldChanges[i][j] = iChange;
				}
			}
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_piPrereqOrVicinityBonuses, pClassInfo->m_piPrereqOrVicinityBonuses);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aePrereqOrRawVicinityBonuses, pClassInfo->getPrereqOrRawVicinityBonuses());

	for ( int i = 0; i < GC.getNumTechInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if ( getTechCommerceChange(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getTechCommerceChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiTechCommerceChange == NULL )
					{
						m_ppaiTechCommerceChange = new int*[GC.getNumTechInfos()];
						for(int k = 0; k < GC.getNumTechInfos(); k++)
						{
							m_ppaiTechCommerceChange[k] = NULL;
						}
					}
					if ( m_ppaiTechCommerceChange[i] == NULL )
					{
						m_ppaiTechCommerceChange[i] = new int[NUM_COMMERCE_TYPES];
						for(int k = 0; k < NUM_COMMERCE_TYPES; k++)
						{
							m_ppaiTechCommerceChange[i][k] = NULL;
						}
					}

					m_ppaiTechCommerceChange[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumTechInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getTechYieldChange(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getTechYieldChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiTechYieldChange == NULL )
					{
						m_ppaiTechYieldChange = new int*[GC.getNumTechInfos()];
						for(int k = 0; k < GC.getNumTechInfos(); k++)
						{
							m_ppaiTechYieldChange[k] = NULL;
						}
					}
					if ( m_ppaiTechYieldChange[i] == NULL )
					{
						m_ppaiTechYieldChange[i] = new int[NUM_YIELD_TYPES];
						for(int k = 0; k < NUM_YIELD_TYPES; k++)
						{
							m_ppaiTechYieldChange[i][k] = NULL;
						}
					}

					m_ppaiTechYieldChange[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumTechInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < GC.getNumSpecialistInfos(); j++)
		{
			if ( getTechSpecialistChange(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getTechSpecialistChange(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiTechSpecialistChange == NULL )
					{
						m_ppaiTechSpecialistChange = new int*[GC.getNumTechInfos()];
						for(int k = 0; k < GC.getNumTechInfos(); k++)
						{
							m_ppaiTechSpecialistChange[k] = NULL;
						}
					}
					if ( m_ppaiTechSpecialistChange[i] == NULL )
					{
						m_ppaiTechSpecialistChange[i] = new int[GC.getNumSpecialistInfos()];
						for(int k = 0; k < GC.getNumSpecialistInfos(); k++)
						{
							m_ppaiTechSpecialistChange[i][k] = NULL;
						}
					}

					m_ppaiTechSpecialistChange[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumTechInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_COMMERCE_TYPES; j++)
		{
			if ( getTechCommerceModifier(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getTechCommerceModifier(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiTechCommerceModifier == NULL )
					{
						m_ppaiTechCommerceModifier = new int*[GC.getNumTechInfos()];
						for(int k = 0; k < GC.getNumTechInfos(); k++)
						{
							m_ppaiTechCommerceModifier[k] = NULL;
						}
					}
					if ( m_ppaiTechCommerceModifier[i] == NULL )
					{
						m_ppaiTechCommerceModifier[i] = new int[NUM_COMMERCE_TYPES];
						for(int k = 0; k < NUM_COMMERCE_TYPES; k++)
						{
							m_ppaiTechCommerceModifier[i][k] = NULL;
						}
					}

					m_ppaiTechCommerceModifier[i][j] = iChange;
				}
			}
		}
	}

	for ( int i = 0; i < GC.getNumTechInfos(); i++)	// "Init2DIntList" verify method
	{
		for ( int j = 0; j < NUM_YIELD_TYPES; j++)
		{
			if ( getTechYieldModifier(i,j) == iDefault )
			{
				const int iChange = pClassInfo->getTechYieldModifier(i, j);

				if ( iChange != iDefault )
				{
					if ( m_ppaiTechYieldModifier == NULL )
					{
						m_ppaiTechYieldModifier = new int*[GC.getNumTechInfos()];
						for(int k = 0; k < GC.getNumTechInfos(); k++)
						{
							m_ppaiTechYieldModifier[k] = NULL;
						}
					}
					if ( m_ppaiTechYieldModifier[i] == NULL )
					{
						m_ppaiTechYieldModifier[i] = new int[NUM_YIELD_TYPES];
						for(int k = 0; k < NUM_YIELD_TYPES; k++)
						{
							m_ppaiTechYieldModifier[i][k] = NULL;
						}
					}

					m_ppaiTechYieldModifier[i][j] = iChange;
				}
			}
		}
	}
	//These are done differently because of the ReadPass3
	for ( int i = 0; i < pClassInfo->isPrereqOrCivicsVectorSize(); i++ )
	{
		m_abPrereqOrCivicsforPass3.push_back(pClassInfo->isPrereqOrCivicsValuesVectorElement(i));
		m_aszPrereqOrCivicsforPass3.push_back(pClassInfo->isPrereqOrCivicsNamesVectorElement(i));
	}

	for ( int i = 0; i < pClassInfo->isPrereqAndCivicsVectorSize(); i++ )
	{
		m_abPrereqAndCivicsforPass3.push_back(pClassInfo->isPrereqAndCivicsValuesVectorElement(i));
		m_aszPrereqAndCivicsforPass3.push_back(pClassInfo->isPrereqAndCivicsNamesVectorElement(i));
	}

	m_Properties.copyNonDefaults(pClassInfo->getProperties());
	m_PropertiesAllCities.copyNonDefaults(pClassInfo->getPropertiesAllCities());
	m_PrereqMinProperties.copyNonDefaults(pClassInfo->getPrereqMinProperties());
	m_PrereqMaxProperties.copyNonDefaults(pClassInfo->getPrereqMaxProperties());
	m_PrereqPlayerMinProperties.copyNonDefaults(pClassInfo->getPrereqPlayerMinProperties());
	m_PrereqPlayerMaxProperties.copyNonDefaults(pClassInfo->getPrereqPlayerMaxProperties());

	m_PropertyManipulators.copyNonDefaults(&pClassInfo->m_PropertyManipulators);

	if (!m_pExprNewCityFree)
	{
		m_pExprNewCityFree = pClassInfo->m_pExprNewCityFree;
		pClassInfo->m_pExprNewCityFree = NULL;
	}

	if (!m_pExprConstructCondition)
	{
		m_pExprConstructCondition = pClassInfo->m_pExprConstructCondition;
		pClassInfo->m_pExprConstructCondition = NULL;
	}

	//TB Combat Mods (Buildings) begin
	if (m_ePropertySpawnUnit == NO_UNIT) m_ePropertySpawnUnit = pClassInfo->getPropertySpawnUnit();
	if (getPropertySpawnProperty() == NO_PROPERTY) m_ePropertySpawnProperty = pClassInfo->getPropertySpawnProperty();
	if (getPromotionLineType() == NO_PROMOTIONLINE) m_ePromotionLineType = pClassInfo->getPromotionLineType();
	//integers
	if (getLinePriority() == iDefault) m_iLinePriority = pClassInfo->getLinePriority();
	if (getOutbreakBase() == iDefault) m_iOutbreakBase = pClassInfo->getOutbreakBase();
	if (getOvercomeBase() == iDefault) m_iOvercomeBase = pClassInfo->getOvercomeBase();
	if (getTradeCommunicability() == iDefault) m_iTradeCommunicability = pClassInfo->getTradeCommunicability();
#ifdef STRENGTH_IN_NUMBERS
	if (getFrontSupportPercentModifier() == iDefault) m_iFrontSupportPercentModifier = pClassInfo->getFrontSupportPercentModifier();
	if (getShortRangeSupportPercentModifier() == iDefault) m_iShortRangeSupportPercentModifier = pClassInfo->getShortRangeSupportPercentModifier();
	if (getMediumRangeSupportPercentModifier() == iDefault) m_iMediumRangeSupportPercentModifier = pClassInfo->getMediumRangeSupportPercentModifier();
	if (getLongRangeSupportPercentModifier() == iDefault) m_iLongRangeSupportPercentModifier = pClassInfo->getLongRangeSupportPercentModifier();
	if (getFlankSupportPercentModifier() == iDefault) m_iFlankSupportPercentModifier = pClassInfo->getFlankSupportPercentModifier();
#endif
	if (getNationalCaptureProbabilityModifier() == iDefault) m_iNationalCaptureProbabilityModifier = pClassInfo->getNationalCaptureProbabilityModifier();
	if (getNationalCaptureResistanceModifier() == iDefault) m_iNationalCaptureResistanceModifier = pClassInfo->getNationalCaptureResistanceModifier();
	if (getLocalCaptureProbabilityModifier() == iDefault) m_iLocalCaptureProbabilityModifier = pClassInfo->getLocalCaptureProbabilityModifier();
	if (getLocalCaptureResistanceModifier() == iDefault) m_iLocalCaptureResistanceModifier = pClassInfo->getLocalCaptureResistanceModifier();
	if (getLocalDynamicDefense() == iDefault) m_iLocalDynamicDefense = pClassInfo->getLocalDynamicDefense();
	if (getRiverDefensePenalty() == iDefault) m_iRiverDefensePenalty = pClassInfo->getRiverDefensePenalty();
	if (m_iLocalRepel == iDefault) m_iLocalRepel = pClassInfo->m_iLocalRepel;
	if (m_iMinDefense == iDefault) m_iMinDefense = pClassInfo->m_iMinDefense;
	if (getBuildingDefenseRecoverySpeedModifier() == iDefault) m_iBuildingDefenseRecoverySpeedModifier = pClassInfo->getBuildingDefenseRecoverySpeedModifier();
	if (getCityDefenseRecoverySpeedModifier() == iDefault) m_iCityDefenseRecoverySpeedModifier = pClassInfo->getCityDefenseRecoverySpeedModifier();
	if (getDamageAttackerChance() == iDefault) m_iDamageAttackerChance = pClassInfo->getDamageAttackerChance();
	if (getDamageToAttacker() == iDefault) m_iDamageToAttacker = pClassInfo->getDamageToAttacker();
	if (m_iMaxPopulationAllowed == -1) m_iMaxPopulationAllowed = pClassInfo->m_iMaxPopulationAllowed;
	if (m_iMaxPopulationChange == iDefault) m_iMaxPopulationChange = pClassInfo->m_iMaxPopulationChange;
	if (getInsidiousness() == iDefault) m_iInsidiousness = pClassInfo->getInsidiousness();
	if (getInvestigation() == iDefault) m_iInvestigation = pClassInfo->getInvestigation();
	if (getPopulationChange() == iDefault) m_iPopulationChange = pClassInfo->getPopulationChange();
	if (getOneTimePopulationPercentLoss() == iDefault) m_iOneTimePopulationPercentLoss = pClassInfo->getOneTimePopulationPercentLoss();
	//Booleans
	if (isDamageToAttackerIgnoresArmor() == bDefault) m_bDamageToAttackerIgnoresArmor = pClassInfo->isDamageToAttackerIgnoresArmor();
	if (isDamageAllAttackers() == bDefault) m_bDamageAllAttackers = pClassInfo->isDamageAllAttackers();
	if (isQuarantine() == bDefault) m_bQuarantine = pClassInfo->isQuarantine();
	if (isDamageAllAttackers() || pClassInfo->isDamageAllAttackers())
	{
		m_bDamageAttackerCapable = true;
	}
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aFreePromoTypes, pClassInfo->m_aFreePromoTypes);
	GC.copyNonDefaultDelayedResolutionVector(m_aiFreeTraitTypes, pClassInfo->m_aiFreeTraitTypes);
	GC.copyNonDefaultDelayedResolutionVector(m_aiPrereqInCityBuildings, pClassInfo->m_aiPrereqInCityBuildings);
	GC.copyNonDefaultDelayedResolutionVector(m_vPrereqNotInCityBuildings, pClassInfo->m_vPrereqNotInCityBuildings);
	GC.copyNonDefaultDelayedResolutionVector(m_vPrereqOrBuilding, pClassInfo->m_vPrereqOrBuilding);
	GC.copyNonDefaultDelayedResolutionVector(m_vReplacementBuilding, pClassInfo->m_vReplacementBuilding);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aHealUnitCombatTypes, pClassInfo->m_aHealUnitCombatTypes);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aBonusAidModifiers, pClassInfo->m_aBonusAidModifiers);
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aAidRateChanges, pClassInfo->m_aAidRateChanges);

	if (getNumEnabledCivilizationTypes() == 0)
	{
		const int iNum = pClassInfo->getNumEnabledCivilizationTypes();
		m_aEnabledCivilizationTypes.resize(iNum);
		for (int i=0; i<iNum; i++)
		{
			GC.copyNonDefaultDelayedResolution((int*)&(m_aEnabledCivilizationTypes[i].eCivilization), (int*)&(pClassInfo->m_aEnabledCivilizationTypes[i].eCivilization));
		}
	}

	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aeMapCategoryTypes, pClassInfo->getMapCategories());
	CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aiUnitCombatRetrainTypes, pClassInfo->m_aiUnitCombatRetrainTypes);

	if (getNumMayDamageAttackingUnitCombatTypes() == 0)
	{
		m_aiMayDamageAttackingUnitCombatTypes.clear();
		for ( int i = 0; i < pClassInfo->getNumMayDamageAttackingUnitCombatTypes(); i++)
		{
			m_aiMayDamageAttackingUnitCombatTypes.push_back(pClassInfo->getMayDamageAttackingUnitCombatType(i));
			if (getMayDamageAttackingUnitCombatType(i) > 0)
			{
				m_bDamageAttackerCapable = true;
			}
		}
	}

	// int vector utilizing pairing without delayed resolution
	if (getNumUnitCombatRepelModifiers()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatRepelModifiers(); i++)
		{
			const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			const int iChange = pClassInfo->getUnitCombatRepelModifier(i, true);
			m_aUnitCombatRepelModifiers.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumUnitCombatRepelAgainstModifiers()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatRepelAgainstModifiers(); i++)
		{
			const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			const int iChange = pClassInfo->getUnitCombatRepelAgainstModifier(i, true);
			m_aUnitCombatRepelAgainstModifiers.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumUnitCombatDefenseAgainstModifiers()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatDefenseAgainstModifiers(); i++)
		{
			const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			const int iChange = pClassInfo->getUnitCombatDefenseAgainstModifier(i);
			m_aUnitCombatDefenseAgainstModifiers.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumUnitCombatProdModifiers()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatProdModifiers(); i++)
		{
			const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			const int iChange = pClassInfo->getUnitCombatProdModifier(i);
			m_aUnitCombatProdModifiers.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumUnitCombatOngoingTrainingDurations()==0)
	{
		for (int i=0; i < pClassInfo->getNumUnitCombatOngoingTrainingDurations(); i++)
		{
			const UnitCombatTypes eUnitCombat = ((UnitCombatTypes)i);
			const int iChange = pClassInfo->getUnitCombatOngoingTrainingDuration(i, true);
			m_aUnitCombatOngoingTrainingDurations.push_back(std::make_pair(eUnitCombat, iChange));
		}
	}

	if (getNumAfflictionOutbreakLevelChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumAfflictionOutbreakLevelChanges(); i++)
		{
			const PromotionLineTypes ePromotionLine = ((PromotionLineTypes)i);
			const int iChange = pClassInfo->getAfflictionOutbreakLevelChange(i);
			m_aAfflictionOutbreakLevelChanges.push_back(std::make_pair(ePromotionLine, iChange));
		}
	}

	if (getNumTechOutbreakLevelChanges()==0)
	{
		for (int i=0; i < pClassInfo->getNumTechOutbreakLevelChanges(); i++)
		{
			const TechTypes eTech = ((TechTypes)i);
			const int iChange = pClassInfo->getTechOutbreakLevelChange(i);
			m_aTechOutbreakLevelChanges.push_back(std::make_pair(eTech, iChange));
		}
	}

	m_aTechHappinessChanges.copyNonDefaults(pClassInfo->getTechHappinessChanges());
	m_aTechHealthChanges.copyNonDefaults(pClassInfo->getTechHealthChanges());

	//Arrays
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

	//if (!m_pExprFreePromotionCondition)
	//{
	//	m_pExprFreePromotionCondition = pClassInfo->m_pExprFreePromotionCondition;
	//	pClassInfo->m_pExprFreePromotionCondition = NULL;
	//}
	//TB Combat Mods (Buildings) end

	//Alberts2 PrereqBonuses
	if (!pClassInfo->m_aePrereqOrBonuses.empty())
	{
		CvXMLLoadUtility::CopyNonDefaultsFromVector(m_aePrereqOrBonuses, pClassInfo->m_aePrereqOrBonuses);
	}

	if (getMaxGlobalInstances() == -1) m_iMaxGlobalInstances = pClassInfo->getMaxGlobalInstances();
	if (getMaxTeamInstances() == -1) m_iMaxTeamInstances = pClassInfo->getMaxTeamInstances();
	if (getMaxPlayerInstances() == -1) m_iMaxPlayerInstances = pClassInfo->getMaxPlayerInstances();
	if (getExtraPlayerInstances() == 0) m_iExtraPlayerInstances = pClassInfo->getExtraPlayerInstances();

	for ( int i = 0; i < GC.getNumVictoryInfos(); i++ )
	{
		if (getVictoryThreshold(i) == 0 && pClassInfo->getVictoryThreshold(i) != 0)
		{
			if ( NULL == m_piVictoryThreshold )
			{
				CvXMLLoadUtility::InitList(&m_piVictoryThreshold,GC.getNumVictoryInfos(), 0);
			}
			m_piVictoryThreshold[i] = pClassInfo->getVictoryThreshold(i);
		}
	}
	GC.copyNonDefaultDelayedResolution((int*)&m_iFreeBuilding, (int*)&pClassInfo->m_iFreeBuilding);
	GC.copyNonDefaultDelayedResolution((int*)&m_iFreeAreaBuilding, (int*)&pClassInfo->m_iFreeAreaBuilding);
	GC.copyNonDefaultDelayedResolution((int*)&m_iProductionContinueBuilding, (int*)&pClassInfo->m_iProductionContinueBuilding);
	GC.copyNonDefaultDelayedResolution((int*)&m_iPrereqAnyoneBuilding, (int*)&pClassInfo->m_iPrereqAnyoneBuilding);
	GC.copyNonDefaultDelayedResolution((int*)&m_iExtendsBuilding, (int*)&pClassInfo->m_iExtendsBuilding);
	GC.copyNonDefaultDelayedResolution((int*)&m_iObsoletesToBuilding, (int*)&pClassInfo->m_iObsoletesToBuilding);

	m_aBuildingHappinessChanges.copyNonDefaultDelayedResolution(pClassInfo->getBuildingHappinessChanges());
	m_aUnitProductionModifier.copyNonDefaultDelayedResolution(pClassInfo->getUnitProductionModifiers());
	m_aBuildingProductionModifier.copyNonDefaultDelayedResolution(pClassInfo->getBuildingProductionModifiers());
	m_aGlobalBuildingProductionModifier.copyNonDefaultDelayedResolution(pClassInfo->getGlobalBuildingProductionModifiers());
	m_aPrereqNumOfBuilding.copyNonDefaultDelayedResolution(pClassInfo->getPrereqNumOfBuildings());
	m_aGlobalBuildingCostModifier.copyNonDefaultDelayedResolution(pClassInfo->getGlobalBuildingCostModifiers());
}

void CvBuildingInfo::copyNonDefaultsReadPass2(CvBuildingInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver)
{
	int iDefault = 0;
	int iTextDefault = -1;
	bool bNoDuplicate = true;

	for (int j = 0; j < GC.getNumBuildingInfos(); j++)
	{
		foreach_(const BuildingCommerceChange& it, m_aGlobalBuildingCommerceChanges)
		{
			if (it.eBuilding == (BuildingTypes)j)
			{
				//obviously some modder already set this Building to some value
				//we don't want to overwrite his settings with the older(assuming he added
				//his tag on purpose)
				bNoDuplicate = false;
				break;
			}
		}
		if (bNoDuplicate)
		{
			for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; iCommerce++)
			{
				if (pClassInfo->getGlobalBuildingCommerceChange(j, iCommerce) != 0)
				{
					BuildingCommerceChange kChange;
					kChange.eBuilding = (BuildingTypes)j;
					kChange.eCommerce = (CommerceTypes)iCommerce;
					kChange.iChange = pClassInfo->getGlobalBuildingCommerceChange(j, iCommerce);
					m_aGlobalBuildingCommerceChanges.push_back(kChange);
				}
			}
		}
	}
}

bool CvBuildingInfo::isNewCityFree(const CvGameObject* pObject)
{
	return m_pExprNewCityFree && m_pExprNewCityFree->evaluate(pObject);
}

const BoolExpr* CvBuildingInfo::getConstructCondition() const
{
	return m_pExprConstructCondition;
}

//TB Building Tags
//const BoolExpr* CvBuildingInfo::getFreePromotionCondition()
//{
//	return m_pExprFreePromotionCondition;
//}


bool CvBuildingInfo::getNotShowInCity() const
{
	return m_bNotShowInCity;
}

void CvBuildingInfo::setNotShowInCity()
{
	m_bNotShowInCity = (m_szArtDefineTag == "" || getArtInfo()->getScale() == 0.0 || stricmp(getArtInfo()->getNIF(), "Art/empty.nif") == 0);
}


int CvBuildingInfo::getNumPrereqOrBonuses() const
{
	return m_aePrereqOrBonuses.size();
}

BonusTypes CvBuildingInfo::getPrereqOrBonuses(int i) const
{
	FASSERT_BOUNDS(0, getNumPrereqOrBonuses(), i)
	return static_cast<BonusTypes>(m_aePrereqOrBonuses[i]);
}

const python::list CvBuildingInfo::cyGetPrereqOrBonuses() const
{
	return Cy::makeList(m_aePrereqOrBonuses);
}

bool CvBuildingInfo::isPrereqOrBuilding(const int i) const
{
	return find(m_vPrereqOrBuilding.begin(), m_vPrereqOrBuilding.end(), i) != m_vPrereqOrBuilding.end();
}

int CvBuildingInfo::getPrereqOrBuilding(const int i) const
{
	return m_vPrereqOrBuilding[i];
}
short CvBuildingInfo::getNumPrereqOrBuilding() const
{
	return m_vPrereqOrBuilding.size();
}

int CvBuildingInfo::getReplacementBuilding(const int i) const
{
	return m_vReplacementBuilding[i];
}
short CvBuildingInfo::getNumReplacementBuilding() const
{
	return m_vReplacementBuilding.size();
}

void CvBuildingInfo::setReplacedBuilding(const int i)
{
	if (find(m_vReplacedBuilding.begin(), m_vReplacedBuilding.end(), i) == m_vReplacedBuilding.end())
	{
		m_vReplacedBuilding.push_back(i);
	}
}
int CvBuildingInfo::getReplacedBuilding(const int i) const
{
	return m_vReplacedBuilding[i];
}
short CvBuildingInfo::getNumReplacedBuilding() const
{
	return m_vReplacedBuilding.size();
}
