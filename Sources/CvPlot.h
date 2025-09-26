#pragma once

#ifndef CvPlot_h__
#define CvPlot_h__

// CvPlot.h

#include "LinkedList.h"
#include "copy_iterator.h"
#include "CvGameObject.h"
#include "CvProperties.h"
#include "CvPlotPaging.h"
#include "idinfo_iterator_base.h"

#pragma warning( disable: 4251 )		// needs to have dll-interface to be used by clients of class

class CvArea;
class CvMap;
class CvPlotBuilder;
class CvRoute;
class CvRiver;
class CvCity;
class CvPlotGroup;
class CvFeature;
class CvSelectionGroup;
class CvUnit;
class CvSymbol;
class CvFlagEntity;
class CvPathGeneratorPlotInfo;
class CvPathPlotInfoStore;
enum UnitValueFlags;

typedef bool (*ConstPlotUnitFunc)( const CvUnit* pUnit, int iData1, int iData2, const CvUnit* eUnit);
typedef bool (*PlotUnitFunc)(CvUnit* pUnit, int iData1, int iData2, const CvUnit* eUnit);

// Flags to control defender scoring (getBestDefender etc)
struct EDefenderScore
{
	enum flags
	{
		None = 0,
		TestAtWar = 1 << 0,
		TestPotentialEnemy = 1 << 1,
		TestCanMove = 1 << 2,
		Assassinate = 1 << 3,
		ClearCache = 1 << 4
	};
};
DECLARE_FLAGS(EDefenderScore::flags);

// Flags for evaluating plot defender strength
struct StrengthFlags
{
	enum flags
	{
		None = 0,
		DefensiveBonuses = 1 << 0,
		TestAtWar = 1 << 1,
		TestPotentialEnemy = 1 << 2,
		CollatoralDamage = 1 << 3
	};
};
DECLARE_FLAGS(StrengthFlags::flags);

//	Koshling - add caching to canBuild calculations
#define CAN_BUILD_VALUE_CACHING
#ifdef CAN_BUILD_VALUE_CACHING
struct canBuildCacheEntry
{
	canBuildCacheEntry()
		: iPlotX(-1)
		, iPlotY(-1)
		, eBuild(NO_BUILD)
		, ePlayer(NO_PLAYER)
		, lResult(-1)
		, iLastUseCount(0)
	{}

	int iPlotX;
	int iPlotY;
	BuildTypes eBuild;
	PlayerTypes ePlayer;
	long lResult;
	int iLastUseCount;
};

#define CAN_BUILD_CACHE_SIZE 64

class canBuildCache
{
public:
	canBuildCache() : currentUseCounter(0) {}

	struct canBuildCacheEntry entries[CAN_BUILD_CACHE_SIZE];
	int currentUseCounter;
};
#endif

// Enum representing the graphical various components of a plot
struct ECvPlotGraphics
{
	enum type {
		// WHAT					MEM		SPEED
		// ==================================
		NONE = 0,
		// Yield icons			Low		Fast
		SYMBOLS = 1 << 0,
		// Improvements,
		// Resources,
		// Terrain features		High	Slow
		FEATURE = 1 << 1,
		// Rivers				Low		Fast
		RIVER = 1 << 2,
		// Roads, paths, rail	Low		Fast
		ROUTE = 1 << 3,
		// Units				High	Med
		// Depends a lot on line of sight
		UNIT = 1 << 4,
		// Cities				High	Med
		CITY = 1 << 5,
		ALL = SYMBOLS | FEATURE | RIVER | ROUTE | UNIT | CITY,
		NUM = 6
	};

	// Get enum flag by index, for use in for loops and the like
	static inline type fromIndex(int idx)
	{
		FAssertMsg(idx >= 0 && idx < type::NUM, "Enum index out of range");
		return static_cast<type>(1 << idx);
	}
};
DECLARE_FLAGS(ECvPlotGraphics::type);

class CvPlot
	: private bst::noncopyable
{
friend CvPathPlotInfoStore;
public:
	CvPlot();
	virtual ~CvPlot();

	CvGameObjectPlot* getGameObject() { return &m_GameObject; };
	const CvGameObjectPlot* getGameObject() const { return &m_GameObject; };

	// Comparison operators
	// Use address identity for now (more than one map means x/y compare wouldn't work)
	friend bool operator==(const CvPlot& lhs, const CvPlot& rhs) { return &lhs == &rhs; }
	friend bool operator!=(const CvPlot& lhs, const CvPlot& rhs) { return &lhs != &rhs; }

	void init(int iX, int iY);
	void uninit();
	void reset(int iX = 0, int iY = 0, bool bConstructorCall=false);
	void setupGraphical();
	void updateGraphicEra();

	void erase();

	void clearModifierTotals();
	void recalculateBaseYield();

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	void destroyGraphics();
	bool isNull() const;
	void setNull(bool bNull);
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	DllExport float getPointX() const;
	DllExport float getPointY() const;
	DllExport NiPoint3 getPoint() const;

	float getSymbolSize() const;
	DllExport float getSymbolOffsetX(int iID) const;
	DllExport float getSymbolOffsetY(int iID) const;

	TeamTypes getTeam() const;

	void doTurn();

	bool doBonusDiscovery();
	void doBonusDepletion();

	void updateCulture(bool bBumpUnits, bool bUpdatePlotGroups);
	void checkCityRevolt();
	void checkFortRevolt();

	void updateFog(const bool bApplyDecay = false);
	void updateVisibility();
#ifdef ENABLE_FOGWAR_DECAY
	void InitFogDecay(const bool pWithRandom = false);
	short getVisibilityDecayBonus(const bool pSeaPlot = false);
#endif

	void updateSymbolDisplay();
	void updateSymbolVisibility();
	void updateSymbols();

	void updateMinimapColor();

	bool unitHere(const CvUnit* pUnit) const;
	CvUnit* getPreferredCenterUnit() const;
	void updateCenterUnit();
	void enableCenterUnitRecalc(bool bEnable);

	void verifyUnitValidPlot();

	void nukeExplosion(int iRange, CvUnit* pNukeUnit = NULL);

	bool isConnectedTo( const CvCity* pCity) const;
	bool isConnectedToCapital(PlayerTypes ePlayer = NO_PLAYER) const;
	int getPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;
	bool isPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;
	bool isAdjacentPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const;
	void updatePlotGroupBonus(bool bAdd);
	bool isBonusExtracted(const TeamTypes eTeamPerspective = NO_TEAM) const;

	bool isAdjacentToArea(int iAreaID) const;
	bool isAdjacentToArea(const CvArea* pArea) const;
	bool shareAdjacentArea( const CvPlot* pPlot) const;
	bool isAdjacentToLand() const;
	bool isCoastalLand(int iMinWaterSize = -1) const;
	bool isCoastal(int iMinWaterSize = -1) const;

	bool isVisibleWorked() const;
	bool isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer = NO_PLAYER) const;

	DllExport bool isLake() const;

	bool isFreshWater() const;
	bool isWaterAndIsFresh() const;

	bool isPotentialIrrigation() const;
	bool canHavePotentialIrrigation() const;
	DllExport bool isIrrigationAvailable(bool bIgnoreSelf = false) const;

	bool isRiverMask() const;
	DllExport bool isRiverCrossingFlowClockwise(DirectionTypes eDirection) const;
	bool isRiverSide() const;
	bool isRiver() const;
	bool isRiverConnection(DirectionTypes eDirection) const;

	int isLandWater(const bool bLand) const;
	int getDistanceToLandOrCoast(const int iMaxReturn = MAX_INT) const;
	int setClimateAppropriateWaterTerrain(const int iDistance, ClimateZoneTypes eClimate = NO_CLIMATE_ZONE);
	bool correctWaterTerrain(int &iLastDistance);
	void correctWaterTerrains(int iLastDistance, const DirectionTypes dir, const bool bContinue = false);

	int getElevationLevel(const bool bExtra = false) const;
	int getTerrainElevation() const;
	int seeFromLevel(TeamTypes eTeam) const;
	int seeThroughLevel() const;
	void changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, CvUnit* pUnit, bool bUpdatePlotGroups);
	bool canSeePlot(const CvPlot* plot, TeamTypes eTeam) const;
	bool canSeeDisplacementPlot(TeamTypes eTeam, int dx, int dy, int dx0, int dy0, int& iTopElevation, int& iTopElevationDistance, bool bEndPoint = false) const;
	void updateSight(bool bIncrement, bool bUpdatePlotGroups);
	void updateSeeFromSight(bool bIncrement, bool bUpdatePlotGroups);

	bool canHaveBonus(BonusTypes eBonus, bool bIgnoreLatitude = false) const;

	bool canBuildImprovement(ImprovementTypes eImprovement, TeamTypes eTeam) const;
	bool canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam = NO_TEAM, bool bPotential = false, bool bUpgradeCheck = false) const;

	bool canBuild(BuildTypes eBuild, PlayerTypes ePlayer = NO_PLAYER, bool bTestVisible = false, bool bIncludePythonOverrides = true) const;
	static bool hasCachedCanBuildEntry(int iX, int iY, BuildTypes eBuild, PlayerTypes ePlayer, struct canBuildCacheEntry*& entry);
	long canBuildFromPython(BuildTypes eBuild, PlayerTypes ePlayer) const;
	long canBuildFromPythonInternal(BuildTypes eBuild, PlayerTypes ePlayer) const;
	int getBuildTime(BuildTypes eBuild) const;
	int getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer) const;
	int getBuildTurnsLeft(BuildTypes eBuild, int iNowExtra, int iThenExtra, bool bIncludeUnits = true) const;

	int getFeatureProduction(BuildTypes eBuild, TeamTypes eTeam, CvCity** ppCity) const;

	// Don't call this at all
	DllExport CvUnit* getBestDefenderExternal(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false) const;
	CvUnit* getBestDefender(EDefenderScore::flags flags, PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL) const;
	CvUnit* getFirstDefender(EDefenderScore::flags flags, PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker) const;

	CvUnit* getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false, bool bAssassinate = false, bool bClearCache = false) const;
	// Deprecated, use the function above
	CvUnit* getFirstDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false) const;

	int AI_sumStrength(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, DomainTypes eDomainType = NO_DOMAIN, StrengthFlags::flags flags = StrengthFlags::DefensiveBonuses, int iRange = 0) const;
	CvUnit* getSelectedUnit() const;
	int getUnitPower(PlayerTypes eOwner = NO_PLAYER) const;

	// Super Forts *bombard*
	bool isBombardable(const CvUnit* pUnit) const;
	bool isBombarded() const;
	void setBombarded(bool bNewValue);
	int getDefenseDamage() const;
	void changeDefenseDamage(int iChange);

	// Super Forts *culture*
	void pushCultureFromImprovement(PlayerTypes ePlayer, int iChange, int iRange, bool bUpdate);
	void doImprovementCulture(PlayerTypes ePlayer, const CvImprovementInfo& imp);

	// Super Forts *canal* *choke*
	int countRegionPlots(const CvPlot* pInvalidPlot = NULL) const;
	int countAdjacentPassableSections(bool bWater) const;
	int countImpassableCardinalDirections() const;
	int getCanalValue() const;
	void setCanalValue(int iNewValue);
	void calculateCanalValue();
	int getChokeValue() const;
	void setChokeValue(int iNewValue);
	void calculateChokeValue();
	// ! Super Forts

	int defenseModifier(TeamTypes eDefender, bool bIgnoreBuilding, bool bHelp = false) const;
	int movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot) const;
	static void flushMovementCostCache();

	//	Koshling - count of mountain leaders present per team maintained for efficiency of movement calculations
	// TB: This was not working properly so has been changed to a plotcount method.
	int getHasMountainLeader(TeamTypes eTeam) const;

	bool isAdjacentOwned() const;
	bool isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly = false) const;
	bool isAdjacentTeam(TeamTypes eTeam, bool bLandOnly = false) const;

	bool isHasPathToEnemyCity( TeamTypes eAttackerTeam, bool bIgnoreBarb = true ) const;
	bool isHasPathToPlayerCity( TeamTypes eMoveTeam, PlayerTypes eOtherPlayer = NO_PLAYER ) const;
	int calculatePathDistanceToPlot( TeamTypes eTeam, CvPlot* pTargetPlot ) const;

	// Plot danger cache
	inline int getActivePlayerSafeRangeCache(const bool bTestMoves) const
	{
		if (bTestMoves)
		{
			return m_iActivePlayerSafeRangeCacheTestMoves;
		}
		return m_iActivePlayerSafeRangeCache;
	}
	inline void setActivePlayerSafeRangeCache(const int iRange, const bool bTestMoves) const 
	{
		if (bTestMoves)
		{
			m_iActivePlayerSafeRangeCacheTestMoves = iRange;
		}
		else m_iActivePlayerSafeRangeCache = iRange;
	}
	inline bool getActivePlayerHasDangerCache(const bool bTestMoves) const
	{
		if (bTestMoves)
		{
			return m_bActivePlayerHasDangerCacheTestMoves;
		}
		return m_bActivePlayerHasDangerCache;
	}
	inline void setActivePlayerHasDangerCache(const bool bNewValue, const bool bTestMoves) const
	{
		if (bTestMoves)
		{
			m_bActivePlayerHasDangerCacheTestMoves = bNewValue;
		}
		else m_bActivePlayerHasDangerCache = bNewValue;
	}
	inline bool getBorderDangerCache(const TeamTypes eTeam) const { return m_borderDangerCache[eTeam]; }
	inline void setBorderDangerCache(const TeamTypes eTeam, const bool bNewValue) const { m_borderDangerCache[eTeam] = bNewValue; }
	void invalidateBorderDangerCache() const;
	void invalidateActivePlayerPlotCache();

	CvCity* getAdjacentCity(PlayerTypes ePlayer = NO_PLAYER) const;
	bool changeBuildProgress(BuildTypes eBuild, int iChange, PlayerTypes ePlayer = NO_PLAYER);
	bool isHasValidBonus() const;
	PlayerTypes getClaimingOwner() const;
	void setClaimingOwner(PlayerTypes eNewValue);

	bool isActsAsCity() const;
	bool isCanMoveSeaUnits() const;
	bool isSeaTunnel() const;
	int getRevoltProtection() const;
	int getAverageEnemyStrength(TeamTypes eTeam) const;
	int getAverageEnemyDamage(TeamTypes eTeam) const;
	LandmarkTypes getLandmarkType() const;
	void setLandmarkType(LandmarkTypes eLandmark);
	CvWString getLandmarkName() const;
	void setLandmarkName(CvWString szName);
	CvWString getLandmarkMessage() const;
	void setLandmarkMessage(CvWString szName);
	bool isCountedPlot() const;
	void setCountedPlot(bool bNewVal);
	const CvPlot* isInFortControl(bool bIgnoreObstructions, PlayerTypes eDefendingPlayer, PlayerTypes eAttackingPlayer, const CvPlot* pFort = NULL) const;
	PlayerTypes controlsAdjacentZOCSource(TeamTypes eAttackingTeam) const;
	bool isInCityZoneOfControl(PlayerTypes ePlayer) const;
	bool isInUnitZoneOfControl(PlayerTypes ePlayer) const;
	int getTotalTurnDamage(const CvSelectionGroup* pGroup) const;
	int getTerrainTurnDamage(const CvUnit* pUnit = NULL) const;
	int getFeatureTurnDamage() const;
	int getTotalTurnDamage(const CvUnit* pUnit = NULL) const;
	CvUnit* getWorstDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer = NO_PLAYER, const CvUnit* pAttacker = NULL, bool bTestAtWar = false, bool bTestPotentialEnemy = false, bool bTestCanMove = false, bool bAssassinate = false) const;
	bool isBorder(bool bIgnoreWater = false) const;

	int getNumVisibleAdjacentEnemyDefenders(const CvUnit* pUnit) const;

	void addSign(PlayerTypes ePlayer, CvWString szMessage);
	void removeSign(PlayerTypes ePlayer);
	void removeSignForAllPlayers();

	int getNumVisibleEnemyUnits(const CvUnit* pUnit) const;
	int getNumVisibleEnemyTargetUnits(const CvUnit* pUnit) const;
	int getNumVisibleEnemyUnits(PlayerTypes ePlayer) const;
	int getNumVisibleEnemyCombatUnits(PlayerTypes ePlayer) const;
	int getBorderPlotCount() const;
	int getEnemyBorderPlotCount(PlayerTypes ePlayer) const;
	int getVisibleEnemyStrength(PlayerTypes ePlayer, int iRange = 0) const;
	int getVisibleNonAllyStrength(PlayerTypes ePlayer) const;

	int getCultureRateThisTurn(const PlayerTypes ePlayer) const;
	int getCultureRateLastTurn(const PlayerTypes ePlayer) const;

	void setInCultureRangeOfCityByPlayer(const PlayerTypes ePlayer);
	bool isInCultureRangeOfCityByPlayer(const PlayerTypes ePlayer) const;

protected:
	CvGameObjectPlot m_GameObject;

	char /*PlayerTypes*/ m_eClaimingOwner;
	void doTerritoryClaiming();
	CvWString m_szLandmarkMessage;
	CvWString m_szLandmarkName;
	LandmarkTypes m_eLandmarkType;
	bool m_bCounted;
	static stdext::hash_map<int,int>* m_resultHashMap;

	std::vector<std::pair<PlayerTypes, int> > m_cultureRatesThisTurn;
	std::vector<std::pair<PlayerTypes, int> > m_cultureRatesLastTurn;
	std::vector<PlayerTypes> m_influencedByCityByPlayerLastTurn;
	std::vector<PlayerTypes> m_influencedByCityByPlayer;

public:
	PlayerTypes calculateCulturalOwner(bool bCountLastTurn = true) const;
	//PlayerTypes getPlayerWithTerritorySurroundingThisPlotCardinally() const;

	void plotAction(PlotUnitFunc func, int iData1 = -1, int iData2 = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM);
	int plotCount(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, const CvUnit* pUnit = NULL, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1, int iRange = 0) const;
	CvUnit* plotCheck(ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, const CvUnit* pUnit = NULL, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1) const;
	int plotStrength(UnitValueFlags eFlags, ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1, int iRange = 0) const;
	int plotStrengthTimes100(UnitValueFlags eFlags, ConstPlotUnitFunc funcA, int iData1A = -1, int iData2A = -1, PlayerTypes eOwner = NO_PLAYER, TeamTypes eTeam = NO_TEAM, ConstPlotUnitFunc funcB = NULL, int iData1B = -1, int iData2B = -1, int iRange = 0) const;

	bool isOwned() const;
	bool isNPC() const;
	bool isHominid() const;

	DllExport bool isActiveVisible(bool bDebug) const;

	bool isVisible(TeamTypes eTeam, bool bDebug) const;
	bool isVisibleToCivTeam() const;
	bool isVisibleToWatchingHuman() const;
	bool isAdjacentVisible(TeamTypes eTeam, bool bDebug) const;
	bool isAdjacentNonvisible(TeamTypes eTeam) const;

	DllExport bool isGoody(TeamTypes eTeam = NO_TEAM) const;

	bool isRevealedGoody(TeamTypes eTeam = NO_TEAM) const;
	void removeGoody();

	DllExport bool isCity(bool bCheckImprovement = false, TeamTypes eForTeam = NO_TEAM) const;

	bool isFriendlyCity(const CvUnit& kUnit, bool bCheckImprovement) const;
	bool isEnemyCity(const CvUnit& kUnit, bool bOnlyRealCities = false) const;

	bool isOccupation() const;
	bool isBeingWorked() const;

	bool isUnit() const;
	bool isInvestigate(TeamTypes eTeam) const;
	bool isVisibleEnemyDefender(const CvUnit* pUnit) const;
	CvUnit *getVisibleEnemyDefender(PlayerTypes ePlayer) const;
	int getNumDefenders(PlayerTypes ePlayer) const;
	int getNumAttackers(PlayerTypes ePlayer) const;
	int getNumVisibleEnemyDefenders(const CvUnit* pUnit) const;
	int getNumCriminals() const;
	int getNumPlayerUnitAI(UnitAITypes eUnitAI, PlayerTypes ePlayer) const;
	int getNumVisibleWantedCriminals(PlayerTypes ePlayer) const;
	int getNumVisiblePotentialEnemyDefenders(const CvUnit* pUnit) const;
	int getNumVisiblePotentialEnemyDefenderless(const CvUnit* pUnit) const;
	bool isVisiblePotentialEnemyDefender(const CvUnit* pUnit) const;
	bool isVisiblePotentialEnemyDefenderless(const CvUnit* pUnit) const;

	DllExport bool isFighting() const;
	DllExport bool isVisibleEnemyUnit(PlayerTypes ePlayer) const;
	DllExport int getNumVisibleUnits(PlayerTypes ePlayer) const;

	bool isBattle() const;

	bool isVisibleEnemyUnit(const CvUnit* pUnit) const;
	bool isVisibleOtherUnit(PlayerTypes ePlayer) const;

	bool canHaveFeature(FeatureTypes eFeature, bool bOverExistingFeature = false) const;

	DllExport bool isRoute() const;

	bool isValidRoute(const CvUnit* pUnit) const;
	bool isTradeNetworkImpassable(TeamTypes eTeam) const;
	bool isNetworkTerrain(TeamTypes eTeam) const;
	bool isBonusNetwork(TeamTypes eTeam) const;
	bool isTradeNetwork(TeamTypes eTeam) const;
	bool isTradeNetworkConnected(const CvPlot * pPlot, TeamTypes eTeam) const;
	bool isRiverNetwork(TeamTypes eTeam) const;

	bool isValidDomainForLocation(const CvUnit& unit) const;
	bool isValidDomainForAction(const CvUnit& unit) const;

	bool isImpassable(TeamTypes eTeam = NO_TEAM) const;

	DllExport int getViewportX() const;
	inline int getX() const { return m_iX; }
	DllExport int getViewportY() const;
	inline int getY() const { return m_iY; }
	bool isInViewport(int comfortBorderSize = 0) const;

	// Base iterator type for iterating over adjacent valid plots
	struct adjacent_iterator :
		public bst::iterator_facade<adjacent_iterator, CvPlot*, bst::forward_traversal_tag, CvPlot*>
	{
		adjacent_iterator();
		adjacent_iterator(int centerX, int centerY, int numPlots, const int* plotDirectionX, const int* plotDirectionY);

	private:
		friend class bst::iterator_core_access;
		void increment();
		bool equal(adjacent_iterator const& other) const;
		CvPlot* dereference() const { return m_curr; }

		const int m_centerX;
		const int m_centerY;
		const int m_numPlots;
		const int* m_plotDirectionX;
		const int* m_plotDirectionY;
		const CvMap* m_map;
		CvPlot* m_curr;
		int m_idx;
	};

	adjacent_iterator beginAdjacent(int numPlots, const int* plotDirectionX, const int* plotDirectionY) const;
	adjacent_iterator endAdjacent() const;

	typedef bst::iterator_range<adjacent_iterator> adjacent_range;

	adjacent_range adjacent() const;
	adjacent_range cardinalDirectionAdjacent() const;

	// Base iterator type for iterating over a rectangle of plots
	template < class Value_ >
	struct rect_iterator_base :
		public bst::iterator_facade<rect_iterator_base<Value_>, Value_*, bst::forward_traversal_tag, Value_*>
	{
		rect_iterator_base() : m_centerX(-1), m_centerY(-1), m_wid(-1), m_hgt(-1), m_curr(nullptr), m_x(0), m_y(0){}
		rect_iterator_base(int centerX, int centerY, int halfwid, int halfhgt) : m_centerX(centerX), m_centerY(centerY), m_wid(halfwid), m_hgt(halfhgt), m_curr(nullptr), m_x(-halfwid), m_y(-halfhgt)
		{
			increment();
		}

	private:
		friend class bst::iterator_core_access;
		void increment()
		{
			m_curr = nullptr;
			while (m_curr == nullptr && m_x <= m_wid)
			{
				m_curr = plotXY(m_centerX, m_centerY, m_x, m_y);
				++m_y;
				if(m_y > m_hgt)
				{
					m_y = -m_hgt;
					++m_x;
				}
			}
		}

		bool equal(rect_iterator_base const& other) const
		{
			return this->m_curr == other.m_curr;
			//(this->m_centerX == other.m_centerX
			//&& this->m_centerY == other.m_centerY
			//&& this->m_x == other.m_x
			//&& this->m_y == other.m_y)
			//|| (this->m_curr == NULL && other.m_curr == NULL);
		}

		Value_* dereference() const { return m_curr; }

		const int m_centerX;
		const int m_centerY;
		const int m_wid;
		const int m_hgt;
		Value_* m_curr;
		int m_x;
		int m_y;
	};
	typedef rect_iterator_base<CvPlot> rect_iterator;

	static rect_iterator beginRect(int centerX, int centerY, int halfWid, int halfHgt) { return rect_iterator(centerX, centerY, halfWid, halfHgt); }
	static rect_iterator endRect() { return rect_iterator(); }

	typedef bst::iterator_range<rect_iterator> rect_range;
	//static rect_range rect(int centerX, int centerY, int halfWid, int halfHgt) { return rect_range(beginRect(centerX, centerY, halfWid, halfHgt), endRect()); }
	rect_range rect(int halfWid, int halfHgt) const { return rect_range(beginRect(m_iX, m_iY, halfWid, halfHgt), endRect()); }

	// ==========================================================================================
	// PAGING SYSTEM
	//
	// Set what graphics we require to be visible (probably based on camera position)
	void setRequireGraphicsVisible(ECvPlotGraphics::type graphics, bool visible);
	bool isGraphicsVisible(ECvPlotGraphics::type graphics) const;
	bool isGraphicPagingEnabled() const;

	// Show all required but not visible graphics
	void showRequiredGraphics();
	void updateGraphics(ECvPlotGraphics::type toShow = ECvPlotGraphics::ALL);

	// Hide all non required but visible graphics
	void hideNonRequiredGraphics();
	void hideGraphics(ECvPlotGraphics::type toHide = ECvPlotGraphics::ALL);

	// Get mask of all graphics components that aren't required to be visible but currently are
	ECvPlotGraphics::type getNonRequiredGraphicsMask() const;
	void enableGraphicsPaging();
	void disableGraphicsPaging();
	// ==========================================================================================

	bool shouldHaveGraphics() const;

	bool at(int iX, int iY) const;
	// BUG - Lat/Long Coordinates
	int calculateMinutes(int iPlotIndex, int iPlotCount, bool bWrap, int iDegreeMin, int iDegreeMax) const;
	int getLongitudeMinutes() const;
	int getLatitudeMinutes() const;
	// ! BUG
	int getLatitude() const;
	int getLatitudeRaw() const;
	int getLongitude() const;
	int getLongitudeRaw() const;
	int getFOWIndex() const;

	CvArea* area() const;
	CvArea* waterArea(bool bNoImpassable = false) const;
	CvArea* secondWaterArea() const;
	int getArea() const;
	void setArea(int iNewValue);

	DllExport int getFeatureVariety() const;

	int getOwnershipDuration() const;
	bool isOwnershipScore() const;
	void setOwnershipDuration(int iNewValue);

	int getImprovementUpgradeProgress() const;
	int getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const;
	void setImprovementUpgradeProgress(int iNewValue);
	void changeImprovementUpgradeProgress(int iChange);

	int getCityRadiusCount() const;
	int isCityRadius() const;
	void changeCityRadiusCount(int iChange);

	bool isStartingPlot() const;
	void setStartingPlot(bool bNewValue);

	DllExport bool isNOfRiver() const;
	void setNOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir);

	DllExport bool isWOfRiver() const;
	void setWOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir);

	DllExport CardinalDirectionTypes getRiverNSDirection() const;
	DllExport CardinalDirectionTypes getRiverWEDirection() const;

	CvPlot* getInlandCorner() const;
	bool hasCoastAtSECorner() const;

	bool isIrrigated() const;
	void setIrrigated(bool bNewValue);
	void updateIrrigated();

	bool isPotentialCityWork() const;
	bool isPotentialCityWorkForArea(const CvArea* pArea) const;
	void updatePotentialCityWork();

	bool isShowCitySymbols() const;
	void updateShowCitySymbols();

	bool isFlagDirty() const;
	void setFlagDirty(bool bNewValue);

	DllExport inline PlayerTypes getOwner() const { return (PlayerTypes)m_eOwner; }
	void setOwner(PlayerTypes eNewValue, bool bCheckUnits, bool bUpdatePlotGroup);
	static void	setDeferredPlotGroupRecalculationMode(bool bDefer);

	PlotTypes getPlotType() const;
	bool isWater() const;
	DllExport bool isWaterExternal() const;
	bool isFlatlands() const;
	DllExport bool isHills() const;
	DllExport bool isPeak() const;
	bool isAsPeak() const;
	void setPlotType(PlotTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true);

	DllExport TerrainTypes getTerrainTypeExternal() const;
	TerrainTypes getTerrainType() const;
	void setTerrainType(TerrainTypes eNewValue, bool bRecalculate = true, bool bRebuildGraphics = true);

	DllExport FeatureTypes getFeatureTypeExternal() const;
	FeatureTypes getFeatureType() const;
	void setFeatureType(FeatureTypes eNewValue, int iVariety = -1, bool bImprovementSet = false);
	void setFeatureDummyVisibility(const char *dummyTag, bool show);
	void addFeatureDummyModel(const char *dummyTag, const char *modelTag);
	void setFeatureDummyTexture(const char *dummyTag, const char *textureTag);
	CvString pickFeatureDummyTag(int mouseX, int mouseY);
	void resetFeatureModel();

	DllExport BonusTypes getBonusType(TeamTypes eTeam = NO_TEAM) const;
	BonusTypes getNonObsoleteBonusType(TeamTypes eTeam = NO_TEAM) const;
	void setBonusType(BonusTypes eNewValue);

	void setImprovementCurrentValue();
	int getImprovementCurrentValue() const;

	DllExport ImprovementTypes getImprovementType() const;
	void setImprovementType(ImprovementTypes eNewValue);

	bool isImprovementDestructible() const;

	RouteTypes getRouteType() const;
	void setRouteType(RouteTypes eNewValue, bool bUpdatePlotGroup);
	void updateCityRoute(bool bUpdatePlotGroup);

	DllExport CvCity* getPlotCity() const;
	void setPlotCity(CvCity* pNewValue);

	CvCity* getWorkingCity() const;
	void updateWorkingCity();

	CvCity* getWorkingCityOverride() const;
	void setWorkingCityOverride( const CvCity* pNewValue);

	int getRiverID() const;
	void setRiverID(int iNewValue);

	int getMinOriginalStartDist() const;
	void setMinOriginalStartDist(int iNewValue);

	int getReconCount() const;
	void changeReconCount(int iChange);

	int getRiverCrossingCount() const;
	void changeRiverCrossingCount(int iChange);

	int getBaseYield(const YieldTypes eIndex) const;
	void changeBaseYield(const short* pYieldChange);

	void setExtraYield(YieldTypes eYield, short iExtraYield);
	short* getYield() const;
	void updateYield();
	int calculateYield(YieldTypes eIndex, bool bDisplay = false) const;
	DllExport int getYield(YieldTypes eIndex) const;
	int calculateNatureYield(YieldTypes eIndex, TeamTypes eTeam, bool bIgnoreFeature = false) const;
	int calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const;
	int calculateTotalBestNatureYield(TeamTypes eTeam) const;
	int calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal = false, bool bBestRoute = false) const;
	bool hasYield() const;
	int getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade) const;

	int getCulture(PlayerTypes eIndex) const;
	int countTotalCulture() const;
	int countFriendlyCulture(TeamTypes eTeam) const;
	PlayerTypes findHighestCulturePlayer(const bool bCountLegacyCulture = true, const bool bCountLastTurn = true) const;
	int calculateCulturePercent(PlayerTypes eIndex, int iExtraDigits = 0) const;
	int calculateTeamCulturePercent(TeamTypes eIndex) const;
	void setCulture(PlayerTypes eIndex, int iNewValue, bool bUpdate, bool bUpdatePlotGroups, const bool bDecay = false);
	void changeCulture(PlayerTypes eIndex, int iChange, bool bUpdate);
	int countNumAirUnits(TeamTypes eTeam) const;
	int countNumAirUnitCargoVolume(TeamTypes eTeam) const;
	int airUnitSpaceAvailable(TeamTypes eTeam) const;
	int countAirInterceptorsActive(TeamTypes eTeam) const;

	int getFoundValue(PlayerTypes eIndex);
	bool isBestAdjacentFound(PlayerTypes eIndex);
	void clearFoundValue(PlayerTypes eIndex);
	void setFoundValue(PlayerTypes eIndex, int iNewValue);

	int getPlayerCityRadiusCount(PlayerTypes eIndex) const;
	bool isPlayerCityRadius(PlayerTypes eIndex) const;
	void changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange);

	int getPlotGroupId(PlayerTypes ePlayer) const;
	CvPlotGroup* getPlotGroup(PlayerTypes ePlayer) const;
	CvPlotGroup* getOwnerPlotGroup() const;
	void setPlotGroup(PlayerTypes ePlayer, CvPlotGroup* pNewValue, bool bRecalculateBonuses = true);
	void updatePlotGroup();
	void updatePlotGroup(PlayerTypes ePlayer, bool bRecalculate = true, bool bRecalculateBonuses = true);

	void clearVisibilityCounts();
	int getVisibilityCount(TeamTypes eTeam) const;
	void changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bUpdatePlotGroups, int iIntensity = 0, int iUnitID = 0);

	int getDangerCount(int /*PlayerTypes*/ ePlayer) const;
	void setDangerCount(int /*PlayerTypes*/ ePlayer, int iNewCount);

	int getStolenVisibilityCount(TeamTypes eTeam) const;
	void changeStolenVisibilityCount(TeamTypes eTeam, int iChange);

	int getLastVisibleTurn(TeamTypes eTeam) const;
	void setLastVisibleTurn(TeamTypes eTeam, short turn);

	int getBlockadedCount(TeamTypes eTeam) const;
	void changeBlockadedCount(TeamTypes eTeam, int iChange);
	void resetBlockadedCounts();

	DllExport PlayerTypes getRevealedOwner(TeamTypes eTeam, bool bDebug) const;
	TeamTypes getRevealedTeam(TeamTypes eTeam, bool bDebug) const;
	void setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue);
	void updateRevealedOwner(TeamTypes eTeam);

	DllExport bool isRiverCrossing(DirectionTypes eIndex) const;
	void updateRiverCrossing(DirectionTypes eIndex);
	void updateRiverCrossing();

	DllExport bool isRevealed(TeamTypes eTeam, bool bDebug) const;
	void setRevealed(const TeamTypes eTeam, const bool bNewValue, const bool bTerrainOnly, const TeamTypes eFromTeam, const bool bUpdatePlotGroup, const bool bUpdateFog = true);
	bool isAdjacentRevealed(TeamTypes eTeam, bool bDebug = false) const;
	bool isAdjacentNonrevealed(TeamTypes eTeam) const;

	ImprovementTypes getRevealedImprovementType(TeamTypes eTeam, bool bDebug = false) const;
	void setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue);

	RouteTypes getRevealedRouteType(TeamTypes eTeam, bool bDebug) const;
	void setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue);

	int getBuildProgress(BuildTypes eBuild) const;
	bool changeBuildProgress(BuildTypes eBuild, int iChange, TeamTypes eTeam = NO_TEAM);
	bool hasAnyBuildProgress() const;

	void updateFeatureSymbolVisibility();
	void updateFeatureSymbol(bool bForce = false);

	DllExport bool isLayoutDirty() const; // The plot layout contains bonuses and improvements --- it's, like the city layout, passively computed by LSystems
	DllExport void setLayoutDirty(bool bDirty);
	DllExport bool isLayoutStateDifferent() const;
	DllExport void setLayoutStateToCurrent();
	bool updatePlotBuilder();

	DllExport void getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked) const; // determines how the improvement state is shown in the engine
	DllExport void getVisibleBonusState(BonusTypes& eType, bool& bImproved, bool& bWorked) const; // determines how the bonus state is shown in the engine

	bool shouldUsePlotBuilder() const;
	//CvPlotBuilder* getPlotBuilder() const { return m_pPlotBuilder; }

	DllExport CvRoute* getRouteSymbol() const;
	void updateRouteSymbol(bool bForce = false, bool bAdjacent = false);

	DllExport CvRiver* getRiverSymbol() const;
	void updateRiverSymbol(bool bForce = false, bool bAdjacent = false);
	void updateRiverSymbolArt(bool bAdjacent = true);

	CvFeature* getFeatureSymbol() const;

	DllExport CvFlagEntity* getFlagSymbol() const;
	CvFlagEntity* getFlagSymbolOffset() const;
	DllExport void updateFlagSymbol();

	DllExport CvUnit* getCenterUnit() const;
	DllExport CvUnit* getDebugCenterUnit() const;

	CvUnit* getCenterUnit(const bool bForced) const;

	int getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const;
	bool isSpotterInSight(TeamTypes eTeam, InvisibleTypes eInvisible) const;
	void changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange);
	void setSpotIntensity(TeamTypes eTeam, InvisibleTypes eInvisible, int iUnitID, int iIntensity);

	int getNumPlotTeamVisibilityIntensity() const;
	PlotTeamVisibilityIntensity& getPlotTeamVisibilityIntensity(int iIndex);
	int getHighestPlotTeamVisibilityIntensity(InvisibleTypes eInvisibility, TeamTypes eTeam) const;

	static void	NextCachePathEpoch();
	bool HaveCachedPathValidityResult(void* entity, bool bIsAlternateResult, bool& cachedResult) const;
	void CachePathValidityResult(void* entity, bool bIsAlternateResult, bool cachedResult);

	int getNumUnits() const;
	CvUnit* getUnitByIndex(int iIndex) const;
	void addUnit(CvUnit* pUnit, bool bUpdate = true);
	void removeUnit(CvUnit* pUnit, bool bUpdate = true);
	DllExport CLLNode<IDInfo>* nextUnitNode(CLLNode<IDInfo>* pNode) const;
	CLLNode<IDInfo>* prevUnitNode(CLLNode<IDInfo>* pNode) const;
	DllExport CLLNode<IDInfo>* headUnitNode() const;
	CLLNode<IDInfo>* tailUnitNode() const;

	// For iterating over units on a plot
	DECLARE_IDINFO_ITERATOR(CvUnit, unit_iterator)

	unit_iterator beginUnits() const { return unit_iterator(&m_units); }
	unit_iterator endUnits() const { return unit_iterator(); }
	typedef bst::iterator_range<unit_iterator> unit_range;
	unit_range units() const { return unit_range(beginUnits(), endUnits()); }

	safe_unit_iterator beginUnitsSafe() const { return safe_unit_iterator(beginUnits(), endUnits()); }
	safe_unit_iterator endUnitsSafe() const { return safe_unit_iterator(); }
	typedef bst::iterator_range<safe_unit_iterator> safe_unit_range;
	safe_unit_range units_safe() const { return safe_unit_range(beginUnitsSafe(), endUnitsSafe()); }

	// As the plot doesn't own the units they aren't const even if the plot is, so no point in a const unit iterator
	//DECLARE_IDINFO_ITERATOR(const CvUnit, const_unit_iterator)

	//const_unit_iterator beginUnits() const { return const_unit_iterator(&m_units); }
	//const_unit_iterator endUnits() const { return const_unit_iterator(); }
	//typedef bst::iterator_range<const_unit_iterator> const_unit_range;
	//const_unit_range units() const { return const_unit_range(beginUnits(), endUnits()); }

	int getNumSymbols() const;
	CvSymbol* getSymbol(int iID) const;
	CvSymbol* addSymbol();

	void deleteSymbol(int iID);
	void deleteAllSymbols();

	// Script data needs to be a narrow string for pickling in Python
	CvString getScriptData() const;
	void setScriptData(const char* szNewValue);

	bool canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer) const;
	bool canApplyEvent(EventTypes eEvent) const;
	void applyEvent(EventTypes eEvent);

	bool canTrain(UnitTypes eUnit, bool bTestVisible) const;

	bool isEspionageCounterSpy(TeamTypes eTeam) const;

	DllExport int getAreaIdForGreatWall() const;
	DllExport int getSoundScriptId() const;
	DllExport int get3DAudioScriptFootstepIndex(int iFootstepTag) const;
	DllExport float getAqueductSourceWeight() const;  // used to place aqueducts on the map
	DllExport bool shouldDisplayBridge(CvPlot* pToPlot, PlayerTypes ePlayer) const;
	DllExport bool checkLateEra() const;

	void read(FDataStreamBase* pStream);
	void write(FDataStreamBase* pStream);

protected:
/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	bool m_bNull;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/
	short m_iX;
	short m_iY;
	int m_iArea;
	mutable CvArea *m_pPlotArea;
	short m_iFeatureVariety;
	short m_iOwnershipDuration;
	short m_iUpgradeProgress;
	short m_iCityRadiusCount;
	int m_iRiverID;
	short m_iMinOriginalStartDist;
	short m_iReconCount;
	short m_iRiverCrossingCount;
	mutable int m_iImprovementUpgradeHash;
	mutable int m_iCurrentRoundofUpgradeCache;
	int m_iImprovementCurrentValue;

	// Super Forts *canal* *choke*
	int m_iCanalValue;
	int m_iChokeValue;
	// Super Forts *bombard*
	int m_iDefenseDamage;
	bool m_bBombarded;
	// ! Super Forts

	bool m_bStartingPlot:1;
	bool m_bHills:1;
	bool m_bNOfRiver:1;
	bool m_bWOfRiver:1;
	bool m_bIrrigated:1;
	bool m_bPotentialCityWork:1;
	bool m_bShowCitySymbols:1;
	bool m_bFlagDirty:1;
	bool m_bPlotLayoutDirty:1;
	bool m_bLayoutStateWorked:1;

	char /*PlayerTypes*/ m_eOwner;
	short /*PlotTypes*/ m_ePlotType;
	short /*TerrainTypes*/ m_eTerrainType;
	short /*FeatureTypes*/ m_eFeatureType;
	short /*BonusTypes*/ m_eBonusType;
	short /*ImprovementTypes*/ m_eImprovementType;
	short /*RouteTypes*/ m_eRouteType;
	char /*CardinalDirectionTypes*/ m_eRiverNSDirection;
	char /*CardinalDirectionTypes*/ m_eRiverWEDirection;

	IDInfo m_plotCity;
	IDInfo m_workingCity;
	IDInfo m_workingCityOverride;

	// Plot danger cache
	mutable int m_iActivePlayerSafeRangeCache;
	mutable int m_iActivePlayerSafeRangeCacheTestMoves;
	mutable bool m_bActivePlayerHasDangerCache;
	mutable bool m_bActivePlayerHasDangerCacheTestMoves;
	mutable bool* m_borderDangerCache;

	static	int m_iGlobalCachePathEpoch;
	mutable int		m_iCachePathEpoch;
	mutable void*	m_cachedPathValidityEntity;
	mutable void*	m_cachedPathAlternateValidityEntity;
	mutable bool	m_cachedPathAlternateValidity;
	mutable bool	m_cachedPathValidity;

	// Super Forts begin *culture*
	//	Koshling - not needed in the C2C implemenattion due to AND's existing
	//	ellision of fort and city culture
	//short* m_aiCultureRangeForts;
	// Super Forts end

	short* m_baseYields;
	short* m_aiYield;
	bst::array<short, NUM_YIELD_TYPES> m_aExtraYield;
	std::vector<std::pair<PlayerTypes,int> > m_aiCulture;
	std::vector<PlotTeamVisibilityIntensity> m_aPlotTeamVisibilityIntensity;
	unsigned int* m_aiFoundValue;
	char* m_aiPlayerCityRadiusCount;
	int* m_aiPlotGroup; // IDs - keep as int
	short* m_aiVisibilityCount;
	short* m_aiStolenVisibilityCount;
	short* m_aiBlockadedCount;
	char* m_aiRevealedOwner;

	bool* m_abRiverCrossing; // bit vector
	bool* m_abRevealed;

	short* /*ImprovementTypes*/ m_aeRevealedImprovementType;
	short* /*RouteTypes*/ m_aeRevealedRouteType;

	char* m_szScriptData;

	int* m_paiBuildProgress;

	short* m_aiLastSeenTurn;

#ifdef ENABLE_FOGWAR_DECAY
	short m_iVisibilityDecay;
#endif

	//	This array will be sparsely populated on most plots so not worth
	//	an indirection, but look to find a way to use the sparseness to reduce memory usage
	//	at some point
	short* m_aiDangerCount;

	CvFeature* m_pFeatureSymbol;
	CvRoute* m_pRouteSymbol;
	CvRiver* m_pRiverSymbol;
	CvFlagEntity* m_pFlagSymbol;
	CvFlagEntity* m_pFlagSymbolOffset;
	CvUnit* m_pCenterUnit;

	CvPlotBuilder* m_pPlotBuilder; // builds bonuses and improvements

	short** m_apaiInvisibleVisibilityCount;

	/* Koshling - need to cache presence of mountain leaders in mountain plots so that CanMoveThrough calculations don't get bogged down searching unit stacks.
	This is a count of mountain leader units in the plot for each team.
	The array is only created if the plot is a mountain plot and at least one team has a mountain leader present.
	The array is cleared when the last leader leaves, so the memory overhead is low */
	short*	m_aiMountainLeaderCount;
	// ! Koshling

	CLinkList<IDInfo> m_units;

	std::vector<CvSymbol*> m_symbols;

	void doFeature();
	void doCulture();

	void processArea(CvArea* pArea, int iChange);
	void doImprovementUpgrade(const ImprovementTypes eType);

	ColorTypes plotMinimapColor() const;

	// added so under cheat mode we can access protected stuff
	friend class CvGameTextMgr;

// From Lead From Behind by UncutDragon
public:
	bool hasDefender(
		bool bCheckCanAttack,
		PlayerTypes eOwner,
		PlayerTypes eAttackingPlayer = NO_PLAYER,
		const CvUnit* pAttacker = NULL,
		bool bTestAtWar = false,
		bool bTestPotentialEnemy = false,
		bool bTestCanMove = false,
		bool bTestCanFight = false
	) const;
	bool hasStealthDefender(const CvUnit* victim, const bool bReveal = false);

	void doPreAttackTraps(CvUnit* pAttacker);

	void countCommander(bool bNewVal, const CvUnit* pUnit);
	void countCommodore(bool bNewVal, const CvUnit* pUnit);
	bool inCommandField(const PlayerTypes ePlayer) const;
	bool inCommandCommodoreField(const PlayerTypes ePlayer) const;

#ifdef CAN_BUILD_VALUE_CACHING
public:
	static void ClearCanBuildCache();

private:
	static canBuildCache g_canBuildCache;
	static int canBuildCacheHits;
	static int canBuildCacheReads;
#endif

	//	Koshling - add Zobrist hashing of plotGroups to reduce recalculation.
	//	Each plot has a contribution value to any hash it is included in
private:
	int	m_zobristContribution;
	int m_movementCharacteristicsHash;
	bool m_bPlotGroupsDirty;

	std::map<uint8_t, uint16_t> m_commanderCount; // PlayerType : CommanderCount
	std::map<uint8_t, uint16_t> m_commodoreCount; // PlayerType : CommodoreCount
	void changeCommanderCount(const PlayerTypes ePlayer, const bool bAdd);
	void changeCommodoreCount(const PlayerTypes ePlayer, const bool bAdd);

	static bool bDeferPlotGroupRecalculation;

	bool updateSymbolsInternal();

protected:
	//	Directly accessed cached pathing info per plot for performance reasons
	volatile mutable int m_pathGenerationSeq;
	mutable CvPathGeneratorPlotInfo* m_currentPathInfo;

public:
	int m_groupGenerationNumber;
	//	Toggle plot in/out of contribution
	void ToggleInPlotGroupsZobristContributors();

	inline int getZobristContribution() const { return m_zobristContribution; }

	inline int getMovementCharacteristicsHash() const { return m_movementCharacteristicsHash; }

	//TB Combat Mod AI
	bool isImprovementUpgradable() const;
	void setImprovementUpgradeCache(const int iNewValue);

	int getInjuredUnitCombatsUnsupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain = NO_DOMAIN) const;
	int getUnitCombatsUnsupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain = NO_DOMAIN) const;
	int getOverloadedUnitCombatsSupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain = NO_DOMAIN) const;

	void unitGameStateCorrections();

	bool isMapCategoryType(MapCategoryTypes eMapCategory) const;
	const std::vector<MapCategoryTypes>& getMapCategories() const;

	int countSeeInvisibleActive(PlayerTypes ePlayer, InvisibleTypes eVisible) const;

#ifdef OUTBREAKS_AND_AFFLICTIONS
	int getNumAfflictedUnits(PlayerTypes eOwner, PromotionLineTypes eAfflictionLine) const;
	int getCommunicability(PromotionLineTypes ePromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume) const;
#endif // OUTBREAKS_AND_AFFLICTIONS

protected:
	// AIAndy: Properties
	CvProperties m_Properties;
public:
	CvProperties* getProperties();
	const CvProperties* getPropertiesConst() const;

private:
	void pageGraphicsOut();

protected:
	bool m_bInhibitCenterUnitCalculation;
	bool m_bImprovementUpgradable;

	ECvPlotGraphics::type m_requiredVisibleGraphics;
	ECvPlotGraphics::type m_visibleGraphics;
	CvPlotPaging::paging_handle m_pagingHandle;

public:
	//
	// Algorithm/range helpers
	//
	struct fn {
		DECLARE_MAP_FUNCTOR_1(CvPlot, void, updateRevealedOwner, TeamTypes);
		DECLARE_MAP_FUNCTOR_1(CvPlot, void, setWorkingCityOverride, const CvCity*);

		DECLARE_MAP_FUNCTOR_2(CvPlot, void, updateCulture, bool, bool);
		DECLARE_MAP_FUNCTOR_2(CvPlot, void, updateRouteSymbol, bool, bool);
		DECLARE_MAP_FUNCTOR_2(CvPlot, void, updateRiverSymbol, bool, bool);

		DECLARE_MAP_FUNCTOR_CONST(CvPlot, ImprovementTypes, getImprovementType);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, TerrainTypes, getTerrainType);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, FeatureTypes, getFeatureType);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, TeamTypes, getTeam);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, PlayerTypes, getOwner);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, bool, isOwned);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, bool, isImpassable);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, bool, isIrrigated);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, bool, isFreshWater);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, bool, isLake);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, bool, isWater);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, bool, isInViewport);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, int, getArea);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, const CvArea*, area);
		DECLARE_MAP_FUNCTOR_CONST(CvPlot, const CvCity*, getWorkingCityOverride);

		DECLARE_MAP_FUNCTOR_CONST_1(CvPlot, bool, isConnectedToCapital, PlayerTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvPlot, BonusTypes, getBonusType, TeamTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvPlot, int, getBlockadedCount, TeamTypes);
		DECLARE_MAP_FUNCTOR_CONST_1(CvPlot, bool, isBombardable, const CvUnit*);

		DECLARE_MAP_FUNCTOR_CONST_2(CvPlot, bool, isCity, bool, TeamTypes);
		DECLARE_MAP_FUNCTOR_CONST_2(CvPlot, bool, isRevealed, TeamTypes, bool);
		DECLARE_MAP_FUNCTOR_CONST_2(CvPlot, bool, isVisible, TeamTypes, bool);

		DECLARE_MAP_FUNCTOR_CONST_2(CvPlot, bool, isPlotGroupConnectedBonus, PlayerTypes, BonusTypes);
	};
};

#endif // CvPlot_h__
