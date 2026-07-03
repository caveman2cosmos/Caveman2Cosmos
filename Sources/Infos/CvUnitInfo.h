#pragma once

#ifndef CV_UNIT_INFO_H
#define CV_UNIT_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoUnit;

class CvUnitInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvUnitInfo();
	virtual ~CvUnitInfo();

	int getMaxGlobalInstances() const;
	int getMaxPlayerInstances() const;
	bool isUnlimitedException() const;
	bool isCivilizationUnit(const PlayerTypes ePlayer=NO_PLAYER) const;

	int getInstanceCostModifier() const;
	int getAIWeight() const;
	int getProductionCost() const;
	int getHurryCostModifier() const;
	int getAdvancedStartCost() const;
	int getMinAreaSize() const;
	int getMoves() const;
	int getAirRange() const;
	int getAirUnitCap() const;
	int getDropRange() const;
	int getNukeRange() const;
	int getWorkRate() const;
	int getBaseDiscover() const;
	int getDiscoverMultiplier() const;
	int getBaseHurry() const;
	int getHurryMultiplier() const;
	int getBaseTrade() const;
	int getTradeMultiplier() const;
	int getGreatWorkCulture() const;
	int getEspionagePoints() const;
	int getCombat() const;

	int getCombatLimit() const;
	//TB Tweaked for Size Matters
	int getAirCombat() const;

	int getAirCombatLimit() const;
	int getXPValueAttack() const;
	int getXPValueDefense() const;
	int getFirstStrikes() const;
	int getChanceFirstStrikes() const;
	int getInterceptionProbability() const;
	//ls612: Advanced Nuke Interception
	//int getNukeInterceptionProbability() const;
	//int getNukeInterceptionRange() const;
	int getEvasionProbability() const;
	int getWithdrawalProbability() const;
	int getCollateralDamage() const;
	int getCollateralDamageLimit() const;
	int getCollateralDamageMaxUnits() const;
	int getCityAttackModifier() const;
	int getCityDefenseModifier() const;
	int getAnimalCombatModifier() const;
	int getHillsAttackModifier() const;
	int getHillsDefenseModifier() const;
	int getBombRate() const;
	int getBombardRate() const;
	int getSpecialCargo() const;
	int getSMNotSpecialCargo() const;
	int getDomainCargo() const;
	int getCargoSpace() const;
	int getSMCargoSpace() const;
	int getSMCargoVolume() const;
	int getConscriptionValue() const;
	int getCultureGarrisonValue() const;
	int getBaseUpkeep() const;
	int getAssetValue() const;
	int getPowerValue() const;
	int getSpecialUnitType() const;
	UnitTypes getUnitCaptureType() const { return m_eUnitCaptureType; }
	int getUnitCombatType() const;
	DomainTypes getDomainType() const;
	UnitAITypes getDefaultUnitAIType() const;
	int getInvisibleType() const;
	int getSeeInvisibleType(int i) const;
	int getNumSeeInvisibleTypes() const;
	int getAdvisorType() const;
	int getMaxStartEra() const;
	int getObsoleteTech() const;
	bool isStateReligion() const;
	int getPrereqGameOption() const;
	int getNotGameOption() const;
	int getHolyCity() const;
	int getReligionType() const;
	int getStateReligion() const;
	int getPrereqReligion() const;
	int getPrereqCorporation() const;
	int getPrereqOrBuildingsNum() const;
	BuildingTypes getPrereqOrBuilding(int i) const;
	int getPrereqAndTech() const;
	int getPrereqAndBonus() const;
	// the initial number of individuals in the unit group
	int getGroupSize() const;
	// the number of UnitMeshGroups for this unit
	int getGroupDefinitions() const;
	int getMeleeWaveSize() const;
	int getRangedWaveSize() const;
	int getNumUnitNames() const;
	int getCommandType() const;
	void setCommandType(int iNewType);
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	bool isFoodProduction() const;
	bool isNoBadGoodies() const;
	bool isOnlyDefensive() const;
	bool isNoCapture() const;
	bool isRivalTerritory() const;
	bool isMilitaryHappiness() const;
	bool isMilitarySupport() const;
	bool isMilitaryProduction() const;
	bool isPillage() const;
	bool isSpy() const;
	bool isSabotage() const;
	bool isDestroy() const;
	bool isStealPlans() const;
	bool isInvestigate() const;
	bool isCounterSpy() const;
	bool isFound() const;
	bool isGoldenAge() const;
	bool isInvisible() const;
	void setInvisible(bool bEnable) ;
	bool isFirstStrikeImmune() const;
	bool isNoDefensiveBonus() const;
	bool isIgnoreBuildingDefense() const;
	bool isCanMoveImpassable() const;
	bool isCanMoveAllTerrain() const;
	bool isFlatMovementCost() const;
	bool isIgnoreTerrainCost() const;
	bool isNukeImmune() const;
	bool isMechUnit() const;
	bool isRenderBelowWater() const;
	bool isRenderAlways() const;
	bool isSuicide() const;
	bool isLineOfSight() const;
	bool isHiddenNationality() const;
	bool isAlwaysHostile() const;
	bool isFreeDrop() const;
	bool isNoRevealMap() const;
	bool isInquisitor() const;
	//ls612: Can't enter non-Owned cities
	bool isNoNonOwnedEntry() const;

	void setPowerValue(int iNewValue);
	int getPrereqVicinityBonus() const;
	bool isRequiresStateReligionInCity() const;
	bool isWorkerTrade() const;
	bool isMilitaryTrade() const;
	bool isForceUpgrade() const;
	bool isGreatGeneral() const;
	bool isSlave() const;
	bool getPassableRouteNeeded(int i) const;
	int getBaseFoodChange() const;
	int getControlPoints() const;
	int getCommandRange() const;
	const char* getClassicalArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setClassicalArtDefineTag(int i, const char* szVal);
	const char* getRennArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setRennArtDefineTag(int i, const char* szVal);
	const char* getIndustrialArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setIndustrialArtDefineTag(int i, const char* szVal);
	const char* getFutureArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setFutureArtDefineTag(int i, const char* szVal);

	CvWString getCivilizationName(int i) const;
	int getCivilizationNamesVectorSize() const;
	CvWString getCivilizationNamesNamesVectorElement(int i) const;
	CvWString getCivilizationNamesValuesVectorElement(int i) const;

	//TB Combat Mod Begin  TB SubCombat Mod begin
	//Functions
	int getEraInfo() const;

	int getAttackCombatModifier() const;
	int getDefenseCombatModifier() const;
	int getPursuit() const;
	int getEarlyWithdraw() const;
	int getVSBarbs() const;
	int getOverrun() const;
	int getRepel() const;
	int getFortRepel() const;
	int getRepelRetries() const;
	int getUnyielding() const;
	int getKnockback() const;
	int getKnockbackRetries() const;
#ifdef BATTLEWORN
	int getStrAdjperRnd() const;
	int getStrAdjperAtt() const;
	int getStrAdjperDef() const;
	int getWithdrawAdjperAtt() const;
#endif // BATTLEWORN
	int getUnnerve() const;
	int getEnclose() const;
	int getLunge() const;
	int getDynamicDefense() const;
	int getFortitude() const;
#ifdef STRENGTH_IN_NUMBERS
	int getFrontSupportPercent() const;
	int getShortRangeSupportPercent() const;
	int getMediumRangeSupportPercent() const;
	int getLongRangeSupportPercent() const;
	int getFlankSupportPercent() const;
#endif // STRENGTH_IN_NUMBERS
	int getCriticalModifier() const;
	int getEndurance() const;
	int getPoisonProbabilityModifier() const;
	int getCaptureProbabilityModifier() const;
	int getCaptureResistanceModifier() const;
	int getHillsWorkModifier() const;
	int getPeaksWorkModifier() const;
	int getBreakdownChance() const;
	int getBreakdownDamage() const;
	int getTaunt() const;
	int getMaxHP(bool bForLoad = false) const;
	int getDamageModifier() const;
	int getRBombardDamage() const;
	int getRBombardDamageLimit() const;
	int getRBombardDamageMaxUnits() const;
	int getCombatModifierPerSizeMore() const;
	int getCombatModifierPerSizeLess() const;
	int getCombatModifierPerVolumeMore() const;
	int getCombatModifierPerVolumeLess() const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getInsidiousness() const;
	int getInvestigation() const;
	int getStealthStrikes() const;
	int getStealthCombatModifier() const;
	int getTrapDamageMax() const;
	int getTrapDamageMin() const;
	int getTrapComplexity() const;
	int getNumTriggers() const;
	int getAggression() const;
	int getAnimalIgnoresBorders() const;
	int getReligiousCombatModifier() const;

	bool isStampede() const;
	bool isOnslaught() const;
	bool isAttackOnlyCities() const;
	bool isIgnoreNoEntryLevel() const;
	bool isIgnoreZoneofControl() const;
	bool isFliesToMove() const;
	bool isRBombardForceAbility() const;
	bool isNoSelfHeal() const;
	bool isExcile() const;
	bool isPassage() const;
	bool isNoNonOwnedCityEntry() const;
	bool isBarbCoExist() const;
	bool isBlendIntoCity() const;
	bool isUpgradeAnywhere() const;
	bool isAssassin() const;
	bool isStealthDefense() const;
	bool isNoInvisibility() const;
	bool isTriggerBeforeAttack() const;
	bool isAnimal() const;
	bool isWildAnimal() const;
	bool canAnimalIgnoresBorders() const;
	bool canAnimalIgnoresImprovements() const;
	bool canAnimalIgnoresCities() const;
	bool isNoNonTypeProdMods() const;
	bool isGatherHerd() const;
	bool canMergeSplit() const { return m_bCanMergeSplit; }

	UnitCombatTypes getSubCombatType(int i) const;
	int getNumSubCombatTypes() const;
	bool isSubCombatType(UnitCombatTypes e) const;
	const std::vector<UnitCombatTypes>& getSubCombatTypes() const;

	int getCureAfflictionType(int i) const;
	int getNumCureAfflictionTypes() const;
	bool isCureAfflictionType(int i) const;

	int getHealAsType(int i) const;
	int getNumHealAsTypes() const;
	bool isHealAsType(int i) const;

	bool isTerrainImpassableType(TerrainTypes e) const;
	const std::vector<TerrainTypes>& getImpassableTerrains() const { return m_vTerrainImpassableTypes; }

	bool isFeatureImpassableType(FeatureTypes e) const;
	const std::vector<FeatureTypes>& getImpassableFeatures() const { return m_vFeatureImpassableTypes; }

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

	int getTrapSetWithPromotionType(int i) const;
	int getNumTrapSetWithPromotionTypes() const;
	bool isTrapSetWithPromotionType(int i) const;

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	int getNumHealUnitCombatTypes() const;
	const HealUnitCombat& getHealUnitCombatType(int iUnitCombat) const;

	int getNumGroupSpawnUnitCombatTypes() const;
	const GroupSpawnUnitCombat& getGroupSpawnUnitCombatType(int iIndex) const;

	int getNumInvisibleTerrainChanges() const;
	const InvisibleTerrainChanges& getInvisibleTerrainChange(int iIndex) const;

	int getNumInvisibleFeatureChanges() const;
	const InvisibleFeatureChanges& getInvisibleFeatureChange(int iIndex) const;

	int getNumInvisibleImprovementChanges() const;
	const InvisibleImprovementChanges& getInvisibleImprovementChange(int iIndex) const;

	int getNumVisibleTerrainChanges() const;
	const InvisibleTerrainChanges& getVisibleTerrainChange(int iIndex) const;

	int getNumVisibleFeatureChanges() const;
	const InvisibleFeatureChanges& getVisibleFeatureChange(int iIndex) const;

	int getNumVisibleImprovementChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementChange(int iIndex) const;

	int getNumVisibleTerrainRangeChanges() const;
	const InvisibleTerrainChanges& getVisibleTerrainRangeChange(int iIndex) const;

	int getNumVisibleFeatureRangeChanges() const;
	const InvisibleFeatureChanges& getVisibleFeatureRangeChange(int iIndex) const;

	int getNumDistanceAttackCommunicabilityTypeChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementRangeChange(int iIndex) const;

	int getNumVisibleImprovementRangeChanges() const;
	const AfflictionLineChanges& getDistanceAttackCommunicabilityTypeChange(int iIndex) const;

	int getNumEnabledCivilizationTypes() const;
	const EnabledCivilizations& getEnabledCivilizationType(int iIndex) const;

	int getNumFlankingStrikesbyUnitCombatTypes() const;
	int getFlankingStrengthbyUnitCombatType(int iUnitCombat) const;
	bool isFlankingStrikebyUnitCombatType(int iUnitCombat) const;

	int getNumWithdrawOnTerrainTypes() const;
	int getWithdrawOnTerrainType(int iTerrain) const;

	int getNumWithdrawOnFeatureTypes() const;
	int getWithdrawOnFeatureType(int iFeature) const;

	int getNumWithdrawVSUnitCombatTypes() const;
	int getWithdrawVSUnitCombatType(int iUnitCombat) const;
	bool isWithdrawVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getWithdrawVSUnitCombatTypes() const;

	int getNumPursuitVSUnitCombatTypes() const;
	int getPursuitVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isPursuitVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	const UnitCombatModifierArray& getPursuitVSUnitCombatTypes() const;

	int getNumRepelVSUnitCombatTypes() const;
	int getRepelVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isRepelVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	const UnitCombatModifierArray& getRepelVSUnitCombatTypes() const;

	int getNumKnockbackVSUnitCombatTypes() const;
	int getKnockbackVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isKnockbackVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	const UnitCombatModifierArray& getKnockbackVSUnitCombatTypes() const;

	int getNumCriticalVSUnitCombatTypes() const;
	int getCriticalVSUnitCombatType(int iUnitCombat) const;
	bool isCriticalVSUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getCriticalVSUnitCombatTypes() const;

	int getNumTrapDisableUnitCombatTypes() const;
	int getTrapDisableUnitCombatType(int iUnitCombat) const;
	bool isTrapDisableUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getTrapDisableUnitCombatTypes() const;

	int getNumTrapAvoidanceUnitCombatTypes() const;
	int getTrapAvoidanceUnitCombatType(int iUnitCombat) const;
	bool isTrapAvoidanceUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getTrapAvoidanceUnitCombatTypes() const;

	int getNumTrapTriggerUnitCombatTypes() const;
	int getTrapTriggerUnitCombatType(int iUnitCombat) const;
	bool isTrapTriggerUnitCombatType(int iUnitCombat) const;
	const UnitCombatModifierArray& getTrapTriggerUnitCombatTypes() const;

	int getNumVisibilityIntensityTypes() const;
	int getVisibilityIntensityType(int iInvisibility) const;
	bool isVisibilityIntensityType(int iInvisibility) const;
	const InvisibilityArray& getVisibilityIntensityTypes() const;

	int getNumInvisibilityIntensityTypes() const;
	int getInvisibilityIntensityType(int iInvisibility) const;
	bool isInvisibilityIntensityType(int iInvisibility) const;
	const InvisibilityArray& getInvisibilityIntensityTypes() const;

	int getNumVisibilityIntensityRangeTypes() const;
	int getVisibilityIntensityRangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeType(int iInvisibility) const;
	const InvisibilityArray& getVisibilityIntensityRangeTypes() const;

	int getNumTerrainWorkRateModifierTypes() const;
	int getTerrainWorkRateModifierType(int iTerrain) const;
	bool isTerrainWorkRateModifierType(int iTerrain) const;

	int getNumFeatureWorkRateModifierTypes() const;
	int getFeatureWorkRateModifierType(int iFeature) const;
	bool isFeatureWorkRateModifierType(int iFeature) const;

	int getNumBuildWorkRateModifierTypes() const;
	int getBuildWorkRateModifierType(int iBuild) const;
	bool isBuildWorkRateModifierType(int iBuild) const;

	bool hasUnitCombat(UnitCombatTypes eUnitCombat) const;
	int getTotalModifiedCombatStrength100(const bool bSizeMatters) const;
	int getBaseGroupRank() const;
	int getBaseCargoVolume() const;

	bool isQualifiedPromotionType(int i) const;
	bool setQualifiedPromotionType(const int iPromo, std::vector<int>& checklist);
	void setQualifiedPromotionTypes();
	void setCanAnimalIgnores();

	const std::vector<BonusTypes>& getPrereqOrBonuses() const;
	const std::vector<BonusTypes>& getPrereqOrVicinityBonuses() const;
	const std::vector<TechTypes>& getPrereqAndTechs() const;
	const std::vector<HeritageTypes>& getPrereqAndHeritage() const { return m_prereqAndHeritage; }
	const std::vector<HeritageTypes>& getPrereqOrHeritage() const { return m_prereqOrHeritage; }

	virtual const wchar_t* getExtraHoverText() const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	const BoolExpr* getTrainCondition() const;

protected:
	int m_iMaxGlobalInstances;
	int m_iMaxPlayerInstances;
	bool m_bUnlimitedException;
	int m_iInstanceCostModifier;
	bool* m_pbPassableRouteNeeded;
	std::vector<BonusTypes> m_piPrereqOrVicinityBonuses;
	bool m_bWorkerTrade;
	bool m_bMilitaryTrade;
	bool m_bForceUpgrade;
	bool m_bGreatGeneral;
	bool m_bSlave;
	bool m_bRequiresStateReligionInCity;
	mutable bool* m_abHasCombatType;
	int m_iPrereqVicinityBonus;
	int m_iBaseFoodChange;
	int m_iControlPoints;
	int m_iCommandRange;
	int m_zobristValue;
	CvString* m_paszClassicalArtDefineTags;
	CvString* m_paszRennArtDefineTags;
	CvString* m_paszIndustrialArtDefineTags;
	CvString* m_paszFutureArtDefineTags;
	CvWString* m_paszCivilizationNames;
	std::vector<CvString> m_aszCivilizationNamesforPass3;
	std::vector<CvWString> m_aszCivilizationNamesValueforPass3;

	const BoolExpr* m_pExprTrainCondition;

public:
	int getDCMBombRange() const;
	int getDCMBombAccuracy() const;
	bool getDCMAirBomb1() const;
	bool getDCMAirBomb2() const;
	bool getDCMAirBomb3() const;
	bool getDCMAirBomb4() const;
	bool getDCMAirBomb5() const;
	bool getDCMFighterEngage() const;

	float getUnitMaxSpeed() const;
	float getUnitPadTime() const;

	bool canAcquireExperience() const;

	// Arrays
	int getFlavorValue(int i) const;
	int getTerrainAttackModifier(int i) const;
	int getTerrainDefenseModifier(int i) const;
	int getFeatureAttackModifier(int i) const;
	int getFeatureDefenseModifier(int i) const;
	const IDValueMap<UnitTypes, int>& getUnitAttackModifiers() const { return m_piUnitAttackModifier; }
	const IDValueMap<UnitTypes, int>& getUnitDefenseModifiers() const { return m_piUnitDefenseModifier; }
	int getUnitCombatModifier(int i) const;
	int getUnitCombatCollateralImmune(int i) const;
	int getDomainModifier(int i) const;
	int getBonusProductionModifier(int i) const;
	int getUnitGroupRequired(int i) const;
	int getReligionSpreads(int i) const;
	int getCorporationSpreads(int i) const;
	int getTerrainPassableTech(int i) const;
	int getFeaturePassableTech(int i) const;
	const IDValueMap<UnitTypes, int, -1>& getFlankingStrikeUnits() const { return m_piFlankingStrikeUnit; }

	bool isPrereqOrCivics(int i) const;

	const std::vector<BuildTypes>& getBuilds() const { return m_workerBuilds; }
	BuildTypes getBuild(int i) const;
	short getNumBuilds() const;
	bool hasBuild(BuildTypes e) const;

	int getPrereqAndBuilding(int i) const;
	int getNumPrereqAndBuildings() const;
	bool isPrereqAndBuilding(int i) const;
	bool isPrereqOrBuilding(int i) const;

	int getTargetUnit(int i) const;
	int getNumTargetUnits() const;
	bool isTargetUnit(int i) const;

	int getDefendAgainstUnit(int i) const;
	int getNumDefendAgainstUnits() const;
	bool isDefendAgainstUnit(int i) const;

	int getSupersedingUnit(int i) const;
	short getNumSupersedingUnits() const;
	bool isSupersedingUnit(int i) const;

	int getUnitUpgrade(int i) const;
	int getNumUnitUpgrades() const;
	bool isUnitUpgrade(int i) const;

	std::vector<int> getUnitUpgradeChain() const;
	void addUnitToUpgradeChain(int i);

	bool getTargetUnitCombat(int i) const;
	bool getDefenderUnitCombat(int i) const;
	bool getUnitAIType(int i) const;
	bool getNotUnitAIType(int i) const;
	bool getGreatPeoples(int i) const;

	int getBuildings(int i) const;
	bool getHasBuilding(int i) const;
	int getNumBuildings() const;

	int getHeritage(int i) const;
	bool getHasHeritage(int i) const;
	int getNumHeritage() const;

	bool getTerrainNative(int i) const;
	bool getFeatureNative(int i) const;
	bool getFreePromotions(int i) const;
	int getLeaderPromotion() const;
	int getLeaderExperience() const;

	const CvOutcomeList* getKillOutcomeList() const;
	int getNumActionOutcomes() const;
	const CvOutcomeList* getActionOutcomeList(int index) const;
	MissionTypes getActionOutcomeMission(int index) const;
	const CvOutcomeList* getActionOutcomeListByMission(MissionTypes eMission) const;
	const CvOutcomeMission* getOutcomeMission(int index) const;
	const CvOutcomeMission* getOutcomeMissionByMission(MissionTypes eMission) const;

	const char* getEarlyArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setEarlyArtDefineTag(int i, const char* szVal);
	const char* getLateArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setLateArtDefineTag(int i, const char* szVal);
	const char* getMiddleArtDefineTag(int i, UnitArtStyleTypes eStyle) const;
	void setMiddleArtDefineTag(int i, const char* szVal);
	const char* getUnitNames(int i) const;
	const char* getFormationType() const;

	const char* getButton() const;
	void updateArtDefineButton();

	const CvArtInfoUnit* getArtInfo(int i, EraTypes eEra, UnitArtStyleTypes eStyle) const;

	void getDataMembers(CvInfoUtil& util);
	bool read(CvXMLLoadUtility* pXML);
	bool readPass3();
	void copyNonDefaults(CvUnitInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;
	void doPostLoadCaching(uint32_t iThis);

private:
	CvPropertyManipulators m_PropertyManipulators;

	int m_iDCMBombRange;
	int m_iDCMBombAccuracy;
	bool m_bDCMFighterEngage;
	bool m_bDCMAirBomb1;
	bool m_bDCMAirBomb2;
	bool m_bDCMAirBomb3;
	bool m_bDCMAirBomb4;
	bool m_bDCMAirBomb5;

	int m_iAIWeight;
	int m_iProductionCost;
	int m_iHurryCostModifier;
	int m_iAdvancedStartCost;
	int m_iMinAreaSize;
	int m_iMoves;
	int m_iAirRange;
	int m_iAirUnitCap;
	int m_iDropRange;
	int m_iNukeRange;
	int m_iWorkRate;
	int m_iBaseDiscover;
	int m_iDiscoverMultiplier;
	int m_iBaseHurry;
	int m_iHurryMultiplier;
	int m_iBaseTrade;
	int m_iTradeMultiplier;
	int m_iGreatWorkCulture;
	int m_iEspionagePoints;
	int m_iCombat;
	int m_iCombatLimit;
	int m_iAirCombat;
	int m_iAirCombatLimit;
	int m_iXPValueAttack;
	int m_iXPValueDefense;
	int m_iFirstStrikes;
	int m_iChanceFirstStrikes;
	int m_iInterceptionProbability;
	//ls612: Advanced Nuke Interception
	//int m_iNukeInterceptionProbability;
	//int m_iNukeInterceptionRange;
	int m_iEvasionProbability;
	int m_iWithdrawalProbability;
	int m_iCollateralDamage;
	int m_iCollateralDamageLimit;
	int m_iCollateralDamageMaxUnits;
	int m_iCityAttackModifier;
	int m_iCityDefenseModifier;
	int m_iAnimalCombatModifier;
	int m_iHillsAttackModifier;
	int m_iHillsDefenseModifier;
	int m_iBombRate;
	int m_iBombardRate;
	int m_iSpecialCargo;
	int m_iSMNotSpecialCargo;
	int m_iDomainCargo;
	int m_iCargoSpace;
	int m_iSMCargoSpace;
	int m_iSMCargoVolume;
	int m_iConscriptionValue;
	int m_iCultureGarrisonValue;
	int m_iBaseUpkeep;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iSpecialUnitType;
	UnitTypes m_eUnitCaptureType;
	int m_iUnitCombatType;
	DomainTypes m_iDomainType;
	UnitAITypes m_iDefaultUnitAIType;
	int m_iInvisibleType;
	int m_iAdvisorType;
	int m_iMaxStartEra;
	int m_iObsoleteTech;
	bool m_bStateReligion;
	int m_iPrereqGameOption;
	int m_iNotGameOption;
	int m_iHolyCity;
	int m_iReligionType;
	int m_iStateReligion;
	int m_iPrereqReligion;
	int m_iPrereqCorporation;
	int m_iPrereqAndTech;
	int m_iPrereqAndBonus;

	std::vector<BuildTypes> m_workerBuilds;
	std::vector<HeritageTypes> m_prereqAndHeritage;
	std::vector<HeritageTypes> m_prereqOrHeritage;
	std::vector<int> m_aiPrereqAndBuildings;
	std::vector<int> m_aiPrereqOrBuildings;
	std::vector<int> m_aiTargetUnit;
	std::vector<int> m_aiDefendAgainstUnit;
	std::vector<int> m_aiSupersedingUnits;
	std::vector<int> m_aiUnitUpgrades;
	std::vector<int> m_aiUnitUpgradeChain;

	int m_iGroupSize;
	int m_iGroupDefinitions;
	int m_iUnitMeleeWaveSize;
	int m_iUnitRangedWaveSize;
	int m_iNumUnitNames;
	int m_iCommandType;
	int m_iLeaderExperience;

	bool m_bFoodProduction;
	bool m_bNoBadGoodies;
	bool m_bOnlyDefensive;
	bool m_bNoCapture;
	bool m_bRivalTerritory;
	bool m_bMilitaryHappiness;
	bool m_bMilitarySupport;
	bool m_bMilitaryProduction;
	bool m_bPillage;
	bool m_bSpy;
	bool m_bSabotage;
	bool m_bDestroy;
	bool m_bStealPlans;
	bool m_bInvestigate;
	bool m_bCounterSpy;
	bool m_bFound;
	bool m_bGoldenAge;
	bool m_bInvisible;
	bool m_bFirstStrikeImmune;
	bool m_bNoDefensiveBonus;
	bool m_bIgnoreBuildingDefense;
	bool m_bCanMoveImpassable;
	bool m_bCanMoveAllTerrain;
	bool m_bFlatMovementCost;
	bool m_bIgnoreTerrainCost;
	bool m_bNukeImmune;
	bool m_bMechanized;
	bool m_bRenderBelowWater;
	bool m_bRenderAlways;
	bool m_bSuicide;
	bool m_bLineOfSight;
	bool m_bHiddenNationality;
	bool m_bAlwaysHostile;
	bool m_bFreeDrop;
	bool m_bNoRevealMap;
	bool m_bInquisitor;

	//ls612: Can't enter non-Owned cities
	bool m_bNoNonOwnedEntry;

	int m_iLeaderPromotion;

	float m_fUnitMaxSpeed;
	float m_fUnitPadTime;

	// Arrays

	std::vector<TechTypes> m_piPrereqAndTechs;
	std::vector<BonusTypes> m_piPrereqOrBonuses;
	int* m_piFlavorValue;
	int* m_piTerrainAttackModifier;
	int* m_piTerrainDefenseModifier;
	int* m_piFeatureAttackModifier;
	int* m_piFeatureDefenseModifier;
	IDValueMap<UnitTypes, int> m_piUnitAttackModifier;
	IDValueMap<UnitTypes, int> m_piUnitDefenseModifier;
	int* m_piUnitCombatModifier;
	int* m_piUnitCombatCollateralImmune;
	int* m_piDomainModifier;
	int* m_piBonusProductionModifier;
	int* m_piUnitGroupRequired;
	int* m_piReligionSpreads;
	int* m_piCorporationSpreads;
	int* m_piTerrainPassableTech;
	int* m_piFeaturePassableTech;
	IDValueMap<UnitTypes, int, -1> m_piFlankingStrikeUnit;

	bool* m_pbPrereqOrCivics;

	bool* m_pbTargetUnitCombat;
	bool* m_pbDefenderUnitCombat;
	bool* m_pbUnitAIType;
	bool* m_pbNotUnitAIType;
	bool* m_pbGreatPeoples;
	std::vector<int> m_pbBuildings;
	std::vector<int> m_addHeritage;
	bool* m_pbTerrainNative;
	bool* m_pbFeatureNative;
	//bool* m_pbTerrainImpassable;
	//bool* m_pbFeatureImpassable;
	bool* m_pbFreePromotions;

	CvString* m_paszEarlyArtDefineTags;
	CvString* m_paszLateArtDefineTags;
	CvString* m_paszMiddleArtDefineTags;
	CvString* m_paszUnitNames;
	CvString m_szFormationType;
	CvString m_szArtDefineButton;
	CvWString m_szExtraHoverTextKey;

	std::vector<int> m_aiSeeInvisibleTypes;

	CvOutcomeList m_KillOutcomeList;
	std::vector<const CvOutcomeMission*> m_aOutcomeMissions;

	//TB Combat Mods Start  TB SubCombat Mod begin
	//integers
	int m_iAttackCombatModifier;
	int m_iDefenseCombatModifier;
	int m_iPursuit;
	int m_iEarlyWithdraw;
	int m_iVSBarbs;
	int m_iOverrun;
	int m_iRepel;
	int m_iFortRepel;
	int m_iRepelRetries;
	int m_iUnyielding;
	int m_iKnockback;
	int m_iKnockbackRetries;
#ifdef BATTLEWORN
	int m_iStrAdjperRnd;
	int m_iStrAdjperAtt;
	int m_iStrAdjperDef;
	int m_iWithdrawAdjperAtt;
#endif // BATTLEWORN
	int m_iUnnerve;
	int m_iEnclose;
	int m_iLunge;
	int m_iDynamicDefense;
	int m_iFortitude;
#ifdef STRENGTH_IN_NUMBERS
	int m_iFrontSupportPercent;
	int m_iShortRangeSupportPercent;
	int m_iMediumRangeSupportPercent;
	int m_iLongRangeSupportPercent;
	int m_iFlankSupportPercent;
#endif
	int m_iCriticalModifier;
	int m_iEndurance;
	int m_iPoisonProbabilityModifier;
	int m_iCaptureProbabilityModifier;
	int m_iCaptureResistanceModifier;
	//WorkRateMod
	int m_iHillsWorkModifier;
	int m_iPeaksWorkModifier;

	int m_iBreakdownChance;
	int m_iBreakdownDamage;
	int m_iTaunt;
	int m_iMaxHP;
	int m_iDamageModifier;
	int m_iTotalCombatStrengthModifierBase;
	int m_iTotalCombatStrengthChangeBase;
	int m_iBaseCargoVolume;
	int m_iRBombardDamage;
	int m_iRBombardDamageLimit;
	int m_iRBombardDamageMaxUnits;
	int m_iBaseGroupRank;
	int m_iCombatModifierPerSizeMore;
	int m_iCombatModifierPerSizeLess;
	int m_iCombatModifierPerVolumeMore;
	int m_iCombatModifierPerVolumeLess;
	int m_iSelfHealModifier;
	int m_iNumHealSupport;
	int m_iInsidiousness;
	int m_iInvestigation;
	int m_iStealthStrikes;
	int m_iStealthCombatModifier;
	int m_iTrapDamageMax;
	int m_iTrapDamageMin;
	int m_iTrapComplexity;
	int m_iNumTriggers;
	int m_iAggression;
	int m_iAnimalIgnoresBorders;
	int m_iReligiousCombatModifier;

	//booleans
	bool m_bStampede;
	bool m_bOnslaught;
	bool m_bAttackOnlyCities;
	bool m_bIgnoreNoEntryLevel;
	bool m_bIgnoreZoneofControl;
	bool m_bFliesToMove;
	bool m_bRBombardForceAbility;
	bool m_bNoSelfHeal;
	bool m_bExcile;
	bool m_bPassage;
	bool m_bNoNonOwnedCityEntry;
	bool m_bBarbCoExist;
	bool m_bBlendIntoCity;
	bool m_bUpgradeAnywhere;
	bool m_bAssassin;
	bool m_bStealthDefense;
	bool m_bNoInvisibility;
	bool m_bTriggerBeforeAttack;
	bool m_bCanAnimalIgnoresBorders;
	bool m_bCanAnimalIgnoresImprovements;
	bool m_bCanAnimalIgnoresCities;
	bool m_bNoNonTypeProdMods;
	bool m_bGatherHerd;
	bool m_bCanMergeSplit;

	std::vector<UnitCombatTypes> m_aiSubCombatTypes;
	std::vector<int> m_aiCureAfflictionTypes;
	std::vector<int> m_aiHealAsTypes;
	std::vector<TerrainTypes> m_vTerrainImpassableTypes;
	std::vector<FeatureTypes> m_vFeatureImpassableTypes;
	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
	std::vector<int> m_aiTrapSetWithPromotionTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	std::vector<int> m_aiCategories;

	std::vector<HealUnitCombat> m_aHealUnitCombatTypes;
	std::vector<GroupSpawnUnitCombat> m_aGroupSpawnUnitCombatTypes;
	std::vector<InvisibleTerrainChanges> m_aInvisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aInvisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aInvisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainRangeChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureRangeChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementRangeChanges;
	std::vector<EnabledCivilizations> m_aEnabledCivilizationTypes;
	UnitCombatModifierArray m_aFlankingStrengthbyUnitCombatType;
	TerrainModifierArray m_aWithdrawOnTerrainTypes;
	FeatureModifierArray m_aWithdrawOnFeatureTypes;
	UnitCombatModifierArray m_aWithdrawVSUnitCombatTypes;
	UnitCombatModifierArray m_aPursuitVSUnitCombatTypes;
	UnitCombatModifierArray m_aRepelVSUnitCombatTypes;
	UnitCombatModifierArray m_aKnockbackVSUnitCombatTypes;
	UnitCombatModifierArray m_aCriticalVSUnitCombatTypes;
	UnitCombatModifierArray m_aTrapDisableUnitCombatTypes;
	UnitCombatModifierArray m_aTrapAvoidanceUnitCombatTypes;
	UnitCombatModifierArray m_aTrapTriggerUnitCombatTypes;
	InvisibilityArray m_aVisibilityIntensityTypes;
	InvisibilityArray m_aInvisibilityIntensityTypes;
	InvisibilityArray m_aVisibilityIntensityRangeTypes;
	TerrainModifierArray m_aTerrainWorkRateModifierTypes;
	FeatureModifierArray m_aFeatureWorkRateModifierTypes;
	BuildModifierArray m_aBuildWorkRateModifierTypes;

	//Pediahelp
	std::vector<int> m_aiQualifiedPromotionTypes;
};

#endif // CV_UNIT_INFO_H
