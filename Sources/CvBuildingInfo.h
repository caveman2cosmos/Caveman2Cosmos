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
class CvArtInfoBuilding;
class CvArtInfoMovie;
class CvBuildingInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBuildingInfo();
	virtual ~CvBuildingInfo();

	int getMaxGlobalInstances() const   { return m_iMaxGlobalInstances; } // Exposed to Python
	int getMaxTeamInstances() const     { return m_iMaxTeamInstances; } // Exposed to Python
	int getMaxPlayerInstances() const   { return m_iMaxPlayerInstances; } // Exposed to Python
	int getExtraPlayerInstances() const { return m_iExtraPlayerInstances; } // Exposed to Python
	bool isNoLimit() const; // Exposed to Python

	int getVictoryPrereq() const; // Exposed to Python
	int getFreeStartEra() const; // Exposed to Python
	int getMaxStartEra() const; // Exposed to Python
	int getObsoleteTech() const; // Exposed to Python
	int getPrereqAndTech() const; // Exposed to Python
	int getNoBonus() const; // Exposed to Python
	int getPowerBonus() const; // Exposed to Python
	int getFreeBonus() const; // Exposed to Python
	int getNumFreeBonuses() const; // Exposed to Python
	int getNumExtraFreeBonuses() const;
	BonusTypes getExtraFreeBonus(int i) const;
	int getExtraFreeBonusNum(int i) const;
	bool hasExtraFreeBonus(BonusTypes eBonus) const;
	int getFreeBuilding() const; // Exposed to Python
	int getFreeAreaBuilding() const;
	int getFreePromotion() const; // Exposed to Python
	int getCivicOption() const; // Exposed to Python
	int getAIWeight() const; // Exposed to Python
	int getProductionCost() const; // Exposed to Python
	int getHurryCostModifier() const; // Exposed to Python
	int getHurryAngerModifier() const; // Exposed to Python
	int getAdvancedStartCost() const; // Exposed to Python
	int getAdvancedStartCostIncrease() const; // Exposed to Python
	int getMinAreaSize() const; // Exposed to Python
	int getNumCitiesPrereq() const; // Exposed to Python
	int getNumTeamsPrereq() const; // Exposed to Python
	int getUnitLevelPrereq() const; // Exposed to Python
	int getMinLatitude() const; // Exposed to Python
	int getMaxLatitude() const; // Exposed to Python
	int getGreatPeopleRateModifier() const; // Exposed to Python
	int getGreatGeneralRateModifier() const; // Exposed to Python
	int getDomesticGreatGeneralRateModifier() const; // Exposed to Python
	int getGlobalGreatPeopleRateModifier() const; // Exposed to Python
	int getAnarchyModifier() const; // Exposed to Python
	int getGoldenAgeModifier() const; // Exposed to Python
	int getGlobalHurryModifier() const; // Exposed to Python
	int getFreeExperience() const; // Exposed to Python
	int getGlobalFreeExperience() const; // Exposed to Python
	int getFoodKept() const; // Exposed to Python
	int getAirlift() const; // Exposed to Python
	int getAirModifier() const; // Exposed to Python
	int getAirUnitCapacity() const; // Exposed to Python
	int getNukeModifier() const; // Exposed to Python
	int getNukeExplosionRand() const; // Exposed to Python
	int getFreeSpecialist() const; // Exposed to Python
	int getAreaFreeSpecialist() const; // Exposed to Python
	int getGlobalFreeSpecialist() const; // Exposed to Python
	int getHappiness() const; // Exposed to Python
	int getAreaHappiness() const; // Exposed to Python
	int getGlobalHappiness() const; // Exposed to Python
	int getStateReligionHappiness() const; // Exposed to Python
	int getWorkerSpeedModifier() const; // Exposed to Python
	int getMilitaryProductionModifier() const; // Exposed to Python
	int getSpaceProductionModifier() const; // Exposed to Python
	int getGlobalSpaceProductionModifier() const; // Exposed to Python
	int getTradeRoutes() const; // Exposed to Python
	int getCoastalTradeRoutes() const; // Exposed to Python
	int getGlobalTradeRoutes() const; // Exposed to Python
	int getTradeRouteModifier() const; // Exposed to Python
	int getForeignTradeRouteModifier() const; // Exposed to Python
	int getAssetValue(bool bForLoad = false) const; // Exposed to Python
	int getPowerValue(bool bForLoad = false) const; // Exposed to Python
	int getSpecialBuildingType() const; // Exposed to Python
	int getAdvisorType() const; // Exposed to Python

	// REVDCM CanConstruct 2/16/10 by phungus420
	int getPrereqGameOption() const; // Exposed to Python
	int getNotGameOption() const; // Exposed to Python

	int getHolyCity() const; // Exposed to Python
	int getReligionType() const; // Exposed to Python
	int getStateReligion() const; // Exposed to Python
	int getPrereqReligion() const; // Exposed to Python
	int getPrereqCorporation() const; // Exposed to Python
	int getFoundsCorporation() const; // Exposed to Python
	int getGlobalReligionCommerce() const; // Exposed to Python
	int getGlobalCorporationCommerce() const; // Exposed to Python
	int getPrereqAndBonus() const; // Exposed to Python
	int getGreatPeopleUnitType() const; // Exposed to Python
	int getGreatPeopleRateChange() const; // Exposed to Python
	int getConquestProbability() const; // Exposed to Python
	int getMaintenanceModifier() const; // Exposed to Python
	//DPII < Maintenance Modifiers >
	int getGlobalMaintenanceModifier() const;
	int getAreaMaintenanceModifier() const;
	int getOtherAreaMaintenanceModifier() const;
	int getDistanceMaintenanceModifier() const;
	int getNumCitiesMaintenanceModifier() const;
	int getCoastalDistanceMaintenanceModifier() const;
	int getConnectedCityMaintenanceModifier() const;
	//DPII < Maintenance Modifiers >
	int getWarWearinessModifier() const; // Exposed to Python
	int getGlobalWarWearinessModifier() const; // Exposed to Python
	int getEnemyWarWearinessModifier() const; // Exposed to Python
	int getHealRateChange() const; // Exposed to Python
	int getHealth() const; // Exposed to Python
	int getAreaHealth() const; // Exposed to Python
	int getGlobalHealth() const; // Exposed to Python
	int getGlobalPopulationChange() const; // Exposed to Python
	int getFreeTechs() const; // Exposed to Python
	TechTypes getFreeSpecialTech() const;
	int getDefenseModifier() const; // Exposed to Python
	int getBombardDefenseModifier() const; // Exposed to Python
	int getAllCityDefenseModifier() const; // Exposed to Python
	int getEspionageDefenseModifier() const; // Exposed to Python

	// REVDCM Building Effects 4/09/10 by phungus420
	int getUnitUpgradePriceModifier() const; // Exposed to Python
	int getRevIdxLocal() const; // Exposed to Python
	int getRevIdxNational() const; // Exposed to Python
	int getRevIdxDistanceModifier() const; // Exposed to Python

	int getMissionType() const; // Exposed to Python
	void setMissionType(int iNewType);
	int getVoteSourceType() const; // Exposed to Python

	float getVisibilityPriority() const;

	bool isTeamShare() const; // Exposed to Python
	bool isAutoBuild() const;
	bool isWater() const; // Exposed to Python
	bool isRiver() const; // Exposed to Python
	bool isFreshWater() const; // Exposed to Python
	bool isPower() const; // Exposed to Python
	bool isDirtyPower() const; // Exposed to Python
	bool isAreaCleanPower() const; // Exposed to Python
	bool isOrbital() const;
	bool isOrbitalInfrastructure() const;
	bool isNoHolyCity() const;
	bool isAreaBorderObstacle() const; // Exposed to Python
	bool isForceTeamVoteEligible() const; // Exposed to Python
	bool isCapital() const; // Exposed to Python
	// DCM 04/19/09 Johny Smith
	// Dale - AB: Bombing START
	int getDCMAirbombMission() const;
protected:
public:
	// Dale - AB: Bombing END
	bool isGovernmentCenter() const; // Exposed to Python
	bool isGoldenAge() const; // Exposed to Python
	bool isMapCentering() const; // Exposed to Python
	bool isNoUnhappiness() const; // Exposed to Python
	bool isNoUnhealthyPopulation() const; // Exposed to Python
	bool isBuildingOnlyHealthy() const; // Exposed to Python
	bool isNeverCapture() const; // Exposed to Python
	bool isNukeImmune() const; // Exposed to Python
	bool isCenterInCity() const; // Exposed to Python
	bool isStateReligion() const; // Exposed to Python
	bool isAllowsNukes() const; // Exposed to Python

	const TCHAR* getConstructSound() const; // Exposed to Python
	const TCHAR* getArtDefineTag() const; // Exposed to Python
	const TCHAR* getMovieDefineTag() const; // Exposed to Python

	// Arrays

	int getYieldChange(int i) const; // Exposed to Python
	int* getYieldChangeArray() const;
	int getYieldPerPopChange(int i) const; // Exposed to Python
	int* getYieldPerPopChangeArray() const;
	int getYieldModifier(int i) const; // Exposed to Python
	int* getYieldModifierArray() const;
	int getPowerYieldModifier(int i) const; // Exposed to Python
	int* getPowerYieldModifierArray() const;
	int getAreaYieldModifier(int i) const; // Exposed to Python
	int* getAreaYieldModifierArray() const;
	int getGlobalYieldModifier(int i) const; // Exposed to Python
	int* getGlobalYieldModifierArray() const;
	int getSeaPlotYieldChange(int i) const; // Exposed to Python
	int* getSeaPlotYieldChangeArray() const;
	int getRiverPlotYieldChange(int i) const; // Exposed to Python
	int* getRiverPlotYieldChangeArray() const;
	int getGlobalSeaPlotYieldChange(int i) const; // Exposed to Python
	int* getGlobalSeaPlotYieldChangeArray() const;

	int getCommerceChange(int i) const; // Exposed to Python
	int* getCommerceChangeArray() const;
	int getCommercePerPopChange(int i) const;
	int* getCommercePerPopChangeArray() const;
	int getObsoleteSafeCommerceChange(int i) const; // Exposed to Python
	int* getObsoleteSafeCommerceChangeArray() const;
	int getCommerceChangeDoubleTime(int i) const; // Exposed to Python
	int getCommerceModifier(int i) const; // Exposed to Python
	int* getCommerceModifierArray() const;
	int getGlobalCommerceModifier(int i) const; // Exposed to Python
	int* getGlobalCommerceModifierArray() const;
	int getSpecialistExtraCommerce(int i) const; // Exposed to Python
	int* getSpecialistExtraCommerceArray() const;
	int getStateReligionCommerce(int i) const; // Exposed to Python
	int* getStateReligionCommerceArray() const;
	int getCommerceHappiness(int i) const; // Exposed to Python
	int getReligionChange(int i) const; // Exposed to Python
	int getSpecialistCount(int i) const; // Exposed to Python
	int getFreeSpecialistCount(int i) const; // Exposed to Python
	int getBonusHealthChanges(int i) const; // Exposed to Python
	int getBonusHappinessChanges(int i) const; // Exposed to Python
	int getBonusProductionModifier(int i) const; // Exposed to Python
	int getUnitCombatFreeExperience(int i) const; // Exposed to Python
	int getDomainFreeExperience(int i) const; // Exposed to Python
	bool isAnyUnitCombatFreeExperience() const;
	bool isAnyDomainFreeExperience() const;
	int getDomainProductionModifier(int i) const; // Exposed to Python
	int getPrereqAndTechs(int i) const; // Exposed to Python
	int getBuildingHappinessChanges(int i) const; // Exposed to Python
	int getPrereqNumOfBuilding(int i) const; // Exposed to Python

	int getFlavorValue(int i) const; // Exposed to Python
	int getImprovementFreeSpecialist(int i) const; // Exposed to Python

	bool isCommerceFlexible(int i) const; // Exposed to Python
	bool isCommerceChangeOriginalOwner(int i) const; // Exposed to Python

	int getPrereqInCityBuilding(int i) const;
	int getNumPrereqInCityBuildings() const;
	bool isPrereqInCityBuilding(int i) const; // Exposed to Python


	int getSpecialistYieldChange(int i, int j) const; // Exposed to Python
	int* getSpecialistYieldChangeArray(int i) const;

	int getSpecialistCommerceChange(int i, int j) const; // Exposed to Python
	int* getSpecialistCommerceChangeArray(int i) const;

	int getBonusYieldModifier(int i, int j) const; // Exposed to Python
	int* getBonusYieldModifierArray(int i) const;

	int getGlobalBuildingCommerceChange(int iBuilding, int iCommerce) const;
	int getNumGlobalBuildingCommerceChanges() const;

	// Other

	const CvArtInfoBuilding* getArtInfo() const;
	const CvArtInfoMovie* getMovieInfo() const;
	const TCHAR* getButton() const;
	const TCHAR* getMovie() const;

	// serialization
	void read(FDataStreamBase*) {}
	void write(FDataStreamBase*) {}
	bool read(CvXMLLoadUtility* pXML);

	// Afforess 12/9/09
	int getFreePromotion_2() const;
	int getFreePromotion_3() const;
	int getPrereqVicinityBonus() const;
	int getPrereqRawVicinityBonus() const;
	int getGlobalPopulationgrowthratepercentage() const;
	int getPopulationgrowthratepercentage() const;
	bool isBuildOnlyOnPeaks() const;
	bool isPrereqPower() const;
	bool isApplyFreePromotionOnMove() const;
	bool isNoEnemyPillagingIncome() const;
	bool isProvidesFreshWater() const;
	bool isForceAllTradeRoutes() const;
	bool isForceNoPrereqScaling() const;
	bool isPrereqWar() const;
	bool isRequiresActiveCivics() const;
	bool isZoneOfControl() const;
	bool isProtectedCulture() const;
	int getPillageGoldModifier() const;
	int getWorldTradeRoutes() const;
	int getLineOfSight() const;
	int getInflationModifier() const;
	int getInvasionChance() const;
	int getAdjacentDamagePercent() const;
	int getPrereqPopulation() const;
	int getProductionContinueBuilding() const;
	int getPrereqCultureLevel() const;
	int getWorkableRadius() const;
	int getPrereqAnyoneBuilding() const;
	int getExtendsBuilding() const;
	int getOccupationTimeModifier() const;
	int getNoEntryDefenseLevel(bool bForLoad = false) const;
	int getNumUnitFullHeal() const;
	int getNumPopulationEmployed() const;
	int getHappinessPercentPerPopulation() const;
	int getHealthPercentPerPopulation() const;

	int getUnitProductionModifier(int i) const;
	int getPrereqOrVicinityBonuses(int i) const;
	int getPrereqOrRawVicinityBonuses(int i) const;

	bool isPrereqOrBuilding(int i) const;

	bool isPrereqOrGameSpeed(int i) const;

	bool isPrereqOrCivics(int iCivic) const;
	bool isPrereqAndCivics(int iCivic) const;

	bool isPrereqOrTerrain(int i) const;		//Exposed to Python
	bool isPrereqAndTerrain(int i) const;		//Exposed to Python
	bool isPrereqOrImprovement(int i) const;	//Exposed to Python
	bool isPrereqOrFeature(int i) const;		//Exposed to Python
	bool isPrereqNotBuilding(int i) const;
	int  getBuildingProductionModifier(int i) const;
	int  getGlobalBuildingProductionModifier(int i) const;
	int  getGlobalBuildingCostModifier(int i) const;

	int  getBonusDefenseChanges(int i) const;

	std::vector<CvString> m_aszPrereqOrCivicsforPass3;
	std::vector<bool> m_abPrereqOrCivicsforPass3;

	int isPrereqOrCivicsVectorSize() const;
	CvString isPrereqOrCivicsNamesVectorElement(const int i) const;
	int isPrereqOrCivicsValuesVectorElement(const int i) const;

	int isPrereqAndCivicsVectorSize() const;
	CvString isPrereqAndCivicsNamesVectorElement(const int i) const;
	int isPrereqAndCivicsValuesVectorElement(const int i) const;

	std::vector<CvString> m_aszPrereqAndCivicsforPass3;
	std::vector<bool> m_abPrereqAndCivicsforPass3;

	std::vector<CvString> m_aszUnitProdModforPass3;
	std::vector<int> m_aiUnitProdModforPass3;

	bool isReplaceBuilding(int i) const;

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

	int getTechHealthChanges(int i) const;		//Exposed to Python
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

	const CvProperties* getProperties() const;
	const CvProperties* getPropertiesAllCities() const;
	const CvProperties* getPrereqMinProperties() const;
	const CvProperties* getPrereqMaxProperties() const;
	const CvProperties* getPrereqPlayerMinProperties() const;
	const CvProperties* getPrereqPlayerMaxProperties() const;

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
	int getFrontSupportPercentModifier() const;
	int getShortRangeSupportPercentModifier() const;
	int getMediumRangeSupportPercentModifier() const;
	int getLongRangeSupportPercentModifier() const;
	int getFlankSupportPercentModifier() const;
	//Team Project (3)
	int getNationalCaptureProbabilityModifier() const;
	int getNationalCaptureResistanceModifier() const;
	int getLocalCaptureProbabilityModifier() const;
	int getLocalCaptureResistanceModifier() const;
	int getLocalDynamicDefense() const;
	int getRiverDefensePenalty() const;
	int getLocalRepel(bool bForLoad = false) const;
	int getMinDefense(bool bForLoad = false) const;
	int getBuildingDefenseRecoverySpeedModifier() const;
	int getCityDefenseRecoverySpeedModifier() const;
	int getDamageAttackerChance() const;
	int getDamageToAttacker() const;
	int getMaxPopulationAllowed(bool bForLoad = false) const;
	int getMaxPopulationChange(bool bForLoad = false) const;
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

	//Team Project (1)
	int getNumTechHappinessTypes() const;
	int getTechHappinessType(int iTech) const;
	const TechModifierArray& getTechHappinessTypeArray() const { return m_aTechHappinessTypes; }

	int getNumTechHealthTypes() const;
	int getTechHealthType(int iTech) const;
	const TechModifierArray& getTechHealthTypeArray() const { return m_aTechHealthTypes; }

	//Arrays
	int getLocalSpecialistYieldChange(int i, int j) const;
	int* getLocalSpecialistYieldChangeArray(int i) const;

	int getLocalSpecialistCommerceChange(int i, int j) const;
	int* getLocalSpecialistCommerceChangeArray(int i) const;

	bool isAnySpecialistCommerceChanges() const;
	bool isAnyLocalSpecialistCommerceChanges() const;
	bool isHurry(int i) const;

	//ls612 Begin:
	int getMaxPopAllowed() const;
	int getMaxCultureLevelAllowed() const;
	//ls612 End:

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

	void getCheckSum(unsigned int& iSum);

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	bool isNewCityFree(CvGameObject* pObject);

	BoolExpr* getConstructCondition() const;

	int getVictoryThreshold(int i) const;

	bool getNotShowInCity() const;

	bool m_bNoLimit;
	bool m_bNotShowInCity;
	bool m_bAnySpecialistYieldChanges;
	bool m_bAnySpecialistCommerceChanges;
	bool m_bAnyBonusYieldModifiers;
	bool m_bAnyTechCommerceChanges;
	bool m_bAnyTechYieldChanges;
	bool m_bAnyTechSpecialistChanges;
	bool m_bAnyTechCommerceModifiers;
	bool m_bAnyTechYieldModifiers;
	bool m_bAnyBonusCommerceModifiers;
	bool m_bAnyBonusYieldChanges;
	bool m_bAnyVicinityBonusYieldChanges;
	bool m_bAnyBonusCommercePercentChanges;

protected:
	void setNotShowInCity();

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
	int* m_piPrereqOrRawVicinityBonuses;
	int* m_piUnitProductionModifier;
	bool* m_pbPrereqOrCivics;
	bool* m_pbPrereqAndCivics;

	int* m_piCommerceAttacks;

	bool* m_pbPrereqOrGameSpeed;
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
	bool* m_pbPrereqNotBuilding;
	bool* m_pbReplaceBuilding;
	bool* m_pbPrereqOrBuilding;

	CvPropertyManipulators m_PropertyManipulators;

public:
	bool readPass2(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvBuildingInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void copyNonDefaultsReadPass2(CvBuildingInfo* pClassInfo, CvXMLLoadUtility* pXML, bool bOver = false);
	//Alberts2 PrereqBonuses
	int getNumPrereqOrBonuses() const; // Exposed to Python
	BonusTypes getPrereqOrBonuses(int i) const; // Exposed to Python

	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------
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
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
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
	bool m_bStateReligion;
	bool m_bAllowsNukes;

	CvString m_szConstructSound;
	CvString m_szArtDefineTag;
	CvString m_szMovieDefineTag;

	// Arrays

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
	int* m_piObsoleteSafeCommerceChange;
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
	bool m_bAnyUnitCombatFreeExperience;
	bool m_bAnyDomainFreeExperience;
	int* m_piDomainProductionModifier;
	std::vector<int> m_aiPrereqInCityBuildings;
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
	//integers
	int m_iLinePriority;
	int m_iOutbreakBase;
	int m_iOvercomeBase;
	int m_iTradeCommunicability;
	int m_iFrontSupportPercentModifier;
	int m_iShortRangeSupportPercentModifier;
	int m_iMediumRangeSupportPercentModifier;
	int m_iLongRangeSupportPercentModifier;
	int m_iFlankSupportPercentModifier;
	//Team Project (3)
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
	//Booleans
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
	//Team Project (1)
	TechModifierArray m_aTechHappinessTypes;
	TechModifierArray m_aTechHealthTypes;
	//arrays
	int** m_ppaiLocalSpecialistYieldChange;
	int** m_ppaiLocalSpecialistCommerceChange;
	//Alberts2 PrereqBonuses
	std::vector<int> m_aePrereqOrBonuses;
public:
	bool m_bAnyLocalSpecialistYieldChanges;
	bool m_bAnyLocalSpecialistCommerceChanges;

protected:
	bool* m_pabHurry;

	//ls612 Begin:
	int m_iMaxPopAllowed;
	int m_iMaxCultureLevelAllowed;
	//ls612 End:

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
