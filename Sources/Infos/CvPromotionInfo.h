#pragma once

#ifndef CV_PROMOTION_INFO_H
#define CV_PROMOTION_INFO_H

#include "CvInfoBase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionInfo
	: public CvHotkeyInfo
	, private bst::noncopyable
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvPromotionInfo();
	virtual ~CvPromotionInfo();

	int getLayerAnimationPath() const;
	PromotionTypes getPrereqPromotion() const { return m_iPrereqPromotion; }
	PromotionTypes getPrereqOrPromotion1() const { return m_iPrereqOrPromotion1; }
	PromotionTypes getPrereqOrPromotion2() const { return m_iPrereqOrPromotion2; }

	TechTypes getTechPrereq() const;
	int getMinEraType() const;
	int getMaxEraType() const;
	int getStateReligionPrereq() const;
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
	//ls612: Work rate modifiers
	int getWorkRatePercent() const;
	int getCommandType() const;
	void setCommandType(int iNewType);

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
	bool isDefensiveVictoryMove() const;
	bool isFreeDrop() const;
	bool isOffensiveVictoryMove() const;
	bool isOneUp() const;

	bool isPillageEspionage() const;
	bool isPillageMarauder() const;
	bool isPillageOnMove() const;
	bool isPillageOnVictory() const;
	bool isPillageResearch() const;

	bool isLeader() const;
	bool isBlitz() const;
	bool isAmphib() const;
	bool isRiver() const;
	bool isEnemyRoute() const;
	bool isAlwaysHeal() const;
	bool isHillsDoubleMove() const;
	bool isImmuneToFirstStrikes() const;

	const char* getSound() const;

	bool changesMoveThroughPlots() const;
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	// Arrays

	int getTerrainAttackPercent(int i) const;
	bool isAnyTerrainAttackPercent() const;
	int getTerrainDefensePercent(int i) const;
	bool isAnyTerrainDefensePercent() const;
	int getFeatureAttackPercent(int i) const;
	bool isAnyFeatureAttackPercent() const;
	int getFeatureDefensePercent(int i) const;
	bool isAnyFeatureDefensePercent() const;
	int getUnitCombatModifierPercent(int i) const;
	bool isAnyUnitCombatModifierPercent() const;
	int getDomainModifierPercent(int i) const;
	bool isAnyDomainModifierPercent() const;
	//ls612: Terrain Work Modifiers
	int getTerrainWorkPercent(int i) const;
	int getFeatureWorkPercent(int i) const;

	bool getTerrainDoubleMove(int i) const;
	bool getFeatureDoubleMove(int i) const;
	bool getUnitCombat(int i) const;
	bool isCanMovePeaks() const;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool isCanLeadThroughPeaks() const;
	//const std::vector<PromotionTypes>& getPromotionOverwrites() const { return m_vPromotionOverwrites; }
	TechTypes getObsoleteTech() const;
	int getControlPoints() const;
	int getCommandRange() const;
	bool isZoneOfControl() const;
	int getIgnoreTerrainDamage() const;

	void getCheckSum(uint32_t& iSum) const;

	const CvPropertyManipulators* getPropertyManipulators() const { return &m_PropertyManipulators; }

	//TB Combat Mods Begin  TB SubCombat Mod begin
	//Text Strings
	const wchar_t* getRenamesUnitTo() const;
	// Textual References
	PromotionLineTypes getPromotionLine() const;
	UnitCombatTypes getReplacesUnitCombat() const;
	DomainTypes getDomainCargoChange() const;
	SpecialUnitTypes getSpecialCargoChange() const;
	SpecialUnitTypes getSpecialCargoPrereq() const;
	SpecialUnitTypes getSMNotSpecialCargoChange() const;
	SpecialUnitTypes getSMNotSpecialCargoPrereq() const;
	SpecialUnitTypes setSpecialUnit() const;
	// integers
	int getAttackCombatModifierChange() const;
	int getDefenseCombatModifierChange() const;
	// Fight or Flight
	int getPursuitChange() const;
	int getEarlyWithdrawChange() const;
	int getTauntChange() const;
	//
	int getVSBarbsChange() const;
	// S&D Extended
	int getUnnerveChange() const;
	int getEncloseChange() const;
	int getLungeChange() const;
	int getDynamicDefenseChange() const;
	//
	int getStrengthChange() const;
	int getLinePriority() const;
	int getDamageperTurn() const;
	int getStrAdjperTurn() const;
	int getWeakenperTurn() const;
	int getEnduranceChange() const;
	int getPoisonProbabilityModifierChange() const;

	int getCaptureProbabilityModifierChange() const;
	int getCaptureResistanceModifierChange() const;

	//WorkRateMod
	int getPeaksWorkPercent() const;
	//
	int getBreakdownChanceChange() const;
	int getBreakdownDamageChange() const;
	// Size Matters
	int getMaxHPChange() const;
	int getStrengthModifier() const;
	//
	int getQualityChange() const;
	int getGroupChange() const;
	int getLevelPrereq() const;
	int getDamageModifierChange() const;
	int getUpkeepModifier() const;
	int getExtraUpkeep100() const;
	int getRBombardDamageChange() const;
	int getRBombardDamageLimitChange() const;
	int getRBombardDamageMaxUnitsChange() const;
	int getDCMBombRangeChange() const;
	int getDCMBombAccuracyChange() const;
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
	int getUpgradeAnywhereChange() const;
	int getInsidiousnessChange() const;
	int getInvestigationChange() const;
	int getAssassinChange() const;
	int getStealthStrikesChange() const;
	int getStealthCombatModifierChange() const;
	int getStealthDefenseChange() const;
	int getDefenseOnlyChange() const;
	int getNoInvisibilityChange() const;
	int getTrapDamageMax() const;
	int getTrapDamageMin() const;
	int getTrapComplexity() const;
	int getNumTriggers() const;
	int getTriggerBeforeAttackChange() const;
	int getHiddenNationalityChange() const;
	int getAnimalIgnoresBordersChange() const;
	int getNoDefensiveBonusChange() const;
	int getGatherHerdChange() const;
	int getReligiousCombatModifierChange() const;
	//booleans
	bool isStampedeChange() const;
	bool isRemoveStampede() const;
	bool isOnslaughtChange() const;
	bool isParalyze() const;
	bool isAttackOnlyCitiesAdd() const;
	bool isAttackOnlyCitiesSubtract() const;
	bool isIgnoreNoEntryLevelAdd() const;
	bool isIgnoreNoEntryLevelSubtract() const;
	bool isIgnoreZoneofControlAdd() const;
	bool isIgnoreZoneofControlSubtract() const;
	bool isFliesToMoveAdd() const;
	bool isFliesToMoveSubtract() const;
	bool isZeroesXP() const;
	bool isForOffset() const;
	bool isCargoPrereq() const;
	bool isRBombardPrereq() const;
	bool isNoSelfHeal() const;
	bool isSetOnHNCapture() const;
	bool isSetOnInvestigated() const;
	bool isStatus() const;
	bool isPrereqNormInvisible() const;
	bool isPlotPrereqsKeepAfter() const;
	bool isRemoveAfterSet() const;
	bool isQuick() const;
	//Arrays
	//int getAIWeightbyUnitCombatType(int i) const;
	//bool isAnyAIWeightbyUnitCombatType() const;

	// bool vector without delayed resolution
	int getSubCombatChangeType(int i) const;
	int getNumSubCombatChangeTypes() const;
	bool isSubCombatChangeType(int i) const;

	int getRemovesUnitCombatType(int i) const;
	int getNumRemovesUnitCombatTypes() const;
	bool isRemovesUnitCombatType(int i) const;

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i) const;

	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i) const;

	int getFreetoUnitCombat(int i) const;
	int getNumFreetoUnitCombats() const;
	bool isFreetoUnitCombat(int i) const;

	int getNotOnUnitCombatType(int i) const;
	int getNumNotOnUnitCombatTypes() const;
	bool isNotOnUnitCombatType(int i) const;

	int getNotOnDomainType(int i) const;
	int getNumNotOnDomainTypes() const;
	bool isNotOnDomainType(int i) const;

	int getCategory(int i) const;
	int getNumCategories() const;
	bool isCategory(int i) const;

	const std::vector<MapCategoryTypes>& getMapCategories() const { return m_aeMapCategoryTypes; }

	const std::vector<BonusTypes>& getPrereqBonuses() const { return m_aiPrereqBonusTypes; }

	int getAddsBuildType(int i) const;
	int getNumAddsBuildTypes() const;
	bool isAddsBuildType(int i) const;

	int getNegatesInvisibilityType(int i) const;
	int getNumNegatesInvisibilityTypes() const;
	bool isNegatesInvisibilityType(int i) const;

	int getPrereqTerrainType(int i) const;
	int getNumPrereqTerrainTypes() const;
	bool isPrereqTerrainType(int i) const;

	int getPrereqFeatureType(int i) const;
	int getNumPrereqFeatureTypes() const;
	bool isPrereqFeatureType(int i) const;

	int getPrereqImprovementType(int i) const;
	int getNumPrereqImprovementTypes() const;
	bool isPrereqImprovementType(int i) const;

	int getPrereqPlotBonusType(int i) const;
	int getNumPrereqPlotBonusTypes() const;
	bool isPrereqPlotBonusType(int i) const;

	int getPrereqLocalBuildingType(int i) const;
	int getNumPrereqLocalBuildingTypes() const;
	bool isPrereqLocalBuildingType(int i) const;

	int getTrapSetWithPromotionType(int i) const;
	int getNumTrapSetWithPromotionTypes() const;
	bool isTrapSetWithPromotionType(int i) const;

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i) const;

	int getTargetUnitCombatType(int i) const;
	int getNumTargetUnitCombatTypes() const;
	bool isTargetUnitCombatType(int i) const;

	// int vector utilizing pairing without delayed resolution

	int getNumFlankingStrikesbyUnitCombatTypesChange() const;
	int getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const;
	bool isFlankingStrikebyUnitCombatTypeChange(int iUnitCombat) const;

	int getNumWithdrawOnTerrainTypeChanges() const;
	int getWithdrawOnTerrainTypeChange(int iTerrain) const;

	int getNumWithdrawOnFeatureTypeChanges() const;
	int getWithdrawOnFeatureTypeChange(int iFeature) const;

	int getNumWithdrawVSUnitCombatChangeTypes() const;
	int getWithdrawVSUnitCombatChangeType(int iUnitCombat) const;
	bool isWithdrawVSUnitCombatChangeType(int iUnitCombat) const;

	int getNumPursuitVSUnitCombatChangeTypes() const;
	int getPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;
	bool isPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;

	int getNumTrapDisableUnitCombatTypes() const;
	int getTrapDisableUnitCombatType(int iUnitCombat) const;
	bool isTrapDisableUnitCombatType(int iUnitCombat) const;

	int getNumTrapAvoidanceUnitCombatTypes() const;
	int getTrapAvoidanceUnitCombatType(int iUnitCombat) const;
	bool isTrapAvoidanceUnitCombatType(int iUnitCombat) const;

	int getNumTrapTriggerUnitCombatTypes() const;
	int getTrapTriggerUnitCombatType(int iUnitCombat) const;
	bool isTrapTriggerUnitCombatType(int iUnitCombat) const;

	int getNumBuildWorkRateModifierChangeTypes() const;
	int getBuildWorkRateModifierChangeType(int iBuild) const;
	bool isBuildWorkRateModifierChangeType(int iBuild) const;

	int getNumVisibilityIntensityChangeTypes() const;
	int getVisibilityIntensityChangeType(int iInvisibility) const;
	bool isVisibilityIntensityChangeType(int iInvisibility) const;

	int getNumInvisibilityIntensityChangeTypes() const;
	int getInvisibilityIntensityChangeType(int iInvisibility) const;
	bool isInvisibilityIntensityChangeType(int iInvisibility) const;

	int getNumVisibilityIntensityRangeChangeTypes() const;
	int getVisibilityIntensityRangeChangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeChangeType(int iInvisibility) const;

	int getNumAIWeightbyUnitCombatTypes() const;
	const UnitCombatModifier& getAIWeightbyUnitCombatType(int iUnitCombat) const;

	int getNumHealUnitCombatChangeTypes() const;
	const HealUnitCombat& getHealUnitCombatChangeType(int iUnitCombat) const;

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

	// TB Combat Mods End  TB SubCombat Mod end

	//Pediahelp
	int getQualifiedUnitCombatType(int i) const;
	int getNumQualifiedUnitCombatTypes() const;
	bool isQualifiedUnitCombatType(int i) const;
	void setQualifiedUnitCombatTypes();

	int getDisqualifiedUnitCombatType(int i) const;
	int getNumDisqualifiedUnitCombatTypes() const;
	void setDisqualifiedUnitCombatTypes();

	bool hasNegativeEffects() const;
	inline bool isStarsign() const { return m_bStarsign; }

protected:
	bool m_bCanMovePeaks;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool m_bCanLeadThroughPeaks;
	int m_iNumPromotionOverwrites;
	//std::vector<PromotionTypes> m_vPromotionOverwrites;
	TechTypes m_iObsoleteTech;
	int m_iControlPoints;
	int m_iCommandRange;
	bool m_bZoneOfControl;
	int m_iIgnoreTerrainDamage;
	int m_zobristValue;
public:
	void getDataMembers(CvInfoUtil& util);
	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(const CvPromotionInfo* pClassInfo);

private:
	bool m_bStarsign;
	CvPropertyManipulators m_PropertyManipulators;

//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	int m_iLayerAnimationPath;
	PromotionTypes m_iPrereqPromotion;
	PromotionTypes m_iPrereqOrPromotion1;
	PromotionTypes m_iPrereqOrPromotion2;

	TechTypes m_iTechPrereq;
	int m_iStateReligionPrereq;
	int m_iMinEraType;
	int m_iMaxEraType;
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
	int m_iCommandType;
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
	bool m_bDefensiveVictoryMove;
	bool m_bFreeDrop;
	bool m_bOffensiveVictoryMove;
	bool m_bOneUp;

	bool m_bPillageEspionage;
	bool m_bPillageMarauder;
	bool m_bPillageOnMove;
	bool m_bPillageOnVictory;
	bool m_bPillageResearch;

	bool m_bLeader;
	bool m_bBlitz;
	bool m_bAmphib;
	bool m_bRiver;
	bool m_bEnemyRoute;
	bool m_bAlwaysHeal;
	bool m_bHillsDoubleMove;
	bool m_bImmuneToFirstStrikes;

	CvString m_szSound;

	// Arrays
	int* m_piTerrainAttackPercent;
	int* m_piTerrainDefensePercent;
	int* m_piFeatureAttackPercent;
	int* m_piFeatureDefensePercent;
	int* m_piUnitCombatModifierPercent;
	int* m_piDomainModifierPercent;
	//ls612: Terrain Work Modifiers
	int* m_piTerrainWorkPercent;
	int* m_piFeatureWorkPercent;

	bool* m_pbTerrainDoubleMove;
	bool* m_pbFeatureDoubleMove;
	bool* m_pbUnitCombat;

	//TB Combat Mods Begin TB SubCombat Mod begin
	//Text Strings
	CvWString m_szRenamesUnitTo;
	//Textual References
	PromotionLineTypes m_ePromotionLine;
	UnitCombatTypes m_eReplacesUnitCombat;
	DomainTypes m_eDomainCargoChange;
	SpecialUnitTypes m_eSpecialCargoChange;
	SpecialUnitTypes m_eSpecialCargoPrereq;
	SpecialUnitTypes m_eSMNotSpecialCargoChange;
	SpecialUnitTypes m_eSMNotSpecialCargoPrereq;
	SpecialUnitTypes m_eSetSpecialUnit;
	//integers
	int m_iAttackCombatModifierChange;
	int m_iDefenseCombatModifierChange;
	int m_iPursuitChange;
	int m_iEarlyWithdrawChange;
	int m_iVSBarbsChange;
	int m_iUnnerveChange;
	int m_iEncloseChange;
	int m_iLungeChange;
	int m_iDynamicDefenseChange;
	int m_iStrengthChange;
	int m_iLinePriority;
	int m_iDamageperTurn;
	int m_iStrAdjperTurn;
	int m_iWeakenperTurn;
	int m_iEnduranceChange;
	int m_iPoisonProbabilityModifierChange;

	int m_iCaptureProbabilityModifierChange;
	int m_iCaptureResistanceModifierChange;

	//WorkRateMod
	int m_iPeaksWorkPercent;

	int m_iBreakdownChanceChange;
	int m_iBreakdownDamageChange;
	int m_iTauntChange;
	int m_iMaxHPChange;
	int m_iStrengthModifier;
	int m_iQualityChange;
	int m_iGroupChange;
	int m_iLevelPrereq;
	int m_iDamageModifierChange;

	int m_iUpkeepModifier;
	int m_iExtraUpkeep100;

	int m_iRBombardDamageChange;
	int m_iRBombardDamageLimitChange;
	int m_iRBombardDamageMaxUnitsChange;
	int m_iDCMBombRangeChange;
	int m_iDCMBombAccuracyChange;
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
	int m_iUpgradeAnywhereChange;
	int m_iInsidiousnessChange;
	int m_iInvestigationChange;
	int m_iAssassinChange;
	int m_iStealthStrikesChange;
	int m_iStealthCombatModifierChange;
	int m_iStealthDefenseChange;
	int m_iDefenseOnlyChange;
	int m_iNoInvisibilityChange;
	int m_iTrapDamageMax;
	int m_iTrapDamageMin;
	int m_iTrapComplexity;
	int m_iNumTriggers;
	int m_iTriggerBeforeAttackChange;
	int m_iHiddenNationalityChange;
	int m_iAnimalIgnoresBordersChange;
	int m_iNoDefensiveBonusChange;
	int m_iGatherHerdChange;
	int m_iReligiousCombatModifierChange;
	//booleans
	bool m_bStampedeChange;
	bool m_bRemoveStampede;
	bool m_bOnslaughtChange;
	bool m_bParalyze;
	bool m_bAttackOnlyCitiesAdd;
	bool m_bAttackOnlyCitiesSubtract;
	bool m_bIgnoreNoEntryLevelAdd;
	bool m_bIgnoreNoEntryLevelSubtract;
	bool m_bIgnoreZoneofControlAdd;
	bool m_bIgnoreZoneofControlSubtract;
	bool m_bFliesToMoveAdd;
	bool m_bFliesToMoveSubtract;
	bool m_bZeroesXP;
	bool m_bForOffset;
	bool m_bCargoPrereq;
	bool m_bRBombardPrereq;
	bool m_bNoSelfHeal;
	bool m_bSetOnHNCapture;
	bool m_bSetOnInvestigated;
	bool m_bStatus;
	bool m_bPrereqNormInvisible;
	bool m_bPlotPrereqsKeepAfter;
	bool m_bRemoveAfterSet;
	bool m_bQuick;
	//bool m_bAnyAIWeightbyUnitCombat;
	//Arrays
	//int* m_piAIWeightbyUnitCombatTypes;
	// bool vectors without delayed resolution
	std::vector<int> m_aiSubCombatChangeTypes;
	std::vector<int> m_aiRemovesUnitCombatTypes;
	std::vector<int> m_aiOnGameOptions;
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiFreetoUnitCombats;
	std::vector<int> m_aiNotOnUnitCombatTypes;
	std::vector<int> m_aiNotOnDomainTypes;
	std::vector<int> m_aiCategories;
	std::vector<MapCategoryTypes> m_aeMapCategoryTypes;
	std::vector<BonusTypes> m_aiPrereqBonusTypes;
	std::vector<int> m_aiAddsBuildTypes;
	std::vector<int> m_aiNegatesInvisibilityTypes;
	std::vector<int> m_aiPrereqTerrainTypes;
	std::vector<int> m_aiPrereqFeatureTypes;
	std::vector<int> m_aiPrereqImprovementTypes;
	std::vector<int> m_aiPrereqPlotBonusTypes;
	std::vector<int> m_aiPrereqLocalBuildingTypes;
	std::vector<int> m_aiTrapSetWithPromotionTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	std::vector<int> m_aiTargetUnitCombatTypes;
	// int vectors utilizing pairing without delayed resolution
	UnitCombatModifierArray m_aFlankingStrengthbyUnitCombatTypeChange;
	TerrainModifierArray m_aWithdrawOnTerrainTypesChange;
	FeatureModifierArray m_aWithdrawOnFeatureTypesChange;
	UnitCombatModifierArray m_aWithdrawVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aPursuitVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aTrapDisableUnitCombatTypes;
	UnitCombatModifierArray m_aTrapAvoidanceUnitCombatTypes;
	UnitCombatModifierArray m_aTrapTriggerUnitCombatTypes;

	BuildModifierArray m_aBuildWorkRateModifierChangeTypes;
	InvisibilityArray m_aVisibilityIntensityChangeTypes;
	InvisibilityArray m_aInvisibilityIntensityChangeTypes;
	InvisibilityArray m_aVisibilityIntensityRangeChangeTypes;
	// int vector utilizing struct with delayed resolution
	std::vector<UnitCombatModifier> m_aAIWeightbyUnitCombatTypes;
	std::vector<HealUnitCombat> m_aHealUnitCombatChangeTypes;
	std::vector<InvisibleTerrainChanges> m_aInvisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aInvisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aInvisibleImprovementChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureRangeChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementRangeChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainRangeChanges;
	//TB Combat Mods End  TB SubCombat Mod end

	//Pediahelp
	std::vector<int> m_aiQualifiedUnitCombatTypes;
	std::vector<int> m_disqualifiedUnitCombatTypes;

};

#endif // CV_PROMOTION_INFO_H
