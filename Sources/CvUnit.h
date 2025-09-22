#pragma once

// unit.h

#ifndef CIV4_UNIT_H
#define CIV4_UNIT_H

#include "CvDLLEntity.h"
#include "CvGameObject.h"
#include "CvProperties.h"
#include "CvUnitComponents.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvArea;
class CvCity;
class CvPlot;
class CvSelectionGroup;
class CvArtInfoUnit;
class CvUnitInfo;
class CvUnitSelectionCriteria;
#ifdef USE_OLD_PATH_GENERATOR
class FAStarNode;
#endif

enum UnitValueFlags
{
	UNITVALUE_FLAGS_DEFENSIVE = 1 << 0,
	UNITVALUE_FLAGS_OFFENSIVE = 1 << 1,
	UNITVALUE_FLAGS_UTILITY = 1 << 2,
	UNITVALUE_FLAGS_ALL = UNITVALUE_FLAGS_DEFENSIVE | UNITVALUE_FLAGS_OFFENSIVE | UNITVALUE_FLAGS_UTILITY
};
DECLARE_FLAGS(UnitValueFlags);

struct PromotionRequirements
{
	enum flags
	{
		None = 0,
		IgnoreHas = 1 << 0,
		Equip = 1 << 1,
		Afflict = 1 << 2,
		// TODO: this doesn't appear to actually do anything in the promote functions, so remove it (check the function ofcourse)
		Promote = 1 << 3,
		ForLeader = 1 << 4,
		ForOffset = 1 << 5,
		ForFree = 1 << 6,
		ForBuildUp = 1 << 7,
		ForStatus = 1 << 8
	};
};
DECLARE_FLAGS(PromotionRequirements::flags);

struct PromotionApply
{
	enum flags
	{
		None = 0,
		Free = 1 << 0,
		Dying = 1 << 1,
		Initial = 1 << 2,
		FromTrait = 1 << 3
	};
};
DECLARE_FLAGS(PromotionApply::flags);

struct MoveCheck
{
	enum flags
	{
		None = 0,
		// Checking for a potential attack
		Attack = 1 << 0,
		// Allow declaration of war (human only check)
		DeclareWar = 1 << 1,
		// Don't allow loading into transports
		IgnoreLoad = 1 << 2,
		IgnoreTileLimit = 1 << 3,
		IgnoreLocation = 1 << 4,
		IgnoreAttack = 1 << 5,
		CheckForBest = 1 << 6,
		Assassinate = 1 << 7,
		Suprise = 1 << 8
	};
};
DECLARE_FLAGS(MoveCheck::flags);


/************************************************************************************************/
/* Afforess	                  Start		 02/22/10                                               */
/*                                                                                              */
/* Defender Withdraw                                                                            */
/************************************************************************************************/
struct CombatResult
{
	bool bDefenderWithdrawn;
	//TB Combat Mods Begin
	bool bAttackerPursued;
	bool bDefenderPursued;
	bool bAttackerPursuedSustain;
	bool bDefenderPursuedSustain;
	bool bAttackerRepelled;
	bool bAttackerRefusedtoYield;
	bool bDefenderRefusedtoYield;
	bool bAttackerRefusedtoYieldSustain;
	bool bDefenderRefusedtoYieldSustain;
	bool bDefenderKnockedBack;
	bool bAttackerStampedes;
	bool bAttackerWithdraws;
	bool bAttackerOnslaught;
	bool bAttackerInjured;
	bool bDefenderInjured;
	int iTurnCount;
	bool bDeathMessaged;
	bool bDefenderHitAttackerWithDistanceAttack;
	bool bAttackerHitDefenderWithDistanceAttack;
	bool bNeverMelee;
	//TB Combat Mods End
	CvPlot* pPlot;
	int iAttacksCount;
	bool bStealthDefense;

	CombatResult()
		: bDefenderWithdrawn(false)
		//TB Combat Mods Begin
		, bAttackerPursued(false)
		, bDefenderPursued(false)
		, bAttackerPursuedSustain(false)
		, bDefenderPursuedSustain(false)
		, bAttackerRepelled(false)
		, bAttackerRefusedtoYield(false)
		, bDefenderRefusedtoYield(false)
		, bAttackerRefusedtoYieldSustain(false)
		, bDefenderRefusedtoYieldSustain(false)
		, bDefenderKnockedBack(false)
		, bAttackerStampedes(false)
		, bAttackerWithdraws(false)
		, bAttackerOnslaught(false)
		, bAttackerInjured(false)
		, bDefenderInjured(false)
		, iTurnCount(0)
		, bDeathMessaged(true)
		, bDefenderHitAttackerWithDistanceAttack(false)
		, bAttackerHitDefenderWithDistanceAttack(false)
		, bNeverMelee(true)
		//TB Combat Mods End
		, pPlot(NULL)
		, iAttacksCount(0)
		, bStealthDefense(false)
		{}
};
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
struct CombatDetails
{
	int iExtraCombatPercent;
	int iAnimalCombatModifierTA;
	int iAIAnimalCombatModifierTA;
	int iAnimalCombatModifierAA;
	int iAIAnimalCombatModifierAA;
	int iBarbarianCombatModifierTB;
	int iAIBarbarianCombatModifierTB;
	int iBarbarianCombatModifierAB;
	int iAIBarbarianCombatModifierAB;
	int iPlotDefenseModifier;
	int iFortifyModifier;
	int iCityDefenseModifier;
	int iHillsAttackModifier;
	int iHillsDefenseModifier;
	int iFeatureAttackModifier;
	int iFeatureDefenseModifier;
	int iTerrainAttackModifier;
	int iTerrainDefenseModifier;
	int iCityAttackModifier;
	int iDomainDefenseModifier;
	int iCityBarbarianDefenseModifier;
	int iDefenseModifier;
	int iAttackModifier;
	int iCombatModifierT;
	int iCombatModifierA;
	int iDomainModifierA;
	int iDomainModifierT;
	int iAnimalCombatModifierA;
	int iAnimalCombatModifierT;
	int iRiverAttackModifier;
	int iAmphibAttackModifier;
	int iKamikazeModifier;
	int iModifierTotal;
	int iBaseCombatStr;
	int iCombat;
	int iMaxCombatStr;
	int iCurrHitPoints;
	int iMaxHitPoints;
	int iCurrCombatStr;
	PlayerTypes eOwner;
	PlayerTypes eVisualOwner;
	std::wstring sUnitName;
};

class PromotionKeyedInfo
{
public:
	PromotionKeyedInfo() :	m_bHasPromotion(false),
							m_iAfflictOnAttackCount(0),
							m_iCureAfflictionCount(0),
							m_iAfflictionTurnCount(0),
							m_iAfflictionHitCount(0),
							m_iAfflictionTolerance(0),
							m_iFortitudeModifierTypeAmount(0),
							m_iPromotionFreeCount(0),
							m_iTrapSetWithPromotionCount(0),
							m_iPromotionFromTraitCount(0)
	{
	}

	bool Empty() const
	{
		return (!m_bHasPromotion &&
			m_iAfflictOnAttackCount == 0 &&
			m_iCureAfflictionCount == 0 &&
			m_iAfflictionTurnCount == 0 &&
			m_iAfflictionHitCount == 0 &&
			m_iAfflictionTolerance == 0 &&
			m_iFortitudeModifierTypeAmount == 0 &&
			m_iPromotionFreeCount == 0 &&
			m_iTrapSetWithPromotionCount == 0 &&
			m_iPromotionFromTraitCount == 0);
	}

	//TB Combat Mods Begin
	bool	m_bHasPromotion;
	int		m_iAfflictOnAttackCount;
	int		m_iCureAfflictionCount;
	int		m_iAfflictionTurnCount;
	int		m_iAfflictionHitCount;
	int		m_iAfflictionTolerance;
	int		m_iFortitudeModifierTypeAmount;
	int		m_iPromotionFreeCount;
	int		m_iTrapSetWithPromotionCount;
	int		m_iPromotionFromTraitCount;
};

typedef std::map<PromotionTypes, PromotionKeyedInfo>::iterator PromotionIterator;

class PromotionLineKeyedInfo
{
public:
	PromotionLineKeyedInfo() : m_bValidBuildUp(false)
#ifdef OUTBREAKS_AND_AFFLICTIONS
							 , m_iCureAfflictionTypeCount(0)
							 , m_iAfflictionTurnTypeCount(0)
							 , m_iAfflictionLineCount(0)
							 , m_iAfflictionTypeTolerance(0)
							 , m_iFortitudeModifierAmount(0)
							 , m_iAfflictOnAttackTypeProbability(0)
							 , m_iAfflictOnAttackTypeCount(0)
							 , m_iAfflictOnAttackTypeImmediateCount(0)
							 , m_iAfflictOnAttackTypeAttemptedCount(0)
							 , m_iDistanceAttackCommunicability(0)
							 , m_iAfflictOnAttackTypeMeleeCount(0)
							 , m_iAfflictOnAttackTypeDistanceCount(0)
#endif
	{
	}

	bool Empty() const
	{
		return !m_bValidBuildUp
#ifdef OUTBREAKS_AND_AFFLICTIONS
			&& m_iCureAfflictionTypeCount == 0
			&& m_iAfflictionTurnTypeCount == 0
			&& m_iAfflictionLineCount == 0
			&& m_iAfflictionTypeTolerance == 0
			&& m_iFortitudeModifierAmount == 0
			&& m_iAfflictOnAttackTypeProbability == 0
			&& m_iAfflictOnAttackTypeCount == 0
			&& m_iAfflictOnAttackTypeImmediateCount == 0
			&& m_iAfflictOnAttackTypeAttemptedCount == 0
			&& m_iDistanceAttackCommunicability == 0
			&& m_iAfflictOnAttackTypeMeleeCount == 0
			&& m_iAfflictOnAttackTypeDistanceCount == 0
#endif
		;
	}

	bool	m_bValidBuildUp;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	int		m_iCureAfflictionTypeCount;
	int		m_iAfflictionTurnTypeCount;
	int		m_iAfflictionLineCount;
	int		m_iAfflictionTypeTolerance;
	int		m_iFortitudeModifierAmount;
	int		m_iAfflictOnAttackTypeProbability;
	int		m_iAfflictOnAttackTypeCount;
	int		m_iAfflictOnAttackTypeImmediateCount;
	int		m_iAfflictOnAttackTypeAttemptedCount;
	int		m_iDistanceAttackCommunicability;
	int		m_iAfflictOnAttackTypeMeleeCount;
	int		m_iAfflictOnAttackTypeDistanceCount;
#endif
};

class TerrainKeyedInfo
{
public:
	TerrainKeyedInfo() :
		m_iTerrainProtected(0),
		m_iTerrainDoubleMoveCount(0),
		m_iExtraTerrainAttackPercent(0),
		m_iExtraTerrainDefensePercent(0),
		m_iExtraTerrainWorkPercent(0),
		m_iExtraWithdrawOnTerrainType(0)
	{ }

	bool Empty() const
	{
		return (m_iTerrainProtected == 0 &&
			m_iTerrainDoubleMoveCount == 0 &&
			m_iExtraTerrainAttackPercent == 0 &&
			m_iExtraTerrainDefensePercent == 0 &&
			m_iExtraTerrainWorkPercent == 0 &&
			m_iExtraWithdrawOnTerrainType == 0);
	}

	int	m_iTerrainProtected;
	int	m_iTerrainDoubleMoveCount;
	int	m_iExtraTerrainAttackPercent;
	int	m_iExtraTerrainDefensePercent;
	int m_iExtraTerrainWorkPercent;
	int m_iExtraWithdrawOnTerrainType;
};

class FeatureKeyedInfo
{
public:
	FeatureKeyedInfo() :
		m_iFeatureDoubleMoveCount(0),
		m_iExtraFeatureAttackPercent(0),
		m_iExtraFeatureDefensePercent(0),
		m_iExtraFeatureWorkPercent(0),
		m_iExtraWithdrawOnFeatureType(0)
	{ }

	bool Empty() const
	{
		return (m_iFeatureDoubleMoveCount == 0 &&
			m_iExtraFeatureAttackPercent == 0 &&
			m_iExtraFeatureDefensePercent == 0 &&
			m_iExtraFeatureWorkPercent == 0 &&
			m_iExtraWithdrawOnFeatureType == 0);
	}

	int	m_iFeatureDoubleMoveCount;
	int	m_iExtraFeatureAttackPercent;
	int	m_iExtraFeatureDefensePercent;
	int m_iExtraFeatureWorkPercent;
	int m_iExtraWithdrawOnFeatureType;
};

// @SAVEBREAK Restructure/Rethink - Toffer
class UnitCombatKeyedInfo
{
public:
	UnitCombatKeyedInfo() :		m_bHasUnitCombat(false),
								m_iExtraUnitCombatModifier(0),
								m_iExtraFlankingStrengthbyUnitCombatType(0),
								m_iExtraWithdrawVSUnitCombatType(0),
								m_iExtraPursuitVSUnitCombatType(0),
								m_iExtraRepelVSUnitCombatType(0),
								m_iExtraKnockbackVSUnitCombatType(0),
								m_iExtraPunctureVSUnitCombatType(0),
								m_iExtraArmorVSUnitCombatType(0),
								m_iExtraDodgeVSUnitCombatType(0),
								m_iExtraPrecisionVSUnitCombatType(0),
								m_iExtraCriticalVSUnitCombatType(0),
								m_iExtraRoundStunVSUnitCombatType(0),
								m_iHealUnitCombatTypeVolume(0),
								m_iHealUnitCombatTypeAdjacentVolume(0),
								m_iHealAsDamage(0),
								m_iTrapImmunityUnitCombatCount(0),
								m_iTargetUnitCombatCount(0),
								m_iExtraTrapDisableUnitCombatType(0),
								m_iExtraTrapAvoidanceUnitCombatType(0),
								m_iExtraTrapTriggerUnitCombatType(0)
	{
	}

	bool Empty() const
	{
		return (!m_bHasUnitCombat &&
			m_iExtraUnitCombatModifier == 0 &&
			m_iExtraFlankingStrengthbyUnitCombatType == 0 &&
			m_iExtraWithdrawVSUnitCombatType == 0 &&
			m_iExtraPursuitVSUnitCombatType == 0 &&
			m_iExtraRepelVSUnitCombatType == 0 &&
			m_iExtraKnockbackVSUnitCombatType == 0 &&
			m_iExtraPunctureVSUnitCombatType == 0 &&
			m_iExtraArmorVSUnitCombatType == 0 &&
			m_iExtraDodgeVSUnitCombatType == 0 &&
			m_iExtraPrecisionVSUnitCombatType == 0 &&
			m_iExtraCriticalVSUnitCombatType == 0 &&
			m_iExtraRoundStunVSUnitCombatType == 0 &&
			m_iHealUnitCombatTypeVolume == 0 &&
			m_iHealUnitCombatTypeAdjacentVolume == 0 &&
			m_iHealAsDamage == 0 &&
			m_iTrapImmunityUnitCombatCount == 0 &&
			m_iTargetUnitCombatCount == 0 &&
			m_iExtraTrapDisableUnitCombatType == 0 &&
			m_iExtraTrapAvoidanceUnitCombatType == 0 &&
			m_iExtraTrapTriggerUnitCombatType == 0);
	}

	bool m_bHasUnitCombat;
	int	m_iExtraUnitCombatModifier;
	int m_iExtraFlankingStrengthbyUnitCombatType;
	int m_iExtraWithdrawVSUnitCombatType;
	int m_iExtraPursuitVSUnitCombatType;
	int m_iExtraRepelVSUnitCombatType;
	int m_iExtraKnockbackVSUnitCombatType;
	int m_iExtraPunctureVSUnitCombatType;
	int m_iExtraArmorVSUnitCombatType;
	int m_iExtraDodgeVSUnitCombatType;
	int m_iExtraPrecisionVSUnitCombatType;
	int m_iExtraCriticalVSUnitCombatType;
	int m_iExtraRoundStunVSUnitCombatType;
	int m_iHealUnitCombatTypeVolume;
	int m_iHealUnitCombatTypeAdjacentVolume;
	int m_iHealAsDamage;
	int	m_iTrapImmunityUnitCombatCount;
	int	m_iTargetUnitCombatCount;
	int	m_iExtraTrapDisableUnitCombatType;
	int	m_iExtraTrapAvoidanceUnitCombatType;
	int	m_iExtraTrapTriggerUnitCombatType;
};
// SAVEBREAK@

class CvUnit : public CvDLLEntity
{
public:
	CvUnit(bool bIsDummy = false);
	virtual ~CvUnit();

	CvGameObjectUnit* getGameObject() { return &m_GameObject; }
	const CvGameObjectUnit* getGameObject() const { return &m_GameObject; }

	CvUnit& operator=(const CvUnit& other);

	// Comparison operators
	// Use address identity for now (more than one map means x/y compare wouldn't work)
	friend bool operator==(const CvUnit& lhs, const CvUnit& rhs) { return &lhs == &rhs; }
	friend bool operator!=(const CvUnit& lhs, const CvUnit& rhs) { return &lhs != &rhs; }

	DllExport void NotifyEntity(MissionTypes eMission);
	DllExport PlayerTypes getNationality() const;
	DllExport DomainTypes getDomainType() const;
	DllExport bool canMove() const;
	DllExport bool hasMoved() const;
	DllExport BuildTypes getBuildType() const;
	DllExport bool isFighting() const;
	DllExport bool isAttacking() const;
	DllExport bool isDefending() const;
	DllExport int maxHitPoints() const;
	DllExport bool isDead() const;
	DllExport float maxCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	DllExport float currCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	DllExport bool canFight() const;
	DllExport float airMaxCombatStrFloat(const CvUnit* pOther) const;
	DllExport float airCurrCombatStrFloat(const CvUnit* pOther) const;
	DllExport bool canAirAttack() const;
	DllExport bool isWaiting() const;
	DllExport bool isRanged() const;
	DllExport bool isInvisible(TeamTypes eTeam, bool bDebug = false, bool bCheckCargo = true) const;
	DllExport int getID() const;
	DllExport IDInfo getIDInfo() const;
	DllExport CvSelectionGroup* getGroup() const;
	DllExport int getHotKeyNumber();
	DllExport int getViewportX() const;
	DllExport int getViewportY() const;
	DllExport bool atPlot(const CvPlot* pPlot) const;
	DllExport CvPlot* plotExternal() const;
	DllExport int getDamage() const;
	DllExport int getCombatTimer() const;
	DllExport DirectionTypes getFacingDirection(bool checkLineOfSightProperty) const;
	DllExport bool isSuicide() const;
	DllExport bool isPromotionReady() const;
	DllExport bool isInfoBarDirty() const;
	DllExport void setInfoBarDirty(bool bNewValue);
	DllExport inline PlayerTypes getOwner() const { return m_eOwner; }
	DllExport PlayerTypes getVisualOwner(TeamTypes eForTeam = NO_TEAM) const;
	DllExport TeamTypes getTeam() const;
	DllExport const UnitTypes getUnitType() const;
	DllExport const UnitTypes getLeaderUnitType() const;
	DllExport CvUnit* getCombatUnit() const;
	DllExport const CvWString getName(uint uiForm = 0) const;
	DllExport int getSubUnitsAlive() const;
	DllExport const CvArtInfoUnit* getArtInfo(int i, EraTypes eEra) const;
	DllExport const char* getButton() const;
	DllExport int getGroupSize() const;
	DllExport int getGroupDefinitions() const;
	DllExport int getUnitGroupRequired(int i) const;
	DllExport bool isRenderAlways() const;
	DllExport float getAnimationMaxSpeed() const;
	DllExport float getAnimationPadTime() const;
	DllExport const char* getFormationType() const;
	DllExport bool isMechUnit() const;
	DllExport bool isRenderBelowWater() const;
	DllExport int getRenderPriority(UnitSubEntityTypes eUnitSubEntity, int iMeshGroupType, int UNIT_MAX_SUB_TYPES) const;
	DllExport bool shouldShowEnemyGlow(TeamTypes eForTeam) const;
	DllExport bool shouldShowFoundBorders() const;
	DllExport void cheat(bool bCtrl, bool bAlt, bool bShift);
	DllExport float getHealthBarModifier() const;
	DllExport void getLayerAnimationPaths(std::vector<AnimationPathTypes>& aAnimationPaths) const;
	DllExport int getSelectionSoundScript() const;

protected:
	CvGameObjectUnit m_GameObject;

	// Toffer - UnitComponents
	UnitCompCommander*
		m_commander;
	UnitCompWorker*
		m_worker;
	UnitCompCommodore*
        m_commodore;

public:
	bool isInBattle() const;

	void reloadEntity(bool bForceLoad = false);
	void init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, int iBirthmark);
	void changeIdentity(UnitTypes eUnit);
	void reset(int iID = 0, UnitTypes eUnit = NO_UNIT, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false, bool bIdentityChange = false);
	void setupGraphical();

	void convert(CvUnit* pUnit, const bool bKillOriginal=true);
	void kill(bool bDelay, PlayerTypes ePlayer = NO_PLAYER, bool bMessaged = false);
	void killUnconditional(bool bDelay, PlayerTypes ePlayer, bool bMessaged = false); // Used internally


	void doTurn();

	void updateCombat(CvUnit* pSelectedDefender = NULL, bool bSamePlot = false, bool bStealth = false, bool bNoCache = false);
	void updateAirCombat(bool bQuick = false);
	void updateAirStrike(CvPlot* pPlot, bool bFinish);

	bool isActionRecommended(int iAction) const;

	int defenderValue(const CvUnit* pAttacker) const;
	bool isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker) const;

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bTestBusy = true) const;
	void doCommand(CommandTypes eCommand, int iData1, int iData2);

#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* getPathLastNode() const;
#else
	int getPathMovementRemaining() const;
#endif
	CvPlot* getPathEndTurnPlot() const;
	bool generatePath(const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, int iMaxTurns = MAX_INT, int iOptimizationLimit = -1) const;

	bool canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage = false) const;
	bool canEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage = false) const;
	TeamTypes getDeclareWarMove(const CvPlot* pPlot) const;

	bool canEnterPlot(const CvPlot* pPlot, MoveCheck::flags flags = MoveCheck::None, CvUnit** ppDefender = nullptr) const;

	bool canEnterOrAttackPlot(const CvPlot* pPlot, bool bDeclareWar = false) const;
	bool canMoveThrough(const CvPlot* pPlot, bool bDeclareWar = false) const;
	void attack(CvPlot* pPlot, bool bStealth = false, bool bNoCache = false);
	void fightInterceptor(const CvPlot* pPlot, bool bQuick);
	void move(CvPlot* pPlot, bool bShow);

	bool jumpToNearestValidPlot(bool bKill = true);

	bool canAutomate(AutomateTypes eAutomate) const;
	void automate(AutomateTypes eAutomate);

	bool canScrap() const;
	int calculateScrapValue() const;
	void scrap();

	bool canTradeUnit(PlayerTypes eReceivingPlayer) const;

	void tradeUnit(PlayerTypes eReceivingPlayer);
	void spyNuke(int iX, int iY, bool bCaught);
	bool canClaimTerritory(const CvPlot* pPlot) const;
	bool claimTerritory();
	int getMaxHurryFood() const;
	int getHurryFood(const CvPlot* pPlot) const;
	bool canHurryFood(const CvPlot* pPlot) const;
	bool hurryFood();

	bool canPerformInquisition(const CvPlot* pPlot) const;
	bool performInquisition();
	//SAD
	int surroundedDefenseModifier(const CvPlot *pPlot, const CvUnit *pDefender) const;

	bool isCanMovePeaks() const;
	void changeCanMovePeaksCount(int iChange);
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	bool isCanLeadThroughPeaks() const;
	void changeCanLeadThroughPeaksCount(int iChange);

	void setNationality(PlayerTypes eNewNationality);

	int interceptionChance(const CvPlot* pPlot) const;

	CvUnit* getCommander() const;
	void tryUseCommander();
	bool isCommander() const;
	bool isCommanderReady() const;
	void setCommander(bool bNewVal);
	void nullLastCommander();
	UnitCompCommander* getCommanderComp() const;

	CvUnit* getUsedCommander() const;
	CvUnit* getLastCommander() const;

	CvUnit* getCommodore() const;
    void tryUseCommodore();
    bool isCommodore() const;
    bool isCommodoreReady() const;
    void setCommodore(bool bNewVal);
    void nullLastCommodore();
   	UnitCompCommodore* getCommodoreComp() const;

    CvUnit* getUsedCommodore() const;
    CvUnit* getLastCommodore() const;

	int getZoneOfControlCount() const;
	bool isZoneOfControl() const;
	void changeZoneOfControlCount(int iChange);

	bool sleepForEspionage();
	PlayerTypes getOriginalOwner() const;

	int getExperience100() const;
	void setExperience100(int iNewValue);
	void changeExperience100(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false);

	void doBattleFieldPromotions(CvUnit* pDefender, const CombatDetails& cdDefenderDetails, const CvPlot* pPlot, bool bAttackerHasLostNoHP, bool bAttackerWithdrawn, int iAttackerInitialDamage, int iWinningOdds, int iInitialAttXP, int iInitialAttGGXP, int iDefenderInitialDamage, int iInitialDefXP, int iInitialDefGGXP, bool &bAttackerPromoted, bool &bDefenderPromoted, int iNonLethalAttackWinChance, int iNonLethalDefenseWinChance, int iDefenderFirstStrikes, int iAttackerFirstStrikes);

	void doDynamicXP(CvUnit* pDefender, const CvPlot* pPlot, int iAttackerInitialDamage, int iWinningOdds, int iDefenderInitialDamage, bool bPromotion = false, bool bDefPromotion = false);
	void applyDynamicXP(const int iExperience, const bool bHomeTerritory, int iMaxTotalXP);
	int getVanquishDynamicXP(const int iLoseOdds, const int iInitialDamage, const int iMaxXP) const;
	int getEngagementDynamicXP(const CvUnit* enemy, const int iLoseOdds, const int iInitialDamageEnemy, const int iInitialDamage, const int iMaxXP) const;

	void changeTerrainProtected(TerrainTypes eIndex, int iNewValue);
	bool isTerrainProtected(TerrainTypes eIndex) const;
	int getTerrainProtectedCount(TerrainTypes eIndex) const;

	bool isAutoPromoting() const;
	void setAutoPromoting(bool bNewValue);
	bool isAutoUpgrading() const;
	void setAutoUpgrading(bool bNewValue);

	const CvWString getDescription(uint uiForm = 0) const;

	bool canShadow() const;
	bool canShadowAt(const CvPlot* pShadowPlot, CvUnit* pShadowUnit = NULL) const;

	void setShadowUnit(const CvUnit* pUnit);
	CvUnit* getShadowUnit() const;

	bool canGift(bool bTestVisible = false, bool bTestTransport = true) const;
	void gift(bool bTestTransport = true);

	bool canLoadOntoUnit(const CvUnit* pUnit, const CvPlot* pPlot) const;
	void loadOntoUnit(CvUnit* pUnit);

	bool canLoad(const CvPlot* pPlot) const;
	void load();
	bool shouldLoadOnMove(const CvPlot* pPlot) const;

	bool canUnload() const;
	void unload();

	bool canUnloadAll() const;
	void unloadAll();

	bool canHold() const;
	bool canSleep() const;
	bool canFortify() const;
	bool canBuildUp() const;
	bool canAirPatrol(const CvPlot* pPlot) const;
	void airCircle(bool bStart);

	bool canSeaPatrol(const CvPlot* pPlot) const;

	bool canHeal(const CvPlot* pPlot) const;
	bool canSentry(const CvPlot* pPlot) const;

	int healRate(const CvPlot* pPlot, bool bHealCheck = false) const;
	int getHealRateAsType(const CvPlot* pPlot, bool bHealCheck = false, UnitCombatTypes eHealAsType = NO_UNITCOMBAT) const;
	int healTurns(const CvPlot* pPlot) const;
	int healTurnsAsType(const CvPlot* pPlot, UnitCombatTypes eHealAsType) const;
	void doHeal();
#ifdef OUTBREAKS_AND_AFFLICTIONS
	void doAffliction(const bool bHeal);
#endif

	bool canAirlift(const CvPlot* pPlot) const;
	bool canAirliftAt(const CvPlot* pPlot, int iX, int iY) const;
	bool airlift(int iX, int iY);

	void nukeDiplomacy(bool* nukedTeams);
	bool isNukeVictim(const CvPlot* pPlot, const TeamTypes eTeam, int iRange) const;
	bool canNuke() const;
	bool canNukeAt(const CvPlot* pPlot, int iX, int iY) const;
	bool nuke(int iX, int iY, bool bTrap = false);

	bool canRecon() const;
	bool canReconAt(const CvPlot* pPlot, int iX, int iY) const;
	bool recon(int iX, int iY);

	bool canAirBomb() const;
	bool canAirBombAt(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb(int iX, int iY);

	CvCity* bombardTarget(const CvPlot* pPlot) const;
	// Super Forts begin *bombard*
	CvPlot* bombardImprovementTarget(const CvPlot* pPlot) const;
	// Super Forts end
	bool canBombard(const CvPlot* pPlot, bool bIgnoreHasAttacked = false) const;
	bool bombard();

	bool canParadrop(const CvPlot* pPlot) const;
	bool canParadropAt(const CvPlot* pPlot, int iX, int iY) const;
	bool paradrop(int iX, int iY);

	bool canPillage(const CvPlot* pPlot) const;
	bool pillage(const bool bAutoPillage = false);

	//TSHEEP Assassin Mission
	bool canAssassin(const CvPlot* pPlot, bool bTestVisible) const;
	//int assassinCost(const CvUnit* pTargetUnit) const;
	//int assassinProb(const CvUnit* pTargetUnit) const;
	//bool assassin();
	//TSHEEP Other functions
	bool awardSpyExperience(TeamTypes eTargetTeam, int iModifier);
	//TSHEEP End
	//RevolutionDCM start
	bool canBribe(const CvPlot* pPlot, bool bTestVisible) const;
	//RevolutionDCM end

	bool canPlunder(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool plunder();
	void updatePlunder(int iChange, bool bUpdatePlotGroups);

	int sabotageProb(const CvPlot* pPlot, ProbabilityTypes eProbStyle = PROBABILITY_REAL) const;
	bool canSabotage(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool sabotage();

	int destroyCost(const CvPlot* pPlot) const;
	int destroyProb(const CvPlot* pPlot, ProbabilityTypes eProbStyle = PROBABILITY_REAL) const;
	bool canDestroy(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool destroy();

	int stealPlansCost(const CvPlot* pPlot) const;
	int stealPlansProb(const CvPlot* pPlot, ProbabilityTypes eProbStyle = PROBABILITY_REAL) const;
	bool canStealPlans(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool stealPlans();

	bool canFound(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool found();

	bool canSpread(const CvPlot* pPlot, ReligionTypes eReligion, bool bTestVisible = false) const;
	bool spread(ReligionTypes eReligion);

	bool canSpreadCorporation(const CvPlot* pPlot, CorporationTypes eCorporation, bool bTestVisible = false) const;
	bool spreadCorporation(CorporationTypes eCorporation);
	int spreadCorporationCost(CorporationTypes eCorporation, const CvCity* pCity) const;

	bool canJoin(const CvPlot* pPlot, SpecialistTypes eSpecialist) const;
	bool join(SpecialistTypes eSpecialist);

	bool canConstruct(const CvPlot* pPlot, BuildingTypes eBuilding, bool bTestVisible = false) const;
	bool construct(BuildingTypes eBuilding);
	bool canAddHeritage(const CvPlot* pPlot, const HeritageTypes eType, const bool bTestVisible = false) const;
	bool addHeritage(const HeritageTypes eType);

	TechTypes getDiscoveryTech() const;
	int getDiscoverResearch(const TechTypes eTech = NO_TECH) const;
	bool canDiscover() const;
	bool discover(TechTypes eTech = NO_TECH);

	int getMaxHurryProduction(const CvCity* pCity) const;
	int getHurryProduction(const CvPlot* pPlot) const;
	bool canHurry(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool hurry();

	int getTradeGold(const CvPlot* pPlot) const;
	bool canTrade(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool trade();

	int getGreatWorkCulture() const;
	bool canGreatWork(const CvPlot* pPlot) const;
	bool greatWork();

	bool doOutcomeMission(MissionTypes eMission);

	int getEspionagePoints() const;
	bool canInfiltrate() const;
	bool canInfiltrate(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool infiltrate();

	bool canEspionage(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool espionage(EspionageMissionTypes eMission, int iData);
	bool testSpyIntercepted(PlayerTypes eTargetPlayer, int iModifier = 0);
	int getSpyInterceptPercent(TeamTypes eTargetTeam) const;
	bool isIntruding() const;

	bool canGoldenAge(bool bTestVisible = false) const;
	bool goldenAge();

	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false) const;
	bool build(BuildTypes eBuild);

	bool canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const;
	bool promote(PromotionTypes ePromotion, int iLeaderUnitId);

	int canLead(const CvPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);

	int canGiveExperience(const CvPlot* pPlot) const;
	bool giveExperience();
	int getStackExperienceToGive(int iNumUnits) const;

	int upgradePrice(UnitTypes eUnit) const;
	bool upgradeAvailable(UnitTypes eFromUnit, UnitTypes eToUnit) const;
	bool canUpgrade(UnitTypes eUnit, bool bTestVisible = false) const;
	bool isReadyForUpgrade() const;
	bool hasUpgrade(bool bSearch = false) const;
	bool hasUpgrade(UnitTypes eUnit, bool bSearch = false) const;
	CvCity* getUpgradeCity(bool bSearch = false) const;
	CvCity* getUpgradeCity(UnitTypes eUnit, bool bSearch = false, int* iSearchValue = NULL) const;
	bool upgrade(UnitTypes eUnit);

	HandicapTypes getHandicapType() const;
	CivilizationTypes getCivilizationType() const;
	const wchar_t* getVisualCivAdjective(TeamTypes eForTeam) const;
	SpecialUnitTypes getSpecialUnitType() const;
	UnitTypes getCaptureUnitType() const;
	UnitCombatTypes getUnitCombatType() const;
	InvisibleTypes getInvisibleType() const;
	int getNumSeeInvisibleTypes() const;
	InvisibleTypes getSeeInvisibleType(int i) const;

	int flavorValue(FlavorTypes eFlavor) const;

	bool isNPC() const;
	bool isHominid() const;
	bool isHuman() const;

	int visibilityRange(const CvPlot* pPlot = NULL) const;

	int baseMoves() const;
	int maxMoves() const;
	int movesLeft() const;

	int airRange() const;
	int nukeRange() const;

	bool canBuildRoute() const;
	ImprovementTypes getBuildTypeImprovement() const;

	bool isAnimal() const;
	bool isNoBadGoodies() const;

	bool isOnlyDefensive() const;
	void changeOnlyDefensiveCount(int iChange);

	bool hasRBombardForceAbility() const;
	int getRBombardForceAbilityCount() const;
	void changeRBombardForceAbilityCount(int iChange);

	bool isNoCapture() const;
	bool isRivalTerritory() const;
	bool isMilitaryHappiness() const;
	bool isMilitaryBranch() const;
	bool isInvestigate() const;
	bool isCounterSpy() const;
	bool isSpy() const;
	bool isFound() const;

	bool isGoldenAge() const;

	// Can this unit always coexist with all units on the specified plot?
	bool canCoexistAlwaysOnPlot(const CvPlot& onPlot) const;
	// Can this unit coexist with the specified team anywhere?
	bool canCoexistWithTeam(const TeamTypes withTeam) const;
	// Can this unit coexist with the specified team, on the specified plot?
	bool canCoexistWithTeamOnPlot(const TeamTypes withTeam, const CvPlot& onPlot) const;
	// Can this unit coexist with an attacking unit (possibly performing an assassination)?
	bool canCoexistWithAttacker(const CvUnit& attacker, bool bStealthDefend = false, bool bAssassinate = false) const;

	// Checks for differing domains, transport status, amnesty game setting
	// TODO: roll this into the other Coexist functions
	bool canUnitCoexistWithArrivingUnit(const CvUnit& pUnit) const;

	bool isCombat() const;

	int withdrawalHP(int iMaxHitPoints, int iAttackerEarly) const;
	bool isHurt() const;

	void setBaseCombatStr(int iCombat);
	int baseCombatStr() const;
	int baseCombatStrNonGranular() const;
	int baseCombatStrPreCheck() const;
	int baseAirCombatStrPreCheck() const;
	float fbaseCombatStr() const;
	/*** Dexy - Surround and Destroy START ****/
	int maxCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL, bool bSurroundedModifier = true) const;
	int currCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL, bool bSurroundedModifier = true) const;
	/*** Dexy - Surround and Destroy  END  ****/
	int currFirepower(const CvPlot* pPlot, const CvUnit* pAttacker) const;
	int currEffectiveStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const;

	bool canAttackNow() const;
	bool canAttack() const;
	bool canAttack(const CvUnit& defender) const;
	bool canAmbush(const CvUnit& defender, const bool bAssassinate) const;
	bool canDefend(const CvPlot* pPlot = NULL) const;
	bool canStealthDefend(const CvUnit* victim) const;
	bool canSiege(TeamTypes eTeam) const;

	int airBaseCombatStr() const;
	float fairBaseCombatStr() const;
	int airMaxCombatStr(const CvUnit* pOther) const;
	int airCurrCombatStr(const CvUnit* pOther) const;
	int combatLimit(const CvUnit* pOpponent = NULL) const;
	int airCombatLimit(const CvUnit* pOpponent = NULL) const;
	bool canAirDefend(const CvPlot* pPlot = NULL) const;
	int airCombatDamage(const CvUnit* pDefender) const;
	int rangeCombatDamage(const CvUnit* pDefender) const;
	CvUnit* bestInterceptor(const CvPlot* pPlot) const;
	CvUnit* bestSeaPillageInterceptor(const CvUnit* pPillager, int iMinOdds) const;

	bool isAutomated() const;
	bool isFortifyable() const;

	bool isBuildUpable() const;
	int fortifyModifier() const;
	//int establishModifier() const;
	//int escapeModifier() const;
	//TB Combat Mods begin
	int fortifyRepelModifier() const;
	//TB Combat Mods End
	int experienceNeeded(int iLvlOffset = 0) const;
	int attackXPValue() const;
	int defenseXPValue() const;
	int maxXPValue(const CvUnit* pVictim, bool bBarb = false) const;

	int firstStrikes() const;
	int chanceFirstStrikes() const;
	int maxFirstStrikes() const;

	bool alwaysInvisible() const;
	bool immuneToFirstStrikes() const;
	bool noDefensiveBonus() const;
	int getExtraNoDefensiveBonusCount() const;
	void changeExtraNoDefensiveBonusCount(int iChange);
	bool ignoreBuildingDefense() const;
	bool canMoveImpassable() const;
	bool canMoveAllTerrain() const;
	bool flatMovementCost() const;
	bool ignoreTerrainCost() const;
	bool isNeverInvisible() const;
	int getNoInvisibilityCount() const;
	void changeNoInvisibilityCount(int iChange);
	bool isNukeImmune() const;
	bool isInquisitor() const;

	int maxInterceptionProbability(bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	int currInterceptionProbability() const;
	int evasionProbability(bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	int withdrawalProbability() const;
//TB Combat Mods Begin
	int attackCombatModifierTotal() const;
	int defenseCombatModifierTotal() const;
	int pursuitProbability() const;
	int earlyWithdrawTotal() const;
	int vsBarbsModifier() const;
	int religiousCombatModifierTotal(ReligionTypes eReligion = NO_RELIGION, bool bDisplay = false) const;
	int armorTotal() const;
	int punctureTotal() const;
	int damageModifierTotal() const;
	int costModifierTotal() const;
	int overrunTotal() const;
	int repelTotal() const;
	int fortRepelTotal() const;
	int repelRetriesTotal() const;
	int unyieldingTotal() const;
	int knockbackTotal() const;
	int knockbackRetriesTotal() const;
	bool canStampede() const;
	bool canAttackOnlyCities() const;
	bool canIgnoreNoEntryLevel() const;
	bool canIgnoreZoneofControl() const;
	bool canFliesToMove() const;
	int strAdjperRndTotal() const;
	int strAdjperAttTotal() const;
	int strAdjperDefTotal() const;
	int withdrawAdjperAttTotal() const;
	int currentStrAdjperRndTotal() const;
	int currentStrAdjperAttTotal() const;
	int currentStrAdjperDefTotal() const;
	int currentWithdrawAdjperAttTotal() const;
	int unnerveTotal() const;
	int encloseTotal() const;
	int lungeTotal() const;
	int dynamicDefenseTotal() const;
	bool canAnimalIgnoresBorders() const;
	bool canAnimalIgnoresImprovements() const;
	bool canAnimalIgnoresCities() const;
	bool canOnslaught() const;
	bool hasCureAfflictionType(PromotionLineTypes ePromotionLineType) const;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	int fortitudeTotal() const;
#endif
	int aidTotal(PropertyTypes eProperty) const;
	int dodgeTotal() const;
	int precisionTotal() const;
	int powerShotsTotal() const;
	int powerShotCombatModifierTotal() const;
	int powerShotPunctureModifierTotal() const;
	int powerShotPrecisionModifierTotal() const;
	int powerShotCriticalModifierTotal() const;
	int criticalModifierTotal() const;
	int enduranceTotal() const;
	bool dealsColdDamage() const;
	bool hasImmunitytoColdDamage() const;
	int poisonProbabilityModifierTotal() const;
//TB Combat Mods End
	int collateralDamage() const;
	int collateralDamageLimit() const;
	int collateralDamageMaxUnits() const;

	int cityAttackModifier() const;
	int cityDefenseModifier() const;
	int cityDefenseVSOpponent(const CvUnit* pOpponent) const;
	int animalCombatModifier() const;
	int hillsAttackModifier() const;
	int hillsDefenseModifier() const;
	int terrainAttackModifier(TerrainTypes eTerrain) const;
	int terrainDefenseModifier(TerrainTypes eTerrain) const;
	int featureAttackModifier(FeatureTypes eFeature) const;
	int featureDefenseModifier(FeatureTypes eFeature) const;
	int unitAttackModifier(UnitTypes eUnit) const;
	int unitDefenseModifier(UnitTypes eUnit) const;
	int unitCombatModifier(UnitCombatTypes eUnitCombat) const;
	int domainModifier(DomainTypes eDomain) const;

	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const;
	int cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool hasCargo() const;
	bool canCargoAllMove() const;
	bool canCargoEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage) const;
	int getUnitAICargo(UnitAITypes eUnitAI) const;

	int getIndex() const;
	void setID(int iID);

	int getGroupID() const;
	bool isInGroup() const;
	bool isGroupHead() const;
	bool canJoinGroup(const CvPlot* pPlot, const CvSelectionGroup* pSelectionGroup) const;
	void joinGroup(CvSelectionGroup* pSelectionGroup, bool bRemoveSelected = false, bool bRejoin = true);

	void setHotKeyNumber(int iNewValue);

	inline int getX() const { return m_iX; }
	inline int getY() const { return m_iY; }
	bool isInViewport() const;
	void setXY(int iX, int iY, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false, bool bInit = false);
	bool isTempUnit() const;
	bool at(int iX, int iY) const;
	void addMission(const CvMissionDefinition& mission);

	CvPlot* plot() const;
	int getArea() const;
	CvArea* area() const;

	int getLastMoveTurn() const;
	void setLastMoveTurn(int iNewValue);

	CvPlot* getReconPlot() const;
	void setReconPlot(CvPlot* pNewValue);

	int getGameTurnCreated() const;
	void setGameTurnCreated(int iNewValue);

	int getHealAsDamage(UnitCombatTypes eHealAsType) const;
	void changeHealAsDamage(UnitCombatTypes eHealAsType, int iChange, PlayerTypes ePlayer = NO_PLAYER);
	void setHealAsDamage(UnitCombatTypes eHealAsType, int iNewValue, PlayerTypes ePlayer = NO_PLAYER, bool bNotifyEntity = false);
	int getDamagePercent() const;
	void setDamage(int iNewValue, PlayerTypes ePlayer = NO_PLAYER, bool bNotifyEntity = true, UnitCombatTypes eHealAsType = NO_UNITCOMBAT, bool bSecondPass = false);
	void changeDamage(int iChange, PlayerTypes ePlayer = NO_PLAYER, UnitCombatTypes eHealAsType = NO_UNITCOMBAT);
	void changeDamagePercent(int iChange, PlayerTypes ePlayer = NO_PLAYER);
	void setupPreCombatDamage();
	int getPreCombatDamage() const;

	int getMoves() const;
	void setMoves(int iNewValue);
	void changeMoves(int iChange);
	void finishMoves();

	int getExperience() const;
	void setExperience(int iNewValue);
	void changeExperience(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false);

	int getLevel() const;
	void setLevel(int iNewValue);
	void changeLevel(int iChange);

	int getCargo() const;
	int SMgetCargo() const;
	void changeCargo(int iChange);
	void SMchangeCargo(int iChange);
	void getCargoUnits(std::vector<CvUnit*>& aUnits) const;
	void validateCargoUnits();

	CvPlot* getAttackPlot() const;
	void setAttackPlot(const CvPlot* pNewValue, bool bAirCombat);
	bool isAirCombat() const;

	void setCombatTimer(int iNewValue);
	void changeCombatTimer(int iChange);

	int getCombatFirstStrikes() const;
	void setCombatFirstStrikes(int iNewValue);
	void changeCombatFirstStrikes(int iChange);

	int getFortifyTurns() const;
	void setFortifyTurns(int iNewValue);

	int getBlitzCount() const;
	bool isBlitz() const;
	void changeBlitzCount(int iChange);

	int getAmphibCount() const;
	bool isAmphib() const;
	void changeAmphibCount(int iChange);

	int getRiverCount() const;
	bool isRiver() const;
	void changeRiverCount(int iChange);

	int getEnemyRouteCount() const;
	bool isEnemyRoute() const;
	void changeEnemyRouteCount(int iChange);

	int getAlwaysHealCount() const;
	bool isAlwaysHeal() const;
	void changeAlwaysHealCount(int iChange);

	int getHillsDoubleMoveCount() const;
	bool isHillsDoubleMove() const;
	void changeHillsDoubleMoveCount(int iChange);

	int getImmuneToFirstStrikesCount() const;
	void changeImmuneToFirstStrikesCount(int iChange);

	int getAlwaysInvisibleCount() const;
	void changeAlwaysInvisibleCount(int iChange);

	int getDefensiveVictoryMoveCount() const;
	bool isDefensiveVictoryMove() const;
	void changeDefensiveVictoryMoveCount(int iChange);

	int getFreeDropCount() const;
	bool isFreeDrop() const;
	void changeFreeDropCount(int iChange);

	int getOffensiveVictoryMoveCount() const;
	bool isOffensiveVictoryMove() const;
	void changeOffensiveVictoryMoveCount(int iChange);

	int getOneUpCount() const;
	bool isOneUp() const;
	void changeOneUpCount(int iChange);

	int getPillageEspionageCount() const;
	bool isPillageEspionage() const;
	void changePillageEspionageCount(int iChange);

	int getPillageMarauderCount() const;
	bool isPillageMarauder() const;
	void changePillageMarauderCount(int iChange);

	int getPillageOnMoveCount() const;
	bool isPillageOnMove() const;
	void changePillageOnMoveCount(int iChange);

	int getPillageOnVictoryCount() const;
	bool isPillageOnVictory() const;
	void changePillageOnVictoryCount(int iChange);

	int getPillageResearchCount() const;
	bool isPillageResearch() const;
	void changePillageResearchCount(int iChange);

	int getAirCombatLimitChange() const;
	void changeAirCombatLimitChange(int iChange);

	int getCelebrityHappy() const;
	void changeCelebrityHappy(int iChange);

	int getCollateralDamageLimitChange() const;
	void changeCollateralDamageLimitChange(int iChange);

	int getCollateralDamageMaxUnitsChange() const;
	void changeCollateralDamageMaxUnitsChange(int iChange);

	int getCombatLimitChange() const;
	void changeCombatLimitChange(int iChange);

	int getExtraDropRange() const;
	void changeExtraDropRange(int iChange);

	int getSurvivorChance() const;
	void changeSurvivorChance(int iChange);

	int getVictoryAdjacentHeal() const;
	void changeVictoryAdjacentHeal(int iChange);

	int getVictoryHeal() const;
	void changeVictoryHeal(int iChange);

	int getVictoryStackHeal() const;
	void changeVictoryStackHeal(int iChange);

	int getExtraVisibilityRange() const;
	void changeExtraVisibilityRange(int iChange);

	int getExtraMoves() const;
	void changeExtraMoves(int iChange);

	int getExtraMoveDiscount() const;
	void changeExtraMoveDiscount(int iChange);

	int getExtraAirRange() const;
	void changeExtraAirRange(int iChange);

	int getExtraIntercept(bool bIgnoreCommanders = false,bool bIgnoreCommodores = false) const;
	void changeExtraIntercept(int iChange);

	int getExtraEvasion(bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraEvasion(int iChange);

	int getExtraFirstStrikes() const;
	void changeExtraFirstStrikes(int iChange);

	int getExtraChanceFirstStrikes() const;
	void changeExtraChanceFirstStrikes(int iChange);

	int getExtraWithdrawal(bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraWithdrawal(int iChange);

	//TB Combat Mods Start
	int getExtraAttackCombatModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraAttackCombatModifier (int iChange);

	int getExtraDefenseCombatModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraDefenseCombatModifier (int iChange);

	int getExtraPursuit (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPursuit (int iChange);

	int getExtraEarlyWithdraw (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraEarlyWithdraw (int iChange);

	int getExtraVSBarbs (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraVSBarbs (int iChange);

	int getExtraReligiousCombatModifier(bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraReligiousCombatModifier(int iChange);

	int getExtraArmor (bool bIgnoreCommanders = false,bool bIgnoreCommodores = false) const;
	void changeExtraArmor (int iChange);

	int getExtraPuncture (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPuncture (int iChange);

	int getExtraDamageModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraDamageModifier (int iChange);

	void changeExtraUpkeep100(const int iChange);
	void changeUpkeepModifier(const int iChange);
	void calcUpkeepMultiplierSM(const int iGroupOffset);
	void calcUpkeep100();
	void recalculateUnitUpkeep();
	int getExtraUpkeep100() const;
	int getUpkeepModifier() const;
	int getUpkeepMultiplierSM() const;
	int getUpkeep100() const;

	int getExtraOverrun(bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraOverrun (int iChange);

	int getExtraRepel(bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraRepel (int iChange);

	int getExtraFortRepel() const;
	void changeExtraFortRepel (int iChange);

	int getExtraRepelRetries() const;
	void changeExtraRepelRetries (int iChange);

	int getExtraUnyielding (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraUnyielding (int iChange);

	int getExtraKnockback (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraKnockback (int iChange);

	int getExtraKnockbackRetries (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraKnockbackRetries (int iChange);

	int getStampedeCount() const;
	bool cannotStampede() const;
	bool mayStampede() const;
	void changeStampedeCount(int iChange);

	int getAttackOnlyCitiesCount() const;
	void setAttackOnlyCitiesCount(int iChange);
	void changeAttackOnlyCitiesCount(int iChange);

	int getIgnoreNoEntryLevelCount() const;
	void setIgnoreNoEntryLevelCount(int iChange);
	void changeIgnoreNoEntryLevelCount(int iChange);

	int getIgnoreZoneofControlCount() const;
	void changeIgnoreZoneofControlCount(int iChange);

	int getFliesToMoveCount() const;
	void setFliesToMoveCount(int iChange);
	void changeFliesToMoveCount(int iChange);

	void changeExtraStrAdjperRnd(int iChange);
	void changeExtraStrAdjperAtt(int iChange);
	void changeExtraStrAdjperDef(int iChange);
	void changeExtraWithdrawAdjperAtt(int iChange);

	int getExtraUnnerve (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraUnnerve (int iChange);

	int getExtraEnclose (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraEnclose (int iChange);

	int getExtraLunge (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraLunge (int iChange);

	int getExtraDynamicDefense (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraDynamicDefense (int iChange);

	int getExtraStrength () const;
	void changeExtraStrength (int iChange);

	int getSMStrength () const;
	void setSMStrength ();

	int getAnimalIgnoresBordersCount() const;
	void changeAnimalIgnoresBordersCount(int iChange);

	int getOnslaughtCount() const;
	bool mayOnslaught() const;
	void changeOnslaughtCount(int iChange);

	int getCureAfflictionCount(PromotionLineTypes ePromotionLineType) const;
	bool hasExtraCureAffliction(PromotionLineTypes ePromotionLineType) const;
	void changeCureAfflictionCount(PromotionLineTypes ePromotionLineType, int iChange);

	int getExtraFortitude (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraFortitude (int iChange);

	int getExtraDodgeModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraDodgeModifier (int iChange);

	int getExtraPrecisionModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPrecisionModifier (int iChange);

	int getExtraPowerShots (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPowerShots (int iChange);

	int getExtraPowerShotCombatModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPowerShotCombatModifier (int iChange);

	int getExtraPowerShotPunctureModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPowerShotPunctureModifier (int iChange);

	int getExtraPowerShotPrecisionModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPowerShotPrecisionModifier (int iChange);

	int getExtraPowerShotCriticalModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPowerShotCriticalModifier (int iChange);

	int getExtraCriticalModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraCriticalModifier (int iChange);

	int getExtraEndurance (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraEndurance (int iChange);

	int getDealColdDamageCount() const;
	bool cannotDealColdDamage() const;
	bool mayDealColdDamage() const;
	void changeDealColdDamageCount(int iChange);

	int getColdImmuneCount() const;
	bool cannotColdImmune() const;
	bool mayColdImmune() const;
	void changeColdImmuneCount(int iChange);

	int getExtraPoisonProbabilityModifier (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraPoisonProbabilityModifier (int iChange);
	//TB Combat Mods End

	int getExtraCollateralDamage() const;
	void changeExtraCollateralDamage(int iChange);

	int getExtraEnemyHeal() const;
	void changeExtraEnemyHeal(int iChange);

	int getExtraNeutralHeal() const;
	void changeExtraNeutralHeal(int iChange);

	int getExtraFriendlyHeal() const;
	void changeExtraFriendlyHeal(int iChange);

	int getSameTileHeal() const;
	void changeSameTileHeal(int iChange);

	int getAdjacentTileHeal() const;
	void changeAdjacentTileHeal(int iChange);

	int getExtraCombatPercent() const;
	void changeExtraCombatPercent(int iChange);

	int getExtraCityAttackPercent() const;
	void changeExtraCityAttackPercent(int iChange);

	int getExtraCityDefensePercent() const;
	void changeExtraCityDefensePercent(int iChange);

	int getExtraHillsAttackPercent() const;
	void changeExtraHillsAttackPercent(int iChange);

	int getExtraHillsDefensePercent() const;
	void changeExtraHillsDefensePercent(int iChange);

	// WorkRateMod
	int getWorkModifier() const;
	int hillsWorkModifier() const;
	int peaksWorkModifier() const;
	int getExtraWorkModForBuild(const BuildTypes eBuild) const;

	int getCollateralDamageProtection() const;
	void changeCollateralDamageProtection(int iChange);

	int getPillageChange() const;
	void changePillageChange(int iChange);

	int getUpgradeDiscount() const;
	void changeUpgradeDiscount(int iChange);

	int getExperiencePercent() const;
	void changeExperiencePercent(int iChange);

	int getKamikazePercent() const;
	void changeKamikazePercent(int iChange);

	void setFacingDirection(DirectionTypes facingDirection);
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();

	int getDropRange() const;

	bool isMadeAttack() const;
	void setMadeAttack(bool bNewValue);
	//TB Combat Mods (Att&DefCounters)
	int getRoundCount () const;
	void changeRoundCount (int iChange);
	int getAttackCount() const;
	void changeAttackCount(int iChange);
	int getDefenseCount() const;
	void changeDefenseCount(int iChange);
	//TB Combat Mods end

	bool isMadeInterception() const;
	void setMadeInterception(bool bNewValue);

	void setPromotionReady(bool bNewValue);
	void testPromotionReady();

	bool isDelayedDeath() const;
	bool doDelayedDeath();

	bool isBlockading() const;
	void setBlockading(bool bNewValue);
	void collectBlockadeGold();

	PlayerTypes getCombatOwner(TeamTypes eForTeam, const CvPlot* pPlot) const;

	PlayerTypes getCapturingPlayer() const;
	void setCapturingPlayer(PlayerTypes eNewValue);

	CvUnit* getCapturingUnit() const;
	void setCapturingUnit(const CvUnit* pCapturingUnit);

	const CvUnitInfo& getUnitInfo() const;

	void setLeaderUnitType(UnitTypes leaderUnitType);

	void setCombatUnit(CvUnit* pUnit, bool bAttacking = false, bool bQuick = true, bool bStealthAttack = false, bool bStealthDefense = false);
	bool showSeigeTower(const CvUnit* pDefender) const; // K-Mod

	CvUnit* getTransportUnit() const;
	bool isCargo() const;
	void setTransportUnit(CvUnit* pTransportUnit);

	int getExtraDomainModifier(DomainTypes eIndex) const;
	void changeExtraDomainModifier(DomainTypes eIndex, int iChange);

// BUG - Unit Name - start
	bool isDescInName() const;
// BUG - Unit Name - end
	const wchar_t* getNameKey() const;
	const CvWString& getNameNoDesc() const;
	void setName(const CvWString szNewValue);

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const;
	void setScriptData(std::string szNewValue);

	int getTerrainDoubleMoveCount(TerrainTypes eIndex) const;
	bool isTerrainDoubleMove(TerrainTypes eIndex) const;
	void changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange);

	int getFeatureDoubleMoveCount(FeatureTypes eIndex) const;
	bool isFeatureDoubleMove(FeatureTypes eIndex) const;
	void changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange);

	int getExtraTerrainAttackPercent(TerrainTypes eIndex) const;
	void changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange);
	int getExtraTerrainDefensePercent(TerrainTypes eIndex) const;
	void changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange);
	int getExtraFeatureAttackPercent(FeatureTypes eIndex) const;
	void changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange);
	int getExtraFeatureDefensePercent(FeatureTypes eIndex) const;
	void changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange);

	int getExtraTerrainWorkPercent(TerrainTypes eIndex) const;
	void changeExtraTerrainWorkPercent (TerrainTypes eIndex, int iChange);
	int getExtraFeatureWorkPercent(FeatureTypes eIndex) const;
	void changeExtraFeatureWorkPercent(FeatureTypes eIndex, int iChange);
	int terrainWorkPercent(TerrainTypes eIndex) const;
	int featureWorkPercent(FeatureTypes eIndex) const;
	int buildWorkPercent(BuildTypes eIndex) const;

	int getExtraUnitCombatModifier(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange);
	//TB Combat Mods (adjusted the following line to include ", bool bEquip = false, bool bAfflict = false, bool bPromote = false"
	bool canAcquirePromotion(PromotionTypes ePromotion, PromotionRequirements::flags requirements) const;
	// Deprecated, use the one above that takes enum flags instead for increased readability.
	bool canAcquirePromotion(PromotionTypes ePromotion, bool bIgnoreHas = false, bool bEquip = false, bool bAfflict = false, bool bForLeader = false, bool bForOffset = false, bool bForFree = false, bool bForBuildUp = false, bool bForStatus = false) const;
	//TB Combat Mods end
	bool canAcquirePromotionAny() const;
	bool isPromotionValid(PromotionTypes ePromotion, bool bFree = false, bool bKeepCheck = false) const;
	bool isHealsUnitCombat(UnitCombatTypes eIndex) const;
	bool isHasUnitCombat(UnitCombatTypes eIndex) const;
	void processUnitCombat(UnitCombatTypes eIndex, bool bAdding, bool bByPromo = false);
	void setHasUnitCombat(UnitCombatTypes eIndex, bool bNewValue, bool bByPromo = false);
	bool isHasPromotion(PromotionTypes eIndex) const;

	void setHasPromotion(PromotionTypes eIndex, bool bNewValue, PromotionApply::flags flags);
	// Deprecated, use the one above that takes enum flags instead for increased readability.
	void setHasPromotion(PromotionTypes eIndex, bool bNewValue, bool bFree = true, bool bDying = false, bool bInitial = false, bool bFromTrait = false);

	typedef cvInternalGlobals::PromotionPredicateFn PromotionPredicateFn;

	// Consecutively apply a number of promotions to a set of units.
	// promotionPredicateFn defines what promotions are valid to be applied.
	// Number defines how many to apply
	static bool applyUnitPromotions(const std::vector<CvUnit*>& units, int number, PromotionPredicateFn promotionPredicateFn);
	static bool applyUnitPromotions(CvUnit* unit, int number, PromotionPredicateFn promotionPredicateFn);
	// Apply a number of either upgrade or downgrade promotions to a set of units.
	// The valid promotions for upgrade and downgrade are defined by the predicate functions.
	// Offset defines which type and how many to apply. offset < 0 means that downgrades are required, offset > 0 means upgrades are required.
	static bool normalizeUnitPromotions(const std::vector<CvUnit*>& units, int offset, PromotionPredicateFn upgradePredicateFn, PromotionPredicateFn downgradePredicateFn);
	static bool normalizeUnitPromotions(CvUnit* unit, int offset, PromotionPredicateFn upgradePredicateFn, PromotionPredicateFn downgradePredicateFn);

	UnitCombatTypes getBestHealingType();
	UnitCombatTypes getBestHealingTypeConst() const;

	int getSubUnitCount() const;
	int getSubUnitsAlive(int iDamage) const;

	bool isTargetOf(const CvUnit& attacker) const;

	bool isEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL, const CvUnit* pUnit = NULL) const;
	bool isPotentialEnemy(TeamTypes eTeam, const CvPlot* pPlot = NULL, const CvUnit* pUnit = NULL) const;

	bool canRangeStrike() const;
	bool canRangeStrikeAt(const CvPlot* pPlot, int iX, int iY) const;
	bool rangeStrike(int iX, int iY);

	int getTriggerValue(EventTriggerTypes eTrigger, const CvPlot* pPlot, bool bCheckPlot) const;
	bool canApplyEvent(EventTypes eEvent) const;
	void applyEvent(EventTypes eEvent);

	int getImmobileTimer() const;
	void setImmobileTimer(int iNewValue);
	void changeImmobileTimer(int iChange);

	bool isCanRespawn() const;
	void setCanRespawn(bool bNewValue);

	bool isSurvivor() const;
	void setSurvivor(bool bNewValue);

	bool potentialWarAction(const CvPlot* pPlot) const;
	bool willRevealByMove(const CvPlot* pPlot) const;

	bool isAlwaysHostile(const CvPlot* pPlot) const;

	bool verifyStackValid();

// Dale - AB: Bombing START
	bool canAirBomb1() const;
	bool canAirBomb1At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb1(int iX, int iY);
	bool canAirBomb2() const;
	bool canAirBomb2At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb2(int iX, int iY);
	bool canAirBomb3() const;
	bool canAirBomb3At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb3(int iX, int iY);
	bool canAirBomb4() const;
	bool canAirBomb4At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb4(int iX, int iY);
	bool canAirBomb5() const;
	bool canAirBomb5At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb5(int iX, int iY);

// Dale - RB: Field Bombard START
	bool canRBombard(bool bEver = false) const;
	bool canBombardAtRanged(const CvPlot* pPlot, int iX, int iY) const;
	bool bombardRanged(int iX, int iY, bool sAttack = false);

// RevolutionDCM - ranged bombard
	bool isRbombardable(int iMinStack) const;
	int getRbombardSeigeCount(const CvPlot* pPlot) const;

// Dale - SA: Opp Fire
	void doOpportunityFire();

// Dale - SA: Active Defense
	void doActiveDefense();

// Dale - FE: Fighters
	bool canFEngage() const;
	bool canFEngageAt(const CvPlot* pPlot, int iX, int iY) const;
	bool fighterEngage(int iX, int iY);
// ! Dale

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

	virtual void AI_init(UnitAITypes eUnitAI, int iBirthmark) = 0;
	virtual void AI_uninit() = 0;
	virtual void AI_reset(UnitAITypes eUnitAI = NO_UNITAI, bool bConstructor = false) = 0;
	virtual bool AI_update() = 0;
	virtual bool AI_follow() = 0;
	virtual bool AI_upgrade() = 0;
	virtual bool AI_promote() = 0;
	virtual int AI_groupFirstVal() const = 0;
	virtual int AI_groupSecondVal() const = 0;
	virtual int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, CvUnit** ppDefender = NULL, bool bAssassinate = false) = 0;
	//	Variant to test a specific defender AS IF it was in the specified plot
	virtual int AI_attackOddsAtPlot(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults = false) = 0;
	virtual bool AI_bestCityBuild(const CvCity* pCity, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, const CvPlot* pIgnorePlot = NULL, const CvUnit* pUnit = NULL) = 0;
	virtual bool AI_isCityAIType() const = 0;
	virtual UnitAITypes AI_getUnitAIType() const = 0;
	virtual void AI_setUnitAIType(UnitAITypes eNewValue) = 0;
	virtual int AI_sacrificeValue(const CvPlot* pPlot) const = 0;
	virtual bool AI_isAwaitingContract() const = 0;
	virtual bool AI_isCityGarrison(const CvCity* pCity) const = 0;
	virtual void AI_setAsGarrison(const CvCity* pCity) = 0;
	virtual bool AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT) = 0;
	virtual void AI_flushValueCache() = 0;
	virtual int	AI_genericUnitValueTimes100(UnitValueFlags eFlags) const = 0;
	virtual int AI_getBirthmark() const = 0;
	virtual void setToWaitOnUnitAI(UnitAITypes eUnitAI, bool bAdd) = 0;
	virtual bool isWaitingOnUnitAI(int iIndex) const = 0;
	virtual bool isWaitingOnUnitAIAny() const = 0;
	virtual bool processContracts(int iMinPriority = 0) = 0;

	inline int getMovementCharacteristicsHash() const { return m_movementCharacteristicsHash; }

	PlayerTypes m_eOriginalOwner;

	bool isWorker() const;
	UnitCompWorker* CvUnit::getWorkerComponent() const;
	void deselect(const bool bQuick = false);
	void forceInvalidCoordinates();

protected:
	int m_iDCMBombRange;
	int m_iDCMBombAccuracy;
	int m_iHealUnitCombatCount;

	DomainTypes m_eNewDomainCargo;
	SpecialUnitTypes m_eNewSpecialCargo;
	SpecialUnitTypes m_eNewSMNotSpecialCargo;
	SpecialUnitTypes m_eSpecialUnit;
	MissionTypes m_eSleepType;
	PromotionLineTypes m_eCurrentBuildUpType;

	bool m_bHiddenNationality;
	bool m_bHasHNCapturePromotion;
	int m_iCanMovePeaksCount;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	int m_iCanLeadThroughPeaksCount;

	int m_movementCharacteristicsHash;

	PlayerTypes m_eNationality;
	CombatResult m_combatResult;
	int m_iSleepTimer;
	int m_iZoneOfControlCount;

	bool m_bAutoPromoting;
	bool m_bAutoUpgrading;
	IDInfo m_shadowUnit;
	//Great Commanders... By KillmePlease
	mutable int m_iCommanderID; //id of commander. used for game save/load
	int m_iUsedCommanderID;
	mutable int m_iCommanderCacheTurn;
	mutable int m_iCommodoreID; //id of commodore. used for game save/load
    int m_iUsedCommodoreID;
    mutable int m_iCommodoreCacheTurn;
#define	NO_COMMANDER_ID	-2	//	Pseudo-id used to signify an assertion that the unit has no commander
	int m_iPreCombatDamage;

	int m_iID;
	int m_iGroupID;
	int m_iHotKeyNumber;
	int m_iX;
	int m_iY;
	int m_iLastMoveTurn;
	int m_iReconX;
	int m_iReconY;
	int m_iGameTurnCreated;
	int m_iDamage;
	int m_iMoves;
	int m_iExperience;
	int m_iLevel;
	int m_iCargo;
	int m_iCargoCapacity;
	int m_iSMCargo;
	int m_iSMCargoCapacity;
	int m_iAttackPlotX;
	int m_iAttackPlotY;
	int m_iCombatTimer;
	int m_iCombatFirstStrikes;
	int m_iCombatDamage;
	int m_iFortifyTurns;
	int m_iBuildUpTurns;
	int m_iBlitzCount;
	int m_iRBombardForceAbilityCount;
	int m_iAmphibCount;
	int m_iRiverCount;
	int m_iEnemyRouteCount;
	int m_iAlwaysHealCount;
	int m_iHillsDoubleMoveCount;
	int m_iImmuneToFirstStrikesCount;
	int m_iAlwaysInvisibleCount;

	int m_iDefensiveVictoryMoveCount;
	int m_iFreeDropCount;
	int m_iOffensiveVictoryMoveCount;

	int m_iOneUpCount;
	int m_iPillageCultureCount;
	int m_iPillageEspionageCount;
	int m_iPillageMarauderCount;
	int m_iPillageOnMoveCount;
	int m_iPillageOnVictoryCount;
	int m_iPillageResearchCount;
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

	int m_iExtraVisibilityRange;
	int m_iExtraMoves;
	int m_iExtraMoveDiscount;
	int m_iExtraAirRange;
	int m_iExtraIntercept;
	int m_iExtraEvasion;
	int m_iExtraFirstStrikes;
	int m_iExtraChanceFirstStrikes;
	int m_iExtraWithdrawal;
	//TB Combat Mods Begin
	int m_iExtraAttackCombatModifier;
	int m_iExtraDefenseCombatModifier;
	int m_iExtraPursuit;
	int m_iExtraEarlyWithdraw;
	int m_iExtraVSBarbs;
	int m_iExtraReligiousCombatModifier;
	int m_iExtraArmor;
	int m_iExtraPuncture;
	int m_iExtraOverrun;
	int m_iExtraRepel;
	int m_iExtraFortRepel;
	int m_iExtraRepelRetries;
	int m_iExtraUnyielding;
	int m_iExtraKnockback;
	int m_iExtraKnockbackRetries;
	int m_iStampedeCount;
	int m_iAttackOnlyCitiesCount;
	int m_iIgnoreNoEntryLevelCount;
	int m_iIgnoreZoneofControlCount;
	int m_iFliesToMoveCount;
	int m_iExtraStrAdjperRnd;
	int m_iExtraStrAdjperAtt;
	int m_iExtraStrAdjperDef;
	int m_iExtraWithdrawAdjperAtt;
	int m_iExtraUnnerve;
	int m_iExtraEnclose;
	int m_iExtraLunge;
	int m_iExtraDynamicDefense;
	int m_iExtraStrength;
	int m_iSMStrength;
	int m_iAnimalIgnoresBordersCount;
	int m_iOnslaughtCount;
	int m_iExtraFortitude;
#ifdef STRENGTH_IN_NUMBERS
	int m_iExtraFrontSupportPercent;
	int m_iExtraShortRangeSupportPercent;
	int m_iExtraMediumRangeSupportPercent;
	int m_iExtraLongRangeSupportPercent;
	int m_iExtraFlankSupportPercent;
	int m_iSupportCount;
	int m_iAttackFromPlotX;
	int m_iAttackFromPlotY;
#endif // STRENGTH_IN_NUMBERS
	int m_iExtraSelfHealModifier;
	int m_iExtraNumHealSupport;
	int m_iHealSupportUsed;
	int m_iNoSelfHealCount;
	int m_iExcileCount;
	int m_iPassageCount;
	int m_iNoNonOwnedCityEntryCount;
	int m_iBarbCoExistCount;
	int m_iBlendIntoCityCount;
	int m_iUpgradeAnywhereCount;
	int m_iExtraInsidiousness;
	int m_iExtraInvestigation;
	int m_iDebugCount;
	int m_iAssassinCount;
	int m_iExtraStealthStrikes;
	int m_iExtraStealthCombatModifier;
	int m_iStealthDefenseCount;
	int m_iOnlyDefensiveCount;
	int m_iNoInvisibilityCount;
	int m_iNoCaptureCount;
	int m_iExtraNoDefensiveBonusCount;
	int m_iExtraGatherHerdCount;
	int m_iExtraTrapDamageMax;
	int m_iExtraTrapDamageMin;
	int m_iExtraTrapComplexity;
	int m_iExtraNumTriggers;
	int m_iNumTimesTriggered;
	int m_iTriggerBeforeAttackCount;
	int m_iHiddenNationalityCount;
	bool m_bIsArmed;
	bool m_bHasAnyInvisibility;
	bool m_bRevealed;
#ifdef STRENGTH_IN_NUMBERS
	IDInfo afIUnit;
	IDInfo afIIUnit;
	IDInfo asrIUnit;
	IDInfo asrIIUnit;
	IDInfo amrIUnit;
	IDInfo amrIIUnit;
	IDInfo alrIUnit;
	IDInfo alrIIUnit;
	IDInfo aflIUnit;
	IDInfo aflIIUnit;
	IDInfo dfIUnit;
	IDInfo dfIIUnit;
	IDInfo dsrIUnit;
	IDInfo dsrIIUnit;
	IDInfo dmrIUnit;
	IDInfo dmrIIUnit;
	IDInfo dlrIUnit;
	IDInfo dlrIIUnit;
	IDInfo dflIUnit;
	IDInfo dflIIUnit;
#endif // STRENGTH_IN_NUMBERS
	int m_iExtraDodgeModifier;
	int m_iExtraPrecisionModifier;
	int m_iExtraPowerShots;
	int m_iExtraPowerShotCombatModifier;
	int m_iExtraPowerShotPunctureModifier;
	int m_iExtraPowerShotPrecisionModifier;
	int m_iExtraPowerShotCriticalModifier;
	int m_iExtraCriticalModifier;
	int m_iExtraEndurance;
	int m_iColdDamage;
	int m_iDealColdDamageCount;
	int m_iColdImmuneCount;
	int m_iCombatPowerShots;
	int m_iCombatKnockbacks;
	int m_iCombatRepels;
	int m_iExtraRoundStunProb;
	int m_iCombatStuns;
	int m_iExtraPoisonProbabilityModifier;
	int m_iRetrainsAvailable;
	int m_iQualityBaseTotal;
	int m_iGroupBaseTotal;
	int m_iSizeBaseTotal;
	int m_iExtraQuality;
	int m_iExtraGroup;
	int m_iExtraSize;
	int m_iCannotMergeSplitCount;
	int m_iSMCargoVolume;
	int m_iSMExtraCargoVolume;
	int m_iSMCargoVolumeModifier;

	int m_iExtraCaptureProbabilityModifier;
	int m_iExtraCaptureResistanceModifier;

	int m_iExtraBreakdownChance;
	int m_iExtraBreakdownDamage;
	int m_iExtraTaunt;
	int m_iExtraCombatModifierPerSizeMore;
	int m_iExtraCombatModifierPerSizeLess;
	int m_iExtraCombatModifierPerVolumeMore;
	int m_iExtraCombatModifierPerVolumeLess;
	int m_iExtraMaxHP;
	int m_iExtraStrengthModifier;
	int m_iExtraDamageModifier;

	int m_iExtraUpkeep100;
	int m_iUpkeepModifier;
	int m_iUpkeepMultiplierSM;
	int m_iUpkeep100;

	int m_iSMAssetValue;
	int m_iSMPowerValue;
	int m_iSMHPValue;
	int m_iExtraRBombardDamage;
	int m_iExtraRBombardDamageLimit;
	int m_iExtraRBombardDamageMaxUnits;
	int m_iExtraDCMBombRange;
	int m_iExtraDCMBombAccuracy;
	int m_iBaseRBombardDamage;
	int m_iBaseRBombardDamageLimit;
	int m_iBaseRBombardDamageMaxUnits;
	int m_iBaseDCMBombRange;
	int m_iBaseDCMBombAccuracy;
	int m_iBombardDirectCount;
	//TB Combat Mods End
	int m_iExtraCollateralDamage;
	int m_iExtraBombardRate;
	int m_iSMBombardRate;
	int m_iSMAirBombBaseRate;
	int m_iSMBaseWorkRate;
	int m_iSMRevoltProtection;
	int m_iExtraEnemyHeal;
	int m_iExtraNeutralHeal;
	int m_iExtraFriendlyHeal;
	int m_iSameTileHeal;
	int m_iAdjacentTileHeal;
	int m_iExtraCombatPercent;
	int m_iExtraCityAttackPercent;
	int m_iExtraCityDefensePercent;
	int m_iExtraHillsAttackPercent;
	int m_iExtraHillsDefensePercent;

	int m_iRevoltProtection;
	int m_iCollateralDamageProtection;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iKamikazePercent;
	int m_iBaseCombat;
	DirectionTypes m_eFacingDirection;
	int m_iImmobileTimer;

	bool m_bCanRespawn;
	bool m_bSurvivor;

	bool m_bMadeAttack;
	//TB Combat Mods (Att&DefCounters)
	int m_iRoundCount;
	int m_iAttackCount;
	int m_iDefenseCount;
	//TB Combat Mods end
	bool m_bMadeInterception;
	bool m_bPromotionReady;
	bool m_bDeathDelay;
	bool m_bInfoBarDirty;
	bool m_bBlockading;
	bool m_bAirCombat;
	bool m_bHasBuildUp;
	bool m_bInhibitMerge;
	bool m_bInhibitSplit;
	bool m_bIsBuildUp;
	bool m_bIsReligionLocked;

	PlayerTypes m_eOwner;
	PlayerTypes m_eCapturingPlayer;
	IDInfo m_eCapturingUnit;
	UnitTypes m_eUnitType;
	UnitTypes m_eLeaderUnitType;
	UnitTypes m_eGGExperienceEarnedTowardsType;
	CvUnitInfo *m_pUnitInfo;
	ReligionTypes m_eReligionType;

	IDInfo m_combatUnit;
	IDInfo m_transportUnit;

	int* m_aiExtraDomainModifier;

	int* m_aiExtraVisibilityIntensity;
	int* m_aiExtraInvisibilityIntensity;
	int* m_aiExtraVisibilityIntensityRange;
	int* m_aiNegatesInvisibleCount;
	int* m_aiExtraVisibilityIntensitySameTile;

	CvWString m_szName;
	CvString m_szScriptData;

	mutable int	m_maxMoveCache;
	mutable int	m_iMaxMoveCacheTurn;

	mutable std::map<PromotionTypes, PromotionKeyedInfo>	m_promotionKeyedInfo;
	mutable std::map<PromotionLineTypes, PromotionLineKeyedInfo>	m_promotionLineKeyedInfo;
	mutable std::map<TerrainTypes, TerrainKeyedInfo>		m_terrainKeyedInfo;
	mutable std::map<FeatureTypes, FeatureKeyedInfo>		m_featureKeyedInfo;
	mutable std::map<UnitCombatTypes, UnitCombatKeyedInfo>	m_unitCombatKeyedInfo;

	bool canAdvance(const CvPlot* pPlot, int iThreshold) const;
	void collateralCombat(const CvPlot* pPlot, CvUnit* pSkipUnit = NULL);
	void rBombardCombat(const CvPlot* pPlot, CvUnit* pFirstUnit = NULL);
	void flankingStrikeCombat(const CvPlot* pPlot, int iAttackerStrength, int iAttackerFirepower, int iDefenderOdds, int iDefenderDamage, CvUnit* pSkipUnit);

	bool interceptTest(const CvPlot* pPlot);
	CvUnit* airStrikeTarget(const CvPlot* pPlot) const;

	// Dale - SA: Stack Attack
public:
	bool canAirStrike(const CvPlot* pPlot) const;
	bool airStrike(CvPlot* pPlot);
protected:
	// ! Dale

	int planBattle( CvBattleDefinition & kBattleDefinition ) const;
	int computeUnitsToDie( const CvBattleDefinition & kDefinition, bool bRanged, BattleUnitTypes iUnit ) const;
	bool verifyRoundsValid( const CvBattleDefinition & battleDefinition ) const;
	void increaseBattleRounds( CvBattleDefinition & battleDefinition ) const;
	int computeWaveSize( bool bRangedRound, int iAttackerMax, int iDefenderMax ) const;

	void getDefenderCombatValues(const CvUnit& kDefender, const CvPlot* pPlot, int iOurStrength, int iOurFirepower, int& iTheirOdds,
		int& iTheirStrength, int& iOurDamage, int& iTheirDamage, CombatDetails* pTheirDetails, const CvUnit* pDefender) const;

	bool isCombatVisible(const CvUnit* pDefender) const;
	void resolveCombat(CvUnit* pDefender, CvPlot* pPlot, CvBattleDefinition& kBattle, bool bSamePlot = false);
	void resolveAirCombat(CvUnit* pInterceptor, CvPlot* pPlot, CvAirMissionDefinition& kBattle);

	// ------ BEGIN InfluenceDrivenWar -------------------------------
	int doVictoryInfluence(CvUnit* pLoserUnit, bool bAttacking, bool bWithdrawal);
	void influencePlots(CvPlot* pCentralPlot, const PlayerTypes eTargetPlayer, const int iLocationMultiplier);
	int doPillageInfluence();
	// ------ END InfluenceDrivenWar ---------------------------------

	const PromotionKeyedInfo*	findPromotionKeyedInfo(PromotionTypes ePromotion) const;
	PromotionKeyedInfo*	findOrCreatePromotionKeyedInfo(PromotionTypes ePromotion, bool bCreate = true);
	const PromotionLineKeyedInfo*	findPromotionLineKeyedInfo(PromotionLineTypes ePromotionLine) const;
	PromotionLineKeyedInfo*	findOrCreatePromotionLineKeyedInfo(PromotionLineTypes ePromotionLine, bool bCreate = true);
	const TerrainKeyedInfo*	findTerrainKeyedInfo(TerrainTypes eTerrain) const;
	TerrainKeyedInfo*	findOrCreateTerrainKeyedInfo(TerrainTypes eTerrain, bool bCreate = true);
	const FeatureKeyedInfo*	findFeatureKeyedInfo(FeatureTypes eFeature) const;
	FeatureKeyedInfo*	findOrCreateFeatureKeyedInfo(FeatureTypes eFeature, bool bCreate = true);
	const UnitCombatKeyedInfo*	findUnitCombatKeyedInfo(UnitCombatTypes eUnitCombat) const;
	UnitCombatKeyedInfo*	findOrCreateUnitCombatKeyedInfo(UnitCombatTypes eUnitCombat, bool bCreate = true);

	// AIAndy: Properties
	CvProperties m_Properties;
public:
	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;

	PromotionIterator getPromotionBegin();
	PromotionIterator getPromotionEnd();

	//	KOSHLING - inform the AI of unit losses so that it can adjust internal counts
	virtual void AI_killed() = 0;
	//	Set an override to the group head priority for this unit, to allow a unit that
	//	would not nomally lead a stack to do so - priority will be preserved when other groups
	//	joing one lead by a unit with an override, but reset if we join another group
	virtual void AI_setLeaderPriority(int iPriority) = 0; //	 -1 means reset to default
	virtual int AI_getPredictedHitPoints() const = 0;
	virtual void AI_setPredictedHitPoints(int iPredictedHitPoints) = 0;
	virtual bool AI_getHasAttacked() const = 0;
	virtual int AI_beneficialPropertyValueToCity(const CvCity* pCity, PropertyTypes eProperty) const = 0;

	bool isUsingDummyEntities() const;
	static bool isDummyEntity(const CvEntity* entity);
	static bool isRealEntity(const CvEntity* entity);

	std::map<UnitCombatTypes, UnitCombatKeyedInfo>&  getUnitCombatKeyedInfo() const;
	const std::map<PromotionTypes, PromotionKeyedInfo>& getPromotionKeyedInfo() const;
	std::map<PromotionLineTypes, PromotionLineKeyedInfo>&  getPromotionLineKeyedInfo() const;

private:
	bool	bGraphicsSetup;
	int m_iXOrigin;
	int m_iYOrigin;
	void doStarsign();

	//TB Combat Mods begin
public:
	bool isArcher() const;
	bool isPromotionOverriden(PromotionTypes ePromotionType) const;
#ifdef OUTBREAKS_AND_AFFLICTIONS
	bool canCure(const CvPlot* pPlot, PromotionLineTypes eAfflictionLine) const;
	bool CureAffliction(PromotionLineTypes eAfflictionLine);
	int getTotalCommunicableExposure(PromotionLineTypes eAfflictionLine) const;
	int getUnitCommunicability(PromotionLineTypes eAfflictionLine) const;
	int getCityCommunicability(PromotionLineTypes eAfflictionLine) const;
	void doOvercomeAttempt(PromotionLineTypes eAfflictionLine);
	int getChancetoOvercome(PromotionLineTypes eAfflictionLine) const;
	int currentOvercome(PromotionLineTypes eAfflictionLine) const;
	int getAfflictionTurnCount(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictionTurnCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setAfflictionTurnCount(PromotionLineTypes ePromotionLineType, int iChange);
	int aidVolume(const CvPlot* pPlot, PropertyTypes eProperty) const;
	int getAfflictionHitCount(PromotionTypes ePromotionType) const;
	void changeAfflictionHitCount(PromotionTypes ePromotionType, int iChange);
	void setAfflictionHitCount(PromotionTypes ePromotionType, int iChange);
	bool removeAfflictionHits(PromotionTypes ePromotionType);
	bool updateAfflictionHits(PromotionTypes ePromotionType);
	bool checkContractDisease(PromotionLineTypes eAfflictionLine, int iCommunicableExposure = 0);
	int getChancetoContract(PromotionLineTypes eAfflictionLine, int iCommunicableExposure = 0) const;
	int getUnitAfflictionTolerance(PromotionLineTypes ePromotionLineType) const;
	void changeUnitAfflictionTolerance(PromotionLineTypes ePromotionLineType, int iChange);
	void setUnitAfflictionTolerance(PromotionLineTypes ePromotionLineType, int iChange);
	int getContractChanceChange(PromotionLineTypes eAfflictionLine) const;
	int getOvercomeChange(PromotionLineTypes eAfflictionLine) const;
	int getFortitudeModifierTypeAmount(PromotionLineTypes ePromotionLineType) const;
	void changeFortitudeModifierTypeAmount(PromotionLineTypes ePromotionLineType, int iChange);
	void setFortitudeModifierTypeAmount(PromotionLineTypes ePromotionLineType, int iChange);
#endif
	int getCityRepel() const;

#ifdef STRENGTH_IN_NUMBERS
	int getCityFrontSupportPercentModifier() const;
	int getCityShortRangeSupportPercentModifier() const;
	int getCityMediumRangeSupportPercentModifier() const;
	int getCityLongRangeSupportPercentModifier() const;
	int getCityFlankSupportPercentModifier() const;

	int getExtraFrontSupportPercent (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraFrontSupportPercent (int iChange);

	int getExtraShortRangeSupportPercent (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraShortRangeSupportPercent (int iChange);

	int getExtraMediumRangeSupportPercent (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraMediumRangeSupportPercent (int iChange);

	int getExtraLongRangeSupportPercent (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraLongRangeSupportPercent (int iChange);

	int getExtraFlankSupportPercent (bool bIgnoreCommanders = false, bool bIgnoreCommodores = false) const;
	void changeExtraFlankSupportPercent (int iChange);

	int frontSupportPercentTotal() const;
	int shortRangeSupportPercentTotal() const;
	int mediumRangeSupportPercentTotal() const;
	int longRangeSupportPercentTotal() const;
	int flankSupportPercentTotal() const;
	bool isFrontSupporter() const;
	bool isShortRangeSupporter() const;
	bool isMediumRangeSupporter() const;
	bool isLongRangeSupporter() const;
	bool isFlankSupporter() const;

	CvPlot* getAttackFromPlot() const;
	void setAttackFromPlot(const CvPlot* pNewValue);

	int getAttackerSupportValue() const;
	int getAttackerFirstFrontSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerSecondFrontSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerFirstShortRangeSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerSecondShortRangeSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerFirstMediumRangeSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerSecondMediumRangeSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerFirstLongRangeSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerSecondLongRangeSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerFirstFlankSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	int getAttackerSecondFlankSupportValue(const CvPlot* aPlot, const CvPlot* pPlot, const CvUnit* pDefender) const;
	CvUnit* getAttackerFirstFrontSupportingUnit() const;
	CvUnit* getAttackerSecondFrontSupportingUnit() const;
	CvUnit* getAttackerFirstShortRangeSupportingUnit() const;
	CvUnit* getAttackerSecondShortRangeSupportingUnit() const;
	CvUnit* getAttackerFirstMediumRangeSupportingUnit() const;
	CvUnit* getAttackerSecondMediumRangeSupportingUnit() const;
	CvUnit* getAttackerFirstLongRangeSupportingUnit() const;
	CvUnit* getAttackerSecondLongRangeSupportingUnit() const;
	CvUnit* getAttackerFirstFlankSupportingUnit() const;
	CvUnit* getAttackerSecondFlankSupportingUnit() const;
	void setAttackerFirstFrontSupportingUnit(CvUnit* pBestUnit);
	void setAttackerSecondFrontSupportingUnit(CvUnit* pBestUnit);
	void setAttackerFirstShortRangeSupportingUnit(CvUnit* pBestUnit);
	void setAttackerSecondShortRangeSupportingUnit(CvUnit* pBestUnit);
	void setAttackerFirstMediumRangeSupportingUnit(CvUnit* pBestUnit);
	void setAttackerSecondMediumRangeSupportingUnit(CvUnit* pBestUnit);
	void setAttackerFirstLongRangeSupportingUnit(CvUnit* pBestUnit);
	void setAttackerSecondLongRangeSupportingUnit(CvUnit* pBestUnit);
	void setAttackerFirstFlankSupportingUnit(CvUnit* pBestUnit);
	void setAttackerSecondFlankSupportingUnit(CvUnit* pBestUnit);

	int getDefenderSupportValue(const CvUnit* pAttacker) const;
	int getDefenderFirstFrontSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderSecondFrontSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderFirstShortRangeSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderSecondShortRangeSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderFirstMediumRangeSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderSecondMediumRangeSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderFirstLongRangeSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderSecondLongRangeSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderFirstFlankSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	int getDefenderSecondFlankSupportValue(const CvUnit* pAttacker, const CvPlot* pPlot) const;
	CvUnit* getDefenderFirstFrontSupportingUnit() const;
	CvUnit* getDefenderSecondFrontSupportingUnit() const;
	CvUnit* getDefenderFirstShortRangeSupportingUnit() const;
	CvUnit* getDefenderSecondShortRangeSupportingUnit() const;
	CvUnit* getDefenderFirstMediumRangeSupportingUnit() const;
	CvUnit* getDefenderSecondMediumRangeSupportingUnit() const;
	CvUnit* getDefenderFirstLongRangeSupportingUnit() const;
	CvUnit* getDefenderSecondLongRangeSupportingUnit() const;
	CvUnit* getDefenderFirstFlankSupportingUnit() const;
	CvUnit* getDefenderSecondFlankSupportingUnit() const;
	void setDefenderFirstFrontSupportingUnit(CvUnit* pBestUnit);
	void setDefenderSecondFrontSupportingUnit(CvUnit* pBestUnit);
	void setDefenderFirstShortRangeSupportingUnit(CvUnit* pBestUnit);
	void setDefenderSecondShortRangeSupportingUnit(CvUnit* pBestUnit);
	void setDefenderFirstMediumRangeSupportingUnit(CvUnit* pBestUnit);
	void setDefenderSecondMediumRangeSupportingUnit(CvUnit* pBestUnit);
	void setDefenderFirstLongRangeSupportingUnit(CvUnit* pBestUnit);
	void setDefenderSecondLongRangeSupportingUnit(CvUnit* pBestUnit);
	void setDefenderFirstFlankSupportingUnit(CvUnit* pBestUnit);
	void setDefenderSecondFlankSupportingUnit(CvUnit* pBestUnit);

	bool isSupporting();
	void setSupportCount(int iChange);
	void ClearSupports();
#endif

	void checkPromotionObsoletion();
	void processPromotion(PromotionTypes eIndex, bool bAdding, bool bInitial = false);

	int getColdDamage() const;
	void changeColdDamage(int iChange);
	void setColdDamage(int iChange);

	int getCombatPowerShots() const;
	void setCombatPowerShots(int iNewValue);
	void changeCombatPowerShots(int iChange);

	int getCombatKnockbacks() const;
	void setCombatKnockbacks(int iNewValue);
	void changeCombatKnockbacks(int iChange);

	int getCombatRepels() const;
	void setCombatRepels(int iNewValue);
	void changeCombatRepels(int iChange);

#ifdef OUTBREAKS_AND_AFFLICTIONS
	void checkForCritical(int iDamage, CvUnit* pOpponent);
	void assignCritical(CvUnit* pOpponent);
#endif
	bool canKeepPromotion(PromotionTypes ePromotion, bool bAssertFree = false, bool bMessageOnFalse = false) const;
	bool isPromotionFree(PromotionTypes ePromotion) const;
	int getPromotionFreeCount(PromotionTypes ePromotion) const;
	void setPromotionFreeCount(PromotionTypes ePromotion, int iChange);
	void checkFreetoCombatClass();
	//TB Combat Mods end
	bool meetsUnitSelectionCriteria(const CvUnitSelectionCriteria* criteria) const;
	bool shouldUseWithdrawalOddsCap() const;
	bool isPursuitinUse() const;
	bool canSwitchEquipment(PromotionTypes eEquipment) const;
	void reEquip(PromotionTypes eEquipment);
	void statusUpdate(PromotionTypes eStatus);

	int flankingStrengthbyUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraFlankingStrengthbyUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraFlankingStrengthbyUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int withdrawOnTerrainTotal(TerrainTypes eTerrainType) const;
	int getExtraWithdrawOnTerrainType(TerrainTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraWithdrawOnTerrainType(TerrainTypes eIndex, int iChange);

	int withdrawOnFeatureTotal(FeatureTypes eFeatureType) const;
	int getExtraWithdrawOnFeatureType(FeatureTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraWithdrawOnFeatureType(FeatureTypes eIndex, int iChange);

	int withdrawVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraWithdrawVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraWithdrawVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int pursuitVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraPursuitVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraPursuitVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int repelVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraRepelVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraRepelVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int knockbackVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraKnockbackVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraKnockbackVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int punctureVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraPunctureVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraPunctureVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int armorVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraArmorVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraArmorVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int dodgeVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraDodgeVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraDodgeVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int precisionVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraPrecisionVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraPrecisionVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int criticalVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraCriticalVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraCriticalVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int roundStunVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraRoundStunVSUnitCombatType(UnitCombatTypes eIndex, const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraRoundStunVSUnitCombatType(UnitCombatTypes eIndex, int iChange);

	int getExtraRoundStunProb(const bool bCommander = true, const bool bCommodore = true) const;
	void changeExtraRoundStunProb(int iChange);
	int roundStunProbTotal() const;

	int roundStunVSOpponentProbTotal(CvUnit* pOpponent) const;

	int getCombatStuns() const;
	void setCombatStuns(int iNewValue);
	void changeCombatStuns(int iChange);
	void checkForStun(int iDamage, CvUnit* pOpponent);

	int withdrawVSOpponentProbTotal(const CvUnit* pOpponent, const CvPlot* pPlot) const;
	int pursuitVSOpponentProbTotal(const CvUnit* pOpponent) const;
	int repelVSOpponentProbTotal(const CvUnit* pOpponent) const;
	int knockbackVSOpponentProbTotal(const CvUnit* pOpponent) const;
	int punctureVSOpponentProbTotal(const CvUnit* pOpponent) const;
	int armorVSOpponentProbTotal(const CvUnit* pOpponent) const;
	int dodgeVSOpponentProbTotal(const CvUnit* pOpponent) const;
	int precisionVSOpponentProbTotal(const CvUnit* pOpponent) const;
	int criticalVSOpponentProbTotal(const CvUnit* pOpponent) const;

	bool canInflictCritical(PromotionTypes eCritical) const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getAfflictionLineCount(PromotionLineTypes ePromotionLineType) const;
	bool hasAfflictionLine(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictionLineCount(PromotionLineTypes ePromotionLineType, int iChange);

	void afflict(PromotionLineTypes eAfflictionLine, bool bPoisoned = false, CvUnit* pOpponent = NULL, bool bTrap = false, int iTrapDmg = 0);
	void recover(PromotionLineTypes eAfflictionLine);

	int getAfflictOnAttackTypeProbability(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictOnAttackTypeProbability(PromotionLineTypes ePromotionLineType, int iChange);
	void setAfflictOnAttackTypeProbability(PromotionLineTypes ePromotionLineType, int iChange);

	int getAfflictOnAttackTypeCount(PromotionLineTypes ePromotionLineType) const;
	bool hasAfflictOnAttackType(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictOnAttackTypeCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setAfflictOnAttackTypeCount(PromotionLineTypes ePromotionLineType, int iChange);

	int getAfflictOnAttackTypeImmediateCount(PromotionLineTypes ePromotionLineType) const;
	bool isAfflictOnAttackTypeImmediate(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictOnAttackTypeImmediateCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setAfflictOnAttackTypeImmediateCount(PromotionLineTypes ePromotionLineType, int iChange);

	int getAfflictOnAttackTypeMeleeCount(PromotionLineTypes ePromotionLineType) const;
	bool isAfflictOnAttackTypeMelee(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictOnAttackTypeMeleeCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setAfflictOnAttackTypeMeleeCount(PromotionLineTypes ePromotionLineType, int iChange);

	int getAfflictOnAttackTypeDistanceCount(PromotionLineTypes ePromotionLineType) const;
	bool isAfflictOnAttackTypeDistance(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictOnAttackTypeDistanceCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setAfflictOnAttackTypeDistanceCount(PromotionLineTypes ePromotionLineType, int iChange);

	int getAfflictOnAttackTypeAttemptedCount(PromotionLineTypes ePromotionLineType) const;
	bool isAfflictOnAttackTypeAttempted(PromotionLineTypes ePromotionLineType) const;
	void changeAfflictOnAttackTypeAttemptedCount(PromotionLineTypes ePromotionLineType, int iChange);
	void setAfflictOnAttackTypeAttemptedCount(PromotionLineTypes ePromotionLineType, int iChange);

	int worsenedProbabilitytoAfflict(PromotionLineTypes eAfflictionLine) const;
#endif // OUTBREAKS_AND_AFFLICTIONS

	int getHealUnitCombatCount() const;
	int getHealUnitCombatTypeTotal(UnitCombatTypes eUnitCombatType) const;
	void changeHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombatType, int iChange);
	int getHealUnitCombatTypeAdjacentTotal(UnitCombatTypes eUnitCombatType) const;
	void changeHealUnitCombatTypeAdjacentVolume(UnitCombatTypes eUnitCombatType, int iChange);

	void doSetUnitCombats();
	void doSetFreePromotions(bool bAdding, TraitTypes eTrait = NO_TRAIT);
	void setFreePromotion(PromotionTypes ePromotion, bool bAdding, TraitTypes eTrait = NO_TRAIT);

	int getRetrainsAvailable() const;
	void setRetrainsAvailable(int iNewValue);
	void changeRetrainsAvailable(int iChange);

	int getExperiencefromWithdrawal(const int iWithdrawalProbability) const;

	void changeExtraCaptureProbabilityModifier(int iChange);
	int captureProbabilityTotal() const;

	void changeExtraCaptureResistanceModifier(int iChange);
	int captureResistanceTotal() const;

	void changeExtraBreakdownChance(int iChange);
	int breakdownChanceTotal() const;

	void changeExtraBreakdownDamage(int iChange);
	int breakdownDamageTotal() const;

	void changeExtraTaunt(int iChange);
	int tauntTotal() const;

	int getExtraCombatModifierPerSizeMore() const;
	void changeExtraCombatModifierPerSizeMore(int iChange);
	void setExtraCombatModifierPerSizeMore(int iChange);
	int combatModifierPerSizeMoreTotal() const;

	int getExtraCombatModifierPerSizeLess() const;
	void changeExtraCombatModifierPerSizeLess(int iChange);
	void setExtraCombatModifierPerSizeLess(int iChange);
	int combatModifierPerSizeLessTotal() const;

	int getExtraCombatModifierPerVolumeMore() const;
	void changeExtraCombatModifierPerVolumeMore(int iChange);
	void setExtraCombatModifierPerVolumeMore(int iChange);
	int combatModifierPerVolumeMoreTotal() const;

	int getExtraCombatModifierPerVolumeLess() const;
	void changeExtraCombatModifierPerVolumeLess(int iChange);
	void setExtraCombatModifierPerVolumeLess(int iChange);
	int combatModifierPerVolumeLessTotal() const;

	int getExtraStrengthModifier() const;
	void changeExtraStrengthModifier(int iChange);
	void setExtraStrengthModifier(int iChange);

	void checkCityAttackDefensesDamage(CvCity* pCity, const std::vector<UnitCombatTypes>& kDamagableUnitCombatTypes);

	bool isBreakdownCombat(const CvPlot* pPlot, bool bSamePlot = false) const;
	void resolveBreakdownAttack(const CvPlot* pPlot, const int AdjustedRepel);

	int getDiminishingReturn(int i) const;

	bool isPursuitPossible(const CvUnit* pOpponent) const;

	bool hasCannotMergeSplit() const;
	int getCannotMergeSplitCount() const;
	void changeCannotMergeSplitCount(int iNewValue);

	int getQualityBaseTotal() const;
	void setQualityBaseTotal(int iNewValue);

	int getGroupBaseTotal() const;
	void setGroupBaseTotal(int iNewValue);

	int getSizeBaseTotal() const;
	void setSizeBaseTotal(int iNewValue);

	int getExtraQuality() const;
	void changeExtraQuality(int iChange);

	int getExtraGroup() const;
	void changeExtraGroup(int iChange);

	int getExtraSize() const;
	void changeExtraSize(int iChange);

	int qualityRank() const;
	int groupRank() const;
	int sizeRank() const;

	// Functions for evaluating upgrades when splitting and merging
	bool isGroupUpgradePromotion(PromotionTypes promotion) const;
	bool isGroupDowngradePromotion(PromotionTypes promotion) const;
	bool isQualityUpgradePromotion(PromotionTypes promotion) const;
	bool isQualityDowngradePromotion(PromotionTypes promotion) const;

	bool canMerge(bool bAutocheck = false) const;
	bool canSplit() const;

	void doMerge();
	void doSplit();

	void setGGExperienceEarnedTowardsType();
	UnitTypes getGGExperienceEarnedTowardsType() const;

	int eraGroupMergeLimit() const;
	int eraGroupSplitLimit() const;

	DomainTypes getDomainCargo() const;
	void setNewDomainCargo(DomainTypes eDomain);
	SpecialUnitTypes getSpecialCargo() const;
	void setNewSpecialCargo(SpecialUnitTypes eSpecialUnit);
	SpecialUnitTypes getSMSpecialCargo() const;
	SpecialUnitTypes getSMNotSpecialCargo() const;
	void setNewSMNotSpecialCargo(SpecialUnitTypes eSpecialUnit);

	void changeSMCargoSpace(int iChange);
	int SMcargoSpaceFilter() const;
	int SMcargoCapacityPreCheck() const;
	int getSMCargoCapacity() const;
	void setSMCargoCapacity();

	int getExtraMaxHP() const;
	void changeExtraMaxHP(int iChange);
	int getMaxHP() const;
	int getHP() const;
	int HPValueTotalPreCheck() const;
	int getSMHPValue() const;
	void setSMHPValue();

	int getPowerValueTotal() const;
	void setSMPowerValue(bool bForLoad = false);

	int assetValueTotal() const;
	void setSMAssetValue(bool bForLoad = false);

	int getCargoVolumeModifier() const;
	void setCargoVolumeModifier(int iNewValue);
	void changeCargoVolumeModifier(int iChange);

	int getExtraCargoVolume() const;
	void setExtraCargoVolume(int iNewValue);
	void changeExtraCargoVolume(int iChange);
	int getSMCargoVolumeBase() const;
	int getCargoVolume() const;
	void setCargoVolume(int iNewValue);
	int SMCargoVolume() const;
	void setSMCargoVolume();


	void changeCargoVolume(int iChange);

	int getSizeMattersOffsetValue() const;
	int getSizeMattersSpacialOffsetValue() const;
	int getCargoCapacitybyType(int iValue) const;
	bool isCarrier() const;
	bool isUnitAtBaseGroup() const;
	bool isUnitAboveBaseGroup() const;
	bool isUnitBelowBaseGroup() const;

	//int getExtraGeneric() const;
	//void changeExtraGeneric(int iChange);
	//void setExtraGeneric(int iChange);
	//int genericTotal();
	//int genericTotalPreCheck();
	//int getSMGeneric();
	//void setSMGeneric();

	int getExtraBombardRate() const;
	void changeExtraBombardRate(int iChange);
	int getBombardRate() const;
	void setSMBombardRate();

	int getAirBombCurrRate() const;
	int getAirBombBaseRate() const;
	int getSMAirBombBaseRateTotalBase() const;
	int getSMAirBombBaseRate() const;
	void setSMAirBombBaseRate();

	int workRate(bool bMax) const;
	int baseWorkRate() const;
	int baseWorkRatePreCheck() const;
	int getSMBaseWorkRate() const;
	void setSMBaseWorkRate();

	int getRevoltProtection() const;
	void changeRevoltProtection(int iChange);
	int revoltProtectionTotal() const;
	//int getSMRevoltProtection() const;
	//void setSMRevoltProtection();

	bool canPerformActionSM() const;
	void setSMValues(bool bForLoad = false);

	int rBombardDamage() const;
	int getExtraRBombardDamage() const;
	void changeExtraRBombardDamage(int iChange);
	int getBaseRBombardDamage() const;
	void changeBaseRBombardDamage(int iChange, bool bAdding, UnitCombatTypes eUnitCombat);

	int rBombardDamageLimit() const;
	int getExtraRBombardDamageLimit() const;
	void changeExtraRBombardDamageLimit(int iChange);
	int getBaseRBombardDamageLimit() const;
	void changeBaseRBombardDamageLimit(int iChange, bool bAdding, UnitCombatTypes eUnitCombat);

	int rBombardDamageMaxUnits() const;
	int getExtraRBombardDamageMaxUnits() const;
	void changeExtraRBombardDamageMaxUnits(int iChange);
	int getBaseRBombardDamageMaxUnits() const;
	void changeBaseRBombardDamageMaxUnits(int iChange, bool bAdding, UnitCombatTypes eUnitCombat);

	int getDCMBombRange() const;
	int getExtraDCMBombRange() const;
	void changeExtraDCMBombRange(int iChange);
	int getBaseDCMBombRange() const;
	void changeBaseDCMBombRange(int iChange, bool bAdding, UnitCombatTypes eUnitCombat);

	int getDCMBombAccuracy() const;
	int getExtraDCMBombAccuracy() const;
	void changeExtraDCMBombAccuracy(int iChange);
	int getBaseDCMBombAccuracy() const;
	void changeBaseDCMBombAccuracy(int iChange, bool bAdding, UnitCombatTypes eUnitCombat);
	bool isRBombardDirect() const;
	void changeBombardDirectCount(int iChange);

	int getNoSelfHealCount() const;
	bool hasNoSelfHeal() const;
	void changeNoSelfHealCount(int iChange);

	int getSelfHealModifierTotal() const;
	void changeExtraSelfHealModifier(int iChange);

	int getNumHealSupportTotal() const;
	void changeExtraNumHealSupport(int iChange);

	int getHealSupportUsedTotal() const;
	void changeHealSupportUsed(int iChange);
	void setHealSupportUsed(int iChange);
	int getHealSupportRemaining() const;
	bool hasHealSupportRemaining() const;

	MissionTypes getSleepType() const;
	void setSleepType(MissionTypes eSleepType);
	void establishBuildups();
	PromotionLineTypes getBuildUpType() const;
	void setBuildUpType(PromotionLineTypes ePromotionLine = NO_PROMOTIONLINE, MissionTypes eSleepType = NO_MISSION);
	void clearBuildups();
	void incrementBuildUp();
	bool isInhibitMerge() const;
	void setInhibitMerge(bool bNewValue);
	bool isInhibitSplit() const;
	void setInhibitSplit(bool bNewValue);
	bool isBuildUp() const;
	void setSpecialUnit(bool bChange, SpecialUnitTypes eSpecialUnit);
	bool isHiddenNationality() const;
	void doHNCapture();
	void removeHNCapturePromotion();

	void processLoadedSpecialUnit(bool bChange, SpecialUnitTypes eSpecialUnit);

	bool hasBuild(BuildTypes eBuild) const;
	void changeExtraBuildType(bool bChange, BuildTypes eBuild);

	bool isExcile() const;
	void changeExcileCount(int iChange);

	bool isPassage() const;
	void changePassageCount(int iChange);

	bool isNoNonOwnedCityEntry() const;
	void changeNoNonOwnedCityEntryCount(int iChange);

	bool isBarbCoExist() const;
	void changeBarbCoExistCount(int iChange);

	bool isBlendIntoCity() const;
	void changeBlendIntoCityCount(int iChange);

	bool isUpgradeAnywhere() const;
	void changeUpgradeAnywhereCount(int iChange);

	void updateSpotIntensity(const InvisibleTypes eInvisibleType = NO_INVISIBLE, const bool bSameTile = false);
	int visibilityIntensityTotal(InvisibleTypes eInvisibleType) const;
	int getExtraVisibilityIntensityType(InvisibleTypes eIndex) const;
	void changeExtraVisibilityIntensityType(InvisibleTypes eIndex, int iChange);

	bool hasAnyInvisibilityType() const;
	bool hasInvisibilityType(InvisibleTypes eInvisibleType) const;
	int invisibilityIntensityTotal(InvisibleTypes eInvisibleType) const;
	int getExtraInvisibilityIntensityType(InvisibleTypes eIndex) const;
	void changeExtraInvisibilityIntensityType(InvisibleTypes eIndex, int iChange);
	void setHasAnyInvisibility();

	//These need text displays on the unit
	int visibilityIntensityRangeTotal(InvisibleTypes eInvisibleType) const;
	int getExtraVisibilityIntensityRangeType(InvisibleTypes eIndex) const;
	void changeExtraVisibilityIntensityRangeType(InvisibleTypes eIndex, int iChange);

	int visibilityIntensitySameTileTotal(InvisibleTypes eInvisibleType) const;
	void changeExtraVisibilityIntensitySameTileType(InvisibleTypes eIndex, int iChange);

	int getNumExtraInvisibleTerrains() const;
	InvisibleTerrainChanges& getExtraInvisibleTerrain(int iIndex);
	void changeExtraInvisibleTerrain(InvisibleTypes eInvisible, TerrainTypes eTerrain, int iChange);
	int extraInvisibleTerrain(InvisibleTypes eInvisible, TerrainTypes eTerrain) const;

	int getNumExtraInvisibleFeatures() const;
	InvisibleFeatureChanges& getExtraInvisibleFeature(int iIndex);
	void changeExtraInvisibleFeature(InvisibleTypes eInvisible, FeatureTypes eFeature, int iChange);
	int extraInvisibleFeature(InvisibleTypes eInvisible, FeatureTypes eTFeature) const;

	int getNumExtraInvisibleImprovements() const;
	InvisibleImprovementChanges& getExtraInvisibleImprovement(int iIndex);
	void changeExtraInvisibleImprovement(InvisibleTypes eInvisible, ImprovementTypes eImprovement, int iChange);
	int extraInvisibleImprovement(InvisibleTypes eInvisible, ImprovementTypes eImprovement) const;

	int getNumExtraVisibleTerrains() const;
	InvisibleTerrainChanges& getExtraVisibleTerrain(int iIndex);
	void changeExtraVisibleTerrain(InvisibleTypes eInvisible, TerrainTypes eTerrain, int iChange);
	int extraVisibleTerrain(InvisibleTypes eInvisible, TerrainTypes eTerrain) const;

	int getNumExtraVisibleFeatures() const;
	InvisibleFeatureChanges& getExtraVisibleFeature(int iIndex);
	void changeExtraVisibleFeature(InvisibleTypes eInvisible, FeatureTypes eFeature, int iChange);
	int extraVisibleFeature(InvisibleTypes eInvisible, FeatureTypes eTFeature) const;

	int getNumExtraVisibleImprovements() const;
	InvisibleImprovementChanges& getExtraVisibleImprovement(int iIndex);
	void changeExtraVisibleImprovement(InvisibleTypes eInvisible, ImprovementTypes eImprovement, int iChange);
	int extraVisibleImprovement(InvisibleTypes eInvisible, ImprovementTypes eImprovement) const;

	int getNumExtraVisibleTerrainRanges() const;
	InvisibleTerrainChanges& getExtraVisibleTerrainRange(int iIndex);
	void changeExtraVisibleTerrainRange(InvisibleTypes eInvisible, TerrainTypes eTerrain, int iChange);
	int extraVisibleTerrainRange(InvisibleTypes eInvisible, TerrainTypes eTerrain) const;

	int getNumExtraVisibleFeatureRanges() const;
	InvisibleFeatureChanges& getExtraVisibleFeatureRange(int iIndex);
	void changeExtraVisibleFeatureRange(InvisibleTypes eInvisible, FeatureTypes eFeature, int iChange);
	int extraVisibleFeatureRange(InvisibleTypes eInvisible, FeatureTypes eTFeature) const;

	int getNumExtraVisibleImprovementRanges() const;
	InvisibleImprovementChanges& getExtraVisibleImprovementRange(int iIndex);
	void changeExtraVisibleImprovementRange(InvisibleTypes eInvisible, ImprovementTypes eImprovement, int iChange);
	int extraVisibleImprovementRange(InvisibleTypes eInvisible, ImprovementTypes eImprovement) const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumExtraAidChanges() const;
	AidStruct& getExtraAidChange(int iIndex);
	void changeExtraAidChange(PropertyTypes eProperty, int iChange);
	int extraAidChange(PropertyTypes eProperty) const;
#endif
	bool isNegatesInvisible(InvisibleTypes eInvisible) const;
	int getNegatesInvisibleCount(InvisibleTypes eInvisible) const;
	void changeNegatesInvisibleCount(InvisibleTypes eInvisible, int iChange);

	bool hasInvisibleAbility() const;
	bool isCriminal() const;

	int getInsidiousnessTotal(bool bCriminalCheck = false) const;
	void changeExtraInsidiousness(int iChange);
	int getInvestigationTotal() const;
	void changeExtraInvestigation(int iChange);

	bool criminalSuccessCheck();
	void doInsidiousnessVSInvestigationCheck();
	void doRemoveInvestigatedPromotionCheck();
	bool isWantedbyPlayer(PlayerTypes ePlayer) const;
	bool isWanted() const;

	void attackSamePlotSpecifiedUnit(CvUnit* pSelectedDefender);
	bool canArrest() const;
	void doArrest();

	bool canAmbush(const CvPlot* pPlot, bool bAssassinate = false) const;
	bool doAmbush(bool bAssassinate = false);
	void enactAmbush(bool bAssassinate = false, CvUnit* pSelectedDefender = NULL);

	void changeDebugCount(int iChange);
	void setDebugCount(int iValue);

	bool isAssassin() const;
	int getAssassinCount() const;
	void changeAssassinCount(int iChange);

	int stealthStrikesTotal() const;
	int getExtraStealthStrikes() const;
	void changeExtraStealthStrikes(int iChange);

	int stealthCombatModifierTotal() const;
	int getExtraStealthCombatModifier() const;
	void changeExtraStealthCombatModifier(int iChange);

	bool hasStealthDefense() const;
	int getStealthDefenseCount() const;
	void changeStealthDefenseCount(int iChange);

	void reveal();
	bool isRevealed() const;

	void doSetDefaultStatuses();

	int trapDamageMaxTotal() const;
	int getExtraTrapDamageMax() const;
	void changeExtraTrapDamageMax(int iChange);

	int trapDamageMinTotal() const;
	int getExtraTrapDamageMin() const;
	void changeExtraTrapDamageMin(int iChange);

	int trapComplexityTotal() const;
	int getExtraTrapComplexity() const;
	void changeExtraTrapComplexity(int iChange);

	int trapNumTriggers() const;
	int getExtraNumTriggers() const;
	void changeExtraNumTriggers(int iChange);

	int trapNumTriggersRemaining() const;
	int getNumTimesTriggered() const;
	void changeNumTimesTriggered(int iChange);

	bool isTriggerBeforeAttack() const;
	int getTriggerBeforeAttackCount() const;
	void changeTriggerBeforeAttackCount(int iChange);

	int getTrapSetWithPromotionCount(PromotionTypes ePromotion) const;
	bool hasTrapSetWithPromotion(PromotionTypes ePromotion) const;
	void changeTrapSetWithPromotionCount(PromotionTypes ePromotion, int iChange);

	int getTrapImmunityUnitCombatCount(UnitCombatTypes eUnitCombat) const;
	bool hasTrapImmunityUnitCombat(UnitCombatTypes eUnitCombat) const;
	void changeTrapImmunityUnitCombatCount(UnitCombatTypes eUnitCombat, int iChange);

	bool hasTargetUnitCombat(UnitCombatTypes eUnitCombat) const;
	void changeTargetUnitCombatCount(UnitCombatTypes eUnitCombat, int iChange);

	int trapDisableUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraTrapDisableUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraTrapDisableUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraTrapDisableUnitCombatType(UnitCombatTypes eIndex) const;

	int trapAvoidanceUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraTrapAvoidanceUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraTrapAvoidanceUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraTrapAvoidanceUnitCombatType(UnitCombatTypes eIndex) const;

	int trapTriggerUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraTrapTriggerUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraTrapTriggerUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraTrapTriggerUnitCombatType(UnitCombatTypes eIndex) const;

	bool isTrap() const;
	bool isArmedTrap() const;
	void setTrap(CvUnit* pUnit);
	bool isArmed() const;
	void doTrap(CvUnit* pUnit);
	void doTrapTrigger(CvUnit* pUnit, bool bImmune);
	bool doTrapDisable(CvUnit* pUnit);

	void changeHiddenNationalityCount(int iValue);
	int getHiddenNationalityCount() const;

	int getNoCaptureCount() const;
	void changeNoCaptureCount(int iChange);

	void makeWanted(const CvCity* pCity);

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getDistanceAttackCommunicability(PromotionLineTypes eAfflictionLine) const;
	void changeDistanceAttackCommunicability(PromotionLineTypes eAfflictionLine, int iChange);
	void setDistanceAttackCommunicability(PromotionLineTypes eAfflictionLine, int iValue);
#endif
	void setCityOfOrigin(CvCity* pCity);
	void clearCityOfOrigin();
	CvCity* getCityOfOrigin() const;

	bool isPromotionFromTrait(PromotionTypes ePromotion) const;
	void setPromotionFromTrait(PromotionTypes ePromotion, bool iChange);

	bool isGatherHerd() const;
	int getGatherHerdCount() const;
	int getExtraGatherHerdCount() const;
	void changeExtraGatherHerdCount(int iChange);

	void defineReligion();
	ReligionTypes getReligion() const;

private:
	//	static buffers allocated once and used during read and write only
	static int*	g_paiTempPromotionFreeCount;
	static int*	g_paiTempAfflictOnAttackCount;
	static int*	g_paiTempCureAfflictionCount;
	static int*	g_paiTempCureAfflictionTypeCount;
	static int*	g_paiTempAfflictionLineCount;
	static int*	g_paiTempAfflictionTurnCount;
	static int*	g_paiTempAfflictionTurnTypeCount;
	static int*	g_paiTempAfflictionHitCount;
	static int*	g_paiTempAfflictionTolerance;
	static int*	g_paiTempTrapImmunityUnitCombatCount;
	static int*	g_paiTempTargetUnitCombatCount;
	static int*	g_paiTempExtraTrapDisableUnitCombatType;
	static int*	g_paiTempExtraTrapAvoidanceUnitCombatType;
	static int*	g_paiTempExtraTrapTriggerUnitCombatType;
	static int*	g_paiTempAfflictionTypeTolerance;
	static int*	g_paiTempFortitudeModifierTypeAmount;
	static int*	g_paiTempFortitudeModifierAmount;
	static int*	g_paiTempTrapSetWithPromotionCount;
	static int* g_paiTempPromotionFromTraitCount;
	static bool*	g_pabTempValidBuildUp;
	static int* g_paiTempAfflictOnAttackTypeProbability;
	static int* g_paiTempAfflictOnAttackTypeCount;
	static int* g_paiTempAfflictOnAttackTypeImmediateCount;
	static int* g_paiTempAfflictOnAttackTypeAttemptedCount;
	static int* g_paiTempDistanceAttackCommunicability;
	static int* g_paiTempAfflictOnAttackTypeMeleeCount;
	static int* g_paiTempAfflictOnAttackTypeDistanceCount;
	static int*	g_paiTempExtraUnitCombatModifier;
	static bool* g_pabTempHasPromotion;
	static bool* g_pabTempHasUnitCombat;
	static int* g_paiTempExtraFlankingStrengthbyUnitCombatType;
	static int* g_paiTempExtraWithdrawVSUnitCombatType;
	static int* g_paiTempExtraPursuitVSUnitCombatType;
	static int* g_paiTempExtraRepelVSUnitCombatType;
	static int* g_paiTempExtraKnockbackVSUnitCombatType;
	static int* g_paiTempExtraPunctureVSUnitCombatType;
	static int* g_paiTempExtraArmorVSUnitCombatType;
	static int* g_paiTempExtraDodgeVSUnitCombatType;
	static int* g_paiTempExtraPrecisionVSUnitCombatType;
	static int* g_paiTempExtraCriticalVSUnitCombatType;
	static int* g_paiTempExtraRoundStunVSUnitCombatType;
	static int*	g_paiTempHealUnitCombatTypeVolume;
	static int*	g_paiTempHealUnitCombatTypeAdjacentVolume;
	static int* g_paiTempHealAsDamage;

	static	bool			m_staticsInitialized;

	std::vector<InvisibleTerrainChanges> m_aExtraInvisibleTerrains;
	std::vector<InvisibleFeatureChanges> m_aExtraInvisibleFeatures;
	std::vector<InvisibleImprovementChanges> m_aExtraInvisibleImprovements;
	std::vector<InvisibleTerrainChanges> m_aExtraVisibleTerrains;
	std::vector<InvisibleFeatureChanges> m_aExtraVisibleFeatures;
	std::vector<InvisibleImprovementChanges> m_aExtraVisibleImprovements;
	std::vector<InvisibleTerrainChanges> m_aExtraVisibleTerrainRanges;
	std::vector<InvisibleFeatureChanges> m_aExtraVisibleFeatureRanges;
	std::vector<InvisibleImprovementChanges> m_aExtraVisibleImprovementRanges;
	std::vector<AidStruct> m_aExtraAidChanges;

	PlayerTypes m_pPlayerInvestigated;

public:
	//
	// Algorithm/range helpers
	// Pass these to the filtered/transformed range adapters like:
	// foreach_(BuildTypes buildType, units() | filtered(!CvUnit::fn::isDead())
	//										  | transformed(CvUnit::fn::getBuildType())) {}
	// or algorithms like:
	// fn::find_if(units(), CvUnit::fn::isAutoUpgrading() && CvUnit::fn::isReadyForUpgrade())
	//
	struct fn {
		DECLARE_MAP_FUNCTOR(CvUnit, void, doActiveDefense);
		DECLARE_MAP_FUNCTOR(CvUnit, void, doOpportunityFire);
		DECLARE_MAP_FUNCTOR(CvUnit, void, setSMValues);
		DECLARE_MAP_FUNCTOR(CvUnit, void, checkPromotionObsoletion);
		DECLARE_MAP_FUNCTOR(CvUnit, void, reloadEntity);
		DECLARE_MAP_FUNCTOR(CvUnit, void, defineReligion);
		DECLARE_MAP_FUNCTOR(CvUnit, void, unloadAll);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, verifyStackValid);

		DECLARE_MAP_FUNCTOR_1(CvUnit, bool, jumpToNearestValidPlot, bool);
		DECLARE_MAP_FUNCTOR_1(CvUnit, void, kill, bool);
		DECLARE_MAP_FUNCTOR_1(CvUnit, void, NotifyEntity, MissionTypes);
		DECLARE_MAP_FUNCTOR_1(CvUnit, void, airCircle, bool);
		DECLARE_MAP_FUNCTOR_1(CvUnit, void, setBlockading, bool);
		DECLARE_MAP_FUNCTOR_1(CvUnit, void, setTransportUnit, CvUnit*);
		DECLARE_MAP_FUNCTOR_1(CvUnit, void, setHealSupportUsed, int);
		DECLARE_MAP_FUNCTOR_1(CvUnit, void, joinGroup, CvSelectionGroup*);

		DECLARE_MAP_FUNCTOR_2(CvUnit, void, doSetFreePromotions, bool, TraitTypes);
		DECLARE_MAP_FUNCTOR_2(CvUnit, void, updatePlunder, int, bool);

		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, canRBombard);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isNukeImmune);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isGroupHead);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isUsingDummyEntities);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isBuildUp);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, canAttack);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isDead);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, hasCargo);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isCargo);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isFull);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, canMove);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, canMoveAllTerrain);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, hasMoved);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isZoneOfControl);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, canIgnoreZoneofControl);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isAutoUpgrading);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isReadyForUpgrade);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isPromotionReady);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isCombat);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isAnimal);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, canFight);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, canAttackNow);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, canDefend);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, alwaysInvisible);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, IsSelected);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isCommander);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isCommodore);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isGoldenAge);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isBlockading);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, bool, isSpy);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, cargoSpace);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, getFortifyTurns);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, visibilityRange);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, collateralDamage);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, getBombardRate);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, getDamage);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, getID);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, TeamTypes, getTeam);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, PlayerTypes, getOwner);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, UnitTypes, getUnitType);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, UnitCombatTypes, getUnitCombatType);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, UnitAITypes, AI_getUnitAIType);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, DomainTypes, getDomainType);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, getArea);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, const CvArea*, area);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, const CvPlot*, plot);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, const CvUnitInfo&, getUnitInfo);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, const CvUnit*, getTransportUnit);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, BuildTypes, getBuildType);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, ImprovementTypes, getBuildTypeImprovement);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, getCargo);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, SMgetCargo);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, SMCargoVolume);
		DECLARE_MAP_FUNCTOR_CONST(CvUnit, int, revoltProtectionTotal);

		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, bool, canClaimTerritory, const CvPlot*);
		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, bool, meetsUnitSelectionCriteria, const CvUnitSelectionCriteria*);
		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, bool, canPillage, const CvPlot*);
#ifdef OUTBREAKS_AND_AFFLICTIONS
		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, bool, hasAfflictionLine, PromotionLineTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, int, worsenedProbabilitytoAfflict, PromotionLineTypes);
#endif
		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, bool, hasBuild, BuildTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, int, upgradePrice, UnitTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, int, aidTotal, PropertyTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvUnit, int, isEnemy, TeamTypes);

		DECLARE_MAP_FUNCTOR_CONST_2(CvUnit, bool, isInvisible, TeamTypes, bool);
		DECLARE_MAP_FUNCTOR_CONST_2(CvUnit, bool, canUpgrade, UnitTypes, bool);

		DECLARE_MAP_FUNCTOR_CONST_3(CvUnit, bool, canEnterArea, TeamTypes, const CvArea*, bool);
		DECLARE_MAP_FUNCTOR_CONST_3(CvUnit, bool, canBombardAtRanged, const CvPlot*, int, int);
		DECLARE_MAP_FUNCTOR_CONST_3(CvUnit, int, getTriggerValue, EventTriggerTypes /*eTrigger*/, const CvPlot* /*pPlot*/, bool /*bCheckPlot*/);
	};
};

typedef std::vector<CvUnit*> UnitVector;
typedef std::vector<const CvUnit*> ConstUnitVector;

#endif
