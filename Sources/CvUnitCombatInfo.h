#pragma once

#ifndef Cv_UnitCombat_Info
#define Cv_UnitCombat_Info

#include "CvInfos.h"


// AIAndy: A proper info class for unit combat types
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitCombatInfo
//
//  DESC:   Contains unit combat types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitCombatInfo
	: public CvInfoBase
	, private bst::noncopyable
{
public:

	CvUnitCombatInfo();
	virtual ~CvUnitCombatInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUnitCombatInfo* pClassInfo);
	void getCheckSum(uint32_t& iSum) const;

	const CvOutcomeList* getKillOutcomeList() const;
	int getNumActionOutcomes() const;
	const CvOutcomeList* getActionOutcomeList(int index) const;
	MissionTypes getActionOutcomeMission(int index) const;
	const CvOutcomeList* getActionOutcomeListByMission(MissionTypes eMission) const;
	const CvOutcomeMission* getOutcomeMission(int index) const;
	const CvOutcomeMission* getOutcomeMissionByMission(MissionTypes eMission) const;

protected:
	CvOutcomeList m_KillOutcomeList;
	std::vector<const CvOutcomeMission*> m_aOutcomeMissions;

public:
	// Textual References
	ReligionTypes getReligion() const;
	BonusTypes getCulture() const;
	EraTypes getEra() const;
	// integers
	int getVisibilityChange() const;
	int getMovesChange() const;
	int getMoveDiscountChange() const;
	int getAirRangeChange() const;
	int getInterceptChange() const;
	int getEvasionChange() const;
	int getWithdrawalChange() const;
	int getCargoChange() const;
	int getSMCargoChange() const;
	int getSMCargoVolumeChange() const;
	int getSMCargoVolumeModifierChange() const;
	int getCollateralDamageChange() const;
	int getBombardRateChange() const;
	int getFirstStrikesChange() const;
	int getChanceFirstStrikesChange() const;
	int getEnemyHealChange() const;
	int getNeutralHealChange() const;
	int getFriendlyHealChange() const;
	int getSameTileHealChange() const;
	int getAdjacentTileHealChange() const;
	int getCombatPercent() const;
	int getCityAttackPercent() const;
	int getCityDefensePercent() const;
	int getHillsAttackPercent() const;
	int getHillsDefensePercent() const;
	int getHillsWorkPercent() const;
	int getWorkRatePercent() const;
	int getRevoltProtection() const;
	int getCollateralDamageProtection() const;
	int getPillageChange() const;
	int getUpgradeDiscount() const;
	int getExperiencePercent() const;
	int getKamikazePercent() const;
	int getAirCombatLimitChange() const;
	int getCelebrityHappy() const;
	int getCollateralDamageLimitChange() const;
	int getCollateralDamageMaxUnitsChange() const;
	int getCombatLimitChange() const;
	int getExtraDropRange() const;
	int getSurvivorChance() const;
	int getVictoryAdjacentHeal() const;
	int getVictoryHeal() const;
	int getVictoryStackHeal() const;
	int getAttackCombatModifierChange() const;
	int getDefenseCombatModifierChange() const;
	int getPursuitChange() const;
	int getEarlyWithdrawChange() const;
	int getVSBarbsChange() const;
	int getArmorChange() const;
	int getPunctureChange() const;
	int getOverrunChange() const;
	int getRepelChange() const;
	int getFortRepelChange() const;
	int getRepelRetriesChange() const;
	int getUnyieldingChange() const;
	int getKnockbackChange() const;
	int getKnockbackRetriesChange() const;
#ifdef BATTLEWORN
	int getStrAdjperAttChange() const;
	int getStrAdjperDefChange() const;
	int getWithdrawAdjperAttChange() const;
#endif // BATTLEWORN
	int getUnnerveChange() const;
	int getEncloseChange() const;
	int getLungeChange() const;
	int getDynamicDefenseChange() const;
	int getStrengthChange() const;
	int getFortitudeChange() const;
#ifdef STRENGTH_IN_NUMBERS
	int getFrontSupportPercentChange() const;
	int getShortRangeSupportPercentChange() const;
	int getMediumRangeSupportPercentChange() const;
	int getLongRangeSupportPercentChange() const;
	int getFlankSupportPercentChange() const;
#endif // STRENGTH_IN_NUMBERS
	int getDodgeModifierChange() const;
	int getPrecisionModifierChange() const;
	int getPowerShotsChange() const;
	int getPowerShotCombatModifierChange() const;
	int getPowerShotPunctureModifierChange() const;
	int getPowerShotPrecisionModifierChange() const;
	int getPowerShotCriticalModifierChange() const;
	int getCriticalModifierChange() const;
	int getEnduranceChange() const;
	int getRoundStunProbChange() const;
	int getPoisonProbabilityModifierChange() const;
	int getCaptureProbabilityModifierChange() const;
	int getCaptureResistanceModifierChange() const;
	int getPeaksWorkPercent() const;
	int getBreakdownChanceChange() const;
	int getBreakdownDamageChange() const;
	int getTauntChange() const;
	int getMaxHPChange() const;
	int getStrengthModifier() const;
	int getQualityBase() const;
	int getGroupBase() const;
	int getSizeBase() const;
	int getDamageModifierChange() const;
	int getUpkeepModifier() const;
	int getExtraUpkeep100() const;
	int getRBombardDamageBase() const;
	int getRBombardDamageLimitBase() const;
	int getRBombardDamageMaxUnitsBase() const;
	int getDCMBombRangeBase() const;
	int getDCMBombAccuracyBase() const;
	int getCombatModifierPerSizeMoreChange() const;
	int getCombatModifierPerSizeLessChange() const;
	int getCombatModifierPerVolumeMoreChange() const;
	int getCombatModifierPerVolumeLessChange() const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getExcileChange() const;
	int getPassageChange() const;
	int getNoNonOwnedCityEntryChange() const;
	int getBarbCoExistChange() const;
	int getBlendIntoCityChange() const;
	int getInsidiousnessChange() const;
	int getInvestigationChange() const;
	int getStealthStrikesChange() const;
	int getStealthCombatModifierChange() const;
	int getStealthDefenseChange() const;
	int getDefenseOnlyChange() const;
	int getNoInvisibilityChange() const;
	int getNoCaptureChange() const;
	int getAnimalIgnoresBordersChange() const;
	int getNoDefensiveBonusChange() const;
	int getGatherHerdChange() const;
	int getReligiousCombatModifierChange() const;

	//booleans
	bool isDefensiveVictoryMove() const;
	bool isFreeDrop() const;
	bool isOffensiveVictoryMove() const;
	bool isOneUp() const;
	bool isPillageEspionage() const;
	bool isPillageMarauder() const;
	bool isPillageOnMove() const;
	bool isPillageOnVictory() const;
	bool isPillageResearch() const;
	bool isBlitz() const;
	bool isAmphib() const;
	bool isRiver() const;
	bool isEnemyRoute() const;
	bool isAlwaysHeal() const;
	bool isHillsDoubleMove() const;
	bool isImmuneToFirstStrikes() const;
	bool isStampedeChange() const;
	bool isRemoveStampede() const;
	bool isOnslaughtChange() const;
	bool isMakesDamageCold() const;
	bool isMakesDamageNotCold() const;
	bool isAddsColdImmunity() const;
	bool isRemovesColdImmunity() const;
	bool isAttackOnlyCitiesAdd() const;
	bool isAttackOnlyCitiesSubtract() const;
	bool isIgnoreNoEntryLevelAdd() const;
	bool isIgnoreNoEntryLevelSubtract() const;
	bool isIgnoreZoneofControlAdd() const;
	bool isIgnoreZoneofControlSubtract() const;
	bool changesMoveThroughPlots() const;
	bool isCanMovePeaks() const;
	bool isCanLeadThroughPeaks() const;
	bool isZoneOfControl() const;
	bool isSpy() const;
	bool isFliesToMoveAdd() const;
	bool isFliesToMoveSubtract() const;
	bool isCannotMergeSplit() const;
	bool isRBombardDirect() const;
	bool isRBombardForceAbility() const;
	bool isAlwaysInvisible() const;
	bool isForMilitary() const;
	bool isForNavalMilitary() const;
	bool isHealsAs() const;
	bool isNoSelfHeal() const;
	// Inline Int
	inline int getZobristValue() const { return m_zobristValue; }

	// Arrays
	int getDomainModifierPercent(int i) const;
	bool isAnyDomainModifierPercent() const;

	int getTerrainIgnoreDamageChangeType(int i) const;
	int getNumTerrainIgnoreDamageChangeTypes() const;
	bool isTerrainIgnoreDamageChangeType(int i) const;

	int getTerrainDoubleMoveChangeType(int i) const;
	int getNumTerrainDoubleMoveChangeTypes() const;
	bool isTerrainDoubleMoveChangeType(int i) const;

	int getFeatureDoubleMoveChangeType(int i) const;
	int getNumFeatureDoubleMoveChangeTypes() const;
	bool isFeatureDoubleMoveChangeType(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getGGptsforUnitType(int i) const;
	int getNumGGptsforUnitTypes() const;
	bool isGGptsforUnitType(int i) const;

	int getDefaultStatusType(int i) const;
	int getNumDefaultStatusTypes() const;
	bool isDefaultStatusType(int i) const;

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	// int vector utilizing pairing without delayed resolution
	int getNumWithdrawOnTerrainTypeChanges() const;
	int getWithdrawOnTerrainTypeChange(int iTerrain) const;

	int getNumWithdrawOnFeatureTypeChanges() const;
	int getWithdrawOnFeatureTypeChange(int iFeature) const;

	int getNumVisibilityIntensityChangeTypes() const;
	int getVisibilityIntensityChangeType(int iInvisibility) const;
	bool isVisibilityIntensityChangeType(int iInvisibility) const;

	int getNumInvisibilityIntensityChangeTypes() const;
	int getInvisibilityIntensityChangeType(int iInvisibility) const;
	bool isInvisibilityIntensityChangeType(int iInvisibility) const;

	int getNumVisibilityIntensityRangeChangeTypes() const;
	int getVisibilityIntensityRangeChangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeChangeType(int iInvisibility) const;

	int getNumVisibilityIntensitySameTileChangeTypes() const;
	int getVisibilityIntensitySameTileChangeType(int iInvisibility) const;
	bool isVisibilityIntensitySameTileChangeType(int iInvisibility) const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty) const;

	int getCureAfflictionChangeType(int i) const;
	int getNumCureAfflictionChangeTypes() const;
	bool isCureAfflictionChangeType(int i) const;

	int getNumAfflictionFortitudeChangeModifiers() const;
	const PromotionLineModifier& getAfflictionFortitudeChangeModifier(int iAfflictionLine) const;

	int getNumAfflictOnAttackChangeTypes() const;
	const AfflictOnAttackChange& getAfflictOnAttackChangeType(int iAfflictionLine) const;
#endif // OUTBREAKS_AND_AFFLICTIONS

	int getNumTerrainAttackChangeModifiers() const;
	const TerrainModifier& getTerrainAttackChangeModifier(int iTerrain) const;

	int getNumTerrainDefenseChangeModifiers() const;
	const TerrainModifier& getTerrainDefenseChangeModifier(int iTerrain) const;

	int getNumTerrainWorkChangeModifiers() const;
	const TerrainModifier& getTerrainWorkChangeModifier(int iTerrain) const;

	int getNumBuildWorkChangeModifiers() const;
	const BuildModifier& getBuildWorkChangeModifier(int iBuild) const;

	int getNumFeatureAttackChangeModifiers() const;
	const FeatureModifier& getFeatureAttackChangeModifier(int iFeature) const;

	int getNumFeatureDefenseChangeModifiers() const;
	const FeatureModifier& getFeatureDefenseChangeModifier(int iFeature) const;

	int getNumFeatureWorkChangeModifiers() const;
	const FeatureModifier& getFeatureWorkChangeModifier(int iFeature) const;

	int getNumUnitCombatChangeModifiers() const;
	const UnitCombatModifier& getUnitCombatChangeModifier(int iUnitCombat) const;

	int getNumFlankingStrengthbyUnitCombatTypesChange() const;
	const UnitCombatModifier& getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const;

	int getNumWithdrawVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getWithdrawVSUnitCombatTypeChange(int iUnitCombat) const;

	//Fight or Flight only
	int getNumPursuitVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getPursuitVSUnitCombatTypeChange(int iUnitCombat) const;

	//Heart of War only
	int getNumRepelVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getRepelVSUnitCombatTypeChange(int iUnitCombat) const;

	//Heart of War only
	int getNumKnockbackVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getKnockbackVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumPunctureVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getPunctureVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumArmorVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getArmorVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumDodgeVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getDodgeVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumPrecisionVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getPrecisionVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumCriticalVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getCriticalVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumRoundStunVSUnitCombatTypesChange() const;
	const UnitCombatModifier& getRoundStunVSUnitCombatTypeChange(int iUnitCombat) const;

	int getNumTrapAvoidanceUnitCombatTypes() const;
	const UnitCombatModifier& getTrapAvoidanceUnitCombatType(int iIndex) const;

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

	int getNumVisibleImprovementRangeChanges() const;
	const InvisibleImprovementChanges& getVisibleImprovementRangeChange(int iIndex) const;

	int getNumDistanceAttackCommunicabilityTypeChanges() const;
	const AfflictionLineChanges& getDistanceAttackCommunicabilityTypeChange(int iIndex) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

private:
	CvPropertyManipulators m_PropertyManipulators;

protected:
	// Textual References
	ReligionTypes m_eReligion;
	BonusTypes m_eCulture;
	EraTypes m_eEra;
	//Integers
	int m_iIgnoreTerrainDamage;
	int m_zobristValue;
	int m_iVisibilityChange;
	int m_iMovesChange;
	int m_iMoveDiscountChange;
	int m_iAirRangeChange;
	int m_iInterceptChange;
	int m_iEvasionChange;
	int m_iWithdrawalChange;
	int m_iCargoChange;
	int m_iSMCargoChange;
	int m_iSMCargoVolumeChange;
	int m_iSMCargoVolumeModifierChange;
	int m_iCollateralDamageChange;
	int m_iBombardRateChange;
	int m_iFirstStrikesChange;
	int m_iChanceFirstStrikesChange;
	int m_iEnemyHealChange;
	int m_iNeutralHealChange;
	int m_iFriendlyHealChange;
	int m_iSameTileHealChange;
	int m_iAdjacentTileHealChange;
	int m_iCombatPercent;
	int m_iCityAttackPercent;
	int m_iCityDefensePercent;
	int m_iHillsAttackPercent;
	int m_iHillsDefensePercent;
	int m_iHillsWorkPercent;
	int m_iWorkRatePercent;
	int m_iRevoltProtection;
	int m_iCollateralDamageProtection;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iKamikazePercent;
	int m_iAirCombatLimitChange;
	int m_iCelebrityHappy;
	int m_iCollateralDamageLimitChange;
	int m_iCollateralDamageMaxUnitsChange;
	int m_iCombatLimitChange;
	int m_iExtraDropRange;
	int m_iSurvivorChance;
	int m_iVictoryAdjacentHeal;
	int m_iVictoryHeal;
	int m_iVictoryStackHeal;
	int m_iAttackCombatModifierChange;
	int m_iDefenseCombatModifierChange;
	int m_iPursuitChange;
	int m_iEarlyWithdrawChange;
	int m_iVSBarbsChange;
	int m_iArmorChange;
	int m_iPunctureChange;
	int m_iOverrunChange;
	int m_iRepelChange;
	int m_iFortRepelChange;
	int m_iRepelRetriesChange;
	int m_iUnyieldingChange;
	int m_iKnockbackChange;
	int m_iKnockbackRetriesChange;
#ifdef BATTLEWORN
	int m_iStrAdjperAttChange;
	int m_iStrAdjperDefChange;
	int m_iWithdrawAdjperAttChange;
#endif // BATTLEWORN
	int m_iUnnerveChange;
	int m_iEncloseChange;
	int m_iLungeChange;
	int m_iDynamicDefenseChange;
	int m_iStrengthChange;
	int m_iFortitudeChange;
#ifdef STRENGTH_IN_NUMBERS
	int m_iFrontSupportPercentChange;
	int m_iShortRangeSupportPercentChange;
	int m_iMediumRangeSupportPercentChange;
	int m_iLongRangeSupportPercentChange;
	int m_iFlankSupportPercentChange;
#endif // STRENGTH_IN_NUMBERS
	int m_iDodgeModifierChange;
	int m_iPrecisionModifierChange;
	int m_iPowerShotsChange;
	int m_iPowerShotCombatModifierChange;
	int m_iPowerShotPunctureModifierChange;
	int m_iPowerShotPrecisionModifierChange;
	int m_iPowerShotCriticalModifierChange;
	int m_iCriticalModifierChange;
	int m_iEnduranceChange;
	int m_iRoundStunProbChange;
	int m_iPoisonProbabilityModifierChange;
	int m_iCaptureProbabilityModifierChange;
	int m_iCaptureResistanceModifierChange;
	int m_iPeaksWorkPercent;
	int m_iBreakdownChanceChange;
	int m_iBreakdownDamageChange;
	int m_iTauntChange;
	int m_iMaxHPChange;
	int m_iStrengthModifier;
	int m_iQualityBase;
	int m_iGroupBase;
	int m_iSizeBase;
	int m_iDamageModifierChange;

	int m_iUpkeepModifier;
	int m_iExtraUpkeep100;

	int m_iRBombardDamageBase;
	int m_iRBombardDamageLimitBase;
	int m_iRBombardDamageMaxUnitsBase;
	int m_iDCMBombRangeBase;
	int m_iDCMBombAccuracyBase;
	int m_iCombatModifierPerSizeMoreChange;
	int m_iCombatModifierPerSizeLessChange;
	int m_iCombatModifierPerVolumeMoreChange;
	int m_iCombatModifierPerVolumeLessChange;
	int m_iSelfHealModifier;
	int m_iNumHealSupport;
	int m_iExcileChange;
	int m_iPassageChange;
	int m_iNoNonOwnedCityEntryChange;
	int m_iBarbCoExistChange;
	int m_iBlendIntoCityChange;
	int m_iInsidiousnessChange;
	int m_iInvestigationChange;
	int m_iStealthStrikesChange;
	int m_iStealthCombatModifierChange;
	int m_iStealthDefenseChange;
	int m_iDefenseOnlyChange;
	int m_iNoInvisibilityChange;
	int m_iNoCaptureChange;
	int m_iAnimalIgnoresBordersChange;
	int m_iNoDefensiveBonusChange;
	int m_iGatherHerdChange;
	int m_iReligiousCombatModifierChange;

	//Booleans
	bool m_bCanMovePeaks;
	bool m_bCanLeadThroughPeaks;
	bool m_bZoneOfControl;
	bool m_bDefensiveVictoryMove;
	bool m_bFreeDrop;
	bool m_bOffensiveVictoryMove;
	bool m_bOneUp;
	bool m_bPillageEspionage;
	bool m_bPillageMarauder;
	bool m_bPillageOnMove;
	bool m_bPillageOnVictory;
	bool m_bPillageResearch;
	bool m_bBlitz;
	bool m_bAmphib;
	bool m_bRiver;
	bool m_bEnemyRoute;
	bool m_bAlwaysHeal;
	bool m_bHillsDoubleMove;
	bool m_bImmuneToFirstStrikes;
	bool m_bStampedeChange;
	bool m_bRemoveStampede;
	bool m_bOnslaughtChange;
	bool m_bMakesDamageCold;
	bool m_bMakesDamageNotCold;
	bool m_bAddsColdImmunity;
	bool m_bRemovesColdImmunity;
	bool m_bAttackOnlyCitiesAdd;
	bool m_bAttackOnlyCitiesSubtract;
	bool m_bIgnoreNoEntryLevelAdd;
	bool m_bIgnoreNoEntryLevelSubtract;
	bool m_bIgnoreZoneofControlAdd;
	bool m_bIgnoreZoneofControlSubtract;
	bool m_bSpy;
	bool m_bFliesToMoveAdd;
	bool m_bFliesToMoveSubtract;
	bool m_bCannotMergeSplit;
	bool m_bRBombardDirect;
	bool m_bRBombardForceAbility;
	bool m_bInvisible;
	bool m_bForMilitary;
	bool m_bForNavalMilitary;
	bool m_bHealsAs;
	bool m_bNoSelfHeal;

	//Arrays
	int* m_piDomainModifierPercent;

	// bool vectors without delayed resolution
	// bool vector with delayed resolution
	std::vector<int> m_aiTerrainIgnoreDamageChangeTypes;
	std::vector<int> m_aiTerrainDoubleMoveChangeTypes;
	std::vector<int> m_aiFeatureDoubleMoveChangeTypes;
	std::vector<int> m_aiOnGameOptions;
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiGGptsforUnitTypes;
	std::vector<int> m_aiDefaultStatusTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	std::vector<int> m_aiCategories;
	// int vectors utilizing pairing without delayed resolution
	TerrainModifierArray m_aWithdrawOnTerrainTypesChange;
	FeatureModifierArray m_aWithdrawOnFeatureTypesChange;
	InvisibilityArray m_aVisibilityIntensityChangeTypes;
	InvisibilityArray m_aInvisibilityIntensityChangeTypes;
	InvisibilityArray m_aVisibilityIntensityRangeChangeTypes;
	InvisibilityArray m_aVisibilityIntensitySameTileChangeTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	AidArray m_aAidChanges;
#endif
	// int vector utilizing struct with delayed resolution
	std::vector<TerrainModifier> m_aTerrainAttackChangeModifiers;
	std::vector<TerrainModifier> m_aTerrainDefenseChangeModifiers;
	std::vector<TerrainModifier> m_aTerrainWorkChangeModifiers;
	std::vector<BuildModifier> m_aBuildWorkChangeModifiers;
	std::vector<FeatureModifier> m_aFeatureAttackChangeModifiers;
	std::vector<FeatureModifier> m_aFeatureDefenseChangeModifiers;
	std::vector<FeatureModifier> m_aFeatureWorkChangeModifiers;
	std::vector<UnitCombatModifier> m_aUnitCombatChangeModifiers;
	std::vector<UnitCombatModifier> m_aFlankingStrengthbyUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aWithdrawVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aPursuitVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aRepelVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aKnockbackVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aPunctureVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aArmorVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aDodgeVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aPrecisionVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aCriticalVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aRoundStunVSUnitCombatTypeChange;
	std::vector<UnitCombatModifier> m_aTrapAvoidanceUnitCombatTypes;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	std::vector<int> m_aiCureAfflictionChangeTypes;
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeChangeModifiers;
	std::vector<AfflictOnAttackChange> m_aAfflictOnAttackChangeTypes;
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
#endif // OUTBREAKS_AND_AFFLICTIONS
	std::vector<InvisibleTerrainChanges> m_aInvisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aInvisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aInvisibleImprovementChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureRangeChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementRangeChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainRangeChanges;
};


#endif;