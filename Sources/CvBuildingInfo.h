#pragma once

#ifndef CV_BUILDING_INFO_H
#define CV_BUILDING_INFO_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildingInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "CvInfoUtil.h"
#include "CvPython.h"
#include "IDValuemap.h"

class BoolExpr;
class CvArtInfoBuilding;
class CvArtInfoMovie;
class CvHotkeyInfo;
class CvProperties;
class CvPropertyManipulators;
class CvXMLLoadUtility;
//struct CvInfoUtil;

class CvBuildingInfo : public CvHotkeyInfo
{
public:
	CvBuildingInfo();
	virtual ~CvBuildingInfo();

	bool isPrereqPower() const						{ return m_bPrereqPower; }
	bool isApplyFreePromotionOnMove() const			{ return m_bApplyFreePromotionOnMove; }
	bool isNoEnemyPillagingIncome() const			{ return m_bNoEnemyPillagingIncome; }
	bool isProvidesFreshWater() const				{ return m_bProvidesFreshWater; }
	bool isForceAllTradeRoutes() const				{ return m_bForceAllTradeRoutes; }
	bool isForceNoPrereqScaling() const				{ return m_bForceNoPrereqScaling; }
	bool isPrereqWar() const						{ return m_bPrereqWar; }
	bool isRequiresActiveCivics() const				{ return m_bRequiresActiveCivics; }
	bool isZoneOfControl() const					{ return m_bZoneOfControl; }
	bool isProtectedCulture() const					{ return m_bProtectedCulture; }
	bool isNoLimit() const							{ return m_bNoLimit; }
	bool isTeamShare() const						{ return m_bTeamShare; }
	bool isAutoBuild() const						{ return m_bAutoBuild; }
	bool isWater() const							{ return m_bWater; }
	bool isRiver() const							{ return m_bRiver; }
	bool isFreshWater() const						{ return m_bFreshWater; }
	bool isPower() const							{ return m_bPower; }
	bool isOrbital() const							{ return m_bOrbital; }
	bool isOrbitalInfrastructure() const			{ return m_bOrbitalInfrastructure; }
	bool isNoHolyCity() const						{ return m_bNoHolyCity; }
	bool isAreaBorderObstacle() const				{ return m_bAreaBorderObstacle; }
	bool isForceTeamVoteEligible() const			{ return m_bForceTeamVoteEligible; }
	bool isCapital() const							{ return m_bCapital; }
	bool isGovernmentCenter() const					{ return m_bGovernmentCenter; }
	bool isGoldenAge() const						{ return m_bGoldenAge; }
	bool isMapCentering() const						{ return m_bMapCentering; }
	bool isNoUnhappiness() const					{ return m_bNoUnhappiness; }
	bool isNoUnhealthyPopulation() const			{ return m_bNoUnhealthyPopulation; }
	bool isBuildingOnlyHealthy() const				{ return m_bBuildingOnlyHealthy; }
	bool isNeverCapture() const						{ return m_bNeverCapture; }
	bool isNukeImmune() const						{ return m_bNukeImmune; }
	bool isCenterInCity() const						{ return m_bCenterInCity; }
	bool needStateReligionInCity() const			{ return m_bStateReligionInCity; }
	bool isAllowsNukes() const						{ return m_bAllowsNukes; }
	bool isDamageToAttackerIgnoresArmor() const;
	bool isDamageAllAttackers() const;
	bool isDamageAttackerCapable() const;
	bool isQuarantine() const;
	bool getNotShowInCity() const;
	bool EnablesOtherBuildings() const;
	bool EnablesUnits() const						{ return m_bEnablesUnits; }

	int getMaxGlobalInstances() const				{ return m_iMaxGlobalInstances; }
	int getMaxTeamInstances() const					{ return m_iMaxTeamInstances; }
	int getMaxPlayerInstances() const				{ return m_iMaxPlayerInstances; }
	int getExtraPlayerInstances() const				{ return m_iExtraPlayerInstances; }
	int getVictoryPrereq() const					{ return m_iVictoryPrereq; }
	EraTypes getFreeStartEra() const				{ return m_iFreeStartEra; }
	int getMaxStartEra() const						{ return m_iMaxStartEra; }
	TechTypes getObsoleteTech() const				{ return m_iObsoleteTech; }
	int getPrereqAndTech() const					{ return m_iPrereqAndTech; }
	int getNoBonus() const							{ return m_iNoBonus; }
	int getPowerBonus() const						{ return m_iPowerBonus; }
	int getFreeBonus() const						{ return m_iFreeBonus; }
	int getNumFreeBonuses() const					{ return m_iNumFreeBonuses; }
	int getNumExtraFreeBonuses() const				{ return m_aExtraFreeBonuses.size(); }
	BuildingTypes getFreeBuilding() const			{ return m_iFreeBuilding; }
	BuildingTypes getFreeAreaBuilding() const		{ return m_iFreeAreaBuilding; }
	int getFreePromotion() const					{ return m_iFreePromotion; }
	int getCivicOption() const						{ return m_iCivicOption; }
	int getAIWeight() const							{ return m_iAIWeight; }
	int getProductionCost() const					{ return m_iProductionCost; }
	int getHurryCostModifier() const				{ return m_iHurryCostModifier; }
	int getHurryAngerModifier() const				{ return m_iHurryAngerModifier; }
	int getMinAreaSize() const						{ return m_iMinAreaSize; }
	int getNumCitiesPrereq() const					{ return m_iNumCitiesPrereq; }
	int getNumTeamsPrereq() const					{ return m_iNumTeamsPrereq; }
	int getUnitLevelPrereq() const					{ return m_iUnitLevelPrereq; }
	int getMinLatitude() const						{ return m_iMinLatitude; }
	int getMaxLatitude() const						{ return m_iMaxLatitude; }
	int getGreatPeopleRateModifier() const			{ return m_iGreatPeopleRateModifier; }
	int getGreatGeneralRateModifier() const			{ return m_iGreatGeneralRateModifier; }
	int getDomesticGreatGeneralRateModifier() const	{ return m_iDomesticGreatGeneralRateModifier; }
	int getGlobalGreatPeopleRateModifier() const	{ return m_iGlobalGreatPeopleRateModifier; }
	int getAnarchyModifier() const					{ return m_iAnarchyModifier; }
	int getGoldenAgeModifier() const				{ return m_iGoldenAgeModifier; }
	int getGlobalHurryModifier() const				{ return m_iGlobalHurryModifier; }
	int getFreeExperience() const					{ return m_iFreeExperience; }
	int getGlobalFreeExperience() const				{ return m_iGlobalFreeExperience; }
	int getFoodKept() const							{ return m_iFoodKept; }
	int getAirlift() const							{ return m_iAirlift; }
	int getAirModifier() const						{ return m_iAirModifier; }
	int getAirUnitCapacity() const					{ return m_iAirUnitCapacity; }
	int getNukeModifier() const						{ return m_iNukeModifier; }
	int getNukeExplosionRand() const				{ return m_iNukeExplosionRand; }
	int getFreeSpecialist() const					{ return m_iFreeSpecialist; }
	int getAreaFreeSpecialist() const				{ return m_iAreaFreeSpecialist; }
	int getGlobalFreeSpecialist() const				{ return m_iGlobalFreeSpecialist; }
	int getHappiness() const						{ return m_iHappiness; }
	int getAreaHappiness() const					{ return m_iAreaHappiness; }
	int getGlobalHappiness() const					{ return m_iGlobalHappiness; }
	int getStateReligionHappiness() const			{ return m_iStateReligionHappiness; }
	int getWorkerSpeedModifier() const				{ return m_iWorkerSpeedModifier; }
	int getMilitaryProductionModifier() const		{ return m_iMilitaryProductionModifier; }
	int getSpaceProductionModifier() const			{ return m_iSpaceProductionModifier; }
	int getGlobalSpaceProductionModifier() const	{ return m_iGlobalSpaceProductionModifier; }
	int getTradeRoutes() const						{ return m_iTradeRoutes; }
	int getCoastalTradeRoutes() const				{ return m_iCoastalTradeRoutes; }
	int getGlobalTradeRoutes() const				{ return m_iGlobalTradeRoutes; }
	int getTradeRouteModifier() const				{ return m_iTradeRouteModifier; }
	int getForeignTradeRouteModifier() const		{ return m_iForeignTradeRouteModifier; }
	int getAssetValue() const						{ return m_iAssetValue * 100; }
	int getPowerValue() const						{ return m_iPowerValue * 100; }
	int getAdvisorType() const						{ return m_iAdvisorType; }
	int getPrereqGameOption() const					{ return m_iPrereqGameOption; }
	int getNotGameOption() const					{ return m_iNotGameOption; }
	int getHolyCity() const							{ return m_iHolyCity; }
	int getReligionType() const						{ return m_iReligionType; }
	int getPrereqStateReligion() const				{ return m_iStateReligion; }
	int getPrereqReligion() const					{ return m_iPrereqReligion; }
	int getPrereqCorporation() const				{ return m_iPrereqCorporation; }
	int getFoundsCorporation() const				{ return m_iFoundsCorporation; }
	int getGlobalReligionCommerce() const			{ return m_iGlobalReligionCommerce; }
	int getGlobalCorporationCommerce() const		{ return m_iGlobalCorporationCommerce; }
	int getPrereqAndBonus() const					{ return m_iPrereqAndBonus; }
	int getGreatPeopleUnitType() const				{ return m_iGreatPeopleUnitType; }
	int getGreatPeopleRateChange() const			{ return m_iGreatPeopleRateChange; }
	int getConquestProbability() const				{ return m_iConquestProbability; }
	int getMaintenanceModifier() const				{ return m_iMaintenanceModifier; }
	int getGlobalMaintenanceModifier() const		{ return m_iGlobalMaintenanceModifier; }
	int getAreaMaintenanceModifier() const			{ return m_iAreaMaintenanceModifier; }
	int getOtherAreaMaintenanceModifier() const		{ return m_iOtherAreaMaintenanceModifier; }
	int getDistanceMaintenanceModifier() const		{ return m_iDistanceMaintenanceModifier; }
	int getNumCitiesMaintenanceModifier() const		{ return m_iNumCitiesMaintenanceModifier; }
	int getCoastalDistanceMaintenanceModifier() const { return m_iCoastalDistanceMaintenanceModifier; }
	int getConnectedCityMaintenanceModifier() const	{ return m_iConnectedCityMaintenanceModifier; }
	int getWarWearinessModifier() const				{ return m_iWarWearinessModifier; }
	int getGlobalWarWearinessModifier() const		{ return m_iGlobalWarWearinessModifier; }
	int getEnemyWarWearinessModifier() const		{ return m_iEnemyWarWearinessModifier; }
	int getHealRateChange() const					{ return m_iHealRateChange; }
	int getHealth() const							{ return m_iHealth; }
	int getAreaHealth() const						{ return m_iAreaHealth; }
	int getGlobalHealth() const						{ return m_iGlobalHealth; }
	int getGlobalPopulationChange() const			{ return m_iGlobalPopulationChange; }
	int getFreeTechs() const						{ return m_iFreeTechs; }
	int getDefenseModifier() const					{ return m_iDefenseModifier; }
	int getBombardDefenseModifier() const			{ return m_iBombardDefenseModifier; }
	int getAllCityDefenseModifier() const			{ return m_iAllCityDefenseModifier; }
	int getEspionageDefenseModifier() const			{ return m_iEspionageDefenseModifier; }
	int getUnitUpgradePriceModifier() const			{ return m_iUnitUpgradePriceModifier; }
	int getRevIdxLocal() const						{ return m_iRevIdxLocal; }
	int getRevIdxNational() const					{ return m_iRevIdxNational; }
	int getRevIdxDistanceModifier() const			{ return m_iRevIdxDistanceModifier; }
	int getMissionType() const						{ return m_iMissionType; }
	void setMissionType(int iNewType)				{ m_iMissionType = iNewType; }
	int getVoteSourceType() const					{ return m_iVoteSourceType; }
	int getDCMAirbombMission() const				{ return m_iDCMAirbombMission; }
	int getFreePromotion_2() const					{ return m_iFreePromotion_2; }
	int getFreePromotion_3() const					{ return m_iFreePromotion_3; }
	int getPrereqVicinityBonus() const				{ return m_iPrereqVicinityBonus; }
	int getPrereqRawVicinityBonus() const			{ return m_iPrereqRawVicinityBonus; }
	int getGlobalPopulationgrowthratepercentage() const { return m_iGlobalPopulationgrowthratepercentage; }
	int getPopulationgrowthratepercentage() const	{ return m_iPopulationgrowthratepercentage; }
	int getPillageGoldModifier() const				{ return m_iPillageGoldModifier; }
	int getWorldTradeRoutes() const					{ return m_iWorldTradeRoutes; }
	int getLineOfSight() const						{ return m_iLineOfSight; }
	int getInflationModifier() const				{ return m_iInflationModifier; }
	int getInvasionChance() const					{ return m_iInvasionChance; }
	int getAdjacentDamagePercent() const			{ return m_iAdjacentDamagePercent; }
	int getPrereqPopulation() const					{ return m_iPrereqPopulation; }
	BuildingTypes getProductionContinueBuilding() const	{ return m_iProductionContinueBuilding; }
	int getPrereqCultureLevel() const				{ return m_iPrereqCultureLevel; }
	int getWorkableRadius() const					{ return m_iWorkableRadius; }
	BuildingTypes getPrereqAnyoneBuilding() const	{ return m_iPrereqAnyoneBuilding; }
	BuildingTypes getExtendsBuilding() const		{ return m_iExtendsBuilding; }
	BuildingTypes getObsoletesToBuilding() const	{ return m_iObsoletesToBuilding; }
	int getOccupationTimeModifier() const			{ return m_iOccupationTimeModifier; }
	int getNoEntryDefenseLevel() const;
	int getNumUnitFullHeal() const					{ return m_iNumUnitFullHeal; }
	int getNumPopulationEmployed() const			{ return m_iNumPopulationEmployed; }
	int getHappinessPercentPerPopulation() const	{ return m_iHappinessPercentPerPopulation; }
	int getHealthPercentPerPopulation() const		{ return m_iHealthPercentPerPopulation; }
	int getLinePriority() const;
	int getOutbreakBase() const;
	int getOvercomeBase() const;
	int getTradeCommunicability() const;
#ifdef STRENGTH_IN_NUMBERS
	int getFrontSupportPercentModifier() const			{ return m_iFrontSupportPercentModifier; }
	int getShortRangeSupportPercentModifier() const		{ return m_iShortRangeSupportPercentModifier; }
	int getMediumRangeSupportPercentModifier() const	{ return m_iMediumRangeSupportPercentModifier; }
	int getLongRangeSupportPercentModifier() const		{ return m_iLongRangeSupportPercentModifier; }
	int getFlankSupportPercentModifier() const			{ return m_iFlankSupportPercentModifier; }
#endif // STRENGTH_IN_NUMBERS
	int getNationalCaptureProbabilityModifier() const;
	int getNationalCaptureResistanceModifier() const;
	int getLocalCaptureProbabilityModifier() const;
	int getLocalCaptureResistanceModifier() const;
	int getLocalDynamicDefense() const;
	int getRiverDefensePenalty() const;
	int getLocalRepel() const;
	int getMinDefense() const;
	int getBuildingDefenseRecoverySpeedModifier() const;
	int getCityDefenseRecoverySpeedModifier() const;
	int getDamageAttackerChance() const;
	int getDamageToAttacker() const;
	int getMaxPopulationAllowed() const;
	int getMaxPopulationChange() const;
	int getInsidiousness() const;
	int getInvestigation() const;
	int getPopulationChange() const;
	int getOneTimePopulationPercentLoss() const;
	int getMaxPopAllowed() const;

	float getVisibilityPriority() const				{ return m_fVisibilityPriority; }

	SpecialBuildingTypes getSpecialBuilding() const	{ return m_eSpecialBuilding; }
	TechTypes getFreeSpecialTech() const			{ return m_eFreeSpecialTech; }
	UnitTypes getPropertySpawnUnit() const;
	PropertyTypes getPropertySpawnProperty() const;
	PromotionLineTypes getPromotionLineType() const;

	const TCHAR* getConstructSound() const			{ return m_szConstructSound; }
	const TCHAR* getArtDefineTag() const			{ return m_szArtDefineTag; }
	const TCHAR* getMovieDefineTag() const			{ return m_szMovieDefineTag; }
	const TCHAR* getButton() const;
	const TCHAR* getMovie() const;
	const CvArtInfoBuilding* getArtInfo() const;
	const CvArtInfoMovie* getMovieInfo() const;

	int getYieldChange(int i) const;
	int* getYieldChangeArray() const;
	int getYieldPerPopChange(int i) const;
	int* getYieldPerPopChangeArray() const;
	int getYieldModifier(int i) const;
	int* getYieldModifierArray() const;
	int getPowerYieldModifier(int i) const;
	int* getPowerYieldModifierArray() const;
	int getAreaYieldModifier(int i) const;
	int* getAreaYieldModifierArray() const;
	int getGlobalYieldModifier(int i) const;
	int* getGlobalYieldModifierArray() const;
	int getSeaPlotYieldChange(int i) const;
	int* getSeaPlotYieldChangeArray() const;
	int getRiverPlotYieldChange(int i) const;
	int* getRiverPlotYieldChangeArray() const;
	int getGlobalSeaPlotYieldChange(int i) const;
	int* getGlobalSeaPlotYieldChangeArray() const;

	int getCommerceChange(int i) const;
	int* getCommerceChangeArray() const;
	int getCommercePerPopChange(int i) const;
	int* getCommercePerPopChangeArray() const;
	int getCommerceChangeDoubleTime(int i) const;
	int getCommerceModifier(int i) const;
	int* getCommerceModifierArray() const;
	int getGlobalCommerceModifier(int i) const;
	int* getGlobalCommerceModifierArray() const;
	int getSpecialistExtraCommerce(int i) const;
	int* getSpecialistExtraCommerceArray() const;
	int getStateReligionCommerce(int i) const;
	int* getStateReligionCommerceArray() const;
	int getCommerceHappiness(int i) const;
	const IDValueMap<ReligionTypes, int>& getReligionChanges() const { return m_religionChange; }
	int getSpecialistCount(int i) const;
	int getFreeSpecialistCount(int i) const;

	int getBonusHealthChanges(int i) const;
	const IDValueMap<BonusTypes, int>& getBonusHealth() const { return m_piBonusHealthChanges; }

	int getBonusHappinessChanges(int i) const;
	const IDValueMap<BonusTypes, int>& getBonusHappiness() const { return m_piBonusHappinessChanges; }

	int getBonusProductionModifier(int i) const;
	int getDomainFreeExperience(int i) const;
	bool isAnyDomainFreeExperience() const { return m_piDomainFreeExperience != NULL; }
	int getDomainProductionModifier(int i) const;

	const IDValueMap<UnitCombatTypes, int>& getUnitCombatFreeExperience() const { return m_aUnitCombatFreeExperience; }

	const std::vector<TechTypes>& getPrereqAndTechs() const;

	const IDValueMap<BuildingTypes, int>& getBuildingHappinessChanges() const { return m_aBuildingHappinessChanges; }

	int getFlavorValue(int i) const;
	int getImprovementFreeSpecialist(int i) const;

	bool isCommerceFlexible(int i) const;

	const IDValueMap<BuildingTypes, int>& getPrereqNumOfBuildings() const { return m_aPrereqNumOfBuilding; }

	int getPrereqOrBuilding(const int i) const;
	short getNumPrereqOrBuilding() const;
	bool isPrereqOrBuilding(const int i) const;

	int getPrereqInCityBuilding(const int i) const;
	short getNumPrereqInCityBuildings() const;
	bool isPrereqInCityBuilding(const int i) const;

	int getPrereqNotInCityBuilding(const int i) const;
	short getNumPrereqNotInCityBuildings() const;

	int getSpecialistYieldChange(int i, int j) const;
	int* getSpecialistYieldChangeArray(int i) const;

	int getSpecialistCommerceChange(int i, int j) const;
	int* getSpecialistCommerceChangeArray(int i) const;

	int getBonusYieldModifier(int i, int j) const;
	int* getBonusYieldModifierArray(int i) const;

	int getGlobalBuildingCommerceChange(int iBuilding, int iCommerce) const;
	const std::vector<BuildingCommerceChange>& getGlobalBuildingCommerceChanges() const;
	const python::list cyGetGlobalBuildingCommerceChanges() const;

	const IDValueMap<UnitTypes, int>& getUnitProductionModifiers() const { return m_aUnitProductionModifier; }

	const IDValueMap<TechTypes, int*>& getTechYieldChanges100() const { return m_techYieldChanges; }
	const python::list cyGetTechYieldChanges100() const;

	const IDValueMap<TechTypes, int*>& getTechYieldModifiers() const { return m_techYieldModifiers; }
	const python::list cyGetTechYieldModifiers() const;

	const IDValueMap<TechTypes, int*>& getTechCommerceChanges100() const { return m_techCommerceChanges; }
	const python::list cyGetTechCommerceChanges100() const;

	const IDValueMap<TechTypes, int*>& getTechCommerceModifiers() const { return m_techCommerceModifiers; }
	const python::list cyGetTechCommerceModifiers() const;

	const IDValueMap<TerrainTypes, int*>& getTerrainYieldChanges() const { return m_aTerrainYieldChanges; }
	const python::list cyGetTerrainYieldChanges() const;

	BonusTypes getExtraFreeBonus(int i) const;
	int getExtraFreeBonusNum(int i) const;
	bool hasExtraFreeBonus(BonusTypes eBonus) const;

	bool isPrereqOrCivics(int iCivic) const;
	bool isPrereqAndCivics(int iCivic) const;
	bool isPrereqOrTerrain(int i) const;
	bool isPrereqAndTerrain(int i) const;
	bool isPrereqOrImprovement(int i) const;
	bool isPrereqOrFeature(int i) const;

	const IDValueMap<BuildingTypes, int>& getBuildingProductionModifiers() const { return m_aBuildingProductionModifier; }
	const IDValueMap<BuildingTypes, int>& getGlobalBuildingProductionModifiers() const { return m_aGlobalBuildingProductionModifier; }
	const IDValueMap<BuildingTypes, int>& getGlobalBuildingCostModifiers() const { return m_aGlobalBuildingCostModifier; }

	int getBonusDefenseChanges(int i) const;

	std::vector<CvString> m_aszPrereqOrCivicsforPass3;
	std::vector<bool> m_abPrereqOrCivicsforPass3;

	int isPrereqOrCivicsVectorSize() const;
	CvString isPrereqOrCivicsNamesVectorElement(int i) const;
	int isPrereqOrCivicsValuesVectorElement(int i) const;

	int isPrereqAndCivicsVectorSize() const;
	CvString isPrereqAndCivicsNamesVectorElement(int i) const;
	int isPrereqAndCivicsValuesVectorElement(int i) const;

	std::vector<CvString> m_aszPrereqAndCivicsforPass3;
	std::vector<bool> m_abPrereqAndCivicsforPass3;

	int getReplacementBuilding(const int i) const;
	short getNumReplacementBuilding() const;

	void setReplacedBuilding(const int i);
	int getReplacedBuilding(const int i) const;
	short getNumReplacedBuilding() const;

	int getImprovementYieldChanges(int i, int j) const;

	int getBonusCommerceModifier(int i, int j) const;
	int* getBonusCommerceModifierArray(int i) const;

	int getBonusYieldChanges(int i, int j) const;
	int* getBonusYieldChangesArray(int i) const;

	int getVicinityBonusYieldChanges(int i, int j) const;
	int* getVicinityBonusYieldChangesArray(int i) const;

	int getBonusCommercePercentChanges(int i, int j) const;
	int* getBonusCommercePercentChangesArray(int i) const;

	int getTechSpecialistChange(int i, int j) const;
	int* getTechSpecialistChangeArray(int i) const;

	const IDValueMap<UnitCombatTypes, int>& getUnitCombatExtraStrength() const { return m_aUnitCombatExtraStrength; }

	int getUnitCombatRetrainType(int i) const;
	int getNumUnitCombatRetrainTypes() const;
	bool isUnitCombatRetrainType(int i) const;

	int getMayDamageAttackingUnitCombatType(int i) const;
	int getNumMayDamageAttackingUnitCombatTypes() const;
	bool isMayDamageAttackingUnitCombatType(int i) const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

	int getNumUnitCombatRepelModifiers() const;
	int getUnitCombatRepelModifier(int iUnitCombat, bool bForLoad = false) const;

	int getNumUnitCombatRepelAgainstModifiers() const;
	int getUnitCombatRepelAgainstModifier(int iUnitCombat, bool bForLoad = false) const;

	int getNumUnitCombatDefenseAgainstModifiers() const;
	int getUnitCombatDefenseAgainstModifier(int iUnitCombat) const;

	int getNumUnitCombatProdModifiers() const;
	int getUnitCombatProdModifier(int iUnitCombat) const;

	int getNumUnitCombatOngoingTrainingDurations() const;
	int getUnitCombatOngoingTrainingDuration(int iUnitCombat, bool bForLoad = false) const;

	int getNumAfflictionOutbreakLevelChanges() const;
	int getAfflictionOutbreakLevelChange(int iAfflictionLine) const;

	int getNumTechOutbreakLevelChanges() const;
	int getTechOutbreakLevelChange(int iTech) const;

	int getLocalSpecialistYieldChange(int i, int j) const;
	int* getLocalSpecialistYieldChangeArray(int i) const;
	bool isAnyLocalSpecialistYieldChanges() const { return m_ppaiLocalSpecialistYieldChange; }

	int getLocalSpecialistCommerceChange(int i, int j) const;
	int* getLocalSpecialistCommerceChangeArray(int i) const;
	bool isAnyLocalSpecialistCommerceChanges() const { return m_ppaiLocalSpecialistCommerceChange; }

	bool isHurry(int i) const;

	const std::vector<FreePromoTypes>& getFreePromoTypes() const { return m_aFreePromoTypes; }
	const python::list cyGetFreePromoTypes() const;

	const std::vector<TraitTypes>& getFreeTraitTypes() const { return m_aiFreeTraitTypes; }

	int getNumHealUnitCombatTypes() const;
	const HealUnitCombat& getHealUnitCombatType(int iUnitCombat) const;

	int getNumBonusAidModifiers() const;
	const BonusAidModifiers& getBonusAidModifier(int iIndex) const;

	int getNumAidRateChanges() const;
	const AidRateChanges& getAidRateChange(int iIndex) const;

	int getNumEnabledCivilizationTypes() const;
	const EnabledCivilizations& getEnabledCivilizationType(int iIndex) const;

	int getVictoryThreshold(int i) const;

	bool isAnySpecialistYieldChanges() const		{ return m_ppaiSpecialistYieldChange != NULL; }
	bool isAnySpecialistCommerceChanges() const		{ return m_ppaiSpecialistCommerceChange != NULL; }
	bool isAnyBonusYieldModifiers() const			{ return m_ppaiBonusYieldModifier != NULL; }
	bool isAnyTechSpecialistChanges() const			{ return m_ppaiTechSpecialistChange != NULL; }
	bool isAnyBonusCommerceModifiers() const		{ return m_ppaiBonusCommerceModifier != NULL; }
	bool isAnyBonusYieldChanges() const				{ return m_ppaiBonusYieldChanges != NULL; }
	bool isAnyVicinityBonusYieldChanges() const		{ return m_ppaiVicinityBonusYieldChanges != NULL; }
	bool isAnyBonusCommercePercentChanges() const	{ return m_ppaiBonusCommercePercentChanges != NULL; }

	const std::vector<BonusTypes>& getPrereqOrBonuses() const { return m_aePrereqOrBonuses; }
	const std::vector<BonusTypes>& getPrereqOrVicinityBonuses() const { return m_piPrereqOrVicinityBonuses; }
	const std::vector<BonusTypes>& getPrereqOrRawVicinityBonuses() const { return m_aePrereqOrRawVicinityBonuses; }

	int getTechHappiness(TechTypes eTech) const;
	const IDValueMap<TechTypes, int>& getTechHappinessChanges() const { return m_aTechHappinessChanges; }

	int getTechHealth(TechTypes eTech) const;
	const IDValueMap<TechTypes, int>& getTechHealthChanges() const { return m_aTechHealthChanges; }

	bool isFreeBonusOfBuilding(BonusTypes eBonus) const;

	bool isNewCityFree(const CvGameObject* pObject);

	const BoolExpr* getConstructCondition() const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	const CvProperties* getProperties() const { return &m_Properties; }
	const CvProperties* getPropertiesAllCities() const { return &m_PropertiesAllCities; }
	const CvProperties* getPrereqMinProperties() const { return &m_PrereqMinProperties; }
	const CvProperties* getPrereqMaxProperties() const { return &m_PrereqMaxProperties; }
	const CvProperties* getPrereqPlayerMinProperties() const { return &m_PrereqPlayerMinProperties; }
	const CvProperties* getPrereqPlayerMaxProperties() const { return &m_PrereqPlayerMaxProperties; }

	void getDataMembers(CvInfoUtil& util);
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	bool readPass3();
	void copyNonDefaults(CvBuildingInfo* pClassInfo);
	void copyNonDefaultsReadPass2(CvBuildingInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver = false);
	void getCheckSum(uint32_t& iSum) const;
	void doPostLoadCaching(BuildingTypes eThis);

private:
	void setNotShowInCity();

	bool m_bNoLimit;
	bool m_bNotShowInCity;
	bool m_bPrereqPower;
	bool m_bApplyFreePromotionOnMove;
	bool m_bNoEnemyPillagingIncome;
	bool m_bProvidesFreshWater;
	bool m_bForceAllTradeRoutes;
	bool m_bForceNoPrereqScaling;
	bool m_bPrereqWar;
	bool m_bRequiresActiveCivics;
	bool m_bZoneOfControl;
	bool m_bProtectedCulture;
	bool m_bTeamShare;
	bool m_bWater;
	bool m_bRiver;
	bool m_bPower;
	bool m_bFreshWater;
	bool m_bOrbital;
	bool m_bOrbitalInfrastructure;
	bool m_bNoHolyCity;
	bool m_bAreaBorderObstacle;
	bool m_bForceTeamVoteEligible;
	bool m_bCapital;
	bool m_bGovernmentCenter;
	bool m_bGoldenAge;
	bool m_bMapCentering;
	bool m_bNoUnhappiness;
	bool m_bNoUnhealthyPopulation;
	bool m_bBuildingOnlyHealthy;
	bool m_bNeverCapture;
	bool m_bNukeImmune;
	bool m_bCenterInCity;
	bool m_bStateReligionInCity;
	bool m_bAllowsNukes;
	bool m_bDamageToAttackerIgnoresArmor;
	bool m_bDamageAllAttackers;
	bool m_bDamageAttackerCapable;
	bool m_bAutoBuild;
	bool m_bQuarantine;
	mutable bool m_bEnablesOtherBuildingsCalculated;
	mutable bool m_bEnablesOtherBuildingsValue;
	bool m_bEnablesUnits;

	int m_iFreePromotion_2;
	int m_iFreePromotion_3;
	int m_iPrereqVicinityBonus;
	int m_iPrereqRawVicinityBonus;
	int m_iGlobalPopulationgrowthratepercentage;
	int m_iPopulationgrowthratepercentage;
	int m_iWorldTradeRoutes;
	int m_iLineOfSight;
	int m_iInflationModifier;
	int m_iPillageGoldModifier;
	int m_iInvasionChance;
	int m_iAdjacentDamagePercent;
	int m_iPrereqPopulation;
	int m_iPrereqCultureLevel;
	int m_iWorkableRadius;
	BuildingTypes m_iPrereqAnyoneBuilding;
	BuildingTypes m_iExtendsBuilding;
	BuildingTypes m_iObsoletesToBuilding;
	int m_iOccupationTimeModifier;
	int m_iNoEntryDefenseLevel;
	int m_iNumUnitFullHeal;
	BuildingTypes m_iProductionContinueBuilding;
	int m_iNumPopulationEmployed;
	int m_iHappinessPercentPerPopulation;
	int m_iHealthPercentPerPopulation;
	int m_iVictoryPrereq;
	EraTypes m_iFreeStartEra;
	int m_iMaxStartEra;
	TechTypes m_iObsoleteTech;
	int m_iPrereqAndTech;
	int m_iNoBonus;
	int m_iPowerBonus;
	int m_iFreeBonus;
	int m_iNumFreeBonuses;
	BuildingTypes m_iFreeBuilding;
	BuildingTypes m_iFreeAreaBuilding;
	int m_iFreePromotion;
	int m_iCivicOption;
	int m_iAIWeight;
	int m_iProductionCost;
	int m_iHurryCostModifier;
	int m_iHurryAngerModifier;
	int m_iMinAreaSize;
	int m_iNumCitiesPrereq;
	int m_iNumTeamsPrereq;
	int m_iUnitLevelPrereq;
	int m_iMinLatitude;
	int m_iMaxLatitude;
	int m_iGreatPeopleRateModifier;
	int m_iGreatGeneralRateModifier;
	int m_iDomesticGreatGeneralRateModifier;
	int m_iGlobalGreatPeopleRateModifier;
	int m_iAnarchyModifier;
	int m_iGoldenAgeModifier;
	int m_iGlobalHurryModifier;
	int m_iFreeExperience;
	int m_iGlobalFreeExperience;
	int m_iFoodKept;
	int m_iAirlift;
	int m_iAirModifier;
	int m_iAirUnitCapacity;
	int m_iNukeModifier;
	int m_iNukeExplosionRand;
	int m_iFreeSpecialist;
	int m_iAreaFreeSpecialist;
	int m_iGlobalFreeSpecialist;
	int m_iHappiness;
	int m_iAreaHappiness;
	int m_iGlobalHappiness;
	int m_iStateReligionHappiness;
	int m_iWorkerSpeedModifier;
	int m_iMilitaryProductionModifier;
	int m_iSpaceProductionModifier;
	int m_iGlobalSpaceProductionModifier;
	int m_iTradeRoutes;
	int m_iCoastalTradeRoutes;
	int m_iGlobalTradeRoutes;
	int m_iTradeRouteModifier;
	int m_iForeignTradeRouteModifier;
	int m_iAssetValue;
	int m_iPowerValue;
	SpecialBuildingTypes m_eSpecialBuilding;
	int m_iAdvisorType;
	int m_iPrereqGameOption;
	int m_iNotGameOption;
	int m_iHolyCity;
	int m_iReligionType;
	int m_iStateReligion;
	int m_iPrereqReligion;
	int m_iPrereqCorporation;
	int m_iFoundsCorporation;
	int m_iGlobalReligionCommerce;
	int m_iGlobalCorporationCommerce;
	int m_iPrereqAndBonus;
	int m_iGreatPeopleUnitType;
	int m_iGreatPeopleRateChange;
	int m_iConquestProbability;
	int m_iMaintenanceModifier;
	int m_iGlobalMaintenanceModifier;
	int m_iAreaMaintenanceModifier;
	int m_iOtherAreaMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCoastalDistanceMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	int m_iWarWearinessModifier;
	int m_iGlobalWarWearinessModifier;
	int m_iEnemyWarWearinessModifier;
	int m_iHealRateChange;
	int m_iHealth;
	int m_iAreaHealth;
	int m_iGlobalHealth;
	int m_iGlobalPopulationChange;
	int m_iFreeTechs;
	int m_iDefenseModifier;
	int m_iBombardDefenseModifier;
	int m_iAllCityDefenseModifier;
	int m_iEspionageDefenseModifier;
	int m_iUnitUpgradePriceModifier;
	int m_iRevIdxLocal;
	int m_iRevIdxNational;
	int m_iRevIdxDistanceModifier;
	int m_iMissionType;
	int m_iVoteSourceType;
	int m_iDCMAirbombMission;
	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iMaxPlayerInstances;
	int m_iExtraPlayerInstances;
	int m_iLinePriority;
	int m_iOutbreakBase;
	int m_iOvercomeBase;
	int m_iTradeCommunicability;
#ifdef STRENGTH_IN_NUMBERS
	int m_iFrontSupportPercentModifier;
	int m_iShortRangeSupportPercentModifier;
	int m_iMediumRangeSupportPercentModifier;
	int m_iLongRangeSupportPercentModifier;
	int m_iFlankSupportPercentModifier;
#endif
	int m_iNationalCaptureProbabilityModifier;
	int m_iNationalCaptureResistanceModifier;
	int m_iLocalCaptureProbabilityModifier;
	int m_iLocalCaptureResistanceModifier;
	int m_iLocalDynamicDefense;
	int m_iRiverDefensePenalty;
	int m_iLocalRepel;
	int m_iMinDefense;
	int m_iBuildingDefenseRecoverySpeedModifier;
	int m_iCityDefenseRecoverySpeedModifier;
	int m_iDamageAttackerChance;
	int m_iDamageToAttacker;
	int m_iMaxPopulationAllowed;
	int m_iMaxPopulationChange;
	int m_iInsidiousness;
	int m_iInvestigation;
	int m_iPopulationChange;
	int m_iOneTimePopulationPercentLoss;
	int m_iMaxPopAllowed;

	float m_fVisibilityPriority;

	TechTypes m_eFreeSpecialTech;
	UnitTypes m_ePropertySpawnUnit;
	PropertyTypes m_ePropertySpawnProperty;
	PromotionLineTypes m_ePromotionLineType;
	//TechTypes m_eFreeSpecialTech;

	CvString m_szConstructSound;
	CvString m_szArtDefineTag;
	CvString m_szMovieDefineTag;

	bool* m_pbPrereqOrCivics;
	bool* m_pbPrereqAndCivics;
	bool* m_pbPrereqOrTerrain;
	bool* m_pbPrereqAndTerrain;
	bool* m_pbPrereqOrImprovement;
	bool* m_pbPrereqOrFeature;
	bool* m_pbCommerceFlexible;
	bool* m_pabHurry;

	int* m_piBonusDefenseChanges;
	int* m_piSeaPlotYieldChange;
	int* m_piRiverPlotYieldChange;
	int* m_piGlobalSeaPlotYieldChange;
	int* m_piYieldChange;
	int* m_piYieldPerPopChange;
	int* m_piYieldModifier;
	int* m_piPowerYieldModifier;
	int* m_piAreaYieldModifier;
	int* m_piGlobalYieldModifier;
	int* m_piCommerceChange;
	int* m_piCommercePerPopChange;
	int* m_piCommerceChangeDoubleTime;
	int* m_piCommerceModifier;
	int* m_piGlobalCommerceModifier;
	int* m_piSpecialistExtraCommerce;
	int* m_piStateReligionCommerce;
	int* m_piCommerceHappiness;
	int* m_piSpecialistCount;
	int* m_piFreeSpecialistCount;
	int* m_piBonusProductionModifier;
	int* m_piDomainFreeExperience;
	int* m_piDomainProductionModifier;
	int* m_piFlavorValue;
	int* m_piImprovementFreeSpecialist;
	int* m_piVictoryThreshold;

	int** m_ppaiBonusCommerceModifier;
	int** m_ppaiTechSpecialistChange;
	int** m_ppiImprovementYieldChanges;
	int** m_ppaiBonusYieldChanges;
	int** m_ppaiBonusCommercePercentChanges;
	int** m_ppaiVicinityBonusYieldChanges;
	int** m_ppaiSpecialistYieldChange;
	int** m_ppaiSpecialistCommerceChange;
	int** m_ppaiBonusYieldModifier;
	int** m_ppaiLocalSpecialistYieldChange;
	int** m_ppaiLocalSpecialistCommerceChange;

	std::vector<BonusTypes> m_piPrereqOrVicinityBonuses;
	std::vector<BonusTypes> m_aePrereqOrRawVicinityBonuses;
	std::vector<TechTypes> m_piPrereqAndTechs;
	//std::vector<int> m_aiFreePromoTypes;
	std::vector<int> m_aiUnitCombatRetrainTypes;
	std::vector<int> m_aiMayDamageAttackingUnitCombatTypes;
	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
	std::vector<int> m_aiPrereqInCityBuildings;
	std::vector<int> m_vPrereqNotInCityBuildings;
	std::vector<BonusTypes> m_aePrereqOrBonuses;
	std::vector<int> m_vPrereqOrBuilding;
	std::vector<int> m_vReplacementBuilding;
	std::vector<int> m_vReplacedBuilding;
	std::vector<FreePromoTypes> m_aFreePromoTypes;
	std::vector<TraitTypes> m_aiFreeTraitTypes;
	std::vector<EnabledCivilizations> m_aEnabledCivilizationTypes;

	std::vector<HealUnitCombat> m_aHealUnitCombatTypes;
	std::vector<BonusAidModifiers> m_aBonusAidModifiers;
	std::vector<AidRateChanges> m_aAidRateChanges;
	std::vector<BuildingCommerceChange> m_aGlobalBuildingCommerceChanges;
	UnitCombatModifierArray m_aUnitCombatRepelModifiers;
	UnitCombatModifierArray m_aUnitCombatRepelAgainstModifiers;
	UnitCombatModifierArray m_aUnitCombatDefenseAgainstModifiers;
	UnitCombatModifierArray m_aUnitCombatProdModifiers;
	UnitCombatModifierArray m_aUnitCombatOngoingTrainingDurations;
	PromotionLineModifierArray m_aAfflictionOutbreakLevelChanges;
	TechModifierArray m_aTechOutbreakLevelChanges;
	IDValueMap<BonusTypes, int> m_piBonusHealthChanges;
	IDValueMap<BonusTypes, int> m_piBonusHappinessChanges;
	IDValueMap<BuildingTypes, int> m_aBuildingProductionModifier;
	IDValueMap<BuildingTypes, int> m_aGlobalBuildingProductionModifier;
	IDValueMap<BuildingTypes, int> m_aPrereqNumOfBuilding;
	IDValueMap<BuildingTypes, int> m_aBuildingHappinessChanges;
	IDValueMap<BuildingTypes, int> m_aGlobalBuildingCostModifier;
	IDValueMap<ReligionTypes, int> m_religionChange;
	IDValueMap<TechTypes, int> m_aTechHappinessChanges;
	IDValueMap<TechTypes, int> m_aTechHealthChanges;
	IDValueMap<UnitCombatTypes, int> m_aUnitCombatExtraStrength;
	IDValueMap<UnitCombatTypes, int> m_aUnitCombatFreeExperience;
	IDValueMap<UnitTypes, int> m_aUnitProductionModifier;
	std::vector<std::pair<BonusTypes, int> > m_aExtraFreeBonuses;

	IDValueMap<TechTypes, int*> m_techYieldChanges;
	IDValueMap<TechTypes, int*> m_techYieldModifiers;
	IDValueMap<TechTypes, int*> m_techCommerceChanges;
	IDValueMap<TechTypes, int*> m_techCommerceModifiers;
	IDValueMap<TerrainTypes, int*> m_aTerrainYieldChanges;

	CvPropertyManipulators m_PropertyManipulators;

	CvProperties m_Properties;
	CvProperties m_PropertiesAllCities;
	CvProperties m_PrereqMinProperties;
	CvProperties m_PrereqMaxProperties;
	CvProperties m_PrereqPlayerMinProperties;
	CvProperties m_PrereqPlayerMaxProperties;

	const BoolExpr* m_pExprNewCityFree;
	const BoolExpr* m_pExprConstructCondition;
	//const BoolExpr* m_pExprFreePromotionCondition;
};

#endif
