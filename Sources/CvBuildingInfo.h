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

class BoolExpr;
class CvArtInfoBuilding;
class CvArtInfoMovie;
class CvHotkeyInfo;
class CvProperties;
class CvPropertyManipulators;
class CvXMLLoadUtility;
class FDataStreamBase;

class CvBuildingInfo : public CvHotkeyInfo
{
public:
	CvBuildingInfo();
	virtual ~CvBuildingInfo();

	int getMaxGlobalInstances() const				{ return m_iMaxGlobalInstances; }
	int getMaxTeamInstances() const					{ return m_iMaxTeamInstances; }
	int getMaxPlayerInstances() const				{ return m_iMaxPlayerInstances; }
	int getExtraPlayerInstances() const				{ return m_iExtraPlayerInstances; }
	bool isNoLimit() const							{ return m_bNoLimit; }

	int getVictoryPrereq() const					{ return m_iVictoryPrereq; }
	int getFreeStartEra() const						{ return m_iFreeStartEra; }
	int getMaxStartEra() const						{ return m_iMaxStartEra; }
	int getObsoleteTech() const						{ return m_iObsoleteTech; }
	int getPrereqAndTech() const					{ return m_iPrereqAndTech; }
	int getNoBonus() const							{ return m_iNoBonus; }
	int getPowerBonus() const						{ return m_iPowerBonus; }
	int getFreeBonus() const						{ return m_iFreeBonus; }
	int getNumFreeBonuses() const					{ return m_iNumFreeBonuses; }
	int getNumExtraFreeBonuses() const				{ return m_aExtraFreeBonuses.size(); }
	BonusTypes getExtraFreeBonus(int i) const;
	int getExtraFreeBonusNum(int i) const;
	bool hasExtraFreeBonus(BonusTypes eBonus) const;
	int getFreeBuilding() const						{ return m_iFreeBuilding; }
	int getFreeAreaBuilding() const					{ return m_iFreeAreaBuilding; }
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
	int getSpecialBuildingType() const				{ return m_iSpecialBuildingType; }
	int getAdvisorType() const						{ return m_iAdvisorType; }

	// REVDCM CanConstruct 2/16/10 by phungus420
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
	//DPII < Maintenance Modifiers >
	int getGlobalMaintenanceModifier() const		{ return m_iGlobalMaintenanceModifier; }
	int getAreaMaintenanceModifier() const			{ return m_iAreaMaintenanceModifier; }
	int getOtherAreaMaintenanceModifier() const		{ return m_iOtherAreaMaintenanceModifier; }
	int getDistanceMaintenanceModifier() const		{ return m_iDistanceMaintenanceModifier; }
	int getNumCitiesMaintenanceModifier() const		{ return m_iNumCitiesMaintenanceModifier; }
	int getCoastalDistanceMaintenanceModifier() const { return m_iCoastalDistanceMaintenanceModifier; }
	int getConnectedCityMaintenanceModifier() const	{ return m_iConnectedCityMaintenanceModifier; }
	//DPII < Maintenance Modifiers >
	int getWarWearinessModifier() const				{ return m_iWarWearinessModifier; }
	int getGlobalWarWearinessModifier() const		{ return m_iGlobalWarWearinessModifier; }
	int getEnemyWarWearinessModifier() const		{ return m_iEnemyWarWearinessModifier; }
	int getHealRateChange() const					{ return m_iHealRateChange; }
	int getHealth() const							{ return m_iHealth; }
	int getAreaHealth() const						{ return m_iAreaHealth; }
	int getGlobalHealth() const						{ return m_iGlobalHealth; }
	int getGlobalPopulationChange() const			{ return m_iGlobalPopulationChange; }
	int getFreeTechs() const						{ return m_iFreeTechs; }
	TechTypes getFreeSpecialTech() const			{ return m_eFreeSpecialTech; }
	int getDefenseModifier() const					{ return m_iDefenseModifier; }
	int getBombardDefenseModifier() const			{ return m_iBombardDefenseModifier; }
	int getAllCityDefenseModifier() const			{ return m_iAllCityDefenseModifier; }
	int getEspionageDefenseModifier() const			{ return m_iEspionageDefenseModifier; }

	// REVDCM Building Effects 4/09/10 by phungus420
	int getUnitUpgradePriceModifier() const			{ return m_iUnitUpgradePriceModifier; }
	int getRevIdxLocal() const						{ return m_iRevIdxLocal; }
	int getRevIdxNational() const					{ return m_iRevIdxNational; }
	int getRevIdxDistanceModifier() const			{ return m_iRevIdxDistanceModifier; }

	int getMissionType() const						{ return m_iMissionType; }
	void setMissionType(int iNewType)				{ m_iMissionType = iNewType; }
	int getVoteSourceType() const					{ return m_iVoteSourceType; }

	float getVisibilityPriority() const				{ return m_fVisibilityPriority; }

	bool isTeamShare() const						{ return m_bTeamShare; }
	bool isAutoBuild() const						{ return m_bAutoBuild; }
	bool isWater() const							{ return m_bWater; }
	bool isRiver() const							{ return m_bRiver; }
	bool isFreshWater() const						{ return m_bFreshWater; }
	bool isPower() const							{ return m_bPower; }
	bool isDirtyPower() const						{ return m_bDirtyPower; }
	bool isAreaCleanPower() const					{ return m_bAreaCleanPower; }
	bool isOrbital() const							{ return m_bOrbital; }
	bool isOrbitalInfrastructure() const			{ return m_bOrbitalInfrastructure; }
	bool isNoHolyCity() const						{ return m_bNoHolyCity; }
	bool isAreaBorderObstacle() const				{ return m_bAreaBorderObstacle; }
	bool isForceTeamVoteEligible() const			{ return m_bForceTeamVoteEligible; }
	bool isCapital() const							{ return m_bCapital; }
	// DCM 04/19/09 Johny Smith
	// Dale - AB: Bombing START
	int getDCMAirbombMission() const				{ return m_iDCMAirbombMission; }
	// Dale - AB: Bombing END
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

	const TCHAR* getConstructSound() const			{ return m_szConstructSound; }
	const TCHAR* getArtDefineTag() const			{ return m_szArtDefineTag; }
	const TCHAR* getMovieDefineTag() const			{ return m_szMovieDefineTag; }

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
	int getReligionChange(int i) const;
	int getSpecialistCount(int i) const;
	int getFreeSpecialistCount(int i) const;
	int getBonusHealthChanges(int i) const;
	int getBonusHappinessChanges(int i) const;
	int getBonusProductionModifier(int i) const;
	int getUnitCombatFreeExperience(int i) const;
	int getDomainFreeExperience(int i) const;
	bool isAnyUnitCombatFreeExperience() const { return m_piUnitCombatFreeExperience != NULL; }
	bool isAnyDomainFreeExperience() const { return m_piDomainFreeExperience != NULL; }
	int getDomainProductionModifier(int i) const;
	int getPrereqAndTechs(int i) const;
	int getBuildingHappinessChanges(int i) const;
	int getPrereqNumOfBuilding(int i) const;

	int getFlavorValue(int i) const;
	int getImprovementFreeSpecialist(int i) const;

	bool isCommerceFlexible(int i) const;
	bool isCommerceChangeOriginalOwner(int i) const;

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
	int getNumGlobalBuildingCommerceChanges() const;

	const CvArtInfoBuilding* getArtInfo() const;
	const CvArtInfoMovie* getMovieInfo() const;
	const TCHAR* getButton() const;
	const TCHAR* getMovie() const;

	bool read(CvXMLLoadUtility* pXML);

	// Afforess 12/9/09
	int getFreePromotion_2() const					{ return m_iFreePromotion_2; }
	int getFreePromotion_3() const					{ return m_iFreePromotion_3; }
	int getPrereqVicinityBonus() const				{ return m_iPrereqVicinityBonus; }
	int getPrereqRawVicinityBonus() const			{ return m_iPrereqRawVicinityBonus; }
	int getGlobalPopulationgrowthratepercentage() const { return m_iGlobalPopulationgrowthratepercentage; }
	int getPopulationgrowthratepercentage() const	{ return m_iPopulationgrowthratepercentage; }
	bool isBuildOnlyOnPeaks() const					{ return m_bBuildOnlyOnPeaks; }
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
	int getPillageGoldModifier() const				{ return m_iPillageGoldModifier; }
	int getWorldTradeRoutes() const					{ return m_iWorldTradeRoutes; }
	int getLineOfSight() const						{ return m_iLineOfSight; }
	int getInflationModifier() const				{ return m_iInflationModifier; }
	int getInvasionChance() const					{ return m_iInvasionChance; }
	int getAdjacentDamagePercent() const			{ return m_iAdjacentDamagePercent; }
	int getPrereqPopulation() const					{ return m_iPrereqPopulation; }
	int getProductionContinueBuilding() const		{ return m_iProductionContinueBuilding; }
	int getPrereqCultureLevel() const				{ return m_iPrereqCultureLevel; }
	int getWorkableRadius() const					{ return m_iWorkableRadius; }
	int getPrereqAnyoneBuilding() const				{ return m_iPrereqAnyoneBuilding; }
	int getExtendsBuilding() const					{ return m_iExtendsBuilding; }
	int getObsoletesToBuilding() const				{ return m_iObsoletesToBuilding; }
	int getOccupationTimeModifier() const			{ return m_iOccupationTimeModifier; }
	int getNoEntryDefenseLevel() const;
	int getNumUnitFullHeal() const					{ return m_iNumUnitFullHeal; }
	int getNumPopulationEmployed() const			{ return m_iNumPopulationEmployed; }
	int getHappinessPercentPerPopulation() const	{ return m_iHappinessPercentPerPopulation; }
	int getHealthPercentPerPopulation() const		{ return m_iHealthPercentPerPopulation; }

	int getUnitProductionModifier(int i) const;
	int getPrereqOrVicinityBonuses(int i) const;
	const std::vector<BonusTypes> getPrereqOrRawVicinityBonuses() const { return m_aePrereqOrRawVicinityBonuses; }

	bool isPrereqOrCivics(int iCivic) const;
	bool isPrereqAndCivics(int iCivic) const;

	bool isPrereqOrTerrain(int i) const;
	bool isPrereqAndTerrain(int i) const;
	bool isPrereqOrImprovement(int i) const;
	bool isPrereqOrFeature(int i) const;
	int  getBuildingProductionModifier(int i) const;
	int  getGlobalBuildingProductionModifier(int i) const;
	int  getGlobalBuildingCostModifier(int i) const;

	int  getBonusDefenseChanges(int i) const;

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

	std::vector<CvString> m_aszUnitProdModforPass3;
	std::vector<int> m_aiUnitProdModforPass3;

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

	int getTechHappinessChanges(int i) const;
	int* m_piTechHappinessChanges;

	int getTechHealthChanges(int i) const;
	int* m_piTechHealthChanges;

	int getTechCommerceChange(int i, int j) const;
	int* getTechCommerceChangeArray(int i) const;

	int getTechYieldChange(int i, int j) const;
	int* getTechYieldChangeArray(int i) const;

	int getTechSpecialistChange(int i, int j) const;
	int* getTechSpecialistChangeArray(int i) const;

	int getTechCommerceModifier(int i, int j) const;
	int* getTechCommerceModifierArray(int i) const;

	int getTechYieldModifier(int i, int j) const;
	int* getTechYieldModifierArray(int i) const;

	int getUnitCombatExtraStrength(int i) const;

	int getCommerceAttacks(int i) const;
	int* getCommerceAttacksArray() const;

	const CvProperties* getProperties() const { return &m_Properties; }
	const CvProperties* getPropertiesAllCities() const { return &m_PropertiesAllCities; }
	const CvProperties* getPrereqMinProperties() const { return &m_PrereqMinProperties; }
	const CvProperties* getPrereqMaxProperties() const { return &m_PrereqMaxProperties; }
	const CvProperties* getPrereqPlayerMinProperties() const { return &m_PrereqPlayerMinProperties; }
	const CvProperties* getPrereqPlayerMaxProperties() const { return &m_PrereqPlayerMaxProperties; }

	//TB Combat Mods (Buildings) begin
	UnitTypes getPropertySpawnUnit() const;
	PropertyTypes getPropertySpawnProperty() const;
	PromotionLineTypes getPromotionLineType() const;
	//TechTypes getFreeSpecialTech() const;
	//integers
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
#endif
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
	//Booleans
	bool isDamageToAttackerIgnoresArmor() const;
	bool isDamageAllAttackers() const;
	bool isDamageAttackerCapable() const;
	bool isQuarantine() const;

	//boolean vectors without delayed resolution
	//int getFreePromoType(int i) const;
	//int getNumFreePromoTypes() const;
	//bool isFreePromoType(int i);

	int getUnitCombatRetrainType(int i) const;
	int getNumUnitCombatRetrainTypes() const;
	bool isUnitCombatRetrainType(int i) const;

	int getMayDamageAttackingUnitCombatType(int i) const;
	int getNumMayDamageAttackingUnitCombatTypes() const;
	bool isMayDamageAttackingUnitCombatType(int i) const;

	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i) const;

	//integer vectors with pairing method without delayed resolution
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

	int getNumTechHappinessTypes() const;
	int getTechHappinessType(int iTech) const;
	const TechModifierArray& getTechHappinessTypeArray() const { return m_aTechHappinessTypes; }

	int getNumTechHealthTypes() const;
	int getTechHealthType(int iTech) const;
	const TechModifierArray& getTechHealthTypeArray() const { return m_aTechHealthTypes; }

	int getLocalSpecialistYieldChange(int i, int j) const;
	int* getLocalSpecialistYieldChangeArray(int i) const;
	bool isAnyLocalSpecialistYieldChanges() const { return m_ppaiLocalSpecialistYieldChange; }

	int getLocalSpecialistCommerceChange(int i, int j) const;
	int* getLocalSpecialistCommerceChangeArray(int i) const;
	bool isAnyLocalSpecialistCommerceChanges() const { return m_ppaiLocalSpecialistCommerceChange; }

	bool isHurry(int i) const;

	int getMaxPopAllowed() const;

	//BoolExpr* getFreePromotionCondition();
	//Struct
	int getNumFreePromoTypes() const;
	//FreePromoTypes& getFreePromoType(int iPromotion);
	const FreePromoTypes& getFreePromoType(int iPromotion) const;

	int getNumFreeTraitTypes() const;
	int getFreeTraitType(int iIndex) const;

	int getNumHealUnitCombatTypes() const;
	const HealUnitCombat& getHealUnitCombatType(int iUnitCombat) const;

	int getNumBonusAidModifiers() const;
	const BonusAidModifiers& getBonusAidModifier(int iIndex) const;

	int getNumAidRateChanges() const;
	const AidRateChanges& getAidRateChange(int iIndex) const;

	//Bool vector utilizing delayed resolution
	int getNumEnabledCivilizationTypes() const;
	const EnabledCivilizations& getEnabledCivilizationType(int iIndex) const;
	//TB Combat Mods (Buildings) end

	bool EnablesOtherBuildings() const;
	bool isFreeBonusOfBuilding(BonusTypes eBonus) const;
	bool EnablesUnits() const;

	bool readPass3();

	void getCheckSum(unsigned int& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	bool isNewCityFree(CvGameObject* pObject);

	BoolExpr* getConstructCondition() const;

	int getVictoryThreshold(int i) const;

	bool getNotShowInCity() const;

	bool isAnySpecialistYieldChanges() const		{ return m_ppaiSpecialistYieldChange != NULL; }
	bool isAnySpecialistCommerceChanges() const		{ return m_ppaiSpecialistCommerceChange != NULL; }
	bool isAnyBonusYieldModifiers() const			{ return m_ppaiBonusYieldModifier != NULL; }
	bool isAnyTechCommerceChanges() const			{ return m_ppaiTechCommerceChange != NULL; }
	bool isAnyTechYieldChanges() const				{ return m_ppaiTechYieldChange != NULL; }
	bool isAnyTechSpecialistChanges() const			{ return m_ppaiTechSpecialistChange != NULL; }
	bool isAnyTechCommerceModifiers() const			{ return m_ppaiTechCommerceModifier != NULL; }
	bool isAnyTechYieldModifiers() const			{ return m_ppaiTechYieldModifier != NULL; }
	bool isAnyBonusCommerceModifiers() const		{ return m_ppaiBonusCommerceModifier != NULL; }
	bool isAnyBonusYieldChanges() const				{ return m_ppaiBonusYieldChanges != NULL; }
	bool isAnyVicinityBonusYieldChanges() const		{ return m_ppaiVicinityBonusYieldChanges != NULL; }
	bool isAnyBonusCommercePercentChanges() const	{ return m_ppaiBonusCommercePercentChanges != NULL; }

protected:
	void setNotShowInCity();

	bool m_bNoLimit;
	bool m_bNotShowInCity;

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
	int m_iPrereqAnyoneBuilding;
	int m_iExtendsBuilding;
	int m_iObsoletesToBuilding;
	int m_iOccupationTimeModifier;
	int m_iNoEntryDefenseLevel;
	int m_iNumUnitFullHeal;
	int m_iProductionContinueBuilding;
	int m_iNumPopulationEmployed;
	int m_iHappinessPercentPerPopulation;
	int m_iHealthPercentPerPopulation;
	bool m_bBuildOnlyOnPeaks;
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

	int* m_piPrereqOrVicinityBonuses;
	std::vector<BonusTypes> m_aePrereqOrRawVicinityBonuses;
	int* m_piUnitProductionModifier;
	bool* m_pbPrereqOrCivics;
	bool* m_pbPrereqAndCivics;

	int* m_piCommerceAttacks;

	bool* m_pbPrereqOrTerrain;
	bool* m_pbPrereqAndTerrain;
	bool* m_pbPrereqOrImprovement;
	bool* m_pbPrereqOrFeature;
	int* m_piBonusDefenseChanges;
	int** m_ppaiBonusCommerceModifier;
	int* m_piUnitCombatExtraStrength;
	int** m_ppaiTechCommerceChange;
	int** m_ppaiTechYieldChange;
	int** m_ppaiTechSpecialistChange;
	int** m_ppaiTechCommerceModifier;
	int** m_ppaiTechYieldModifier;
	int** m_ppiImprovementYieldChanges;
	int** m_ppaiBonusYieldChanges;
	int** m_ppaiBonusCommercePercentChanges;
	int** m_ppaiVicinityBonusYieldChanges;

private:
	int* m_piBuildingProductionModifier;
	int* m_piGlobalBuildingProductionModifier;
	int* m_piGlobalBuildingCostModifier;
	int* m_piBuildingHappinessChanges;
	int* m_piPrereqNumOfBuilding;

	CvPropertyManipulators m_PropertyManipulators;

public:
	bool readPass2(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvBuildingInfo* pClassInfo);
	void copyNonDefaultsReadPass2(CvBuildingInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver = false);
	//Alberts2 PrereqBonuses
	int getNumPrereqOrBonuses() const;
	BonusTypes getPrereqOrBonuses(int i) const;

protected:
	int m_iVictoryPrereq;
	int m_iFreeStartEra;
	int m_iMaxStartEra;
	int m_iObsoleteTech;
	int m_iPrereqAndTech;
	int m_iNoBonus;
	int m_iPowerBonus;
	int m_iFreeBonus;
	int m_iNumFreeBonuses;
	std::vector<std::pair<BonusTypes,int> > m_aExtraFreeBonuses;

	int m_iFreeBuilding;
	int m_iFreeAreaBuilding;
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
	int m_iSpecialBuildingType;
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
	//DPII < Maintenance Modifier >
	int m_iGlobalMaintenanceModifier;
	int m_iAreaMaintenanceModifier;
	int m_iOtherAreaMaintenanceModifier;
	int m_iDistanceMaintenanceModifier;
	int m_iNumCitiesMaintenanceModifier;
	int m_iCoastalDistanceMaintenanceModifier;
	int m_iConnectedCityMaintenanceModifier;
	//DPII < Maintenance Modifier >
	int m_iWarWearinessModifier;
	int m_iGlobalWarWearinessModifier;
	int m_iEnemyWarWearinessModifier;
	int m_iHealRateChange;
	int m_iHealth;
	int m_iAreaHealth;
	int m_iGlobalHealth;
	int m_iGlobalPopulationChange;
	int m_iFreeTechs;
	TechTypes m_eFreeSpecialTech;
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

	float m_fVisibilityPriority;

	bool m_bTeamShare;
	bool m_bWater;
	bool m_bRiver;
	bool m_bPower;
	bool m_bFreshWater;
	bool m_bDirtyPower;
	bool m_bAreaCleanPower;
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

	CvString m_szConstructSound;
	CvString m_szArtDefineTag;
	CvString m_szMovieDefineTag;

	int* m_piPrereqAndTechs;
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
	int* m_piReligionChange;
	int* m_piSpecialistCount;
	int* m_piFreeSpecialistCount;
	int* m_piBonusHealthChanges;
	int* m_piBonusHappinessChanges;
	int* m_piBonusProductionModifier;
	int* m_piUnitCombatFreeExperience;
	int* m_piDomainFreeExperience;
	int* m_piDomainProductionModifier;
	int* m_piFlavorValue;
	int* m_piImprovementFreeSpecialist;
	int* m_piVictoryThreshold;

	bool* m_pbCommerceFlexible;
	bool* m_pbCommerceChangeOriginalOwner;

	int** m_ppaiSpecialistYieldChange;
	int** m_ppaiSpecialistCommerceChange;
	int** m_ppaiBonusYieldModifier;

	CvProperties m_Properties;
	CvProperties m_PropertiesAllCities;
	CvProperties m_PrereqMinProperties;
	CvProperties m_PrereqMaxProperties;
	CvProperties m_PrereqPlayerMinProperties;
	CvProperties m_PrereqPlayerMaxProperties;

	BoolExpr* m_pExprNewCityFree;
	BoolExpr* m_pExprConstructCondition;

	std::vector<BuildingCommerceChange> m_aGlobalBuildingCommerceChanges;

	//TB Combat Mods (Buildings) begin
	UnitTypes m_ePropertySpawnUnit;
	PropertyTypes m_ePropertySpawnProperty;
	PromotionLineTypes m_ePromotionLineType;
	//TechTypes m_eFreeSpecialTech;

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

	bool m_bDamageToAttackerIgnoresArmor;
	bool m_bDamageAllAttackers;
	bool m_bDamageAttackerCapable;
	bool m_bQuarantine;
	//boolean vectors without delayed resolution
	//std::vector<int> m_aiFreePromoTypes;
	std::vector<int> m_aiUnitCombatRetrainTypes;
	std::vector<int> m_aiMayDamageAttackingUnitCombatTypes;
	std::vector<int> m_aiMapCategoryTypes;
	//integer vectors with pairing without delayed resolution
	UnitCombatModifierArray m_aUnitCombatRepelModifiers;
	UnitCombatModifierArray m_aUnitCombatRepelAgainstModifiers;
	UnitCombatModifierArray m_aUnitCombatDefenseAgainstModifiers;
	UnitCombatModifierArray m_aUnitCombatProdModifiers;
	UnitCombatModifierArray m_aUnitCombatOngoingTrainingDurations;
	PromotionLineModifierArray m_aAfflictionOutbreakLevelChanges;
	TechModifierArray m_aTechOutbreakLevelChanges;
	TechModifierArray m_aTechHappinessTypes;
	TechModifierArray m_aTechHealthTypes;

	int** m_ppaiLocalSpecialistYieldChange;
	int** m_ppaiLocalSpecialistCommerceChange;

	std::vector<int> m_aiPrereqInCityBuildings;
	std::vector<int> m_vPrereqNotInCityBuildings;
	std::vector<int> m_aePrereqOrBonuses;
	std::vector<int> m_vPrereqOrBuilding;
	std::vector<int> m_vReplacementBuilding;
	std::vector<int> m_vReplacedBuilding;

protected:
	bool* m_pabHurry;

	int m_iMaxPopAllowed;

	/*
	BoolExpr* m_pExprFreePromotionCondition;*/
	//Structs
	std::vector<FreePromoTypes> m_aFreePromoTypes;
	std::vector<int> m_aiFreeTraitTypes;
	std::vector<HealUnitCombat> m_aHealUnitCombatTypes;
	std::vector<EnabledCivilizations> m_aEnabledCivilizationTypes;
	std::vector<BonusAidModifiers> m_aBonusAidModifiers;
	std::vector<AidRateChanges> m_aAidRateChanges;
	//TB Combat Mods (Buildings) end

	bool m_bAutoBuild;

	mutable bool m_bEnablesOtherBuildingsCalculated;
	mutable bool m_bEnablesOtherBuildingsValue;
	mutable bool m_bEnablesUnitsCalculated;
	mutable bool m_bEnablesUnits;
};

#endif
