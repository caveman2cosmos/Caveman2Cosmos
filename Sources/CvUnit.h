#pragma once

// unit.h

#ifndef CIV4_UNIT_H
#define CIV4_UNIT_H

#include "CvDLLEntity.h"
#include "CvEnums.h"
#include "CvStructs.h"  // don't disable! used for modules
#include "CvGameObject.h"
#include "CvCity.h"
#include <map>

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvPlot;
class CvArea;
class CvUnitInfo;
class CvSelectionGroup;
class CvArtInfoUnit;
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
struct CombatDetails					// Exposed to Python
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
	PromotionLineKeyedInfo() :	m_iCureAfflictionTypeCount(0),
								m_iAfflictionTurnTypeCount(0),
								m_iAfflictionLineCount(0),
								m_iAfflictionTypeTolerance(0),
								m_iFortitudeModifierAmount(0),
								m_iAfflictOnAttackTypeProbability(0),
								m_iAfflictOnAttackTypeCount(0),
								m_iAfflictOnAttackTypeImmediateCount(0),
								m_iAfflictOnAttackTypeAttemptedCount(0),
								m_iDistanceAttackCommunicability(0),
								m_bValidBuildUp(false),
								m_iAfflictOnAttackTypeMeleeCount(0),
								m_iAfflictOnAttackTypeDistanceCount(0)
	{
	}

	bool Empty() const
	{
		return (m_iCureAfflictionTypeCount == 0 &&
			m_iAfflictionTurnTypeCount == 0 &&
			m_iAfflictionLineCount == 0 &&
			m_iAfflictionTypeTolerance == 0 &&
			m_iFortitudeModifierAmount == 0 &&
			m_iAfflictOnAttackTypeProbability == 0 &&
			m_iAfflictOnAttackTypeCount == 0 &&
			m_iAfflictOnAttackTypeImmediateCount == 0 &&
			m_iAfflictOnAttackTypeAttemptedCount == 0 &&
			m_iDistanceAttackCommunicability == 0 &&
			!m_bValidBuildUp &&
			m_iAfflictOnAttackTypeMeleeCount == 0 &&
			m_iAfflictOnAttackTypeDistanceCount == 0);
	}

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
	bool	m_bValidBuildUp;
	int		m_iAfflictOnAttackTypeMeleeCount;
	int		m_iAfflictOnAttackTypeDistanceCount;
};

class TerrainKeyedInfo
{
public:
	TerrainKeyedInfo() :	m_iTerrainProtected(0),
							m_iTerrainDoubleMoveCount(0),
							m_iExtraTerrainAttackPercent(0),
							m_iExtraTerrainDefensePercent(0),
							m_iTerrainWorkPercent(0),
//Team Project (4)
	//WorkRateMod
							m_iExtraTerrainWorkPercent(0),
							m_iExtraWithdrawOnTerrainType(0)
	{
	}

	bool Empty() const
	{
		return (m_iTerrainProtected == 0 &&
			m_iTerrainDoubleMoveCount == 0 &&
			m_iExtraTerrainAttackPercent == 0 &&
			m_iExtraTerrainDefensePercent == 0 &&
			m_iTerrainWorkPercent == 0 &&
			m_iExtraTerrainWorkPercent == 0 &&
			m_iExtraWithdrawOnTerrainType == 0);
	}

	int	m_iTerrainProtected;
	int	m_iTerrainDoubleMoveCount;
	int	m_iExtraTerrainAttackPercent;
	int	m_iExtraTerrainDefensePercent;
	//ls612: Terrain Work Modifiers
	int m_iTerrainWorkPercent;
//Team Project (4)
	//WorkRateMod
	int m_iExtraTerrainWorkPercent;
	int m_iExtraWithdrawOnTerrainType;
};

class FeatureKeyedInfo
{
public:
	FeatureKeyedInfo() :	m_iFeatureDoubleMoveCount(0),
							m_iExtraFeatureAttackPercent(0),
							m_iExtraFeatureDefensePercent(0),
							m_iFeatureWorkPercent(0),
//Team Project (4)
	//WorkRateMod
							m_iExtraFeatureWorkPercent(0),
							m_iExtraWithdrawOnFeatureType(0)
	{
	}

	bool Empty() const
	{
		return (m_iFeatureDoubleMoveCount == 0 &&
			m_iExtraFeatureAttackPercent == 0 &&
			m_iExtraFeatureDefensePercent == 0 &&
			m_iFeatureWorkPercent == 0 &&
			m_iExtraFeatureWorkPercent == 0 &&
			m_iExtraWithdrawOnFeatureType == 0);
	}

	int	m_iFeatureDoubleMoveCount;
	int	m_iExtraFeatureAttackPercent;
	int	m_iExtraFeatureDefensePercent;
	//ls612: Terrain Work Modifiers
	int m_iFeatureWorkPercent;
//Team Project (4)
	//WorkRateMod
	int m_iExtraFeatureWorkPercent;
	int m_iExtraWithdrawOnFeatureType;
};

class UnitCombatKeyedInfo
{
public:
	UnitCombatKeyedInfo() :		m_bHasUnitCombat(false),
								m_iExtraUnitCombatModifier(0),
								m_iSubCombatTypeCount(0),
								m_iOngoingTrainingCount(0),
								m_iRemovesUnitCombatTypeCount(0),
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
			m_iSubCombatTypeCount == 0 &&
			m_iOngoingTrainingCount == 0 &&
			m_iRemovesUnitCombatTypeCount == 0 &&
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
	int m_iSubCombatTypeCount;
	int m_iOngoingTrainingCount;
	int m_iRemovesUnitCombatTypeCount;
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

class CvUnit : public CvDLLEntity
{

public:

	CvUnit(bool bIsDummy = false);
	virtual ~CvUnit();

	CvGameObjectUnit* getGameObject() { return &m_GameObject; }
	const CvGameObjectUnit* getGameObject() const { return &m_GameObject; }

	// Comparison operators
	// Use address identity for now (more than one map means x/y compare wouldn't work)
	friend bool operator==(const CvUnit& lhs, const CvUnit& rhs) { return &lhs == &rhs; }
	friend bool operator!=(const CvUnit& lhs, const CvUnit& rhs) { return &lhs != &rhs; }

protected:
	CvGameObjectUnit m_GameObject;

public:
	// < M.A.D. Nukes Start >
	bool isMADEnabled() const;
	void setMADEnabled(bool bValue);
	CvPlot* getMADTargetPlot() const;
	int getMADTargetPlotX() const;
	int getMADTargetPlotY() const;
	void setMADTargetPlot(CvPlot* pPlot);
	bool setMADTargetPlot(int iX, int iY);
	bool clearMADTargetPlot();
	PlayerTypes getMADTargetPlotOwner();
	void setMADTargetPlotOwner(PlayerTypes pPlayer);
	void doMADNukes(bool bForceRetarget);
protected:
	bool m_bMADEnabled;
	int m_iMADTargetPlotX;
	int m_iMADTargetPlotY;
	PlayerTypes m_pMADTargetPlotOwner;
public:
	// < M.A.D. Nukes End   >

	void reloadEntity(bool bForceLoad = false);
	void init(int iID, UnitTypes eUnit, UnitAITypes eUnitAI, PlayerTypes eOwner, int iX, int iY, DirectionTypes eFacingDirection, int iBirthmark);
	void uninit();
	void changeIdentity(UnitTypes eUnit);
	void reset(int iID = 0, UnitTypes eUnit = NO_UNIT, PlayerTypes eOwner = NO_PLAYER, bool bConstructorCall = false, bool bIdentityChange = false);
	void setupGraphical();

	void convert(CvUnit* pUnit, const bool bKillOriginal=true); // Exposed to Python
	void kill(bool bDelay, PlayerTypes ePlayer = NO_PLAYER, bool bMessaged = false); // Exposed to Python
	void killUnconditional(bool bDelay, PlayerTypes ePlayer, bool bMessaged = false); // Used internally

	DllExport void NotifyEntity(MissionTypes eMission);

	void doTurn();

	void updateCombat(bool bQuick = false, CvUnit* pSelectedDefender = NULL, bool bSamePlot = false, bool bStealth = false, bool bNoCache = false);
	void updateAirCombat(bool bQuick = false);
	void updateAirStrike(CvPlot* pPlot, bool bQuick, bool bFinish);

	bool isActionRecommended(int iAction);

	bool isBetterDefenderThan(CvUnit* pDefender, CvUnit* pAttacker) const; // Exposed to Python

	bool canDoCommand(CommandTypes eCommand, int iData1, int iData2, bool bTestVisible = false, bool bTestBusy = true); // Exposed to Python
	void doCommand(CommandTypes eCommand, int iData1, int iData2); // Exposed to Python

#ifdef USE_OLD_PATH_GENERATOR
	FAStarNode* getPathLastNode() const;
#else
	int getPathMovementRemaining() const;
#endif
	CvPlot* getPathEndTurnPlot() const; // Exposed to Python
	bool generatePath(const CvPlot* pToPlot, int iFlags = 0, bool bReuse = false, int* piPathTurns = NULL, int iMaxTurns = MAX_INT, int iOptimizationLimit = -1) const; // Exposed to Python

	bool canEnterTerritory(TeamTypes eTeam, bool bIgnoreRightOfPassage = false) const; // Exposed to Python
	bool canEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage = false) const; // Exposed to Python
	TeamTypes getDeclareWarMove(const CvPlot* pPlot) const; // Exposed to Python


	bool canMoveInto(const CvPlot* pPlot, MoveCheck::flags flags = MoveCheck::None, CvUnit** ppDefender = nullptr) const;
	// Deprecated - use method above
	//bool canMoveInto(const CvPlot* pPlot,
	//	bool bAttack = false,
	//	bool bDeclareWar = false,
	//	bool bIgnoreLoad = false,
	//	bool bIgnoreTileLimit = false,
	//	bool bIgnoreLocation = false,
	//	bool bIgnoreAttack = false,
	//	CvUnit** pDefender = NULL,
	//	bool bCheckForBest = false,
	//	bool bAssassinate = false,
	//	bool bSuprise = false) const; // Exposed to Python

	bool canMoveOrAttackInto(const CvPlot* pPlot, bool bDeclareWar = false) const; // Exposed to Python
	bool canMoveThrough(const CvPlot* pPlot, bool bDeclareWar = false) const; // Exposed to Python
	void attack(CvPlot* pPlot, bool bQuick, bool bStealth = false, bool bNoCache = false);
	void attackForDamage(CvUnit *pDefender, int attackerDamageChange, int defenderDamageChange);
	void fightInterceptor(const CvPlot* pPlot, bool bQuick);
	void move(CvPlot* pPlot, bool bShow, bool bFree = false);
/************************************************************************************************/
/* Afforess	                  Start		 06/13/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool jumpToNearestValidPlot(bool bKill = true); // Exposed to Python
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	bool canAutomate(AutomateTypes eAutomate) const; // Exposed to Python
	void automate(AutomateTypes eAutomate);

	bool canScrap() const; // Exposed to Python
	void scrap();
/************************************************************************************************/
/* Afforess	                  Start		 02/14/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool canTradeUnit(PlayerTypes eReceivingPlayer);

	void tradeUnit(PlayerTypes eReceivingPlayer);
	bool spyNuke(int iX, int iY, bool bCaught);
	bool spyNukeAffected(const CvPlot* pPlot, TeamTypes eTeam, int iRange) const;
	bool canClaimTerritory(const CvPlot* pPlot) const;
	bool claimTerritory();
	int getMaxHurryFood(CvCity* pCity) const;
	int getHurryFood(const CvPlot* pPlot) const;
	bool canHurryFood(const CvPlot* pPlot) const;
	bool hurryFood();

	bool canPerformInquisition(const CvPlot* pPlot) const;
	bool performInquisition();
	//SAD
	int surroundedDefenseModifier(const CvPlot *pPlot, const CvUnit *pDefender) const;

	int getCanMovePeaksCount() const;
	bool isCanMovePeaks() const;
	void changeCanMovePeaksCount(int iChange);
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	int getCanLeadThroughPeaksCount() const;
	bool isCanLeadThroughPeaks() const;
	void changeCanLeadThroughPeaksCount(int iChange);

	DllExport PlayerTypes getNationality() const;
	void setNationality(PlayerTypes eNewNationality);
	void combatWon(CvUnit* pLoser, bool bAttacking);

	int interceptionChance(const CvPlot* pPlot) const;

	int getRandomMinExperienceTimes100() const;

	//Great Commanders... By KillmePlease

	//for combat units:
	CvUnit* getCommander() const;
	void tryUseCommander(); //assigns m_pUsedCommander by call to getCommander() and consumes command points from used commander.
	bool isCommander() const; //Exposed to python
	void setCommander(bool bNewVal);
	void nullUsedCommander(); //delete m_pUsedCommander
	void clearCommanderCache() ; //	Should be called prior to each turn

	CvUnit* getUsedCommander() const;

	//for commander units:
	int controlPointsLeft() const; //Exposed to python
	int controlPoints() const; //Exposed to python
	int commandRange() const; //Exposed to python
	//from promotions:
	int getExtraControlPoints() const; //control
	void changeExtraControlPoints(int iChange);
	int getExtraCommandRange() const; //command
	void changeExtraCommandRange(int iChange);

	int getZoneOfControlCount() const;
	bool isZoneOfControl() const;
	void changeZoneOfControlCount(int iChange);

	bool sleepForEspionage();
	PlayerTypes getOriginalOwner() const;

	int getExperience100() const; // Exposed to Python
	void setExperience100(int iNewValue, int iMax = -1); // Exposed to Python
	void changeExperience100(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false); // Exposed to Python

	void doBattleFieldPromotions(CvUnit* pDefender, const CombatDetails& cdDefenderDetails, const CvPlot* pPlot, bool bAttackerHasLostNoHP, bool bAttackerWithdrawn, int iAttackerInitialDamage, int iWinningOdds, int iInitialAttXP, int iInitialAttGGXP, int iDefenderInitialDamage, int iInitialDefXP, int iInitialDefGGXP, bool &bAttackerPromoted, bool &bDefenderPromoted, int iNonLethalAttackWinChance, int iNonLethalDefenseWinChance, int iDefenderFirstStrikes, int iAttackerFirstStrikes);
	void doDynamicXP(CvUnit* pDefender, const CvPlot* pPlot, int iAttackerInitialDamage, int iWinningOdds, int iDefenderInitialDamage, int iInitialAttXP, int iInitialDefXP, int iInitialAttGGXP, int iInitialDefGGXP, bool bPromotion, bool bDefPromotion);

	void changeTerrainProtected(TerrainTypes eIndex, int iNewValue);
	bool isTerrainProtected(TerrainTypes eIndex) const;
	int getTerrainProtectedCount(TerrainTypes eIndex, bool bIgnoreCommanders = false) const;

	void doCommerceAttacks(const CvUnit* pDefender, const CvPlot* pPlot);

	bool isAutoPromoting() const;
	void setAutoPromoting(bool bNewValue);
	bool isAutoUpgrading() const;
	void setAutoUpgrading(bool bNewValue);

	const CvWString getDescription(uint uiForm = 0) const;

	bool canShadow() const;
	bool canShadowAt(const CvPlot* pShadowPlot, CvUnit* pShadowUnit = NULL) const;
	
	void setShadowUnit(CvUnit* pUnit);
	CvUnit* getShadowUnit() const;

	TechTypes getDesiredDiscoveryTech() const;
	void setDesiredDiscoveryTech(TechTypes eTech);
	void waitForTech(int iFlag, int eTech);
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	bool canGift(bool bTestVisible = false, bool bTestTransport = true); // Exposed to Python
	void gift(bool bTestTransport = true);

	bool canLoadOntoUnit(const CvUnit* pUnit, const CvPlot* pPlot) const; // Exposed to Python
	void loadOntoUnit(CvUnit* pUnit);

	bool canLoad(const CvPlot* pPlot) const; // Exposed to Python
	void load();
	bool shouldLoadOnMove(const CvPlot* pPlot) const;

	bool canUnload() const; // Exposed to Python
	void unload();

	bool canUnloadAll() const; // Exposed to Python
	void unloadAll();

	bool canHold(const CvPlot* pPlot) const; // Exposed to Python
	bool canSleep(const CvPlot* pPlot) const; // Exposed to Python
	bool canFortify(const CvPlot* pPlot) const; // Exposed to Python
	bool canEstablish(const CvPlot* pPlot) const;
	bool canEscape(const CvPlot* pPlot) const;
	bool canBuildUp(const CvPlot* pPlot) const;
	bool canAirPatrol(const CvPlot* pPlot) const; // Exposed to Python
	void airCircle(bool bStart);

	bool canSeaPatrol(const CvPlot* pPlot) const; // Exposed to Python

	bool canHeal(const CvPlot* pPlot) const; // Exposed to Python
	bool canSentry(const CvPlot* pPlot) const; // Exposed to Python

	int healRate(const CvPlot* pPlot, bool bHealCheck = false) const;
	int getHealRateAsType(const CvPlot* pPlot, bool bHealCheck = false, UnitCombatTypes eHealAsType = NO_UNITCOMBAT) const;
	int healTurns(const CvPlot* pPlot) const;
	int healTurnsAsType(const CvPlot* pPlot, UnitCombatTypes eHealAsType) const;
	void doHeal();

	bool canAirlift(const CvPlot* pPlot) const; // Exposed to Python
	bool canAirliftAt(const CvPlot* pPlot, int iX, int iY) const; // Exposed to Python
	bool airlift(int iX, int iY);

	bool isNukeVictim(const CvPlot* pPlot, TeamTypes eTeam) const; // Exposed to Python
	bool canNuke(const CvPlot* pPlot) const; // Exposed to Python
/************************************************************************************************/
/* Afforess	                  Start		 09/09/10                                               */
/*                                                                                              */
/*  M.A.D Nukes                                                                                 */
/************************************************************************************************/
	bool canNukeAt(const CvPlot* pPlot, int iX, int iY, bool bTestAtWar = true) const; // Exposed to Python
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	bool nuke(int iX, int iY, bool bTrap = false);

	bool canRecon(const CvPlot* pPlot) const; // Exposed to Python
	bool canReconAt(const CvPlot* pPlot, int iX, int iY) const; // Exposed to Python
	bool recon(int iX, int iY);

	bool canAirBomb(const CvPlot* pPlot) const; // Exposed to Python
	bool canAirBombAt(const CvPlot* pPlot, int iX, int iY) const; // Exposed to Python
	bool airBomb(int iX, int iY);

	CvCity* bombardTarget(const CvPlot* pPlot) const; // Exposed to Python
	// Super Forts begin *bombard*
	CvPlot* bombardImprovementTarget(const CvPlot* pPlot) const;
	// Super Forts end
	bool canBombard(const CvPlot* pPlot, bool bIgnoreHasAttacked = false) const; // Exposed to Python
	bool bombard();

	bool canParadrop(const CvPlot* pPlot) const; // Exposed to Python
	bool canParadropAt(const CvPlot* pPlot, int iX, int iY) const; // Exposed to Python
	bool paradrop(int iX, int iY);

	bool canPillage(const CvPlot* pPlot) const; // Exposed to Python
	bool pillage();

	//TSHEEP Assassin Mission
	bool canAssassin(const CvPlot* pPlot, bool bTestVisible) const; // Exposed to Python
	//int assassinCost(const CvUnit* pTargetUnit) const;
	//int assassinProb(const CvUnit* pTargetUnit) const;
	//bool assassin();
	//TSHEEP Other functions
	bool awardSpyExperience(TeamTypes eTargetTeam, int iModifier);
	//TSHEEP End
	//RevolutionDCM start
	bool canBribe(const CvPlot* pPlot, bool bTestVisible) const;
	//RevolutionDCM end

	bool canPlunder(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool plunder();
	void updatePlunder(int iChange, bool bUpdatePlotGroups);

	int sabotageCost(const CvPlot* pPlot) const; // Exposed to Python
	int sabotageProb(const CvPlot* pPlot, ProbabilityTypes eProbStyle = PROBABILITY_REAL) const; // Exposed to Python
	bool canSabotage(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool sabotage();

	int destroyCost(const CvPlot* pPlot) const; // Exposed to Python
	int destroyProb(const CvPlot* pPlot, ProbabilityTypes eProbStyle = PROBABILITY_REAL) const; // Exposed to Python
	bool canDestroy(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool destroy();

	int stealPlansCost(const CvPlot* pPlot) const; // Exposed to Python
	int stealPlansProb(const CvPlot* pPlot, ProbabilityTypes eProbStyle = PROBABILITY_REAL) const; // Exposed to Python
	bool canStealPlans(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool stealPlans();

	bool canFound(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool found();

	bool canSpread(const CvPlot* pPlot, ReligionTypes eReligion, bool bTestVisible = false) const; // Exposed to Python
	bool spread(ReligionTypes eReligion);

	bool canSpreadCorporation(const CvPlot* pPlot, CorporationTypes eCorporation, bool bTestVisible = false) const; // Exposed to Python
	bool spreadCorporation(CorporationTypes eCorporation);
	int spreadCorporationCost(CorporationTypes eCorporation, CvCity* pCity) const;

	bool canJoin(const CvPlot* pPlot, SpecialistTypes eSpecialist) const; // Exposed to Python
	bool join(SpecialistTypes eSpecialist);

	bool canConstruct(const CvPlot* pPlot, BuildingTypes eBuilding, bool bTestVisible = false) const; // Exposed to Python
	bool construct(BuildingTypes eBuilding);

	TechTypes getDiscoveryTech() const; // Exposed to Python
	int getDiscoverResearch(TechTypes eTech) const; // Exposed to Python
	bool canDiscover() const; // Exposed to Python
	bool discover();

	int getMaxHurryProduction(CvCity* pCity) const; // Exposed to Python
	int getHurryProduction(const CvPlot* pPlot) const; // Exposed to Python
	bool canHurry(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool hurry();

	int getTradeGold(const CvPlot* pPlot) const; // Exposed to Python
	bool canTrade(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool trade();

	int getGreatWorkCulture(const CvPlot* pPlot) const; // Exposed to Python
	bool canGreatWork(const CvPlot* pPlot) const; // Exposed to Python
	bool greatWork();

	bool doOutcomeMission(MissionTypes eMission);

	int getEspionagePoints(const CvPlot* pPlot) const; // Exposed to Python
	bool canInfiltrate(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool infiltrate();

	bool canEspionage(const CvPlot* pPlot, bool bTestVisible = false) const;
	bool espionage(EspionageMissionTypes eMission, int iData);
	bool testSpyIntercepted(PlayerTypes eTargetPlayer, int iModifier = 0);
	int getSpyInterceptPercent(TeamTypes eTargetTeam) const;
	bool isIntruding() const;

	bool canGoldenAge(const CvPlot* pPlot, bool bTestVisible = false) const; // Exposed to Python
	bool goldenAge();

	bool canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestVisible = false) const; // Exposed to Python
	bool build(BuildTypes eBuild);

	bool canPromote(PromotionTypes ePromotion, int iLeaderUnitId) const; // Exposed to Python
	bool promote(PromotionTypes ePromotion, int iLeaderUnitId); // Exposed to Python

	int canLead(const CvPlot* pPlot, int iUnitId) const;
	bool lead(int iUnitId);

	int canGiveExperience(const CvPlot* pPlot) const; // Exposed to Python
	bool giveExperience(); // Exposed to Python
	int getStackExperienceToGive(int iNumUnits) const;

	int upgradePrice(UnitTypes eUnit) const; // Exposed to Python
	bool upgradeAvailable(UnitTypes eFromUnit, UnitTypes eToUnit) const; // Exposed to Python
	bool canUpgrade(UnitTypes eUnit, bool bTestVisible = false) const; // Exposed to Python
	bool isReadyForUpgrade() const;
	bool hasUpgrade(bool bSearch = false) const; // Exposed to Python
	bool hasUpgrade(UnitTypes eUnit, bool bSearch = false) const;
	CvCity* getUpgradeCity(bool bSearch = false) const;
	CvCity* getUpgradeCity(UnitTypes eUnit, bool bSearch = false, int* iSearchValue = NULL) const;
	bool upgrade(UnitTypes eUnit);

	HandicapTypes getHandicapType() const; // Exposed to Python
	CivilizationTypes getCivilizationType() const; // Exposed to Python
	const wchar* getVisualCivAdjective(TeamTypes eForTeam) const;
	SpecialUnitTypes getSpecialUnitType() const; // Exposed to Python								
	UnitTypes getCaptureUnitType() const; // Exposed to Python
	UnitCombatTypes getUnitCombatType() const; // Exposed to Python
	DllExport DomainTypes getDomainType() const; // Exposed to Python
	InvisibleTypes getInvisibleType() const; // Exposed to Python
	int getNumSeeInvisibleTypes() const; // Exposed to Python
	InvisibleTypes getSeeInvisibleType(int i) const; // Exposed to Python

	int flavorValue(FlavorTypes eFlavor) const; // Exposed to Python

	bool isBarbarian() const;
	bool isNPC() const; // Exposed to Python
	bool isHominid() const; // Exposed to Python
	bool isHuman() const; // Exposed to Python

	int visibilityRange(const CvPlot* pPlot = NULL) const; // Exposed to Python

	int baseMoves() const; // Exposed to Python
	int maxMoves() const; // Exposed to Python
	int movesLeft() const; // Exposed to Python
	DllExport bool canMove() const; // Exposed to Python
	DllExport bool hasMoved() const; // Exposed to Python

	int airRange() const; // Exposed to Python
	int nukeRange() const; // Exposed to Python			

	bool canBuildRoute() const; // Exposed to Python
	DllExport BuildTypes getBuildType() const; // Exposed to Python
	ImprovementTypes getBuildTypeImprovement() const;

	bool isAnimal() const; // Exposed to Python
	bool isNoBadGoodies() const; // Exposed to Python
	bool isOnlyDefensive() const;

	bool hasRBombardForceAbility() const;
	int getRBombardForceAbilityCount() const;
	void changeRBombardForceAbilityCount(int iChange);

	bool isNoCapture() const; // Exposed to Python
	bool isRivalTerritory() const; // Exposed to Python
	bool isMilitaryHappiness() const; // Exposed to Python
	bool isInvestigate() const; // Exposed to Python
	bool isCounterSpy() const; // Exposed to Python
	bool isSpy() const;
	bool isFound() const; // Exposed to Python
/********************************************************************************/
/**		REVOLUTION_MOD							1/1/08				DPII		*/
/**																				*/
/**		 																		*/
/********************************************************************************/
	/*bool isCanBeRebel() const;
	bool isCanRebelCapture() const;
	bool isCannotDefect() const;
	bool isCanQuellRebellion() const;
	*/
/********************************************************************************/
/**		REVOLUTION_MOD							END								*/
/********************************************************************************/
	bool isGoldenAge() const; // Exposed to Python

	// Can this unit always coexist with all other units anywhere?
	bool canCoexistAlways() const;
	// Can this unit always coexist with all units on the specified plot?
	bool canCoexistAlwaysOnPlot(const CvPlot& onPlot) const;
	// Can this unit coexist with the specified team anywhere?
	bool canCoexistWithTeam(const TeamTypes withTeam) const;
	// Can this unit coexist with the specified team, on the specified plot?
	bool canCoexistWithTeamOnPlot(const TeamTypes withTeam, const CvPlot& onPlot) const;
	// Can this unit coexist with an attacking unit (possibly performing an assassination)?
	bool canCoexistWithAttacker(const CvUnit& attacker, bool bAssassinate = false) const;

	// Checks for differing domains, transport status, amnesty game setting
	// TODO: roll this into the other Coexist functions
	bool canUnitCoexistWithArrivingUnit(const CvUnit& pUnit) const;

	DllExport bool isFighting() const; // Exposed to Python
	DllExport bool isAttacking() const; // Exposed to Python
	DllExport bool isDefending() const; // Exposed to Python
	bool isCombat() const; // Exposed to Python

	int withdrawalHP(int maxHitPoints, int iAttackerEarly) const;
	DllExport int maxHitPoints() const; // Exposed to Python
	int currHitPoints() const; // Exposed to Python
	bool isHurt() const; // Exposed to Python
	DllExport bool isDead() const; // Exposed to Python

	void setBaseCombatStr(int iCombat); // Exposed to Python
	int baseCombatStr() const; // Exposed to Python
	int baseCombatStrNonGranular() const;
	int baseCombatStrPreCheck() const;
	int baseAirCombatStrPreCheck() const;
	float fbaseCombatStr() const;
	/*** Dexy - Surround and Destroy START ****/
	int maxCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL, bool bSurroundedModifier = true) const; // Exposed to Python
	int currCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL, bool bSurroundedModifier = true) const; // Exposed to Python
	// OLD CODE
	// int maxCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const; // Exposed to Python
	// int currCombatStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const; // Exposed to Python
	/*** Dexy - Surround and Destroy  END  ****/
	int currFirepower(const CvPlot* pPlot, const CvUnit* pAttacker) const; // Exposed to Python
	int currEffectiveStr(const CvPlot* pPlot, const CvUnit* pAttacker, CombatDetails* pCombatDetails = NULL) const;
	DllExport float maxCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const; // Exposed to Python
	DllExport float currCombatStrFloat(const CvPlot* pPlot, const CvUnit* pAttacker) const; // Exposed to Python

	DllExport bool canFight() const; // Exposed to Python
	bool canAttack() const; // Exposed to Python
	bool canAttack(const CvUnit& defender) const;
	bool canDefend(const CvPlot* pPlot = NULL) const; // Exposed to Python
	bool canSiege(TeamTypes eTeam) const; // Exposed to Python

	int airBaseCombatStr() const;
	float fairBaseCombatStr() const; // Exposed to Python
	int airMaxCombatStr(const CvUnit* pOther) const; // Exposed to Python
	int airCurrCombatStr(const CvUnit* pOther) const; // Exposed to Python
	DllExport float airMaxCombatStrFloat(const CvUnit* pOther) const; // Exposed to Python
	DllExport float airCurrCombatStrFloat(const CvUnit* pOther) const; // Exposed to Python
	int combatLimit(const CvUnit* pOpponent = NULL) const; // Exposed to Python
	int airCombatLimit(const CvUnit* pOpponent = NULL) const; // Exposed to Python
	DllExport bool canAirAttack() const; // Exposed to Python
	bool canAirDefend(const CvPlot* pPlot = NULL) const; // Exposed to Python
	int airCombatDamage(const CvUnit* pDefender) const; // Exposed to Python
	int rangeCombatDamage(const CvUnit* pDefender) const; // Exposed to Python
	CvUnit* bestInterceptor(const CvPlot* pPlot) const; // Exposed to Python
	CvUnit* bestSeaPillageInterceptor(CvUnit* pPillager, int iMinOdds) const; // Exposed to Python

	bool isAutomated() const; // Exposed to Python
	DllExport bool isWaiting() const; // Exposed to Python
	bool isFortifyable() const; // Exposed to Python
	bool isEstablishable() const;
	bool isEscapable() const;
	bool isBuildUpable() const;
	int fortifyModifier() const; // Exposed to Python
	//int establishModifier() const;
	//int escapeModifier() const;
	int buildupLevel() const;
	//TB Combat Mods begin
	int fortifyRepelModifier() const;
	//TB Combat Mods End
	int experienceNeeded() const; // Exposed to Python
	int attackXPValue() const; // Exposed to Python
	int defenseXPValue() const; // Exposed to Python
	int maxXPValue(const CvUnit* pVictim, bool bBarb = false) const; // Exposed to Python

	int firstStrikes() const; // Exposed to Python
	int chanceFirstStrikes() const; // Exposed to Python
	int maxFirstStrikes() const; // Exposed to Python
	DllExport bool isRanged() const; // Exposed to Python

	bool alwaysInvisible() const; // Exposed to Python
	bool immuneToFirstStrikes() const; // Exposed to Python
	bool noDefensiveBonus() const;
	int getExtraNoDefensiveBonusCount() const;
	void changeExtraNoDefensiveBonusCount(int iChange);
	bool ignoreBuildingDefense() const; // Exposed to Python
	bool canMoveImpassable() const; // Exposed to Python
	bool canMoveAllTerrain() const; // Exposed to Python
	bool flatMovementCost() const; // Exposed to Python
	bool ignoreTerrainCost() const; // Exposed to Python
	bool isNeverInvisible() const;
	int getNoInvisibilityCount() const;
	void changeNoInvisibilityCount(int iChange);
	DllExport bool isInvisible(TeamTypes eTeam, bool bDebug = false, bool bCheckCargo = true) const; // Exposed to Python
	bool isNukeImmune() const; // Exposed to Python
/************************************************************************************************/
/* REVDCM_OC                              02/16/10                                phungus420    */
/*                                                                                              */
/* Inquisitions                                                                                 */
/************************************************************************************************/
	bool isInquisitor() const; // Exposed to Python
/************************************************************************************************/
/* REVDCM_OC                               END                                                  */
/************************************************************************************************/

	int maxInterceptionProbability(bool bIgnoreCommanders = false) const; // Exposed to Python
	int currInterceptionProbability() const; // Exposed to Python
	int evasionProbability(bool bIgnoreCommanders = false) const; // Exposed to Python
	int withdrawalProbability() const; // Exposed to Python
//TB Combat Mods Begin
	int attackCombatModifierTotal() const;
	int defenseCombatModifierTotal() const;
	int pursuitProbability() const; // Exposed to Python
	int earlyWithdrawTotal() const; // Exposed to Python
	int vsBarbsModifier() const; // Exposed to Python
	int religiousCombatModifierTotal(ReligionTypes eReligion = NO_RELIGION, bool bDisplay = false) const;
	int armorTotal() const; // Exposed to Python
	int punctureTotal() const; // Exposed to Python
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
	bool hasCombatType(UnitCombatTypes eCombatType) const;
	bool hasSubCombatType(UnitCombatTypes eCombatType) const;
	bool hasCureAfflictionType(PromotionLineTypes ePromotionLineType) const;
	int fortitudeTotal() const;
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
	int collateralDamage() const; // Exposed to Python
	int collateralDamageLimit() const; // Exposed to Python
	int collateralDamageMaxUnits() const; // Exposed to Python

	int cityAttackModifier() const; // Exposed to Python
	int cityDefenseModifier() const;
	int cityDefenseVSOpponent(const CvUnit* pOpponent) const;
	int animalCombatModifier() const; // Exposed to Python
	int hillsAttackModifier() const; // Exposed to Python
	int hillsDefenseModifier() const; // Exposed to Python
	int terrainAttackModifier(TerrainTypes eTerrain) const; // Exposed to Python
	int terrainDefenseModifier(TerrainTypes eTerrain) const; // Exposed to Python
	int featureAttackModifier(FeatureTypes eFeature) const; // Exposed to Python
	int featureDefenseModifier(FeatureTypes eFeature) const; // Exposed to Python
	int unitAttackModifier(UnitTypes eUnit) const; // Exposed to Python
	int unitDefenseModifier(UnitTypes eUnit) const; // Exposed to Python
	int unitCombatModifier(UnitCombatTypes eUnitCombat) const; // Exposed to Python
	int domainModifier(DomainTypes eDomain) const;

	SpecialUnitTypes specialCargo() const; // Exposed to Python
	SpecialUnitTypes SMspecialCargo() const;
	SpecialUnitTypes SMnotSpecialCargo() const;
	DomainTypes domainCargo() const; // Exposed to Python
	int cargoSpace() const;
	void changeCargoSpace(int iChange);
	bool isFull() const; // Exposed to Python
	int cargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const; // Exposed to Python
	int SMcargoSpaceAvailable(SpecialUnitTypes eSpecialCargo = NO_SPECIALUNIT, DomainTypes eDomainCargo = NO_DOMAIN) const;
	bool hasCargo() const; // Exposed to Python
	bool canCargoAllMove() const; // Exposed to Python
	bool canCargoEnterArea(TeamTypes eTeam, const CvArea* pArea, bool bIgnoreRightOfPassage) const;
	int getUnitAICargo(UnitAITypes eUnitAI) const; // Exposed to Python

	DllExport int getID() const; // Exposed to Python
	int getIndex() const;
	DllExport IDInfo getIDInfo() const;
	void setID(int iID);

	int getGroupID() const; // Exposed to Python
	bool isInGroup() const; // Exposed to Python
	bool isGroupHead() const; // Exposed to Python
	DllExport CvSelectionGroup* getGroup() const; // Exposed to Python
	bool canJoinGroup(const CvPlot* pPlot, CvSelectionGroup* pSelectionGroup) const;
	void joinGroup(CvSelectionGroup* pSelectionGroup, bool bRemoveSelected = false, bool bRejoin = true);

	DllExport int getHotKeyNumber(); // Exposed to Python
	void setHotKeyNumber(int iNewValue); // Exposed to Python

	DllExport int getViewportX() const; // Exposed to Python
	inline int getX() const { return m_iX; }
	DllExport int getViewportY() const; // Exposed to Python
	inline int getY() const { return m_iY; }
	bool isInViewport() const;
	void setXY(int iX, int iY, bool bGroup = false, bool bUpdate = true, bool bShow = false, bool bCheckPlotVisible = false, bool bInit = false); // Exposed to Python
	bool isTempUnit() const;
	bool at(int iX, int iY) const; // Exposed to Python
	void addMission(const CvMissionDefinition& mission);

	DllExport bool atPlot(const CvPlot* pPlot) const; // Exposed to Python
	CvPlot* plot() const; // Exposed to Python
	DllExport CvPlot* plotExternal() const; // Exposed to Python
	int getArea() const; // Exposed to Python
	CvArea* area() const; // Exposed to Python
	bool onMap() const;

	int getLastMoveTurn() const;
	void setLastMoveTurn(int iNewValue);

	CvPlot* getReconPlot() const; // Exposed to Python
	void setReconPlot(CvPlot* pNewValue); // Exposed to Python

	int getGameTurnCreated() const; // Exposed to Python
	void setGameTurnCreated(int iNewValue);

	DllExport int getDamage() const;
	int getHealAsDamage(UnitCombatTypes eHealAsType) const;
	void changeHealAsDamage(UnitCombatTypes eHealAsType, int iChange, PlayerTypes ePlayer = NO_PLAYER);
	void setHealAsDamage(UnitCombatTypes eHealAsType, int iNewValue, PlayerTypes ePlayer = NO_PLAYER, bool bNotifyEntity = false);
	int getDamagePercent() const; // Exposed to Python
	void setDamage(int iNewValue, PlayerTypes ePlayer = NO_PLAYER, bool bNotifyEntity = true, UnitCombatTypes eHealAsType = NO_UNITCOMBAT, bool bSecondPass = false);
	void changeDamage(int iChange, PlayerTypes ePlayer = NO_PLAYER, UnitCombatTypes eHealAsType = NO_UNITCOMBAT); // Exposed to Python
	void changeDamagePercent(int iChange, PlayerTypes ePlayer = NO_PLAYER);
	void setupPreCombatDamage();
	int getPreCombatDamage() const;

	int getMoves() const; // Exposed to Python
	void setMoves(int iNewValue); // Exposed to Python
	void changeMoves(int iChange); // Exposed to Python
	void finishMoves(); // Exposed to Python

	int getExperience() const; // Exposed to Python
	void setExperience(int iNewValue, int iMax = -1); // Exposed to Python
	void changeExperience(int iChange, int iMax = -1, bool bFromCombat = false, bool bInBorders = false, bool bUpdateGlobal = false); // Exposed to Python

	int getLevel() const; // Exposed to Python
	void setLevel(int iNewValue);
	void changeLevel(int iChange);

	int getCargo() const;
	int SMgetCargo() const; // Exposed to Python
	void changeCargo(int iChange); // Exposed to Python
	void SMchangeCargo(int iChange);
	void getCargoUnits(std::vector<CvUnit*>& aUnits) const;
	void validateCargoUnits();

	CvPlot* getAttackPlot() const;
	void setAttackPlot(const CvPlot* pNewValue, bool bAirCombat);
	bool isAirCombat() const;

	DllExport int getCombatTimer() const;
	void setCombatTimer(int iNewValue);
	void changeCombatTimer(int iChange);

	int getCombatFirstStrikes() const;
	void setCombatFirstStrikes(int iNewValue);
	void changeCombatFirstStrikes(int iChange);

	int getFortifyTurns() const; // Exposed to Python
	void setFortifyTurns(int iNewValue);
	void changeFortifyTurns(int iChange);

	int getBlitzCount() const;
	bool isBlitz() const; // Exposed to Python
	void changeBlitzCount(int iChange);

	int getAmphibCount() const;
	bool isAmphib() const; // Exposed to Python
	void changeAmphibCount(int iChange);

	int getRiverCount() const;
	bool isRiver() const; // Exposed to Python
	void changeRiverCount(int iChange);

	int getEnemyRouteCount() const;
	bool isEnemyRoute() const; // Exposed to Python
	void changeEnemyRouteCount(int iChange);

	int getAlwaysHealCount() const;
	bool isAlwaysHeal() const; // Exposed to Python
	void changeAlwaysHealCount(int iChange);

	int getHillsDoubleMoveCount() const;
	bool isHillsDoubleMove() const; // Exposed to Python
	void changeHillsDoubleMoveCount(int iChange);

	int getImmuneToFirstStrikesCount() const;
	void changeImmuneToFirstStrikesCount(int iChange);

	int getAlwaysInvisibleCount() const;
	void changeAlwaysInvisibleCount(int iChange);

/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 16.09.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: New Tag Definition                                                               **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	int getDefensiveVictoryMoveCount() const;
	bool isDefensiveVictoryMove() const; // Exposed to Python
	void changeDefensiveVictoryMoveCount(int iChange);

	int getFreeDropCount() const;
	bool isFreeDrop() const; // Exposed to Python
	void changeFreeDropCount(int iChange);

	int getOffensiveVictoryMoveCount() const;
	bool isOffensiveVictoryMove() const; // Exposed to Python
	void changeOffensiveVictoryMoveCount(int iChange);

//Team Project (2)
	int getOneUpCount() const;
	bool isOneUp() const;
	void changeOneUpCount(int iChange);

	int getPillageCultureCount() const;
	bool isPillageCulture() const; // Exposed to Python
	void changePillageCultureCount(int iChange);

	int getPillageEspionageCount() const;
	bool isPillageEspionage() const; // Exposed to Python
	void changePillageEspionageCount(int iChange);

	int getPillageMarauderCount() const;
	bool isPillageMarauder() const; // Exposed to Python
	void changePillageMarauderCount(int iChange);

	int getPillageOnMoveCount() const;
	bool isPillageOnMove() const; // Exposed to Python
	void changePillageOnMoveCount(int iChange);

	int getPillageOnVictoryCount() const;
	bool isPillageOnVictory() const; // Exposed to Python
	void changePillageOnVictoryCount(int iChange);

	int getPillageResearchCount() const;
	bool isPillageResearch() const; // Exposed to Python
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

//Team Project (2)
	int getSurvivorChance() const;
	void changeSurvivorChance(int iChange);

	int getVictoryAdjacentHeal() const;
	void changeVictoryAdjacentHeal(int iChange);

	int getVictoryHeal() const;
	void changeVictoryHeal(int iChange);

//Team Project (2)
	int getVictoryStackHeal() const;
	void changeVictoryStackHeal(int iChange);
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags                                                             **/
/*****************************************************************************************************/

	int getExtraVisibilityRange() const; // Exposed to Python
	void changeExtraVisibilityRange(int iChange);

	int getExtraMoves() const; // Exposed to Python
	void changeExtraMoves(int iChange);

	int getExtraMoveDiscount() const; // Exposed to Python
	void changeExtraMoveDiscount(int iChange);

	int getExtraAirRange() const; // Exposed to Python
	void changeExtraAirRange(int iChange);

	int getExtraIntercept(bool bIgnoreCommanders = false) const; // Exposed to Python
	void changeExtraIntercept(int iChange);

	int getExtraEvasion(bool bIgnoreCommanders = false) const; // Exposed to Python
	void changeExtraEvasion(int iChange);

	int getExtraFirstStrikes() const; // Exposed to Python
	void changeExtraFirstStrikes(int iChange);

	int getExtraChanceFirstStrikes() const; // Exposed to Python
	void changeExtraChanceFirstStrikes(int iChange);

	int getExtraWithdrawal(bool bIgnoreCommanders = false) const; // Exposed to Python
	void changeExtraWithdrawal(int iChange);

	//TB Combat Mods Start
	int getExtraAttackCombatModifier (bool bIgnoreCommanders = false) const;
	void changeExtraAttackCombatModifier (int iChange);

	int getExtraDefenseCombatModifier (bool bIgnoreCommanders = false) const;
	void changeExtraDefenseCombatModifier (int iChange);

	int getExtraPursuit (bool bIgnoreCommanders = false) const;
	void changeExtraPursuit (int iChange);

	int getExtraEarlyWithdraw (bool bIgnoreCommanders = false) const;
	void changeExtraEarlyWithdraw (int iChange);

	int getExtraVSBarbs (bool bIgnoreCommanders = false) const;
	void changeExtraVSBarbs (int iChange);

	int getExtraReligiousCombatModifier(bool bIgnoreCommanders = false) const;
	void changeExtraReligiousCombatModifier(int iChange);

	int getExtraArmor (bool bIgnoreCommanders = false) const;
	void changeExtraArmor (int iChange);

	int getExtraPuncture (bool bIgnoreCommanders = false) const;
	void changeExtraPuncture (int iChange);

	int getExtraDamageModifier (bool bIgnoreCommanders = false) const;
	void changeExtraDamageModifier (int iChange);

	int getExtraCostModifier () const;
	void changeExtraCostModifier (int iChange);

	int getExtraUnitCost100 () const;

	int getExtraOverrun (bool bIgnoreCommanders = false) const;
	void changeExtraOverrun (int iChange);

	int getExtraRepel (bool bIgnoreCommanders = false) const;
	void changeExtraRepel (int iChange);

	int getExtraFortRepel (bool bIgnoreCommanders = false) const;
	void changeExtraFortRepel (int iChange);

	int getExtraRepelRetries (bool bIgnoreCommanders = false) const;
	void changeExtraRepelRetries (int iChange);

	int getExtraUnyielding (bool bIgnoreCommanders = false) const;
	void changeExtraUnyielding (int iChange);

	int getExtraKnockback (bool bIgnoreCommanders = false) const;
	void changeExtraKnockback (int iChange);

	int getExtraKnockbackRetries (bool bIgnoreCommanders = false) const;
	void changeExtraKnockbackRetries (int iChange);

	int getStampedeCount() const;
	bool cannotStampede() const;
	bool mayStampede() const; // Exposed to Python
	void changeStampedeCount(int iChange);

	int getAttackOnlyCitiesCount() const;
	void setAttackOnlyCitiesCount(int iChange);
	void changeAttackOnlyCitiesCount(int iChange);

	int getIgnoreNoEntryLevelCount() const;
	void setIgnoreNoEntryLevelCount(int iChange);
	void changeIgnoreNoEntryLevelCount(int iChange);

	int getIgnoreZoneofControlCount() const;
	void setIgnoreZoneofControlCount(int iChange);
	void changeIgnoreZoneofControlCount(int iChange);

	int getFliesToMoveCount() const;
	void setFliesToMoveCount(int iChange);
	void changeFliesToMoveCount(int iChange);

	int getExtraStrAdjperRnd (bool bIgnoreCommanders = false) const;
	void changeExtraStrAdjperRnd (int iChange);

	int getExtraStrAdjperAtt (bool bIgnoreCommanders = false) const;
	void changeExtraStrAdjperAtt (int iChange);

	int getExtraStrAdjperDef (bool bIgnoreCommanders = false) const;
	void changeExtraStrAdjperDef (int iChange);

	int getExtraWithdrawAdjperAtt (bool bIgnoreCommanders = false) const;
	void changeExtraWithdrawAdjperAtt (int iChange);

	int getExtraUnnerve (bool bIgnoreCommanders = false) const;
	void changeExtraUnnerve (int iChange);

	int getExtraEnclose (bool bIgnoreCommanders = false) const;
	void changeExtraEnclose (int iChange);

	int getExtraLunge (bool bIgnoreCommanders = false) const;
	void changeExtraLunge (int iChange);

	int getExtraDynamicDefense (bool bIgnoreCommanders = false) const;
	void changeExtraDynamicDefense (int iChange);

	int getExtraStrength () const;
	void changeExtraStrength (int iChange);

	int getSMStrength () const;
	void setSMStrength ();

	int getAnimalIgnoresBordersCount() const;
	bool mayAnimalIgnoresBorders() const; // Exposed to Python
	void changeAnimalIgnoresBordersCount(int iChange);

	int getOnslaughtCount() const;
	bool mayOnslaught() const; // Exposed to Python
	void changeOnslaughtCount(int iChange);

	int getSubCombatTypeCount(UnitCombatTypes eCombatType) const;
	bool hasExtraSubCombatType(UnitCombatTypes eCombatType) const; // Exposed to Python
	void changeSubCombatTypeCount(UnitCombatTypes eCombatType, int iChange);

	int getRemovesUnitCombatTypeCount(UnitCombatTypes eCombatType) const;
	bool hasRemovesUnitCombatType(UnitCombatTypes eCombatType) const; // Exposed to Python
	void changeRemovesUnitCombatTypeCount(UnitCombatTypes eCombatType, int iChange);

	int getCureAfflictionCount(PromotionLineTypes ePromotionLineType) const;
	bool hasExtraCureAffliction(PromotionLineTypes ePromotionLineType) const; // Exposed to Python
	void changeCureAfflictionCount(PromotionLineTypes ePromotionLineType, int iChange);

	int getExtraFortitude (bool bIgnoreCommanders = false) const;
	void changeExtraFortitude (int iChange);

	int getExtraDodgeModifier (bool bIgnoreCommanders = false) const;
	void changeExtraDodgeModifier (int iChange);

	int getExtraPrecisionModifier (bool bIgnoreCommanders = false) const;
	void changeExtraPrecisionModifier (int iChange);

	int getExtraPowerShots (bool bIgnoreCommanders = false) const;
	void changeExtraPowerShots (int iChange);

	int getExtraPowerShotCombatModifier (bool bIgnoreCommanders = false) const;
	void changeExtraPowerShotCombatModifier (int iChange);

	int getExtraPowerShotPunctureModifier (bool bIgnoreCommanders = false) const;
	void changeExtraPowerShotPunctureModifier (int iChange);

	int getExtraPowerShotPrecisionModifier (bool bIgnoreCommanders = false) const;
	void changeExtraPowerShotPrecisionModifier (int iChange);

	int getExtraPowerShotCriticalModifier (bool bIgnoreCommanders = false) const;
	void changeExtraPowerShotCriticalModifier (int iChange);

	int getExtraCriticalModifier (bool bIgnoreCommanders = false) const;
	void changeExtraCriticalModifier (int iChange);

	int getExtraEndurance (bool bIgnoreCommanders = false) const;
	void changeExtraEndurance (int iChange);

	int getDealColdDamageCount() const;
	bool cannotDealColdDamage() const;
	bool mayDealColdDamage() const; // Exposed to Python
	void changeDealColdDamageCount(int iChange);

	int getColdImmuneCount() const;
	bool cannotColdImmune() const;
	bool mayColdImmune() const; // Exposed to Python
	void changeColdImmuneCount(int iChange);

	int getExtraPoisonProbabilityModifier (bool bIgnoreCommanders = false) const;
	void changeExtraPoisonProbabilityModifier (int iChange);
	//TB Combat Mods End

	int getExtraCollateralDamage() const; // Exposed to Python
	void changeExtraCollateralDamage(int iChange);

	int getExtraEnemyHeal() const; // Exposed to Python
	void changeExtraEnemyHeal(int iChange);

	int getExtraNeutralHeal() const; // Exposed to Python
	void changeExtraNeutralHeal(int iChange);

	int getExtraFriendlyHeal() const; // Exposed to Python
	void changeExtraFriendlyHeal(int iChange);

	int getSameTileHeal() const; // Exposed to Python
	void changeSameTileHeal(int iChange);

	int getAdjacentTileHeal() const; // Exposed to Python
	void changeAdjacentTileHeal(int iChange);

	int getExtraCombatPercent() const; // Exposed to Python
	void changeExtraCombatPercent(int iChange);

	//ls612: Work Rate Modifiers
	int getExtraWorkPercent() const;
	void changeExtraWorkPercent(int iChange);

	int getExtraCityAttackPercent() const; // Exposed to Python
	void changeExtraCityAttackPercent(int iChange);

	int getExtraCityDefensePercent() const; // Exposed to Python
	void changeExtraCityDefensePercent(int iChange);

	int getExtraHillsAttackPercent() const; // Exposed to Python
	void changeExtraHillsAttackPercent(int iChange);

	int getExtraHillsDefensePercent() const; // Exposed to Python
	void changeExtraHillsDefensePercent(int iChange);

	//Team Project (4)
	//WorkRateMod
	int getExtraHillsWorkPercent() const;
	void changeExtraHillsWorkPercent(int iChange);
	int hillsWorkModifier() const;

	int getExtraPeaksWorkPercent() const;
	void changeExtraPeaksWorkPercent(int iChange);
	int peaksWorkModifier() const;

	int getCollateralDamageProtection() const; // Exposed to Python
	void changeCollateralDamageProtection(int iChange);

	int getPillageChange() const; // Exposed to Python
	void changePillageChange(int iChange);

	int getUpgradeDiscount() const; // Exposed to Python
	void changeUpgradeDiscount(int iChange);

	int getExperiencePercent() const; // Exposed to Python
	void changeExperiencePercent(int iChange);

	int getKamikazePercent() const; // Exposed to Python
	void changeKamikazePercent(int iChange);

	DllExport DirectionTypes getFacingDirection(bool checkLineOfSightProperty) const;
	void setFacingDirection(DirectionTypes facingDirection);
	void rotateFacingDirectionClockwise();
	void rotateFacingDirectionCounterClockwise();

	DllExport bool isSuicide() const; // Exposed to Python
	int getDropRange() const;

	bool isMadeAttack() const; // Exposed to Python
	void setMadeAttack(bool bNewValue); // Exposed to Python
	//TB Combat Mods (Att&DefCounters)
	int getRoundCount () const;
	void changeRoundCount (int iChange);
	int getAttackCount() const;
	void changeAttackCount(int iChange);
	int getDefenseCount() const;
	void changeDefenseCount(int iChange);
	//TB Combat Mods end

	bool isMadeInterception() const; // Exposed to Python
	void setMadeInterception(bool bNewValue); // Exposed to Python

	DllExport bool isPromotionReady() const; // Exposed to Python
	void setPromotionReady(bool bNewValue); // Exposed to Python
	void testPromotionReady();

	bool isDelayedDeath() const;
	void startDelayedDeath();
	bool doDelayedDeath();

	bool isCombatFocus() const;

	DllExport bool isInfoBarDirty() const;
	DllExport void setInfoBarDirty(bool bNewValue);

	bool isBlockading() const;
	void setBlockading(bool bNewValue);
	void collectBlockadeGold();

	DllExport inline PlayerTypes getOwner() const { return m_eOwner; } // Exposed to Python
	DllExport PlayerTypes getVisualOwner(TeamTypes eForTeam = NO_TEAM) const; // Exposed to Python
	PlayerTypes getCombatOwner(TeamTypes eForTeam, const CvPlot* pPlot) const; // Exposed to Python
	DllExport TeamTypes getTeam() const; // Exposed to Python

	PlayerTypes getCapturingPlayer() const;
	void setCapturingPlayer(PlayerTypes eNewValue);

	CvUnit* getCapturingUnit() const;
	void setCapturingUnit(CvUnit* pCapturingUnit);

	DllExport const UnitTypes getUnitType() const; // Exposed to Python
	CvUnitInfo &getUnitInfo() const;

	DllExport const UnitTypes getLeaderUnitType() const;
	void setLeaderUnitType(UnitTypes leaderUnitType);

	DllExport CvUnit* getCombatUnit() const;
	void setCombatUnit(CvUnit* pUnit, bool bAttacking = false, bool bStealthAttack = false, bool bStealthDefense = false);
	bool showSeigeTower(const CvUnit* pDefender) const; // K-Mod

	CvUnit* getTransportUnit() const; // Exposed to Python
	bool isCargo() const; // Exposed to Python
	void setTransportUnit(CvUnit* pTransportUnit); // Exposed to Python

	int getExtraDomainModifier(DomainTypes eIndex) const; // Exposed to Python
	void changeExtraDomainModifier(DomainTypes eIndex, int iChange);

	DllExport const CvWString getName(uint uiForm = 0) const; // Exposed to Python
// BUG - Unit Name - start
	bool isDescInName() const;
// BUG - Unit Name - end
	const wchar* getNameKey() const; // Exposed to Python
	const CvWString& getNameNoDesc() const; // Exposed to Python
	void setName(const CvWString szNewValue); // Exposed to Python

	// Script data needs to be a narrow string for pickling in Python
	std::string getScriptData() const; // Exposed to Python
	void setScriptData(std::string szNewValue); // Exposed to Python

	int getTerrainDoubleMoveCount(TerrainTypes eIndex) const;
	bool isTerrainDoubleMove(TerrainTypes eIndex) const; // Exposed to Python
	void changeTerrainDoubleMoveCount(TerrainTypes eIndex, int iChange);

	int getFeatureDoubleMoveCount(FeatureTypes eIndex) const;
	bool isFeatureDoubleMove(FeatureTypes eIndex) const; // Exposed to Python
	void changeFeatureDoubleMoveCount(FeatureTypes eIndex, int iChange);

	int getExtraTerrainAttackPercent(TerrainTypes eIndex) const; // Exposed to Python
	void changeExtraTerrainAttackPercent(TerrainTypes eIndex, int iChange);
	int getExtraTerrainDefensePercent(TerrainTypes eIndex) const; // Exposed to Python
	void changeExtraTerrainDefensePercent(TerrainTypes eIndex, int iChange);
	int getExtraFeatureAttackPercent(FeatureTypes eIndex) const; // Exposed to Python
	void changeExtraFeatureAttackPercent(FeatureTypes eIndex, int iChange);
	int getExtraFeatureDefensePercent(FeatureTypes eIndex) const; // Exposed to Python
	void changeExtraFeatureDefensePercent(FeatureTypes eIndex, int iChange);

	//ls612: Terrain Work Modifiers
//Team Project (4)
//WorkRateMod
	int getTerrainWorkPercent(TerrainTypes eIndex) const;
	//void changeTerrainWorkPercent (TerrainTypes eIndex, int iChange);
	int getFeatureWorkPercent(FeatureTypes eIndex) const;
	//void changeFeatureWorkPercent(FeatureTypes eIndex, int iChange);
	int getExtraTerrainWorkPercent(TerrainTypes eIndex) const;
	void changeExtraTerrainWorkPercent (TerrainTypes eIndex, int iChange);
	int getExtraFeatureWorkPercent(FeatureTypes eIndex) const;
	void changeExtraFeatureWorkPercent(FeatureTypes eIndex, int iChange);
	int getExtraBuildWorkPercent(BuildTypes eIndex) const;
	void changeExtraBuildWorkPercent(BuildTypes eIndex, int iChange);
	int terrainWorkPercent(TerrainTypes eIndex) const;
	int featureWorkPercent(FeatureTypes eIndex) const;
	int buildWorkPercent(BuildTypes eIndex) const;

	int getExtraUnitCombatModifier(UnitCombatTypes eIndex) const; // Exposed to Python
	void changeExtraUnitCombatModifier(UnitCombatTypes eIndex, int iChange);
	//TB Combat Mods (adjusted the following line to include ", bool bEquip = false, bool bAfflict = false, bool bPromote = false"
	bool canAcquirePromotion(PromotionTypes ePromotion, PromotionRequirements::flags requirements) const;
	// Deprecated, use the one above that takes enum flags instead for increased readability.
	bool canAcquirePromotion(PromotionTypes ePromotion, bool bIgnoreHas = false, bool bEquip = false, bool bAfflict = false, bool bPromote = false, bool bForLeader = false, bool bForOffset = false, bool bForFree = false, bool bForBuildUp = false, bool bForStatus = false) const; // Exposed to Python
	//TB Combat Mods end
	bool canAcquirePromotionAny() const; // Exposed to Python
	bool isPromotionValid(PromotionTypes ePromotion, bool bKeepCheck = false) const;
	bool isHealsUnitCombat(UnitCombatTypes eIndex) const;
	bool isHasUnitCombat(UnitCombatTypes eIndex) const;
	void processUnitCombat(UnitCombatTypes eIndex, bool bAdding, bool bByPromo = false);
	void setHasUnitCombat(UnitCombatTypes eIndex, bool bNewValue, bool bByPromo = false);
	bool isHasPromotion(PromotionTypes eIndex) const; // Exposed to Python

	void setHasPromotion(PromotionTypes eIndex, bool bNewValue, PromotionApply::flags flags);
	// Deprecated, use the one above that takes enum flags instead for increased readability.
	void setHasPromotion(PromotionTypes eIndex, bool bNewValue, bool bFree = true, bool bDying = false, bool bInitial = false, bool bFromTrait = false); // Exposed to Python

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
	DllExport int getSubUnitsAlive() const;
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

	int getImmobileTimer() const; // Exposed to Python
	void setImmobileTimer(int iNewValue); // Exposed to Python
	void changeImmobileTimer(int iChange);

//Team Project (2)
	bool isCanRespawn() const;
	void setCanRespawn(bool bNewValue);

	bool isSurvivor() const;
	void setSurvivor(bool bNewValue);

	bool potentialWarAction(const CvPlot* pPlot) const;
	bool willRevealByMove(const CvPlot* pPlot) const;

	bool isAlwaysHostile(const CvPlot* pPlot) const;

	bool verifyStackValid();

	DllExport const CvArtInfoUnit* getArtInfo(int i, EraTypes eEra) const; // Exposed to Python
	DllExport const TCHAR* getButton() const; // Exposed to Python
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

/************************************************************************************************/
/* DCM                                     04/19/09                                Johny Smith  */
/************************************************************************************************/
// Dale - AB: Bombing START
	bool canAirBomb1(const CvPlot* pPlot) const;
	bool canAirBomb1At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb1(int iX, int iY);
	bool canAirBomb2(const CvPlot* pPlot) const;
	bool canAirBomb2At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb2(int iX, int iY);
	bool canAirBomb3(const CvPlot* pPlot) const;
	bool canAirBomb3At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb3(int iX, int iY);
	bool canAirBomb4(const CvPlot* pPlot) const;
	bool canAirBomb4At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb4(int iX, int iY);
	bool canAirBomb5(const CvPlot* pPlot) const;
	bool canAirBomb5At(const CvPlot* pPlot, int iX, int iY) const;
	bool airBomb5(int iX, int iY);
// Dale - AB: Bombing END
// Dale - RB: Field Bombard START
	bool canRBombard(bool bEver = false) const;
	bool canBombardAtRanged(const CvPlot* pPlot, int iX, int iY) const;
	bool bombardRanged(int iX, int iY, bool sAttack = false);
	// RevolutionDCM - ranged bombard
	bool isRbombardable(int iMinStack);
	int getRbombardSeigeCount(CvPlot* pPlot);
	// RevolutionDCM - end
// Dale - SA: Opp Fire START
	void doOpportunityFire();
// Dale - SA: Opp Fire END
// Dale - SA: Active Defense START
	void doActiveDefense();
// Dale - SA: Active Defense END
// Dale - FE: Fighters START
	bool canFEngage() const;
	bool canFEngageAt(const CvPlot* pPlot, int iX, int iY) const;
	bool fighterEngage(int iX, int iY);
// Dale - FE: Fighters END
/************************************************************************************************/
/* RevolutionDCM	                  Start		 05/31/10                        Afforess       */
/*                                                                                              */
/* Battle Effects                                                                               */
/************************************************************************************************/
	void setBattlePlot(CvPlot* pPlot, const CvUnit* pDefender = NULL);
/************************************************************************************************/
/* RevolutionDCM	             Battle Effects END                                             */
/************************************************************************************************/
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

	virtual void AI_init(UnitAITypes eUnitAI, int iBirthmark) = 0;
	virtual void AI_uninit() = 0;
	virtual void AI_reset(UnitAITypes eUnitAI = NO_UNITAI, bool bConstructor = false) = 0;
	virtual bool AI_update() = 0;
	virtual bool AI_follow() = 0;
	virtual bool AI_upgrade() = 0;
	virtual bool AI_promote() = 0;
	virtual int AI_groupFirstVal() = 0;
	virtual int AI_groupSecondVal() = 0;
	virtual int AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy, CvUnit** ppDefender = NULL, bool bAssassinate = false) = 0;
	//	Variant to test a specific defender AS IF it was in the specified plot
	virtual int AI_attackOddsAtPlot(const CvPlot* pPlot, CvUnit* pDefender, bool modifyPredictedResults = false) = 0;
	virtual bool AI_bestCityBuild(CvCity* pCity, CvPlot** ppBestPlot = NULL, BuildTypes* peBestBuild = NULL, CvPlot* pIgnorePlot = NULL, CvUnit* pUnit = NULL) = 0;
	virtual bool AI_isCityAIType() const = 0;
	virtual UnitAITypes AI_getUnitAIType() const = 0; // Exposed to Python
	virtual void AI_setUnitAIType(UnitAITypes eNewValue) = 0;
	virtual int AI_sacrificeValue(const CvPlot* pPlot) const = 0;
	virtual bool AI_isAwaitingContract() const = 0;
	virtual bool AI_isCityGarrison(const CvCity* pCity) const = 0;
	virtual void AI_setAsGarrison(const CvCity* pCity) = 0;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/05/10                                jdog5000      */
/*                                                                                              */
/* Unit AI                                                                                      */
/************************************************************************************************/
	virtual bool AI_load(UnitAITypes eUnitAI, MissionAITypes eMissionAI, UnitAITypes eTransportedUnitAI = NO_UNITAI, int iMinCargo = -1, int iMinCargoSpace = -1, int iMaxCargoSpace = -1, int iMaxCargoOurUnitAI = -1, int iFlags = 0, int iMaxPath = MAX_INT, int iMaxTransportPath = MAX_INT) = 0;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	virtual void AI_flushValueCache() = 0;
	virtual int	AI_genericUnitValueTimes100(UnitValueFlags eFlags) const = 0;
	virtual int AI_getBirthmark() const = 0;
	virtual void setToWaitOnUnitAI(UnitAITypes eUnitAI, bool bAdd) = 0;
	virtual bool isWaitingOnUnitAI(int iIndex) = 0;
	virtual bool isWaitingOnUnitAIAny() = 0;

	inline int getMovementCharacteristicsHash() const { return m_movementCharacteristicsHash; }

	PlayerTypes m_eOriginalOwner;

protected:
	int m_iDCMBombRange;
	int m_iDCMBombAccuracy;
	int m_iHealUnitCombatCount;
	std::vector<int> m_aiExtraBuildTypes;

	DomainTypes m_eNewDomainCargo;
	SpecialUnitTypes m_eNewSpecialCargo;
	SpecialUnitTypes m_eNewSMSpecialCargo;
	SpecialUnitTypes m_eNewSMNotSpecialCargo;
	SpecialUnitTypes m_eSpecialUnit;
	MissionTypes m_eSleepType;
	PromotionLineTypes m_eCurrentBuildUpType;

	bool m_bHiddenNationality;
	bool m_bHasHNCapturePromotion;
/************************************************************************************************/
/* Afforess	                  Start		 06/14/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	int m_iCanMovePeaksCount;
	//	Koshling - enhanced mountaineering mode to differentiate between ability to move through
	//	mountains, and ability to lead a stack through mountains
	int m_iCanLeadThroughPeaksCount;

	int m_movementCharacteristicsHash;

	PlayerTypes m_eNationality;
	CombatResult m_combatResult;
	int m_iSleepTimer;
	bool m_bCommander;
	int m_iZoneOfControlCount;

	bool m_bAutoPromoting;
	bool m_bAutoUpgrading;
	IDInfo m_shadowUnit;
	TechTypes m_eDesiredDiscoveryTech;
	//Great Commanders... By KillmePlease
	int m_iExtraControlPoints;
	int m_iExtraCommandRange;
	//auxillary members:
	int m_iControlPointsLeft;
	int m_iCommanderID; //id of commander. used for game save/load
	mutable int m_iCommanderCacheTurn;
	mutable int m_iCachedCommander;
#define	NO_COMMANDER_ID	-2	//	Pseudo-id used to signify an assertion that the unit has no commander
	int m_iPreCombatDamage;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

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
	int m_iBlitzCount;
	int m_iRBombardForceAbilityCount;
	int m_iAmphibCount;
	int m_iRiverCount;
	int m_iEnemyRouteCount;
	int m_iAlwaysHealCount;
	int m_iHillsDoubleMoveCount;
	int m_iImmuneToFirstStrikesCount;
	int m_iAlwaysInvisibleCount;
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 16.09.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: New Tag Definition                                                               **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	int m_iDefensiveVictoryMoveCount;
	int m_iFreeDropCount;
	int m_iOffensiveVictoryMoveCount;
//Team Project (2)
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
//Team Project (2)
	int m_iSurvivorChance;
	int m_iVictoryAdjacentHeal;
	int m_iVictoryHeal;
//Team Project (2)
	int m_iVictoryStackHeal;
/*****************************************************************************************************/
/**  TheLadiesOgre; 16.09.2009; TLOTags                                                             **/
/*****************************************************************************************************/
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
	bool m_bHasAnyInvisibilityAbility;
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
//Team Project (3)
	int m_iExtraCaptureProbabilityModifier;
	int m_iExtraCaptureResistanceModifier;
	//
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
	int m_iExtraCostModifier;
	int m_iExtraPowerValue;
	int m_iExtraAssetValue;
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
	//ls612: Work Rate Modifiers
	int m_iExtraWorkPercent;
	int m_iExtraCityAttackPercent;
	int m_iExtraCityDefensePercent;
	int m_iExtraHillsAttackPercent;
	int m_iExtraHillsDefensePercent;
	//Team Project (4)
	//WorkRateMod
	int m_iExtraHillsWorkPercent;
	int m_iExtraPeaksWorkPercent;
	//
	int m_iRevoltProtection;
	int m_iCollateralDamageProtection;
	int m_iPillageChange;
	int m_iUpgradeDiscount;
	int m_iExperiencePercent;
	int m_iKamikazePercent;
	int m_iBaseCombat;
	DirectionTypes m_eFacingDirection;
	int m_iImmobileTimer;
//Team Project (2)
/*****************************************************************************************************/
/**  Author: TheLadiesOgre                                                                          **/
/**  Date: 21.09.2009                                                                               **/
/**  ModComp: TLOTags                                                                               **/
/**  Reason Added: New Tag Definition                                                               **/
/**  Notes:                                                                                         **/
/*****************************************************************************************************/
	bool m_bCanRespawn;
	bool m_bSurvivor;
/*****************************************************************************************************/
/**  TheLadiesOgre; 21.09.2009; TLOTags                                                             **/
/*****************************************************************************************************/

	bool m_bMadeAttack;
	//TB Combat Mods (Att&DefCounters)
	int m_iRoundCount;
	int m_iAttackCount;
	int m_iDefenseCount;
	//TB Combat Mods end
	bool m_bMadeInterception;
	bool m_bPromotionReady;
	bool m_bDeathDelay;
	bool m_bCombatFocus;
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

//Team Project (4)
	//WorkRateMod
	std::map<short, short> m_extraBuildWorkPercent;

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
	void flankingStrikeCombat(const CvPlot* pPlot, int iAttackerStrength, int iAttackerFirepower, int iDefenderOdds, int iDefenderDamage, CvUnit* pSkipUnit = NULL, bool bSamePlot = false);

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

	void getDefenderCombatValues(CvUnit& kDefender, const CvPlot* pPlot, int iOurStrength, int iOurFirepower,
		int& iTheirOdds, int& iTheirStrength, int& iOurDamage, int& iTheirDamage,
		CombatDetails* pTheirDetails = NULL, CvUnit* pDefender = NULL, bool bSamePlot = false) const;

	bool isCombatVisible(const CvUnit* pDefender) const;
	void resolveCombat(CvUnit* pDefender, CvPlot* pPlot, CvBattleDefinition& kBattle, bool bSamePlot = false);
	void resolveAirCombat(CvUnit* pInterceptor, CvPlot* pPlot, CvAirMissionDefinition& kBattle);
	void checkRemoveSelectionAfterAttack();

	// ------ BEGIN InfluenceDrivenWar -------------------------------
	float doVictoryInfluence(CvUnit* pLoserUnit, bool bAttacking, bool bWithdrawal);
	void influencePlots(CvPlot* pCentralPlot, PlayerTypes eTargetPlayer, float fLocationMultiplier);
	float doPillageInfluence();
	// ------ END InfluenceDrivenWar ---------------------------------

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
/*                                                                                              */
/* Lead From Behind                                                                             */
/************************************************************************************************/
// From Lead From Behind by UncutDragon
public:
	int defenderValue(const CvUnit* pAttacker) const;
	bool isBetterDefenderThan(const CvUnit* pDefender, const CvUnit* pAttacker, int* pBestDefenderRank) const;
protected:
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
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
	virtual bool AI_getHasAttacked() = 0;
	virtual int AI_beneficialPropertyValueToCity(CvCity* pCity, PropertyTypes eProperty) const = 0;

	bool isUsingDummyEntities() const;
	static bool isDummyEntity(const CvEntity* entity);
	static bool isRealEntity(const CvEntity* entity);

	std::map<UnitCombatTypes, UnitCombatKeyedInfo>&  getUnitCombatKeyedInfo() const;
	std::map<PromotionLineTypes, PromotionLineKeyedInfo>&  getPromotionLineKeyedInfo() const;

private:
	bool	bGraphicsSetup;
	int m_iXOrigin;
	int m_iYOrigin;

	//TB Combat Mods begin
public:
	bool isArcher() const;
	bool isPromotionOverriden(PromotionTypes ePromotionType) const;
	bool canCure(CvPlot* pPlot, PromotionLineTypes eAfflictionLine) const;
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

	int getCityRepel() const;

#ifdef STRENGTH_IN_NUMBERS
	int getCityFrontSupportPercentModifier() const;
	int getCityShortRangeSupportPercentModifier() const;
	int getCityMediumRangeSupportPercentModifier() const;
	int getCityLongRangeSupportPercentModifier() const;
	int getCityFlankSupportPercentModifier() const;

	int getExtraFrontSupportPercent (bool bIgnoreCommanders = false) const;
	void changeExtraFrontSupportPercent (int iChange);

	int getExtraShortRangeSupportPercent (bool bIgnoreCommanders = false) const;
	void changeExtraShortRangeSupportPercent (int iChange);

	int getExtraMediumRangeSupportPercent (bool bIgnoreCommanders = false) const;
	void changeExtraMediumRangeSupportPercent (int iChange);

	int getExtraLongRangeSupportPercent (bool bIgnoreCommanders = false) const;
	void changeExtraLongRangeSupportPercent (int iChange);

	int getExtraFlankSupportPercent (bool bIgnoreCommanders = false) const;
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

	int getOngoingTrainingCount(UnitCombatTypes eUnitCombatType) const;
	void changeOngoingTrainingCount(UnitCombatTypes eUnitCombatType, int iChange);
	void setOngoingTrainingCount(UnitCombatTypes eUnitCombatType, int iChange);

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

	void checkForCritical(int iDamage, CvUnit* pOpponent);
	void assignCritical(CvUnit* pOpponent);

	bool canKeepPromotion(PromotionTypes ePromotion, bool bAssertFree = false, bool bMessageOnFalse = false) const;
	bool isPromotionFree(PromotionTypes ePromotion) const;
	int getPromotionFreeCount(PromotionTypes ePromotion) const;
	void changePromotionFreeCount(PromotionTypes ePromotion, int iChange);
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
	int getExtraFlankingStrengthbyUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraFlankingStrengthbyUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraFlankingStrikebyUnitCombatType(UnitCombatTypes eIndex) const;

	int withdrawOnTerrainTotal(TerrainTypes eTerrainType) const;
	int getExtraWithdrawOnTerrainType(TerrainTypes eIndex) const;
	void changeExtraWithdrawOnTerrainType(TerrainTypes eIndex, int iChange);
	bool hasExtraWithdrawOnTerrainType(TerrainTypes eIndex) const;

	int withdrawOnFeatureTotal(FeatureTypes eFeatureType) const;
	int getExtraWithdrawOnFeatureType(FeatureTypes eIndex) const;
	void changeExtraWithdrawOnFeatureType(FeatureTypes eIndex, int iChange);
	bool hasExtraWithdrawOnFeatureType(FeatureTypes eIndex) const;

	int withdrawVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraWithdrawVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraWithdrawVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraWithdrawVSUnitCombatType(UnitCombatTypes eIndex) const;

	int pursuitVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraPursuitVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraPursuitVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraPursuitVSUnitCombatType(UnitCombatTypes eIndex) const;

	int repelVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraRepelVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraRepelVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraRepelVSUnitCombatType(UnitCombatTypes eIndex) const;

	int knockbackVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraKnockbackVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraKnockbackVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraKnockbackVSUnitCombatType(UnitCombatTypes eIndex) const;

	int punctureVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraPunctureVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraPunctureVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraPunctureVSUnitCombatType(UnitCombatTypes eIndex) const;

	int armorVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraArmorVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraArmorVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraArmorVSUnitCombatType(UnitCombatTypes eIndex) const;

	int dodgeVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraDodgeVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraDodgeVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraDodgeVSUnitCombatType(UnitCombatTypes eIndex) const;

	int precisionVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraPrecisionVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraPrecisionVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraPrecisionVSUnitCombatType(UnitCombatTypes eIndex) const;

	int criticalVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraCriticalVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraCriticalVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraCriticalVSUnitCombatType(UnitCombatTypes eIndex) const;

	int roundStunVSUnitCombatTotal(UnitCombatTypes eCombatType) const;
	int getExtraRoundStunVSUnitCombatType(UnitCombatTypes eIndex) const;
	void changeExtraRoundStunVSUnitCombatType(UnitCombatTypes eIndex, int iChange);
	bool hasExtraRoundStunVSUnitCombatType(UnitCombatTypes eIndex) const;

	int getExtraRoundStunProb (bool bIgnoreCommanders = false) const;
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

	int getAfflictionLineCount(PromotionLineTypes ePromotionLineType) const;
	bool hasAfflictionLine(PromotionLineTypes ePromotionLineType) const; // Exposed to Python
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

	bool hasHealUnitCombat() const;
	int getHealUnitCombatCount() const;
	void setHealUnitCombatCount();
	int getHealUnitCombatTypeTotal(UnitCombatTypes eUnitCombatType) const;
	void changeHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombatType, int iChange);
	void setHealUnitCombatTypeVolume(UnitCombatTypes eUnitCombatType, int iChange);
	int getHealUnitCombatTypeAdjacentTotal(UnitCombatTypes eUnitCombatType) const;
	void changeHealUnitCombatTypeAdjacentVolume(UnitCombatTypes eUnitCombatType, int iChange);
	void setHealUnitCombatTypeAdjacentVolume(UnitCombatTypes eUnitCombatType, int iChange);

	void doSetUnitCombats();
	void doSetFreePromotions(bool bAdding, TraitTypes eTrait = NO_TRAIT);
	void setFreePromotion(PromotionTypes ePromotion, bool bAdding, TraitTypes eTrait = NO_TRAIT);

	int getRetrainsAvailable() const;
	void setRetrainsAvailable(int iNewValue);
	void changeRetrainsAvailable(int iChange);

	int getExperiencefromWithdrawal(const int iWithdrawalProbability) const;

//Team Project (3)
	int getExtraCaptureProbabilityModifier(bool bIgnoreCommanders = false) const;
	void changeExtraCaptureProbabilityModifier(int iChange);
	int captureProbabilityTotal() const;

	int getExtraCaptureResistanceModifier (bool bIgnoreCommanders = false) const;
	void changeExtraCaptureResistanceModifier(int iChange);
	int captureResistanceTotal() const;
	//
	int getExtraBreakdownChance (bool bIgnoreCommanders = false) const;
	void changeExtraBreakdownChance(int iChange);
	int breakdownChanceTotal() const;

	int getExtraBreakdownDamage (bool bIgnoreCommanders = false) const;
	void changeExtraBreakdownDamage(int iChange);
	int breakdownDamageTotal() const;

	int getExtraTaunt() const;
	void changeExtraTaunt(int iChange);
	void setExtraTaunt(int iChange);
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
	void resolveBreakdownAttack(const CvPlot* pPlot, CvUnit* pDefender, const int AdjustedRepel);

	int getDiminishingReturn(int i) const;
	int getApproaching0Return(int i) const;

	bool isPursuitPossible(const CvUnit* pOpponent) const;

	bool hasCannotMergeSplit() const;
	int getCannotMergeSplitCount() const;
	void changeCannotMergeSplitCount(int iNewValue);

	int getQualityBaseTotal() const;
	void setQualityBaseTotal(int iNewValue);
	void changeQualityBaseTotal(int iChange);

	int getGroupBaseTotal() const;
	void setGroupBaseTotal(int iNewValue);
	void changeGroupBaseTotal(int iChange);

	int getSizeBaseTotal() const;
	void setSizeBaseTotal(int iNewValue);
	void changeSizeBaseTotal(int iChange);

	int getExtraQuality() const;
	void setExtraQuality(int iNewValue);
	void changeExtraQuality(int iChange);

	int getExtraGroup() const;
	void setExtraGroup(int iNewValue);
	void changeExtraGroup(int iChange);

	int getExtraSize() const;
	void setExtraSize(int iNewValue);
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
	void setNewSMSpecialCargo(SpecialUnitTypes eSpecialUnit);
	SpecialUnitTypes getSMNotSpecialCargo() const;
	void setNewSMNotSpecialCargo(SpecialUnitTypes eSpecialUnit);

	void changeSMCargoSpace(int iChange);
	int SMcargoSpace() const;
	int SMcargoSpaceFilter() const;
	int SMcargoCapacityPreCheck() const;
	int getSMCargoCapacity() const;
	void setSMCargoCapacity();

	int getExtraMaxHP() const;
	void changeExtraMaxHP(int iChange);
	void setExtraMaxHP(int iChange);
	int maxHPTotal() const;
	int HPValueTotalPreCheck() const;
	int getSMHPValue() const;
	void setSMHPValue();

	int getExtraPowerValue() const;
	void changeExtraPowerValue(int iChange);
	int getPowerValueTotal() const;
	int getSMPowerValueTotalBase() const;
	int getSMPowerValue() const;
	void setSMPowerValue(bool bForLoad = false);

	int getExtraAssetValue() const;
	void changeExtraAssetValue(int iChange);
	int assetValueTotal() const;
	int assetValueTotalPreCheck() const;
	int getSMAssetValue() const;
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

	int getExtraBombardRate() const; // Exposed to Python
	void changeExtraBombardRate(int iChange);
	void setExtraBombardRate(int iChange);
	int getBombardRate() const;
	int getSMBombardRateTotalBase() const;
	int getSMBombardRate() const;
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

	int getRevoltProtection() const; // Exposed to Python
	void changeRevoltProtection(int iChange);
	int revoltProtectionTotal() const;
	int revoltProtectionTotalPreCheck() const;
	int getSMRevoltProtection() const;
	void setSMRevoltProtection();

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

	static int applySMRank(int value, int rankChange, int rankMultiplier);

	int getNoSelfHealCount() const;
	bool hasNoSelfHeal() const; // Exposed to Python
	void changeNoSelfHealCount(int iChange);

	int getSelfHealModifierTotal() const; // Exposed to Python
	void changeExtraSelfHealModifier(int iChange);

	int getNumHealSupportTotal() const; // Exposed to Python
	void changeExtraNumHealSupport(int iChange);

	int getHealSupportUsedTotal() const; // Exposed to Python
	void changeHealSupportUsed(int iChange); // Exposed to Python
	void setHealSupportUsed(int iChange);
	int getHealSupportRemaining() const;
	bool hasHealSupportRemaining() const;

	MissionTypes getSleepType() const;
	void setSleepType(MissionTypes eSleepType);
	void establishBuildups();
	PromotionLineTypes getBuildUpType() const;
	void setBuildUpType(PromotionLineTypes ePromotionLine = NO_PROMOTIONLINE, bool bRemove = false, MissionTypes eSleepType = NO_MISSION);
	void clearBuildups();
	bool isInhibitMerge() const;
	void setInhibitMerge(bool bNewValue);
	bool isInhibitSplit() const;
	void setInhibitSplit(bool bNewValue);
	bool isBuildUp() const;
	void setBuildUp(bool bNewValue);
	void setSpecialUnit(bool bChange, SpecialUnitTypes eSpecialUnit);
	bool isHiddenNationality() const;
	void doHNCapture();
	void removeHNCapturePromotion();

	void processLoadedSpecialUnit(bool bChange, SpecialUnitTypes eSpecialUnit);

	bool hasBuild(BuildTypes eBuild) const;
	bool isExtraBuild(BuildTypes eBuild) const;
	int getExtraBuildType(int i) const;
	int getNumExtraBuildTypes() const;
	void changeExtraBuildType(bool bChange, BuildTypes eBuild);

	bool isExcile() const;
	int getExcileCount() const;
	void changeExcileCount(int iChange);

	bool isPassage() const;
	int getPassageCount() const;
	void changePassageCount(int iChange);

	bool isNoNonOwnedCityEntry() const;
	int getNoNonOwnedCityEntryCount() const;
	void changeNoNonOwnedCityEntryCount(int iChange);

	bool isBarbCoExist() const;
	int getBarbCoExistCount() const;
	void changeBarbCoExistCount(int iChange);

	bool isBlendIntoCity() const;
	int getBlendIntoCityCount() const;
	void changeBlendIntoCityCount(int iChange);

	bool isUpgradeAnywhere() const;
	int getUpgradeAnywhereCount() const;
	void changeUpgradeAnywhereCount(int iChange);

	bool hasVisibilityType(InvisibleTypes eInvisibleType) const;
	int visibilityIntensityTotal(InvisibleTypes eInvisibleType) const;
	int getExtraVisibilityIntensityType(InvisibleTypes eIndex) const;
	void changeExtraVisibilityIntensityType(InvisibleTypes eIndex, int iChange);

	bool hasAnyInvisibilityType(bool bAbilityCheck = false) const;
	bool hasInvisibilityType(InvisibleTypes eInvisibleType, bool bAbilityCheck = false) const;
	int invisibilityIntensityTotal(InvisibleTypes eInvisibleType, bool bAbilityCheck = false) const;
	int getExtraInvisibilityIntensityType(InvisibleTypes eIndex) const;
	void changeExtraInvisibilityIntensityType(InvisibleTypes eIndex, int iChange);
	void setHasAnyInvisibility();

	//These need text displays on the unit
	bool hasVisibilityRangeType(InvisibleTypes eInvisibleType) const;
	int visibilityIntensityRangeTotal(InvisibleTypes eInvisibleType) const;
	int getExtraVisibilityIntensityRangeType(InvisibleTypes eIndex) const;
	void changeExtraVisibilityIntensityRangeType(InvisibleTypes eIndex, int iChange);

	int visibilityIntensitySameTileTotal(InvisibleTypes eInvisibleType) const;
	int getExtraVisibilityIntensitySameTileType(InvisibleTypes eIndex) const;
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

	int getNumExtraAidChanges() const;
	AidStruct& getExtraAidChange(int iIndex);
	void changeExtraAidChange(PropertyTypes eProperty, int iChange);
	int extraAidChange(PropertyTypes eProperty) const;

	void deleteVisibility();
	void addVisibility();

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

	bool canAmbush(CvPlot* pPlot, bool bAssassinate = false) const;
	bool doAmbush(bool bAssassinate = false);
	void enactAmbush(bool bAssassinate = false);

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

	int getOnlyDefensiveCount() const;
	void changeOnlyDefensiveCount(int iChange);

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

	int getTargetUnitCombatCount(UnitCombatTypes eUnitCombat) const;
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

	int getDistanceAttackCommunicability(PromotionLineTypes eAfflictionLine) const;
	void changeDistanceAttackCommunicability(PromotionLineTypes eAfflictionLine, int iChange);
	void setDistanceAttackCommunicability(PromotionLineTypes eAfflictionLine, int iValue);

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
	static int*	g_paiTempTerrainDoubleMoveCount;
	static int*	g_paiTempFeatureDoubleMoveCount;
	static int*	g_paiTempExtraTerrainAttackPercent;
	static int*	g_paiTempExtraTerrainDefensePercent;
	static int* g_paiTempAfflictOnAttackTypeMeleeCount;
	static int* g_paiTempAfflictOnAttackTypeDistanceCount;
//Team Project (4)
	//WorkRateMod
	//ls612: Terrain Work Modifiers
	static int* g_paiTempExtraBuildWorkPercent;
	static int* g_paiTempTerrainWorkPercent;
	static int* g_paiTempFeatureWorkPercent;
	static int* g_paiTempExtraTerrainWorkPercent;
	static int* g_paiTempExtraFeatureWorkPercent;
	static int*	g_paiTempExtraFeatureAttackPercent;
	static int*	g_paiTempExtraFeatureDefensePercent;
	static int*	g_paiTempExtraUnitCombatModifier;
	static bool*	g_pabTempHasPromotion;
	static bool*	g_pabTempHasUnitCombat;
	static int*	g_paiTempTerrainProtected;
	static int* g_paiTempSubCombatTypeCount;
	static int* g_paiTempOngoingTrainingCount;
	static int* g_paiTempRemovesUnitCombatTypeCount;
	static int* g_paiTempExtraFlankingStrengthbyUnitCombatType;
	static int* g_paiTempExtraWithdrawVSUnitCombatType;
	static int* g_paiTempExtraWithdrawOnTerrainType;
	static int* g_paiTempExtraWithdrawOnFeatureType;
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
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isDead);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, hasCargo);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isCargo);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isFull);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, canMove);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, canMoveAllTerrain);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, hasMoved);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, canIgnoreZoneofControl);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isAutoUpgrading);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isReadyForUpgrade);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isPromotionReady);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isCombat);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isAnimal);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, canFight);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, canDefend);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, alwaysInvisible);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, IsSelected);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isCommander);
		DECLARE_MAP_FUNCTOR(CvUnit, bool, isGoldenAge);

		DECLARE_MAP_FUNCTOR_1(CvUnit, bool, meetsUnitSelectionCriteria, const CvUnitSelectionCriteria*);
		DECLARE_MAP_FUNCTOR_1(CvUnit, bool, canPillage, const CvPlot*);
		DECLARE_MAP_FUNCTOR_1(CvUnit, bool, hasAfflictionLine, PromotionLineTypes);

		DECLARE_MAP_FUNCTOR_1(CvUnit, int, upgradePrice, UnitTypes);
		DECLARE_MAP_FUNCTOR_2(CvUnit, bool, canUpgrade, UnitTypes, bool);

		DECLARE_MAP_FUNCTOR(CvUnit, int, getFortifyTurns);
		DECLARE_MAP_FUNCTOR(CvUnit, int, visibilityRange);
		DECLARE_MAP_FUNCTOR(CvUnit, int, collateralDamage);
		DECLARE_MAP_FUNCTOR(CvUnit, int, getBombardRate);
		DECLARE_MAP_FUNCTOR(CvUnit, int, getDamage);
		DECLARE_MAP_FUNCTOR(CvUnit, int, getID);
		DECLARE_MAP_FUNCTOR(CvUnit, TeamTypes, getTeam);
		DECLARE_MAP_FUNCTOR(CvUnit, PlayerTypes, getOwner);
		DECLARE_MAP_FUNCTOR(CvUnit, UnitTypes, getUnitType);
		DECLARE_MAP_FUNCTOR(CvUnit, UnitCombatTypes, getUnitCombatType);
		DECLARE_MAP_FUNCTOR(CvUnit, UnitAITypes, AI_getUnitAIType);

		DECLARE_MAP_FUNCTOR(CvUnit, int, getArea);
		DECLARE_MAP_FUNCTOR(CvUnit, const CvArea*, area);
		DECLARE_MAP_FUNCTOR(CvUnit, const CvPlot*, plot);
		DECLARE_MAP_FUNCTOR(CvUnit, const CvUnitInfo&, getUnitInfo);
		DECLARE_MAP_FUNCTOR(CvUnit, BuildTypes, getBuildType);
		DECLARE_MAP_FUNCTOR(CvUnit, ImprovementTypes, getBuildTypeImprovement);
		DECLARE_MAP_FUNCTOR(CvUnit, int, getCargo);
		DECLARE_MAP_FUNCTOR(CvUnit, int, SMgetCargo);
		DECLARE_MAP_FUNCTOR(CvUnit, int, SMCargoVolume);

		DECLARE_MAP_FUNCTOR_1(CvUnit, int, worsenedProbabilitytoAfflict, PromotionLineTypes);
		DECLARE_MAP_FUNCTOR_1(CvUnit, int, aidTotal, PropertyTypes);

		DECLARE_MAP_FUNCTOR_2(CvUnit, bool, isInvisible, TeamTypes, bool);

		DECLARE_MAP_FUNCTOR_3(CvUnit, bool, canBombardAtRanged, const CvPlot*, int, int);
		DECLARE_MAP_FUNCTOR_3(CvUnit, int, getTriggerValue, EventTriggerTypes /*eTrigger*/, const CvPlot* /*pPlot*/, bool /*bCheckPlot*/);

	};
};

typedef std::vector<CvUnit*> UnitVector;
typedef std::vector<const CvUnit*> ConstUnitVector;

// Safe unit iterators (they copy the whole range before iterating, but this is just copying pointers so not a big deal in most cases
// However it shouldn't be used in inner loops
typedef copy_iterator<CvUnit> safe_unit_iterator;

#endif
