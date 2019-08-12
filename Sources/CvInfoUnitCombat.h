#ifndef CV_INFO_UNITCOMBAT_H
#define CV_INFO_UNITCOMBAT_H

#include "CvOutcomeList.h"
#include "CvOutcomeMission.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionInfo :	public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvPromotionInfo();
	virtual ~CvPromotionInfo();

	int getLayerAnimationPath() const;
	int getPrereqPromotion() const;				// Exposed to Python
	void setPrereqPromotion(int i);				// Exposed to Python
	int getPrereqOrPromotion1() const;			// Exposed to Python
	void setPrereqOrPromotion1(int i);			// Exposed to Python
	int getPrereqOrPromotion2() const;			// Exposed to Python
	void setPrereqOrPromotion2(int i);			// Exposed to Python

	int getTechPrereq() const;					// Exposed to Python
	int getMinEraType() const;
	int getMaxEraType() const;
	int getStateReligionPrereq() const;			// Exposed to Python
	int getVisibilityChange() const;			// Exposed to Python
	int getMovesChange() const;					// Exposed to Python
	int getMoveDiscountChange() const;			// Exposed to Python
	int getAirRangeChange() const;				// Exposed to Python
	int getInterceptChange() const;				// Exposed to Python
	int getEvasionChange() const;				// Exposed to Python
	int getWithdrawalChange() const;			// Exposed to Python
	int getCargoChange() const;					// Exposed to Python
	int getSMCargoChange() const;
	int getSMCargoVolumeChange() const;
	int getSMCargoVolumeModifierChange() const;
	int getCollateralDamageChange() const;		// Exposed to Python
	int getBombardRateChange() const;			// Exposed to Python
	int getFirstStrikesChange() const;			// Exposed to Python
	int getChanceFirstStrikesChange() const;	// Exposed to Python
	int getEnemyHealChange() const;				// Exposed to Python
	int getNeutralHealChange() const;			// Exposed to Python
	int getFriendlyHealChange() const;			// Exposed to Python
	int getSameTileHealChange() const;			// Exposed to Python
	int getAdjacentTileHealChange() const;		// Exposed to Python
	int getCombatPercent() const;				// Exposed to Python
	int getCityAttackPercent() const;			// Exposed to Python
	int getCityDefensePercent() const;			// Exposed to Python
	int getHillsAttackPercent() const;			// Exposed to Python
	int getHillsDefensePercent() const;			// Exposed to Python
	int getHillsWorkPercent() const;
	//ls612: Work rate modifiers
	int getWorkRatePercent() const;
	int getCommandType() const;					// Exposed to Python
	void setCommandType(int iNewType);

	int getRevoltProtection() const;			// Exposed to Python
	int getCollateralDamageProtection() const;	// Exposed to Python
	int getPillageChange() const;				// Exposed to Python
	int getUpgradeDiscount() const;				// Exposed to Python
	int getExperiencePercent() const;			// Exposed to Python
	int getKamikazePercent() const;				// Exposed to Python
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	int getAirCombatLimitChange() const;		// Exposed to Python
	int getCelebrityHappy() const;				// Exposed to Python
	int getCollateralDamageLimitChange() const;		// Exposed to Python
	int getCollateralDamageMaxUnitsChange() const;	// Exposed to Python
	int getCombatLimitChange() const;			// Exposed to Python
	int getExtraDropRange() const;				// Exposed to Python
	//Team Project (2)
	int getSurvivorChance() const;
	int getVictoryAdjacentHeal() const;
	//
	int getVictoryHeal() const;					// Exposed to Python
	//Team Project (2)
	int getVictoryStackHeal() const;
	//
	bool isDefensiveVictoryMove() const;		// Exposed to Python
	bool isFreeDrop() const;					// Exposed to Python
	bool isOffensiveVictoryMove() const;		// Exposed to Python
	//Team Project (2)
	bool isOneUp() const;
	//
	bool isPillageCulture() const;				// Exposed to Python
	bool isPillageEspionage() const;			// Exposed to Python
	bool isPillageMarauder() const;				// Exposed to Python
	bool isPillageOnMove() const;				// Exposed to Python
	bool isPillageOnVictory() const;			// Exposed to Python
	bool isPillageResearch() const;				// Exposed to Python
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/

	bool isLeader() const;						// Exposed to Python
	bool isBlitz() const;						// Exposed to Python
	bool isAmphib() const;						// Exposed to Python
	bool isRiver() const;						// Exposed to Python
	bool isEnemyRoute() const;					// Exposed to Python
	bool isAlwaysHeal() const;					// Exposed to Python
	bool isHillsDoubleMove() const;				// Exposed to Python
	bool isImmuneToFirstStrikes() const;		// Exposed to Python

	const TCHAR* getSound() const;				// Exposed to Python
	void setSound(const TCHAR* szVal);

	bool changesMoveThroughPlots() const;
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	// Arrays

	int getTerrainAttackPercent(int i) const;	// Exposed to Python
	bool isAnyTerrainAttackPercent() const;
	int getTerrainDefensePercent(int i) const;	// Exposed to Python
	bool isAnyTerrainDefensePercent() const;
	int getFeatureAttackPercent(int i) const;	// Exposed to Python
	bool isAnyFeatureAttackPercent() const;
	int getFeatureDefensePercent(int i) const;	// Exposed to Python
	bool isAnyFeatureDefensePercent() const;
	int getUnitCombatModifierPercent(int i) const;	// Exposed to Python
	bool isAnyUnitCombatModifierPercent() const;
	int getDomainModifierPercent(int i) const;		// Exposed to Python
	bool isAnyDomainModifierPercent() const;
	//ls612: Terrain Work Modifiers
	int getTerrainWorkPercent(int i) const;
	int getFeatureWorkPercent(int i) const;

	bool getTerrainDoubleMove(int i) const;		// Exposed to Python
	bool getFeatureDoubleMove(int i) const;		// Exposed to Python
	bool getUnitCombat(int i) const;			// Exposed to Python


	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	bool isCanMovePeaks() const;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool isCanLeadThroughPeaks() const;
	int getNumPromotionOverwrites() const;
	PromotionTypes getPromotionOverwrites(int iI) const;
	int getObsoleteTech() const;
	int getControlPoints() const;
	int getCommandRange() const;
	bool isZoneOfControl() const;
	int getAssetMultiplier() const;
	int getPowerMultiplier() const;
	int getIgnoreTerrainDamage() const;

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	//TB Combat Mods Begin  TB SubCombat Mod begin
	//Text Strings
	const wchar* getRenamesUnitTo() const;
	// Textual References
	PromotionLineTypes getPromotionLine() const;
	UnitCombatTypes getReplacesUnitCombat() const;
	DomainTypes getDomainCargoChange() const;
	SpecialUnitTypes getSpecialCargoChange() const;
	SpecialUnitTypes getSpecialCargoPrereq() const;
	SpecialUnitTypes getSMSpecialCargoChange() const;
	SpecialUnitTypes getSMSpecialCargoPrereq() const;
	SpecialUnitTypes getSMNotSpecialCargoChange() const;
	SpecialUnitTypes getSMNotSpecialCargoPrereq() const;
	SpecialUnitTypes setSpecialUnit() const;
	// integers
	int getAttackCombatModifierChange() const;
	int getDefenseCombatModifierChange() const;
	// Fight or Flight
	int getPursuitChange(bool bForLoad = false) const;
	int getEarlyWithdrawChange(bool bForLoad = false) const;
	int getTauntChange() const;
	//
	int getVSBarbsChange() const;
	int getArmorChange() const;
	int getPunctureChange() const;
	// Heart of War
	int getOverrunChange(bool bForLoad = false) const;
	int getRepelChange(bool bForLoad = false) const;
	int getFortRepelChange(bool bForLoad = false) const;
	int getRepelRetriesChange(bool bForLoad = false) const;
	int getUnyieldingChange(bool bForLoad = false) const;
	int getKnockbackChange(bool bForLoad = false) const;
	int getKnockbackRetriesChange(bool bForLoad = false) const;
	//
	// Battleworn
	int getStrAdjperRndChange(bool bForLoad = false) const;
	int getStrAdjperAttChange(bool bForLoad = false) const;
	int getStrAdjperDefChange(bool bForLoad = false) const;
	int getWithdrawAdjperAttChange(bool bForLoad = false) const;
	//
	// S&D Extended
	int getUnnerveChange(bool bForLoad = false) const;
	int getEncloseChange(bool bForLoad = false) const;
	int getLungeChange(bool bForLoad = false) const;
	int getDynamicDefenseChange(bool bForLoad = false) const;
	//
	int getStrengthChange() const;
	int getLinePriority() const;
	int getFortitudeChange() const;
	int getDamageperTurn() const;
	int getStrAdjperTurn() const;
	int getWeakenperTurn() const;
	// Strength in Numbers
	int getFrontSupportPercentChange(bool bForLoad = false) const;
	int getShortRangeSupportPercentChange(bool bForLoad = false) const;
	int getMediumRangeSupportPercentChange(bool bForLoad = false) const;
	int getLongRangeSupportPercentChange(bool bForLoad = false) const;
	int getFlankSupportPercentChange(bool bForLoad = false) const;
	//
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
	//Team Project (3)
	int getCaptureProbabilityModifierChange() const;
	int getCaptureResistanceModifierChange() const;
	//Team Project (4)
	//WorkRateMod
	int getHillsWorkModifierChange() const;
	int getPeaksWorkModifierChange() const;
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
	int getCostModifierChange() const;
	int getRBombardDamageChange() const;
	int getRBombardDamageLimitChange() const;
	int getRBombardDamageMaxUnitsChange() const;
	int getDCMBombRangeChange() const;
	int getDCMBombAccuracyChange() const;
	int getCombatModifierPerSizeMoreChange(bool bForLoad = false) const;
	int getCombatModifierPerSizeLessChange(bool bForLoad = false) const;
	int getCombatModifierPerVolumeMoreChange(bool bForLoad = false) const;
	int getCombatModifierPerVolumeLessChange(bool bForLoad = false) const;
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
	int getStealthStrikesChange(bool bForLoad = false) const;
	int getStealthCombatModifierChange(bool bForLoad = false) const;
	int getStealthDefenseChange(bool bForLoad = false) const;
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
	bool isEquipment() const;
	bool isAffliction() const;
	bool isParalyze() const;
	bool isMakesDamageCold() const;
	bool isMakesDamageNotCold() const;
	bool isAddsColdImmunity() const;
	bool isRemovesColdImmunity() const;
	bool isCritical() const;
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
	bool isSubCombatChangeType(int i);

	int getRemovesUnitCombatType(int i) const;
	int getNumRemovesUnitCombatTypes() const;
	bool isRemovesUnitCombatType(int i);

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i);

	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i);

	int getFreetoUnitCombat(int i) const;
	int getNumFreetoUnitCombats() const;
	bool isFreetoUnitCombat(int i);

	int getNotOnUnitCombatType(int i) const;
	int getNumNotOnUnitCombatTypes() const;
	bool isNotOnUnitCombatType(int i);

	int getNotOnDomainType(int i) const;
	int getNumNotOnDomainTypes() const;
	bool isNotOnDomainType(int i);

	int getNoAutoEquiptoCombatClassType(int i) const;
	int getNumNoAutoEquiptoCombatClassTypes() const;
	bool isNoAutoEquiptoCombatClassType(int i);

	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	// bool vector with delayed resolution
	int getCureAfflictionChangeType(int i) const;
	int getNumCureAfflictionChangeTypes() const;
	bool isCureAfflictionChangeType(int i);

	int getPrereqBonusType(int i) const;
	int getNumPrereqBonusTypes() const;
	bool isPrereqBonusType(int i);

	int getAddsBuildType(int i) const;
	int getNumAddsBuildTypes() const;
	bool isAddsBuildType(int i);

	int getNegatesInvisibilityType(int i) const;
	int getNumNegatesInvisibilityTypes() const;
	bool isNegatesInvisibilityType(int i);

	int getPrereqTerrainType(int i) const;
	int getNumPrereqTerrainTypes() const;
	bool isPrereqTerrainType(int i);

	int getPrereqFeatureType(int i) const;
	int getNumPrereqFeatureTypes() const;
	bool isPrereqFeatureType(int i);

	int getPrereqImprovementType(int i) const;
	int getNumPrereqImprovementTypes() const;
	bool isPrereqImprovementType(int i);

	int getPrereqPlotBonusType(int i) const;
	int getNumPrereqPlotBonusTypes() const;
	bool isPrereqPlotBonusType(int i);

	int getPrereqLocalBuildingClassType(int i) const;
	int getNumPrereqLocalBuildingClassTypes() const;
	bool isPrereqLocalBuildingClassType(int i);

	int getTrapSetWithPromotionType(int i) const;
	int getNumTrapSetWithPromotionTypes() const;
	bool isTrapSetWithPromotionType(int i);

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i);

	int getTargetUnitCombatType(int i) const;
	int getNumTargetUnitCombatTypes() const;
	bool isTargetUnitCombatType(int i);

	// int vector utilizing pairing without delayed resolution

	int getNumFlankingStrikesbyUnitCombatTypesChange() const;
	int getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat) const;
	bool isFlankingStrikebyUnitCombatTypeChange(int iUnitCombat);

	int getNumWithdrawOnTerrainTypeChanges() const;
	int getWithdrawOnTerrainTypeChange(int iTerrain) const;

	int getNumWithdrawOnFeatureTypeChanges() const;
	int getWithdrawOnFeatureTypeChange(int iFeature) const;

	int getNumWithdrawVSUnitCombatChangeTypes() const;
	int getWithdrawVSUnitCombatChangeType(int iUnitCombat) const;
	bool isWithdrawVSUnitCombatChangeType(int iUnitCombat);

	int getNumPursuitVSUnitCombatChangeTypes() const;
	int getPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;
	bool isPursuitVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false);

	int getNumRepelVSUnitCombatChangeTypes() const;
	int getRepelVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;
	bool isRepelVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false);

	int getNumKnockbackVSUnitCombatChangeTypes() const;
	int getKnockbackVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false) const;
	bool isKnockbackVSUnitCombatChangeType(int iUnitCombat, bool bForLoad = false);

	int getNumPunctureVSUnitCombatChangeTypes() const;
	int getPunctureVSUnitCombatChangeType(int iUnitCombat) const;
	bool isPunctureVSUnitCombatChangeType(int iUnitCombat);

	int getNumArmorVSUnitCombatChangeTypes() const;
	int getArmorVSUnitCombatChangeType(int iUnitCombat) const;
	bool isArmorVSUnitCombatChangeType(int iUnitCombat);

	int getNumDodgeVSUnitCombatChangeTypes() const;
	int getDodgeVSUnitCombatChangeType(int iUnitCombat) const;
	bool isDodgeVSUnitCombatChangeType(int iUnitCombat);

	int getNumPrecisionVSUnitCombatChangeTypes() const;
	int getPrecisionVSUnitCombatChangeType(int iUnitCombat) const;
	bool isPrecisionVSUnitCombatChangeType(int iUnitCombat);

	int getNumCriticalVSUnitCombatChangeTypes() const;
	int getCriticalVSUnitCombatChangeType(int iUnitCombat) const;
	bool isCriticalVSUnitCombatChangeType(int iUnitCombat);

	int getNumRoundStunVSUnitCombatChangeTypes() const;
	int getRoundStunVSUnitCombatChangeType(int iUnitCombat) const;
	bool isRoundStunVSUnitCombatChangeType(int iUnitCombat);

	int getNumTrapDisableUnitCombatTypes() const;
	int getTrapDisableUnitCombatType(int iUnitCombat) const;
	bool isTrapDisableUnitCombatType(int iUnitCombat);

	int getNumTrapAvoidanceUnitCombatTypes() const;
	int getTrapAvoidanceUnitCombatType(int iUnitCombat) const;
	bool isTrapAvoidanceUnitCombatType(int iUnitCombat);

	int getNumTrapTriggerUnitCombatTypes() const;
	int getTrapTriggerUnitCombatType(int iUnitCombat) const;
	bool isTrapTriggerUnitCombatType(int iUnitCombat);

	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty);
	//Team Project (4)
	//WorkRateMod
	int getNumTerrainWorkRateModifierChangeTypes() const;
	int getTerrainWorkRateModifierChangeType(int iTerrain) const;
	bool isTerrainWorkRateModifierChangeType(int iTerrain);

	int getNumFeatureWorkRateModifierChangeTypes() const;
	int getFeatureWorkRateModifierChangeType(int iFeature) const;
	bool isFeatureWorkRateModifierChangeType(int iFeature);

	int getNumBuildWorkRateModifierChangeTypes() const;
	int getBuildWorkRateModifierChangeType(int iBuild) const;
	bool isBuildWorkRateModifierChangeType(int iBuild);

	int getNumVisibilityIntensityChangeTypes() const;
	int getVisibilityIntensityChangeType(int iInvisibility) const;
	bool isVisibilityIntensityChangeType(int iInvisibility);

	int getNumInvisibilityIntensityChangeTypes() const;
	int getInvisibilityIntensityChangeType(int iInvisibility) const;
	bool isInvisibilityIntensityChangeType(int iInvisibility);

	int getNumVisibilityIntensityRangeChangeTypes() const;
	int getVisibilityIntensityRangeChangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeChangeType(int iInvisibility);

	// int vector utilizing struct with delayed resolution
	int getNumAIWeightbyUnitCombatTypes() const;
	UnitCombatModifier& getAIWeightbyUnitCombatType(int iUnitCombat);

	int getNumAfflictionFortitudeChangeModifiers() const;
	PromotionLineModifier& getAfflictionFortitudeChangeModifier(int iAfflictionLine);

	int getNumAfflictOnAttackChangeTypes() const;
	AfflictOnAttackChange& getAfflictOnAttackChangeType(int iAfflictionLine);

	int getNumHealUnitCombatChangeTypes() const;
	HealUnitCombat& getHealUnitCombatChangeType(int iUnitCombat);

	int getNumInvisibleTerrainChanges() const;
	InvisibleTerrainChanges& getInvisibleTerrainChange(int iIndex);

	int getNumInvisibleFeatureChanges() const;
	InvisibleFeatureChanges& getInvisibleFeatureChange(int iIndex);

	int getNumInvisibleImprovementChanges() const;
	InvisibleImprovementChanges& getInvisibleImprovementChange(int iIndex);

	int getNumVisibleTerrainChanges() const;
	InvisibleTerrainChanges& getVisibleTerrainChange(int iIndex);

	int getNumVisibleFeatureChanges() const;
	InvisibleFeatureChanges& getVisibleFeatureChange(int iIndex);

	int getNumVisibleImprovementChanges() const;
	InvisibleImprovementChanges& getVisibleImprovementChange(int iIndex);

	int getNumVisibleTerrainRangeChanges() const;
	InvisibleTerrainChanges& getVisibleTerrainRangeChange(int iIndex);

	int getNumVisibleFeatureRangeChanges() const;
	InvisibleFeatureChanges& getVisibleFeatureRangeChange(int iIndex);

	int getNumVisibleImprovementRangeChanges() const;
	InvisibleImprovementChanges& getVisibleImprovementRangeChange(int iIndex);

	int getNumDistanceAttackCommunicabilityTypeChanges() const;
	AfflictionLineChanges& getDistanceAttackCommunicabilityTypeChange(int iIndex);

	// TB Combat Mods End  TB SubCombat Mod end

	//Pediahelp
	int getQualifiedUnitCombatType(int i) const;
	int getNumQualifiedUnitCombatTypes() const;
	bool isQualifiedUnitCombatType(int i);
	void setQualifiedUnitCombatTypes();

protected:
	bool m_bCanMovePeaks;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool m_bCanLeadThroughPeaks;
	int m_iNumPromotionOverwrites;
	int* m_piPromotionOverwrites;
	int m_iObsoleteTech;
	int m_iControlPoints;
	int m_iCommandRange;
	bool m_bZoneOfControl;
	int m_iAssetMultiplier;
	int m_iPowerMultiplier;
	int m_iIgnoreTerrainDamage;
	int m_zobristValue;
public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvPromotionInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	void copyNonDefaultsReadPass2(CvPromotionInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL, bool bOver = false);

//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iLayerAnimationPath;
	int m_iPrereqPromotion;
	int m_iPrereqOrPromotion1;
	int m_iPrereqOrPromotion2;

	int m_iTechPrereq;
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
	//ls612: Work rate modifiers
	int m_iWorkRatePercent;
	int m_iCommandType;
	int m_iRevoltProtection;
	int m_iCollateralDamageProtection;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iKamikazePercent;
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 16.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	int m_iAirCombatLimitChange;
	int m_iCelebrityHappy;
	int m_iCollateralDamageLimitChange;
	int m_iCollateralDamageMaxUnitsChange;
	int m_iCombatLimitChange;
	int m_iExtraDropRange;
	//Team Project (2)
	int m_iSurvivorChance;
	int m_iVictoryAdjacentHeal;
	int m_iVictoryHeal;
	//Team Project (2)
	int m_iVictoryStackHeal;
	bool m_bDefensiveVictoryMove;
	bool m_bFreeDrop;
	bool m_bOffensiveVictoryMove;
	//Team Project (2)
	bool m_bOneUp;
	bool m_bPillageCulture;
	bool m_bPillageEspionage;
	bool m_bPillageMarauder;
	bool m_bPillageOnMove;
	bool m_bPillageOnVictory;
	bool m_bPillageResearch;
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags															 **/
/*****************************************************************************************************/

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
	bool m_bAnyTerrainAttackPercent;
	bool m_bAnyTerrainDefensePercent;
	bool m_bAnyFeatureAttackPercent;
	bool m_bAnyFeatureDefensePercent;
	bool m_bAnyUnitCombatModifierPercent;
	bool m_bAnyDomainModifierPercent;
	//bool m_bAnyAIWeightbyUnitCombatType;
	//ls612: Terrain Work Modifiers
	int* m_piTerrainWorkPercent;
	int* m_piFeatureWorkPercent;

	bool* m_pbTerrainDoubleMove;
	bool* m_pbFeatureDoubleMove;
	bool* m_pbUnitCombat;

	CvPropertyManipulators m_PropertyManipulators;

	//TB Combat Mods Begin TB SubCombat Mod begin
	//Text Strings
	CvWString m_szRenamesUnitTo;
	//Textual References
	PromotionLineTypes m_ePromotionLine;
	UnitCombatTypes m_eReplacesUnitCombat;
	DomainTypes m_eDomainCargoChange;
	SpecialUnitTypes m_eSpecialCargoChange;
	SpecialUnitTypes m_eSpecialCargoPrereq;
	SpecialUnitTypes m_eSMSpecialCargoChange;
	SpecialUnitTypes m_eSMSpecialCargoPrereq;
	SpecialUnitTypes m_eSMNotSpecialCargoChange;
	SpecialUnitTypes m_eSMNotSpecialCargoPrereq;
	SpecialUnitTypes m_eSetSpecialUnit;
	//integers
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
	int m_iStrAdjperRndChange;
	int m_iStrAdjperAttChange;
	int m_iStrAdjperDefChange;
	int m_iWithdrawAdjperAttChange;
	int m_iUnnerveChange;
	int m_iEncloseChange;
	int m_iLungeChange;
	int m_iDynamicDefenseChange;
	int m_iStrengthChange;
	int m_iLinePriority;
	int m_iFortitudeChange;
	int m_iDamageperTurn;
	int m_iStrAdjperTurn;
	int m_iWeakenperTurn;
	int m_iFrontSupportPercentChange;
	int m_iShortRangeSupportPercentChange;
	int m_iMediumRangeSupportPercentChange;
	int m_iLongRangeSupportPercentChange;
	int m_iFlankSupportPercentChange;
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
	//Team Project (3)
	int m_iCaptureProbabilityModifierChange;
	int m_iCaptureResistanceModifierChange;
	//Team Project (4)
	//WorkRateMod
	int m_iHillsWorkModifierChange;
	int m_iPeaksWorkModifierChange;

	int m_iBreakdownChanceChange;
	int m_iBreakdownDamageChange;
	int m_iTauntChange;
	int m_iMaxHPChange;
	int m_iStrengthModifier;
	int m_iQualityChange;
	int m_iGroupChange;
	int m_iLevelPrereq;
	int m_iDamageModifierChange;
	int m_iCostModifierChange;
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
	bool m_bEquipment;
	bool m_bAffliction;
	bool m_bParalyze;
	bool m_bMakesDamageCold;
	bool m_bMakesDamageNotCold;
	bool m_bAddsColdImmunity;
	bool m_bRemovesColdImmunity;
	bool m_bCritical;
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
	std::vector<int> m_aiNoAutoEquiptoCombatClassTypes;
	std::vector<int> m_aiMapCategoryTypes;
	// bool vector with delayed resolution
	std::vector<int> m_aiCureAfflictionChangeTypes;
	std::vector<int> m_aiPrereqBonusTypes;
	std::vector<int> m_aiAddsBuildTypes;
	std::vector<int> m_aiNegatesInvisibilityTypes;
	std::vector<int> m_aiPrereqTerrainTypes;
	std::vector<int> m_aiPrereqFeatureTypes;
	std::vector<int> m_aiPrereqImprovementTypes;
	std::vector<int> m_aiPrereqPlotBonusTypes;
	std::vector<int> m_aiPrereqLocalBuildingClassTypes;
	std::vector<int> m_aiTrapSetWithPromotionTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	std::vector<int> m_aiTargetUnitCombatTypes;
	// int vectors utilizing pairing without delayed resolution
	UnitCombatModifierArray m_aFlankingStrengthbyUnitCombatTypeChange;
	TerrainModifierArray m_aWithdrawOnTerrainTypesChange;
	FeatureModifierArray m_aWithdrawOnFeatureTypesChange;
	UnitCombatModifierArray m_aWithdrawVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aPursuitVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aRepelVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aKnockbackVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aPunctureVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aArmorVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aDodgeVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aPrecisionVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aCriticalVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aRoundStunVSUnitCombatChangeTypes;
	UnitCombatModifierArray m_aTrapDisableUnitCombatTypes;
	UnitCombatModifierArray m_aTrapAvoidanceUnitCombatTypes;
	UnitCombatModifierArray m_aTrapTriggerUnitCombatTypes;
	AidArray m_aAidChanges;
	//Team Project (4)
		//WorkRateMod
	TerrainModifierArray m_aTerrainWorkRateModifierChangeTypes;
	FeatureModifierArray m_aFeatureWorkRateModifierChangeTypes;
	BuildModifierArray m_aBuildWorkRateModifierChangeTypes;
	InvisibilityArray m_aVisibilityIntensityChangeTypes;
	InvisibilityArray m_aInvisibilityIntensityChangeTypes;
	InvisibilityArray m_aVisibilityIntensityRangeChangeTypes;
	// int vector utilizing struct with delayed resolution
	std::vector<UnitCombatModifier> m_aAIWeightbyUnitCombatTypes;
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeChangeModifiers;
	std::vector<AfflictOnAttackChange> m_aAfflictOnAttackChangeTypes;
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
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
	//TB Combat Mods End  TB SubCombat Mod end

	//Pediahelp
	std::vector<int> m_aiQualifiedUnitCombatTypes;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvMissionInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvMissionInfo : public CvHotkeyInfo
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvMissionInfo();
	CvMissionInfo(const char* szType);
	virtual ~CvMissionInfo();

	DllExport int getTime() const;			// Exposed to Python

	bool isSound() const;					// Exposed to Python
	DllExport bool isTarget() const;		// Exposed to Python
	bool isBuild() const;					// Exposed to Python
	bool getVisible() const;				// Exposed to Python
	DllExport EntityEventTypes getEntityEvent() const;

	const TCHAR* getWaypoint() const;		// effect type, Exposed to Python

	bool read(CvXMLLoadUtility* pXML);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	void copyNonDefaults(CvMissionInfo* pClassInfo, CvXMLLoadUtility* pXML);

	// Should not be needed to checksum this as this will likely not get changed without DLL changes
	//void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iTime;

	bool m_bSound;
	bool m_bTarget;
	bool m_bBuild;
	bool m_bVisible;
	EntityEventTypes m_eEntityEvent;

	CvString m_szWaypoint;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvCommandInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvCommandInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvCommandInfo();
	virtual ~CvCommandInfo();

	int getAutomate() const;
	void setAutomate(int i);

	bool getConfirmCommand() const;
	bool getVisible() const;
	bool getAll() const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvCommandInfo* pClassInfo, CvXMLLoadUtility* pXML);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	int m_iAutomate;

	bool m_bConfirmCommand;
	bool m_bVisible;
	bool m_bAll;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvAutomateInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvAutomateInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvAutomateInfo();
	virtual ~CvAutomateInfo();

	int getCommand() const;
	void setCommand(int i);
	int getAutomate() const;
	void setAutomate(int i);

	bool getConfirmCommand() const;
	void setConfirmCommand(bool bVal);
	bool getVisible() const;
	void setVisible(bool bVal);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvAutomateInfo* pClassInfo, CvXMLLoadUtility* pXML);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:
	int m_iCommand;
	int m_iAutomate;

	bool m_bConfirmCommand;
	bool m_bVisible;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// class	: CvSpawnInfo
//
// \brief	: Holds information relating to auto unit spawns
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvSpawnInfo : public CvInfoBase
{
public:
	CvSpawnInfo();
	virtual ~CvSpawnInfo();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	virtual bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvSpawnInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);

	int	getNumBonuses();
	int	getNumTerrains();
	int	getNumFeatures();
	int	getNumFeatureTerrains();
	int	getNumSpawnGroup();
	PlayerTypes		getPlayer();
	BonusTypes		getBonus(int index);
	TerrainTypes	getTerrain(int index);
	FeatureTypes	getFeature(int index);
	TerrainTypes	getFeatureTerrain(int index);
	UnitTypes		getSpawnGroup(int index);
	int getTurnRate();
	int getGlobalTurnRate();
	int getMaxLocalDensity();
	int getMaxAreaTotalDensity();
	int getMaxAreaUnitDensity();
	int getStartDate();
	int	getEndDate();
	int	getMinLatitude();
	int	getMaxLatitude();
	bool getLatitudeAbs();
	int	getMinLongitude();
	int	getMaxLongitude();
	bool getTreatAsBarbarian();
	bool getNeutralOnly();
	bool getNoSpeedNormalization();
	bool getNotInView();
	bool getHills();
	bool getFlatlands();
	bool getPeaks();
	bool getFreshWaterOnly();
	int getRateOverride();
	UnitTypes	getUnitType();
	TechTypes	getPrereqTechType();
	TechTypes	getObsoleteTechType();

	BoolExpr* getSpawnCondition();

	void getCheckSum(unsigned int& iSum);

private:
	UnitTypes				m_eUnitType;
	TechTypes				m_ePrereqTechType;
	TechTypes				m_eObsoleteTechType;
	int						m_iPlayerType;
	int						m_iTurns;
	int						m_iGlobalTurns;
	int						m_iMaxLocalDensity;
	int						m_iMaxAreaTotalDensity;
	int						m_iMaxAreaUnitDensity;
	int						m_iStartDate;
	int						m_iEndDate;
	bool					m_bTreatAsBarbarian;
	bool					m_bNeutralOnly;
	bool					m_bNotInView;
	bool					m_bNoSpeedNormalization;
	bool					m_bHills;
	bool					m_bFlatlands;
	bool					m_bPeaks;
	bool					m_bFreshWaterOnly;

	bool					m_bLatitudeAbs;
	int						m_iMinLatitude;
	int						m_iMaxLatitude;
	int						m_iMinLongitude;
	int						m_iMaxLongitude;

	int						m_iRateOverride;
	std::vector<BonusTypes>	m_bonusTypes;
	std::vector<TerrainTypes>	m_terrainTypes;
	std::vector<FeatureTypes>	m_featureTypes;
	std::vector<TerrainTypes>	m_featureTerrainTypes;
	std::vector<UnitTypes>		m_spawnGroup;

	BoolExpr*				m_pExprSpawnCondition;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// class	: CvUnitFormationInfo
//
// \brief	: Holds information relating to the formation of sub-units within a unit
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CvUnitEntry
{
public:
	CvUnitEntry()
	{
	}

	CvUnitEntry(const NiPoint2 &position, float radius, float facingDirection, float facingVariance) :
	m_position(position),
	m_fRadius(radius),
	m_fFacingDirection(facingDirection),
	m_fFacingVariance(facingVariance)
	{
	}

	NiPoint2 m_position;
	float m_fRadius;
	float m_fFacingDirection;
	float m_fFacingVariance;
};

class CvUnitFormationInfo : public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	DllExport CvUnitFormationInfo();
	virtual ~CvUnitFormationInfo();

	DllExport const TCHAR* getFormationType() const;
	int getNumEventTypes() const;
	const EntityEventTypes &getEventType(int index) const;
	DllExport const std::vector<EntityEventTypes> & getEventTypes() const;

	DllExport int getNumUnitEntries() const;
	DllExport const CvUnitEntry &getUnitEntry(int index) const;
	DllExport void addUnitEntry(const CvUnitEntry &unitEntry);
	int getNumGreatUnitEntries() const;
	DllExport const CvUnitEntry &getGreatUnitEntry(int index) const;
	int getNumSiegeUnitEntries() const;
	DllExport const CvUnitEntry &getSiegeUnitEntry(int index) const;

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUnitFormationInfo* pClassInfo, CvXMLLoadUtility* pXML);

	//---------------------------------------PUBLIC MEMBER VARIABLES---------------------------------

protected:

	CvString m_szFormationType;
	std::vector<EntityEventTypes>	m_vctEventTypes;		//!< The list of EntityEventTypes that this formation is intended for

	std::vector<CvUnitEntry> m_vctUnitEntries;
	std::vector<CvUnitEntry> m_vctGreatUnitEntries;
	std::vector<CvUnitEntry> m_vctSiegeUnitEntries;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvSpecialUnitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvSpecialUnitInfo :
	public CvInfoBase
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvSpecialUnitInfo();
	virtual ~CvSpecialUnitInfo();

	bool isValid() const;
	bool isCityLoad() const;
	bool isSMLoadSame() const;

	// Arrays

	bool isCarrierUnitAIType(int i) const;				// Exposed to Python
/*	int getProductionTraits(int i) const;*/				// Exposed to Python

	int getCombatPercent() const;
	int getWithdrawalChange() const;
	int getPursuitChange(bool bForLoad = false) const;

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvSpecialUnitInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	bool m_bValid;
	bool m_bCityLoad;
	bool m_bSMLoadSame;

	int m_iCombatPercent;
	int m_iWithdrawalChange;
	int m_iPursuitChange;

	// Arrays
	bool* m_pbCarrierUnitAITypes;
	//int* m_piProductionTraits;

};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitClassInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitClassInfo :
	public CvInfoBase
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvUnitClassInfo();
	virtual ~CvUnitClassInfo();

	int getMaxGlobalInstances() const;				// Exposed to Python
	int getMaxTeamInstances() const;				// Exposed to Python
	int getMaxPlayerInstances() const;				// Exposed to Python
	//TB Unlimited National Units Mod
	bool isUnlimitedException() const;				// Exposed to Python
	//TB Unlimited National Units End
	int getInstanceCostModifier() const;			// Exposed to Python
	int getDefaultUnitIndex() const;				// Exposed to Python
	void setDefaultUnitIndex(int i);
/************************************************************************************************/
/* Afforess					  Start		 12/23/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	bool isUnique() const;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/

	int getDefaultUnitIndexVector();
	CvString getDefaultUnitIndexVectorElement(int i);

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUnitClassInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);
	bool readPass3();

	void getCheckSum(unsigned int& iSum);

	// serialization
	void read(FDataStreamBase* pStream) {}
	void write(FDataStreamBase* pStream) {}

//----------------------PROTECTED MEMBER VARIABLES----------------------------
protected:

	int m_iMaxGlobalInstances;
	int m_iMaxTeamInstances;
	int m_iMaxPlayerInstances;
	//TB Unlimited National Units Mod
	bool m_bUnlimitedException;
	//TB Unlimited National Units End
	int m_iInstanceCostModifier;
	int m_iDefaultUnitIndex;
/************************************************************************************************/
/* Afforess					  Start		 12/23/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	bool m_bUnique;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/



};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvBuildInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvBuildInfo : public CvHotkeyInfo
{
	//---------------------------PUBLIC INTERFACE---------------------------------
public:

	CvBuildInfo();
	virtual ~CvBuildInfo();

	int getTime() const;					// Exposed to Python
	int getCost() const;					// Exposed to Python
	int getTechPrereq() const;				// Exposed to Python
	int getImprovement() const;				// Exposed to Python
	int getRoute() const;					// Exposed to Python
/************************************************************************************************/
/* Afforess					  Start		 05/25/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int getTerrainChange() const;			// Exposed to Python
	int getFeatureChange() const;			// Exposed to Python
	int getObsoleteTech() const;
	bool isMine() const;
	bool isNoTechCanRemoveWithNoProductionGain(int i) const;
	bool isDisabled() const;
	bool isHideObsoleteExempt() const;
	void setDisabled(bool bNewVal);
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	DllExport int getEntityEvent() const;	// Exposed to Python
	DllExport int getMissionType() const;	// Exposed to Python
	void setMissionType(int iNewType);

	bool isKill() const;					// Exposed to Python

	// Arrays

	int getFeatureTech(int i) const;		// Exposed to Python
	int getFeatureTime(int i) const;		// Exposed to Python
	int getFeatureProduction(int i) const;	// Exposed to Python

	bool isFeatureRemove(int i) const;		// Exposed to Python

	// Vectors
	int getPrereqBonusType(int i) const;
	int getNumPrereqBonusTypes() const;
	bool isPrereqBonusType(int i);

	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	//Struct Vector with delayed resolution
	int getNumTerrainStructs() const;
	TerrainStructs& getTerrainStruct(int iIndex);

	int getNumPlaceBonusTypes() const;
	PlaceBonusTypes& getPlaceBonusType(int iIndex);

	bool read(CvXMLLoadUtility* pXML);
	void read(FDataStreamBase* stream) {}
	void write(FDataStreamBase* stream) {}
	void copyNonDefaults(CvBuildInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);

	void getCheckSum(unsigned int& iSum);

	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iTime;
	int m_iCost;
	int m_iTechPrereq;
	int m_iImprovement;
	int m_iRoute;
/************************************************************************************************/
/* Afforess					  Start		 05/25/10											   */
/*																							  */
/*																							  */
/************************************************************************************************/
	int m_iTerrainChange;
	int m_iFeatureChange;
	int m_iObsoleteTech;
	bool m_bMine;
	bool m_bDisabled;
	bool m_bHideObsoleteExempt;
	bool* m_pabNoTechCanRemoveWithNoProductionGain;
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/
	int m_iEntityEvent;
	int m_iMissionType;

	bool m_bKill;

	// Arrays

	int* m_paiFeatureTech;
	int* m_paiFeatureTime;
	int* m_paiFeatureProduction;

	bool* m_pabFeatureRemove;

	std::vector<int> m_aiPrereqBonusTypes;
	std::vector<int> m_aiMapCategoryTypes;

	std::vector<TerrainStructs> m_aTerrainStructs;
	std::vector<PlaceBonusTypes> m_aPlaceBonusTypes;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvOutcomeInfo
//
//  DESC:   Contains info about outcome types which can be the result of a kill or of actions
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvOutcomeInfo :
	public CvInfoBase
{
public:

	CvOutcomeInfo();
	virtual ~CvOutcomeInfo();

	CvWString getMessageText() const;
	TechTypes getPrereqTech() const;
	TechTypes getObsoleteTech() const;
	CivicTypes getPrereqCivic() const;
	bool getToCoastalCity() const;
	bool getFriendlyTerritory() const;
	bool getNeutralTerritory() const;
	bool getHostileTerritory() const;
	bool getBarbarianTerritory() const;
	bool getCity() const;
	bool getNotCity() const;
	bool isCapture() const;
	int getNumPrereqBuildings() const;
	BuildingTypes getPrereqBuilding(int i) const;
	int getNumExtraChancePromotions() const;
	PromotionTypes getExtraChancePromotion(int i) const;
	int getExtraChancePromotionChance(int i) const;
	int getNumReplaceOutcomes() const;
	OutcomeTypes getReplaceOutcome(int i) const;

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvOutcomeInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void copyNonDefaultsReadPass2(CvOutcomeInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL, bool bOver = false);

	void getCheckSum(unsigned int& iSum);

protected:

	TechTypes m_ePrereqTech;
	TechTypes m_eObsoleteTech;
	CivicTypes m_ePrereqCivic;
	bool m_bToCoastalCity;
	bool m_bFriendlyTerritory;
	bool m_bNeutralTerritory;
	bool m_bHostileTerritory;
	bool m_bBarbarianTerritory;
	bool m_bCity;
	bool m_bNotCity;
	bool m_bCapture;
	std::vector<BuildingTypes> m_aePrereqBuildings;
	std::vector<std::pair<PromotionTypes, int> > m_aeiExtraChancePromotions;
	std::vector<OutcomeTypes> m_aeReplaceOutcomes;
	CvWString m_szMessageText;
};

//TB Promotion Line Mod begin
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvPromotionLineInfo
//
//  DESC:   Contains info about Promotion Lines, definitions of promotion upgrade chains
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvPromotionLineInfo :
	public CvInfoBase
{
public:

	CvPromotionLineInfo();
	virtual ~CvPromotionLineInfo();

	TechTypes getPrereqTech() const;
	TechTypes getObsoleteTech() const;
	PropertyTypes getPropertyType() const;
	int getToleranceBuildup() const;
	int getToleranceDecay() const;
	int getCommunicability() const;
	int getWorseningProbabilityIncrementModifier() const;
	int getWorsenedCommunicabilityIncrementModifier() const;
	int getWorsenedOvercomeIncrementModifier() const;
	int getOvercomeProbability() const;
	int getOvercomeAdjperTurn() const;
	int getOutbreakModifier() const;
	int getOvercomeModifier() const;
	bool isAffliction() const;
	bool isEquipment() const;
	bool isCritical() const;
	bool isNoSpreadonBattle() const;
	bool isNoSpreadUnitProximity() const;
	bool isNoSpreadUnittoCity() const;
	bool isNoSpreadCitytoUnit() const;
	bool isBuildUp() const;
	bool isPoison() const;

	// bool vectors without delayed resolution
	int getUnitCombatPrereqType(int i) const;
	int getNumUnitCombatPrereqTypes() const;
	bool isUnitCombatPrereqType(int i);

	int getNotOnUnitCombatType(int i) const;
	int getNumNotOnUnitCombatTypes() const;
	bool isNotOnUnitCombatType(int i);

	int getNotOnDomainType(int i) const;
	int getNumNotOnDomainTypes() const;
	bool isNotOnDomainType(int i);

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i);

	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i);

	int getCriticalOriginCombatClassType(int i) const;
	int getNumCriticalOriginCombatClassTypes() const;
	bool isCriticalOriginCombatClassType(int i);

	// int vector utilizing pairing without delayed resolution
	int getNumUnitCombatContractChanceChanges() const;
	int getUnitCombatContractChanceChange(int iUnitCombat) const;
	bool isUnitCombatContractChanceChange(int iUnitCombat);

	int getNumUnitCombatOvercomeChanges() const;
	int getUnitCombatOvercomeChange(int iUnitCombat) const;
	bool isUnitCombatOvercomeChange(int iUnitCombat);

	int getNumTechContractChanceChanges() const;
	int getTechContractChanceChange(int iTech) const;
	bool isTechContractChanceChange(int iTech);

	int getNumTechOvercomeChanges() const;
	int getTechOvercomeChange(int iTech) const;
	bool isTechOvercomeChange(int iTech);

	//Post Load

	int getPromotion(int i) const;
	int getNumPromotions() const;
	bool isPromotion(int i);
	void setPromotions();

	int getBuilding(int i) const;
	int getNumBuildings() const;
	bool isBuilding(int i);
	void setBuildings();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvPromotionLineInfo* pClassInfo, CvXMLLoadUtility* pXML);

	void getCheckSum(unsigned int& iSum);

protected:

	TechTypes m_ePrereqTech;
	TechTypes m_eObsoleteTech;
	PropertyTypes m_ePropertyType;
	int m_iToleranceBuildup;
	int m_iToleranceDecay;
	int m_iCommunicability;
	int m_iWorseningProbabilityIncrementModifier;
	int m_iWorsenedCommunicabilityIncrementModifier;
	int m_iWorsenedOvercomeIncrementModifier;
	int m_iOvercomeProbability;
	int m_iOvercomeAdjperTurn;
	int m_iOutbreakModifier;
	int m_iOvercomeModifier;
	bool m_bAffliction;
	bool m_bEquipment;
	bool m_bCritical;
	bool m_bNoSpreadonBattle;
	bool m_bNoSpreadUnitProximity;
	bool m_bNoSpreadUnittoCity;
	bool m_bNoSpreadCitytoUnit;
	bool m_bBuildUp;
	bool m_bPoison;


	// bool vectors without delayed resolution
	std::vector<int> m_aiUnitCombatPrereqTypes;
	std::vector<int> m_aiNotOnUnitCombatTypes;
	std::vector<int> m_aiNotOnDomainTypes;
	std::vector<int> m_aiOnGameOptions;
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiCriticalOriginCombatClassTypes;
	std::vector<int> m_aiPromotions;
	std::vector<int> m_aiBuildings;

	// int vector utilizing pairing without delayed resolution
	UnitCombatModifierArray m_aUnitCombatContractChanceChanges;
	UnitCombatModifierArray m_aUnitCombatOvercomeChanges;
	TechModifierArray m_aTechContractChanceChanges;
	TechModifierArray m_aTechOvercomeChanges;
};
//TB Promotion Line Mod end

// AIAndy: A proper info class for unit combat types
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitCombatInfo
//
//  DESC:   Contains unit combat types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvUnitCombatInfo :
	public CvInfoBase
{
public:

	CvUnitCombatInfo();
	virtual ~CvUnitCombatInfo();

	bool read(CvXMLLoadUtility* pXML);
	void copyNonDefaults(CvUnitCombatInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void getCheckSum(unsigned int& iSum);

	//bool readPass2(CvXMLLoadUtility* pXML);
	//void copyNonDefaultsReadPass2(CvUnitCombatInfo* pClassInfo = NULL , CvXMLLoadUtility* pXML = NULL);

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	static UnitCombatTypes getUnitCombatAnimal()	{ return m_UnitCombatAnimal; }
	static void setUnitCombatAnimal(UnitCombatTypes eIndex) { m_UnitCombatAnimal = eIndex; }
	static UnitCombatTypes getUnitCombatWildAnimal()	{ return m_UnitCombatWildAnimal; }
	static void setUnitCombatWildAnimal(UnitCombatTypes eIndex) { m_UnitCombatWildAnimal = eIndex; }

	CvOutcomeList* getKillOutcomeList();
	int getNumActionOutcomes();
	CvOutcomeList* getActionOutcomeList(int index);
	MissionTypes getActionOutcomeMission(int index);
	CvOutcomeList* getActionOutcomeListByMission(MissionTypes eMission);
	CvOutcomeMission* getOutcomeMission(int index);
	CvOutcomeMission* getOutcomeMissionByMission(MissionTypes eMission);

protected:

	static UnitCombatTypes m_UnitCombatAnimal;
	static UnitCombatTypes m_UnitCombatWildAnimal;

	CvOutcomeList m_KillOutcomeList;
	std::vector<CvOutcomeMission*> m_aOutcomeMissions;

public:
	// Textual References
	ReligionTypes getReligion() const;
	BonusTypes getCulture() const;
	EraTypes getEra() const;
	// integers
	int getAssetMultiplier() const;
	int getPowerMultiplier() const;
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
	int getPursuitChange(bool bForLoad = false) const;
	int getEarlyWithdrawChange(bool bForLoad = false) const;
	int getVSBarbsChange() const;
	int getArmorChange() const;
	int getPunctureChange() const;
	int getOverrunChange(bool bForLoad = false) const;
	int getRepelChange(bool bForLoad = false) const;
	int getFortRepelChange(bool bForLoad = false) const;
	int getRepelRetriesChange(bool bForLoad = false) const;
	int getUnyieldingChange(bool bForLoad = false) const;
	int getKnockbackChange(bool bForLoad = false) const;
	int getKnockbackRetriesChange(bool bForLoad = false) const;
	int getStrAdjperAttChange(bool bForLoad = false) const;
	int getStrAdjperDefChange(bool bForLoad = false) const;
	int getWithdrawAdjperAttChange(bool bForLoad = false) const;
	int getUnnerveChange(bool bForLoad = false) const;
	int getEncloseChange(bool bForLoad = false) const;
	int getLungeChange(bool bForLoad = false) const;
	int getDynamicDefenseChange(bool bForLoad = false) const;
	int getStrengthChange() const;
	int getFortitudeChange() const;
	int getFrontSupportPercentChange(bool bForLoad = false) const;
	int getShortRangeSupportPercentChange(bool bForLoad = false) const;
	int getMediumRangeSupportPercentChange(bool bForLoad = false) const;
	int getLongRangeSupportPercentChange(bool bForLoad = false) const;
	int getFlankSupportPercentChange(bool bForLoad = false) const;
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
	int getHillsWorkModifierChange() const;
	int getPeaksWorkModifierChange() const;
	int getBreakdownChanceChange() const;
	int getBreakdownDamageChange() const;
	int getTauntChange() const;
	int getMaxHPChange(bool bForLoad = false) const;
	int getStrengthModifier() const;
	int getQualityBase() const;
	int getGroupBase() const;
	int getSizeBase() const;
	int getDamageModifierChange() const;
	int getCostModifierChange() const;
	int getRBombardDamageBase() const;
	int getRBombardDamageLimitBase() const;
	int getRBombardDamageMaxUnitsBase() const;
	int getDCMBombRangeBase() const;
	int getDCMBombAccuracyBase() const;
	int getCombatModifierPerSizeMoreChange(bool bForLoad = false) const;
	int getCombatModifierPerSizeLessChange(bool bForLoad = false) const;
	int getCombatModifierPerVolumeMoreChange(bool bForLoad = false) const;
	int getCombatModifierPerVolumeLessChange(bool bForLoad = false) const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getExcileChange() const;
	int getPassageChange() const;
	int getNoNonOwnedCityEntryChange() const;
	int getBarbCoExistChange() const;
	int getBlendIntoCityChange() const;
	int getInsidiousnessChange() const;
	int getInvestigationChange() const;
	int getStealthStrikesChange(bool bForLoad = false) const;
	int getStealthCombatModifierChange(bool bForLoad = false) const;
	int getStealthDefenseChange(bool bForLoad = false) const;
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
	bool isPillageCulture() const;
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
	inline int	getZobristValue() const { return m_zobristValue; }

	// Arrays
	int getDomainModifierPercent(int i) const;				// Exposed to Python
	bool isAnyDomainModifierPercent() const;

	// bool vector with delayed resolution
	int getCureAfflictionChangeType(int i) const;
	int getNumCureAfflictionChangeTypes() const;
	bool isCureAfflictionChangeType(int i);

	int getTerrainIgnoreDamageChangeType(int i) const;
	int getNumTerrainIgnoreDamageChangeTypes() const;
	bool isTerrainIgnoreDamageChangeType(int i);

	int getTerrainDoubleMoveChangeType(int i) const;
	int getNumTerrainDoubleMoveChangeTypes() const;
	bool isTerrainDoubleMoveChangeType(int i);

	int getFeatureDoubleMoveChangeType(int i) const;
	int getNumFeatureDoubleMoveChangeTypes() const;
	bool isFeatureDoubleMoveChangeType(int i);

	int getOnGameOption(int i) const;
	int getNumOnGameOptions() const;
	bool isOnGameOption(int i);

	int getNotOnGameOption(int i) const;
	int getNumNotOnGameOptions() const;
	bool isNotOnGameOption(int i);

	int getGGptsforUnitType(int i) const;
	int getNumGGptsforUnitTypes() const;
	bool isGGptsforUnitType(int i);

	int getDefaultStatusType(int i) const;
	int getNumDefaultStatusTypes() const;
	bool isDefaultStatusType(int i);

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i);

	// int vector utilizing pairing without delayed resolution
	int getNumWithdrawOnTerrainTypeChanges() const;
	int getWithdrawOnTerrainTypeChange(int iTerrain) const;

	int getNumWithdrawOnFeatureTypeChanges() const;
	int getWithdrawOnFeatureTypeChange(int iFeature) const;

	int getNumVisibilityIntensityChangeTypes() const;
	int getVisibilityIntensityChangeType(int iInvisibility) const;
	bool isVisibilityIntensityChangeType(int iInvisibility);

	int getNumInvisibilityIntensityChangeTypes() const;
	int getInvisibilityIntensityChangeType(int iInvisibility) const;
	bool isInvisibilityIntensityChangeType(int iInvisibility);

	int getNumVisibilityIntensityRangeChangeTypes() const;
	int getVisibilityIntensityRangeChangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeChangeType(int iInvisibility);

	int getNumVisibilityIntensitySameTileChangeTypes() const;
	int getVisibilityIntensitySameTileChangeType(int iInvisibility) const;
	bool isVisibilityIntensitySameTileChangeType(int iInvisibility);

	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty);

	// int vector utilizing struct with delayed resolution
	int getNumAfflictionFortitudeChangeModifiers() const;
	PromotionLineModifier& getAfflictionFortitudeChangeModifier(int iAfflictionLine);

	int getNumTerrainAttackChangeModifiers() const;
	TerrainModifier& getTerrainAttackChangeModifier(int iTerrain);

	int getNumTerrainDefenseChangeModifiers() const;
	TerrainModifier& getTerrainDefenseChangeModifier(int iTerrain);

	int getNumTerrainWorkChangeModifiers() const;
	TerrainModifier& getTerrainWorkChangeModifier(int iTerrain);

	int getNumBuildWorkChangeModifiers() const;
	BuildModifier& getBuildWorkChangeModifier(int iBuild);

	int getNumFeatureAttackChangeModifiers() const;
	FeatureModifier& getFeatureAttackChangeModifier(int iFeature);

	int getNumFeatureDefenseChangeModifiers() const;
	FeatureModifier& getFeatureDefenseChangeModifier(int iFeature);

	int getNumFeatureWorkChangeModifiers() const;
	FeatureModifier& getFeatureWorkChangeModifier(int iFeature);

	int getNumUnitCombatChangeModifiers() const;
	UnitCombatModifier& getUnitCombatChangeModifier(int iUnitCombat);

	int getNumFlankingStrengthbyUnitCombatTypesChange() const;
	UnitCombatModifier& getFlankingStrengthbyUnitCombatTypeChange(int iUnitCombat);

	int getNumWithdrawVSUnitCombatTypesChange() const;
	UnitCombatModifier& getWithdrawVSUnitCombatTypeChange(int iUnitCombat);

	//Fight or Flight only
	int getNumPursuitVSUnitCombatTypesChange() const;
	UnitCombatModifier& getPursuitVSUnitCombatTypeChange(int iUnitCombat);

	//Heart of War only
	int getNumRepelVSUnitCombatTypesChange() const;
	UnitCombatModifier& getRepelVSUnitCombatTypeChange(int iUnitCombat);

	//Heart of War only
	int getNumKnockbackVSUnitCombatTypesChange() const;
	UnitCombatModifier& getKnockbackVSUnitCombatTypeChange(int iUnitCombat);

	int getNumPunctureVSUnitCombatTypesChange() const;
	UnitCombatModifier& getPunctureVSUnitCombatTypeChange(int iUnitCombat);

	int getNumArmorVSUnitCombatTypesChange() const;
	UnitCombatModifier& getArmorVSUnitCombatTypeChange(int iUnitCombat);

	int getNumDodgeVSUnitCombatTypesChange() const;
	UnitCombatModifier& getDodgeVSUnitCombatTypeChange(int iUnitCombat);

	int getNumPrecisionVSUnitCombatTypesChange() const;
	UnitCombatModifier& getPrecisionVSUnitCombatTypeChange(int iUnitCombat);

	int getNumCriticalVSUnitCombatTypesChange() const;
	UnitCombatModifier& getCriticalVSUnitCombatTypeChange(int iUnitCombat);

	int getNumRoundStunVSUnitCombatTypesChange() const;
	UnitCombatModifier& getRoundStunVSUnitCombatTypeChange(int iUnitCombat);

	int getNumTrapAvoidanceUnitCombatTypes() const;
	UnitCombatModifier& getTrapAvoidanceUnitCombatType(int iIndex);

	int getNumAfflictOnAttackChangeTypes() const;
	AfflictOnAttackChange& getAfflictOnAttackChangeType(int iAfflictionLine);

	int getNumInvisibleTerrainChanges() const;
	InvisibleTerrainChanges& getInvisibleTerrainChange(int iIndex);

	int getNumInvisibleFeatureChanges() const;
	InvisibleFeatureChanges& getInvisibleFeatureChange(int iIndex);

	int getNumInvisibleImprovementChanges() const;
	InvisibleImprovementChanges& getInvisibleImprovementChange(int iIndex);

	int getNumVisibleTerrainChanges() const;
	InvisibleTerrainChanges& getVisibleTerrainChange(int iIndex);

	int getNumVisibleFeatureChanges() const;
	InvisibleFeatureChanges& getVisibleFeatureChange(int iIndex);

	int getNumVisibleImprovementChanges() const;
	InvisibleImprovementChanges& getVisibleImprovementChange(int iIndex);

	int getNumVisibleTerrainRangeChanges() const;
	InvisibleTerrainChanges& getVisibleTerrainRangeChange(int iIndex);

	int getNumVisibleFeatureRangeChanges() const;
	InvisibleFeatureChanges& getVisibleFeatureRangeChange(int iIndex);

	int getNumVisibleImprovementRangeChanges() const;
	InvisibleImprovementChanges& getVisibleImprovementRangeChange(int iIndex);

	int getNumDistanceAttackCommunicabilityTypeChanges() const;
	AfflictionLineChanges& getDistanceAttackCommunicabilityTypeChange(int iIndex);

	//Propery Manipulators
	CvPropertyManipulators* getPropertyManipulators();

protected:
	// Textual References
	ReligionTypes m_eReligion;
	BonusTypes m_eCulture;
	EraTypes m_eEra;
	//Integers
	int m_iAssetMultiplier;
	int m_iPowerMultiplier;
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
	int m_iStrAdjperAttChange;
	int m_iStrAdjperDefChange;
	int m_iWithdrawAdjperAttChange;
	int m_iUnnerveChange;
	int m_iEncloseChange;
	int m_iLungeChange;
	int m_iDynamicDefenseChange;
	int m_iStrengthChange;
	int m_iFortitudeChange;
	int m_iFrontSupportPercentChange;
	int m_iShortRangeSupportPercentChange;
	int m_iMediumRangeSupportPercentChange;
	int m_iLongRangeSupportPercentChange;
	int m_iFlankSupportPercentChange;
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
	int m_iHillsWorkModifierChange;
	int m_iPeaksWorkModifierChange;
	int m_iBreakdownChanceChange;
	int m_iBreakdownDamageChange;
	int m_iTauntChange;
	int m_iMaxHPChange;
	int m_iStrengthModifier;
	int m_iQualityBase;
	int m_iGroupBase;
	int m_iSizeBase;
	int m_iDamageModifierChange;
	int m_iCostModifierChange;
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
	bool m_bPillageCulture;
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
	bool m_bAnyDomainModifierPercent;

	// bool vectors without delayed resolution
	// bool vector with delayed resolution
	std::vector<int> m_aiCureAfflictionChangeTypes;
	std::vector<int> m_aiTerrainIgnoreDamageChangeTypes;
	std::vector<int> m_aiTerrainDoubleMoveChangeTypes;
	std::vector<int> m_aiFeatureDoubleMoveChangeTypes;
	std::vector<int> m_aiOnGameOptions;
	std::vector<int> m_aiNotOnGameOptions;
	std::vector<int> m_aiGGptsforUnitTypes;
	std::vector<int> m_aiDefaultStatusTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	// int vectors utilizing pairing without delayed resolution
	TerrainModifierArray m_aWithdrawOnTerrainTypesChange;
	FeatureModifierArray m_aWithdrawOnFeatureTypesChange;
	InvisibilityArray m_aVisibilityIntensityChangeTypes;
	InvisibilityArray m_aInvisibilityIntensityChangeTypes;
	InvisibilityArray m_aVisibilityIntensityRangeChangeTypes;
	InvisibilityArray m_aVisibilityIntensitySameTileChangeTypes;
	AidArray m_aAidChanges;
	// int vector utilizing struct with delayed resolution
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeChangeModifiers;
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
	std::vector<AfflictOnAttackChange> m_aAfflictOnAttackChangeTypes;
	std::vector<InvisibleTerrainChanges> m_aInvisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aInvisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aInvisibleImprovementChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainChanges;
	std::vector<InvisibleFeatureChanges> m_aVisibleFeatureRangeChanges;
	std::vector<InvisibleImprovementChanges> m_aVisibleImprovementRangeChanges;
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
	std::vector<InvisibleTerrainChanges> m_aVisibleTerrainRangeChanges;

	CvPropertyManipulators m_PropertyManipulators;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvInvisibleInfo
//
//  DESC:   Contains info about Invisible Types
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvInvisibleInfo :
	public CvInfoBase
{
public:

	CvInvisibleInfo();
	virtual ~CvInvisibleInfo();

	virtual void read(FDataStreamBase* pStream) {}
	virtual void write(FDataStreamBase* pStream) {}

	bool read(CvXMLLoadUtility* pXML);
	bool readPass2(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvInvisibleInfo* pClassInfo, CvXMLLoadUtility* pXML);
	void getCheckSum(unsigned int& iSum);

	//References
	//bools
	//integers

	//Processes
	int getChar() const;
	void setChar(int i);
	int getFontButtonIndex() const;

protected:
	//References
	//bools
	//Processes
	int m_iChar;
	int m_iFontButtonIndex;
};

#endif /* CV_INFO_UNITCOMBAT_H */

