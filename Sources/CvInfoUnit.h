#ifndef CV_INFO_UNIT_H
#define CV_INFO_UNIT_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//  class : CvUnitInfo
//
//  DESC:
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CvArtInfoUnit;
class CvUnitInfo : public CvHotkeyInfo
{
//---------------------------PUBLIC INTERFACE---------------------------------
public:
	CvUnitInfo();
	virtual ~CvUnitInfo();

	int getAIWeight() const;					// Exposed to Python
	int getProductionCost() const;				// Exposed to Python
	int getHurryCostModifier() const;			// Exposed to Python
	int getAdvancedStartCost() const;			// Exposed to Python
	int getAdvancedStartCostIncrease() const;	// Exposed to Python
	int getMinAreaSize() const;					// Exposed to Python
	int getMoves() const;						// Exposed to Python
	int getAirRange() const;					// Exposed to Python
	int getAirUnitCap() const;					// Exposed to Python
	int getDropRange() const;					// Exposed to Python
	int getNukeRange() const;					// Exposed to Python
	int getWorkRate() const;					// Exposed to Python
	int getBaseDiscover() const;				// Exposed to Python
	int getDiscoverMultiplier() const;			// Exposed to Python
	int getBaseHurry() const;					// Exposed to Python
	int getHurryMultiplier() const;				// Exposed to Python
	int getBaseTrade() const;					// Exposed to Python
	int getTradeMultiplier() const;				// Exposed to Python
	int getGreatWorkCulture() const;			// Exposed to Python
	int getEspionagePoints() const;				// Exposed to Python
	int getCombat() const;						// Exposed to Python

	void setCombat(int iNum);					// Exposed to Python
	int getCombatLimit() const;					// Exposed to Python
	//TB Tweaked for Size Matters
	int getAirCombat() const;					// Exposed to Python

	int getAirCombatLimit() const;				// Exposed to Python
	int getXPValueAttack() const;				// Exposed to Python
	int getXPValueDefense() const;				// Exposed to Python
	int getFirstStrikes() const;				// Exposed to Python
	int getChanceFirstStrikes() const;			// Exposed to Python
	int getInterceptionProbability() const;		// Exposed to Python
	//ls612: Advanced Nuke Interception
	//int getNukeInterceptionProbability() const;
	//int getNukeInterceptionRange() const;
	int getEvasionProbability() const;			// Exposed to Python
	int getWithdrawalProbability(bool bForLoad = false) const;	// Exposed to Python
	int getCollateralDamage() const;			// Exposed to Python
	int getCollateralDamageLimit() const;		// Exposed to Python
	int getCollateralDamageMaxUnits() const;	// Exposed to Python
	int getCityAttackModifier() const;			// Exposed to Python
	int getCityDefenseModifier() const;			// Exposed to Python
	int getAnimalCombatModifier() const;		// Exposed to Python
	int getHillsAttackModifier() const;			// Exposed to Python
	int getHillsDefenseModifier() const;		// Exposed to Python
	int getBombRate() const;					// Exposed to Python
	int getBombardRate() const;					// Exposed to Python
	int getSpecialCargo(bool bForLoad = false) const;	// Exposed to Python
	int getSMSpecialCargo() const;
	int getSMNotSpecialCargo() const;
	int getDomainCargo() const;					// Exposed to Python
	int getCargoSpace() const;					// Exposed to Python
	int getSMCargoSpace() const;
	int getSMCargoVolume() const;
	int getConscriptionValue() const;			// Exposed to Python
	int getCultureGarrisonValue() const;		// Exposed to Python
	int getExtraCost() const;					// Exposed to Python
	int getAssetValue(bool bForLoad = false) const;	// Exposed to Python
	int getPowerValue(bool bForLoad = false) const;	// Exposed to Python
	int getUnitClassType() const;					// Exposed to Python
	int getSpecialUnitType() const;					// Exposed to Python
	int getUnitCaptureClassType() const;			// Exposed to Python
	int getUnitCombatType() const;					// Exposed to Python
	int getDomainType() const;						// Exposed to Python
	int getDefaultUnitAIType() const;				// Exposed to Python
	int getInvisibleType() const;					// Exposed to Python
	int getSeeInvisibleType(int i) const;			// Exposed to Python
	int getNumSeeInvisibleTypes() const;			// Exposed to Python
	int getAdvisorType() const;						// Exposed to Python

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain 																*/
/********************************************************************************/
	int getMaxStartEra() const;						// Exposed to Python
	int getForceObsoleteTech() const;				// Exposed to Python
	bool isStateReligion() const;					// Exposed to Python
	int getPrereqGameOption() const;				// Exposed to Python
	int getNotGameOption() const;					// Exposed to Python
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	int getHolyCity() const;						// Exposed to Python
	int getReligionType() const;					// Exposed to Python
	int getStateReligion() const;					// Exposed to Python
	int getPrereqReligion() const;					// Exposed to Python
	int getPrereqCorporation() const;				// Exposed to Python
	int getPrereqBuilding() const;					// Exposed to Python
	int getPrereqOrBuildingsNum() const;			// Exposed to Python
	BuildingTypes getPrereqOrBuilding(int i) const;	// Exposed to Python
	int getPrereqAndTech() const;					// Exposed to Python
	int getPrereqAndBonus() const;					// Exposed to Python
	// the initial number of individuals in the unit group
	int getGroupSize() const;						// Exposed to Python
	// the number of UnitMeshGroups for this unit
	int getGroupDefinitions() const;				// Exposed to Python
	int getMeleeWaveSize() const;					// Exposed to Python
	int getRangedWaveSize() const;					// Exposed to Python
	int getNumUnitNames() const;					// Exposed to Python
	int getCommandType() const;						// Exposed to Python
	void setCommandType(int iNewType);
	//	This really belongs on CvInfoBase but you can't change the size of that
	//	object without crashing the core engine :-(
	inline int	getZobristValue() const { return m_zobristValue; }

	bool isFoodProduction() const;				// Exposed to Python
	bool isNoBadGoodies() const;				// Exposed to Python
	bool isOnlyDefensive() const;				// Exposed to Python
	bool isNoCapture() const;					// Exposed to Python
	bool isQuickCombat() const;					// Exposed to Python
	bool isRivalTerritory() const;				// Exposed to Python
	bool isMilitaryHappiness() const;			// Exposed to Python
	bool isMilitarySupport() const;				// Exposed to Python
	bool isMilitaryProduction() const;			// Exposed to Python
	bool isPillage() const;						// Exposed to Python
	bool isSpy() const;							// Exposed to Python
	bool isSabotage() const;					// Exposed to Python
	bool isDestroy() const;						// Exposed to Python
	bool isStealPlans() const;					// Exposed to Python
	bool isInvestigate() const;					// Exposed to Python
	bool isCounterSpy() const;					// Exposed to Python
	bool isFound() const;						// Exposed to Python
	bool isGoldenAge() const;					// Exposed to Python
	bool isInvisible() const;					// Exposed to Python
	void setInvisible(bool bEnable) ;			// Exposed to Python
	bool isFirstStrikeImmune() const;			// Exposed to Python
	bool isNoDefensiveBonus() const;			// Exposed to Python
	bool isIgnoreBuildingDefense() const;		// Exposed to Python
	bool isCanMoveImpassable() const;			// Exposed to Python
	bool isCanMoveAllTerrain() const;			// Exposed to Python
	bool isFlatMovementCost() const;			// Exposed to Python
	bool isIgnoreTerrainCost() const;			// Exposed to Python
	bool isNukeImmune() const;					// Exposed to Python
	bool isPrereqBonuses() const;				// Exposed to Python
	bool isPrereqReligion() const;				// Exposed to Python
	bool isMechUnit() const;					// Exposed to Python
	bool isRenderBelowWater() const;			// Exposed to Python
	bool isRenderAlways() const;				// Exposed to Python
	bool isSuicide() const;						// Exposed to Python
	bool isLineOfSight() const;					// Exposed to Python
	bool isHiddenNationality() const;			// Exposed to Python
	bool isAlwaysHostile() const;				// Exposed to Python
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 21.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	bool isFreeDrop() const;					// Exposed to Python
/*****************************************************************************************************/
/**  TheLadiesOgre; 21.09.2009; TLOTags															 **/
/*****************************************************************************************************/
	bool isNoRevealMap() const;					// Exposed to Python

/********************************************************************************/
/**		REVDCM_OC							2/16/10				phungus420		*/
/**																				*/
/**		 																		*/
/********************************************************************************/
	bool isInquisitor() const;					// Exposed to Python
/********************************************************************************/
/**		REVDCM_OC									END							*/
/********************************************************************************/
	//ls612: Can't enter non-Owned cities
	bool isNoNonOwnedEntry() const;

/************************************************************************************************/
/* Afforess					  Start		 12/9/09												*/
/*																							  */
/*																							  */
/************************************************************************************************/
	void setPowerValue(int iNewValue);
	int getPrereqVicinityBonus() const;
	bool isRequiresStateReligionInCity() const;
	bool isWorkerTrade() const;
	bool isMilitaryTrade() const;
	bool isForceUpgrade() const;
	bool isGreatGeneral() const;
	bool isSlave() const;
	int getPrereqOrVicinityBonuses(int i) const;
	bool getPassableRouteNeeded(int i) const;
	std::vector<int> getUpgradeUnitClassTypes() const;
	void addUpgradeUnitClassTypes(int);
	bool isUpgradeUnitClassTypes(int);
	int getBaseFoodChange() const;
	int getControlPoints() const;
	int getCommandRange() const;
	const TCHAR* getClassicalArtDefineTag(int i, UnitArtStyleTypes eStyle) const;	// Exposed to Python
	void setClassicalArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getRennArtDefineTag(int i, UnitArtStyleTypes eStyle) const;		// Exposed to Python
	void setRennArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getIndustrialArtDefineTag(int i, UnitArtStyleTypes eStyle) const;	// Exposed to Python
	void setIndustrialArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getFutureArtDefineTag(int i, UnitArtStyleTypes eStyle) const;		// Exposed to Python
	void setFutureArtDefineTag(int i, const TCHAR* szVal);

	CvWString getCivilizationName(int i) const;
	int getCivilizationNamesVectorSize();
	CvWString getCivilizationNamesNamesVectorElement(int i);
	CvWString getCivilizationNamesValuesVectorElement(int i);

	//TB Combat Mod Begin  TB SubCombat Mod begin
	//Functions
	void setReligionSubCombat();
	void setCultureSubCombat();
	int getEraInfo() const;
	void setEraSubCombat();
	//integers
	int getAttackCombatModifier() const;
	int getDefenseCombatModifier() const;
	int getPursuit(bool bForLoad = false) const;
	int getEarlyWithdraw(bool bForLoad = false) const;
	int getVSBarbs() const;
	int getArmor() const;
	int getPuncture() const;
	int getOverrun(bool bForLoad = false) const;
	int getRepel(bool bForLoad = false) const;
	int getFortRepel(bool bForLoad = false) const;
	int getRepelRetries(bool bForLoad = false) const;
	int getUnyielding(bool bForLoad = false) const;
	int getKnockback(bool bForLoad = false) const;
	int getKnockbackRetries(bool bForLoad = false) const;
	int getStrAdjperRnd(bool bForLoad = false) const;
	int getStrAdjperAtt(bool bForLoad = false) const;
	int getStrAdjperDef(bool bForLoad = false) const;
	int getWithdrawAdjperAtt(bool bForLoad = false) const;
	int getUnnerve(bool bForLoad = false) const;
	int getEnclose(bool bForLoad = false) const;
	int getLunge(bool bForLoad = false) const;
	int getDynamicDefense(bool bForLoad = false) const;
	int getFortitude(bool bForLoad = false) const;
	int getFrontSupportPercent(bool bForLoad = false) const;
	int getShortRangeSupportPercent(bool bForLoad = false) const;
	int getMediumRangeSupportPercent(bool bForLoad = false) const;
	int getLongRangeSupportPercent(bool bForLoad = false) const;
	int getFlankSupportPercent(bool bForLoad = false) const;
	int getDodgeModifier() const;
	int getPrecisionModifier() const;
	int getPowerShots() const;
	int getPowerShotCombatModifier() const;
	int getPowerShotPunctureModifier() const;
	int getPowerShotPrecisionModifier() const;
	int getPowerShotCriticalModifier() const;
	int getCriticalModifier() const;
	int getEndurance() const;
	int getRoundStunProb() const;
	int getPoisonProbabilityModifier() const;
	//Team Project (3)
	int getCaptureProbabilityModifier() const;
	int getCaptureResistanceModifier() const;
	//Team Project (4)
	//WorkRateMod
	int getHillsWorkModifier() const;
	int getPeaksWorkModifier() const;

	int getBreakdownChance() const;
	int getBreakdownDamage() const;
	int getTaunt(bool bForLoad = false) const;
	int getMaxHP(bool bForLoad = false) const;
	int getDamageModifier() const;
	int getCostModifier() const;
	int getRBombardDamage() const;
	int getRBombardDamageLimit() const;
	int getRBombardDamageMaxUnits() const;
	int getCombatModifierPerSizeMore(bool bForLoad = false) const;
	int getCombatModifierPerSizeLess(bool bForLoad = false) const;
	int getCombatModifierPerVolumeMore(bool bForLoad = false) const;
	int getCombatModifierPerVolumeLess(bool bForLoad = false) const;
	int getSelfHealModifier() const;
	int getNumHealSupport() const;
	int getInsidiousness() const;
	int getInvestigation() const;
	int getStealthStrikes(bool bForLoad = false) const;
	int getStealthCombatModifier(bool bForLoad = false) const;
	int getTrapDamageMax() const;
	int getTrapDamageMin() const;
	int getTrapComplexity() const;
	int getNumTriggers() const;
	int getAggression() const;
	int getAnimalIgnoresBorders() const;
	int getReligiousCombatModifier() const;
	// booleans
	bool isStampede() const;
	bool isOnslaught() const;
	bool isDealsColdDamage() const;
	bool isColdImmune() const;
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
	bool isStealthDefense(bool bForLoad = false) const;
	bool isNoInvisibility() const;
	bool isTriggerBeforeAttack() const;
	bool isAnimal() const;
	bool isWildAnimal() const;
	bool canAnimalIgnoresBorders() const;
	bool canAnimalIgnoresImprovements() const;
	bool canAnimalIgnoresCities() const;
	bool isNoNonTypeProdMods() const;
	bool isGatherHerd() const;

	// boolean vectors without delayed resolution
	int getSubCombatType(int i) const;
	int getNumSubCombatTypes() const;
	bool isSubCombatType(int i);

	int getCureAfflictionType(int i) const;
	int getNumCureAfflictionTypes() const;
	bool isCureAfflictionType(int i);

	int getHealAsType(int i) const;
	int getNumHealAsTypes() const;
	bool isHealAsType(int i);
	void setHealAsTypes();

	int getTerrainImpassableType(int i) const;
	int getNumTerrainImpassableTypes() const;
	bool isTerrainImpassableType(int i);

	int getFeatureImpassableType(int i) const;
	int getNumFeatureImpassableTypes() const;
	bool isFeatureImpassableType(int i);

	int getMapCategoryType(int i) const;
	int getNumMapCategoryTypes() const;
	bool isMapCategoryType(int i);

	int getTrapSetWithPromotionType(int i) const;
	int getNumTrapSetWithPromotionTypes() const;
	bool isTrapSetWithPromotionType(int i);

	int getTrapImmunityUnitCombatType(int i) const;
	int getNumTrapImmunityUnitCombatTypes() const;
	bool isTrapImmunityUnitCombatType(int i);

	// int vector utilizing struct with delayed resolution
	int getNumAfflictionFortitudeModifiers() const;
	PromotionLineModifier& getAfflictionFortitudeModifier(int iAffliction);

	int getNumAfflictOnAttackTypes() const;
	AfflictOnAttack& getAfflictOnAttackType(int iAfflictionLine);

	int getNumHealUnitCombatTypes() const;
	HealUnitCombat& getHealUnitCombatType(int iUnitCombat);

	int getNumGroupSpawnUnitCombatTypes() const;
	GroupSpawnUnitCombat& getGroupSpawnUnitCombatType(int iIndex);

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

	int getNumDistanceAttackCommunicabilityTypeChanges() const;
	InvisibleImprovementChanges& getVisibleImprovementRangeChange(int iIndex);

	int getNumVisibleImprovementRangeChanges() const;
	AfflictionLineChanges& getDistanceAttackCommunicabilityTypeChange(int iIndex);

	//Bool vector utilizing delayed resolution
	int getNumEnabledCivilizationTypes() const;
	EnabledCivilizations& getEnabledCivilizationType(int iIndex);

	// int vector utilizing pairing without delayed resolution
	int getNumFlankingStrikesbyUnitCombatTypes() const;
	int getFlankingStrengthbyUnitCombatType(int iUnitCombat) const;
	bool isFlankingStrikebyUnitCombatType(int iUnitCombat);

	int getNumWithdrawOnTerrainTypes() const;
	int getWithdrawOnTerrainType(int iTerrain) const;

	int getNumWithdrawOnFeatureTypes() const;
	int getWithdrawOnFeatureType(int iFeature) const;

	int getNumWithdrawVSUnitCombatTypes() const;
	int getWithdrawVSUnitCombatType(int iUnitCombat) const;
	bool isWithdrawVSUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getWithdrawVSUnitCombatTypes();

	int getNumPursuitVSUnitCombatTypes() const;
	int getPursuitVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isPursuitVSUnitCombatType(int iUnitCombat, bool bForLoad = false);
	UnitCombatModifierArray& getPursuitVSUnitCombatTypes();

	int getNumRepelVSUnitCombatTypes() const;
	int getRepelVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isRepelVSUnitCombatType(int iUnitCombat, bool bForLoad = false);
	UnitCombatModifierArray& getRepelVSUnitCombatTypes();

	int getNumKnockbackVSUnitCombatTypes() const;
	int getKnockbackVSUnitCombatType(int iUnitCombat, bool bForLoad = false) const;
	bool isKnockbackVSUnitCombatType(int iUnitCombat, bool bForLoad = false);
	UnitCombatModifierArray& getKnockbackVSUnitCombatTypes();

	int getNumPunctureVSUnitCombatTypes() const;
	int getPunctureVSUnitCombatType(int iUnitCombat) const;
	bool isPunctureVSUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getPunctureVSUnitCombatTypes();

	int getNumArmorVSUnitCombatTypes() const;
	int getArmorVSUnitCombatType(int iUnitCombat) const;
	bool isArmorVSUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getArmorVSUnitCombatTypes();

	int getNumDodgeVSUnitCombatTypes() const;
	int getDodgeVSUnitCombatType(int iUnitCombat) const;
	bool isDodgeVSUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getDodgeVSUnitCombatTypes();

	int getNumPrecisionVSUnitCombatTypes() const;
	int getPrecisionVSUnitCombatType(int iUnitCombat) const;
	bool isPrecisionVSUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getPrecisionVSUnitCombatTypes();

	int getNumCriticalVSUnitCombatTypes() const;
	int getCriticalVSUnitCombatType(int iUnitCombat) const;
	bool isCriticalVSUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getCriticalVSUnitCombatTypes();

	int getNumRoundStunVSUnitCombatTypes() const;
	int getRoundStunVSUnitCombatType(int iUnitCombat) const;
	bool isRoundStunVSUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getRoundStunVSUnitCombatTypes();

	int getNumTrapDisableUnitCombatTypes() const;
	int getTrapDisableUnitCombatType(int iUnitCombat) const;
	bool isTrapDisableUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getTrapDisableUnitCombatTypes();

	int getNumTrapAvoidanceUnitCombatTypes() const;
	int getTrapAvoidanceUnitCombatType(int iUnitCombat) const;
	bool isTrapAvoidanceUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getTrapAvoidanceUnitCombatTypes();

	int getNumTrapTriggerUnitCombatTypes() const;
	int getTrapTriggerUnitCombatType(int iUnitCombat) const;
	bool isTrapTriggerUnitCombatType(int iUnitCombat);
	UnitCombatModifierArray& getTrapTriggerUnitCombatTypes();

	int getNumVisibilityIntensityTypes() const;
	int getVisibilityIntensityType(int iInvisibility) const;
	bool isVisibilityIntensityType(int iInvisibility);
	InvisibilityArray& getVisibilityIntensityTypes();

	int getNumInvisibilityIntensityTypes() const;
	int getInvisibilityIntensityType(int iInvisibility) const;
	bool isInvisibilityIntensityType(int iInvisibility);
	InvisibilityArray& getInvisibilityIntensityTypes();

	int getNumVisibilityIntensityRangeTypes() const;
	int getVisibilityIntensityRangeType(int iInvisibility) const;
	bool isVisibilityIntensityRangeType(int iInvisibility);
	InvisibilityArray& getVisibilityIntensityRangeTypes();
	//Team Project (4)
		//WorkRateMod
	int getNumTerrainWorkRateModifierTypes() const;
	int getTerrainWorkRateModifierType(int iTerrain) const;
	bool isTerrainWorkRateModifierType(int iTerrain);

	int getNumFeatureWorkRateModifierTypes() const;
	int getFeatureWorkRateModifierType(int iFeature) const;
	bool isFeatureWorkRateModifierType(int iFeature);

	int getNumBuildWorkRateModifierTypes() const;
	int getBuildWorkRateModifierType(int iBuild) const;
	bool isBuildWorkRateModifierType(int iBuild);

	int getNumAidChanges() const;
	int getAidChange(int iProperty) const;
	bool isAidChange(int iProperty);


	// TB Combat Mod End TB SubCombat Mod end
	bool hasUnitCombat(UnitCombatTypes eUnitCombat) const;
	void setTotalModifiedCombatStrengthDetails();
	int getCombatStrengthModifier() const;
	int getTotalModifiedCombatStrength100() const;
	int getTotalModifiedAirCombatStrength100() const;
	int getApproaching0Return(int i) const;
	int getBaseGroupRank() const;
	void setBaseCargoVolume();
	int getBaseCargoVolume() const;
	void setBaseSizeMattersZeroPoints();
	int getSMRankTotal() const;
	int getSMVolumetricRankTotal() const;
	void setSM();
	int getQualifiedPromotionType(int i) const;
	int getNumQualifiedPromotionTypes() const;
	bool isQualifiedPromotionType(int i);
	void setQualifiedPromotionTypes();
	void setCanAnimalIgnores();

	virtual const wchar* getExtraHoverText() const;

	bool readPass3();

	void getCheckSum(unsigned int& iSum);

	CvPropertyManipulators* getPropertyManipulators();

	BoolExpr* getTrainCondition();

protected:
	std::vector<int> m_aiUpgradeUnitClassTypes;
	bool* m_pbPassableRouteNeeded;
	int* m_piPrereqOrVicinityBonuses;
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

	BoolExpr* m_pExprTrainCondition;

public:
/************************************************************************************************/
/* Afforess						 END															*/
/************************************************************************************************/


/************************************************************************************************/
/* DCM									 04/19/09								Johny Smith  */
/************************************************************************************************/
	// Dale - RB: Field Bombard START
	int getDCMBombRange() const;
	int getDCMBombAccuracy() const;
protected:
	int m_iDCMBombRange;
	int m_iDCMBombAccuracy;
public:
	// Dale - RB: Field Bombard END
	// Dale - AB: Bombing START
	bool getDCMAirBomb1() const;
	bool getDCMAirBomb2() const;
	bool getDCMAirBomb3() const;
	bool getDCMAirBomb4() const;
	bool getDCMAirBomb5() const;
protected:
	bool m_bDCMAirBomb1;
	bool m_bDCMAirBomb2;
	bool m_bDCMAirBomb3;
	bool m_bDCMAirBomb4;
	bool m_bDCMAirBomb5;
public:
	// Dale - AB: Bombing END
	// Dale - FE: Fighters START
	bool getDCMFighterEngage() const;
protected:
	bool m_bDCMFighterEngage;
public:
	// Dale - FE: Fighters END
/************************************************************************************************/
/* DCM									 END												  */
/************************************************************************************************/
/************************************************************************************************/
/* REVOLUTION_MOD								 01/01/08						DPII		  */
/*																							  */
/* CoreComponent																				*/
/************************************************************************************************/
	bool isCanBeRebel() const;
	bool isCanRebelCapture() const;
	bool isCannotDefect() const;
	bool isCanQuellRebellion() const;
/************************************************************************************************/
/* REVOLUTION_MOD						  END												  */
/************************************************************************************************/

	float getUnitMaxSpeed() const;					// Exposed to Python
	float getUnitPadTime() const;					// Exposed to Python

// BUG - Unit Experience - start
	bool canAcquireExperience() const;				// Exposed to Python
// BUG - Unit Experience - end

	// Arrays

	int getPrereqAndTechs(int i) const;				// Exposed to Python
	int getPrereqOrBonuses(int i) const;			// Exposed to Python
/*	int getProductionTraits(int i) const;*/			// Exposed to Python
	int getFlavorValue(int i) const;				// Exposed to Python
	int getTerrainAttackModifier(int i) const;		// Exposed to Python
	int getTerrainDefenseModifier(int i) const;		// Exposed to Python
	int getFeatureAttackModifier(int i) const;		// Exposed to Python
	int getFeatureDefenseModifier(int i) const;		// Exposed to Python
	int getUnitClassAttackModifier(int i) const;	// Exposed to Python
	int getUnitClassDefenseModifier(int i) const;	// Exposed to Python
	int getUnitCombatModifier(int i) const;			// Exposed to Python
	int getUnitCombatCollateralImmune(int i) const;	// Exposed to Python
	int getDomainModifier(int i) const;				// Exposed to Python
	int getBonusProductionModifier(int i) const;	// Exposed to Python
	int getUnitGroupRequired(int i) const;			// Exposed to Python
	int getReligionSpreads(int i) const;			// Exposed to Python
	int getCorporationSpreads(int i) const;			// Exposed to Python
	int getTerrainPassableTech(int i) const;		// Exposed to Python
	int getFeaturePassableTech(int i) const;		// Exposed to Python
	int getFlankingStrikeUnitClass(int i) const;	// Exposed to Python

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain 																*/
/********************************************************************************/
	bool isPrereqOrCivics(int i) const;				// Exposed to Python
	bool isPrereqBuildingClass(int i) const; 		// Exposed to Python
	int getPrereqBuildingClassOverrideTech(int i) const;	//Exposed to Python
	int getPrereqBuildingClassOverrideEra(int i) const; 	//Exposed to Python
	bool getForceObsoleteUnitClass(int i) const; 			//Exposed to Python
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	bool getUpgradeUnitClass(int i) const;			// Exposed to Python
	bool getTargetUnitClass(int i) const;			// Exposed to Python
	bool getTargetUnitCombat(int i) const;			// Exposed to Python
	bool getDefenderUnitClass(int i) const;			// Exposed to Python
	bool getDefenderUnitCombat(int i) const;		// Exposed to Python
	bool getUnitAIType(int i) const;				// Exposed to Python
	bool getNotUnitAIType(int i) const;				// Exposed to Python
	bool getBuilds(int i) const;					// Exposed to Python
	bool getGreatPeoples(int i) const;				// Exposed to Python

	int getBuildings(int i) const;					// Exposed to Python
	bool getHasBuilding(int i) const;				// Exposed to Python
	int getNumBuildings() const;

	//bool getTerrainImpassable(int i) const;		// Exposed to Python
	//bool getFeatureImpassable(int i) const;		// Exposed to Python
	bool getTerrainNative(int i) const;				// Exposed to Python
	bool getFeatureNative(int i) const;				// Exposed to Python
	bool getFreePromotions(int i) const;			// Exposed to Python
	int getLeaderPromotion() const;					// Exposed to Python
	int getLeaderExperience() const;				// Exposed to Python

	CvOutcomeList* getKillOutcomeList();
	int getNumActionOutcomes();
	CvOutcomeList* getActionOutcomeList(int index);
	MissionTypes getActionOutcomeMission(int index);
	CvOutcomeList* getActionOutcomeListByMission(MissionTypes eMission);
	CvOutcomeMission* getOutcomeMission(int index);
	CvOutcomeMission* getOutcomeMissionByMission(MissionTypes eMission);

	const TCHAR* getEarlyArtDefineTag(int i, UnitArtStyleTypes eStyle) const;	// Exposed to Python
	void setEarlyArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getLateArtDefineTag(int i, UnitArtStyleTypes eStyle) const;	// Exposed to Python
	void setLateArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getMiddleArtDefineTag(int i, UnitArtStyleTypes eStyle) const;	// Exposed to Python
	void setMiddleArtDefineTag(int i, const TCHAR* szVal);
	const TCHAR* getUnitNames(int i) const;
	const TCHAR* getFormationType() const;

	const TCHAR* getButton() const;
	void updateArtDefineButton();

	const CvArtInfoUnit* getArtInfo(int i, EraTypes eEra, UnitArtStyleTypes eStyle) const;

	void read(FDataStreamBase* ) {}
	void write(FDataStreamBase* ) {}

	bool read(CvXMLLoadUtility* pXML);

	void copyNonDefaults(CvUnitInfo* pClassInfo = NULL, CvXMLLoadUtility* pXML = NULL);
	std::vector<int>& getSubCombatTypes();
	//----------------------PROTECTED MEMBER VARIABLES----------------------------

protected:

	int m_iAIWeight;
	int m_iProductionCost;
	int m_iHurryCostModifier;
	int m_iAdvancedStartCost;
	int m_iAdvancedStartCostIncrease;
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
	int m_iSMSpecialCargo;
	int m_iSMNotSpecialCargo;
	int m_iDomainCargo;
	int m_iCargoSpace;
	int m_iSMCargoSpace;
	int m_iSMCargoVolume;
	int m_iConscriptionValue;
	int m_iCultureGarrisonValue;
	int m_iExtraCost;
	int m_iAssetValue;
	int m_iPowerValue;
	int m_iUnitClassType;
	int m_iSpecialUnitType;
	int m_iUnitCaptureClassType;
	int m_iUnitCombatType;
	int m_iDomainType;
	int m_iDefaultUnitAIType;
	int m_iInvisibleType;
	int m_iAdvisorType;

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain 																*/
/********************************************************************************/
	int m_iMaxStartEra;
	int m_iForceObsoleteTech;
	bool m_bStateReligion;
	int m_iPrereqGameOption;
	int m_iNotGameOption;
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	int m_iHolyCity;
	int m_iReligionType;
	int m_iStateReligion;
	int m_iPrereqReligion;
	int m_iPrereqCorporation;
	int m_iPrereqBuilding;
	int m_iPrereqAndTech;
	int m_iPrereqAndBonus;
	std::vector<int> m_aePrereqOrBuildings;
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
	bool m_bQuickCombat;
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
	bool m_bPrereqBonuses;
	bool m_bPrereqReligion;
	bool m_bMechanized;
	bool m_bRenderBelowWater;
	bool m_bRenderAlways;
	bool m_bSuicide;
	bool m_bLineOfSight;
	bool m_bHiddenNationality;
	bool m_bAlwaysHostile;
/*****************************************************************************************************/
/**  Author: TheLadiesOgre																		  **/
/**  Date: 21.09.2009																			   **/
/**  ModComp: TLOTags																			   **/
/**  Reason Added: New Tag Definition															   **/
/**  Notes:																						 **/
/*****************************************************************************************************/
	bool m_bFreeDrop;
/*****************************************************************************************************/
/**  TheLadiesOgre; 21.09.2009; TLOTags															 **/
/*****************************************************************************************************/
	bool m_bNoRevealMap;
/************************************************************************************************/
/* REVOLUTION_MOD								 01/01/08						DPII		  */
/*																							  */
/* CoreComponent																				*/
/************************************************************************************************/
	bool m_bInquisitor;
	bool m_bCanBeRebel;
	bool m_bCanRebelCapture;
	bool m_bCannotDefect;
	bool m_bCanQuellRebellion;
/************************************************************************************************/
/* REVOLUTION_MOD						  END												  */
/************************************************************************************************/

	//ls612: Can't enter non-Owned cities
	bool m_bNoNonOwnedEntry;

	int m_iLeaderPromotion;

	float m_fUnitMaxSpeed;
	float m_fUnitPadTime;

	// Arrays

	int* m_piPrereqAndTechs;
	int* m_piPrereqOrBonuses;
	//int* m_piProductionTraits;
	int* m_piFlavorValue;
	int* m_piTerrainAttackModifier;
	int* m_piTerrainDefenseModifier;
	int* m_piFeatureAttackModifier;
	int* m_piFeatureDefenseModifier;
	int* m_piUnitClassAttackModifier;
	int* m_piUnitClassDefenseModifier;
	int* m_piUnitCombatModifier;
	int* m_piUnitCombatCollateralImmune;
	int* m_piDomainModifier;
	int* m_piBonusProductionModifier;
	int* m_piUnitGroupRequired;
	int* m_piReligionSpreads;
	int* m_piCorporationSpreads;
	int* m_piTerrainPassableTech;
	int* m_piFeaturePassableTech;
	int* m_piFlankingStrikeUnitClass;

/********************************************************************************/
/**		REVDCM									2/16/10				phungus420	*/
/**																				*/
/**		CanTrain 																*/
/********************************************************************************/
	bool* m_pbPrereqOrCivics;
	bool* m_pbPrereqBuildingClass;
	int* m_piPrereqBuildingClassOverrideTech;
	int* m_piPrereqBuildingClassOverrideEra;
	bool* m_pbForceObsoleteUnitClass;
/********************************************************************************/
/**		REVDCM									END								*/
/********************************************************************************/

	bool* m_pbUpgradeUnitClass;
	bool* m_pbTargetUnitClass;
	bool* m_pbTargetUnitCombat;
	bool* m_pbDefenderUnitClass;
	bool* m_pbDefenderUnitCombat;
	bool* m_pbUnitAIType;
	bool* m_pbNotUnitAIType;
	bool* m_pbBuilds;
	bool* m_pbGreatPeoples;
	std::vector<int> m_pbBuildings;
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
	std::vector<CvOutcomeMission*> m_aOutcomeMissions;

	CvPropertyManipulators m_PropertyManipulators;

	//TB Combat Mods Start  TB SubCombat Mod begin
	//integers
	int m_iAttackCombatModifier;
	int m_iDefenseCombatModifier;
	int m_iPursuit;
	int m_iEarlyWithdraw;
	int m_iVSBarbs;
	int m_iArmor;
	int m_iPuncture;
	int m_iOverrun;
	int m_iRepel;
	int m_iFortRepel;
	int m_iRepelRetries;
	int m_iUnyielding;
	int m_iKnockback;
	int m_iKnockbackRetries;
	int m_iStrAdjperRnd;
	int m_iStrAdjperAtt;
	int m_iStrAdjperDef;
	int m_iWithdrawAdjperAtt;
	int m_iUnnerve;
	int m_iEnclose;
	int m_iLunge;
	int m_iDynamicDefense;
	int m_iFortitude;
	int m_iFrontSupportPercent;
	int m_iShortRangeSupportPercent;
	int m_iMediumRangeSupportPercent;
	int m_iLongRangeSupportPercent;
	int m_iFlankSupportPercent;
	int m_iDodgeModifier;
	int m_iPrecisionModifier;
	int m_iPowerShots;
	int m_iPowerShotCombatModifier;
	int m_iPowerShotPunctureModifier;
	int m_iPowerShotPrecisionModifier;
	int m_iPowerShotCriticalModifier;
	int m_iCriticalModifier;
	int m_iEndurance;
	int m_iRoundStunProb;
	int m_iPoisonProbabilityModifier;
	//Team Project (3)
	int m_iCaptureProbabilityModifier;
	int m_iCaptureResistanceModifier;
	//Team Project (4)
	//WorkRateMod
	int m_iHillsWorkModifier;
	int m_iPeaksWorkModifier;

	int m_iBreakdownChance;
	int m_iBreakdownDamage;
	int m_iTaunt;
	int m_iMaxHP;
	int m_iDamageModifier;
	int m_iCostModifier;
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
	int m_iBaseSMRankTotal;
	int m_iBaseSMVolumetricRankTotal;
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
	bool m_bDealsColdDamage;
	bool m_bColdImmune;
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
	//boolean vectors without delayed resolution
	std::vector<int> m_aiSubCombatTypes;
	std::vector<int> m_aiCureAfflictionTypes;
	std::vector<int> m_aiHealAsTypes;
	std::vector<int> m_aiTerrainImpassableTypes;
	std::vector<int> m_aiFeatureImpassableTypes;
	std::vector<int> m_aiMapCategoryTypes;
	std::vector<int> m_aiTrapSetWithPromotionTypes;
	std::vector<int> m_aiTrapImmunityUnitCombatTypes;
	// int vectors utilizing struct with delayed resolution
	std::vector<PromotionLineModifier> m_aAfflictionFortitudeModifiers;
	std::vector<AfflictOnAttack> m_aAfflictOnAttackTypes;
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
	std::vector<AfflictionLineChanges> m_aDistanceAttackCommunicabilityTypeChanges;
	std::vector<EnabledCivilizations> m_aEnabledCivilizationTypes;
	// int vectors utilizing pairing without delayed resolution
	UnitCombatModifierArray m_aFlankingStrengthbyUnitCombatType;
	TerrainModifierArray m_aWithdrawOnTerrainTypes;
	FeatureModifierArray m_aWithdrawOnFeatureTypes;
	UnitCombatModifierArray m_aWithdrawVSUnitCombatTypes;
	UnitCombatModifierArray m_aPursuitVSUnitCombatTypes;
	UnitCombatModifierArray m_aRepelVSUnitCombatTypes;
	UnitCombatModifierArray m_aKnockbackVSUnitCombatTypes;
	UnitCombatModifierArray m_aPunctureVSUnitCombatTypes;
	UnitCombatModifierArray m_aArmorVSUnitCombatTypes;
	UnitCombatModifierArray m_aDodgeVSUnitCombatTypes;
	UnitCombatModifierArray m_aPrecisionVSUnitCombatTypes;
	UnitCombatModifierArray m_aCriticalVSUnitCombatTypes;
	UnitCombatModifierArray m_aRoundStunVSUnitCombatTypes;
	UnitCombatModifierArray m_aTrapDisableUnitCombatTypes;
	UnitCombatModifierArray m_aTrapAvoidanceUnitCombatTypes;
	UnitCombatModifierArray m_aTrapTriggerUnitCombatTypes;
	InvisibilityArray m_aVisibilityIntensityTypes;
	InvisibilityArray m_aInvisibilityIntensityTypes;
	InvisibilityArray m_aVisibilityIntensityRangeTypes;
	//Team Project (4)
		//WorkRateMod
	TerrainModifierArray m_aTerrainWorkRateModifierTypes;
	FeatureModifierArray m_aFeatureWorkRateModifierTypes;
	BuildModifierArray m_aBuildWorkRateModifierTypes;
	AidArray m_aAidChanges;
	//TB Combat Mods End  TB SubCombat Mod end
	//Pediahelp
	std::vector<int> m_aiQualifiedPromotionTypes;
};

#endif
