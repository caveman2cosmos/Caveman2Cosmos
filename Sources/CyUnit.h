#pragma once

#ifndef CyUnit_h__
#define CyUnit_h__

class CyArea;
class CyPlot;
class CyCity;
class CvUnit;
class CySelectionGroup;
class CvArtInfoUnit;

//
// Python wrapper class for CvUnit
//
class CyUnit
{
public:
	CyUnit();
	DllExport explicit CyUnit(CvUnit* pUnit);		// Call from C++

	// < M.A.D. Nukes Start >
	bool isMADEnabled();
	CyPlot* getMADTargetPlot();
	int getMADTargetPlotX();
	int getMADTargetPlotY();
	PlayerTypes getMADTargetPlotOwner();
	// < M.A.D. Nukes End   >

	CvUnit* getUnit() { return m_pUnit;	};	// Call from C++
	const CvUnit* getUnit() const { return m_pUnit;	};	// Call from C++
	bool isNone() { return (m_pUnit==NULL); }
	void convert(CyUnit* pUnit, const bool bKillOriginal);
	void kill(bool bDelay, int /*PlayerTypes*/ ePlayer);

	void NotifyEntity(int /*MissionTypes*/ eMission);

	bool isActionRecommended(int i);
	bool isBetterDefenderThan(CyUnit* pDefender, CyUnit* pAttacker);

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible);
	void doCommand(CommandTypes eCommand, int iData1, int iData2);

	CyPlot* getPathEndTurnPlot();
	bool generatePath(CyPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL);

	bool canEnterTerritory(int /*TeamTypes*/ eTeam, bool bIgnoreRightOfPassage);
	bool canEnterArea(int /*TeamTypes*/ eTeam, CyArea* pArea, bool bIgnoreRightOfPassage);
	int /*TeamTypes*/ getDeclareWarMove(CyPlot* pPlot);
	bool canMoveInto(CyPlot* pPlot, bool bAttack, bool bDeclareWar, bool bIgnoreLoad);
	bool canMoveOrAttackInto(CyPlot* pPlot, bool bDeclareWar);
	bool canMoveThrough(CyPlot* pPlot);
	bool jumpToNearestValidPlot();

	bool canAutomate(AutomateTypes eAutomate);
	bool isAutoUpgrading();
	bool isAutoPromoting();

	bool canScrap();
	bool canGift(bool bTestVisible);
	void gift();

	bool canLoadUnit(CyUnit* pUnit, CyPlot* pPlot);
	bool canLoad(CyPlot* pPlot);
	bool canUnload();
	bool canUnloadAll();
	bool canHold(CyPlot* pPlot);
	bool canSleep(CyPlot* pPlot);
	bool canFortify(CyPlot* pPlot);
	bool canEstablish(CyPlot* pPlot);
	bool canEscape(CyPlot* pPlot);
	bool canPlunder(CyPlot* pPlot);
	bool canAirPatrol(CyPlot* pPlot);
	bool canSeaPatrol(CyPlot* pPlot);
	bool canHeal(CyPlot* pPlot);
	bool canSentry(CyPlot* pPlot);

	bool canAirlift(CyPlot* pPlot);
	bool canAirliftAt(CyPlot* pPlot, int iX, int iY);

	bool isNukeVictim(CyPlot* pPlot, int /*TeamTypes*/ eTeam);
	bool canNuke(CyPlot* pPlot);
	bool canNukeAt(CyPlot* pPlot, int iX, int iY);

	bool canRecon(CyPlot* pPlot);
	bool canReconAt(CyPlot* pPlot, int iX, int iY);

	bool canParadrop(CyPlot* pPlot);
	bool canParadropAt(CyPlot* pPlot, int iX, int iY);

	bool canAirBomb(CyPlot* pPlot);
	bool canAirBombAt(CyPlot* pPlot, int iX, int iY);

	CyCity* bombardTarget(CyPlot* pPlot);
	bool canBombard(CyPlot* pPlot);
	bool canPillage(CyPlot* pPlot);

	//TSHEEP Assassin Mission
	bool canAssassin(CyPlot* pPlot, bool bTestVisible);
	//RevolutionDCM start
	bool canBribe(CyPlot* pPlot, bool bTestVisible);
	//RevolutionDCM end
	//int assassinCost(CyUnit* pUnit);
	//int assassinProb(CyUnit* pUnit);
	//TSHEEP End

	int sabotageCost(CyPlot* pPlot);
	int sabotageProb(CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle);
	bool canSabotage(CyPlot* pPlot, bool bTestVisible);

	int destroyCost(CyPlot* pPlot);
	int destroyProb(CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle);
	bool canDestroy(CyPlot* pPlot, bool bTestVisible);

	int stealPlansCost( CyPlot* pPlot);
	int stealPlansProb( CyPlot* pPlot, int /*ProbabilityTypes*/ eProbStyle);
	bool canStealPlans( CyPlot* pPlot, bool bTestVisible);

	bool IsSelected( void );

	bool canFound(CyPlot* pPlot, bool bTestVisible);
	bool canSpread(CyPlot* pPlot, int /*ReligionTypes*/ eReligion, bool bTestVisible);
	bool canJoin(CyPlot* pPlot, int /*SpecialistTypes*/ eSpecialist);
	bool canConstruct(CyPlot* pPlot, int /*BuildingTypes*/ eBuilding);

	int /*TechTypes*/ getDiscoveryTech();
	int getDiscoverResearch(int /*TechTypes*/ eTech);
	bool canDiscover();
	int getMaxHurryProduction(CyCity* pCity);
	int getHurryProduction(CyPlot* pPlot);
	bool canHurry(CyPlot* pPlot, bool bTestVisible);
	int getTradeGold(CyPlot* pPlot);
	bool canTrade(CyPlot* pPlot, bool bTestVisible);
	int getGreatWorkCulture(CyPlot* pPlot);
	bool canGreatWork(CyPlot* pPlot);
	int getEspionagePoints(CyPlot* pPlot);
	bool canInfiltrate(CyPlot* pPlot, bool bTestVisible);
	bool canEspionage(CyPlot* pPlot);

	bool canGoldenAge(CyPlot* pPlot, bool bTestVisible);
	bool canBuild(CyPlot* pPlot, int /*BuildTypes*/ eBuild, bool bTestVisible);
	int canLead(CyPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);
	int canGiveExperience(CyPlot* pPlot) const;
	bool giveExperience();

	bool canPromote(int /*PromotionTypes*/ ePromotion, int iLeaderUnitId);
	void promote(int /*PromotionTypes*/ ePromotion, int iLeaderUnitId);

	int upgradePrice(int /*UnitTypes*/ eUnit);
	bool upgradeAvailable(int /*UnitTypes*/ eFromUnit, int /*UnitTypes*/ eToUnit, int iCount);
	bool canUpgrade(int /*UnitTypes*/ eUnit, bool bTestVisible);
	bool hasUpgrade(bool bSearch);

	int /*HandicapTypes*/ getHandicapType();
	int /*CivilizationTypes*/ getCivilizationType();
	int /*SpecialUnitTypes*/ getSpecialUnitType();
	int /*UnitTypes*/ getCaptureUnitType();
	int /*UnitCombatTypes*/ getUnitCombatType();
	int /*DomainTypes*/ getDomainType();
	int /*InvisibleTypes*/ getInvisibleType();
	int getNumSeeInvisibleTypes();
	int /*InvisibleTypes*/ getSeeInvisibleType(int i);

	int flavorValue(int /*FlavorTypes*/ eFlavor);
	bool isBarbarian();
	bool isNPC();
	bool isHominid();
	bool isHuman();
	int visibilityRange();
	int baseMoves();

	int maxMoves();
	int movesLeft();

	bool canMove();
	bool hasMoved();
	int airRange();
	int nukeRange();

	bool canBuildRoute();
	int /*BuildTypes*/ getBuildType();
	int workRate(bool bMax);

	bool isAnimal();
	bool isNoBadGoodies();
	bool isOnlyDefensive();

	bool isNoCapture();
	bool isRivalTerritory();
	bool isMilitaryHappiness();
	bool isInvestigate();
	bool isCounterSpy();
	bool isFound();
/*
	bool isCanBeRebel();
	bool isCanRebelCapture();
	bool isCannotDefect();
	bool isCanQuellRebellion();
*/
	bool isGoldenAge();
	bool canCoexistWithEnemyUnit(int /*TeamTypes*/ eTeam);

	bool isFighting();
	bool isAttacking();
	bool isDefending();
	bool isCombat();

	int maxHitPoints();
	int currHitPoints();
	bool isHurt();
	bool isDead();
	void setBaseCombatStr(int iCombat);
	int baseCombatStr();
	float fbaseCombatStr();
	int maxCombatStr(CyPlot* pPlot, CyUnit* pAttacker);
	int currCombatStr(CyPlot* pPlot, CyUnit* pAttacker);
	int currFirepower(CyPlot* pPlot, CyUnit* pAttacker);
	float maxCombatStrFloat(CyPlot* pPlot, CyUnit* pAttacker);
	float currCombatStrFloat(CyPlot* pPlot, CyUnit* pAttacker);

	bool canFight();
	bool canAttack();
	bool canDefend(CyPlot* pPlot);
	bool canSiege(int /*TeamTypes*/ eTeam);

	int airBaseCombatStr();
	float fairBaseCombatStr();
	int airMaxCombatStr(CyUnit* pOther);
	int airCurrCombatStr(CyUnit* pOther);
	float airMaxCombatStrFloat(CyUnit* pOther);
	float airCurrCombatStrFloat(CyUnit* pOther);
	int combatLimit();
	int airCombatLimit();
	bool canAirAttack();
	bool canAirDefend(CyPlot* pPlot);
	int airCombatDamage( CyUnit* pDefender);
	CyUnit* bestInterceptor( CyPlot* pPlot);

	bool isAutomated();
	bool isWaiting();
	bool isFortifyable();
	bool isEstablishable();
	bool isEscapable();
	int fortifyModifier();
	//TB Combat Mods begin
	int fortifyRepelModifier();
	//TB Combat Mods end
	int experienceNeeded();
	int attackXPValue();
	int defenseXPValue();
	int firstStrikes();
	int chanceFirstStrikes();
	int maxFirstStrikes();
	bool isRanged();
	bool alwaysInvisible();
	bool immuneToFirstStrikes();
	bool noDefensiveBonus();
	bool ignoreBuildingDefense();
	bool canMoveImpassable();
	bool canMoveAllTerrain();
	bool flatMovementCost();
	bool ignoreTerrainCost();
	bool isNeverInvisible();
	bool isInvisible(int /*TeamTypes*/ eTeam, bool bDebug);
	bool isNukeImmune();
	//RevolutionDCM Inquisitor
	bool isInquisitor();
	//RevolutionDCM end

	int maxInterceptionProbability();
	int currInterceptionProbability();
	int evasionProbability();
	int withdrawalProbability();
	//TB Combat Mods Begin
	int attackCombatModifierTotal();
	int defenseCombatModifierTotal();
	int pursuitProbability();
	int earlyWithdrawTotal();
	int vsBarbsModifier();
	int armorTotal();
	int punctureTotal();
	int damageModifierTotal();
	int overrunTotal();
	int repelTotal();
	int fortRepelTotal();
	int repelRetriesTotal();
	int unyieldingTotal();
	int knockbackTotal();
	int knockbackRetriesTotal();
	bool canStampede();
	int strAdjperRndTotal();
	int strAdjperAttTotal();
	int strAdjperDefTotal();
	int withdrawAdjperAttTotal();
	int currentStrAdjperRndTotal();
	int currentStrAdjperAttTotal();
	int currentStrAdjperDefTotal();
	int currentWithdrawAdjperAttTotal();
	int unnerveTotal();
	int encloseTotal();
	int lungeTotal();
	int dynamicDefenseTotal();
	bool canAnimalIgnoresBorders();
	bool canOnslaught();
	bool hasSubCombatType(int /*UnitCombatTypes*/ eCombatType);
	bool hasCureAfflictionType(int /*PromotionLineTypes*/ ePromotionLineType);
	int fortitudeTotal();
	//int aidTotal();
	//TB Combat Mods End
	int collateralDamage();
	int collateralDamageLimit();
	int collateralDamageMaxUnits();

	int cityAttackModifier();
	int cityDefenseModifier();
	int animalCombatModifier();
	int hillsAttackModifier();
	int hillsDefenseModifier();
	int terrainAttackModifier(int /*TerrainTypes*/ eTerrain);
	int terrainDefenseModifier(int /*TerrainTypes*/ eTerrain);
	int featureAttackModifier(int /*FeatureTypes*/ eFeature);
	int featureDefenseModifier(int /*FeatureTypes*/ eFeature);
	int unitAttackModifier(int /*UnitTypes*/ eUnit);
	int unitDefenseModifier(int /*UnitTypes*/ eUnit);
	int unitCombatModifier(int /*UnitCombatTypes*/ eUnitCombat);
	int domainModifier(int /*DomainTypes*/ eDomain);

	int bombardRate();
	int airBombBaseRate();
	int airBombCurrRate();

	int /*SpecialUnitTypes*/ specialCargo();
	int /*DomainTypes*/ domainCargo();
	int cargoSpace();
	void changeCargoSpace(int iChange);
	bool isFull();
	int cargoSpaceAvailable(int /*SpecialUnitTypes*/ eSpecialCargo, int /*DomainTypes*/ eDomainCargo);
	bool hasCargo();
	bool canCargoAllMove();
	int getUnitAICargo(UnitAITypes eUnitAI);
	int getID();

	int getGroupID();
	bool isInGroup();
	bool isGroupHead();
	CySelectionGroup* getGroup();

	int getHotKeyNumber();
	void setHotKeyNumber(int iNewValue);

	int getX();
	int getY();
	void setXY(int iX, int iY, bool bGroup, bool bUpdate, bool bShow);
	bool at(int iX, int iY);
	bool atPlot(CyPlot* pPlot);
	CyPlot* plot();
	CyArea* area();
	CyPlot* getReconPlot();
	void setReconPlot(CyPlot* pNewValue);

	int getGameTurnCreated();

	int getDamage();
	void setDamage(int iNewValue, int /*PlayerTypes*/ ePlayer);
	void changeDamage(int iChange, int /*PlayerTypes*/ ePlayer);
	int getMoves();
	void setMoves(int iNewValue);
	void changeMoves(int iChange);
	void finishMoves();
	int getExperience();
	void setExperience(int iNewValue, int iMax);
	void changeExperience(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal);
	int getLevel();
	void setLevel(int iNewLevel);
	void changeLevel(int iChange);
	int getFacingDirection();
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();
	int getCargo();
	int getFortifyTurns();
	void setFortifyTurns(int iNewValue);

	int getBlitzCount();
	bool isBlitz();
	int getAmphibCount();
	bool isAmphib();
	int getRiverCount();
	bool isRiver();
	bool isEnemyRoute();
	bool isAlwaysHeal();
	bool isHillsDoubleMove();

	int getExtraVisibilityRange();
	int getExtraMoves();
	int getExtraMoveDiscount();
	int getExtraAirRange();
	int getExtraIntercept();
	int getExtraEvasion();
	int getExtraFirstStrikes();
	int getExtraChanceFirstStrikes();
	int getExtraWithdrawal();
	//TB Combat Mods begin
	int getExtraPursuit();
	int getExtraEarlyWithdraw();
	int getExtraVSBarbs();
	int getExtraArmor();
	int getExtraPuncture();
	int getExtraDamageModifier();
	int getExtraOverrun();
	int getExtraRepel();
	int getExtraFortRepel();
	int getExtraRepelRetries();
	int getExtraUnyielding();
	int getExtraKnockback();
	int getExtraKnockbackRetries();
	bool mayStampede();
	int getExtraStrAdjperRnd();
	int getExtraStrAdjperAtt();
	int getExtraStrAdjperDef();
	int getExtraWithdrawAdjperAtt();
	int getExtraUnnerve();
	int getExtraEnclose();
	int getExtraLunge();
	int getExtraDynamicDefense();
	int getExtraStrength();
	bool mayAnimalIgnoresBorders();
	bool mayOnslaught();
	bool hasExtraSubCombatType(int /*UnitCombatTypes*/ eCombatType);
	bool hasRemovesUnitCombatType(int /*UnitCombatTypes*/ eCombatType);
	bool hasExtraCureAffliction(int /*PromotionLineTypes*/ ePromotionLineType);
	int getExtraFortitude();
	//TB Combat Mods end
	int getExtraCollateralDamage();
	int getExtraEnemyHeal();
	int getExtraNeutralHeal();
	int getExtraFriendlyHeal();

	int getSameTileHeal();
	int getAdjacentTileHeal();

	int getExtraCombatPercent();
	int getExtraCityAttackPercent();
	int getExtraCityDefensePercent();
	int getExtraHillsAttackPercent();
	int getExtraHillsDefensePercent();
	int getRevoltProtection() const;
	int getCollateralDamageProtection() const;
	int getPillageChange() const;
	int getUpgradeDiscount() const;
	int getExperiencePercent() const;
	int getKamikazePercent() const;

	int getImmobileTimer() const;
	void setImmobileTimer(int iNewValue);

	bool isMadeAttack();
	void setMadeAttack(bool bNewValue);
	//TB Combat Mod end
	int getRoundCount () const;
	void changeRoundCount (int iChange);
	int getAttackCount() const;
	void changeAttackCount(int iChange);
	int getDefenseCount() const;
	void changeDefenseCount(int iChange);
	//TB Combat Mod end
	bool isMadeInterception();
	void setMadeInterception(bool bNewValue);

	bool isPromotionReady();
	void setPromotionReady(bool bNewValue);
	int getOwner();
	int getVisualOwner();
	int getCombatOwner(int /* TeamTypes*/ iForTeam);
	int getTeam();

	int /*UnitTypes*/ getUnitType();
	int /*UnitTypes*/ getLeaderUnitType();
	void setLeaderUnitType(int /*UnitTypes*/ leaderUnitType);

	CyUnit* getTransportUnit() const;
	bool isCargo();
	void setTransportUnit(CyUnit* pTransportUnit);

	int getExtraDomainModifier(int /*DomainTypes*/ eIndex);

	std::wstring getName();
	std::wstring getNameForm(int iForm);
	std::wstring getNameKey();
	std::wstring getNameNoDesc();
	void setName(std::wstring szNewValue);
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);
	bool isTerrainDoubleMove(int /*TerrainTypes*/ eIndex);
	bool isFeatureDoubleMove(int /*FeatureTypes*/ eIndex);

	int getExtraTerrainAttackPercent(int /*TerrainTypes*/ eIndex);
	int getExtraTerrainDefensePercent(int /*TerrainTypes*/ eIndex);
	int getExtraFeatureAttackPercent(int /*FeatureTypes*/ eIndex);
	int getExtraFeatureDefensePercent(int /*FeatureTypes*/ eIndex);
	int getExtraUnitCombatModifier(int /*UnitCombatTypes*/ eIndex);
	bool canAcquirePromotion(int /*PromotionTypes*/ ePromotion);
	bool canAcquirePromotionAny();
	bool isPromotionValid(int /*PromotionTypes*/ ePromotion);
	bool isPromotionOverriden(int /*PromotionTypes*/ ePromotion);
	bool isHasPromotion(int /*PromotionTypes*/ ePromotion);
	bool isHasUnitCombat(int /*UnitCombatTypes*/ eUnitCombat);
	void setHasUnitCombat(int /*UnitCombatTypes*/ eIndex, bool bNewValue);
	void setHasPromotion(int /*PromotionTypes*/ eIndex, bool bNewValue);
//Team Project (3)
	//TB Tags
	int captureProbabilityTotal();
	int captureResistanceTotal();
	bool isUnitAtBaseGroup();
	bool isUnitAboveBaseGroup();
	bool isUnitBelowBaseGroup();
	bool isHiddenNationality();
	void doHNCapture();
	CyUnit* getCapturingUnit() const;
	void setCapturingUnit(CyUnit* pCapturingUnit);

	int /*UnitAITypes*/ getUnitAIType();
	void setUnitAIType(int /*UnitAITypes*/ iNewValue);

	const CvArtInfoUnit* getArtInfo(int i, EraTypes eEra) const;
	std::string getButton() const;

	// Python Helper Functions

	void centerCamera();
	void attackForDamage(CyUnit *defender, int attakerDamageChange, int defenderDamageChange);
	void rangeStrike(int iX, int iY);

	bool isCommander() const;
	int controlPointsLeft() const;
	int controlPoints() const;
	int commandRange() const;
	int getExperience100();
	float getRealExperience();
	void setExperience100(int iNewValue, int iMax);
	void changeExperience100(int iChange, int iMax, bool bFromCombat, bool bInBorders, bool bUpdateGlobal);

protected:
	CvUnit* m_pUnit;
};

DECLARE_PY_WRAPPER(CyUnit, CvUnit*);

#endif // CyUnit_h__
