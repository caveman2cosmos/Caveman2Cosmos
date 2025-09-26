// plot.cpp


#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvArtFileMgr.h"
#include "CvBonusInfo.h"
#include "CvBuildingInfo.h"
#include "CvCity.h"
#include "CvEventReporter.h"
#include "CvGameAI.h"
#include "CvGameCoreUtils.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvPlotPaging.h"
#include "CvPython.h"
#include "CvRandom.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvViewport.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLFlagEntityIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLSymbolIFaceBase.h"
#include "CvDLLPlotBuilderIFaceBase.h"
#include "CvDLLUtilityIFaceBase.h"
#include "FAStarNode.h"

#define STANDARD_MINIMAP_ALPHA		(0.6f)

//	Pseudo-value to indicate uninitialised in found values
#define	INVALID_FOUND_VALUE	(0xFFFF)

typedef struct
{
	int iValue;
	int iHealth;
} unitDefenderInfo;

static int	g_plotTypeZobristHashes[NUM_PLOT_TYPES];
static bool g_plotTypeZobristHashesSet = false;
static int	g_riverDirectionZobristHashes[NUM_CARDINALDIRECTION_TYPES];
static bool g_riverDirectionZobristHashesSet = false;
int CvPlot::m_iGlobalCachePathEpoch = 0;
stdext::hash_map<int,int>* CvPlot::m_resultHashMap = NULL;
static const CvPlot* g_bestDefenderCachePlot = NULL;
typedef stdext::hash_map<int, unitDefenderInfo> DefenderScoreCache;
static DefenderScoreCache* g_bestDefenderCache = NULL;

// Public Functions...

#pragma warning( disable : 4355 )
CvPlot::CvPlot()
	: m_GameObject(this)
	, m_Properties(this)
	, m_visibleGraphics(ECvPlotGraphics::NONE)
	, m_requiredVisibleGraphics(ECvPlotGraphics::NONE)
	, m_pagingHandle(CvPlotPaging::INVALID_HANDLE)
{
	PROFILE_EXTRA_FUNC();
	if ( m_resultHashMap == NULL )
	{
		m_resultHashMap = new stdext::hash_map<int,int>();
	}

	if ( g_bestDefenderCache == NULL )
	{
		g_bestDefenderCache = new DefenderScoreCache();
	}

	m_baseYields = new short[NUM_YIELD_TYPES]();
	m_aiYield = new short[NUM_YIELD_TYPES]();

	// Plot danger cache
	m_borderDangerCache = new bool[MAX_TEAMS];

	m_aiFoundValue = NULL;
	m_aiPlayerCityRadiusCount = NULL;
	m_aiPlotGroup = NULL;
	m_aiVisibilityCount = new short[MAX_TEAMS];
	m_aiLastSeenTurn = NULL;
	m_aiDangerCount = NULL;
	m_aiStolenVisibilityCount = NULL;
	m_aiBlockadedCount = NULL;
	m_aiRevealedOwner = NULL;
	m_abRiverCrossing = NULL;
	m_abRevealed = NULL;
	m_aeRevealedImprovementType = NULL;
	m_aeRevealedRouteType = NULL;
	m_paiBuildProgress = NULL;
	m_apaiInvisibleVisibilityCount = NULL;

	m_aiMountainLeaderCount = NULL;	//	Koshling - mountain mod efficiency
	m_pFeatureSymbol = NULL;
	m_pPlotBuilder = NULL;
	m_pRouteSymbol = NULL;
	m_pRiverSymbol = NULL;
	m_pFlagSymbol = NULL;
	m_pFlagSymbolOffset = NULL;
	m_pCenterUnit = NULL;
	m_bInhibitCenterUnitCalculation = false;
	// Toffer - These doesn't recalculate, perhaps they should?
	m_bImprovementUpgradable = false;
	m_iImprovementUpgradeHash = 0;
	m_iCurrentRoundofUpgradeCache = -1;
	// ! Toffer
#ifdef ENABLE_FOGWAR_DECAY
	m_iVisibilityDecay = MAX_DECAY;
#endif
	m_iImprovementCurrentValue = 0;

	m_szScriptData = NULL;
	//Afforess: use async rand so as to not pollute mp games
	m_zobristContribution = GC.getASyncRand().getInt();

	if ( !g_plotTypeZobristHashesSet )
	{
		for(int i = 0; i < NUM_PLOT_TYPES; i++)
		{
			//Afforess: use async rand so as to not pollute mp games
			g_plotTypeZobristHashes[i] = GC.getASyncRand().getInt();
		}

		g_plotTypeZobristHashesSet = true;
	}

	if (!g_riverDirectionZobristHashesSet)
	{
		for (int i = 0; i < NUM_CARDINALDIRECTION_TYPES; i++)
		{
			//Afforess: use async rand so as to not pollute mp games
			g_riverDirectionZobristHashes[i] = GC.getASyncRand().getInt();
		}

		g_riverDirectionZobristHashesSet = true;
	}

	reset(0, 0, true);
}

CvPlot::~CvPlot()
{
	pageGraphicsOut();

	uninit();

	SAFE_DELETE_ARRAY(m_baseYields);
	SAFE_DELETE_ARRAY(m_aiYield);
	SAFE_DELETE_ARRAY(m_borderDangerCache);
}

void CvPlot::init(int iX, int iY)
{
	//--------------------------------
	// Init saved data
	reset(iX, iY);
	m_aPlotTeamVisibilityIntensity.clear();
	//--------------------------------
	// Init non-saved data
#ifdef ENABLE_FOGWAR_DECAY
	m_iVisibilityDecay = MAX_DECAY;
#endif
	//--------------------------------
	// Init other game data
}


void CvPlot::uninit()
{
	SAFE_DELETE_ARRAY(m_szScriptData);

	hideGraphics(ECvPlotGraphics::ALL);
	m_bPlotLayoutDirty = false;
	m_bLayoutStateWorked = false;
	m_bFlagDirty = false;

	//	Don't use 'clear' - we want to release the memory so we need to
	//	ensure the capcity goes to 0
	std::vector<std::pair<PlayerTypes,int> >().swap(m_aiCulture);

	SAFE_DELETE_ARRAY(m_aiFoundValue);

	SAFE_DELETE_ARRAY(m_aiPlayerCityRadiusCount);
	SAFE_DELETE_ARRAY(m_aiPlotGroup);

	SAFE_DELETE_ARRAY(m_aiVisibilityCount);
	SAFE_DELETE_ARRAY(m_aiLastSeenTurn);
	SAFE_DELETE_ARRAY(m_aiDangerCount);
	SAFE_DELETE_ARRAY(m_aiStolenVisibilityCount);
	SAFE_DELETE_ARRAY(m_aiBlockadedCount);
	SAFE_DELETE_ARRAY(m_aiRevealedOwner);

	SAFE_DELETE_ARRAY(m_abRiverCrossing);
	SAFE_DELETE_ARRAY(m_abRevealed);

	SAFE_DELETE_ARRAY(m_aeRevealedImprovementType);
	SAFE_DELETE_ARRAY(m_aeRevealedRouteType);

	SAFE_DELETE_ARRAY(m_aiMountainLeaderCount);
	SAFE_DELETE_ARRAY(m_paiBuildProgress);
	SAFE_DELETE_ARRAY2(m_apaiInvisibleVisibilityCount, MAX_TEAMS);

	m_aPlotTeamVisibilityIntensity.clear();
	m_units.clear();
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlot::reset(int iX, int iY, bool bConstructorCall)
{
	PROFILE_EXTRA_FUNC();
	//--------------------------------
	// Uninit class
	uninit();

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	m_bNull = false;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/
	m_iX = iX;
	m_iY = iY;
	m_iArea = FFreeList::INVALID_INDEX;
	m_pPlotArea = NULL;
	m_iFeatureVariety = 0;
	m_iOwnershipDuration = 0;
	m_iUpgradeProgress = 0;
	m_iCityRadiusCount = 0;
	m_iRiverID = -1;
	m_iMinOriginalStartDist = -1;
	m_iReconCount = 0;
	m_iRiverCrossingCount = 0;

	// Super Forts
	m_iCanalValue = 0;
	m_iChokeValue = 0;
	m_iDefenseDamage = 0;
	m_bBombarded = false;
	// ! Super Forts

	m_bStartingPlot = false;
	m_bHills = false;

	m_eClaimingOwner = NO_PLAYER;
	m_bCounted = false;
	m_eLandmarkType = NO_LANDMARK;
	m_szLandmarkMessage = "";
	m_szLandmarkName = "";

	m_bNOfRiver = false;
	m_bWOfRiver = false;
	m_bIrrigated = false;
	m_bPotentialCityWork = false;
	m_bShowCitySymbols = false;
	m_bFlagDirty = false;
	m_bPlotLayoutDirty = false;
	m_bLayoutStateWorked = false;

	m_movementCharacteristicsHash = 0;

	m_eOwner = NO_PLAYER;
	m_ePlotType = PLOT_OCEAN;
	m_eTerrainType = NO_TERRAIN;
	m_eFeatureType = NO_FEATURE;
	m_eBonusType = NO_BONUS;
	m_eImprovementType = NO_IMPROVEMENT;
	m_eRouteType = NO_ROUTE;
	m_eRiverNSDirection = NO_CARDINALDIRECTION;
	m_eRiverWEDirection = NO_CARDINALDIRECTION;

	m_plotCity.reset();
	m_workingCity.reset();
	m_workingCityOverride.reset();

	m_iActivePlayerSafeRangeCache = -1;
	m_iActivePlayerSafeRangeCacheTestMoves = -1;
	m_bActivePlayerHasDangerCache = false;
	m_bActivePlayerHasDangerCacheTestMoves = false;

	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_borderDangerCache[iI] = false;
	}

	m_pathGenerationSeq = -1;

	m_Properties.clear();
	m_commanderCount.clear();
	m_commodoreCount.clear();
	m_cultureRatesThisTurn.clear();
	m_cultureRatesLastTurn.clear();
	m_influencedByCityByPlayer.clear();
	m_influencedByCityByPlayerLastTurn.clear();

	m_aExtraYield.fill(0);

	m_bPlotGroupsDirty = false;
	m_aiVisibilityCount = new short[MAX_TEAMS];
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiVisibilityCount[iI] = 0;
	}
}

void CvPlot::clearModifierTotals()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		m_baseYields[iI] = 0;
	}
	m_commanderCount.clear();
	m_commodoreCount.clear();

	getProperties()->clearForRecalculate();

	// We will recalculate visibility from first principles
	clearVisibilityCounts();

	// Fix any spurious routes that are on water tiles
	if (isWater() && getRouteType() != NO_ROUTE && !GC.getRouteInfo(getRouteType()).isSeaTunnel())
	{
		setRouteType(NO_ROUTE, false);
	}
	unitGameStateCorrections();

	// Recalc blockades from scratch
	resetBlockadedCounts();
}


void CvPlot::pageGraphicsOut()
{
	if ( m_pagingHandle != CvPlotPaging::INVALID_HANDLE)
	{
		CvPlotPaging::RemovePlot(m_pagingHandle);
		m_pagingHandle = CvPlotPaging::INVALID_HANDLE;
	}

}

void CvPlot::setRequireGraphicsVisible(ECvPlotGraphics::type graphics, bool visible)
{
	if (visible)
	{
		m_requiredVisibleGraphics = m_requiredVisibleGraphics | graphics;
	}
	else
	{
		m_requiredVisibleGraphics = m_requiredVisibleGraphics & ~graphics;
	}
	showRequiredGraphics();
}

bool CvPlot::isGraphicsVisible(ECvPlotGraphics::type graphics) const
{
	return (m_visibleGraphics & graphics) != 0 && isInViewport();
}

bool CvPlot::isGraphicPagingEnabled() const
{
	return m_pagingHandle != CvPlotPaging::INVALID_HANDLE;
}

void CvPlot::showRequiredGraphics()
{
	if (isInViewport())
	{
		ECvPlotGraphics::type toShow = (m_requiredVisibleGraphics ^ m_visibleGraphics) & m_requiredVisibleGraphics;
		m_visibleGraphics = m_visibleGraphics | toShow;
		updateGraphics(toShow);
	}
}

void CvPlot::updateGraphics(ECvPlotGraphics::type toShow /*= ECvPlotGraphics::ALL*/)
{
	if (!isGraphicPagingEnabled() && GC.isGraphicalPaging())
	{
		enableGraphicsPaging();
	}

	if (toShow != ECvPlotGraphics::NONE)
	{
		setLayoutDirty(true);
	}
	if (toShow & ECvPlotGraphics::SYMBOLS)
	{
		updateSymbols();
	}
	if (toShow & ECvPlotGraphics::FEATURE)
	{
		updateFeatureSymbol();
	}
	if (toShow & ECvPlotGraphics::RIVER)
	{
		updateRiverSymbol();
	}
	if (toShow & ECvPlotGraphics::ROUTE)
	{
		updateRouteSymbol();
	}
	if (toShow & ECvPlotGraphics::UNIT)
	{
		updateCenterUnit();
		updateFlagSymbol();
	}
	if (toShow & ECvPlotGraphics::CITY)
	{
		if (getPlotCity() != NULL)
		{
			getPlotCity()->setLayoutDirty(true);
		}
	}
}

void CvPlot::hideNonRequiredGraphics()
{
	ECvPlotGraphics::type toHide = getNonRequiredGraphicsMask();
	hideGraphics(toHide);
}

void CvPlot::hideGraphics(ECvPlotGraphics::type toHide /*= ECvPlotGraphics::ALL*/)
{
	m_visibleGraphics = m_visibleGraphics & ~toHide;

	if (toHide & ECvPlotGraphics::SYMBOLS)
	{
		deleteAllSymbols();
	}
	if (toHide & ECvPlotGraphics::FEATURE)
	{
		gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
		m_pFeatureSymbol = NULL;
		if (m_pPlotBuilder)
		{
			gDLL->getPlotBuilderIFace()->destroy(m_pPlotBuilder);
			m_pPlotBuilder = NULL;
		}
	}
	if (toHide & ECvPlotGraphics::RIVER)
	{
		gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
		m_pRiverSymbol = NULL;
	}
	if (toHide & ECvPlotGraphics::ROUTE)
	{
		gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
		m_pRouteSymbol = NULL;
	}
	if (toHide & ECvPlotGraphics::UNIT)
	{
		updateCenterUnit();
		m_pCenterUnit = NULL;
		gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
		gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
		m_pFlagSymbol = NULL;
		m_pFlagSymbolOffset = NULL;
	}
	if (toHide & ECvPlotGraphics::CITY)
	{
		if (getPlotCity() != NULL)
		{
			getPlotCity()->setLayoutDirty(true);
		}
	}
	// All graphics are paged out now so we can remove ourselves from the active plot list
	if (m_visibleGraphics == ECvPlotGraphics::NONE)
	{
		pageGraphicsOut();
	}
	setLayoutDirty(true);
}

ECvPlotGraphics::type CvPlot::getNonRequiredGraphicsMask() const
{
	return (m_requiredVisibleGraphics ^ m_visibleGraphics) & m_visibleGraphics;
}

void CvPlot::enableGraphicsPaging()
{
	FAssertMsg(!isGraphicPagingEnabled(), "Graphics paging is already enabled");

	m_pagingHandle = CvPlotPaging::AddPlot(this);
}

void CvPlot::disableGraphicsPaging()
{
	FAssertMsg(isGraphicPagingEnabled(), "Graphics paging is already disabled");
	// Show all graphics, as we aren't paging any more
	setRequireGraphicsVisible(ECvPlotGraphics::ALL, true);
	// Disable paging
	pageGraphicsOut();
}

bool CvPlot::shouldHaveGraphics() const
{
	return GC.IsGraphicsInitialized() && isInViewport(); // && isRevealed(GC.getGame().getActiveTeam(), false);
}

//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvPlot::setupGraphical()
{
	PROFILE_FUNC();

	showRequiredGraphics();

	updateVisibility();
}

void CvPlot::updateGraphicEra()
{
	PROFILE_FUNC();

	if(m_pRouteSymbol != NULL)
		gDLL->getRouteIFace()->updateGraphicEra(m_pRouteSymbol);

	if(m_pFlagSymbol != NULL)
		gDLL->getFlagEntityIFace()->updateGraphicEra(m_pFlagSymbol);
}

void CvPlot::erase()
{
	algo::for_each(units_safe(), bind(CvUnit::kill, _1, false, NO_PLAYER, true));

	CvCity* pCity = getPlotCity();
	if (pCity)
	{
		pCity->kill(false);
	}

	setBonusType(NO_BONUS);
	setImprovementType(NO_IMPROVEMENT);
	setRouteType(NO_ROUTE, false);
	setFeatureType(NO_FEATURE);

	// disable rivers
	setNOfRiver(false, NO_CARDINALDIRECTION);
	setWOfRiver(false, NO_CARDINALDIRECTION);
	setRiverID(-1);
}

/*DllExport*/ float CvPlot::getPointX() const
{
	FAssertMsg(this != NULL, "CTD incoming");
#ifdef _DEBUG
	OutputDebugString(CvString::format("exe wants the X point for plot at (%d, %d)\n", getX(), getY()).c_str());
#endif
	const CvViewport* pCurrentViewport = GC.getCurrentViewport();

	return pCurrentViewport->plotXToPointX(pCurrentViewport->getViewportXFromMapX(getX()));
}


/*DllExport*/ float CvPlot::getPointY() const
{
	FAssertMsg(this != NULL, "CTD incoming");
#ifdef _DEBUG
	OutputDebugString(CvString::format("exe wants the Y point for plot at (%d, %d)\n", getX(), getY()).c_str());
#endif
	const CvViewport* pCurrentViewport = GC.getCurrentViewport();

	return pCurrentViewport->plotYToPointY(pCurrentViewport->getViewportYFromMapY(getY()));
}


NiPoint3 CvPlot::getPoint() const
{
	NiPoint3 pt3Point; memset(&pt3Point, 0, sizeof(pt3Point));

	if (this == NULL)
	{
		FErrorMsg("this == NULL");
	}
	else
	{
		const CvViewport* pCurrentViewport = GC.getCurrentViewport();

		pt3Point.x = pCurrentViewport->plotXToPointX(pCurrentViewport->getViewportXFromMapX(getX()));
		pt3Point.y = pCurrentViewport->plotYToPointY(pCurrentViewport->getViewportYFromMapY(getY()));
		pt3Point.z = 0.0f;
		pt3Point.z = gDLL->getEngineIFace()->GetHeightmapZ(pt3Point);
	}
	return pt3Point;
}


float CvPlot::getSymbolSize() const
{
	if (isVisibleWorked())
	{
		if (isShowCitySymbols())
		{
			return 1.6f;
		}
		return 1.2f;
	}
	if (isShowCitySymbols())
	{
		return 1.2f;
	}
	return 0.8f;
}


float CvPlot::getSymbolOffsetX(int iOffset) const
{
	return 40.0f + iOffset * 28.0f * getSymbolSize() - GC.getPLOT_SIZE() / 2.0f;
}


float CvPlot::getSymbolOffsetY(int iOffset) const
{
	return 50.0f - GC.getPLOT_SIZE() / 2.0f;
}


TeamTypes CvPlot::getTeam() const
{
	return isOwned() ? GET_PLAYER(getOwner()).getTeam() : NO_TEAM;
}


void CvPlot::doTurn()
{
	PROFILE_FUNC();

	if (isOwned())
	{
		setOwnershipDuration(getOwnershipDuration() + 1);

		bool bBonusDiscovery = getBonusType() == NO_BONUS;

		if (bBonusDiscovery)
		{
			bBonusDiscovery = doBonusDiscovery();
		}
		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (!bBonusDiscovery)
			{
				// Bonus on plot that was not discovered this turn.
				doBonusDepletion();
			}

			if (isImprovementUpgradable() && (isBeingWorked() || GC.getImprovementInfo(getImprovementType()).isUpgradeRequiresFortify()))
			{
				doImprovementUpgrade(getImprovementType());
			}
		}
	}

	// Super Forts *bombard*
	if (!isBombarded() && getDefenseDamage() > 0)
	{
		changeDefenseDamage(-GC.getCITY_DEFENSE_DAMAGE_HEAL_RATE());
	}
	setBombarded(false);
	// ! Super Forts

	doFeature();

	doCulture();

	verifyUnitValidPlot();

	doTerritoryClaiming();

	if (getOwner() != NO_PLAYER && !GET_PLAYER(getOwner()).isAlive())
	{
		setOwner(NO_PLAYER, false, false);
	}
#ifdef _DEBUG
	foreach_ (CvUnit* unit, units())
	{
		FAssertMsg(unit->atPlot(this), "pLoopUnit is expected to be at the current plot instance");
	}
#endif
}

bool CvPlot::doBonusDiscovery()
{
	PROFILE_FUNC();

	const bool bWorked = isBeingWorked();
	if (getImprovementType() == NO_IMPROVEMENT && !bWorked)
	{
		return false;
	}
	const CvImprovementInfo* improvement = (
		getImprovementType() != NO_IMPROVEMENT
		?
		&GC.getImprovementInfo(getImprovementType())
		:
		NULL
	);
	/* Toffer ToDo - would be nice to filter out all improvements that don't have a single bonus discovery rand with some cache...
	if (!bWorked && getImprovementType() != NO_IMPROVEMENT && !improvement.canDiscover())
	{
		return false;
	}
	*/
	CvGame& game = GC.getGame();
	const CvMap& map = GC.getMap();
	const CvTeam& team = GET_TEAM(getTeam());
	const int iWorldSizeFactor = map.getWorldSize() + 4;
	const int iGameSpeedFactor = GC.getGameSpeedInfo(game.getGameSpeedType()).getHammerCostPercent();
	const int iNumBonuses = GC.getNumMapBonuses();
	// Toffer - Can't check all bonuses for all valid plots every turn
	//	bWorked is more likely to be true for plots in late game, which is when optimization is most dire.
	int iMaxAttempts = bWorked ? 15 : 45;
	int iAttempts = 0;
	int iIndex = game.getSorenRandNum(iNumBonuses, "Random start index");
	int iCount = 0;
	while (iAttempts < iMaxAttempts && iCount++ < iNumBonuses)
	{
		const BonusTypes eBonus = GC.getMapBonus(iIndex);

		bool bImpBonus = false;
		int iOdds = improvement ? improvement->getImprovementBonusDiscoverRand(eBonus) : 0;

		if (iOdds > 0)
		{
			bImpBonus = true;
		}
		else if (bWorked)
		{
			iOdds = 40000; // small chance always there when worked by city.
		}
		else
		{
			// Toffer: we won't go through all map bonuses if it is an unworked tile, limiting it makes sense.
			//	Some improvements don't have any discovery chances... this should be cached on game launch
			//		so we don't have to loop even the limited set for those improvements.
			iAttempts++;
		}

		if (iOdds > 0 && team.isHasTech((TechTypes)GC.getBonusInfo(eBonus).getTechReveal()) && canHaveBonus(eBonus))
		{
			iOdds *= iGameSpeedFactor;
			iOdds /= 100;
			// Bonus density normalization
			iOdds *= 10 * (map.getNumBonuses(eBonus) + 1);
			iOdds /= 25 * iWorldSizeFactor;

			if (iOdds < 2 || game.getSorenRandNum(iOdds, "Bonus Discovery") == 0)
			{
				setBonusType(eBonus);

				const CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);

				if (pCity != NULL && isInViewport())
				{
					if (bImpBonus)
					{
						AddDLLMessage(
							getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE_IMPROVEMENT",
								pCity->getNameKey(), GC.getBonusInfo(eBonus).getTextKeyWide()
							),
							"AS2D_DISCOVERBONUS", MESSAGE_TYPE_MINOR_EVENT, GC.getBonusInfo(eBonus).getButton(),
							GC.getCOLOR_WHITE(), getViewportX(), getViewportY(), true, true
						);
					}
					else
					{
						AddDLLMessage(
							getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
							gDLL->getText(
								"TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE_CITY",
								pCity->getNameKey(), GC.getBonusInfo(eBonus).getTextKeyWide()
							),
							"AS2D_DISCOVERBONUS", MESSAGE_TYPE_MINOR_EVENT, GC.getBonusInfo(eBonus).getButton(),
							GC.getCOLOR_WHITE(), getViewportX(), getViewportY(), true, true
						);
					}
				}
				return true;
			}
			if (!bWorked && bImpBonus)
			{
				return false;
			}
			iAttempts++;
		}
		if (++iIndex == iNumBonuses)
		{
			iIndex = 0;
		}
	}
	return false;
}

void CvPlot::doBonusDepletion()
{
	PROFILE_FUNC();

	if (!GC.getGame().isModderGameOption(MODDERGAMEOPTION_RESOURCE_DEPLETION))
	{
		return;
	}
	const BonusTypes eBonus = getNonObsoleteBonusType(getTeam());

	if (eBonus == NO_BONUS
	|| !GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(eBonus)
	|| !GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo(eBonus).getTechCityTrade())))
	{
		return;
	}
	int iOdds = GC.getImprovementInfo(getImprovementType()).getImprovementBonusDepletionRand(eBonus);
	if (iOdds < 1)
	{
		return;
	}
	iOdds *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
	iOdds /= 100;
	// Bonus density normalization.
	iOdds *= 25 * (GC.getMap().getWorldSize() + 4);
	iOdds /= 10 * (GC.getMap().getNumBonuses(eBonus) + 1);

	// This routine is only called for owned plots, no need to check if NO_PLAYER.
	const int iValue = GET_PLAYER(getOwner()).getResourceConsumption(eBonus);
	if (iValue > 0)
	{
		iOdds *= 100;
		iOdds /= iValue;
	}
	if (iOdds < 2 || GC.getGame().getSorenRandNum(iOdds, "Bonus Depletion") == 0)
	{
		AddDLLMessage(
			getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
			gDLL->getText(
				"TXT_KEY_MISC_RESOURCE_DEPLETED",
				GC.getBonusInfo(eBonus).getTextKeyWide(),
				GC.getImprovementInfo(getImprovementType()).getDescription()
			),
			NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBonusInfo(eBonus).getButton(),
			GC.getCOLOR_RED(), getX(), getY(), true, true
		);
		logging::logMsg("C2C.log", "Resource Depleted! Resource was %d, The odds were 1 in %d\n", eBonus, iOdds);

		setBonusType(NO_BONUS);

		CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);

		if (pCity != NULL) pCity->AI_setAssignWorkDirty(true);

		if (!canHaveImprovement(getImprovementType(), getTeam()))
		{
			setImprovementType(NO_IMPROVEMENT);
		}
	}
}


void CvPlot::doImprovementUpgrade(const ImprovementTypes eType)
{
	PROFILE_EXTRA_FUNC();
	const TeamTypes eTeam = GET_PLAYER(getOwner()).getTeam();

	// Has the situation changed?
	if (GET_TEAM(eTeam).getLastRoundOfValidImprovementCacheUpdate() == m_iCurrentRoundofUpgradeCache)
	{
		return; // Nope
	}
	// Evaluate potential upgrades
	ImprovementTypes eUpgrade = NO_IMPROVEMENT;
	int iHash = 512; // Super simple hash, but should be adequate for this purpose.
	const ImprovementTypes eMainUpgrade = GC.getImprovementInfo(eType).getImprovementUpgrade();

	if (canHaveImprovement(eMainUpgrade, eTeam, false, true))
	{
		// Toffer - Upgrade cost code commented out in setImprovementType() for the time being
		// if (GC.getImprovementInfo(eMainUpgrade).getHighestCost() <= GET_PLAYER(getOwner()).getGold())
		{
			iHash *= -2;
			iHash += eMainUpgrade;
		}
		eUpgrade = eMainUpgrade;
	}
	for (int iI = 0; iI < GC.getImprovementInfo(getImprovementType()).getNumAlternativeImprovementUpgradeTypes(); ++iI)
	{
		const ImprovementTypes eUpgradeX = (ImprovementTypes)GC.getImprovementInfo(getImprovementType()).getAlternativeImprovementUpgradeType(iI);

		if (canHaveImprovement(eUpgradeX, eTeam, false, true))
		{
			eUpgrade = eUpgradeX;
			// Toffer - Upgrade cost code commented out in setImprovementType() for the time being
			//if (GC.getImprovementInfo(eUpgradeX).getHighestCost() <= GET_PLAYER(getOwner()).getGold())
			{
				iHash *= -2;
				iHash += eUpgradeX;
			}
		}
	}
	if (iHash == 512)
	{
		// Lack of money change every turn
		if (eUpgrade == NO_IMPROVEMENT)
		{ // This however warrant a freeze to this check.
			setImprovementUpgradeCache(GET_TEAM(eTeam).getLastRoundOfValidImprovementCacheUpdate());
		}
		return;
	}
	const int iTime = 100*GC.getGame().getImprovementUpgradeTime(eType);

	// Advance upgrade progress if not already finished
	if (
		getImprovementUpgradeProgress() < iTime
	// And not require fortified unit, or unit is fortified on improvement.
	&& (
			!GC.getImprovementInfo(eType).isUpgradeRequiresFortify()
		||
			algo::any_of(units(), CvUnit::fn::getFortifyTurns() > 0 && CvUnit::fn::getTeam() == eTeam && CvUnit::fn::canDefend())
		)
	) changeImprovementUpgradeProgress(GET_PLAYER(getOwner()).getImprovementUpgradeProgressRate(eType));

	if (getImprovementUpgradeProgress() >= iTime)
	{
		setImprovementUpgradeCache(GET_TEAM(eTeam).getLastRoundOfValidImprovementCacheUpdate());

		if (iHash == m_iImprovementUpgradeHash)
		{ // Same selection as last time.
			return;
		}
		m_iImprovementUpgradeHash = iHash;

		if (eUpgrade == eMainUpgrade) // Only one potential upgrade for this plot.
		{
			setImprovementType(eUpgrade);
		}
		else if (GET_PLAYER(getOwner()).isHumanPlayer()) //send popup if player is human
		{
			GET_PLAYER(getOwner()).upgradePlotPopup(getImprovementType(), getX(), getY());
		}
		else // AI choice evaluation
		{
			// Toffer - Improve AI_getImprovementValue ** TODO **
			ImprovementTypes eBestUpgrade = NO_IMPROVEMENT;

			CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);
			if (pCity == NULL) return;

			int iFoodMultiplier = 0;
			int iProductionMultiplier = 0;
			int iCommerceMultiplier = 0;
			int iDesiredFoodChange = 0;
			pCity->AI_getYieldMultipliers(iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, iDesiredFoodChange);

			int iBestValue = pCity->AI_getImprovementValue(this, eType, iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, iDesiredFoodChange);

			// Toffer - Upgrade cost code commented out in setImprovementType() for the time being
			if (/*GC.getImprovementInfo(eMainUpgrade).getHighestCost() <= GET_PLAYER(getOwner()).getGold() && */ canHaveImprovement(eMainUpgrade, eTeam, false, true))
			{
				int iValue = pCity->AI_getImprovementValue(this, eMainUpgrade, iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, iDesiredFoodChange);
				if (iValue > iBestValue)
				{
					eBestUpgrade = eMainUpgrade;
					iBestValue = iValue;
				}
			}
			for (int iI = 0; iI < GC.getImprovementInfo(getImprovementType()).getNumAlternativeImprovementUpgradeTypes(); ++iI)
			{
				const ImprovementTypes eUpgradeX = (ImprovementTypes)GC.getImprovementInfo(getImprovementType()).getAlternativeImprovementUpgradeType(iI);

				// Toffer - Upgrade cost code commented out in setImprovementType() for the time being
				if (/* GC.getImprovementInfo(eUpgradeX).getHighestCost() <= GET_PLAYER(getOwner()).getGold() && */ canHaveImprovement(eUpgradeX, eTeam, false, true))
				{
					int iValue = pCity->AI_getImprovementValue(this, eUpgradeX, iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, iDesiredFoodChange);
					if (iValue > iBestValue)
					{
						eBestUpgrade = eUpgradeX;
						iBestValue = iValue;
					}
				}
			}
			if (eBestUpgrade != NO_IMPROVEMENT)
			{
				setImprovementType(eBestUpgrade);
			}
		}
	}
}


void CvPlot::updateCulture(bool bBumpUnits, bool bUpdatePlotGroups)
{
	// Toffer - Cities and forts only flip ownership from culture at the end of the game turn, not every time culture change.
	if (!getPlotCity() && !isActsAsCity())
	{
		setOwner(calculateCulturalOwner(), bBumpUnits, bUpdatePlotGroups);
	}
}

void CvPlot::checkCityRevolt()
{
	PROFILE_EXTRA_FUNC();
	// Check if city flips (revolts)
	CvCity* pCity = getPlotCity();
	if (!pCity || pCity->isOccupation())
	{
		return;
	}
	const PlayerTypes eCulturalOwner = calculateCulturalOwner(false);
	FAssert(eCulturalOwner != NO_PLAYER);

	if (eCulturalOwner == NO_PLAYER
	// Toffer - Don't flip ownership of city to a player who is not currently adding the most culture to the plot per turn.
	|| getCultureRateThisTurn(eCulturalOwner) <= getCultureRateThisTurn(getOwner())
	// Don't flip cities between team members
	|| GET_PLAYER(eCulturalOwner).getTeam() == getTeam())
	{
		return;
	}
	const int iRevoltTestProb = (
		std::max(
			1,
			GC.getREVOLT_TEST_PROB()* 100/GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()
		)
	);
	// Check to check for revolt, rate adjusted by gamespeed, at least 1% minimum.
	// If adjusted, also update `int iSpeedAdjustment` in CvGameTextMgr, CvDLLWidgetData
	if (GC.getGame().getSorenRandNum(100, "Revolt #1") < iRevoltTestProb)
	{
		// iCityStrength100 is 100x % chance of revolt
		const int iCityStrength100 = pCity->netRevoltRisk100(eCulturalOwner);
		const int iRevoltRoll = GC.getGame().getSorenRandNum(10000, "Revolt #2");

		// Successful revolt
		if (pCity->isNPC() || iRevoltRoll < iCityStrength100)
		{
			foreach_(CvUnit* pLoopUnit, units())
			{
				if (pLoopUnit->isNPC())
				{
					pLoopUnit->kill(false, eCulturalOwner);
				}
				else if (pLoopUnit->canDefend())
				{
					pLoopUnit->changeDamage(pLoopUnit->getHP() / 2, eCulturalOwner);
				}
			}

			if (pCity->isNPC()
			|| !GC.getGame().isOption(GAMEOPTION_CULTURE_NO_CITY_FLIPPING)
			&& (GC.getGame().isOption(GAMEOPTION_CULTURE_FLIPPING_AFTER_CONQUEST) || !pCity->isEverOwned(eCulturalOwner))
			&&	pCity->getNumRevolts(eCulturalOwner) >= GC.getDefineINT("NUM_WARNING_REVOLTS"))
			{
				if (GC.getGame().isOption(GAMEOPTION_CHALLENGE_ONE_CITY))
				{
					pCity->kill(true);
				}
				else
				{
					setOwner(eCulturalOwner, true, true); // Will invalidate pCity pointer.
				}
			}
			else
			{
				pCity->changeNumRevolts(eCulturalOwner, 1);
				pCity->changeOccupationTimer(
					GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent()
					*
					(GC.getDefineINT("BASE_REVOLT_OCCUPATION_TURNS") + iCityStrength100 * GC.getDefineINT("REVOLT_OCCUPATION_TURNS_PERCENT") / 10000)
					/
					100
				);

				// XXX announce for all seen cities?

				if (isInViewport())
				{
					CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_REVOLT_IN_CITY", GET_PLAYER(eCulturalOwner).getCivilizationAdjective(), pCity->getNameKey());
					AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MINOR_EVENT,
						ARTFILEMGR.getInterfaceArtInfo("INTERFACE_RESISTANCE")->getPath(), GC.getCOLOR_RED(), getViewportX(),getViewportY(), true, true);

					AddDLLMessage(eCulturalOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MINOR_EVENT,
						ARTFILEMGR.getInterfaceArtInfo("INTERFACE_RESISTANCE")->getPath(), GC.getCOLOR_GREEN(), getViewportX(),getViewportY(), true, true);
				}
			}
		}
		// Revolt is possible, but got lucky this time
		else if (((iRevoltRoll - iCityStrength100) < iCityStrength100) && isInViewport())
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_FAILED_REVOLT_IN_CITY", GET_PLAYER(eCulturalOwner).getCivilizationAdjective(), pCity->getNameKey());
			AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT_END", MESSAGE_TYPE_MINOR_EVENT,
				ARTFILEMGR.getInterfaceArtInfo("INTERFACE_RESISTANCE")->getPath(), GC.getCOLOR_RED(), getViewportX(),getViewportY(), true, true);

			AddDLLMessage(eCulturalOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT_END", MESSAGE_TYPE_MINOR_EVENT,
				ARTFILEMGR.getInterfaceArtInfo("INTERFACE_RESISTANCE")->getPath(), GC.getCOLOR_GREEN(), getViewportX(),getViewportY(), true, true);
		}
	}
	// ~2x more likely to be alerted of high discontent than revolt or quelled one (don't want every turn)
	else if (GC.getGame().getSorenRandNum(100, "Revolt #1 Alert") < 2 * iRevoltTestProb
		&& isInViewport())
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DISCONTENT_IN_CITY", GET_PLAYER(eCulturalOwner).getCivilizationAdjective(), pCity->getNameKey());
		AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_ADVISOR_SUGGEST", MESSAGE_TYPE_MINOR_EVENT,
			ARTFILEMGR.getInterfaceArtInfo("INTERFACE_ANGRYCITIZEN_TEXTURE")->getPath(), GC.getCOLOR_RED(), getViewportX(),getViewportY(), true, true);

		AddDLLMessage(eCulturalOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_ADVISOR_SUGGEST", MESSAGE_TYPE_MINOR_EVENT,
			ARTFILEMGR.getInterfaceArtInfo("INTERFACE_ANGRYCITIZEN_TEXTURE")->getPath(), GC.getCOLOR_GREEN(), getViewportX(),getViewportY(), true, true);
	}
}

void CvPlot::checkFortRevolt()
{
	if (getImprovementType() == NO_IMPROVEMENT)
		return;

	const CvImprovementInfo& improvement = GC.getImprovementInfo(getImprovementType());
	const PlayerTypes eOwner = getOwner();

	if (improvement.isActsAsCity() && getOwnershipDuration() > GC.getDefineINT("SUPER_FORTS_DURATION_BEFORE_REVOLT"))
	{
		// Check for a fort culture flip. Fixed borders altered threshold checked there
		const PlayerTypes eCulturalOwner = calculateCulturalOwner(false);
		if (eCulturalOwner != NO_PLAYER && eCulturalOwner != eOwner && GET_PLAYER(eCulturalOwner).getTeam() != getTeam())
		{
			// Defenders prevent flipping regardless of fixed borders.
			const bool bDefenderFound = algo::any_of(units(), bind(CvUnit::getOwner, _1) == eOwner && bind(CvUnit::canDefend, _1, this));
			if (!bDefenderFound)
			{
				const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_REVOLTED_JOINED", improvement.getText(), GET_PLAYER(eCulturalOwner).getCivilizationDescriptionKey());
				AddDLLMessage(eOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_INFO,
					improvement.getButton(), GC.getCOLOR_RED(), getX(), getY(), true, true);

				AddDLLMessage(eCulturalOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_INFO,
					improvement.getButton(), GC.getCOLOR_GREEN(), getX(), getY(), true, true);

				setOwner(eCulturalOwner, true, true);
			}
		}
	}
}


void CvPlot::updateFog(const bool bApplyDecay)
{
	PROFILE_FUNC();

	if (!shouldHaveGraphics())
	{
		return;
	}
	const TeamTypes &team = GC.getGame().getActiveTeam();
	const PlayerTypes myID = GC.getGame().getActivePlayer();
	const bool bIsHuman = GET_TEAM(team).isHuman() || GC.getGame().getAIAutoPlay(myID) > 0 || gDLL->GetAutorun();
	const bool bOptionDecay = GC.getGame().isModderGameOption(MODDERGAMEOPTION_FOGWAR_DECAY);
	FAssert(team != NO_TEAM);

	if (isRevealed(team, false))
	{
		if (
			gDLL->getInterfaceIFace()->isBareMapMode()
			||
			(
				isActiveVisible(false)
				&&
				(
					!gDLL->getInterfaceIFace()->isCityScreenUp()
					||
					// City screen - only lighten plots belonging to the city's current workable area.
					gDLL->getInterfaceIFace()->getHeadSelectedCity() == getWorkingCity()
					)
				)
		)
		{
#ifdef ENABLE_FOGWAR_DECAY
			if (bIsHuman && (bApplyDecay || !bOptionDecay))
			{
				bool bSeaPlot = isWater() && !isCoastal();
				m_iVisibilityDecay = GET_TEAM(team).getVisibilityDecay(bSeaPlot);
				if (m_iVisibilityDecay != NO_DECAY)
				{
					m_iVisibilityDecay += getVisibilityDecayBonus(bSeaPlot);
				}
			}
#endif
			gDLL->getEngineIFace()->LightenVisibility(getFOWIndex());
		}
		else
		{
#ifdef ENABLE_FOGWAR_DECAY
			if (!bIsHuman || m_iVisibilityDecay == NO_DECAY || !bOptionDecay)
			{
#endif
				gDLL->getEngineIFace()->DarkenVisibility(getFOWIndex());
#ifdef ENABLE_FOGWAR_DECAY
			}
			else
			{
				if (m_iVisibilityDecay > 0)
				{
					gDLL->getEngineIFace()->DarkenVisibility(getFOWIndex());



					if (bApplyDecay && GC.getGame().getSorenRandNum(12, "Map decay") > 8)
						m_iVisibilityDecay--;
				}
				else if (m_iVisibilityDecay > REMOVE_PLOT_DECAY)
				{
					gDLL->getEngineIFace()->BlackenVisibility(getFOWIndex());
					m_iVisibilityDecay--;
				}
				else
				{
					gDLL->getEngineIFace()->BlackenVisibility(getFOWIndex());
					setRevealed(team, false, false, NO_TEAM, false,false);
				}
			}
#endif
		}

	} else gDLL->getEngineIFace()->BlackenVisibility(getFOWIndex());
}


void CvPlot::updateVisibility()
{
	PROFILE("CvPlot::updateVisibility");

	if ( !shouldHaveGraphics() )
	{
		return;
	}

	setLayoutDirty(true);

	updateSymbolVisibility();
	updateFeatureSymbolVisibility();
	updateRouteSymbol();

	CvCity* pCity = getPlotCity();
	if (pCity != NULL && !pCity->isNPC())
	{
		pCity->updateVisibility();
	}
}

#ifdef ENABLE_FOGWAR_DECAY
void CvPlot::InitFogDecay(const bool pWithRandom)
{
	const TeamTypes& team = GC.getGame().getActiveTeam();
	if (team != NO_TEAM)
	{
		m_iVisibilityDecay = GET_TEAM(team).getVisibilityDecay();
	}
	if (m_iVisibilityDecay <= 0)
	{
		m_iVisibilityDecay = 6;
	}
	m_iVisibilityDecay += getVisibilityDecayBonus();
	if (pWithRandom)
	{
		m_iVisibilityDecay = GC.getGame().getSorenRandNum((std::max(m_iVisibilityDecay * 2 / 3, 2)), "InitFog Decay") + m_iVisibilityDecay / 3;
	}
}

short CvPlot::getVisibilityDecayBonus(const bool pSeaPlot)
{
	const FeatureTypes eFeature = getFeatureType();
	const BonusTypes eBonusType = getBonusType();
	int iVisibilityDecay = 0;
	if (eFeature != NO_FEATURE)
	{
		const CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(eFeature);
		const CvString featureString = kFeatureInfo.getType();
		const CvBonusInfo& kBonusInfo = GC.getBonusInfo(eBonusType);
		if (featureString == "FEATURE_OASIS" || featureString == "FEATURE_CAVES" || featureString == "FEATURE_CITY_RUINS")
		{
			iVisibilityDecay += 3;
		}
		if (featureString == "FEATURE_REEF_LIGHTHOUSE")
		{
			iVisibilityDecay += 9;
		}
		if (featureString.find("FEATURE_PLATY_") != std::string::npos)
		{
			iVisibilityDecay += 40;
		}
	}
	if (eBonusType != NO_BONUS)
	{
		iVisibilityDecay += 3;
	}
	if (getOwner() != NO_PLAYER)
	{
		iVisibilityDecay += 1;
	}
	if (isCityRadius())
	{
		iVisibilityDecay += 2;
	}
	if (isCity())
	{
		iVisibilityDecay += 6;
	}
	if (isFreshWater())
	{
		iVisibilityDecay += 3;
	}
	return iVisibilityDecay;
}
#endif


void CvPlot::updateSymbolDisplay()
{
	PROFILE_FUNC();

	if ( !isGraphicsVisible(ECvPlotGraphics::SYMBOLS))
	{
		return;
	}

	for (int iLoop = 0; iLoop < getNumSymbols(); iLoop++)
	{
		CvSymbol* pLoopSymbol = getSymbol(iLoop);

		if (pLoopSymbol != NULL)
		{
			if (isShowCitySymbols())
			{
				gDLL->getSymbolIFace()->setAlpha(pLoopSymbol, (isVisibleWorked()) ? 1.0f : 0.8f);
			}
			else
			{
				gDLL->getSymbolIFace()->setAlpha(pLoopSymbol, (isVisibleWorked()) ? 0.8f : 0.6f);
			}
			gDLL->getSymbolIFace()->setScale(pLoopSymbol, getSymbolSize());
			gDLL->getSymbolIFace()->updatePosition(pLoopSymbol);
		}
	}
}


void CvPlot::updateSymbolVisibility()
{
	PROFILE_FUNC();

	if ( !isGraphicsVisible(ECvPlotGraphics::SYMBOLS) )
	{
		return;
	}

	if ( updateSymbolsInternal() )
	{
		updateSymbolDisplay();

		for (int iLoop = 0; iLoop < getNumSymbols(); iLoop++)
		{
			CvSymbol* pLoopSymbol = getSymbol(iLoop);

			if (pLoopSymbol != NULL)
			{
				gDLL->getSymbolIFace()->Hide(pLoopSymbol, false);
			}
		}
	}
}


void CvPlot::updateSymbols()
{
	updateSymbolsInternal();
}

bool CvPlot::updateSymbolsInternal()
{
	PROFILE_FUNC();

	deleteAllSymbols();
	if (!isGraphicsVisible(ECvPlotGraphics::SYMBOLS) || !isRevealed(GC.getGame().getActiveTeam(), true))
	{
		return false;
	}

	if (isShowCitySymbols() || gDLL->getInterfaceIFace()->isShowYields() && !gDLL->getInterfaceIFace()->isCityScreenUp())
	{
		int yieldAmounts[NUM_YIELD_TYPES];
		int maxYield = 0;
		int iYield = 0;
		for (int iYieldType = 0; iYieldType < NUM_YIELD_TYPES; iYieldType++)
		{
			iYield = calculateYield((YieldTypes)iYieldType, true);
			yieldAmounts[iYieldType] = iYield;
			if (iYield > maxYield)
			{
				maxYield = iYield;
			}
		}

		if (maxYield)
		{
			const int iMaxYieldStack = GC.getMAX_YIELD_STACK();
			CvSymbol *pSymbol5x = addSymbol();
			CvSymbol *pSymbol= addSymbol();

			for (int iYieldType = 0; iYieldType < NUM_YIELD_TYPES; iYieldType++)
			{
				iYield = yieldAmounts[iYieldType];

				if (iYield)
				{
					if (iYield > 29)
					{
						gDLL->getSymbolIFace()->setTypeYield(pSymbol5x, iYieldType, iMaxYieldStack-1);
						gDLL->getSymbolIFace()->setTypeYield(pSymbol, iYieldType, 4);
					}
					else if (iYield > 4)
					{
						const int iYieldBig = iYield / 5;
						iYield -= 5 * iYieldBig;
						gDLL->getSymbolIFace()->setTypeYield(pSymbol5x, iYieldType, iYieldBig + 4);

						if (iYield)
						{
							gDLL->getSymbolIFace()->setTypeYield(pSymbol, iYieldType, iYield);
						}
						else
						{
							gDLL->getSymbolIFace()->setTypeYield(pSymbol, iYieldType, iMaxYieldStack);
						}
					}
					else
					{
						gDLL->getSymbolIFace()->setTypeYield(pSymbol5x, iYieldType, iMaxYieldStack);
						gDLL->getSymbolIFace()->setTypeYield(pSymbol, iYieldType, iYield);
					}
				}
			}
			for (int i=0; i < getNumSymbols(); i++)
			{
				SymbolTypes eSymbol  = (SymbolTypes)0;
				pSymbol = getSymbol(i);
				gDLL->getSymbolIFace()->init(pSymbol, gDLL->getSymbolIFace()->getID(pSymbol), i, eSymbol, this);
			}
		}
		return true;
	}
	return false;
}


void CvPlot::updateMinimapColor()
{
	PROFILE_FUNC();

	if (!shouldHaveGraphics())
	{
		return;
	}
	gDLL->getInterfaceIFace()->setMinimapColor(MINIMAPMODE_TERRITORY, getViewportX(),getViewportY(), plotMinimapColor(), STANDARD_MINIMAP_ALPHA);
}

bool CvPlot::unitHere(const CvUnit* pUnit) const
{
	return algo::any_of_equal(units(), pUnit);
}

// Toffer - ToDo: Improving this hot mess
CvUnit* CvPlot::getPreferredCenterUnit() const
{
	CvUnit* pNewCenterUnit = getSelectedUnit();

	if (pNewCenterUnit)
	{
		FAssertMsg(pNewCenterUnit->atPlot(this), "Selected unit isn't in the plot that owns it");
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(GC.getGame().getActivePlayer(), NO_PLAYER, NULL, false, false, true);

	if (pNewCenterUnit)
	{
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(GC.getGame().getActivePlayer());

	if (pNewCenterUnit)
	{
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(NO_PLAYER, GC.getGame().getActivePlayer(), gDLL->getInterfaceIFace()->getHeadSelectedUnit(), true);

	if (pNewCenterUnit)
	{
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(NO_PLAYER, GC.getGame().getActivePlayer(), gDLL->getInterfaceIFace()->getHeadSelectedUnit());

	if (pNewCenterUnit)
	{
		return pNewCenterUnit;
	}
	return getBestDefender(NO_PLAYER, GC.getGame().getActivePlayer());
}


void CvPlot::verifyUnitValidPlot()
{
	PROFILE_FUNC();

	bool bAnyMoved = false;

	std::vector<CvUnit*> aUnits(beginUnits(), endUnits());

	for (std::vector<CvUnit*>::iterator it = aUnits.begin(); it != aUnits.end(); )
	{
		CvUnit* unitX = *it;
		bool bErased = false;

		if (unitX->atPlot(this) && !unitX->isCargo() && !unitX->isCombat()
		&& (!isValidDomainForLocation(*unitX) || !unitX->canEnterArea(getTeam(), area())))
		{
			bAnyMoved = true;
			if (!unitX->jumpToNearestValidPlot())
			{
				bErased = true;
			}
		}

		if (bErased)
		{
			it = aUnits.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Check groups are not broken by only some of their constituent units having been moved
	if (bAnyMoved)
	{
		foreach_(CvUnit* unitX, aUnits)
		{
			const CvSelectionGroup* pGroup = unitX->getGroup();

			if (pGroup && !pGroup->isMidMove())
			{
				const CvUnit* pHeadUnit = pGroup->getHeadUnit();

				if (pHeadUnit && pHeadUnit->plot() != unitX->plot())
				{
					unitX->joinGroup(NULL);
				}
			}
		}
	}
}


void CvPlot::nukeExplosion(int iRange, CvUnit* pNukeUnit)
{
	PROFILE_EXTRA_FUNC();
	GC.getGame().changeNukesExploded(1);

	foreach_(CvPlot* plotX, rect(iRange, iRange))
	{
		CvCity* city = plotX->getPlotCity();

		if (!city && !plotX->isWater() && !plotX->isImpassable()
		&& (NO_FEATURE == plotX->getFeatureType() || !GC.getFeatureInfo(plotX->getFeatureType()).isNukeImmune())
		&& GC.getGame().getSorenRandNum(100, "Nuke Fallout") < GC.getDefineINT("NUKE_FALLOUT_PROB"))
		{
			plotX->setImprovementType(NO_IMPROVEMENT);
			plotX->setFeatureType((FeatureTypes)GC.getDefineINT("NUKE_FEATURE"));
		}

		foreach_(CvUnit* pLoopUnit, plotX->units_safe())
		{
			if (pLoopUnit != pNukeUnit && !pLoopUnit->isNukeImmune() && !pLoopUnit->isDelayedDeath())
			{
				int iNukeDamage =
				(
					GC.getDefineINT("NUKE_UNIT_DAMAGE_BASE")
					+ GC.getGame().getSorenRandNum(GC.getDefineINT("NUKE_UNIT_DAMAGE_RAND_1"), "Nuke Damage 1")
					+ GC.getGame().getSorenRandNum(GC.getDefineINT("NUKE_UNIT_DAMAGE_RAND_2"), "Nuke Damage 2")
				);
				if (city)
				{
					iNukeDamage *= std::max(0, city->getNukeModifier() + 100);
					iNukeDamage /= 100;
				}

				if (pLoopUnit->canFight() || pLoopUnit->airBaseCombatStr() > 0)
				{
					pLoopUnit->changeDamage(iNukeDamage, pNukeUnit ? pNukeUnit->getOwner() : NO_PLAYER);
				}
				else if (iNukeDamage >= GC.getDefineINT("NUKE_NON_COMBAT_DEATH_THRESHOLD"))
				{
					pLoopUnit->kill(true, pNukeUnit ? pNukeUnit->getOwner() : NO_PLAYER);
				}
			}
		}

		if (city)
		{
			const int iPop = city->getPopulation();
			if (iPop < 2 && plotX == this)
			{
				GC.getGame().addDestroyedCityName(city->getName());
				city->kill(true);
				continue;
			}
			{
				std::vector<BuildingTypes> temp;

				foreach_(const BuildingTypes eType, city->getHasBuildings())
				{
					if (!GC.getBuildingInfo(eType).isNukeImmune()
					&& GC.getGame().getSorenRandNum(100, "Building Nuked") < GC.getDefineINT("NUKE_BUILDING_DESTRUCTION_PROB"))
					{
						temp.push_back(eType);
					}
				}
				foreach_(const BuildingTypes eType, temp)
				{
					city->changeHasBuilding(eType, false);
				}
			}
			const int iNukedPopulation =
			(
				iPop * std::max(0, city->getNukeModifier() + 100)
				*
				(
					GC.getDefineINT("NUKE_POPULATION_DEATH_BASE")
					+ GC.getGame().getSorenRandNum(GC.getDefineINT("NUKE_POPULATION_DEATH_RAND_1"), "Population Nuked 1")
					+ GC.getGame().getSorenRandNum(GC.getDefineINT("NUKE_POPULATION_DEATH_RAND_2"), "Population Nuked 2")
				)
				/ 10000
			);
			city->changePopulation(-std::min(iPop - 1, iNukedPopulation));
		}
	}
	CvEventReporter::getInstance().nukeExplosion(this, pNukeUnit);
}


bool CvPlot::isConnectedTo(const CvCity* pCity) const
{
	return isOwned() && (getPlotGroup(getOwner()) == pCity->plotGroup(getOwner()) || getPlotGroup(pCity->getOwner()) == pCity->plotGroup(pCity->getOwner()));
}


bool CvPlot::isConnectedToCapital(PlayerTypes ePlayer) const
{
	if (ePlayer == NO_PLAYER)
	{
		ePlayer = getOwner();
	}

	if (ePlayer != NO_PLAYER)
	{
		const CvCity* pCapitalCity = GET_PLAYER(ePlayer).getCapitalCity();

		if (pCapitalCity)
		{
			return isConnectedTo(pCapitalCity);
		}
	}
	return false;
}


int CvPlot::getPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);
	FASSERT_BOUNDS(0, GC.getNumBonusInfos(), eBonus);

	const CvPlotGroup* pPlotGroup = getPlotGroup(ePlayer);

	return pPlotGroup ? pPlotGroup->getNumBonuses(eBonus) : 0;
}


bool CvPlot::isPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	return getPlotGroupConnectedBonus(ePlayer, eBonus) > 0;
}


bool CvPlot::isAdjacentPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	return algo::any_of(adjacent(), CvPlot::fn::isPlotGroupConnectedBonus(ePlayer, eBonus));
}


void CvPlot::updatePlotGroupBonus(bool bAdd)
{
	PROFILE_FUNC();

	if (!isOwned())
	{
		return;
	}
	CvPlotGroup* pPlotGroup = getPlotGroup(getOwner());

	if (pPlotGroup != NULL)
	{
		CvCity* pPlotCity = getPlotCity();

		if (pPlotCity != NULL)
		{
			PROFILE("CvPlot::updatePlotGroupBonus.PlotCity");

			for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
			{
				if (!GET_TEAM(getTeam()).isBonusObsolete((BonusTypes)iI))
				{
					pPlotGroup->changeNumBonuses((BonusTypes)iI, (pPlotCity->getFreeBonus((BonusTypes)iI) * (bAdd ? 1 : -1)));
				}
			}

			if (pPlotCity->isCapital())
			{
				PROFILE("CvPlot::updatePlotGroupBonus.Capital");

				for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					pPlotGroup->changeNumBonuses((BonusTypes)iI, (GET_PLAYER(getOwner()).getBonusExport((BonusTypes)iI) * (bAdd ? -1 : 1)));
					pPlotGroup->changeNumBonuses((BonusTypes)iI, (GET_PLAYER(getOwner()).getBonusImport((BonusTypes)iI) * (bAdd ? 1 : -1)));
				}
			}
		}

		if (isBonusExtracted())
		{
			pPlotGroup->changeNumBonuses(getBonusType(), (bAdd ? 1 : -1));
		}
	}
}

bool CvPlot::isBonusExtracted(const TeamTypes eTeamPerspective) const
{
	PROFILE("CvPlot::isBonusExtracted");

	const TeamTypes eOwnerTeam = (
		eTeamPerspective == NO_TEAM
		?
		getTeam()
		:
		getRevealedTeam(eTeamPerspective, true)
	);
	if (eOwnerTeam == NO_TEAM)
	{
		return false;
	}
	const BonusTypes eRelevantBonus = getNonObsoleteBonusType(eOwnerTeam);

	if (eRelevantBonus == NO_BONUS || !GET_TEAM(eOwnerTeam).isHasTech((TechTypes)GC.getBonusInfo(eRelevantBonus).getTechCityTrade()))
	{
		return false;
	}
	if (!isCity(false, eOwnerTeam) && (getImprovementType() == NO_IMPROVEMENT || !GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(eRelevantBonus)))
	{
		return false;
	}
	return isBonusNetwork(eOwnerTeam);
}


bool CvPlot::isAdjacentToArea(int iAreaID) const
{
	PROFILE_FUNC();

	return algo::any_of(adjacent(), CvPlot::fn::getArea() == iAreaID);
}

bool CvPlot::isAdjacentToArea(const CvArea* pArea) const
{
	return isAdjacentToArea(pArea->getID());
}


bool CvPlot::shareAdjacentArea(const CvPlot* pPlot) const
{
	PROFILE_FUNC();


	int iLastArea = FFreeList::INVALID_INDEX;

	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		const int iCurrArea = pAdjacentPlot->getArea();

		if (iCurrArea != iLastArea)
		{
			if (pPlot->isAdjacentToArea(iCurrArea))
			{
				return true;
			}

			iLastArea = iCurrArea;
		}
	}

	return false;
}


bool CvPlot::isAdjacentToLand() const
{
	PROFILE_FUNC();

	return algo::any_of(adjacent(), !CvPlot::fn::isWater());
}


bool CvPlot::isCoastalLand(int iMinWaterSize) const
{
	PROFILE_FUNC();

	if (isWater())
	{
		return false;
	}

	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		if (pAdjacentPlot->isWater() && pAdjacentPlot->area()->getNumTiles() >= iMinWaterSize)
		{
			return true;
		}
	}
	return false;
}

// Lakeshore tiles can qualify as coastal IF corresponding lake is greater in size than iMinWaterSize
bool CvPlot::isCoastal(int iMinWaterSize) const
{
	PROFILE_FUNC();

	if (isWater())
	{
		if (area()->getNumTiles() < iMinWaterSize)
		{
			return false;
		}
		return isAdjacentToLand();
	}
	// Coastal Land
	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		if (pAdjacentPlot->isWater() && pAdjacentPlot->area()->getNumTiles() >= iMinWaterSize)
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::isVisibleWorked() const
{
	return isBeingWorked() && (getTeam() == GC.getGame().getActiveTeam() || GC.getGame().isDebugMode());
}


bool CvPlot::isWithinTeamCityRadius(TeamTypes eTeam, PlayerTypes eIgnorePlayer) const
{
	PROFILE_FUNC();

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam)
		&& (eIgnorePlayer == NO_PLAYER || (PlayerTypes)iI != eIgnorePlayer)
		&& isPlayerCityRadius((PlayerTypes)iI))
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::isLake() const
{
	const CvArea* pArea = area();
	return pArea ? pArea->isLake() : false;
}


bool CvPlot::isFreshWater() const
{
	if (GC.getTerrainInfo(getTerrainType()).isFreshWaterTerrain())
	{
		return true;
	}

	if (isWater())
	{
		return false;
	}

	if (isRiver())
	{
		return true;
	}

	if (getPlotCity() != NULL && getPlotCity()->hasFreshWater())
	{
		return true;
	}

	if (isImpassable(getTeam()))
	{
		return false;
	}

	if (getFeatureType() != NO_FEATURE && GC.getFeatureInfo(getFeatureType()).isAddsFreshWater())
	{
		return true;
	}

	if (algo::any_of(rect(1, 1), bind(&CvPlot::isWaterAndIsFresh, _1)))
	{
		return true;
	}

	return false;
}

bool CvPlot::isWaterAndIsFresh() const
{
	return isWater() && GC.getTerrainInfo(getTerrainType()).isFreshWaterTerrain();
}


bool CvPlot::isPotentialIrrigation() const
{
	if (isCity() || getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType()).isCarriesIrrigation())
	{
		if (getTeam() != NO_TEAM && GET_TEAM(getTeam()).isIrrigation())
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::canHavePotentialIrrigation() const
{
	PROFILE_EXTRA_FUNC();
	if (isCity())
	{
		return true;
	}
	for (int iI = 0; iI < GC.getNumImprovementInfos(); ++iI)
	{
		if (GC.getImprovementInfo((ImprovementTypes)iI).isCarriesIrrigation()
		&& canHaveImprovement(((ImprovementTypes)iI), NO_TEAM, true))
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::isIrrigationAvailable(bool bIgnoreSelf) const
{
	return (!bIgnoreSelf && isIrrigated())
		|| isFreshWater()
		|| algo::any_of(adjacent(), CvPlot::fn::isIrrigated());
}


bool CvPlot::isRiverMask() const
{
	CvPlot* pPlot;

	if (isNOfRiver())
	{
		return true;
	}

	if (isWOfRiver())
	{
		return true;
	}

	pPlot = plotDirection(getX(), getY(), DIRECTION_EAST);
	if ((pPlot != NULL) && pPlot->isNOfRiver())
	{
		return true;
	}

	pPlot = plotDirection(getX(), getY(), DIRECTION_SOUTH);
	if ((pPlot != NULL) && pPlot->isWOfRiver())
	{
		return true;
	}

	return false;
}


bool CvPlot::isRiverCrossingFlowClockwise(DirectionTypes eDirection) const
{
	CvPlot *pPlot;
	switch(eDirection)
	{
	case DIRECTION_NORTH:
		pPlot = plotDirection(getX(), getY(), DIRECTION_NORTH);
		if (pPlot != NULL)
		{
			return (pPlot->getRiverWEDirection() == CARDINALDIRECTION_EAST);
		}
		break;
	case DIRECTION_EAST:
		return (getRiverNSDirection() == CARDINALDIRECTION_SOUTH);
		break;
	case DIRECTION_SOUTH:
		return (getRiverWEDirection() == CARDINALDIRECTION_WEST);
		break;
	case DIRECTION_WEST:
		pPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
		if(pPlot != NULL)
		{
			return (pPlot->getRiverNSDirection() == CARDINALDIRECTION_NORTH);
		}
		break;
	default:
		FErrorMsg("error");
		break;
	}

	return false;
}


bool CvPlot::isRiverSide() const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPlot* pLoopPlot, cardinalDirectionAdjacent())
	{
		if (isRiverCrossing(directionXY(this, pLoopPlot)))
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::isRiver() const
{
	return (getRiverCrossingCount() > 0);
}


bool CvPlot::isRiverConnection(DirectionTypes eDirection) const
{
	if (eDirection == NO_DIRECTION)
	{
		return false;
	}

	switch (eDirection)
	{
	case DIRECTION_NORTH:
		return (isRiverCrossing(DIRECTION_EAST) || isRiverCrossing(DIRECTION_WEST));
		break;

	case DIRECTION_NORTHEAST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_EAST));
		break;

	case DIRECTION_EAST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_SOUTH));
		break;

	case DIRECTION_SOUTHEAST:
		return (isRiverCrossing(DIRECTION_SOUTH) || isRiverCrossing(DIRECTION_EAST));
		break;

	case DIRECTION_SOUTH:
		return (isRiverCrossing(DIRECTION_EAST) || isRiverCrossing(DIRECTION_WEST));
		break;

	case DIRECTION_SOUTHWEST:
		return (isRiverCrossing(DIRECTION_SOUTH) || isRiverCrossing(DIRECTION_WEST));
		break;

	case DIRECTION_WEST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_SOUTH));
		break;

	case DIRECTION_NORTHWEST:
		return (isRiverCrossing(DIRECTION_NORTH) || isRiverCrossing(DIRECTION_WEST));
		break;

	default:
		FErrorMsg("error");
		break;
	}

	return false;
}


int CvPlot::isLandWater(const bool bLand) const
{
	if (this)
	{
		if (isWater())
		{
			if (!bLand)
			{
				return true;
			}
		}
		else if (bLand)
		{
			return true;
		}
	}
	return false;
}

// Toffer - Needed this for some ocean/sea/coast automatic terrain assignment when plot typåe change between ocean and land.
int CvPlot::getDistanceToLandOrCoast(const int iMaxReturn) const
{
	const bool bToLand = isWater();
	const int x0 = getX();
	const int y0 = getY();
	int iMinDx, iMinDy, iMaxDx, iMaxDy;

	if (GC.getMap().isWrapX())
	{
		iMaxDx = GC.getMap().getGridWidth() / 2;
		iMinDx = -iMaxDx / 2 + 1;
	}
	else
	{
		iMinDx = -x0;
		iMaxDx = GC.getMap().getGridWidth() - 1 - x0;
	}
	if (GC.getMap().isWrapY())
	{
		iMaxDy = GC.getMap().getGridHeight() / 2;
		iMinDy = -iMaxDy / 2 + 1;
	}
	else
	{
		iMinDy = -y0;
		iMaxDy = GC.getMap().getGridHeight() - 1 - y0;
	}
	int iRange = 1;
	bool bSearch = true;

	while (bSearch)
	{
		if (iRange == iMaxReturn)
		{
			return iMaxReturn;
		}
		bSearch = false;

		bool bTopCornersChecked = false;
		bool bBotCornersChecked = false;
		// constant dy, horisontal search.
		if (iRange <= iMaxDy && iMaxDy != y0 || iRange <= -iMinDy && iMinDy != y0)
		{
			bSearch = true;
			int xStart = iRange >= -iMinDx ? iMinDx : -iRange;
			int xEnd = iRange >= iMaxDx ? iMaxDx : iRange;

			if (iRange <= iMaxDy && iMaxDy != y0)
			{
				bTopCornersChecked = true;
				for (int dx = xStart; dx <= xEnd; dx++)
				{
					if (plotXY(x0, y0, dx, iRange)->isLandWater(bToLand))
					{
						return iRange;
					}
				}
			}
			if (iRange <= -iMinDy && iMinDy != y0)
			{
				bBotCornersChecked = true;
				for (int dx = xStart; dx <= xEnd; dx++)
				{
					if (plotXY(x0, y0, dx, -iRange)->isLandWater(bToLand))
					{
						return iRange;
					}
				}
			}
		}
		// constant dx, vertical search.
		if (iRange <= iMaxDx && iMaxDx != x0 || iRange <= -iMinDx && iMinDx != x0)
		{
			bSearch = true;
			int yStart = (iRange >= -iMinDy ? iMinDy : -iRange) + bBotCornersChecked;
			int yEnd = (iRange >= iMaxDy ? iMaxDy : iRange) - bTopCornersChecked;

			if (iRange <= iMaxDx && iMaxDx != x0)
			{
				for (int dy = yStart; dy <= yEnd; dy++)
				{
					if (plotXY(x0, y0, iRange, dy)->isLandWater(bToLand))
					{
						return iRange;
					}
				}
			}
			if (iRange <= -iMinDx && iMinDx != x0)
			{
				for (int dy = yStart; dy <= yEnd; dy++)
				{
					if (plotXY(x0, y0, -iRange, dy)->isLandWater(bToLand))
					{
						return iRange;
					}
				}
			}
		}
		iRange++;
	}
	return 0;
}

int CvPlot::setClimateAppropriateWaterTerrain(const int iDistance, ClimateZoneTypes eClimate)
{
	if (eClimate == NO_CLIMATE_ZONE)
	{
		if (getTerrainType() != NO_TERRAIN)
		{
			eClimate = GC.getTerrainInfo(getTerrainType()).getClimate();
		}
		if (eClimate == NO_CLIMATE_ZONE)
		{
			eClimate = GC.getMap().getClimateZone(getY());
		}
	}
	// @SAVEBREAK - remove (when scenarios handle climate zones)
	// Only world mapscript has been set up to define climate zone for map, and scenarios can also not define climate zones.
	if (eClimate != NO_CLIMATE_ZONE && GC.getMap().getClimateZone(getY()) == NO_CLIMATE_ZONE)
	{
		GC.getMap().setClimateZone(getY(), eClimate);
	}
	// !SAVEBREAK

	// Coast
	if (iDistance == 1)
	{
		if (eClimate == CLIMATE_ZONE_POLAR)
		{
			setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_COAST_POLAR"));
		}
		else if (eClimate == CLIMATE_ZONE_TROPICAL)
		{
			setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_COAST_TROPICAL"));
		}
		else if (eClimate == CLIMATE_ZONE_TEMPERATE || getTerrainType() == NO_TERRAIN || GC.getTerrainInfo(getTerrainType()).getDistanceToLand() != iDistance)
		{
			setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_COAST"));
		}
		return 1;
	}

	// Sea
	if (iDistance == 2)
	{
		if (eClimate == CLIMATE_ZONE_POLAR)
		{
			setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_SEA_POLAR"));
		}
		else if (eClimate == CLIMATE_ZONE_TROPICAL)
		{
			setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_SEA_TROPICAL"));
		}
		else if (eClimate == CLIMATE_ZONE_TEMPERATE || getTerrainType() == NO_TERRAIN || GC.getTerrainInfo(getTerrainType()).getDistanceToLand() != iDistance)
		{
			setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_SEA"));
		}
		return 2;
	}

	// Ocean
	if (eClimate == CLIMATE_ZONE_POLAR)
	{
		setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_OCEAN_POLAR"));
	}
	else if (eClimate == CLIMATE_ZONE_TROPICAL)
	{
		setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_OCEAN_TROPICAL"));
	}
	else if (eClimate == CLIMATE_ZONE_TEMPERATE || getTerrainType() == NO_TERRAIN || GC.getTerrainInfo(getTerrainType()).getDistanceToLand() != iDistance)
	{
		setTerrainType((TerrainTypes)GC.getDefineINT("WATER_TERRAIN_OCEAN"));
	}
	return 3;
}

bool CvPlot::correctWaterTerrain(int &iLastDistance)
{
	if (this && isWater())
	{
		if (isAdjacentToLand())
		{
			if (GC.getTerrainInfo(getTerrainType()).getDistanceToLand() != 1)
			{
				iLastDistance = setClimateAppropriateWaterTerrain(1);
			}
		}
		else if (iLastDistance == 1 || getDistanceToLandOrCoast(3) == 2)
		{
			if (GC.getTerrainInfo(getTerrainType()).getDistanceToLand() != 2)
			{
				iLastDistance = setClimateAppropriateWaterTerrain(2);
			}
		}
		else if (GC.getTerrainInfo(getTerrainType()).getDistanceToLand() != 3)
		{
				iLastDistance = setClimateAppropriateWaterTerrain(3);
		}
		return true;
	}
	return false;
}

// Toffer - This could be generalized to look for anything (and do anything with it) specified with a function passed as a parameter,
//	the function parameter would replace the "correctWaterTerrain" function used below,
//	and the bContinue bool could be changed to an count int that decrement if one wants to do something further out.
void CvPlot::correctWaterTerrains(int iLastDistance, const DirectionTypes dir, const bool bContinue)
{
	switch (dir)
	{
		case DIRECTION_NORTH:
		case DIRECTION_EAST:
		case DIRECTION_SOUTH:
		case DIRECTION_WEST:
		{
			CvPlot* plotY = plotDirection(getX(), getY(), dir);
			if (plotY->correctWaterTerrain(iLastDistance) && bContinue)
			{
				plotY->correctWaterTerrains(iLastDistance, dir);
			}
			break;
		}
		case DIRECTION_NORTHEAST:
		{
			bool bDiagonal = true;

			CvPlot* plotY = plotDirection(getX(), getY(), DIRECTION_NORTH);
			if (plotY->correctWaterTerrain(iLastDistance))
			{
				if (bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_NORTH);
				}
			}
			else bDiagonal = false;

			plotY = plotDirection(getX(), getY(), DIRECTION_EAST);
			if (plotY->correctWaterTerrain(iLastDistance))
			{
				if (bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_EAST);
				}
			}
			else bDiagonal = false;

			if (bDiagonal)
			{
				plotY = plotDirection(getX(), getY(), DIRECTION_NORTHEAST);
				if (plotY->correctWaterTerrain(iLastDistance) && bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_NORTHEAST);
				}
			}
			break;
		}
		case DIRECTION_SOUTHEAST:
		{
			bool bDiagonal = true;

			CvPlot* plotY = plotDirection(getX(), getY(), DIRECTION_SOUTH);
			if (plotY->correctWaterTerrain(iLastDistance))
			{
				if (bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_SOUTH);
				}
			}
			else bDiagonal = false;

			plotY = plotDirection(getX(), getY(), DIRECTION_EAST);
			if (plotY->correctWaterTerrain(iLastDistance))
			{
				if (bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_EAST);
				}
			}
			else bDiagonal = false;

			if (bDiagonal)
			{
				plotY = plotDirection(getX(), getY(), DIRECTION_SOUTHEAST);
				if (plotY->correctWaterTerrain(iLastDistance) && bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_SOUTHEAST);
				}
			}
			break;
		}
		case DIRECTION_SOUTHWEST:
		{
			bool bDiagonal = true;

			CvPlot* plotY = plotDirection(getX(), getY(), DIRECTION_SOUTH);
			if (plotY->correctWaterTerrain(iLastDistance))
			{
				if (bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_SOUTH);
				}
			}
			else bDiagonal = false;

			plotY = plotDirection(getX(), getY(), DIRECTION_WEST);
			if (plotY->correctWaterTerrain(iLastDistance))
			{
				if (bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_WEST);
				}
			}
			else bDiagonal = false;

			if (bDiagonal)
			{
				plotY = plotDirection(getX(), getY(), DIRECTION_SOUTHWEST);
				if (plotY->correctWaterTerrain(iLastDistance) && bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_SOUTHWEST);
				}
			}
			break;
		}
		case DIRECTION_NORTHWEST:
		{
			bool bDiagonal = true;

			CvPlot* plotY = plotDirection(getX(), getY(), DIRECTION_NORTH);
			if (plotY->correctWaterTerrain(iLastDistance))
			{
				if (bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_NORTH);
				}
			}
			else bDiagonal = false;

			plotY = plotDirection(getX(), getY(), DIRECTION_WEST);
			if (plotY->correctWaterTerrain(iLastDistance))
			{
				if (bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_WEST);
				}
			}
			else bDiagonal = false;

			if (bDiagonal)
			{
				plotY = plotDirection(getX(), getY(), DIRECTION_NORTHWEST);
				if (plotY->correctWaterTerrain(iLastDistance) && bContinue)
				{
					plotY->correctWaterTerrains(iLastDistance, DIRECTION_NORTHWEST);
				}
			}
		}
	}
}

int CvPlot::seeFromLevel(TeamTypes eTeam) const
{
	int iLevel = 0;

	if (getImprovementType() != NO_IMPROVEMENT)
	{
		iLevel += GC.getImprovementInfo(getImprovementType()).getSeeFrom();
	}

	if (!isWater())
	{
		iLevel += 1 + getTerrainElevation();
	}
	else if (GET_TEAM(eTeam).isExtraWaterSeeFrom())
	{
		iLevel++;
	}
	return iLevel;
}

int CvPlot::seeThroughLevel() const
{
	int iLevel = isWater() ? 0 : 1 + getTerrainElevation();

	if (getFeatureType() != NO_FEATURE)
	{
		iLevel += GC.getFeatureInfo(getFeatureType()).getSeeThroughChange();
	}
	return iLevel;
}

// Toffer - Quite basic setup:
//	Water/Flatland < Hill < Peak.
int CvPlot::getElevationLevel(const bool bExtra) const
{
	int iLevel = 3 * getTerrainElevation();

	if (bExtra && getImprovementType() != NO_IMPROVEMENT)
	{
		iLevel += GC.getImprovementInfo(getImprovementType()).getSeeFrom();
	}
	return iLevel;
}

int CvPlot::getTerrainElevation() const
{
	if (isAsPeak())
	{
		return 2;
	}
	if (isHills())
	{
		return 1;
	}
	return 0;
}


void CvPlot::changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, CvUnit* pUnit, bool bUpdatePlotGroups)
{
	PROFILE_EXTRA_FUNC();
	const bool bHideSeek = GC.getGame().isOption(GAMEOPTION_COMBAT_HIDE_SEEK);

	int iUnitID = 0;
	//fill invisible types
	std::vector<InvisibleTypes> aSeeInvisibleTypes;

	if (NULL != pUnit)
	{
		iUnitID = pUnit->getID();
		if (bHideSeek)
		{
			for (int iI = GC.getNumInvisibleInfos() - 1; iI > -1; iI--)
			{
				aSeeInvisibleTypes.push_back((InvisibleTypes)iI);
			}
		}
		else
		{
			for (int i=0; i < pUnit->getNumSeeInvisibleTypes(); i++)
			{
				aSeeInvisibleTypes.push_back(pUnit->getSeeInvisibleType(i));
			}
		}
	}
	aSeeInvisibleTypes.push_back(NO_INVISIBLE);

	const bool bAerial = pUnit && pUnit->getDomainType() == DOMAIN_AIR;

	foreach_(const InvisibleTypes eInvisible, aSeeInvisibleTypes)
	{
		for (int dx = -iRange; dx <= iRange; dx++)
		{
			for (int dy = -iRange; dy <= iRange; dy++)
			{
				CvPlot* pPlot = plotXY(getX(), getY(), dx, dy);

				if (NULL != pPlot && (bAerial || canSeePlot(pPlot, eTeam)))
				{
					int iFinalIntensity = 0;

					if (bIncrement && bHideSeek && eInvisible != NO_INVISIBLE && pUnit != NULL)
					{
						const int iDistance = std::max(abs(dx), abs(dy));

						if (iDistance > 0)
						{
							const int iSpotRange = pUnit->visibilityIntensityRangeTotal(eInvisible);

							iFinalIntensity = pUnit->visibilityIntensityTotal(eInvisible) - std::max(0, iDistance - iSpotRange);
						}
						else iFinalIntensity = pUnit->visibilityIntensityTotal(eInvisible) + pUnit->visibilityIntensitySameTileTotal(eInvisible);
					}
					pPlot->changeVisibilityCount(eTeam, (bIncrement ? 1 : -1), eInvisible, bUpdatePlotGroups, iFinalIntensity, iUnitID);
				}
			}
		}
	}
}

bool CvPlot::canSeePlot(const CvPlot* pPlot, TeamTypes eTeam) const
{
	if (pPlot == NULL)
	{
		return false;
	}
	//find displacement
	const int dx = dxWrap(pPlot->getX() - getX());
	const int dy = dyWrap(pPlot->getY() - getY());
	int iDummy1 = 0;
	int iDummy2 = 0;
	return canSeeDisplacementPlot(eTeam, dx, dy, dx, dy, iDummy1, iDummy2, true);
}

bool CvPlot::canSeeDisplacementPlot(TeamTypes eTeam, int dx, int dy, int dx0, int dy0, int& iTopElevation, int& iTopElevationDistance, bool bEndPoint) const
{
	// Base case is current plot
	if (dx == 0 && dy == 0)
	{
		return true;
	}
	const CvPlot* seePlot = plotXY(getX(), getY(), dx, dy);

	if (seePlot == NULL)
	{
		return false;
	}
	bool bCanFail = true;
	if (!bEndPoint)
	{
		iTopElevation = seePlot->getElevationLevel();
		iTopElevationDistance = std::max(abs(dx), abs(dy));

		if (2*iTopElevationDistance >= std::max(abs(dx0), abs(dy0))
		&& plotXY(getX(), getY(), dx0, dy0)->getElevationLevel() > iTopElevation)
		{
			bCanFail = false; // This is a guess for now
		}
	}
	int step1[] = { 0, 0 };
	int step2[] = { dx, dy };
	int iSteps = 0;

	if (abs(dx) > 1 || abs(dy) > 1) // seePlot is not adjacent to "this" plot.
	{
		iSteps++;
		if (dx == 0) // Straight vertical line to "this" plot.
		{
			step1[1] = dy - getSign(dy);
		}
		else if (dy == 0) // Straight horizontal line to "this" plot.
		{
			step1[0] = dx - getSign(dx);
		}
		else // Diagonal is valid for sure
		{
			step1[0] = dx - getSign(dx);
			step1[1] = dy - getSign(dy);

			if (abs(dx) != abs(dy)) // Diagonal to "this" plot is not grid aligned.
			{
				iSteps++;
				// One of the two straight directions is also valid
				if (abs(dx) > abs(dy))
				{
					step2[0] = dx - getSign(dx);
				}
				else
				{
					step2[1] = dy - getSign(dy);
				}
			}
		}
		int iTopElevation1 = 0;
		int iTopElevationDistance1 = 0;
		const bool bFailedStep1 = !canSeeDisplacementPlot(eTeam, step1[0], step1[1], dx0, dy0, iTopElevation1, iTopElevationDistance1);

		bool bFailed = false;
		bool bTestCanFail = false;
		if (iSteps == 2)
		{
			if (bFailedStep1 && 2*abs(step1[0]) < abs(dx0) && 2*abs(step1[1]) < abs(dy0))
			{
				if (bCanFail)
				{
					return false;
				}
				bFailed = true;
			}
			int iTopElevation2 = 0;
			int iTopElevationDistance2 = 0;

			if (!canSeeDisplacementPlot(eTeam, step2[0], step2[1], dx0, dy0, iTopElevation2, iTopElevationDistance2))
			{
				if (bFailedStep1)
				{
					if (bCanFail)
					{
						return false;
					}
					bFailed = true;
				}
				if (!bFailed && 2*abs(step2[0]) > abs(dx0) && 2*abs(step2[1]) > abs(dy0))
				{
					if (bCanFail)
					{
						return false;
					}
					bFailed = true;
				}
			}
			if (iTopElevation1 < iTopElevation2)
			{
				if (iTopElevation1 > iTopElevation)
				{
					iTopElevation = iTopElevation1;
					iTopElevationDistance = iTopElevationDistance1;
					bTestCanFail = true;
				}
			}
			else if (iTopElevation2 > iTopElevation)
			{
				iTopElevation = iTopElevation2;
				iTopElevationDistance = iTopElevationDistance2;
				bTestCanFail = true;
			}
		}
		else if (bFailedStep1)
		{
			if (bCanFail)
			{
				return false;
			}
			bFailed = true;
		}
		else if (iTopElevation1 > iTopElevation)
		{
			iTopElevation = iTopElevation1;
			iTopElevationDistance = iTopElevationDistance1;
			bTestCanFail = true;
		}

		if (bTestCanFail)
		{
			if (plotXY(getX(), getY(), dx0, dy0)->getElevationLevel() > iTopElevation && 2*iTopElevationDistance >= std::max(abs(dx0), abs(dy0)))
			{
				return true;
			}
		}
		else if (bFailed)
		{
			return false;
		}
		else if (!bCanFail)
		{
			return true;
		}
	}

	if (bEndPoint)
	{
		const int iMyElevation = getElevationLevel(true);
		if (iMyElevation < iTopElevation)
		{
			return false;
		}
		if (iMyElevation == iTopElevation)
		{
			if (seePlot->getElevationLevel() < iTopElevation)
			{
				return false;
			}
		}
		else if (seePlot->getElevationLevel() < iTopElevation && 2*iTopElevationDistance >= std::max(dx0, dy0))
		{
			return false;
		}
	}
	else if (seeFromLevel(eTeam) < seePlot->seeThroughLevel())
	{
		return false;
	}
	return true;
}

void CvPlot::updateSight(bool bIncrement, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	// City
	{
		const CvCity* pCity = getPlotCity();

		if (pCity)
		{
			const CvTeam& team = GET_TEAM(pCity->getTeam());

			for (int iI = 0; iI < MAX_PC_TEAMS; ++iI)
			{
				// Vassal
				if (GET_TEAM(getTeam()).isVassal((TeamTypes)iI)
				// Espionage - Enough EPs gives you visibility into someone's cities
				|| pCity->getEspionageVisibility((TeamTypes)iI)
				// Embassy Allows Players to See Capitals
				|| pCity->isCapital() && team.isHasEmbassy((TeamTypes)iI))
				{
					changeAdjacentSight((TeamTypes)iI, 1, bIncrement, NULL, bUpdatePlotGroups);
				}
			}
		}
	}

	// Owned
	if (isOwned())
	{
		changeAdjacentSight(getTeam(), 1, bIncrement, NULL, bUpdatePlotGroups);
	}

	// Unit
	{
		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL)
			{
				if (bIncrement)
					pLoopUnit->changeDebugCount(1);
				else pLoopUnit->changeDebugCount(-1);

				changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->visibilityRange(), bIncrement, pLoopUnit, bUpdatePlotGroups);
			}
			else
			{
				verifyUnitValidPlot();
				pUnitNode = NULL;
			}
		}
	}

	// Recon
	if (getReconCount() > 0)
	{
		const int iRange = GC.getDefineINT("RECON_VISIBILITY_RANGE");
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			foreach_(CvUnit* pLoopUnit, GET_PLAYER((PlayerTypes)iI).units())
			{
				if (pLoopUnit->getReconPlot() == this)
				{
					if (bIncrement)
						pLoopUnit->changeDebugCount(1);
					else pLoopUnit->changeDebugCount(-1);

					changeAdjacentSight(pLoopUnit->getTeam(), iRange, bIncrement, pLoopUnit, bUpdatePlotGroups);
				}
			}
		}
	}
}


void CvPlot::updateSeeFromSight(bool bIncrement, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	const int iRange = GC.getMAX_UNIT_VISIBILITY_RANGE() + 1;

	algo::for_each(rect(iRange, iRange), bind(&CvPlot::updateSight, _1, bIncrement, bUpdatePlotGroups));
}


bool CvPlot::canHaveBonus(BonusTypes eBonus, bool bIgnoreLatitude) const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (eBonus == NO_BONUS)
	{
		return true;
	}
	if (getBonusType() != NO_BONUS || !isPotentialCityWork())
	{
		return false;
	}

	const FeatureTypes eFeature = getFeatureType();

	if (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature).isNoBonus())
	{
		return false;
	}
	const CvBonusInfo& bonus = GC.getBonusInfo(eBonus);

	if (!bonus.isTerrain(getTerrainType())
	&& (eFeature == NO_FEATURE || !bonus.isFeature(eFeature) || !bonus.isFeatureTerrain(getTerrainType())))
	{
		return false;
	}

	if (isHills())
	{
		if (!bonus.isHills())
		{
			return false;
		}
	}
	else if (isAsPeak())
	{
		if (!bonus.isPeaks())
		{
			return false;
		}
	}
	else if (isFlatlands())
	{
		if (!bonus.isFlatlands())
		{
			return false;
		}
	}

	if (bonus.isBonusCoastalOnly() && !isCoastal())
	{
		return false;
	}

	if (bonus.isNoRiverSide() && isRiverSide())
	{
		return false;
	}

	if (bonus.getMinAreaSize() > 1
	&& area()->getNumTiles() < bonus.getMinAreaSize())
	{
		return false;
	}

	if (!bIgnoreLatitude)
	{
		if (getLatitude() > bonus.getMaxLatitude())
		{
			return false;
		}

		if (getLatitude() < bonus.getMinLatitude())
		{
			return false;
		}
	}

	if (!isMapCategory(*this, bonus))
	{
		return false;
	}
	return true;
}


bool CvPlot::canBuildImprovement(ImprovementTypes eImprovement, TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const BuildTypes& eBuild, GC.getImprovementInfo(eImprovement).getBuildTypes())
	{
		if (GET_TEAM(eTeam).isHasTech(GC.getBuildInfo(eBuild).getTechPrereq()))
		{
			return true;
		}
	}
	return false;
}


// Needs to be improved a bit. What if no builds account for the ability to generate the improvement?
// Hmm... maybe a whole nother check that along with this one gets cached significantly?
bool CvPlot::canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam, bool bPotential, bool bUpgradeCheck) const
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	// Universal validator
	if (eImprovement == NO_IMPROVEMENT)
	{
		return true;
	}
	/*----------------------*\
	| Universal Invalidators |
	\*----------------------*/
	if (isCity())
	{
		return false;
	}
	const CvImprovementInfo& pInfo = GC.getImprovementInfo(eImprovement);
	const FeatureTypes eFeature = getFeatureType();

	// Feature forbids
	if (eFeature != NO_FEATURE && GC.getFeatureInfo(eFeature).isNoImprovement() && !pInfo.getFeatureMakesValid(eFeature))
	{
		return false;
	}

	if (!bPotential && eTeam != NO_TEAM && !GET_TEAM(eTeam).isHasTech(pInfo.getPrereqTech()))
	{
		return false;
	}
	// Meant for pseudo improvements that won't ever be on the map, like bonus placement improvements.
	// Toffer - Shouldn't this be part of buildInfo?
	//	I'm thinking bonus placement builds should not be linked to an improvement at all.
	if (pInfo.isNotOnAnyBonus() && getBonusType() != NO_BONUS)
	{
		return false;
	}
	const BonusTypes eBonus = getBonusType(eTeam);

	// Special upgrade rule
	if (bUpgradeCheck && getImprovementType() != NO_IMPROVEMENT && eBonus != NO_BONUS
	&& GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(eBonus)
	&& !pInfo.isImprovementBonusTrade(eBonus))
	{
		// Upgrading would remove access to bonus, bad upgrade.
		return false;
	}

	if (!isMapCategory(*this, pInfo))
	{
		return false;
	}

	/*-----------------------*\
	| Special plot type rules |
	\*-----------------------*/
	bool bValid = false;
	if (isWater())
	{
		if (!pInfo.isWaterImprovement())
		{
			return false;
		}
	}
	else if (pInfo.isWaterImprovement())
	{
		return false;
	}
	else if (isAsPeak())
	{
		if (!pInfo.isPeakImprovement()) // Only requirement that invalidate both ways.
		{
			return false;
		}
		if (pInfo.isPeakMakesValid())
		{
			bValid = true;
		}
	}
	else if (pInfo.isPeakImprovement())
	{
		return false;
	}

	/*---------------------------------------------*\
	| Special rule for bonuses; bonus access is key |
	\*---------------------------------------------*/
	if (eBonus != NO_BONUS)
	{
		// Normal make-valid handling.
		if (pInfo.isImprovementBonusMakesValid(eBonus))
		{
			if (pInfo.isImprovementBonusTrade(eBonus))
			{
				return true;
			}
			bValid = true;
		}
	}
	/*--------------------*\
	| General invalidators |
	\*--------------------*/
	// Irrigation req
	if (!bPotential && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isIgnoreIrrigation())
	&& pInfo.isRequiresIrrigation() && !isIrrigationAvailable())
	{
		return false;
	}

	if(pInfo.isNoFreshWater() && isFreshWater()
	|| pInfo.isRequiresFlatlands() && !isFlatlands()
	|| pInfo.isRequiresFeature() && eFeature == NO_FEATURE
	// Special canal condition
	|| pInfo.isCanMoveSeaUnits() && !isCoastalLand())
	{
		return false;
	}
	/*---------------*\
	| Main validators |
	\*---------------*/
	// Special desert validator
	bool bIgnoreNatureYields = false;
	if (pInfo.isRequiresIrrigation() && getTerrainType() == GC.getTERRAIN_DESERT()
	&& (eTeam != NO_TEAM && GET_TEAM(eTeam).isCanFarmDesert() || getTeam() != NO_TEAM && GET_TEAM(getTeam()).isCanFarmDesert()))
	{
		bValid = true;
		bIgnoreNatureYields = true; // This is a min yield override for desert
	}
	else if (!bValid)
	{
		// General validators
		if(pInfo.getTerrainMakesValid(getTerrainType())
		|| pInfo.isHillsMakesValid() && isHills()
		|| pInfo.isFreshWaterMakesValid() && isFreshWater()
		|| pInfo.isRiverSideMakesValid() && isRiverSide())
		{
			bValid = true;
		}
		else if (eFeature != NO_FEATURE && pInfo.getFeatureMakesValid(eFeature))
		{
			bValid = true;
		}
		else if (eBonus != NO_BONUS && pInfo.isImprovementObsoleteBonusMakesValid(eBonus)
		&& GET_TEAM(eTeam).isHasTech((TechTypes)GC.getBonusInfo(eBonus).getTechObsolete()))
		{
			bValid = true;
		}
	}
	/*------------------*\
	| Final invalidators |
	\*------------------*/
	if (!bValid)
	{
		return false;
	}
	// Minimum yield
	if (!bIgnoreNatureYields)
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (calculateNatureYield(((YieldTypes)iI), eTeam) < pInfo.getPrereqNatureYield(iI))
			{
				return false;
			}
		}
	}
	// Riverside
	if (pInfo.isRequiresRiverSide())
	{
		bValid = false;
		foreach_(const CvPlot* pLoopPlot, cardinalDirectionAdjacent())
		{
			if (isRiverCrossing(directionXY(this, pLoopPlot)))
			{
				// Toffer - Only allowed on one side of the river without additional spacing.
				// An odd rule, but I'm sure it's what the original code intended before I changed it to be consistent.
				// Original code would break the loop as valid after finding first river crossing with a different improvement on the other side.
				// That would almost always be true unless the river had eImprovement on all other plots with a river crossing from "this" plot.
				// Now it checks all river crossings from this plot for the same improvement and only validates if this improvement is not found.
				if (pLoopPlot->getImprovementType() == eImprovement)
				{
					return false;
				}
				bValid = true;
			}
		}
		if (!bValid)
		{
			return false;
		}
	}
	return true;
}


#ifdef CAN_BUILD_VALUE_CACHING

canBuildCache CvPlot::g_canBuildCache;
int CvPlot::canBuildCacheHits = 0;
int CvPlot::canBuildCacheReads = 0;

void CvPlot::ClearCanBuildCache()
{
	PROFILE_EXTRA_FUNC();
	if ( g_canBuildCache.currentUseCounter > 0 )
	{
		OutputDebugString(CvString::format("Clear can build cache  - usage: %d reads with %d hits\n",canBuildCacheReads,canBuildCacheHits).c_str());

		g_canBuildCache.currentUseCounter = 0;

		for(int i = 0; i < CAN_BUILD_CACHE_SIZE; i++)
		{
			g_canBuildCache.entries[i].iLastUseCount = 0;
		}
	}
}
#endif

bool CvPlot::hasCachedCanBuildEntry(int iX, int iY, BuildTypes eBuild, PlayerTypes ePlayer, struct canBuildCacheEntry*& entry)
{
	PROFILE_EXTRA_FUNC();
	//	Check cache first
	int worstLRU = 0x7FFFFFFF;

	struct canBuildCacheEntry* worstLRUEntry = NULL;
	canBuildCacheReads++;

	//OutputDebugString(CvString::format("AI_yieldValue (%d,%d,%d) at seq %d\n", piYields[0], piYields[1], piYields[2], yieldValueCacheReads).c_str());
	//PROFILE_STACK_DUMP

	for(int i = 0; i < CAN_BUILD_CACHE_SIZE; i++)
	{
		entry = &g_canBuildCache.entries[i];
		if ( entry->iLastUseCount == 0 )
		{
			worstLRUEntry = entry;
			break;
		}

		if ( entry->iPlotX == iX &&
			 entry->iPlotY == iY &&
			 entry->eBuild == eBuild &&
			 entry->ePlayer == ePlayer )
		{
			entry->iLastUseCount = ++g_canBuildCache.currentUseCounter;
			canBuildCacheHits++;
#ifdef VERIFY_CAN_BUILD_CACHE_RESULTS
			long lRealValue = canBuildFromPythonInternal(eBuild, ePlayer);

			if ( lRealValue != entry->lResult )
			{
				OutputDebugString(CvString::format("Cache entry %08lx verification failed, turn is %d\n", entry, GC.getGame().getGameTurn()).c_str());
				FErrorMsg("Can build value cache verification failure");
			}
#endif
			return true;
		}
		else if ( entry->iLastUseCount < worstLRU )
		{
			worstLRU = entry->iLastUseCount;
			worstLRUEntry = entry;
		}
	}

	entry = worstLRUEntry;

	return false;
}

long CvPlot::canBuildFromPython(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	PROFILE_FUNC();

#ifdef CAN_BUILD_VALUE_CACHING
#ifdef _DEBUG
//	Uncomment this to perform functional verification
//#define VERIFY_CAN_BUILD_CACHE_RESULTS
#endif

	//	If this player does not own, and cannot build a unit that can perform this build
	//	no need to check with the Python.
	if ( !GET_PLAYER(ePlayer).canHaveBuilder(eBuild) )
	{
		return 0L;
	}

	struct canBuildCacheEntry* entry;

	if ( hasCachedCanBuildEntry(getX(), getY(), eBuild, ePlayer, entry) )
	{
		return entry->lResult;
	}
	else
	{
		int lResult = canBuildFromPythonInternal(eBuild, ePlayer);

		FAssertMsg(entry != NULL, "No can build cache entry found to replace");
		if ( entry != NULL )
		{
			entry->iPlotX = getX();
			entry->iPlotY = getY();
			entry->eBuild = eBuild;
			entry->ePlayer = ePlayer;
			entry->lResult = lResult;
			entry->iLastUseCount = ++g_canBuildCache.currentUseCounter;
		}

		return lResult;
	}
#else
	return canBuildFromPythonInternal(eBuild, ePlayer);
#endif
}

long CvPlot::canBuildFromPythonInternal(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	PROFILE_FUNC();

	return Cy::call<long>(PYGameModule, "canBuild", Cy::Args()
		<< getX()
		<< getY()
		<< eBuild
		<< ePlayer
	);
}

bool CvPlot::canBuild(BuildTypes eBuild, PlayerTypes ePlayer, bool bTestVisible, bool bIncludePythonOverrides) const
{
	PROFILE_FUNC();

	if (eBuild == NO_BUILD)
	{
		return false;
	}
	const CvBuildInfo& info = GC.getBuildInfo(eBuild);
	if (info.getTime() < 0) // Build disabled in xml
	{
		// Less than 0 is nonsensical, and a value of -1 would cause divide by zero when calculating build value.
		return false;
	}
	// Tech requirements are not checked here - they are checked in CvPlayer::canBuild()

	if (bIncludePythonOverrides)
	{
		const int lResult = canBuildFromPython(eBuild, ePlayer);

		if (lResult > -1)
		{
			return lResult > 0;
		}
	}

	if (!info.getPlaceBonusTypes().empty())
	{
		if (getBonusType() != NO_BONUS)
		{
			return false;
		}

		bool bInvalid = true;
		foreach_(const PlaceBonusTypes& obj, info.getPlaceBonusTypes())
		{
			bInvalid =
			(
				obj.bRequiresAccess && !GET_PLAYER(getOwner()).hasBonus(obj.eBonus)
			||
				obj.ePrereqMapCategory != NO_MAPCATEGORY && !isMapCategoryType(obj.ePrereqMapCategory)
			||
				obj.ePrereqTech != NO_TECH && !GET_TEAM(getTeam()).isHasTech(obj.ePrereqTech)
			||
				obj.ePrereqTerrain != NO_TERRAIN && getTerrainType() != obj.ePrereqTerrain
			||
				obj.ePrereqFeature != NO_FEATURE && getFeatureType() != obj.ePrereqFeature
			||
				!canHaveBonus(obj.eBonus)
			);

			if (!bInvalid)
			{
				break;
			}
		}
		if (bInvalid)
		{
			return false;
		}
	}

	foreach_(const BonusTypes ePrereqBonus, info.getPrereqBonuses())
	{
		if (!isAdjacentPlotGroupConnectedBonus(ePlayer, ePrereqBonus))
		{
			return false;
		}
	}

	if (!isMapCategory(*this, info))
	{
		return false;
	}

	const ImprovementTypes eImprovement = info.getImprovement();
	bool bValid = false;

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (!canHaveImprovement(eImprovement, GET_PLAYER(ePlayer).getTeam(), bTestVisible))
		{
			return false;
		}

		const CvImprovementInfo& pInfo = GC.getImprovementInfo(eImprovement);
		// Unique range between improvements within same improvement line (e.g. forts)
		const int iUniqueRange = pInfo.getUniqueRange();
		if (iUniqueRange > 0)
		{
			foreach_(const CvPlot* pLoopPlot, rect(iUniqueRange, iUniqueRange))
			{
				if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT
				&& finalImprovementUpgrade(pLoopPlot->getImprovementType()) == finalImprovementUpgrade(eImprovement))
				{
					return false;
				}
			}
		}

		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (getImprovementType() == eImprovement)
			{
				return false;
			}
		}

		if (!bTestVisible)
		{
			// Anywhere builds only fail in non-team/neutral territory
			if (pInfo.isOutsideBorders())
			{
				if (getTeam() != NO_TEAM &&
					getTeam() != GET_PLAYER(ePlayer).getTeam())
				{
					return false;
				}
			}
			// Fail when on territory owned by non-teammate
			// OR when outside influence of tile owner; this (should?) enable building on valid territory of teammates.
			// NOTE: This prevents workers removing forts when in territory influenced by teammate. Also does not allow
			// 		building on a tile owned by teammate but only influenced by you (rare occurrance though for it not to be a fort tile)
			else if (GET_PLAYER(ePlayer).getTeam() != getTeam()
				|| !isInCultureRangeOfCityByPlayer(getOwner()))
			{
				return false;
			}

			// Super Forts begin *AI_worker* - prevent workers from two different players from building a fort/tower in the same plot
			if(pInfo.isMilitaryStructure())
			{
				foreach_(const CvUnit* pLoopUnit, units())
				{
					if(pLoopUnit->getOwner() != ePlayer
					&& pLoopUnit->getBuildType() != NO_BUILD)
					{
						const ImprovementTypes eImprovementBuild = GC.getBuildInfo(pLoopUnit->getBuildType()).getImprovement();

						if (eImprovementBuild != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovementBuild).isMilitaryStructure())
						{
							return false;
						}
					}
				}
			}
			// Super Forts end
		}
		bValid = true;
	}

	RouteTypes eRoute = ((RouteTypes)(info.getRoute()));

	if (eRoute != NO_ROUTE)
	{
		if (getRouteType() != NO_ROUTE)
		{
			if (GC.getRouteInfo(getRouteType()).getValue() >= GC.getRouteInfo(eRoute).getValue())
			/* Version to allow same route value in future maybe:
			if (GC.getRouteInfo(getRouteType()).getValue() > GC.getRouteInfo(eRoute).getValue()
			||  eRoute == getRouteType()) */
			{
				return false;
			}
		}

		// Check to disallow based on mising route resource prereqs
		if (!bTestVisible)
		{
			if (GC.getRouteInfo(eRoute).getPrereqBonus() != NO_BONUS
			&& !isAdjacentPlotGroupConnectedBonus(ePlayer, (BonusTypes)GC.getRouteInfo(eRoute).getPrereqBonus()))
			{
				return false;
			}

			bool bFoundOrPrereq = true;
			foreach_(const BonusTypes ePrereqBonus, GC.getRouteInfo(eRoute).getPrereqOrBonuses())
			{
				if (isAdjacentPlotGroupConnectedBonus(ePlayer, ePrereqBonus))
				{
					bFoundOrPrereq = true;
					break;
				}
				else bFoundOrPrereq = false;
			}
			if (!bFoundOrPrereq)
			{
				return false;
			}
		}
		bValid = true;
	}

	if (getFeatureType() != NO_FEATURE && info.isFeatureRemove(getFeatureType()))
	{
		if (isOwned() && GET_PLAYER(ePlayer).getTeam() != getTeam() && !atWar(GET_PLAYER(ePlayer).getTeam(), getTeam()))
		{
			return false;
		}
		bValid = true;
	}

	const TerrainTypes eTerrain = (TerrainTypes)info.getTerrainChange();

	if (eTerrain != NO_TERRAIN)
	{
		if (getTerrainType() == eTerrain)
		{
			return false;
		}
		bValid = true;
	}

	const FeatureTypes eFeature = static_cast<FeatureTypes>(info.getFeatureChange());

	if (eFeature != NO_FEATURE)
	{
		if (getFeatureType() == eFeature || !isMapCategory(*this, GC.getFeatureInfo(eFeature)))
		{
			return false;
		}
		bValid = true;
	}

	return bValid;
}


int CvPlot::getBuildTime(BuildTypes eBuild) const
{
	PROFILE_EXTRA_FUNC();
	const CvBuildInfo& kBuild = GC.getBuildInfo(eBuild);
	int iTime = kBuild.getTime();

	if (getFeatureType() != NO_FEATURE)
	{
		iTime += kBuild.getFeatureTime(getFeatureType());
	}

	const TerrainTypes eTerrain = getTerrainType();
	foreach_(const TerrainStructs& kTerrainStruct, kBuild.getTerrainStructs())
	{
		if (kTerrainStruct.eTerrain == eTerrain)
		{
			iTime += kTerrainStruct.iTime;
			break;
		}
	}

	if (isAsPeak())
	{
		iTime *= std::max(0, (GC.getDefineINT("PEAK_BUILD_TIME_MODIFIER") + 100));
		iTime /= 100;
	}

	if (kBuild.getRoute() != NO_ROUTE)
	{
		const RouteTypes eCurrentRoute = getRouteType();
		if (eCurrentRoute != NO_ROUTE && GC.getDefineINT("ROUTES_UPGRADE") > 0)
		{
			for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
			{
				if (GC.getBuildInfo((BuildTypes)iI).getRoute() == eCurrentRoute)
				{
					iTime = std::max(1, iTime - GC.getBuildInfo((BuildTypes)iI).getTime());
					break;
				}
			}
		}
	}

	iTime *= std::max(0, GC.getTerrainInfo(eTerrain).getBuildModifier() + 100);
	iTime /= 100;

	iTime *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
	iTime /= 100;

	iTime *= GC.getEraInfo(GC.getGame().getStartEra()).getBuildPercent();
	iTime /= 100;

	return iTime;
}


int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	const int iWorkRate = getBuildTurnsLeft(eBuild, 0, 0);

	if (iWorkRate == 0)
	{
		return GET_PLAYER(ePlayer).getWorkRate(eBuild);
	}
	return iWorkRate;
}


int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, int iNowExtra, int iThenExtra, bool bIncludeUnits) const
{
	PROFILE_EXTRA_FUNC();
	int iNowBuildRate = iNowExtra;
	int iThenBuildRate = iThenExtra;

	if (bIncludeUnits)
	{
		foreach_(const CvUnit* pLoopUnit, units())
		{
			if (pLoopUnit->getBuildType() == eBuild)
			{
				if (pLoopUnit->canMove())
				{
					iNowBuildRate += pLoopUnit->workRate(false);
				}
				iThenBuildRate += pLoopUnit->workRate(true);
			}
		}
	}
	if (iThenBuildRate == 0)
	{
		// It will take forever under current circumstances
		return MAX_INT;
	}

	const int iBuildLeft = std::max(0, getBuildTime(eBuild) - getBuildProgress(eBuild) - iNowBuildRate);

	int iTurnsLeft = iBuildLeft / iThenBuildRate;

	if (iTurnsLeft * iThenBuildRate < iBuildLeft)
	{
		iTurnsLeft++;
	}
	iTurnsLeft++;

	return std::max(1, iTurnsLeft);
}


int CvPlot::getFeatureProduction(BuildTypes eBuild, TeamTypes eTeam, CvCity** ppCity) const
{

	if (getFeatureType() == NO_FEATURE)
	{
		return 0;
	}

	*ppCity = getWorkingCity();

	if (*ppCity == NULL)
	{
		*ppCity = GC.getMap().findCity(getX(), getY(), NO_PLAYER, eTeam, false);
	}

	if (*ppCity == NULL)
	{
		return 0;
	}

	if (!GET_TEAM(eTeam).isHasTech(GC.getBuildInfo(eBuild).getFeatureTech(getFeatureType())))
	{
		return 0;
	}
	int iProduction = GC.getBuildInfo(eBuild).getFeatureProduction(getFeatureType());
	iProduction -= std::max(0, plotDistance(getX(), getY(), (*ppCity)->getX(), (*ppCity)->getY()) - 2) * 5;

	iProduction *= std::max(0, (GET_PLAYER((*ppCity)->getOwner()).getFeatureProductionModifier() + 100));
	iProduction /= 100;

	iProduction *= std::min((GC.getDefineINT("BASE_FEATURE_PRODUCTION_PERCENT") + (GC.getDefineINT("FEATURE_PRODUCTION_PERCENT_MULTIPLIER") * (*ppCity)->getPopulation())), 100);
	iProduction /= 100;

	if (getTeam() != eTeam)
	{
		iProduction *= GC.getDefineINT("DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT");
		iProduction /= 100;
	}

	iProduction *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getHammerCostPercent();
	iProduction /= 100;

	return std::max(0, iProduction);
}

CvUnit* CvPlot::getBestDefenderExternal(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove) const
{
	CvUnit* bestDefender = getBestDefender(eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove);
	if (bestDefender == NULL || !bestDefender->isInViewport() || bestDefender->isUsingDummyEntities())
	{
		return NULL;
	}
	return bestDefender;
}

namespace {
	int getDefenderScore(
		const CvUnit* unitX,
		PlayerTypes eOwner,
		PlayerTypes eAttackingPlayer,
		const CvUnit* pAttacker,
		bool bTestAtWar,
		bool bTestPotentialEnemy,
		bool bTestCanMove,
		bool bAssassinate,
		ECacheAccess::flags cacheAccess
	)
	{
		FAssert(unitX->plot());

		// Check the unit is valid
		if (unitX->AI_getPredictedHitPoints() == 0 // Going to be dead
		||  unitX->isDead() // Already dead
		// Doesn't belong to the player we are interested in
		||  eOwner != NO_PLAYER && unitX->getOwner() != eOwner)
		{
			return 0;
		}
		const CvPlot* plotX = unitX->plot();

		CvChecksum checksum;
		if (cacheAccess != ECacheAccess::None)
		{
			// Work out our cache key (todo: change this to just be a proper hash key)
			if (pAttacker)
			{
				checksum.add(pAttacker->getID());
			}
			checksum.add((int)unitX->getOwner());
			checksum.add(unitX->getID());
			checksum.add((int)bTestAtWar);
			checksum.add((int)bTestPotentialEnemy * 10);
			checksum.add((int)bTestCanMove * 100);
			checksum.add((int)eAttackingPlayer);
			checksum.add((int)eOwner);
			checksum.add((int)bAssassinate * 100);

			if(cacheAccess & ECacheAccess::Read)
			{
				// look in the cache
				DefenderScoreCache::const_iterator itr = g_bestDefenderCache->find(checksum.get());
				if (itr != g_bestDefenderCache->end()
					&& itr->second.iHealth == unitX->getHP())
				{
					return itr->second.iValue;
				}
			}
		}

		int iValue = 0;

		if (
			// Assassination target, if we have one
			(!bAssassinate || pAttacker && unitX->isTargetOf(*pAttacker))
			// Not invisible to the player, if any
			&& (eAttackingPlayer == NO_PLAYER || !unitX->isInvisible(GET_PLAYER(eAttackingPlayer).getTeam(), false))
			// War enemy (todo: we should just assert that attacking player is valid if we are testing for enemy)
			&& (
					!bTestAtWar
				||	eAttackingPlayer == NO_PLAYER
				||	unitX->isEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), plotX)
				||	(pAttacker && pAttacker->isEnemy(GET_PLAYER(unitX->getOwner()).getTeam(), plotX, unitX))
			)
			// Potential enemy (todo: we should just assert that attacking player is valid if we are testing for potential enemy)
			&& (
					!bTestPotentialEnemy
				||	eAttackingPlayer == NO_PLAYER
				||	unitX->isPotentialEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), plotX)
				||	pAttacker && pAttacker->isPotentialEnemy(GET_PLAYER(unitX->getOwner()).getTeam(), plotX, unitX)
			)
			// If we are testing movement, can the unit move?
			&& (!bTestCanMove || unitX->canMove() && !unitX->isCargo() || bAssassinate)
		)
		{
			iValue = unitX->defenderValue(pAttacker);
		}

		if (cacheAccess & ECacheAccess::Write)
		{
			unitDefenderInfo info;
			info.iHealth = unitX->getHP();
			info.iValue = iValue;
			(*g_bestDefenderCache)[checksum.get()] = info;
		}
		return iValue;
	}
}

CvUnit* CvPlot::getBestDefender(EDefenderScore::flags flags, PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker) const
{
	return getBestDefender(eOwner, eAttackingPlayer, pAttacker,
		flags & EDefenderScore::TestAtWar,
		flags & EDefenderScore::TestPotentialEnemy,
		flags & EDefenderScore::TestCanMove,
		flags & EDefenderScore::Assassinate,
		flags & EDefenderScore::ClearCache
	);
}

CvUnit* CvPlot::getBestDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove, bool bAssassinate, bool bClearCache) const
{
	PROFILE_FUNC();

/*TB - Commented out due to suspicion that this will lead to some of the strange problems we have with bad defense decisions or poor group evaluations changing once attack is launched.
	if (isCity() && getPlotCity()->isInvaded() && !bAssassinate)
	{
		return getWorstDefender(eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove);
	}
*/

	// Heavily cache this routine as during large stack fights the question is asked over and over,
	//	with the same attacker cropping up repeatedly
	if (g_bestDefenderCachePlot != this)
	{
		g_bestDefenderCachePlot = this;
		g_bestDefenderCache->clear();
	}

	int iBestValue = 0;
	if (bAssassinate) iBestValue = -1;
	CvUnit* pBestUnit = nullptr;

	// Can't use this as it requires more than 9 args, and bind only supports 9
	//CvUnit* pBestUnit = scoring::max_score(units(),
	//	bind(&CvSelectionGroup::getDefenderScore, _1, eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove, bAssassinate, bClearCache ? ECacheAccess::Write : ECacheAccess::ReadWrite)
	//).get_value_or(nullptr);

	// Toffer - The above comment is now false as I removed a redundant arg so that it is now exactly 9 arguments.

	foreach_ (CvUnit* unitX, units())
	{
		int iValue = getDefenderScore(unitX, eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove, bAssassinate, bClearCache ? ECacheAccess::Write : ECacheAccess::ReadWrite);

		if (iValue > iBestValue && unitX->getOwner() != eAttackingPlayer)
		{
			pBestUnit = unitX;
			iBestValue = iValue;
		}
	}
	return pBestUnit;
}


CvUnit* CvPlot::getFirstDefender(EDefenderScore::flags flags, PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker) const
{
	return getFirstDefender(eOwner, eAttackingPlayer, pAttacker,
		flags & EDefenderScore::TestAtWar,
		flags & EDefenderScore::TestPotentialEnemy,
		flags & EDefenderScore::TestCanMove
	);
}

CvUnit* CvPlot::getFirstDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvUnit* unitX, units())
	{
		if (getDefenderScore(unitX, eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove, false, ECacheAccess::Write) > 0)
		{
			return unitX;
		}
	}
	return NULL;
}

// returns a sum of the strength (adjusted by firepower) of all the units on a plot
int CvPlot::AI_sumStrength(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, DomainTypes eDomainType, StrengthFlags::flags flags /*= StrengthFlags::DefensiveBonuses*/, int iRange) const
{
	PROFILE_EXTRA_FUNC();
	const int COLLATERAL_COMBAT_DAMAGE = GC.getDefineINT("COLLATERAL_COMBAT_DAMAGE"); // K-Mod. (currently this number is "10")

	const bool bTestAtWar = flags & StrengthFlags::TestAtWar;
	const bool bTestPotentialEnemy = flags & StrengthFlags::TestPotentialEnemy;
	const bool bDefensiveBonuses = flags & StrengthFlags::DefensiveBonuses;
	const bool bCollatoral = flags & StrengthFlags::CollatoralDamage;
	FAssertMsg(!bTestPotentialEnemy || eAttackingPlayer != NO_PLAYER, "Need to specify the attacking player to filter by enemies");

	int	strSum = 0;

	foreach_(const CvPlot* pLoopPlot, rect(iRange, iRange))
	{
		foreach_(const CvUnit* pLoopUnit, pLoopPlot->units())
		{
			if ((eOwner == NO_PLAYER || pLoopUnit->getOwner() == eOwner)
				&& (eAttackingPlayer == NO_PLAYER || !pLoopUnit->isInvisible(GET_PLAYER(eAttackingPlayer).getTeam(), false))
				&& (!bTestAtWar || eAttackingPlayer == NO_PLAYER || atWar(GET_PLAYER(eAttackingPlayer).getTeam(), pLoopUnit->getTeam()))
				&& (!bTestPotentialEnemy || eAttackingPlayer == NO_PLAYER || pLoopUnit->isPotentialEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this))
				&& (eDomainType == NO_DOMAIN || pLoopUnit->getDomainType() == eDomainType)
				)
			{
				// we may want to be more sophisticated about domains
				// somewhere we need to check to see if this is a city, if so, only land units can defend here, etc
				strSum += pLoopUnit->currEffectiveStr(bDefensiveBonuses? this : nullptr, nullptr);

				// K-Mod assume that if we aren't counting defensive bonuses, then we should be counting collateral
				if (pLoopUnit->collateralDamage() > 0 && bCollatoral)
				{
					//int iPossibleTargets = std::min((pAttackedPlot->getNumVisibleEnemyDefenders(pLoopUnit) - 1), pLoopUnit->collateralDamageMaxUnits());
					// unfortunately, we can't count how many targets there are...
					int iPossibleTargets = pLoopUnit->collateralDamageMaxUnits();
					if (iPossibleTargets > 0)
					{
						// collateral damage is not trivial to calculate. This estimate is pretty rough.
						strSum += pLoopUnit->baseCombatStr() * COLLATERAL_COMBAT_DAMAGE * pLoopUnit->collateralDamage() * iPossibleTargets / 100;
					}
				}
				// K-Mod end
			}
		}
	}

	return strSum;
}


CvUnit* CvPlot::getSelectedUnit() const
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvUnit* unit, units() | filtered(bind(CvUnit::IsSelected, _1)))
	{
		return unit;
	}
	return NULL;
}


int CvPlot::getUnitPower(PlayerTypes eOwner) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (eOwner == NO_PLAYER || pLoopUnit->getOwner() == eOwner)
		{
			iCount += pLoopUnit->getUnitInfo().getPowerValue();
		}
	}

	return iCount;
}

// Super Forts begin *bombard*
bool CvPlot::isBombardable(const CvUnit* pUnit) const
{
	if (NULL != pUnit && !pUnit->isEnemy(getTeam()))
	{
		return false;
	}

	const ImprovementTypes eImprovement = getImprovementType();
	if(eImprovement == NO_IMPROVEMENT)
	{
		return false;
	}
	else
	{
		if(GC.getImprovementInfo(eImprovement).isBombardable())
		{
			return (getDefenseDamage() < GC.getImprovementInfo(eImprovement).getDefenseModifier());
		}
	}
	return false;
}

bool CvPlot::isBombarded() const
{
	return m_bBombarded;
}

void CvPlot::setBombarded(bool bNewValue)
{
	m_bBombarded = bNewValue;
}

int CvPlot::getDefenseDamage() const
{
	return m_iDefenseDamage;
}

void CvPlot::changeDefenseDamage(int iChange)
{
	if ((iChange != 0) && (getImprovementType() != NO_IMPROVEMENT))
	{
		m_iDefenseDamage = range((m_iDefenseDamage + iChange), 0, GC.getImprovementInfo(getImprovementType()).getDefenseModifier());

		if (iChange > 0)
		{
			setBombarded(true);
		}
	}
}


void CvPlot::pushCultureFromImprovement(PlayerTypes ePlayer, int iChange, int iRange, bool bUpdate)
{
	PROFILE_EXTRA_FUNC();
	// Pushes a little culture from improvements that generate it; used when improvement is newly placed or acquired
	// to get potential nearby neutral plots. doImprovementCulture() gets regular culture.
	// Removing culture on loss happens by natural decay or IDW
	if (0 == iChange || iRange < 0)
	{
		return;
	}
	for (int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			const int iCultureDistance = plotDistance(0, 0, iDX, iDY);

			if (iCultureDistance <= iRange)
			{
				CvPlot* pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					// Make sure that if a plot is newly in range it gets at least 1 culture
					if (iChange > 0 && pLoopPlot->getCulture(ePlayer) == 0)
					{
						pLoopPlot->changeCulture(ePlayer, 1, false);
					}
					if (bUpdate)
					{
						pLoopPlot->updateCulture(true, true);
					}
				}
			}
		}
	}
}


void CvPlot::doImprovementCulture(PlayerTypes ePlayer, const CvImprovementInfo& imp)
{
	PROFILE_EXTRA_FUNC();
	if (imp.getCulture() < 1)
	{
		return;
	}
	const int iRange = std::max(0, imp.getCultureRange());
	const int iCulture = imp.getCulture();

	foreach_(CvPlot* plotX, rect(iRange, iRange))
	{
		if (plotDistance(getX(), getY(), plotX->getX(), plotX->getY()) <= iRange)
		{
			plotX->changeCulture(ePlayer, iCulture, true);
		}
	}
}


// Super Forts begin *canal* *choke*
int CvPlot::countRegionPlots(const CvPlot* pInvalidPlot) const
{
	int iCount = 0;
	int iInvalidPlot = (pInvalidPlot == NULL) ? 0 : GC.getMap().plotNum(pInvalidPlot->getX(), pInvalidPlot->getY()) + 1;
	FAStar* pRegionFinder = gDLL->getFAStarIFace()->create();
	gDLL->getFAStarIFace()->Initialize(pRegionFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(),
		NULL, NULL, NULL, stepValid, NULL, countRegion, NULL);
	gDLL->getFAStarIFace()->SetData(pRegionFinder, &iCount);
	// Note to self: for GeneratePath() should bReuse be true or false?
	gDLL->getFAStarIFace()->GeneratePath(pRegionFinder, getX(), getY(), -1, -1, false, iInvalidPlot, false);
	gDLL->getFAStarIFace()->destroy(pRegionFinder);
	return iCount;
}

int CvPlot::countAdjacentPassableSections(bool bWater) const
{
	PROFILE_EXTRA_FUNC();
	CvPlot* pAdjacentPlot;
	int iPassableSections = 0;
	bool bInPassableSection = false;

	// Are we looking for water passages or land passages?
	if(bWater)
	{
		bool bPlotIsWater = isWater();
		// This loop is for water
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isWater())
				{
					// Don't count diagonal hops across land isthmus
					if (bPlotIsWater && !isCardinalDirection((DirectionTypes)iI))
					{
						if (!(GC.getMap().plot(getX(), pAdjacentPlot->getY())->isWater()) && !(GC.getMap().plot(pAdjacentPlot->getX(), getY())->isWater()))
						{
							continue;
						}
					}
					if(pAdjacentPlot->isImpassable())
					{
						if(isCardinalDirection((DirectionTypes)iI))
						{
							bInPassableSection = false;
						}
					}
					else if(!bInPassableSection)
					{
						bInPassableSection = true;
						++iPassableSections;
					}
				}
				else
				{
					bInPassableSection = false;
				}
			}
		}
	}
	else
	{
		// This loop is for land
		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
			if(pAdjacentPlot != NULL)
			{
				if(pAdjacentPlot->isWater() || pAdjacentPlot->isImpassable())
				{
					if(isCardinalDirection((DirectionTypes)iI))
					{
						bInPassableSection = false;
					}
				}
				else if(!bInPassableSection)
				{
					bInPassableSection = true;
					++iPassableSections;
				}
			}
		}
	}
	// Corner Case Correction
	pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_NORTH);
	if(pAdjacentPlot != NULL && (bWater == pAdjacentPlot->isWater()) && !pAdjacentPlot->isImpassable())
	{
		pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_NORTHWEST);
		if(pAdjacentPlot != NULL && (bWater == pAdjacentPlot->isWater()))
		{
			if(pAdjacentPlot->isImpassable())
			{
				pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
				if(pAdjacentPlot != NULL && (bWater == pAdjacentPlot->isWater()) && !pAdjacentPlot->isImpassable())
				{
					--iPassableSections;
				}
			}
			else
			{
				--iPassableSections;
			}
		}
		else if(!bWater)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
			if(pAdjacentPlot != NULL && !pAdjacentPlot->isWater() && !pAdjacentPlot->isImpassable())
			{
				--iPassableSections;
			}
		}
	}
	return iPassableSections;
}

int CvPlot::countImpassableCardinalDirections() const
{
	return algo::count_if(cardinalDirectionAdjacent(),
		bind(CvPlot::isImpassable, _1, NO_TEAM) || bind(CvPlot::area, _1) != area()
	);
}
// Super Forts end

// Super Forts begin *canal*
int CvPlot::getCanalValue() const
{
	return m_iCanalValue;
}

void CvPlot::setCanalValue(int iNewValue)
{
	m_iCanalValue = iNewValue;
}

void CvPlot::calculateCanalValue()
{
	PROFILE_EXTRA_FUNC();
	bool bInWaterSection;
	CvPlot *pAdjacentPlot, *apPlotsToCheck[4];
	int iWaterSections, iPlotsFound, iMaxDistance;
	int iCanalValue = 0;

	if(isCoastalLand() && !isImpassable())
	{
		iWaterSections = countAdjacentPassableSections(true);
		if(iWaterSections > 1)
		{
			iMaxDistance = 0;
			iPlotsFound = 0;
			bInWaterSection = false;
			// Find appropriate plots to be used for path distance calculations
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
				if(pAdjacentPlot != NULL)
				{
					if(pAdjacentPlot->isWater())
					{
						if(pAdjacentPlot->isImpassable())
						{
							if(isCardinalDirection((DirectionTypes)iI))
							{
								bInWaterSection = false;
							}
						}
						else if(!bInWaterSection)
						{
							bInWaterSection = true;
							apPlotsToCheck[iPlotsFound] = pAdjacentPlot;
							if((++iPlotsFound) == iWaterSections)
								break;
						}
					}
					else
					{
						bInWaterSection = false;
					}
				}
			}
			// Find the max path distance out of all possible pairs of plots
			for (int iI = 0; iI < (iPlotsFound - 1); ++iI)
			{
				for (int iJ = iI + 1; iJ < iPlotsFound; ++iJ)
				{
					if(!apPlotsToCheck[iI]->isLake() || !apPlotsToCheck[iJ]->isLake())
					{
						int iDistance = GC.getMap().calculatePathDistance(apPlotsToCheck[iI], apPlotsToCheck[iJ]);
						if(iDistance == -1)
						{

							// If no path was found then value is based off the number of plots in the region minus a minimum area
							iDistance = std::min(apPlotsToCheck[iI]->countRegionPlots(), apPlotsToCheck[iJ]->countRegionPlots()) - 7;
							iDistance *= 4;
						}
						else
						{
							// Path already would have required 2 steps, and I don't care that much about saving just 1 or 2 moves
							iDistance -= 4;
						}
						if(iDistance > iMaxDistance)
						{
							iMaxDistance = iDistance;
						}
					}
				}
			}
			iCanalValue = iMaxDistance * (iPlotsFound - 1);
		}
	}

	setCanalValue(iCanalValue);
}
// Super Forts end

// Super Forts begin *choke*
int CvPlot::getChokeValue() const
{
	return m_iChokeValue;
}

void CvPlot::setChokeValue(int iNewValue)
{
	m_iChokeValue = iNewValue;
}

void CvPlot::calculateChokeValue()
{
	PROFILE_EXTRA_FUNC();
	bool bInPassableSection;
	CvPlot *pAdjacentPlot, *apPlotsToCheck[4];
	int iPassableSections, iPlotsFound, iMaxDistance;
	int iChokeValue = 0;
	bool bWater = isWater();

	if(!isImpassable() && countImpassableCardinalDirections() > 1)
	{
		iPassableSections = countAdjacentPassableSections(bWater);
		if(iPassableSections > 1)
		{
			iMaxDistance = 0;
			iPlotsFound = 0;
			bInPassableSection = false;
			// Find appropriate plots to be used for path distance calculations
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
				if(pAdjacentPlot != NULL)
				{
					if(pAdjacentPlot->isWater() == bWater)
					{
						// Don't count diagonal hops across land isthmus
						if (bWater && !isCardinalDirection((DirectionTypes)iI))
						{
							if (!(GC.getMap().plot(getX(), pAdjacentPlot->getY())->isWater()) && !(GC.getMap().plot(pAdjacentPlot->getX(), getY())->isWater()))
							{
								continue;
							}
						}
						if(pAdjacentPlot->isImpassable())
						{
							if(isCardinalDirection((DirectionTypes)iI))
							{
								bInPassableSection = false;
							}
						}
						else if(!bInPassableSection)
						{
							bInPassableSection = true;
							apPlotsToCheck[iPlotsFound] = pAdjacentPlot;
							if((++iPlotsFound) == iPassableSections)
								break;
						}
					}
					else if(bWater || isCardinalDirection((DirectionTypes)iI))
					{
						bInPassableSection = false;
					}
				}
			}
			// Find the max path distance out of all possible pairs of plots
			for (int iI = 0; iI < (iPlotsFound - 1); ++iI)
			{
				for (int iJ = iI + 1; iJ < iPlotsFound; ++iJ)
				{
					int iDistance = GC.getMap().calculatePathDistance(apPlotsToCheck[iI], apPlotsToCheck[iJ], this);
					if(iDistance == -1)
					{
						// If no path was found then value is based off the number of plots in the region minus a minimum area
						iDistance = std::min(apPlotsToCheck[iI]->countRegionPlots(this), apPlotsToCheck[iJ]->countRegionPlots(this)) - 4;
						iDistance *= 4;
					}
					else
					{
						// Path already would have required 2 steps, but we forced the enemy to go another way so there is some value
						iDistance -= 1;
					}
					if(iDistance > iMaxDistance)
					{
						iMaxDistance = iDistance;
					}
				}
			}
			iChokeValue = iMaxDistance * (iPlotsFound - 1);
		}
	}

	setChokeValue(iChokeValue);
}
// Super Forts end

int CvPlot::defenseModifier(TeamTypes eDefender, bool bIgnoreBuilding, bool bHelp) const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	int iModifier = GC.getTerrainInfo(getTerrainType()).getDefenseModifier();

	if (getFeatureType() != NO_FEATURE)
	{
		iModifier += GC.getFeatureInfo(getFeatureType()).getDefenseModifier();
	}

	if (isHills())
	{
		iModifier += GC.getHILLS_EXTRA_DEFENSE();
	}
	else if (isAsPeak())
	{
		iModifier += GC.getPEAK_EXTRA_DEFENSE();
	}

	{
		const ImprovementTypes eImprovement = (
			bHelp
			?
			getRevealedImprovementType(GC.getGame().getActiveTeam())
			:
			getImprovementType()
		);
		if (eImprovement != NO_IMPROVEMENT && eDefender != NO_TEAM
		&& (getTeam() == NO_TEAM || GET_TEAM(eDefender).isFriendlyTerritory(getTeam())))
		{
			// Super Forts begin *bombard*
			iModifier += GC.getImprovementInfo(eImprovement).getDefenseModifier() - getDefenseDamage();
		}
	}

	if (!bHelp)
	{
		const CvCity* pCity = getPlotCity();

		if (pCity)
		{
			iModifier += pCity->getDefenseModifier(bIgnoreBuilding);
		}
	}
	return iModifier;
}

void CvPlot::flushMovementCostCache()
{
	if ( m_resultHashMap != NULL )
	{
		m_resultHashMap->clear();
	}
}

int CvPlot::movementCost(const CvUnit* pUnit, const CvPlot* pFromPlot) const
{
	//PROFILE_FUNC();

	const int iMoveDenominator = GC.getMOVE_DENOMINATOR();

	int iResult;
	int iResultKeyHash = -1;

	//	Don't use the cache for human players because that could cause values cached for unrevealed plots to be used on revealed
	//	ones and visa versa. That not only results in incorrect path generation, but also to OOS errors in multiplayer
	//	due to the cached values being used at different times (so on one machine the plot may be first seen as revealed, while on
	//	another as unrevealed)
	if ( !pUnit->isHuman() ) //&& isRevealed(pUnit->getTeam()->getID()) - cleaner to use this condition directly but
							 // actually its only accessed in the logic below for humans so isHuman() is a cheaper proxy
	{
		iResultKeyHash = pFromPlot->getMovementCharacteristicsHash() ^
						 (getMovementCharacteristicsHash() << 1) ^ pUnit->getMovementCharacteristicsHash() ^
						 GET_PLAYER(pUnit->getOwner()).getZobristValue();

		if ( pFromPlot->getOwner() != NO_PLAYER )
		{
			iResultKeyHash ^= GET_PLAYER(pFromPlot->getOwner()).getZobristValue();
		}

		if ( getOwner() != NO_PLAYER )
		{
			iResultKeyHash ^= GET_PLAYER(getOwner()).getZobristValue();
		}

		iResult = -1;

		stdext::hash_map<int,int>::const_iterator match = m_resultHashMap->find(iResultKeyHash);
		if ( match != m_resultHashMap->end() )
		{
			iResult = match->second;
		}

		if ( iResult != -1 )
		{
			return iResult;
		}
	}

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");


	if (pUnit->flatMovementCost() || (pUnit->getDomainType() == DOMAIN_AIR))
	{
		return iMoveDenominator;
	}
	else if (pUnit->isHuman() && !isRevealed(pUnit->getTeam(), false))
	{
		iResult = pUnit->maxMoves();
	}
	else if (!pFromPlot->isValidDomainForLocation(*pUnit))
	{
		iResult = pUnit->maxMoves();
	}
	else if (!isValidDomainForAction(*pUnit))
	{
		iResult = iMoveDenominator;
	}
	else
	{
		FAssert(pUnit->getDomainType() != DOMAIN_IMMOBILE);

		const bool bRiverCrossing = pFromPlot->isRiverCrossing(directionXY(pFromPlot, this));

		if (pFromPlot->isValidRoute(pUnit) && isValidRoute(pUnit) && (!bRiverCrossing || GET_TEAM(pUnit->getTeam()).isBridgeBuilding()))
		{
			const RouteTypes fromRouteType = pFromPlot->getRouteType();
			const CvRouteInfo& fromRoute = GC.getRouteInfo(fromRouteType);
			const RouteTypes toRouteType = getRouteType();
			const CvRouteInfo& toRoute = GC.getRouteInfo(toRouteType);

			int iRouteCost = fromRoute.getMovementCost() + GET_TEAM(pUnit->getTeam()).getRouteChange(fromRouteType);
			FAssertMsg(iRouteCost > 0, "Route cost is expected to be greater than 0");
			if (toRouteType != fromRouteType)
			{
				const int iToRouteCost = toRoute.getMovementCost() + GET_TEAM(pUnit->getTeam()).getRouteChange(toRouteType);

				if (iToRouteCost > iRouteCost)
				{
					iRouteCost = iToRouteCost;
				}
			}

			const int iRouteFlatCost = std::max(fromRoute.getFlatMovementCost(), toRoute.getFlatMovementCost()) * pUnit->baseMoves();
			FAssert(iRouteFlatCost > 0);
			iResult = std::min(iMoveDenominator, std::min(iRouteCost, iRouteFlatCost));
		}
		else
		{
			int iRegularCost;
			bool bIgnoresTerrainCost = pUnit->ignoreTerrainCost();

			if (bIgnoresTerrainCost)
			{
				iRegularCost = 1;
			}
			else
			{
				iRegularCost = GC.getTerrainInfo(getTerrainType()).getMovementCost();

				if (getFeatureType() != NO_FEATURE)
				{
					iRegularCost += GC.getFeatureInfo(getFeatureType()).getMovementCost();
				}

				if (isHills())
				{
					iRegularCost += GC.getHILLS_EXTRA_MOVEMENT();
				}

				if (bRiverCrossing)
				{
					iRegularCost += GC.getRIVER_EXTRA_MOVEMENT();
				}

				if (isAsPeak())
				{
					if (!GET_TEAM(pUnit->getTeam()).isMoveFastPeaks())
					{
						iRegularCost += GC.getPEAK_EXTRA_MOVEMENT();
					}
					iRegularCost += 3;
				}
			}

			if (iRegularCost > 0)
			{
				iRegularCost = std::max(1, (iRegularCost - pUnit->getExtraMoveDiscount()));

				//if (iRegularCost > pUnit->baseMoves())
				//{
				//	iRegularCost = pUnit->baseMoves();
				//}
			}

			FAssert(iRegularCost > 0);

			if (iRegularCost <= 1) //could use == as well but just to be safe...<=
			{
				bIgnoresTerrainCost = true;//effectively we have this situation by adjustment means, probably by getExtraMoveDiscount()
			}

			iRegularCost *= iMoveDenominator;
			bool bFeatureDoubleMove = ((getFeatureType() != NO_FEATURE && pUnit->isFeatureDoubleMove(getFeatureType())) || (isHills() && pUnit->isHillsDoubleMove()));
			bool bTerrainDoubleMove = pUnit->isTerrainDoubleMove(getTerrainType());

			if (!bIgnoresTerrainCost && bFeatureDoubleMove)
			{
				iRegularCost /= 4;
			}
			else if (bTerrainDoubleMove || (bIgnoresTerrainCost && bFeatureDoubleMove))
			{
				iRegularCost /= 2;
			}

			iResult = std::max(90, iRegularCost);
		}
	}

	FAssert(iResult > 0);
	iResult = std::max(1, iResult);

	if (!pUnit->isHuman())
	{
		(*m_resultHashMap)[iResultKeyHash] = iResult;
	}

	return iResult;
}

int	CvPlot::getHasMountainLeader(TeamTypes eTeam) const
{
	int iCount = plotCount(PUF_isMountainLeader, -1, -1, NULL, NO_PLAYER, eTeam);
	return (iCount > 0);
}

bool CvPlot::isAdjacentOwned() const
{
	return algo::any_of(adjacent(), CvPlot::fn::isOwned());
}


bool CvPlot::isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly) const
{
	return algo::any_of(adjacent(), CvPlot::fn::getOwner() == ePlayer && (!CvPlot::fn::isWater() || !bLandOnly));
}


bool CvPlot::isAdjacentTeam(TeamTypes eTeam, bool bLandOnly) const
{
	return algo::any_of(adjacent(), CvPlot::fn::getTeam() == eTeam && (!CvPlot::fn::isWater() || !bLandOnly));
}


/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/10/10                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
bool CvPlot::isHasPathToEnemyCity( TeamTypes eAttackerTeam, bool bIgnoreBarb ) const
{
	PROFILE_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eAttackerTeam);

	if (area()->getNumCities() - GET_TEAM(eAttackerTeam).countNumCitiesByArea(area()) == 0)
	{
		return false;
	}

	// Imitate instatiation of irrigated finder, pIrrigatedFinder
	// Can't mimic step finder initialization because it requires creation from the exe
	std::vector<TeamTypes> teamVec;
	teamVec.push_back(eAttackerTeam);
	teamVec.push_back(NO_TEAM);
	FAStar* pTeamStepFinder = gDLL->getFAStarIFace()->create();
	gDLL->getFAStarIFace()->Initialize(pTeamStepFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), stepDestValid, stepHeuristic, stepCost, teamStepValid, stepAdd, NULL, NULL);
	gDLL->getFAStarIFace()->SetData(pTeamStepFinder, &teamVec);

	bool bFound = false;

	// First check capitals
	for (int iI = 0; !bFound && iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive()
		&& GET_TEAM(eAttackerTeam).AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_WARPLAN
		&& (!bIgnoreBarb || !GET_PLAYER((PlayerTypes)iI).isNPC() )) //&& !GET_PLAYER((PlayerTypes)iI).isMinorCiv()
		{
			const CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).getCapitalCity();

			if (pLoopCity != NULL && pLoopCity->area() == area())
			{
				bFound = gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, getX(), getY(), pLoopCity->getX(), pLoopCity->getY(), false, 0, true);

				if (bFound)
				{
					break;
				}
			}
		}
	}

	// Check all other cities
	for (int iI = 0; !bFound && iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive()
		&& GET_TEAM(eAttackerTeam).AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_WARPLAN
		&& (!bIgnoreBarb || !GET_PLAYER((PlayerTypes)iI).isNPC() )) //&& !GET_PLAYER((PlayerTypes)iI).isMinorCiv()
		{
			foreach_(const CvCity* pLoopCity, GET_PLAYER((PlayerTypes)iI).cities())
			{
				if (pLoopCity->area() == area() && !pLoopCity->isCapital())
				{
					bFound = gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, getX(), getY(), pLoopCity->getX(), pLoopCity->getY(), false, 0, true);

					if (bFound)
					{
						break;
					}
				}
			}
		}
	}
	gDLL->getFAStarIFace()->destroy(pTeamStepFinder);

	return bFound;
}

bool CvPlot::isHasPathToPlayerCity(TeamTypes eMoveTeam, PlayerTypes eOtherPlayer) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, MAX_TEAMS, eMoveTeam);
	FASSERT_BOUNDS(0, MAX_PLAYERS, eOtherPlayer);

	if (area()->getCitiesPerPlayer(eOtherPlayer) == 0)
	{
		return false;
	}

	// Imitate instatiation of irrigated finder, pIrrigatedFinder
	// Can't mimic step finder initialization because it requires creation from the exe
	std::vector<TeamTypes> teamVec;
	teamVec.push_back(eMoveTeam);
	teamVec.push_back(GET_PLAYER(eOtherPlayer).getTeam());
	FAStar* pTeamStepFinder = gDLL->getFAStarIFace()->create();
	gDLL->getFAStarIFace()->Initialize(pTeamStepFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), stepDestValid, stepHeuristic, stepCost, teamStepValid, stepAdd, NULL, NULL);
	gDLL->getFAStarIFace()->SetData(pTeamStepFinder, &teamVec);

	bool bFound = false;

	foreach_(const CvCity* pLoopCity, GET_PLAYER(eOtherPlayer).cities() | filtered(CvCity::fn::area() == area()))
	{
		bFound = gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, getX(), getY(), pLoopCity->getX(), pLoopCity->getY(), false, 0, true);

		if (bFound)
		{
			break;
		}
	}

	gDLL->getFAStarIFace()->destroy(pTeamStepFinder);

	return bFound;
}

int CvPlot::calculatePathDistanceToPlot( TeamTypes eTeam, CvPlot* pTargetPlot ) const
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if( pTargetPlot->area() != area() )
	{
		return 0;
	}

	// Imitate instatiation of irrigated finder, pIrrigatedFinder
	// Can't mimic step finder initialization because it requires creation from the exe
	std::vector<TeamTypes> teamVec;
	teamVec.push_back(eTeam);
	teamVec.push_back(NO_TEAM);
	FAStar* pTeamStepFinder = gDLL->getFAStarIFace()->create();
	gDLL->getFAStarIFace()->Initialize(pTeamStepFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), stepDestValid, stepHeuristic, stepCost, teamStepValid, stepAdd, NULL, NULL);
	gDLL->getFAStarIFace()->SetData(pTeamStepFinder, &teamVec);
	gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, getX(), getY(), pTargetPlot->getX(), pTargetPlot->getY(), false, 0, true);

	const FAStarNode* pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

	const int iPathDistance = pNode ? pNode->m_iData1 : -1;

	gDLL->getFAStarIFace()->destroy(pTeamStepFinder);

	return iPathDistance;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


void CvPlot::invalidateActivePlayerPlotCache()
{
	m_iActivePlayerSafeRangeCache = -1;
	m_iActivePlayerSafeRangeCacheTestMoves = -1;
	m_bActivePlayerHasDangerCache = false;
	m_bActivePlayerHasDangerCacheTestMoves = false;

	CachePathValidityResult(NULL, false, false);
	CachePathValidityResult(NULL, true, false);
}

void CvPlot::invalidateBorderDangerCache() const
{
	for (int iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_borderDangerCache[iI] = false;
	}
}


/* returns the city adjacent to this plot or NULL if none exists. more than one can't exist, because of the 2-tile spacing btwn cities limit. */
//Alberts2: added eplayer parameter to only return the city if the owner == eplayer
CvCity* CvPlot::getAdjacentCity(PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPlot* pLoopPlot, rect(1, 1))
	{
		CvCity* cityX = pLoopPlot->getPlotCity();
		if (cityX)
		{
			if (ePlayer != NO_PLAYER && cityX->getOwner() != ePlayer)
			{
				return NULL;
			}
			return cityX;
		}
	}
	return NULL;
}


PlayerTypes CvPlot::calculateCulturalOwner(bool bCountLastTurn) const
{
	// Function calculates who *should* own this plot via cultural means
	// Actually setting owner happens in CvPlot::setOwner
	PROFILE("CvPlot::calculateCulturalOwner()");

	const PlayerTypes eOwner = getOwner();
	const PlayerTypes eHighestCulturePlayer = findHighestCulturePlayer(false, bCountLastTurn);

	if (eOwner == NO_PLAYER)
	{
		if (eHighestCulturePlayer != NO_PLAYER)
		{
			return eHighestCulturePlayer;
		}
		// If all plots around this neutral plot (no culture or no owner) are owned by a player, grant that player this plot.
		// Toffer - I don't see why this should be a rule, neutral land is neutral for a reason.
		return NO_PLAYER; //getPlayerWithTerritorySurroundingThisPlotCardinally();
	}

	// non-city, non fort plots that are *adjacent* to cities may always belong to those cities' owners
	if (GC.getGame().isOption(GAMEOPTION_CULTURE_MIN_CITY_BORDER) && !isCity(true))
	{
		const CvCity* adjacentCity = getAdjacentCity();
		if (adjacentCity) return adjacentCity->getOwner();
	}

	if (!GET_PLAYER(eOwner).isAlive())
	{
		return eHighestCulturePlayer;
	}

	// Fixed borders adjustments for culture threshold, unit passive claiming
	// Have to check for the current owner being alive for this to work correctly in the cultural
	//	re-assignment that takes place as he dies during processing of the capture of his last city.
	if (GET_PLAYER(eOwner).hasFixedBorders())
	{
		// If *current* owner has fixed borders, keeps control if
		// they have over xml specified ratio culture of best player.
		if (eHighestCulturePlayer != NO_PLAYER && eHighestCulturePlayer != eOwner
		&& getCulture(eOwner) * GC.getDefineINT("FIXED_BORDERS_CULTURE_RATIO_PERCENT") / 100 >= getCulture(eHighestCulturePlayer)
		// Unit passively maintaining claims.
		|| algo::any_of(units(), CvUnit::fn::getTeam() == getTeam() && CvUnit::fn::canClaimTerritory(NULL)))
		{
			return eOwner;
		}
	}

	/*
	// TODO reimplement
	// I think this would do a tiebreaker between people on same team so that
	// players on the same team would fight less over plots that they could work.
	// So, even if player A had up to 5x the culture of player B, A would not take
	// from B if they were on the same team, were a vassal of B, and B could work/prioritize that plot.
	// Can mostly reimplement by saying instead of 'in range', just compare the distances.
	if (!isCity() && eHighestCulturePlayer != NO_PLAYER)
	{
		int iBestPriority = MAX_INT;

		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			const CvPlot* plotX = plotCity(getX(), getY(), iI);

			if (plotX != NULL)
			{
				const CvCity* cityX = plotX->getPlotCity();

				if (cityX == NULL)
				{
					continue;
				}
				const TeamTypes eBestTeam = GET_PLAYER(eHighestCulturePlayer).getTeam();

				if (cityX->getTeam() == eBestTeam || GET_TEAM(eBestTeam).isVassal(cityX->getTeam()))
				{
					const PlayerTypes ePlayerX = cityX->getOwner();

					// isWithinCultureRange(ePlayerX) would check if the tile is receiving culture from a city of that player
					// if (eHighestCulturePlayer != ePlayerX && getCulture(ePlayerX) > 0 && isWithinCultureRange(ePlayerX))
					if (eHighestCulturePlayer != ePlayerX && getCulture(ePlayerX) > 0)
					{
						const int iPriority = GC.getCityPlotPriority()[iI] + 5 * (cityX->getTeam() == eBestTeam);

						if (iPriority < iBestPriority)
						{
							iBestPriority = iPriority;
							eHighestCulturePlayer = ePlayerX;
						}
					}
				}
			}
		}
	}*/
	return eHighestCulturePlayer;
}

/*
PlayerTypes CvPlot::getPlayerWithTerritorySurroundingThisPlotCardinally() const
{
	PlayerTypes ePlayer = NO_PLAYER;

	foreach_(const CvPlot* plotX, cardinalDirectionAdjacent())
	{
		if (plotX == NULL || !plotX->isOwned() || ePlayer != NO_PLAYER && ePlayer != plotX->getOwner())
		{
			return NO_PLAYER;
		}
		if (GET_PLAYER(plotX->getOwner()).isAlive())
		{
			ePlayer = plotX->getOwner();
		}
	}
	return ePlayer;
}
*/

void CvPlot::plotAction(PlotUnitFunc func, int iData1, int iData2, PlayerTypes eOwner, TeamTypes eTeam)
{
	PROFILE_FUNC();

	foreach_(CvUnit* unitX, units())
	{
		if ((eOwner == NO_PLAYER || unitX->getOwner() == eOwner)
		&& (eTeam == NO_TEAM || unitX->getTeam() == eTeam))
		{
			func(unitX, iData1, iData2, NULL);
		}
	}
}


int CvPlot::plotCount(ConstPlotUnitFunc funcA, int iData1A, int iData2A, const CvUnit* pUnit, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B, int iRange) const
{
	PROFILE_FUNC();

	int iCount = 0;

	if (iRange == 0)
	{
		foreach_(const CvUnit* unitX, units())
		{
			if (unitX->isDead()) continue;

			if ((eOwner == NO_PLAYER || unitX->getOwner() == eOwner)
			&& (eTeam == NO_TEAM || unitX->getTeam() == eTeam)
			&& (funcA == NULL || funcA(unitX, iData1A, iData2A, pUnit))
			&& (funcB == NULL || funcB(unitX, iData1B, iData2B, NULL)))
			{
				iCount++;
			}
		}
	}
	else
	{
		// Recurse for each plot in the specified range
		foreach_(const CvPlot* plotX, rect(iRange, iRange))
		{
			iCount += plotX->plotCount(funcA, iData1A, iData2A, pUnit, eOwner, eTeam, funcB, iData1B, iData2B);
		}
	}
	return iCount;
}

int CvPlot::plotStrength(UnitValueFlags eFlags, ConstPlotUnitFunc funcA, int iData1A, int iData2A, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B, int iRange) const
{
	return plotStrengthTimes100(eFlags, funcA, iData1A, iData1B, eOwner, eTeam, funcB, iData1B, iData2B, iRange) / 100;
}

int CvPlot::plotStrengthTimes100(UnitValueFlags eFlags, ConstPlotUnitFunc funcA, int iData1A, int iData2A, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B, int iRange) const
{
	PROFILE_FUNC();

	int iStrength = 0;

	if ( iRange == 0 )
	{
		foreach_(const CvUnit* pLoopUnit, units())
		{
			if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
			{
				if ((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
				{
					if ((funcA == NULL) || funcA(pLoopUnit, iData1A, iData2A, NULL))
					{
						if ((funcB == NULL) || funcB(pLoopUnit, iData1B, iData2B, NULL))
						{
							iStrength += pLoopUnit->AI_genericUnitValueTimes100(eFlags);
						}
					}
				}
			}
		}
	}
	else
	{
		//	Recurse for each plot in the specified range
		foreach_(const CvPlot* pLoopPlot, rect(iRange, iRange))
		{
			iStrength += pLoopPlot->plotStrengthTimes100(eFlags, funcA, iData1A, iData2A, eOwner, eTeam, funcB, iData1B, iData2B, 0);
		}
	}

	return iStrength;
}


CvUnit* CvPlot::plotCheck(ConstPlotUnitFunc funcA, int iData1A, int iData2A, const CvUnit* pUnit, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(CvUnit* unitX, units())
	{
		if (unitX->isDead()) continue;

		if ((eOwner == NO_PLAYER || unitX->getOwner() == eOwner)
		&& (eTeam == NO_TEAM || unitX->getTeam() == eTeam)
		&& funcA(unitX, iData1A, iData2A, pUnit)
		&& (funcB == NULL || funcB(unitX, iData1B, iData2B, NULL)))
		{
			return unitX;
		}
	}
	return NULL;
}


bool CvPlot::isOwned() const
{
	return getOwner() != NO_PLAYER;
}

bool CvPlot::isNPC() const
{
	return getOwner() >= MAX_PC_PLAYERS;
}

bool CvPlot::isHominid() const
{
	return getOwner() == BARBARIAN_PLAYER || getOwner() == NEANDERTHAL_PLAYER;
}


bool CvPlot::isVisible(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
	{
		return true;
	}

	if (eTeam == NO_TEAM)
	{
		return false;
	}

	return getVisibilityCount(eTeam) > 0 || getStolenVisibilityCount(eTeam) > 0;
}


bool CvPlot::isActiveVisible(bool bDebug) const
{
	return isVisible(GC.getGame().getActiveTeam(), bDebug);
}


bool CvPlot::isVisibleToCivTeam() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PC_TEAMS; ++iI)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && isVisible((TeamTypes)iI, false))
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::isVisibleToWatchingHuman() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive()
		&&  GET_PLAYER((PlayerTypes)iI).isHumanPlayer()
		&& isVisible(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::isAdjacentVisible(TeamTypes eTeam, bool bDebug) const
{
	return algo::any_of(adjacent(), CvPlot::fn::isVisible(eTeam, bDebug));
}

bool CvPlot::isAdjacentNonvisible(TeamTypes eTeam) const
{
	return algo::any_of(adjacent(), !CvPlot::fn::isVisible(eTeam, false));
}


bool CvPlot::isGoody(TeamTypes eTeam) const
{
	if ((eTeam != NO_TEAM) && GET_TEAM(eTeam).isNPC())
	{
		return false;
	}

	return ((getImprovementType() == NO_IMPROVEMENT) ? false : GC.getImprovementInfo(getImprovementType()).isGoody());
}


bool CvPlot::isRevealedGoody(TeamTypes eTeam) const
{
	if (eTeam == NO_TEAM)
	{
		return isGoody();
	}

	if (GET_TEAM(eTeam).isNPC())
	{
		return false;
	}

	return ((getRevealedImprovementType(eTeam) == NO_IMPROVEMENT) ? false : GC.getImprovementInfo(getRevealedImprovementType(eTeam)).isGoody());
}


void CvPlot::removeGoody()
{
	setImprovementType(NO_IMPROVEMENT);
}


bool CvPlot::isCity(bool bCheckImprovement, TeamTypes eForTeam) const
{
	return (
		getPlotCity()
		||
		// Checking for city also includes improvement (fort) w/bCheckImprovement if:
		bCheckImprovement
	&&	isActsAsCity()
	&&	(
			// No team specified, or
			NO_TEAM == eForTeam
			// Tile with fort is on no team, or
		||	NO_TEAM == getTeam() && GC.getImprovementInfo(getImprovementType()).isOutsideBorders()
			// Fort is within friendly territory of specified team
		||	GET_TEAM(eForTeam).isFriendlyTerritory(getTeam())
		)
	);
}


bool CvPlot::isFriendlyCity(const CvUnit& kUnit, bool bCheckImprovement) const
{
	if (!isCity(bCheckImprovement, kUnit.getTeam()))
	{
		return false;
	}

	if (isVisiblePotentialEnemyDefender(&kUnit))
	{
		return false;
	}

	TeamTypes ePlotTeam = getTeam();

	if (NO_TEAM != ePlotTeam)
	{
		if (kUnit.isEnemy(ePlotTeam))
		{
			return false;
		}

		TeamTypes eTeam = GET_PLAYER(kUnit.getCombatOwner(ePlotTeam, this)).getTeam();

		if (eTeam == ePlotTeam)
		{
			return true;
		}

		if (GET_TEAM(eTeam).isOpenBorders(ePlotTeam))
		{
			return true;
		}

		if (GET_TEAM(ePlotTeam).isVassal(eTeam))
		{
			return true;
		}
	}

	return false;
}


// Afforess - Vicinity Bonus - 07/29/09
bool CvPlot::isHasValidBonus() const
{
	if (getBonusType() == NO_BONUS
	|| getImprovementType() == NO_IMPROVEMENT
	|| !isBonusNetwork(getTeam())
	|| !isWithinTeamCityRadius(getTeam()))
	{
		return false;
	}
	const CvBonusInfo& bonus = GC.getBonusInfo(getBonusType());
	return (
		GET_TEAM(getTeam()).isHasTech((TechTypes)bonus.getTechReveal())
		&& (
				bonus.isProvidedByImprovementType(getImprovementType())
			||	GET_TEAM(getTeam()).isHasTech((TechTypes)bonus.getTechObsolete())
		)
	);
}
// ! Afforess - Vicinity Bonus - 07/29/09


bool CvPlot::isEnemyCity(const CvUnit& kUnit, bool bOnlyRealCities) const
{
	return isCity(!bOnlyRealCities) && getTeam() != NO_TEAM && kUnit.isEnemy(getTeam(), this);
}


bool CvPlot::isOccupation() const
{
	const CvCity* pCity = getPlotCity();

	return pCity ? pCity->isOccupation() : false;
}


bool CvPlot::isBeingWorked() const
{
	const CvCity* pWorkingCity = getWorkingCity();

	return pWorkingCity ? pWorkingCity->isWorkingPlot(this) : false;
}


bool CvPlot::isUnit() const
{
	return (getNumUnits() > 0);
}


bool CvPlot::isInvestigate(TeamTypes eTeam) const
{
	return (plotCheck(PUF_isInvestigate, -1, -1, NULL, NO_PLAYER, eTeam) != NULL);
}


bool CvPlot::isVisibleEnemyDefender(const CvUnit* pUnit) const
{
	PROFILE_FUNC();

	return isVisible(pUnit->getTeam(), false) && hasDefender(false, NO_PLAYER, pUnit->getOwner(), pUnit, true, false, false, true);
}


CvUnit *CvPlot::getVisibleEnemyDefender(PlayerTypes ePlayer) const
{
	return plotCheck(PUF_canDefendEnemy, ePlayer, 0, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer);
}


int CvPlot::getNumDefenders(PlayerTypes ePlayer) const
{
	return plotCount(PUF_canDefend, -1, -1, NULL, ePlayer);
}

int CvPlot::getNumAttackers(PlayerTypes ePlayer) const
{
	return plotCount(PUF_canDefend, -1, -1, NULL, ePlayer);
}


int CvPlot::getNumVisibleEnemyDefenders(const CvUnit* pUnit) const
{
	return plotCount(PUF_canDefendEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(this), NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
}


int CvPlot::getNumCriminals() const
{
	return plotCount(PUF_isCriminal);
}

int CvPlot::getNumPlayerUnitAI(UnitAITypes eUnitAI, PlayerTypes ePlayer) const
{
	return plotCount(PUF_isUnitAIType, eUnitAI, -1, NULL, ePlayer);
}

int CvPlot::getNumVisibleWantedCriminals(PlayerTypes ePlayer /*visible to this player*/) const
{
	return plotCount(PUF_isWantedCriminal, -1, -1, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer);
}

int CvPlot::getNumVisiblePotentialEnemyDefenders(const CvUnit* pUnit) const
{
	PROFILE_FUNC();

	if (isVisible(pUnit->getTeam(), false))
	{
		return plotCount(PUF_canDefendPotentialEnemyAgainst, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
	}
	return 0;
}

int CvPlot::getNumVisiblePotentialEnemyDefenderless(const CvUnit* pUnit) const
{
	PROFILE_FUNC();

	if (isVisible(pUnit->getTeam(), false))
	{
		return plotCount(PUF_canDefenselessPotentialEnemyAgainst, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
	}
	return 0;
}

bool CvPlot::isVisiblePotentialEnemyDefender(const CvUnit* pUnit) const
{
	PROFILE_FUNC();

	if (pUnit && isVisible(pUnit->getTeam(), false))
	{
		return (plotCheck(PUF_canDefendPotentialEnemyAgainst, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner()) != NULL);
	}
	return false;
}

bool CvPlot::isVisiblePotentialEnemyDefenderless(const CvUnit* pUnit) const
{
	PROFILE_FUNC();

	if (pUnit != NULL && isVisible(pUnit->getTeam(), false))
	{
		return (plotCheck(PUF_canDefenselessPotentialEnemyAgainst, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner()) != NULL);
	}
	return false;
}

bool CvPlot::isVisibleEnemyUnit(PlayerTypes ePlayer) const
{
	return plotCheck(PUF_isEnemy, ePlayer, 0, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer) != NULL;
}

int CvPlot::getNumVisibleUnits(PlayerTypes ePlayer) const
{
	return plotCount(PUF_isVisibleDebug, ePlayer);
}

int CvPlot::getNumVisibleEnemyUnits(PlayerTypes ePlayer) const
{
	return plotCount(PUF_isEnemy, ePlayer, 0, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer);
}

int CvPlot::getNumVisibleEnemyCombatUnits(PlayerTypes ePlayer) const
{
	PROFILE_FUNC();

	int iCount = 0;
	foreach_(const CvUnit* unitX, units())
	{
		if (!unitX->isDead()
		&& (unitX->canAttack() || unitX->canDefend())
		&& PUF_isEnemy(unitX, ePlayer, 0)
		&& PUF_isVisible(unitX, ePlayer))
		{
			iCount++;
		}
	}
	return iCount;
}

int CvPlot::getNumVisibleEnemyUnits(const CvUnit* pUnit) const
{
	return plotCount(PUF_isEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(this), NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
}

int CvPlot::getNumVisibleEnemyTargetUnits(const CvUnit* pUnit) const
{
	return plotCount(PUF_isEnemyTarget, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
}

int CvPlot::getVisibleEnemyStrength(PlayerTypes ePlayer, int iRange) const
{
	return plotStrength((UnitValueFlags)(UNITVALUE_FLAGS_OFFENSIVE | UNITVALUE_FLAGS_DEFENSIVE), PUF_isEnemy, ePlayer, 0, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer, -1, iRange);
}

int CvPlot::getVisibleNonAllyStrength(PlayerTypes ePlayer) const
{
	return plotStrength((UnitValueFlags)(UNITVALUE_FLAGS_OFFENSIVE | UNITVALUE_FLAGS_DEFENSIVE), PUF_isNonAlly, ePlayer, 0, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer);
}

bool CvPlot::isVisibleEnemyUnit(const CvUnit* pUnit) const
{
	return isVisible(pUnit->getTeam(), false) && plotCheck(PUF_isEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
}

bool CvPlot::isVisibleOtherUnit(PlayerTypes ePlayer) const
{
	return (plotCheck(PUF_isOtherTeam, ePlayer, -1, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer) != NULL);
}


/*DllExport*/ bool CvPlot::isFighting() const
{
#ifdef _DEBUG
	OutputDebugString("exe is asking if there's a unit in battle on this plot\n");
#endif
	return plotCheck(PUF_isInBattle) != NULL;
}

bool CvPlot::isBattle() const
{
	return plotCheck(PUF_isInBattle) != NULL;
}


bool CvPlot::canHaveFeature(FeatureTypes eFeature, bool bOverExistingFeature) const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (eFeature == NO_FEATURE)
	{
		return true;
	}
	if (!bOverExistingFeature
	&& getFeatureType() != NO_FEATURE
	|| getImprovementType() != NO_IMPROVEMENT
	&& !GC.getImprovementInfo(getImprovementType()).getFeatureMakesValid(eFeature)
	|| isPeak())
	{
		return false;
	}
	const CvFeatureInfo& feature = GC.getFeatureInfo(eFeature);

	if (feature.isNoCity() && isCity()
	|| !feature.isTerrain(getTerrainType())
	||  feature.isNoBonus() && getBonusType() != NO_BONUS
	||  feature.isRequiresFlatlands() && isHills()
	||  feature.isNoCoast() && isCoastal()
	||  feature.isCoastalOnly() && !isCoastal()
	||  feature.isNoRiver() && isRiver()
	||  feature.isRequiresRiver() && !isRiver()
	||  feature.isNoAdjacent()
	&& algo::any_of(adjacent(), CvPlot::fn::getFeatureType() == eFeature))
	{
		return false;
	}
	return true;
}


bool CvPlot::isRoute() const
{
	return getRouteType() != NO_ROUTE;
}

//Check is to determine if there's a way for a sea unit to be on the plot.
bool CvPlot::isCanMoveSeaUnits() const
{
	return isWater() || getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType()).isCanMoveSeaUnits();
}

bool CvPlot::isSeaTunnel() const
{
	return isRoute() && GC.getRouteInfo(getRouteType()).isSeaTunnel();
}

bool CvPlot::isValidRoute(const CvUnit* pUnit) const
{
	if (isRoute())
	{
		TeamTypes eTeam = getTeam();

		if (eTeam == NO_TEAM || GET_TEAM(pUnit->getTeam()).isFriendlyTerritory(eTeam) || GET_TEAM(pUnit->getTeam()).isOpenBorders(eTeam) || pUnit->isEnemyRoute())
		{
			if (pUnit->getDomainType() == DOMAIN_SEA && isSeaTunnel())
			{
				return false;
			}
			return true;
		}
	}
	return false;
}


bool CvPlot::isTradeNetworkImpassable(TeamTypes eTeam) const
{
	return isImpassable(eTeam) && !isRiverNetwork(eTeam) && !isRoute();
}

bool CvPlot::isRiverNetwork(TeamTypes eTeam) const
{
	if (!isRiver())
	{
		return false;
	}

	if (GET_TEAM(eTeam).isRiverTrade())
	{
		return true;
	}

	if (getTeam() == eTeam)
	{
		return true;
	}

	return false;
}

bool CvPlot::isNetworkTerrain(TeamTypes eTeam) const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (eTeam == NO_TEAM)
	{
		FErrorMsg("eTeam is not assigned a valid value");
		return false;
	}

	return GET_TEAM(eTeam).isTerrainTrade(getTerrainType())
		|| (isWater() && getTeam() == eTeam);
}


bool CvPlot::isBonusNetwork(TeamTypes eTeam) const
{
	return isRoute()
		|| isRiverNetwork(eTeam)
		|| isNetworkTerrain(eTeam);
}


bool CvPlot::isTradeNetwork(TeamTypes eTeam) const
{
	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");

	if (atWar(eTeam, getTeam()))
	{
		return false;
	}

	if (getBlockadedCount(eTeam) > 0)
	{
		return false;
	}

	if (isTradeNetworkImpassable(eTeam))
	{
		return false;
	}

	if (!isOwned() && !isRevealed(eTeam, false))
	{
		return false;
	}

	return isBonusNetwork(eTeam);
}


bool CvPlot::isTradeNetworkConnected(const CvPlot* pPlot, TeamTypes eTeam) const
{
	FAssertMsg(eTeam != NO_TEAM, "eTeam is not assigned a valid value");

	if (atWar(eTeam, getTeam()) || atWar(eTeam, pPlot->getTeam()))
	{
		return false;
	}

	if (getBlockadedCount(eTeam) > 0)
	{
		return false;
	}

	if (isTradeNetworkImpassable(eTeam) || pPlot->isTradeNetworkImpassable(eTeam))
	{
		return false;
	}

	if (!isOwned() && (!isRevealed(eTeam, false) || !pPlot->isRevealed(eTeam, false)))
	{
		return false;
	}

	if (isRoute() && pPlot->isRoute())
	{
		return true;
	}

	if (isCity(true, eTeam) && pPlot->isNetworkTerrain(eTeam))
	{
		return true;
	}

	if (isNetworkTerrain(eTeam))
	{
		if (pPlot->isCity(true, eTeam))
		{
			return true;
		}

		if (pPlot->isNetworkTerrain(eTeam))
		{
			return true;
		}

		if (pPlot->isRiverNetwork(eTeam) && pPlot->isRiverConnection(directionXY(pPlot, this)))
		{
			return true;
		}
	}

	if (isRiverNetwork(eTeam))
	{
		if (pPlot->isNetworkTerrain(eTeam) && isRiverConnection(directionXY(this, pPlot)))
		{
			return true;
		}

		if (pPlot->isRiverNetwork(eTeam)
		&& (isRiverConnection(directionXY(this, pPlot)) || pPlot->isRiverConnection(directionXY(pPlot, this))))
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::isValidDomainForLocation(const CvUnit& unit) const
{
	return isValidDomainForAction(unit) || isCity(true, unit.getTeam());
}


bool CvPlot::isValidDomainForAction(const CvUnit& unit) const
{
	switch (unit.getDomainType())
	{
		case DOMAIN_SEA:
			return (isWater() || unit.canMoveAllTerrain() || isCanMoveSeaUnits());

		case DOMAIN_AIR:
			return false;

		case DOMAIN_LAND:
			return (!isWater() || unit.canMoveAllTerrain() || isSeaTunnel());

		case DOMAIN_IMMOBILE:
			return (!isWater() || unit.canMoveAllTerrain());

		default:
			FErrorMsg("error");
			return false;
	}
}


bool CvPlot::isImpassable(TeamTypes eTeam) const
{
	if (isAsPeak())
	{
		if (eTeam != NO_TEAM && GET_TEAM(eTeam).isCanPassPeaks())
		{
			return false;
		}
		return true;
	}
	if (getTerrainType() == NO_TERRAIN)
	{
		return false;
	}
	return getFeatureType() == NO_FEATURE ? GC.getTerrainInfo(getTerrainType()).isImpassable() : GC.getFeatureInfo(getFeatureType()).isImpassable();
}

int CvPlot::getViewportX() const
{
	const CvViewport* pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);

	return pCurrentViewPort->getViewportXFromMapX(m_iX);
}


int CvPlot::getViewportY() const
{
	const CvViewport* pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);

	return pCurrentViewPort->getViewportYFromMapY(m_iY);
}

bool CvPlot::isInViewport(int comfortBorderSize) const
{
	const CvViewport* pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);

	return pCurrentViewPort->isInViewport(m_iX, m_iY, comfortBorderSize);
}


bool CvPlot::at(int iX, int iY) const
{
	return ((getX() == iX) && (getY() == iY));
}


// BUG - Lat/Long Coordinates - start
#define MINUTES_PER_DEGREE	60
#define MIN_LONGITUDE		-180
#define MAX_LONGITUDE		180

int CvPlot::calculateMinutes(int iPlotIndex, int iPlotCount, bool bWrap, int iDegreeMin, int iDegreeMax) const
{
	if (!bWrap)
	{
		iPlotCount--;
	}
	return iPlotIndex * (iDegreeMax - iDegreeMin) * MINUTES_PER_DEGREE / iPlotCount + iDegreeMin * MINUTES_PER_DEGREE;
}

int CvPlot::getLongitudeMinutes() const
{
	const CvMap& kMap = GC.getMap();

	if (kMap.isWrapX())
	{
		// normal and toroidal
		return calculateMinutes(getX(), kMap.getGridWidth(), true, MIN_LONGITUDE, MAX_LONGITUDE);
	}
	else if (!kMap.isWrapY())
	{
		// flat
		return calculateMinutes(getX(), kMap.getGridWidth(), false, MIN_LONGITUDE, MAX_LONGITUDE);
	}
	else
	{
		// tilted axis
		return calculateMinutes(getY(), kMap.getGridHeight(), true, MIN_LONGITUDE, MAX_LONGITUDE);
	}
}

int CvPlot::getLatitudeMinutes() const
{
	const CvMap& kMap = GC.getMap();

	if (kMap.isWrapX())
	{
		// normal and toroidal
		return calculateMinutes(getY(), kMap.getGridHeight(), kMap.isWrapY(), kMap.getBottomLatitude(), kMap.getTopLatitude());
	}
	else if (!kMap.isWrapY())
	{
		// flat
		return calculateMinutes(getY(), kMap.getGridHeight(), false, kMap.getBottomLatitude(), kMap.getTopLatitude());
	}
	else
	{
		// tilted axis
		return calculateMinutes(getX(), kMap.getGridWidth(), false, kMap.getBottomLatitude(), kMap.getTopLatitude());
	}
}

int CvPlot::getLatitude() const
{
	return abs(getLatitudeMinutes() / MINUTES_PER_DEGREE);
}

int CvPlot::getLatitudeRaw() const
{
	return getLatitudeMinutes() / MINUTES_PER_DEGREE;
}

int CvPlot::getLongitude() const
{
	return abs(getLongitudeMinutes() / MINUTES_PER_DEGREE);
}

int CvPlot::getLongitudeRaw() const
{
	return getLongitudeMinutes() / MINUTES_PER_DEGREE;
}
// BUG - Lat/Long Coordinates - end


int CvPlot::getFOWIndex() const
{
	const CvViewport* pCurrentViewport = GC.getCurrentViewport();

	return ((((pCurrentViewport->getGridHeight() - 1) - pCurrentViewport->getViewportYFromMapY(getY())) * pCurrentViewport->getGridWidth() * LANDSCAPE_FOW_RESOLUTION * LANDSCAPE_FOW_RESOLUTION) + (pCurrentViewport->getViewportXFromMapX(getX()) * LANDSCAPE_FOW_RESOLUTION));
}


CvArea* CvPlot::area() const
{
	if(m_pPlotArea == NULL)
	{
		m_pPlotArea = GC.getMap().getArea(getArea());
	}

	return m_pPlotArea;
}


CvArea* CvPlot::waterArea(bool bNoImpassable) const
{
	PROFILE_EXTRA_FUNC();
	if (isWater())
	{
		return area();
	}
	int iBestValue = 0;
	CvArea* pBestArea = NULL;

	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		if (pAdjacentPlot->isWater() && (!bNoImpassable || !pAdjacentPlot->isImpassable()))
		{
			const int iValue = pAdjacentPlot->area()->getNumTiles();

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestArea = pAdjacentPlot->area();
			}
		}
	}
	return pBestArea;
}

CvArea* CvPlot::secondWaterArea() const
{
	PROFILE_EXTRA_FUNC();
	FAssert(!isWater());

	const int iWaterArea = waterArea() != NULL ? waterArea()->getID() : FFreeList::INVALID_INDEX;
	int iBestValue = 0;
	CvArea* pBestArea = NULL;

	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		if (pAdjacentPlot->isWater() && pAdjacentPlot->getArea() != iWaterArea)
		{
			const int iValue = pAdjacentPlot->area()->getNumTiles();

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				pBestArea = pAdjacentPlot->area();
			}
		}
	}
	return pBestArea;
}


int CvPlot::getArea() const
{
	return m_iArea;
}


void CvPlot::setArea(int iNewValue)
{
	if (getArea() != iNewValue)
	{
		if (area() != NULL)
		{
			processArea(area(), -1);
		}

		m_iArea = iNewValue;
		m_pPlotArea = NULL;

		if (area() != NULL)
		{
			processArea(area(), 1);

			updateIrrigated();
			updateYield();
		}
	}
}


int CvPlot::getFeatureVariety() const
{
	FAssert((getFeatureType() == NO_FEATURE) || (m_iFeatureVariety < GC.getFeatureInfo(getFeatureType()).getArtInfo()->getNumVarieties()));
	FASSERT_NOT_NEGATIVE(m_iFeatureVariety);
	return m_iFeatureVariety;
}


int CvPlot::getOwnershipDuration() const
{
	return m_iOwnershipDuration;
}


bool CvPlot::isOwnershipScore() const
{
	return m_iOwnershipDuration >= GC.getDefineINT("OWNERSHIP_SCORE_DURATION_THRESHOLD");
}


void CvPlot::setOwnershipDuration(int iNewValue)
{
	if (m_iOwnershipDuration != iNewValue)
	{
		const bool bOldOwnershipScore = isOwnershipScore();

		FASSERT_NOT_NEGATIVE(iNewValue);
		m_iOwnershipDuration = iNewValue;

		if (bOldOwnershipScore != isOwnershipScore() && isOwned() && !isWater())
		{
			GET_PLAYER(getOwner()).changeTotalLandScored(isOwnershipScore() ? 1 : -1);
		}
	}
}


int CvPlot::getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const
{
	const int iRemaining =
	(
		100 * GC.getGame().getImprovementUpgradeTime(eImprovement)
		-
		((getImprovementType() == eImprovement) ? getImprovementUpgradeProgress() : 0)
	);
	const int iUpgradeRate =
	(
		ePlayer == NO_PLAYER ? 100
		:
		GET_PLAYER(ePlayer).getImprovementUpgradeProgressRate(eImprovement)
	);
	const int iTurns = iRemaining / iUpgradeRate;

	if (iTurns * iUpgradeRate < iRemaining)
	{
		return iTurns + 1;
	}
	return iTurns;
}


int CvPlot::getImprovementUpgradeProgress() const
{
	return m_iUpgradeProgress;
}

void CvPlot::setImprovementUpgradeProgress(int iNewValue)
{
	m_iUpgradeProgress = iNewValue;
	FASSERT_NOT_NEGATIVE(m_iUpgradeProgress);
}

void CvPlot::changeImprovementUpgradeProgress(int iChange)
{
	setImprovementUpgradeProgress(m_iUpgradeProgress + iChange);
}


int CvPlot::getCityRadiusCount() const
{
	return m_iCityRadiusCount;
}


int CvPlot::isCityRadius() const
{
	return (getCityRadiusCount() > 0);
}


void CvPlot::changeCityRadiusCount(int iChange)
{
	m_iCityRadiusCount += iChange;
	FASSERT_NOT_NEGATIVE(getCityRadiusCount());
}


bool CvPlot::isStartingPlot() const
{
	return m_bStartingPlot;
}


void CvPlot::setStartingPlot(bool bNewValue)
{
	m_bStartingPlot = bNewValue;
}


bool CvPlot::isNOfRiver() const
{
	return m_bNOfRiver;
}


void CvPlot::setNOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir)
{
	PROFILE_EXTRA_FUNC();
	if (isNOfRiver() != bNewValue || eRiverDir != m_eRiverWEDirection)
	{
		if (isNOfRiver() != bNewValue)
		{
			updatePlotGroupBonus(false);
			m_bNOfRiver = bNewValue;
			updatePlotGroupBonus(true);

			updateRiverCrossing();
			updateYield();

			foreach_(CvPlot* pAdjacentPlot, adjacent())
			{
				pAdjacentPlot->updateRiverCrossing();
				pAdjacentPlot->updateYield();
			}

			if (area() != NULL)
			{
				area()->changeNumRiverEdges((isNOfRiver()) ? 1 : -1);
			}
		}

		FAssertMsg(eRiverDir == CARDINALDIRECTION_WEST || eRiverDir == CARDINALDIRECTION_EAST || eRiverDir == NO_CARDINALDIRECTION, "invalid parameter");

		if (m_eRiverWEDirection != NO_CARDINALDIRECTION)
		{
			m_movementCharacteristicsHash ^= g_riverDirectionZobristHashes[m_eRiverWEDirection];
		}
		if (eRiverDir != NO_CARDINALDIRECTION)
		{
			m_movementCharacteristicsHash ^= g_riverDirectionZobristHashes[eRiverDir];
		}

		m_eRiverWEDirection = eRiverDir;

		updateRiverSymbol(true, true);
	}
}


bool CvPlot::isWOfRiver() const
{
	return m_bWOfRiver;
}


void CvPlot::setWOfRiver(bool bNewValue, CardinalDirectionTypes eRiverDir)
{
	PROFILE_EXTRA_FUNC();
	if (isWOfRiver() != bNewValue || eRiverDir != m_eRiverNSDirection)
	{
		if (isWOfRiver() != bNewValue)
		{
			updatePlotGroupBonus(false);
			m_bWOfRiver = bNewValue;
			updatePlotGroupBonus(true);

			updateRiverCrossing();
			updateYield();

			foreach_(CvPlot* pAdjacentPlot, adjacent())
			{
				pAdjacentPlot->updateRiverCrossing();
				pAdjacentPlot->updateYield();
			}

			if (area())
			{
				area()->changeNumRiverEdges((isWOfRiver()) ? 1 : -1);
			}
		}

		FAssertMsg(eRiverDir == CARDINALDIRECTION_NORTH || eRiverDir == CARDINALDIRECTION_SOUTH || eRiverDir == NO_CARDINALDIRECTION, "invalid parameter");

		if (m_eRiverNSDirection != NO_CARDINALDIRECTION)
		{
			m_movementCharacteristicsHash ^= g_riverDirectionZobristHashes[m_eRiverNSDirection];
		}
		if (eRiverDir != NO_CARDINALDIRECTION)
		{
			m_movementCharacteristicsHash ^= g_riverDirectionZobristHashes[eRiverDir];
		}

		m_eRiverNSDirection = eRiverDir;

		updateRiverSymbol(true, true);
	}
}


CardinalDirectionTypes CvPlot::getRiverNSDirection() const
{
	return (CardinalDirectionTypes)m_eRiverNSDirection;
}


CardinalDirectionTypes CvPlot::getRiverWEDirection() const
{
	return (CardinalDirectionTypes)m_eRiverWEDirection;
}


// This function finds an *inland* corner of this plot at which to place a river.
// It then returns the plot with that corner at its SE.

CvPlot* CvPlot::getInlandCorner() const
{
	PROFILE_EXTRA_FUNC();
	CvPlot* pRiverPlot = NULL; // will be a plot through whose SE corner we want the river to run
	int aiShuffle[4];

	shuffleArray(aiShuffle, 4, GC.getGame().getMapRand());

	for (int iI = 0; iI < 4; ++iI)
	{
		switch (aiShuffle[iI])
		{
		case 0:
			pRiverPlot = GC.getMap().plotSorenINLINE(getX(), getY()); break;
		case 1:
			pRiverPlot = plotDirection(getX(), getY(), DIRECTION_NORTH); break;
		case 2:
			pRiverPlot = plotDirection(getX(), getY(), DIRECTION_NORTHWEST); break;
		case 3:
			pRiverPlot = plotDirection(getX(), getY(), DIRECTION_WEST); break;
		}
		if (pRiverPlot != NULL && !pRiverPlot->hasCoastAtSECorner())
		{
			break;
		}
		else
		{
			pRiverPlot = NULL;
		}
	}

	return pRiverPlot;
}


bool CvPlot::hasCoastAtSECorner() const
{
	CvPlot* pAdjacentPlot;

	if (isWater())
	{
		return true;
	}

	pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_EAST);
	if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
	{
		return true;
	}

	pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_SOUTHEAST);
	if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
	{
		return true;
	}

	pAdjacentPlot = plotDirection(getX(), getY(), DIRECTION_SOUTH);
	if (pAdjacentPlot != NULL && pAdjacentPlot->isWater())
	{
		return true;
	}

	return false;
}


bool CvPlot::isIrrigated() const
{
	return m_bIrrigated;
}


void CvPlot::setIrrigated(bool bNewValue)
{
	PROFILE_EXTRA_FUNC();
	if (isIrrigated() != bNewValue)
	{
		m_bIrrigated = bNewValue;
		foreach_(CvPlot* pLoopPlot, rect(1, 1))
		{
			pLoopPlot->updateYield();
			pLoopPlot->setLayoutDirty(true);
		}
	}
}


void CvPlot::updateIrrigated()
{
	PROFILE("CvPlot::updateIrrigated()");

	if (area() == NULL || !GC.getGame().isFinalInitialized())
	{
		return;
	}
	FAStar* pIrrigatedFinder = gDLL->getFAStarIFace()->create();

	if (isIrrigated())
	{
		if (!isPotentialIrrigation())
		{
			setIrrigated(false);

			foreach_(const CvPlot* pLoopPlot, adjacent())
			{
				bool bFoundFreshWater = false;
				gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), NULL, NULL, NULL, potentialIrrigation, NULL, checkFreshWater, &bFoundFreshWater);
				gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, pLoopPlot->getX(), pLoopPlot->getY(), -1, -1);

				if (!bFoundFreshWater)
				{
					bool bIrrigated = false;
					gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), NULL, NULL, NULL, potentialIrrigation, NULL, changeIrrigated, &bIrrigated);
					gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, pLoopPlot->getX(), pLoopPlot->getY(), -1, -1);
				}
			}
		}
	}
	else if (isPotentialIrrigation() && isIrrigationAvailable(true))
	{
		bool bIrrigated = true;
		gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), NULL, NULL, NULL, potentialIrrigation, NULL, changeIrrigated, &bIrrigated);
		gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, getX(), getY(), -1, -1);
	}
	gDLL->getFAStarIFace()->destroy(pIrrigatedFinder);
}


bool CvPlot::isPotentialCityWork() const
{
	return m_bPotentialCityWork;
}

void CvPlot::updatePotentialCityWork()
{
	PROFILE_FUNC();

	bool bValid = false;
	for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);

		if (pLoopPlot != NULL && !pLoopPlot->isWater())
		{
			bValid = true;
			break;
		}
	}
	if (m_bPotentialCityWork != bValid)
	{
		m_bPotentialCityWork = bValid;
		updateYield();
	}
}


bool CvPlot::isPotentialCityWorkForArea(const CvArea* pArea) const
{
	PROFILE_FUNC();
	const bool bWaterValid = GC.getWATER_POTENTIAL_CITY_WORK_FOR_AREA() > 0;

	for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		const CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);

		if (pLoopPlot != NULL && (bWaterValid || !pLoopPlot->isWater()) && pLoopPlot->area() == pArea)
		{
			return true;
		}
	}
	return false;
}


bool CvPlot::isShowCitySymbols() const
{
	return m_bShowCitySymbols;
}

void CvPlot::updateShowCitySymbols()
{
	PROFILE_FUNC();

	bool bNewShowCitySymbols = false;

	if (gDLL->getInterfaceIFace()->isCityScreenUp())
	{
		const int iX = getX();
		const int iY = getY();

		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			const CvPlot* plotX = plotCity(iX, iY, iI);

			if (plotX)
			{
				const CvCity* cityX = plotX->getPlotCity();

				if (cityX && cityX->isCitySelected() && cityX->canWork(this))
				{
					bNewShowCitySymbols = true;
					break;
				}
			}
		}
	}
	if (m_bShowCitySymbols != bNewShowCitySymbols)
	{
		m_bShowCitySymbols = bNewShowCitySymbols;
		updateSymbolVisibility();
	}
}


bool CvPlot::isFlagDirty() const
{
	return m_bFlagDirty;
}

void CvPlot::setFlagDirty(bool bNewValue)
{
	m_bFlagDirty = bNewValue;
}


void CvPlot::setOwner(PlayerTypes eNewValue, bool bCheckUnits, bool bUpdatePlotGroup)
{
	PROFILE_FUNC();

	if (getOwner() != eNewValue)
	{
		PROFILE("CvPlot::setOwner.changed");

		GC.getGame().addReplayMessage(REPLAY_MESSAGE_PLOT_OWNER_CHANGE, eNewValue, (char*)NULL, getX(), getY());

		CvCity* pOldCity = getPlotCity();

		if (pOldCity != NULL)
		{
			if (isInViewport())
			{
				const CvWString szBuffer =
				(
					gDLL->getText(
						"TXT_KEY_MISC_CITY_REVOLTED_JOINED",
						pOldCity->getNameKey(),
						GET_PLAYER(eNewValue).getCivilizationDescriptionKey()
					)
				);
				AddDLLMessage(
					getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,
					"AS2D_CULTUREFLIP", MESSAGE_TYPE_MAJOR_EVENT,
					ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(),
					GC.getCOLOR_RED(), getViewportX(),getViewportY(), true, true
				);
				AddDLLMessage(
					eNewValue, false, GC.getEVENT_MESSAGE_TIME(), szBuffer,
					"AS2D_CULTUREFLIP", MESSAGE_TYPE_MAJOR_EVENT,
					ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(),
					GC.getCOLOR_GREEN(), getViewportX(),getViewportY(), true, true
				);
			}
			GC.getGame().addReplayMessage(
				REPLAY_MESSAGE_MAJOR_EVENT, getOwner(),
				gDLL->getText(
					"TXT_KEY_MISC_CITY_REVOLTS_JOINS",
					pOldCity->getNameKey(),
					GET_PLAYER(eNewValue).getCivilizationDescriptionKey()
				),
				getX(), getY(), GC.getCOLOR_ALT_HIGHLIGHT_TEXT()
			);
			FAssertMsg(pOldCity->getOwner() != eNewValue, "pOldCity->getOwner() is not expected to be equal with eNewValue");
			GET_PLAYER(eNewValue).acquireCity(pOldCity, false, false, bUpdatePlotGroup); // will delete the pointer
			pOldCity = NULL;
			CvCity* pNewCity = getPlotCity();

			// pNewCity can be null if acquireCity causes autoraze (e.g. pop 1, 1-city-challenge, etc)
			// FAssertMsg(pNewCity != NULL, "NewCity is not assigned a valid value");

			if (pNewCity != NULL)
			{
				algo::for_each(units() | filtered(CvUnit::fn::isEnemy(GET_PLAYER(eNewValue).getTeam())),
					CvUnit::fn::jumpToNearestValidPlot(true)
				);

				int iDummyValue;

				UnitTypes eBestUnit = pNewCity->AI_bestUnitAI(UNITAI_CITY_DEFENSE, iDummyValue);

				if (eBestUnit == NO_UNIT)
				{
					eBestUnit = pNewCity->AI_bestUnitAI(UNITAI_ATTACK, iDummyValue);
				}

				if (eBestUnit != NO_UNIT)
				{
					const int iFreeUnits = GC.getDefineINT("BASE_REVOLT_FREE_UNITS") + pNewCity->getHighestPopulation() * GC.getDefineINT("REVOLT_FREE_UNITS_PERCENT") / 100;

					for (int iI = 0; iI < iFreeUnits; ++iI)
					{
						GET_PLAYER(eNewValue).initUnit(eBestUnit, getX(), getY(), UNITAI_CITY_DEFENSE, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
					}
				}
			}
		}
		else
		{
			setOwnershipDuration(0);

			// Remove effects from old owner of tile (Keep symmetry with below where needed!)
			if (isOwned())
			{
				changeAdjacentSight(getTeam(), 1, false, NULL, bUpdatePlotGroup);

				if (area())
				{
					area()->changeNumOwnedTiles(-1);
				}
				GC.getMap().changeOwnedPlots(-1);

				if (!isWater())
				{
					GET_PLAYER(getOwner()).changeTotalLand(-1);
					GET_TEAM(getTeam()).changeTotalLand(-1);

					if (isOwnershipScore())
					{
						GET_PLAYER(getOwner()).changeTotalLandScored(-1);
					}
				}

				if (getImprovementType() != NO_IMPROVEMENT)
				{
					GET_PLAYER(getOwner()).changeImprovementCount(getImprovementType(), -1);
				}

				updatePlotGroupBonus(false);
			}

			foreach_(CvUnit* pLoopUnit, units())
			{
				if (pLoopUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam())))
				{
					GET_PLAYER(pLoopUnit->getOwner()).changeNumOutsideUnits(-1);
				}

				if (pLoopUnit->isBlockading())
				{
					pLoopUnit->setBlockading(false);
					pLoopUnit->getGroup()->clearMissionQueue();
					pLoopUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
				}
			}

			m_eOwner = eNewValue;

			setWorkingCityOverride(NULL);
			updateWorkingCity();

			// Add same effects to new owner of tile (Keep symmetry with above!)
			if (isOwned())
			{
				changeAdjacentSight(getTeam(), 1, true, NULL, bUpdatePlotGroup);

				if (area())
				{
					area()->changeNumOwnedTiles(1);
				}
				GC.getMap().changeOwnedPlots(1);

				if (!isWater())
				{
					GET_PLAYER(getOwner()).changeTotalLand(1);
					GET_TEAM(getTeam()).changeTotalLand(1);

					if (isOwnershipScore())
					{
						GET_PLAYER(getOwner()).changeTotalLandScored(1);
					}
				}

				if (getImprovementType() != NO_IMPROVEMENT)
				{
					GET_PLAYER(getOwner()).changeImprovementCount(getImprovementType(), 1);
					if (GC.getImprovementInfo(getImprovementType()).getCulture() > 0)
					{
						pushCultureFromImprovement(getOwner(), 1, GC.getImprovementInfo(getImprovementType()).getCultureRange(), true);
					}
				}

				updatePlotGroupBonus(true);
			}

			foreach_(const CvUnit* pLoopUnit, units())
			{
				if (pLoopUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam())))
				{
					GET_PLAYER(pLoopUnit->getOwner()).changeNumOutsideUnits(1);
				}
			}

			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					updateRevealedOwner((TeamTypes)iI);
				}
			}

			updateIrrigated();
			updateYield();

			if (bUpdatePlotGroup)
			{
				updatePlotGroup();
			}

			if (bCheckUnits)
			{
				verifyUnitValidPlot();
			}

			if (isOwned() && !GET_PLAYER(getOwner()).isNPC())
			{
				if (isGoody())
				{
					GET_PLAYER(getOwner()).doGoody(this, NULL);
				}

				for (int iI = 0; iI < MAX_PC_TEAMS; ++iI)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive() && isVisible((TeamTypes)iI, false))
					{
						GET_TEAM((TeamTypes)iI).meet(getTeam(), true);
					}
				}
			}

			if (GC.getGame().isDebugMode())
			{
				updateMinimapColor();

				gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

				gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
			}
		}

		// Plot danger cache
		algo::for_each(rect(2, 2), bind(&CvPlot::invalidateBorderDangerCache, _1));

		updateSymbols();
	}
}


PlotTypes CvPlot::getPlotType() const
{
	return (PlotTypes)m_ePlotType;
}

bool CvPlot::isWaterExternal() const
{
	return
	(
		isWater()
		&&
		(
			!GC.viewportsEnabled()
			|| isRevealed(GC.getGame().getActiveTeam(), true)
			|| isAdjacentRevealed(GC.getGame().getActiveTeam(), true)
		)
	);
}

bool CvPlot::isWater() const
{
	return getPlotType() == PLOT_OCEAN;
}

bool CvPlot::isFlatlands() const
{
	return getPlotType() == PLOT_LAND;
}

bool CvPlot::isHills() const
{
	return getPlotType() == PLOT_HILLS;
}

bool CvPlot::isPeak() const
{
	return getPlotType() == PLOT_PEAK;
}

bool CvPlot::isAsPeak() const
{
	return isPeak() || getFeatureType() != NO_FEATURE && GC.getFeatureInfo(getFeatureType()).isCountsAsPeak();
}


void CvPlot::setPlotType(PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	PROFILE_EXTRA_FUNC();
	const PlotTypes eOldPlotType = getPlotType();

	if (eOldPlotType == eNewValue)
	{
		return;
	}
	const bool bWasWater = eOldPlotType == PLOT_OCEAN;
	const bool bIsWater = eNewValue == PLOT_OCEAN;

	if (bWasWater || bIsWater)
	{
		erase();
	}

	if (eOldPlotType != NO_PLOT)
	{
		m_movementCharacteristicsHash ^= g_plotTypeZobristHashes[eOldPlotType];
	}
	if (eNewValue != NO_PLOT)
	{
		m_movementCharacteristicsHash ^= g_plotTypeZobristHashes[eNewValue];
	}

	updateSeeFromSight(false, true);

	m_ePlotType = eNewValue;

	updateSeeFromSight(true, true);

	short yieldChange[NUM_YIELD_TYPES] = {};
	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (eNewValue == PLOT_PEAK)
		{
			yieldChange[iI] += GC.getYieldInfo((YieldTypes)iI).getPeakChange();
		}
		else if (eNewValue == PLOT_HILLS)
		{
			yieldChange[iI] += GC.getYieldInfo((YieldTypes)iI).getHillsChange();
		}
		if (eOldPlotType == PLOT_PEAK)
		{
			yieldChange[iI] -= GC.getYieldInfo((YieldTypes)iI).getPeakChange();
		}
		else if (eOldPlotType == PLOT_HILLS)
		{
			yieldChange[iI] -= GC.getYieldInfo((YieldTypes)iI).getHillsChange();
		}
	}
	changeBaseYield(yieldChange);

	updatePlotGroup();

	if (getTerrainType() == NO_TERRAIN || GC.getTerrainInfo(getTerrainType()).isWaterTerrain() != bIsWater)
	{
		if (!bIsWater)
		{
			// @SAVEBREAK - remove (when scenarios handle climate zones)
			// Only world mapscript has been set up to define climate zone for map, and scenarios can also not define climate zones.
			if (getTerrainType() != NO_TERRAIN
			&&  GC.getTerrainInfo(getTerrainType()).isWaterTerrain()
			&&  GC.getTerrainInfo(getTerrainType()).getClimate() != NO_CLIMATE_ZONE
			&& GC.getMap().getClimateZone(getY()) == NO_CLIMATE_ZONE)
			{
				GC.getMap().setClimateZone(getY(), GC.getTerrainInfo(getTerrainType()).getClimate());
			}
			// !SAVEBREAK
			TerrainTypes eTerrain = (TerrainTypes)GC.getDefineINT("LAND_TERRAIN");

#ifdef EXPERIMENTAL_FEATURE_ON_PEAK
			if (eNewValue == PLOT_PEAK)
			{
				const TerrainTypes eTerrainPeak = GC.getTERRAIN_PEAK();
				if (eTerrainPeak != -1)
				{
					eTerrain = eTerrainPeak;
				}
			}
#endif
			setTerrainType(eTerrain, bRecalculate, bRebuildGraphics);
		}
		else
		{
			int iDistance = 3;
			if (isAdjacentToLand())
			{
				iDistance = 1;
			}
			else if (getDistanceToLandOrCoast(3) == 2)
			{
				iDistance = 2;
			}
			setClimateAppropriateWaterTerrain(iDistance, GC.getMap().getClimateZone(getY()));
		}
	}

	GC.getMap().resetPathDistance();

	if (bWasWater || bIsWater)
	{
		if (bRecalculate)
		{
			if (bIsWater)
			{
				CvPlot* plotX = plotCardinalDirection(getX(), getY(), CARDINALDIRECTION_NORTH);

				if (plotX && !plotX->isWater() && plotX->isNOfRiver())
				{
					plotX->setNOfRiver(false, NO_CARDINALDIRECTION);
				}

				plotX = plotCardinalDirection(getX(), getY(), CARDINALDIRECTION_WEST);

				if (plotX && !plotX->isWater() && plotX->isWOfRiver())
				{
					plotX->setWOfRiver(false, NO_CARDINALDIRECTION);
				}
			}

			const int x0 = getX();
			const int y0 = getY();

			for (int dx = -1; dx <= 1; dx++)
			{
				for (int dy = -1; dy <= 1; dy++)
				{
					if (dx != 0 || dy != 0)
					{
						CvPlot* plotX = plotXY(x0, y0, dx, dy);
						if (plotX && plotX->isWater())
						{
							int iLastDistance = 0;

							if (bIsWater)
							{
								if (plotX->isAdjacentToLand())
								{
									if (GC.getTerrainInfo(plotX->getTerrainType()).getDistanceToLand() != 1)
									{
										iLastDistance = plotX->setClimateAppropriateWaterTerrain(1);
									}
								}
								else if (plotX->getDistanceToLandOrCoast(3) == 2)
								{
									if (GC.getTerrainInfo(plotX->getTerrainType()).getDistanceToLand() != 2)
									{
										iLastDistance = plotX->setClimateAppropriateWaterTerrain(2);
									}
								}
								else if (GC.getTerrainInfo(plotX->getTerrainType()).getDistanceToLand() != 3)
								{
									iLastDistance = plotX->setClimateAppropriateWaterTerrain(3);
								}
							}
							else if (GC.getTerrainInfo(plotX->getTerrainType()).getDistanceToLand() != 1)
							{
								iLastDistance = plotX->setClimateAppropriateWaterTerrain(1);
							}
							if (iLastDistance)
							{
								plotX->correctWaterTerrains(iLastDistance, estimateDirection(dx, dy), true);
							}
						}
					}
				}
			}
		}

		foreach_(CvPlot* plotX, adjacent())
		{
			plotX->updateYield();
			plotX->updatePlotGroup();
		}

		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* plotX = plotCity(getX(), getY(), iI);

			if (plotX)
			{
				plotX->updatePotentialCityWork();
			}
		}

		GC.getMap().changeLandPlots(bIsWater ? -1 : 1);

		if (getBonusType() != NO_BONUS)
		{
			GC.getMap().changeNumBonusesOnLand(getBonusType(), bIsWater ? -1 : 1);
		}

		if (isOwned())
		{
			GET_PLAYER(getOwner()).changeTotalLand(bIsWater ? -1 : 1);
			GET_TEAM(getTeam()).changeTotalLand(bIsWater ? -1 : 1);
		}

		if (bRecalculate)
		{
			CvArea* pNewArea = NULL;
			bool bRecalculateAreas = false;

			// XXX might want to change this if we allow diagonal water movement...
			if (bIsWater)
			{
				foreach_(const CvPlot* plotX, cardinalDirectionAdjacent())
				{
					if (plotX->area()->isWater())
					{
						if (!pNewArea)
						{
							pNewArea = plotX->area();
						}
						else if (pNewArea != plotX->area())
						{
							bRecalculateAreas = true;
							break;
						}
					}
				}
			}
			else
			{
				foreach_(const CvPlot* plotX, adjacent())
				{
					if (!plotX->area()->isWater())
					{
						if (!pNewArea)
						{
							pNewArea = plotX->area();
						}
						else if (pNewArea != plotX->area())
						{
							bRecalculateAreas = true;
							break;
						}
					}
				}
			}

			if (!bRecalculateAreas)
			{
				CvPlot* plotX = plotDirection(getX(), getY(), (DirectionTypes)(NUM_DIRECTION_TYPES - 1));

				CvArea* pLastArea = plotX ? plotX->area() : NULL;

				int iAreaCount = 0;

				for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					plotX = plotDirection(getX(), getY(), (DirectionTypes)iI);

					CvArea* pCurrArea = plotX ? plotX->area() : NULL;

					if (pCurrArea != pLastArea)
					{
						iAreaCount++;
					}
					pLastArea = pCurrArea;
				}

				if (iAreaCount > 2)
				{
					bRecalculateAreas = true;
				}
			}

			if (!bRecalculateAreas)
			{
				setArea(FFreeList::INVALID_INDEX);

				if (area() && area()->getNumTiles() == 1)
				{
					GC.getMap().deleteArea(getArea());
				}
				if (!pNewArea)
				{
					pNewArea = GC.getMap().addArea();
					pNewArea->init(pNewArea->getID(), bIsWater);
				}
				setArea(pNewArea->getID());
			}
			else GC.getMap().recalculateAreas();
		}
	}

	if (bRebuildGraphics && GC.IsGraphicsInitialized() && shouldHaveGraphics())
	{
		//Update terrain graphical
		gDLL->getEngineIFace()->RebuildPlot(getViewportX(), getViewportY(), true, true);
		updateFeatureSymbol();
		setLayoutDirty(true);
		updateRouteSymbol(false, true);
		updateRiverSymbol(false, true);
	}
}


TerrainTypes CvPlot::getTerrainTypeExternal() const
{
#ifdef EXTREME_PAGING
	if (!shouldHaveFullGraphics())
	{
		if (isWater())
		{
			return (TerrainTypes)GC.getInfoTypeForString("TERRAIN_OCEAN");
		}
		return (TerrainTypes)GC.getInfoTypeForString("TERRAIN_GRASSLAND");
	}
#endif
	if (!GC.viewportsEnabled() || isRevealed(GC.getGame().getActiveTeam(), true))
	{
		return getTerrainType();
	}
	return (TerrainTypes)GC.getInfoTypeForString("TERRAIN_NONE");
}

TerrainTypes CvPlot::getTerrainType() const
{
	return (TerrainTypes)m_eTerrainType;
}


void CvPlot::setTerrainType(TerrainTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	PROFILE_EXTRA_FUNC();
	const TerrainTypes eOldTerrain = getTerrainType();

	if (eOldTerrain != eNewValue)
	{
		m_eTerrainType = eNewValue;

		if (eOldTerrain != NO_TERRAIN)
		{
			m_movementCharacteristicsHash ^= GC.getTerrainInfo(eOldTerrain).getZobristValue();
		}
		if (eNewValue != NO_TERRAIN)
		{
			m_movementCharacteristicsHash ^= GC.getTerrainInfo(eNewValue).getZobristValue();
		}
		short yieldChange[NUM_YIELD_TYPES] = {};
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (eNewValue != NO_TERRAIN)
			{
				yieldChange[iI] += GC.getTerrainInfo(eNewValue).getYield((YieldTypes)iI);
			}
			if (eOldTerrain != NO_TERRAIN)
			{
				yieldChange[iI] -= GC.getTerrainInfo(eOldTerrain).getYield((YieldTypes)iI);
			}
		}
		changeBaseYield(yieldChange);
		updatePlotGroup();

		if (eNewValue != NO_TERRAIN)
		{
			if (bRebuildGraphics && shouldHaveGraphics())
			{
				// Update terrain graphics
				gDLL->getEngineIFace()->RebuildPlot(getViewportX(), getViewportY(),false,true);
			}

			if (GC.getTerrainInfo(eNewValue).isWaterTerrain() != isWater())
			{
				setPlotType(isWater() ? PLOT_LAND : PLOT_OCEAN, bRecalculate, bRebuildGraphics);
			}
		}
	}
}


FeatureTypes CvPlot::getFeatureTypeExternal() const
{
	if (!GC.viewportsEnabled() || isRevealed(GC.getGame().getActiveTeam(), true))
	{
		return (FeatureTypes)m_eFeatureType;
	}
	return NO_FEATURE;
}

FeatureTypes CvPlot::getFeatureType() const
{
	return (FeatureTypes)m_eFeatureType;
}


void CvPlot::setFeatureType(FeatureTypes eNewValue, int iVariety, bool bImprovementSet)
{
	PROFILE_EXTRA_FUNC();
	FeatureTypes eOldFeature = getFeatureType();

	if (eNewValue != NO_FEATURE)
	{
		if (iVariety == -1)
		{
			iVariety = GC.getFeatureInfo(eNewValue).getArtInfo()->getNumVarieties() * (getLatitude()*9/8) / 90;
		}
		iVariety = range(iVariety, 0, (GC.getFeatureInfo(eNewValue).getArtInfo()->getNumVarieties() - 1));
	}
	else
	{
		iVariety = 0;
	}

	//Feature Removed or Fallout
	if (GC.getGame().isOption(GAMEOPTION_MAP_PERSONALIZED))
	{
		if (getOwner() != NO_PLAYER
		&& (eNewValue == NO_FEATURE || GC.getFeatureInfo(eNewValue).getHealthPercent() < 0)
		&& (getLandmarkType() == LANDMARK_FOREST || getLandmarkType() == LANDMARK_JUNGLE))
		{
			for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				CvPlot* plotX = plotCity(getX(), getY(), iI);
				if (plotX)
				{
					CvCity* cityX = plotX->getPlotCity();
					if (cityX && cityX->getOwner() == getOwner())
					{
						cityX->changeLandmarkAngerTimer(GC.getDefineINT("LANDMARK_ANGER_DIVISOR") * 2);
					}
				}
			}
		}
		if (getLandmarkType() == LANDMARK_FOREST || getLandmarkType() == LANDMARK_JUNGLE)
		{
			setLandmarkType(NO_LANDMARK);
			removeSignForAllPlayers();
		}
	}

	if (eOldFeature != eNewValue || m_iFeatureVariety != iVariety)
	{
		bool bUpdateSight = false;

		if (eOldFeature == NO_FEATURE || eNewValue == NO_FEATURE
		|| GC.getFeatureInfo(eOldFeature).getSeeThroughChange() != GC.getFeatureInfo(eNewValue).getSeeThroughChange())
		{
			bUpdateSight = true;
		}
		if (bUpdateSight)
		{
			updateSeeFromSight(false, true);
		}

		if (eOldFeature != eNewValue)
		{
			setImprovementUpgradeCache(-1);
		}
		if (eOldFeature != NO_FEATURE)
		{
			m_movementCharacteristicsHash ^= GC.getFeatureInfo(eOldFeature).getZobristValue();
		}
		if (eNewValue != NO_FEATURE)
		{
			m_movementCharacteristicsHash ^= GC.getFeatureInfo(eNewValue).getZobristValue();
		}
		m_eFeatureType = eNewValue;
		m_iFeatureVariety = iVariety;

		if (bUpdateSight)
		{
			updateSeeFromSight(true, true);
		}
		short yieldChange[NUM_YIELD_TYPES] = {};
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (eNewValue != NO_FEATURE)
			{
				yieldChange[iI] += GC.getFeatureInfo(eNewValue).getYieldChange((YieldTypes)iI);

				if (isRiver())
				{
					yieldChange[iI] += GC.getFeatureInfo(eNewValue).getRiverYieldChange((YieldTypes)iI);
				}
			}
			if (eOldFeature != NO_FEATURE)
			{
				yieldChange[iI] -= GC.getFeatureInfo(eOldFeature).getYieldChange((YieldTypes)iI);

				if (isRiver())
				{
					yieldChange[iI] -= GC.getFeatureInfo(eOldFeature).getRiverYieldChange((YieldTypes)iI);
				}
			}
		}
		changeBaseYield(yieldChange);
		updateFeatureSymbol();

		if (eOldFeature != NO_FEATURE && GC.getFeatureInfo(eOldFeature).getArtInfo()->isRiverArt()
		|| getFeatureType() != NO_FEATURE && GC.getFeatureInfo(getFeatureType()).getArtInfo()->isRiverArt())
		{
			updateRiverSymbolArt(true);
		}

		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* plotX = plotCity(getX(), getY(), iI);
			if (plotX)
			{
				CvCity* cityX = plotX->getPlotCity();
				if (cityX)
				{
					cityX->updateFeatureHealth();
					cityX->updateFeatureHappiness();
				}
			}
		}

		// Remove improvement that requries old feature unless prevented by bImprovementSet arg
		if (getFeatureType() == NO_FEATURE)
		{
			if (getImprovementType() != NO_IMPROVEMENT && !bImprovementSet
			&& GC.getImprovementInfo(getImprovementType()).isRequiresFeature())
			{
				setImprovementType(NO_IMPROVEMENT);
			}
		}
		else if (GC.getFeatureInfo(getFeatureType()).getTurnDamage() > 0)
		{
			foreach_(const CvUnit* pLoopUnit, units())
			{
				// AIAndy: Only awaken units that take damage from features
				if (pLoopUnit->getUnitInfo().getCombat() > 0)
				{
					pLoopUnit->getGroup()->clearMissionQueue();
					pLoopUnit->getGroup()->setActivityType(ACTIVITY_AWAKE);
				}
			}
		}
	}
}

void CvPlot::setFeatureDummyVisibility(const char *dummyTag, bool show)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		gDLL->getFeatureIFace()->setDummyVisibility(m_pFeatureSymbol, dummyTag, show);
	}
}

void CvPlot::addFeatureDummyModel(const char *dummyTag, const char *modelTag)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		gDLL->getFeatureIFace()->addDummyModel(m_pFeatureSymbol, dummyTag, modelTag);
	}
}

void CvPlot::setFeatureDummyTexture(const char *dummyTag, const char *textureTag)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		gDLL->getFeatureIFace()->setDummyTexture(m_pFeatureSymbol, dummyTag, textureTag);
	}
}

CvString CvPlot::pickFeatureDummyTag(int mouseX, int mouseY)
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		return gDLL->getFeatureIFace()->pickDummyTag(m_pFeatureSymbol, mouseX, mouseY);
	}

	return NULL;
}

void CvPlot::resetFeatureModel()
{
	FAssertMsg(m_pFeatureSymbol != NULL, "[Jason] No feature symbol.");
	if(m_pFeatureSymbol != NULL)
	{
		gDLL->getFeatureIFace()->resetModel(m_pFeatureSymbol);
	}
}

BonusTypes CvPlot::getBonusType(TeamTypes eTeam) const
{
	if (eTeam != NO_TEAM && m_eBonusType != NO_BONUS)
	{
		if (!GET_TEAM(eTeam).isHasTech((TechTypes)GC.getBonusInfo((BonusTypes)m_eBonusType).getTechReveal()) && !GET_TEAM(eTeam).isForceRevealedBonus((BonusTypes)m_eBonusType))
		{
			return NO_BONUS;
		}
	}
	return (BonusTypes)m_eBonusType;
}


BonusTypes CvPlot::getNonObsoleteBonusType(TeamTypes eTeam) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (m_eBonusType != NO_BONUS && GET_TEAM(eTeam).isBonusObsolete((BonusTypes)m_eBonusType))
	{
		return NO_BONUS;
	}
	return (BonusTypes)m_eBonusType;
}


void CvPlot::setBonusType(BonusTypes eNewValue)
{
	if (getBonusType() != eNewValue)
	{
		setImprovementUpgradeCache(-1);

		if (getBonusType() != NO_BONUS)
		{
			if (area())
			{
				area()->changeNumBonuses(getBonusType(), -1);
			}
			GC.getMap().changeNumBonuses(getBonusType(), -1);

			if (!isWater())
			{
				GC.getMap().changeNumBonusesOnLand(getBonusType(), -1);
			}

			if ( eNewValue == NO_BONUS && getImprovementType() != NO_IMPROVEMENT )
			{
				//	Bonus removed and this was previously a Zobrist contributor - remove it
				ToggleInPlotGroupsZobristContributors();
			}
		}
		else if (getImprovementType() == NO_IMPROVEMENT)
		{
			//	Bonus added and this was not previously a Zobrist contributor - add it
			ToggleInPlotGroupsZobristContributors();
		}

		if ( isOwned() )
		{
			GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();
			updatePlotGroupBonus(false);
		}
		m_eBonusType = eNewValue;
		if ( isOwned() )
		{
			updatePlotGroupBonus(true);
			GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();
		}

		if (getBonusType() != NO_BONUS)
		{
			if (area())
			{
				area()->changeNumBonuses(getBonusType(), 1);
			}
			GC.getMap().changeNumBonuses(getBonusType(), 1);

			if (!isWater())
			{
				GC.getMap().changeNumBonusesOnLand(getBonusType(), 1);
			}
		}

		updateYield();

		setLayoutDirty(true);

		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
	}
}

void CvPlot::setImprovementCurrentValue()
{
	PROFILE_EXTRA_FUNC();
	ImprovementTypes eImprovement = getImprovementType();
	TeamTypes eTeam = getTeam();
	if (eImprovement != NO_IMPROVEMENT && eTeam != NO_TEAM)
	{
		int iCountervalue = 0;
		BonusTypes eBonus = getNonObsoleteBonusType(eTeam);
		if (getOwner() != NO_PLAYER && eBonus != NO_BONUS)
		{
			iCountervalue = GET_PLAYER(getOwner()).AI_bonusVal(eBonus);
		}
		if (isCityRadius())
		{
			for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
			{
				iCountervalue += 20 * calculateImprovementYieldChange(eImprovement, (YieldTypes)iK, getOwner(), false);
				iCountervalue += 10 * calculateNatureYield((YieldTypes)iK, getTeam(), (getFeatureType() == NO_FEATURE) ? true : false);
			}
		}
		if (GC.getImprovementInfo(eImprovement).isMilitaryStructure())
		{
			int iCounterDefenseValue = GC.getImprovementInfo(eImprovement).getAirBombDefense()/10;
			iCounterDefenseValue += GC.getImprovementInfo(eImprovement).getDefenseModifier()/10;
			iCounterDefenseValue += (GC.getImprovementInfo(eImprovement).isZOCSource() ? 3 : 0);
			if (!isCityRadius())
			{
				iCountervalue += iCounterDefenseValue;
				iCountervalue += GC.getImprovementInfo(eImprovement).getCulture();
			}
			if (isCityRadius() && getWorkingCity() != NULL)
			{
				//if the city is under threat then the priority to replace with forts grows
				int iCounterThreat = iCounterDefenseValue;
				iCounterThreat *= getWorkingCity()->AI_cityThreat();
				iCounterThreat /= 100;
				iCountervalue += iCounterThreat;
			}
		}
		m_iImprovementCurrentValue = iCountervalue;
	}
	else
	{
		m_iImprovementCurrentValue = 0;
	}
}

int CvPlot::getImprovementCurrentValue() const
{
	return m_iImprovementCurrentValue;
}

ImprovementTypes CvPlot::getImprovementType() const
{
	return (ImprovementTypes)m_eImprovementType;
}

// Toffer - Helper function
bool CvPlot::isImprovementDestructible() const
{
	return (
		m_eImprovementType > -1
		&&
		GC.getImprovementInfo(getImprovementType()).getAirBombDefense() > -1
	);
}
// ! Toffer


void CvPlot::setImprovementType(ImprovementTypes eNewImprovement)
{
	PROFILE_EXTRA_FUNC();
	const ImprovementTypes eOldImprovement = getImprovementType();

	if (eOldImprovement != eNewImprovement)
	{
		/* Toffer - This is the wrong place to have this upgrade cost.
		// e.g. Workers building a farm on a seed camp would get charged both when they start the build and here again when they finish the build because seed camp has farm as an upgrade.
		// Should be in an intermediate function that calls this function, an intermediate only the imp upg code calls.
		// This issue was noticed when this happened on an unowned plot, and GET_PLAYER(getOwner()).changeGold() failed.
		if (eOldImprovement != NO_IMPROVEMENT && eNewImprovement != NO_IMPROVEMENT)
		{
			//Charge for Upgrade but be careful not to charge for a downgrade (pillage).
			if (GC.getImprovementInfo(eOldImprovement).getImprovementUpgrade() == eNewImprovement)
			{
				GET_PLAYER(getOwner()).changeGold(-GC.getImprovementInfo(eNewImprovement).getHighestCost());
			}
			else
			{
				for (int iI = 0; iI < GC.getImprovementInfo(eOldImprovement).getNumAlternativeImprovementUpgradeTypes(); iI++)
				{
					if ((ImprovementTypes)GC.getImprovementInfo(eOldImprovement).getAlternativeImprovementUpgradeType(iI) == eNewImprovement)
					{
						GET_PLAYER(getOwner()).changeGold(-GC.getImprovementInfo(eNewImprovement).getHighestCost());
						break;
					}
				}
			}
		}
		*/
		// Remove old improvement
		if (eOldImprovement != NO_IMPROVEMENT)
		{
			if (area())
			{
				area()->changeNumImprovements(eOldImprovement, -1);
			}
			if (isOwned())
			{
				GET_PLAYER(getOwner()).changeImprovementCount(eOldImprovement, -1);
			}
		}
		if (eNewImprovement != NO_IMPROVEMENT)
		{
			for (int iI = 0; iI < GC.getImprovementInfo(eNewImprovement).getNumFeatureChangeTypes(); iI++)
			{
				FeatureTypes eAddingFeature = (FeatureTypes)GC.getImprovementInfo(eNewImprovement).getFeatureChangeType(iI);
				if (canHaveFeature(eAddingFeature, true))
				{
					setFeatureType(eAddingFeature, -1, true);
					break;
				}
			}
			if (GC.getImprovementInfo(eNewImprovement).getBonusChange() != NO_BONUS)
			{
				const BonusTypes eAddingBonus = GC.getImprovementInfo(eNewImprovement).getBonusChange();
				if (canHaveBonus(eAddingBonus, false))
				{
					setBonusType(eAddingBonus);
				}
			}
			if (GC.getImprovementInfo(eNewImprovement).getImprovementUpgrade() != NO_IMPROVEMENT && GC.getGame().getImprovementUpgradeTime(eNewImprovement) > 0)
			{
				m_bImprovementUpgradable = true;
				setImprovementUpgradeProgress(0);
				setImprovementUpgradeCache(-1);
			}
			else m_bImprovementUpgradable = false;
		}
		else m_bImprovementUpgradable = false;

		updateSight(false, true);

		if (eNewImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eNewImprovement).isChangeRemove())
		{
			eNewImprovement = NO_IMPROVEMENT;
		}
		if (isOwned())
		{
			GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();
			updatePlotGroupBonus(false);
		}
		m_eImprovementType = eNewImprovement;
		if (isOwned())
		{
			updatePlotGroupBonus(true);
			GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();
		}

		for (int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive() && isVisible((TeamTypes)iI, false))
			{
				setRevealedImprovementType((TeamTypes)iI, eNewImprovement);
			}
		}

		if (eNewImprovement != NO_IMPROVEMENT)
		{
			if (area())
			{
				area()->changeNumImprovements(eNewImprovement, 1);
			}
			if (isOwned())
			{
				GET_PLAYER(getOwner()).changeImprovementCount(eNewImprovement, 1);
				if (GC.getImprovementInfo(eNewImprovement).getCulture() > 0)
				{
					pushCultureFromImprovement(getOwner(), 1, GC.getImprovementInfo(eNewImprovement).getCultureRange(), true);
				}
			}
		}
		updateSight(true, true);

		if (getBonusType() != NO_BONUS)
		{
			if ((eNewImprovement == NO_IMPROVEMENT) != (eOldImprovement == NO_IMPROVEMENT))
			{
				// Newly added or removed improvement on a bonus
				// Add/remove Zobrist contributuns for all local plot groups
				ToggleInPlotGroupsZobristContributors();
			}

			// Building or removing a fort will now force a plotgroup update to verify resource connections.
			if (
				(NO_IMPROVEMENT != eNewImprovement && GC.getImprovementInfo(eNewImprovement).isImprovementBonusTrade(getBonusType()))
				!=
				(NO_IMPROVEMENT != eOldImprovement && GC.getImprovementInfo(eOldImprovement).isImprovementBonusTrade(getBonusType())))
			{
				updatePlotGroup();
			}
		}
		updateIrrigated();
		updateYield();

		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);

			if (pLoopPlot != NULL)
			{
				CvCity* pLoopCity = pLoopPlot->getPlotCity();

				if (pLoopCity != NULL)
				{
					pLoopCity->updateFeatureHappiness();
					pLoopCity->updateImprovementHealth();

					//	Changed improvement status might change city best build opinions
					pLoopCity->AI_markBestBuildValuesStale();
				}
			}
		}

		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

		if (NO_IMPROVEMENT != eOldImprovement && GC.getImprovementInfo(eOldImprovement).isActsAsCity())
		{
			verifyUnitValidPlot();
		}

		if (GC.getGame().isDebugMode())
		{
			setLayoutDirty(true);
		}

		if (eNewImprovement != NO_IMPROVEMENT)
		{
			CvEventReporter::getInstance().improvementBuilt(eNewImprovement, getX(), getY());
		}

		if (eNewImprovement == NO_IMPROVEMENT)
		{
			CvEventReporter::getInstance().improvementDestroyed(eOldImprovement, getOwner(), getX(), getY());
		}

		CvCity* pWorkingCity = getWorkingCity();
		if (NULL != pWorkingCity)
		{
			if(NO_IMPROVEMENT != eNewImprovement && pWorkingCity->getImprovementFreeSpecialists(eNewImprovement) > 0
			|| NO_IMPROVEMENT != eOldImprovement && pWorkingCity->getImprovementFreeSpecialists(eOldImprovement) > 0)
			{
				pWorkingCity->AI_setAssignWorkDirty(true);
			}
		}
		setImprovementCurrentValue();
		gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
	}
}


RouteTypes CvPlot::getRouteType() const
{
	return (RouteTypes)m_eRouteType;
}


void CvPlot::setRouteType(RouteTypes eNewValue, bool bUpdatePlotGroups)
{
	PROFILE_EXTRA_FUNC();
	const RouteTypes eOldRoute = getRouteType();

	if (eOldRoute == eNewValue)
	{
		return;
	}
	const bool bOldRoute = eOldRoute != NO_ROUTE;
	const bool bNewRoute = eNewValue != NO_ROUTE;
	const bool bOldSeaRoute = isSeaTunnel();

	if (isOwned())
	{
		GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();
		updatePlotGroupBonus(false);
	}

	if (bOldRoute)
	{
		m_movementCharacteristicsHash ^= GC.getRouteInfo(eOldRoute).getZobristValue();
	}
	if (bNewRoute)
	{
		m_movementCharacteristicsHash ^= GC.getRouteInfo(eNewValue).getZobristValue();
	}

	m_eRouteType = eNewValue;

	if (isOwned())
	{
		updatePlotGroupBonus(true);
		GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();
	}

	for (int iI = 0; iI < MAX_TEAMS; ++iI)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive() && isVisible((TeamTypes)iI, false))
		{
			setRevealedRouteType((TeamTypes)iI, eNewValue);
		}
	}
	updateYield();

	if (bUpdatePlotGroups && bOldRoute != bNewRoute)
	{
		updatePlotGroup();
	}

	if (GC.getGame().isDebugMode())
	{
		updateRouteSymbol(true, true);
	}

	if (bNewRoute)
	{
		CvEventReporter::getInstance().routeBuilt(eNewValue, getX(), getY());
	}
	else if (bOldSeaRoute)
	{
		foreach_(CvUnit* pLoopUnit, units())
		{
			// MIGHT need to also specify if the unit isn't cargo of a land unit that isn't cargo itself.
			//	Shouldn't have to though because if the carrier would die, then that would dunk the carrier's units, likely in a fatal manner.
			if (pLoopUnit->getDomainType() == DOMAIN_LAND && !pLoopUnit->isCargo() && !pLoopUnit->canMoveAllTerrain())
			{
				pLoopUnit->kill(true);
			}
		}
	}
}


void CvPlot::updateCityRoute(bool bUpdatePlotGroup)
{
	PROFILE_FUNC();

	RouteTypes eCityRoute;

	if (isCity())
	{
		FAssertMsg(isOwned(), "isOwned is expected to be true");

		eCityRoute = GET_PLAYER(getOwner()).getBestRoute();

		if (eCityRoute == NO_ROUTE)
		{
			eCityRoute = ((RouteTypes)(GC.getDefineINT("INITIAL_CITY_ROUTE_TYPE")));
		}

		setRouteType(eCityRoute, bUpdatePlotGroup);
	}
}


CvCity* CvPlot::getPlotCity() const
{
	return getCity(m_plotCity);
}


void CvPlot::setPlotCity(CvCity* pNewValue)
{
	PROFILE_EXTRA_FUNC();
	if (getPlotCity() == pNewValue)
	{
		return;
	}
	if (isCity())
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* plotX = plotCity(getX(), getY(), iI);

			if (plotX)
			{
				plotX->changeCityRadiusCount(-1);
				plotX->changePlayerCityRadiusCount(getPlotCity()->getOwner(), -1);
			}
		}
	}
	GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();

	updatePlotGroupBonus(false);
	if (isCity())
	{
		CvPlotGroup* pPlotGroup = getPlotGroup(getOwner());

		if (pPlotGroup)
		{
			for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
			{
				getPlotCity()->changeNumBonuses((BonusTypes)iI, -pPlotGroup->getNumBonuses((BonusTypes)iI));
			}
		}
	}
	if (pNewValue)
	{
		m_plotCity = pNewValue->getIDInfo();
	}
	else m_plotCity.reset();


	if (isCity())
	{
		CvPlotGroup* pPlotGroup = getPlotGroup(getOwner());

		if (pPlotGroup)
		{
			for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
			{
				getPlotCity()->changeNumBonuses((BonusTypes)iI, pPlotGroup->getNumBonuses((BonusTypes)iI));
			}
		}
	}
	updatePlotGroupBonus(true);

	GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();

	if (isCity())
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* plotX = plotCity(getX(), getY(), iI);

			if (plotX)
			{
				plotX->changeCityRadiusCount(1);
				plotX->changePlayerCityRadiusCount(getPlotCity()->getOwner(), 1);
			}
		}
	}
	updateIrrigated();
	updateYield();
	updateMinimapColor();
}


CvCity* CvPlot::getWorkingCity() const
{
	return getCity(m_workingCity);
}


void CvPlot::updateWorkingCity()
{
	PROFILE_FUNC();

	CvCity* pBestCity = getPlotCity();

	if (pBestCity == NULL)
	{
		pBestCity = getWorkingCityOverride();
		FAssertMsg(pBestCity == NULL || pBestCity->getOwner() == getOwner(), "pBest city is expected to either be NULL or the current plot instance's");
	}

	if (pBestCity == NULL && isOwned())
	{
		const int* cityPlotPriority = GC.getCityPlotPriority();
		int iBestPlot;

		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* plotX = plotCity(getX(), getY(), iI);

			if (plotX != NULL)
			{
				CvCity* cityX = plotX->getPlotCity();

				if (cityX != NULL && cityX->getOwner() == getOwner()
					&&
					(
						pBestCity == NULL
						||
						cityPlotPriority[iI] < cityPlotPriority[iBestPlot]
						||
						cityPlotPriority[iI] == cityPlotPriority[iBestPlot]
						&&
						(
							// XXX use getGameTurnAcquired() instead???
							cityX->getGameTurnFounded() < pBestCity->getGameTurnFounded()
							||
							cityX->getGameTurnFounded() == pBestCity->getGameTurnFounded()
							&&
							cityX->getID() < pBestCity->getID()
						)
					)
				)
				{
					iBestPlot = iI;
					pBestCity = cityX;
				}
			}
		}
	}
	CvCity* pOldWorkingCity = getWorkingCity();

	if (pOldWorkingCity != pBestCity)
	{
		if (pOldWorkingCity != NULL)
		{
			pOldWorkingCity->setWorkingPlot(pOldWorkingCity->getCityPlotIndex(this), false);
		}

		if (pBestCity != NULL)
		{
			FAssertMsg(isOwned(), "isOwned is expected to be true");
			FAssertMsg(!isBeingWorked(), "isBeingWorked did not return false as expected");
			m_workingCity = pBestCity->getIDInfo();
		}
		else m_workingCity.reset();


		if (pOldWorkingCity != NULL)
		{
			pOldWorkingCity->AI_setAssignWorkDirty(true);
		}
		if (getWorkingCity() != NULL)
		{
			getWorkingCity()->AI_setAssignWorkDirty(true);
		}

		updateYield();
		updateFog();
		updateShowCitySymbols();

		if (getOwner() == GC.getGame().getActivePlayer()
		&& gDLL->getGraphicOption(GRAPHICOPTION_CITY_RADIUS)
		&& gDLL->getInterfaceIFace()->canSelectionListFound())
		{
			gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		}
	}
}


CvCity* CvPlot::getWorkingCityOverride() const
{
	return getCity(m_workingCityOverride);
}


void CvPlot::setWorkingCityOverride( const CvCity* pNewValue)
{
	if (getWorkingCityOverride() != pNewValue)
	{
		if (pNewValue != NULL)
		{
			FAssertMsg(pNewValue->getOwner() == getOwner(), "Argument city pNewValue's owner is expected to be the same as the current instance");
			m_workingCityOverride = pNewValue->getIDInfo();
		}
		else
		{
			m_workingCityOverride.reset();
		}

		updateWorkingCity();
	}
}


int CvPlot::getRiverID() const
{
	return m_iRiverID;
}


void CvPlot::setRiverID(int iNewValue)
{
	m_iRiverID = iNewValue;
}


int CvPlot::getMinOriginalStartDist() const
{
	return m_iMinOriginalStartDist;
}


void CvPlot::setMinOriginalStartDist(int iNewValue)
{
	m_iMinOriginalStartDist = iNewValue;
}


int CvPlot::getReconCount() const
{
	return m_iReconCount;
}


void CvPlot::changeReconCount(int iChange)
{
	m_iReconCount += iChange;
	FASSERT_NOT_NEGATIVE(getReconCount());
}


int CvPlot::getRiverCrossingCount() const
{
	return m_iRiverCrossingCount;
}


void CvPlot::changeRiverCrossingCount(int iChange)
{
	PROFILE_EXTRA_FUNC();
	const bool bWasRiver = m_iRiverCrossingCount > 0;
	m_iRiverCrossingCount += iChange;
	FASSERT_NOT_NEGATIVE(m_iRiverCrossingCount);

	bool bNewRiverPlot = false;
	bool bRiverRemoved = false;
	if (bWasRiver)
	{
		if (m_iRiverCrossingCount < 1)
		{
			bRiverRemoved = true;
		}
	}
	else if (m_iRiverCrossingCount > 0)
	{
		bNewRiverPlot = true;
	}
	if (bNewRiverPlot || bRiverRemoved)
	{
		const FeatureTypes eFeature = getFeatureType();
		short yieldChange[NUM_YIELD_TYPES] = {};
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (bNewRiverPlot)
			{
				yieldChange[iI] += GC.getYieldInfo((YieldTypes)iI).getRiverChange();

				if (eFeature != NO_FEATURE)
				{
					yieldChange[iI] += GC.getFeatureInfo(eFeature).getRiverYieldChange((YieldTypes)iI);
				}
			}
			else if (bRiverRemoved)
			{
				yieldChange[iI] -= GC.getYieldInfo((YieldTypes)iI).getRiverChange();

				if (eFeature != NO_FEATURE)
				{
					yieldChange[iI] -= GC.getFeatureInfo(eFeature).getRiverYieldChange((YieldTypes)iI);
				}
			}
		}
		changeBaseYield(yieldChange);
	}
}


int CvPlot::getBaseYield(const YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_baseYields[eIndex];
}

void CvPlot::changeBaseYield(const short* pYieldChange)
{
	PROFILE_EXTRA_FUNC();

	bool bChange = false;

	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (pYieldChange[iI] != 0)
		{
			m_baseYields[iI] += pYieldChange[iI];
			bChange = true;
		}
	}
	if (bChange)
	{
		updateYield();
	}
}

void CvPlot::recalculateBaseYield()
{
	PROFILE_EXTRA_FUNC();
	const PlotTypes ePlot = getPlotType();
	const TerrainTypes eTerrain = getTerrainType();
	const FeatureTypes eFeature = getFeatureType();
	const bool bRiver = getRiverCrossingCount() > 0;

	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		int iYield = 0;

		if (ePlot == PLOT_PEAK)
		{
			iYield += GC.getYieldInfo((YieldTypes)iI).getPeakChange();
		}
		else if (ePlot == PLOT_HILLS)
		{
			iYield += GC.getYieldInfo((YieldTypes)iI).getHillsChange();
		}
		if (eTerrain != NO_TERRAIN)
		{
			iYield += GC.getTerrainInfo(eTerrain).getYield((YieldTypes)iI);
		}
		if (eFeature != NO_FEATURE)
		{
			iYield += GC.getFeatureInfo(eFeature).getYieldChange((YieldTypes)iI);

			if (bRiver)
			{
				iYield += GC.getFeatureInfo(eFeature).getRiverYieldChange((YieldTypes)iI);
			}
		}
		if (bRiver)
		{
			iYield += GC.getYieldInfo((YieldTypes)iI).getRiverChange();
		}
		m_baseYields[iI] = iYield;
	}
}


void CvPlot::setExtraYield(YieldTypes eYield, short iExtraYield)
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eYield);
	FAssertMsg(iExtraYield != 0, "Redundant function call");

	m_aExtraYield[eYield] += iExtraYield;

	updateYield();
}


short* CvPlot::getYield() const
{
	return m_aiYield;
}

int CvPlot::getYield(YieldTypes eIndex) const
{
	FASSERT_BOUNDS(0, NUM_YIELD_TYPES, eIndex);
	return m_aiYield[eIndex];
}

void CvPlot::updateYield()
{
	PROFILE_FUNC();

	if (!area()) return;

	bool bChange = false;
	CvCity* pWorkingCity = getWorkingCity();
	const bool bWorked = pWorkingCity ? pWorkingCity->isWorkingPlot(this) : false;

	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		const int iNewYield = calculateYield((YieldTypes)iI);
		const int iOldYield = m_aiYield[iI];

		if (iOldYield != iNewYield)
		{
			FASSERT_NOT_NEGATIVE(iNewYield);

			m_aiYield[iI] = iNewYield;

			if (bWorked)
			{
				pWorkingCity->changePlotYield((YieldTypes)iI, iNewYield - iOldYield);
			}
			bChange = true;
		}
	}
	if (bChange)
	{
		if (pWorkingCity)
		{
			pWorkingCity->AI_setAssignWorkDirty(true);
		}
		updateSymbols();
	}
}


int CvPlot::calculateNatureYield(YieldTypes eYield, TeamTypes eTeam, bool bIgnoreFeature) const
{
	PROFILE_FUNC();

	if (isImpassable(getTeam()))
	{
		if (!isAsPeak())
		{
			return 0;
		}
		if (eTeam != NO_TEAM && !isRoute())
		{// It makes sense to only require a route for a city to work the peak even if the player doesn't have the mounteneering tech.
		// Perhaps add a landslide event that destroys route and improvement on peaks, so that players without the mountaneering tech
		// can't rebuild easily mountain improvements conquered from a more advanced civilization.
			return 0;
		}
	}
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	int iYield = getBaseYield(eYield);

	if (eTeam != NO_TEAM && getBonusType(eTeam) != NO_BONUS)
	{
		iYield += GC.getBonusInfo(getBonusType(eTeam)).getYieldChange(eYield);
	}

	if (bIgnoreFeature && getFeatureType() != NO_FEATURE)
	{
		if (isRiver())
		{
			iYield -= GC.getFeatureInfo(getFeatureType()).getRiverYieldChange(eYield);
		}
		iYield -= GC.getFeatureInfo(getFeatureType()).getYieldChange(eYield);
	}
	return std::max(0, iYield);
}


int CvPlot::calculateBestNatureYield(YieldTypes eIndex, TeamTypes eTeam) const
{
	return std::max(calculateNatureYield(eIndex, eTeam, false), calculateNatureYield(eIndex, eTeam, true));
}


int CvPlot::calculateTotalBestNatureYield(TeamTypes eTeam) const
{
	return (calculateBestNatureYield(YIELD_FOOD, eTeam) + calculateBestNatureYield(YIELD_PRODUCTION, eTeam) + calculateBestNatureYield(YIELD_COMMERCE, eTeam));
}


int CvPlot::calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal, bool bBestRoute) const
{
	PROFILE_FUNC();

	int iYield = GC.getImprovementInfo(eImprovement).getYieldChange(eYield);

	if (isRiverSide())
	{
		iYield += GC.getImprovementInfo(eImprovement).getRiverSideYieldChange(eYield);
	}

	if (bOptimal ? true : isIrrigationAvailable())
	{
		iYield += GC.getImprovementInfo(eImprovement).getIrrigatedYieldChange(eYield);
	}

	if (bOptimal)
	{
		int iBestYield = 0;

		for (int iI = 0; iI < GC.getNumRouteInfos(); ++iI)
		{
			iBestYield = std::max(iBestYield, GC.getImprovementInfo(eImprovement).getRouteYieldChanges(iI, eYield));
		}
		iYield += iBestYield;
	}
	else
	{
		const RouteTypes eRoute =
		(
			bBestRoute && ePlayer != NO_PLAYER
			?
			GET_PLAYER(ePlayer).getBestRoute(GC.getMap().plotSorenINLINE(getX(), getY()))
			:
			getRouteType()
		);
		if (eRoute != NO_ROUTE)
		{
			iYield += GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eRoute, eYield);
		}
	}

	if (bOptimal || ePlayer == NO_PLAYER)
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); ++iI)
		{
			iYield += GC.getImprovementInfo(eImprovement).getTechYieldChanges(iI, eYield);
		}

		for (int iI = 0; iI < GC.getNumCivicInfos(); ++iI)
		{
			iYield += GC.getCivicInfo((CivicTypes) iI).getImprovementYieldChanges(eImprovement, eYield);
		}
	}
	else
	{
		iYield += GET_PLAYER(ePlayer).getImprovementYieldChange(eImprovement, eYield);
		iYield += GET_TEAM(GET_PLAYER(ePlayer).getTeam()).getImprovementYieldChange(eImprovement, eYield);
	}

	if (ePlayer != NO_PLAYER)
	{
		const BonusTypes eBonus = getBonusType(GET_PLAYER(ePlayer).getTeam());

		if (eBonus != NO_BONUS)
		{
			iYield += GC.getImprovementInfo(eImprovement).getImprovementBonusYield(eBonus, eYield);
		}
	}

	// Improvement cannot actually produce negative yield
	int iCurrYield = calculateNatureYield(eYield, (ePlayer == NO_PLAYER) ? NO_TEAM : GET_PLAYER(ePlayer).getTeam(), bOptimal);

	return std::max( -iCurrYield, iYield );
}


int CvPlot::calculateYield(YieldTypes eYield, bool bDisplay) const
{
	PROFILE_FUNC();

	if (bDisplay && GC.getGame().isDebugMode())
	{
		return getYield()[eYield];
	}

	if (getTerrainType() == NO_TERRAIN || !isPotentialCityWork())
	{
		return 0;
	}

	PlayerTypes ePlayer;

	if (bDisplay)
	{
		ePlayer = getRevealedOwner(GC.getGame().getActiveTeam(), false);

		if (ePlayer == NO_PLAYER)
		{
			ePlayer = GC.getGame().getActivePlayer();
		}
	}
	else ePlayer = getOwner();

	int iYield = (
		calculateNatureYield(eYield, (ePlayer != NO_PLAYER) ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM)
		+
		m_aExtraYield[eYield]
	);
	bool bCity = false;

	if (ePlayer != NO_PLAYER)
	{
		{
			const CvCity* pCity = getPlotCity();

			if (pCity && (!bDisplay || pCity->isRevealed(GC.getGame().getActiveTeam(), false)))
			{
				iYield += GC.getYieldInfo(eYield).getCityChange();

				if (GC.getYieldInfo(eYield).getPopulationChangeDivisor() != 0)
				{
					iYield += pCity->getPopulation() / GC.getYieldInfo(eYield).getPopulationChangeDivisor();
				}
				bCity = true;
			}
		}
		{
			const bool bReachable = isRoute() || !isImpassable(GET_PLAYER(ePlayer).getTeam());
			if (bReachable)
			{
				iYield += GET_PLAYER(ePlayer).getTerrainYieldChange(getTerrainType(), eYield);

				if (isWater())
				{
					iYield += GET_PLAYER(ePlayer).getSeaPlotYield(eYield);
				}
				const CvCity* pWorkingCity = getWorkingCity();

				if (pWorkingCity && (!bDisplay || pWorkingCity->isRevealed(GC.getGame().getActiveTeam(), false)))
				{
					iYield += pWorkingCity->getYieldChangeAt(this, eYield);
				}
			}
		}
		if (getLandmarkType() != NO_LANDMARK && GC.getGame().isOption(GAMEOPTION_MAP_PERSONALIZED))
		{
			iYield += GET_PLAYER(ePlayer).getLandmarkYield(eYield);
		}

		if (GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield) > 0 && iYield >= GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield))
		{
			iYield += GC.getDefineINT("EXTRA_YIELD");
		}

		if (GET_PLAYER(ePlayer).getLessYieldThreshold(eYield) > 0 && iYield >= GET_PLAYER(ePlayer).getLessYieldThreshold(eYield))
		{
			iYield -= GC.getDefineINT("EXTRA_YIELD");
		}

		if (GET_PLAYER(ePlayer).isGoldenAge() && iYield >= GC.getYieldInfo(eYield).getGoldenAgeYieldThreshold())
		{
			iYield += GC.getYieldInfo(eYield).getGoldenAgeYield();
		}
	}
	if (bCity)
	{
		iYield = std::max(iYield, GC.getYieldInfo(eYield).getMinCity());
	}
	else
	{
		ImprovementTypes eImprovement;
		RouteTypes eRoute;

		if (bDisplay)
		{
			eImprovement = getRevealedImprovementType(GC.getGame().getActiveTeam());
			eRoute = getRevealedRouteType(GC.getGame().getActiveTeam(), false);
		}
		else
		{
			eImprovement = getImprovementType();
			eRoute = getRouteType();
		}

		if (eImprovement != NO_IMPROVEMENT)
		{
			iYield += calculateImprovementYieldChange(eImprovement, eYield, ePlayer);
		}

		if (eRoute != NO_ROUTE)
		{
			iYield += GC.getRouteInfo(eRoute).getYieldChange(eYield);
		}
	}
	return std::max(0, iYield);
}


bool CvPlot::hasYield() const
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (getYield((YieldTypes)iI) > 0)
		{
			return true;
		}
	}
	return false;
}


int CvPlot::getCulture(PlayerTypes eIndex) const
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	for (std::vector<std::pair<PlayerTypes,int> >::const_iterator itr = m_aiCulture.begin(); itr != m_aiCulture.end(); ++itr)
	{
		if ((*itr).first == eIndex)
		{
			return (*itr).second;
		}
	}

	return 0;
}


int CvPlot::countTotalCulture() const
{
	PROFILE_EXTRA_FUNC();
	int iTotalCulture = 0;

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iTotalCulture += getCulture((PlayerTypes)iI);
		}
	}

	return iTotalCulture;
}

PlayerTypes CvPlot::findHighestCulturePlayer(const bool bCountLegacyCulture, const bool bCountLastTurn) const
{
	PROFILE_EXTRA_FUNC();
	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		const PlayerTypes ePlayerX = static_cast<PlayerTypes>(iI);

		if (GET_PLAYER(ePlayerX).isAlive())
		{
			// Equilibium culture game option may result in negative culture when near equilibrium (loss of buildings, etc) and
			// as a result can't be immediately set to unown; we are required to use decay dynamics instead to lose control
			if (bCountLegacyCulture
			|| (getCultureRateThisTurn(ePlayerX) > 0 || GC.getGame().isOption(GAMEOPTION_CULTURE_EQUILIBRIUM))
			|| bCountLastTurn && (getCultureRateLastTurn(ePlayerX) > 0 || GC.getGame().isOption(GAMEOPTION_CULTURE_EQUILIBRIUM)))
			{
				const int iValue = getCulture(ePlayerX);
				if (iValue > 0)
				{
					if (iValue > iBestValue)
					{
						iBestValue = iValue;
						eBestPlayer = ePlayerX;
					}
					// Tiebreaker: goes to current owner rather than lowest index
					else if (iValue == iBestValue && getOwner() == ePlayerX)
					{
						eBestPlayer = ePlayerX;
					}
				}
			}
		}
	}
	return eBestPlayer;
}

// returns value between 0 and 100, with extra 0 per optional iExtraDigits
int CvPlot::calculateCulturePercent(PlayerTypes eIndex, int iExtraDigits) const
{
	PROFILE_FUNC();

	const int iTotalCulture = countTotalCulture();

	if (iTotalCulture > 0)
	{
		return intPow(10, iExtraDigits) * 100 * getCulture(eIndex) / iTotalCulture;
	}
	return 0;
}


int CvPlot::calculateTeamCulturePercent(TeamTypes eIndex) const
{
	PROFILE_EXTRA_FUNC();
	int iTeamCulturePercent = 0;

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eIndex))
		{
			iTeamCulturePercent += calculateCulturePercent((PlayerTypes)iI);
		}
	}
	return iTeamCulturePercent;
}


void CvPlot::setCulture(PlayerTypes eIndex, int iNewValue, bool bUpdate, bool bUpdatePlotGroups, const bool bDecay)
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (iNewValue == 0 && getPlotCity() && getOwner() == eIndex)
	{
		FErrorMsg("Trace: Attempting to set plot culture to zero for player that owns the city on the plot");
		iNewValue = 1;
	}

	// Protection from overzealous decay, others
	iNewValue = std::max(0, iNewValue);

	if (getCulture(eIndex) != iNewValue)
	{
		// Many things apply 1 culture to tile to mark as claimed; setting to 2 instead ensures claim for at least a full turn on EQ setting
		if (GC.getGame().isOption(GAMEOPTION_CULTURE_EQUILIBRIUM) && getCulture(eIndex) == 0 && iNewValue == 1) iNewValue = 2;

		const int iChange = iNewValue - getCulture(eIndex);

		if (bDecay || iChange > 0) // ignore influence driven war reductions
		{
			bool bFirst = true;
			for (std::vector< std::pair<PlayerTypes, int> >::iterator it = m_cultureRatesThisTurn.begin(); it != m_cultureRatesThisTurn.end(); ++it)
			{
				if ((*it).first == eIndex)
				{
					if ((*it).second == -iChange)
					{
						m_cultureRatesThisTurn.erase(it);
					}
					else
					{
						(*it).second += iChange;
					}
					bFirst = false;
					break;
				}
			}
			if (bFirst) m_cultureRatesThisTurn.push_back(std::make_pair(eIndex, iChange));
		}
		std::vector<std::pair<PlayerTypes, int> >::iterator itr;

		// Toffer - 08.08.20
		// 4 byte integer overflow protection
		if (iNewValue > 1000000000) // trigger reduction at a billion
		{
			// This player may not yet be in the vector if it goes from 0 to a billion in one go through worldbuilder or something.
			iNewValue /= 10; // So we do this outside the vector loop.
			// Reduce culture by same factor for all players
			for (itr = m_aiCulture.begin(); itr != m_aiCulture.end(); ++itr)
			{
				if ((*itr).first != eIndex)
				{
					(*itr).second /= 10;
				}
			}
			// This overflow protection will start to break down if a plot is getting 900 million+ culture from a specific player per turn.
			// it reduce 1 billion+ down to 100 million+, this should be an adequate overflow protection in this case.
		}
		// ! Toffer

		for (itr = m_aiCulture.begin(); itr != m_aiCulture.end(); ++itr)
		{
			if ((*itr).first == eIndex)
			{
				break;
			}
		}

		if (itr == m_aiCulture.end())
		{
			m_aiCulture.push_back(std::make_pair(eIndex, iNewValue));
			// Force the capacity to the size() since we need to minimize memory usage and adding cultures is rare
			std::vector<std::pair<PlayerTypes, int> >(m_aiCulture).swap(m_aiCulture);
		}
		else
		{
			(*itr).second = iNewValue;
		}
		FASSERT_NOT_NEGATIVE(getCulture(eIndex));

		if (bUpdate)
		{
			updateCulture(true, bUpdatePlotGroups);
		}

		if (getPlotCity() != NULL)
		{
			getPlotCity()->AI_setAssignWorkDirty(true);
		}
	}
}


void CvPlot::changeCulture(PlayerTypes eIndex, int iChange, bool bUpdate)
{
	if (0 != iChange)
	{
		setCulture(eIndex, std::max(0, getCulture(eIndex) + iChange), bUpdate, true);
	}
}

int CvPlot::getFoundValue(PlayerTypes eIndex)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (NULL == m_aiFoundValue || !isRevealed(GET_PLAYER(eIndex).getTeam(), false) && GET_PLAYER(eIndex).getNumCities() > 0)
	{
		return 0;
	}

	// Note - m_aiFoundValue is scaled by a factor of 10 to prevent wrapping since it's only a short (extra policing is present on the set)
	if (m_aiFoundValue[eIndex] == INVALID_FOUND_VALUE)
	{
		setFoundValue(eIndex, GET_PLAYER(eIndex).AI_foundValue(getX(), getY(), -1, true));

		if (area()->hasBestFoundValue(eIndex) && (int) m_aiFoundValue[eIndex] > area()->getBestFoundValue(eIndex))
		{
			area()->setBestFoundValue(eIndex, m_aiFoundValue[eIndex]);
		}
	}
	return m_aiFoundValue[eIndex];
}


bool CvPlot::isBestAdjacentFound(PlayerTypes eIndex)
{
	PROFILE_EXTRA_FUNC();
	const int iPlotValue = GET_PLAYER(eIndex).AI_foundValue(getX(), getY());

	if (iPlotValue == 0)
	{
		return false;
	}

	foreach_(const CvPlot* pAdjacentPlot, adjacent()
	| filtered(CvPlot::fn::isRevealed(GET_PLAYER(eIndex).getTeam(), false)))
	{
		if (GET_PLAYER(eIndex).AI_foundValue(pAdjacentPlot->getX(), pAdjacentPlot->getY()) > iPlotValue)
		{
			return false;
		}
	}

	return true;
}

void CvPlot::clearFoundValue(PlayerTypes eIndex)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (NULL == m_aiFoundValue)
	{
		m_aiFoundValue = new unsigned int[MAX_PLAYERS];
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			m_aiFoundValue[iI] = 0;
		}
	}

	if (NULL != m_aiFoundValue)
	{
		m_aiFoundValue[eIndex] = INVALID_FOUND_VALUE;
	}
}

void CvPlot::setFoundValue(PlayerTypes eIndex, int iNewValue)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);
	FASSERT_NOT_NEGATIVE(iNewValue);

	if (!m_aiFoundValue && 0 != iNewValue)
	{
		CvXMLLoadUtility::InitList<uint32_t>(&m_aiFoundValue, MAX_PLAYERS, 0);
	}

	if (m_aiFoundValue)
	{
		FASSERT_NOT_NEGATIVE(iNewValue);
		m_aiFoundValue[eIndex] = (uint32_t)iNewValue;
	}
}


int CvPlot::getPlayerCityRadiusCount(PlayerTypes eIndex) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	return m_aiPlayerCityRadiusCount ? m_aiPlayerCityRadiusCount[eIndex] : 0;
}


bool CvPlot::isPlayerCityRadius(PlayerTypes eIndex) const
{
	return (getPlayerCityRadiusCount(eIndex) > 0);
}


void CvPlot::changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange)
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, eIndex);

	if (0 != iChange)
	{
		if (NULL == m_aiPlayerCityRadiusCount)
		{
			CvXMLLoadUtility::InitList<char>(&m_aiPlayerCityRadiusCount, MAX_PLAYERS, 0);
		}

		m_aiPlayerCityRadiusCount[eIndex] += iChange;
		FASSERT_NOT_NEGATIVE(getPlayerCityRadiusCount(eIndex));
	}
}


int CvPlot::getPlotGroupId(PlayerTypes ePlayer) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);

	if (NULL == m_aiPlotGroup)
	{
		return FFreeList::INVALID_INDEX;
	}

	return m_aiPlotGroup[ePlayer];
}

CvPlotGroup* CvPlot::getPlotGroup(PlayerTypes ePlayer) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);

	if (NULL == m_aiPlotGroup)
	{
		return GET_PLAYER(ePlayer).getPlotGroup(FFreeList::INVALID_INDEX);
	}

	return GET_PLAYER(ePlayer).getPlotGroup(m_aiPlotGroup[ePlayer]);
}


CvPlotGroup* CvPlot::getOwnerPlotGroup() const
{
	if (getOwner() == NO_PLAYER)
	{
		return NULL;
	}

	return getPlotGroup(getOwner());
}


void CvPlot::setPlotGroup(PlayerTypes ePlayer, CvPlotGroup* pNewValue, bool bRecalculateEffect)
{
	PROFILE_FUNC();

	CvPlotGroup* pOldPlotGroup = getPlotGroup(ePlayer);

	// The id-level check is to handle correction of an old buggy state where
	// invalid ids could be present (which map to a NULL group if resolved through getPlotGroup())
	if (pOldPlotGroup != pNewValue || pNewValue == NULL && getPlotGroupId(ePlayer) != -1)
	{
		if (NULL ==  m_aiPlotGroup)
		{
			m_aiPlotGroup = new int[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_aiPlotGroup[iI] = FFreeList::INVALID_INDEX;
			}
		}
		CvCity* pCity;

		if (bRecalculateEffect)
		{
			pCity = getPlotCity();

			if (ePlayer == getOwner())
			{
				GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();
				updatePlotGroupBonus(false);
			}

			if (pOldPlotGroup != NULL && pCity != NULL && pCity->getOwner() == ePlayer)
			{
				for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					pCity->changeNumBonuses((BonusTypes)iI, -pOldPlotGroup->getNumBonuses((BonusTypes)iI));
				}
			}
		}

		if (pNewValue == NULL)
		{
			m_aiPlotGroup[ePlayer] = FFreeList::INVALID_INDEX;
		}
		else
		{
			m_aiPlotGroup[ePlayer] = pNewValue->getID();
		}

		if (bRecalculateEffect)
		{
			if (pCity != NULL && getPlotGroup(ePlayer) != NULL && pCity->getOwner() == ePlayer)
			{
				for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					pCity->changeNumBonuses((BonusTypes)iI, getPlotGroup(ePlayer)->getNumBonuses((BonusTypes)iI));
				}
			}
			if (ePlayer == getOwner())
			{
				updatePlotGroupBonus(true);

				GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();
			}
		}
	}
}


void CvPlot::updatePlotGroup()
{
	PROFILE_FUNC();

	if (bDeferPlotGroupRecalculation)
	{
		m_bPlotGroupsDirty = true;
	}
	else
	{
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				updatePlotGroup((PlayerTypes)iI);
			}
		}
		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		m_bPlotGroupsDirty = false;
	}
}

void CalculateClosePlotGroupConnectSet(PlayerTypes ePlayer, const CvPlot* pSeedPlot, std::vector<const CvPlot*>& set, int iRange)
{
	PROFILE_EXTRA_FUNC();
	if (!algo::any_of_equal(set, pSeedPlot))
	{
		set.push_back(pSeedPlot);

		if ( iRange > 0 )
		{
			const CvPlotGroup* seedPlotGroup = pSeedPlot->getPlotGroup(ePlayer);

			foreach_(const CvPlot* pAdjacentPlot, pSeedPlot->adjacent())
			{
				if (pAdjacentPlot->getPlotGroup(ePlayer) == seedPlotGroup)
				{
					CalculateClosePlotGroupConnectSet(ePlayer, pAdjacentPlot, set, iRange-1);
				}
			}
		}
	}
}

void CvPlot::updatePlotGroup(PlayerTypes ePlayer, bool bRecalculate, bool bRecalculateBonuses)
{
	PROFILE("CvPlot::updatePlotGroup(Player)");

#ifdef USE_INTERNAL_PROFILER
	if (!IFPIsMainThread())
	{
		::MessageBox(NULL, "Unexpected plotGroup chnage on background thread", "CvGameCoreDLL", MB_OK);
	}
#endif

	if (!GC.getGame().isFinalInitialized())
	{
		return;
	}

	CvPlotGroup* pPlotGroup = getPlotGroup(ePlayer);

	if (pPlotGroup != NULL)
	{
		if (bRecalculate)
		{
			bool bConnected = false;
			FAssert(bRecalculateBonuses); // Should never call without recalc on a plot that already has a group

			if (isTradeNetwork(GET_PLAYER(ePlayer).getTeam()))
			{
				bConnected = true;
//#if 0
//				foreach_(const CvPlot* pAdjacentPlot, adjacent())
//				{
//					if (pAdjacentPlot->getPlotGroup(ePlayer) == pPlotGroup
//					&& !isTradeNetworkConnected(pAdjacentPlot, GET_PLAYER(ePlayer).getTeam()))
//					{
//						bConnected = false;
//						break;
//					}
//				}
//#endif
			}

			if (!bConnected)
			{
				const bool bEmpty = pPlotGroup->getLengthPlots() == 1;
				FAssertMsg(pPlotGroup->getLengthPlots() > 0, "pPlotGroup should have more than 0 plots");

				pPlotGroup->removePlot(this);

				if (!bEmpty)
				{
					bool	bIsSamePlotGroup[NUM_DIRECTION_TYPES];
					int		iSamePlotGroupAdjacentCount = 0;
					const CvPlot* pAdjacentInstance = NULL;

					for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						bIsSamePlotGroup[iI] = false;
					}

					for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						const CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

						if (pAdjacentPlot != NULL && pAdjacentPlot->getPlotGroup(ePlayer) == pPlotGroup)
						{
							iSamePlotGroupAdjacentCount++;
							bIsSamePlotGroup[iI] = true;

							pAdjacentInstance = pAdjacentPlot;
						}
					}

					if (iSamePlotGroupAdjacentCount > 1 && iSamePlotGroupAdjacentCount < 7)
					{
						PROFILE("CvPlot::updatePlotGroup.PotentialRemove");

						std::vector<const CvPlot*> closeConnected;

						CalculateClosePlotGroupConnectSet(ePlayer, pAdjacentInstance, closeConnected, 3);

						int	iStillConnectedCount = 0;

						foreach_(const CvPlot* pAdjacentPlot, adjacent())
						{
							if (pAdjacentPlot->getPlotGroup(ePlayer) == pPlotGroup
							&& algo::any_of_equal(closeConnected, pAdjacentPlot))
							{
								iStillConnectedCount++;
							}
						}
						if (iSamePlotGroupAdjacentCount != iStillConnectedCount)
						{
							PROFILE("CvPlot::updatePlotGroup.Remove");
							pPlotGroup->recalculatePlots();
						}
					}
				}
			}
		}
		pPlotGroup = getPlotGroup(ePlayer);
	}

	if (isTradeNetwork(GET_PLAYER(ePlayer).getTeam()))
	{
		foreach_(CvPlot* pAdjacentPlot, adjacent())
		{
			CvPlotGroup* pAdjacentPlotGroup = pAdjacentPlot->getPlotGroup(ePlayer);

			if (pAdjacentPlotGroup != NULL && pAdjacentPlotGroup != pPlotGroup
			&& isTradeNetworkConnected(pAdjacentPlot, GET_PLAYER(ePlayer).getTeam()))
			{
				if (pPlotGroup == NULL)
				{
					pAdjacentPlotGroup->addPlot(this, bRecalculateBonuses);
					pPlotGroup = pAdjacentPlotGroup;
					FAssertMsg(getPlotGroup(ePlayer) == pPlotGroup, "ePlayer's plot group is expected to equal pPlotGroup");
				}
				else
				{
					FAssertMsg(getPlotGroup(ePlayer) == pPlotGroup, "ePlayer's plot group is expected to equal pPlotGroup");
					CvMap::combinePlotGroups(pPlotGroup, pAdjacentPlotGroup, bRecalculateBonuses);
					pPlotGroup = getPlotGroup(ePlayer);
					FAssertMsg(pPlotGroup != NULL, "PlotGroup is not assigned a valid value");
				}
			}
		}

		if (pPlotGroup == NULL)
		{
			GET_PLAYER(ePlayer).initPlotGroup(this, bRecalculateBonuses);
		}
	}
}


int CvPlot::getVisibilityCount(TeamTypes eTeam) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	return m_aiVisibilityCount[eTeam];
}

int CvPlot::getDangerCount(int /*PlayerTypes*/ ePlayer) const
{
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);

	return m_aiDangerCount ? m_aiDangerCount[ePlayer] : 0;
}

void CvPlot::setDangerCount(int /*PlayerTypes*/ ePlayer, int iNewCount)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_PLAYERS, ePlayer);

	if (NULL == m_aiDangerCount)
	{
		m_aiDangerCount = new short[MAX_PLAYERS];
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			m_aiDangerCount[iI] = 0;
		}
	}

	m_aiDangerCount[ePlayer] = iNewCount;
}


int CvPlot::getLastVisibleTurn(TeamTypes eTeam) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if ( isVisible(eTeam, false) )
	{
		return GC.getGame().getGameTurn();
	}

	return m_aiLastSeenTurn ? m_aiLastSeenTurn[eTeam] : 0;
}

void CvPlot::setLastVisibleTurn(TeamTypes eTeam, short turn)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (NULL == m_aiLastSeenTurn)
	{
		m_aiLastSeenTurn = new short[MAX_TEAMS];
		for (int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			m_aiLastSeenTurn[iI] = 0;
		}
	}

	m_aiLastSeenTurn[eTeam] = turn;
}

void CvPlot::clearVisibilityCounts()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_TEAMS; ++iI)
	{
		m_aiVisibilityCount[iI] = 0;
	}
	SAFE_DELETE(m_aiStolenVisibilityCount);
	if (NULL != m_apaiInvisibleVisibilityCount)
	{
		for (int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount);
	}

	m_resultHashMap->clear();
	g_bestDefenderCache->clear();
	m_aPlotTeamVisibilityIntensity.clear();
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->setDebugCount(0);
			pUnitNode = nextUnitNode(pUnitNode);
		}
		else
		{
			verifyUnitValidPlot();
			pUnitNode = NULL;
		}
	}
	//TBUPDATERECALC
}

void CvPlot::changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eInvisible, bool bUpdatePlotGroups, int iIntensity, int iUnitID)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (iChange != 0)
	{
		const bool bOldVisible = isVisible(eTeam, false);

		if (eInvisible == NO_INVISIBLE)
		{
			m_aiVisibilityCount[eTeam] += iChange;

			if (m_aiVisibilityCount[eTeam] < 0)
			{
				FErrorMsg("Negative value should never happen, capping it to zero");
				m_aiVisibilityCount[eTeam] = 0;
			}
		}
		else
		{
			changeInvisibleVisibilityCount(eTeam, eInvisible, iChange);
			setSpotIntensity(eTeam, eInvisible, iUnitID, iIntensity);
		}

		if (bOldVisible != isVisible(eTeam, false))
		{
			if (isVisible(eTeam, false))
			{
				setRevealed(eTeam, true, false, NO_TEAM, bUpdatePlotGroups);

				algo::for_each(adjacent(), CvPlot::fn::updateRevealedOwner(eTeam));

				if (getTeam() != NO_TEAM)
				{
					GET_TEAM(getTeam()).meet(eTeam, true);
				}
			}
			else
			{
				setLastVisibleTurn( eTeam, GC.getGame().getGameTurn() - 1 );
			}

			CvCity* pCity = getPlotCity();

			if (pCity != NULL)
			{
				pCity->setInfoDirty(true);
			}

			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive() && GET_TEAM((TeamTypes)iI).isStolenVisibility(eTeam))
				{
					changeStolenVisibilityCount((TeamTypes)iI, isVisible(eTeam, false) ? 1 : -1);
				}
			}

			if (eTeam == GC.getGame().getActiveTeam())
			{
				updateFog();
				updateMinimapColor();
				updateCenterUnit();
			}
		}
	}
}


int CvPlot::getStolenVisibilityCount(TeamTypes eTeam) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	return m_aiStolenVisibilityCount ? m_aiStolenVisibilityCount[eTeam] : 0;
}


void CvPlot::changeStolenVisibilityCount(TeamTypes eTeam, int iChange)
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (iChange != 0)
	{
		if (NULL == m_aiStolenVisibilityCount)
		{
			CvXMLLoadUtility::InitList<int16_t>(&m_aiStolenVisibilityCount, MAX_TEAMS, 0);
		}

		const bool bOldVisible = isVisible(eTeam, false);

		m_aiStolenVisibilityCount[eTeam] += iChange;
		FASSERT_NOT_NEGATIVE(getStolenVisibilityCount(eTeam));

		if (bOldVisible != isVisible(eTeam, false))
		{
			if (isVisible(eTeam, false))
			{
				setRevealed(eTeam, true, false, NO_TEAM, true);
			}
			else
			{
				setLastVisibleTurn( eTeam, GC.getGame().getGameTurn() - 1 );
			}

			CvCity* pCity = getPlotCity();

			if (pCity != NULL)
			{
				pCity->setInfoDirty(true);
			}

			if (eTeam == GC.getGame().getActiveTeam())
			{
				updateFog();
				updateMinimapColor();
				updateCenterUnit();
			}
		}
	}
}

int CvPlot::getBlockadedCount(TeamTypes eTeam) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	return m_aiBlockadedCount ? std::max<int>(0, m_aiBlockadedCount[eTeam]) : 0;
}

void CvPlot::resetBlockadedCounts()
{
	SAFE_DELETE_ARRAY(m_aiBlockadedCount);
}

void CvPlot::changeBlockadedCount(TeamTypes eTeam, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (iChange != 0)
	{
		if (NULL == m_aiBlockadedCount)
		{
			m_aiBlockadedCount = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aiBlockadedCount[iI] = 0;
			}
		}

		m_aiBlockadedCount[eTeam] += iChange;

		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                      06/01/09                                jdog5000      */
		/* Bugfix                                                                                       */
		/************************************************************************************************/
		FAssertMsg(m_aiBlockadedCount[eTeam] >= 0, CvString::format("Blockaded count on a plot should not go lower than 0, it is now %d", getBlockadedCount(eTeam)).c_str());
		FAssertMsg(m_aiBlockadedCount[eTeam] == 0 || isWater(), "Non water tiles cannot have a non-zero Blockaded count");

		// Hack so that never get negative blockade counts as a result of fixing issue causing
		// rare permanent blockades.
		// TB Note: this can throw off counts and we have false additional amounts soooo,
		// the negative may be necessary based on the order of things taking place and
		// when you try to 'fix' it like this you create a permanent blockade.
		//if( getBlockadedCount(eTeam) < 0 )
		//{
		//	m_aiBlockadedCount[eTeam] = 0;
		//}
		/************************************************************************************************/
		/* BETTER_BTS_AI_MOD                       END                                                  */
		/************************************************************************************************/

		CvCity* pWorkingCity = getWorkingCity();
		if (NULL != pWorkingCity)
		{
			pWorkingCity->AI_setAssignWorkDirty(true);
		}
	}
}

PlayerTypes CvPlot::getRevealedOwner(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
	{
		return getOwner();
	}
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	return m_aiRevealedOwner ? (PlayerTypes)m_aiRevealedOwner[eTeam] : NO_PLAYER;
}


TeamTypes CvPlot::getRevealedTeam(TeamTypes eTeam, bool bDebug) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	const PlayerTypes eRevealedOwner = getRevealedOwner(eTeam, bDebug);

	return eRevealedOwner != NO_PLAYER ? GET_PLAYER(eRevealedOwner).getTeam() : NO_TEAM;
}


void CvPlot::setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (getRevealedOwner(eTeam, false) != eNewValue)
	{
		if (NULL == m_aiRevealedOwner)
		{
			m_aiRevealedOwner = new char[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aiRevealedOwner[iI] = -1;
			}
		}

		m_aiRevealedOwner[eTeam] = eNewValue;

		if (eTeam == GC.getGame().getActiveTeam())
		{
			updateMinimapColor();

			if (GC.IsGraphicsInitialized())
			{
				gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

				gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
			}
		}
	}

	FAssert((NULL == m_aiRevealedOwner) || (m_aiRevealedOwner[eTeam] == eNewValue));
}


void CvPlot::updateRevealedOwner(TeamTypes eTeam)
{
	PROFILE_FUNC();

	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (isVisible(eTeam, false) || algo::any_of(adjacent(), CvPlot::fn::isVisible(eTeam, false)))
	{
		setRevealedOwner(eTeam, getOwner());
	}
}


bool CvPlot::isRiverCrossing(DirectionTypes eIndex) const
{
	FAssertMsg(eIndex < NUM_DIRECTION_TYPES, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (eIndex == NO_DIRECTION)
	{
		return false;
	}

	if (NULL == m_abRiverCrossing)
	{
		return false;
	}

	return m_abRiverCrossing[eIndex];
}


void CvPlot::updateRiverCrossing(DirectionTypes eIndex)
{
	PROFILE_FUNC();

	CvPlot* pNorthEastPlot;
	CvPlot* pSouthEastPlot;
	CvPlot* pSouthWestPlot;
	CvPlot* pNorthWestPlot;

	FASSERT_BOUNDS(0, NUM_DIRECTION_TYPES, eIndex);

	CvPlot* pCornerPlot = NULL;
	bool bValid = false;
	CvPlot* pPlot = plotDirection(getX(), getY(), eIndex);

	if ((NULL == pPlot || !pPlot->isWater()) && !isWater())
	{
		switch (eIndex)
		{
		case DIRECTION_NORTH:
			if (pPlot != NULL)
			{
				bValid = pPlot->isNOfRiver();
			}
			break;

		case DIRECTION_NORTHEAST:
			pCornerPlot = plotDirection(getX(), getY(), DIRECTION_NORTH);
			break;

		case DIRECTION_EAST:
			bValid = isWOfRiver();
			break;

		case DIRECTION_SOUTHEAST:
			pCornerPlot = this;
			break;

		case DIRECTION_SOUTH:
			bValid = isNOfRiver();
			break;

		case DIRECTION_SOUTHWEST:
			pCornerPlot = plotDirection(getX(), getY(), DIRECTION_WEST);
			break;

		case DIRECTION_WEST:
			if (pPlot != NULL)
			{
				bValid = pPlot->isWOfRiver();
			}
			break;

		case DIRECTION_NORTHWEST:
			pCornerPlot = plotDirection(getX(), getY(), DIRECTION_NORTHWEST);
			break;

		default:
			FErrorMsg("error");
			break;
		}

		if (pCornerPlot != NULL)
		{
			pNorthEastPlot = plotDirection(pCornerPlot, DIRECTION_EAST);
			pSouthEastPlot = plotDirection(pCornerPlot, DIRECTION_SOUTHEAST);
			pSouthWestPlot = plotDirection(pCornerPlot, DIRECTION_SOUTH);
			pNorthWestPlot = pCornerPlot;

			if (pSouthWestPlot && pNorthWestPlot && pSouthEastPlot && pNorthEastPlot)
			{
				if (pSouthWestPlot->isWOfRiver() && pNorthWestPlot->isWOfRiver())
				{
					bValid = true;
				}
				else if (pNorthEastPlot->isNOfRiver() && pNorthWestPlot->isNOfRiver())
				{
					bValid = true;
				}
				else if ((eIndex == DIRECTION_NORTHEAST) || (eIndex == DIRECTION_SOUTHWEST))
				{
					if (pNorthEastPlot->isNOfRiver() && (pNorthWestPlot->isWOfRiver() || pNorthWestPlot->isWater()))
					{
						bValid = true;
					}
					else if ((pNorthEastPlot->isNOfRiver() || pSouthEastPlot->isWater()) && pNorthWestPlot->isWOfRiver())
					{
						bValid = true;
					}
					else if (pSouthWestPlot->isWOfRiver() && (pNorthWestPlot->isNOfRiver() || pNorthWestPlot->isWater()))
					{
						bValid = true;
					}
					else if ((pSouthWestPlot->isWOfRiver() || pSouthEastPlot->isWater()) && pNorthWestPlot->isNOfRiver())
					{
						bValid = true;
					}
				}
				else
				{
					FAssert((eIndex == DIRECTION_SOUTHEAST) || (eIndex == DIRECTION_NORTHWEST));

					if (pNorthWestPlot->isNOfRiver() && (pNorthWestPlot->isWOfRiver() || pNorthEastPlot->isWater()))
					{
						bValid = true;
					}
					else if ((pNorthWestPlot->isNOfRiver() || pSouthWestPlot->isWater()) && pNorthWestPlot->isWOfRiver())
					{
						bValid = true;
					}
					else if (pNorthEastPlot->isNOfRiver() && (pSouthWestPlot->isWOfRiver() || pSouthWestPlot->isWater()))
					{
						bValid = true;
					}
					else if ((pNorthEastPlot->isNOfRiver() || pNorthEastPlot->isWater()) && pSouthWestPlot->isWOfRiver())
					{
						bValid = true;
					}
				}
			}

		}
	}

	if (isRiverCrossing(eIndex) != bValid)
	{
		if (!m_abRiverCrossing)
		{
			m_abRiverCrossing = new bool[NUM_DIRECTION_TYPES];
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				m_abRiverCrossing[iI] = false;
			}
		}

		m_abRiverCrossing[eIndex] = bValid;

		changeRiverCrossingCount(isRiverCrossing(eIndex) ? 1 : -1);
	}
}


void CvPlot::updateRiverCrossing()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		updateRiverCrossing((DirectionTypes)iI);
	}
}


bool CvPlot::isRevealed(TeamTypes eTeam, bool bDebug) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (bDebug && GC.getGame().isDebugMode())
	{
		return true;
	}

	return m_abRevealed ? m_abRevealed[eTeam] : false;
}


void CvPlot::setRevealed(const TeamTypes eTeam, const bool bNewValue, const bool bTerrainOnly, const TeamTypes eFromTeam, const bool bUpdatePlotGroup, const bool bUpdateFog)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	if (isNull() && !isRevealed(eTeam, false))
		return;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	if (isRevealed(eTeam, false) != bNewValue)
	{
		if (NULL == m_abRevealed)
		{
			m_abRevealed = new bool[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_abRevealed[iI] = false;
			}
		}

		m_abRevealed[eTeam] = bNewValue;

		if (area())
		{
			area()->changeNumRevealedTiles(eTeam, bNewValue ? 1 : -1);
		}

		if (bUpdatePlotGroup)
		{
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAliveAndTeam(eTeam))
				{
					updatePlotGroup((PlayerTypes)iI);
				}
			}
		}

		if (eTeam == GC.getGame().getActiveTeam())
		{
			if (GC.viewportsEnabled() && shouldHaveGraphics())
			{
				foreach_(CvPlot* pAdjacentPlot, adjacent())
				{
					// Unrevealed water plots adjacent to newly revealed plots need redrawing to prevent artifacting
					if (pAdjacentPlot->isWater() && !pAdjacentPlot->isRevealed(eTeam, false) && pAdjacentPlot->isInViewport())
					{
						gDLL->getEngineIFace()->RebuildPlot(pAdjacentPlot->getViewportX(), pAdjacentPlot->getViewportY(),true,true);
						pAdjacentPlot->setLayoutDirty(true);
					}
				}

				if (isWater())
				{
					GC.getGame().noteGraphicRebuildNeeded();
				}

				//Update terrain graphics
				gDLL->getEngineIFace()->RebuildPlot(getViewportX(), getViewportY(),true,true);
			}

			hideGraphics(ECvPlotGraphics::ALL);
			if (bNewValue)
			{
				updateGraphics();
			}
			if (bUpdateFog)
			{
				updateFog();
			}
			updateVisibility();

			gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		}

		if (bNewValue)
		{
			CvEventReporter::getInstance().plotRevealed(this, eTeam);

			if (getLandmarkType() != NO_LANDMARK)
			{
				for (int iJ = 0; iJ < MAX_PC_PLAYERS; ++iJ)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAliveAndTeam(eTeam))
					{
						addSign((PlayerTypes)iJ, getLandmarkMessage());

						if (eFromTeam == NO_TEAM) // Not from map trade
						{
							bool bFirstToDiscover = true;
							for (int iI = 0; iI < MAX_PC_TEAMS; ++iI)
							{
								if (iI != eTeam && GET_TEAM((TeamTypes)iI).isAlive() && isRevealed((TeamTypes)iI, false))
								{
									bFirstToDiscover = false;
									break;
								}
							}
							if (bFirstToDiscover)
							{
								CvString szIcon;

								if (getLandmarkType() == LANDMARK_FOREST || getLandmarkType() == LANDMARK_JUNGLE)
								{
									szIcon = GC.getFeatureInfo(getFeatureType()).getButton();
								}
								else if (getLandmarkType() == LANDMARK_MOUNTAIN_RANGE || getLandmarkType() == LANDMARK_PEAK)
								{
									szIcon = GC.getTerrainInfo(GC.getTERRAIN_PEAK()).getButton();
								}
								else szIcon = GC.getTerrainInfo(getTerrainType()).getButton();

								AddDLLMessage(
									(PlayerTypes)iJ, false, GC.getEVENT_MESSAGE_TIME(),
									gDLL->getText("TXT_KEY_MISC_DISCOVERED_LANDMARK"),
									"AS2D_TECH_GENERIC", MESSAGE_TYPE_MINOR_EVENT, szIcon,
									GC.getCOLOR_WHITE(), getX(), getY(), true, true
								);
							}
						}
					}
				}
			}
		}
	}

	if (!bTerrainOnly)
	{
		CvCity* pCity = getPlotCity();

		if (!bNewValue) // Un-reveal plot
		{
			setRevealedOwner(eTeam, NO_PLAYER);
			setRevealedImprovementType(eTeam, NO_IMPROVEMENT);
			setRevealedRouteType(eTeam, NO_ROUTE);

			if (pCity != NULL)
			{
				pCity->setRevealed(eTeam, false);
			}
		}
		else if (eFromTeam == NO_TEAM)
		{
			setRevealedOwner(eTeam, getOwner());
			setRevealedImprovementType(eTeam, getImprovementType());
			setRevealedRouteType(eTeam, getRouteType());

			if (pCity != NULL)
			{
				pCity->setRevealed(eTeam, true);
			}
		}
		else // Map trade
		{
			if (getRevealedOwner(eFromTeam, false) == getOwner())
			{
				setRevealedOwner(eTeam, getOwner());
			}

			if (getRevealedImprovementType(eFromTeam) == getImprovementType())
			{
				setRevealedImprovementType(eTeam, getImprovementType());
			}

			if (getRevealedRouteType(eFromTeam, false) == getRouteType())
			{
				setRevealedRouteType(eTeam, getRouteType());
			}

			if (pCity != NULL && pCity->isRevealed(eFromTeam, false))
			{
				pCity->setRevealed(eTeam, true);
			}
		}
	}
}

bool CvPlot::isAdjacentRevealed(TeamTypes eTeam, bool bDebug) const
{
	return algo::any_of(adjacent(), CvPlot::fn::isRevealed(eTeam, bDebug));
}

bool CvPlot::isAdjacentNonrevealed(TeamTypes eTeam) const
{
	return algo::any_of(adjacent(), !CvPlot::fn::isRevealed(eTeam, false));
}


ImprovementTypes CvPlot::getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
	{
		return getImprovementType();
	}

	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (NULL == m_aeRevealedImprovementType)
	{
		return NO_IMPROVEMENT;
	}

	return (ImprovementTypes)m_aeRevealedImprovementType[eTeam];
}


void CvPlot::setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (getRevealedImprovementType(eTeam) != eNewValue)
	{
		if (NULL == m_aeRevealedImprovementType)
		{
			m_aeRevealedImprovementType = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aeRevealedImprovementType[iI] = NO_IMPROVEMENT;
			}
		}

		m_aeRevealedImprovementType[eTeam] = eNewValue;

		if (eTeam == GC.getGame().getActiveTeam())
		{
			updateSymbols();
			setLayoutDirty(true);
			//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
		}
	}
}


RouteTypes CvPlot::getRevealedRouteType(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
	{
		return getRouteType();
	}

	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (NULL == m_aeRevealedRouteType)
	{
		return NO_ROUTE;
	}

	return (RouteTypes)m_aeRevealedRouteType[eTeam];
}


void CvPlot::setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

	if (getRevealedRouteType(eTeam, false) != eNewValue)
	{
		if (NULL == m_aeRevealedRouteType)
		{
			m_aeRevealedRouteType = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aeRevealedRouteType[iI] = NO_ROUTE;
			}
		}

		m_aeRevealedRouteType[eTeam] = eNewValue;

		if (eTeam == GC.getGame().getActiveTeam())
		{
			updateSymbols();
			updateRouteSymbol(true, true);
		}
	}
}


int CvPlot::getBuildProgress(BuildTypes eBuild) const
{
	return m_paiBuildProgress ? m_paiBuildProgress[eBuild] : 0;
}


// Returns true if build finished...
bool CvPlot::changeBuildProgress(BuildTypes eBuild, int iChange, TeamTypes eTeam)
{
	PROFILE_EXTRA_FUNC();
	CvCity* pCity = NULL;
	int iProduction;

	bool bFinished = false;
	bool bCultureDistUpdate = false;

	if (iChange != 0)
	{
		if (NULL == m_paiBuildProgress)
		{
			CvXMLLoadUtility::InitList(&m_paiBuildProgress, GC.getNumBuildInfos(), 0);
		}

		m_paiBuildProgress[eBuild] += iChange;
		FASSERT_NOT_NEGATIVE(getBuildProgress(eBuild));

		BonusTypes eBonusPlaced = NO_BONUS;
		if (getBuildProgress(eBuild) >= getBuildTime(eBuild))
		{
			m_paiBuildProgress[eBuild] = 0;

			//TB Bonus Placement
			if (!GC.getBuildInfo(eBuild).getPlaceBonusTypes().empty())
			{
				std::vector<int> m_aBonusResult;
				m_aBonusResult.clear();
				bool bAccessFound = false;
				foreach_(const PlaceBonusTypes& obj, GC.getBuildInfo(eBuild).getPlaceBonusTypes())
				{
					const BonusTypes eBonus = obj.eBonus;
					bAccessFound = false;
					if (obj.bRequiresAccess)
					{
						if (GET_PLAYER(getOwner()).hasBonus(eBonus))
						{
							bAccessFound = true;
						}
					}
					else
					{
						bAccessFound = true;
					}
					//Check MapCategoryType
					if (bAccessFound)
					{
						if (obj.ePrereqMapCategory != NO_MAPCATEGORY)
						{
							bAccessFound = false;
							if (isMapCategoryType(obj.ePrereqMapCategory))
							{
								bAccessFound = true;
							}
						}
					}
					if (bAccessFound)
					{
						if (obj.ePrereqTerrain != NO_TERRAIN)
						{
							bAccessFound = false;
							if (getTerrainType() == obj.ePrereqTerrain)
							{
								bAccessFound = true;
							}
						}
					}
					if (bAccessFound)
					{
						if (obj.ePrereqTech != NO_TECH)
						{
							bAccessFound = false;
							if (GET_TEAM(getTeam()).isHasTech(obj.ePrereqTech))
							{
								bAccessFound = true;
							}
						}
					}
					if (bAccessFound)
					{
						if (obj.ePrereqFeature != NO_FEATURE)
						{
							bAccessFound = false;
							if (getFeatureType() == obj.ePrereqFeature)
							{
								bAccessFound = true;
							}
						}
					}
					if (bAccessFound)
					{
						bAccessFound = false;
						if(canHaveBonus(obj.eBonus))
						{
							bAccessFound = true;
						}
					}
					if (bAccessFound)
					{
						for (int iK = 0; iK < obj.iProbability; iK++)
						{
							m_aBonusResult.push_back((int)obj.eBonus);
						}
					}
				}
				if (!m_aBonusResult.empty())
				{
					const int iPossible = std::max((int)m_aBonusResult.size(), 100);
					const uint32_t iResult = GC.getGame().getSorenRandNum(iPossible, "Select Bonus Placement Type");

					if (iResult > m_aBonusResult.size())
					{
						eBonusPlaced = NO_BONUS;
					}
					else
					{
						eBonusPlaced = (BonusTypes)m_aBonusResult[iResult];
					}
				}
				if (eBonusPlaced != NO_BONUS)
				{
					setBonusType(eBonusPlaced);

					if ( isInViewport() )
					{
						const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLACED_BONUS", GC.getBonusInfo(eBonusPlaced).getTextKeyWide());
						AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, GC.getBonusInfo(getBonusType()).getButton(), GC.getCOLOR_WHITE(), getViewportX(),getViewportY(), true, true);
					}
				}
				else
				{
					if ( isInViewport() )
					{
						const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PLACED_BONUS_FAIL");
						AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, 0, GC.getCOLOR_RED(), getViewportX(),getViewportY(), true, true);
					}
				}
			}

			// Place improvement, so long as it isn't a placeholder improvement for FeatureChange or TerrainChange
			if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT
			&&  GC.getBuildInfo(eBuild).getFeatureChange() == NO_FEATURE
			&&  GC.getBuildInfo(eBuild).getTerrainChange() == NO_TERRAIN)
			{
				setImprovementType(GC.getBuildInfo(eBuild).getImprovement());
			}

			if (GC.getBuildInfo(eBuild).getRoute() != NO_ROUTE)
			{
				setRouteType((RouteTypes)GC.getBuildInfo(eBuild).getRoute(), true);
			}

			//TB Note: Apparently isFeatureRemove is based entirely on the BUILD definition -
			// changing it to the improvement definition would enable us to restructure
			// where this takes place so that a 'natural' upgrade can have the same effect.
			if (getFeatureType() != NO_FEATURE)
			{
				if (GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()))
				{
					FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);

					iProduction = getFeatureProduction(eBuild, eTeam, &pCity);

					if (iProduction > 0)
					{
						pCity->changeFeatureProduction(iProduction);

						if ( isInViewport() )
						{
							const CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CLEARING_FEATURE_BONUS", GC.getFeatureInfo(getFeatureType()).getTextKeyWide(), iProduction, pCity->getNameKey());
							AddDLLMessage(pCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, GC.getFeatureInfo(getFeatureType()).getButton(), GC.getCOLOR_WHITE(), getViewportX(),getViewportY(), true, true);
						}
					}

					// Python Event
					CvEventReporter::getInstance().plotFeatureRemoved(this, getFeatureType(), pCity);

					setFeatureType(NO_FEATURE);
					bCultureDistUpdate = true;
				}
			}

			// Set feature/terrain after removal of existing feature if necessary
			if (GC.getBuildInfo(eBuild).getTerrainChange() != NO_TERRAIN)
			{
				setTerrainType ((TerrainTypes) GC.getBuildInfo(eBuild).getTerrainChange());
				bCultureDistUpdate = true;
			}
			if (GC.getBuildInfo(eBuild).getFeatureChange() != NO_FEATURE)
			{
				setFeatureType ((FeatureTypes) GC.getBuildInfo(eBuild).getFeatureChange());
				bCultureDistUpdate = true;
			}

			bFinished = true;
		}
	}

	return bFinished;
}


// BUG - Partial Builds - start
/*
 * Returns true if the build progress array has been created; false otherwise.
 * A false return value implies that every build has zero progress.
 * A true return value DOES NOT imply that any build has a non-zero progress--just the possibility.
 */
bool CvPlot::hasAnyBuildProgress() const
{
	return NULL != m_paiBuildProgress;
}
// BUG - Partial Builds - end


void CvPlot::updateFeatureSymbolVisibility()
{
	PROFILE_FUNC();

	if (!isGraphicsVisible(ECvPlotGraphics::FEATURE))
	{
		return;
	}

	if (m_pFeatureSymbol != NULL)
	{
		bool bVisible = isRevealed(GC.getGame().getActiveTeam(), true);
		if(getFeatureType() != NO_FEATURE)
		{
			if(GC.getFeatureInfo(getFeatureType()).isVisibleAlways())
				bVisible = true;
		}

		bool wasVisible = !gDLL->getFeatureIFace()->IsHidden(m_pFeatureSymbol);
		if(wasVisible != bVisible)
		{
			gDLL->getFeatureIFace()->Hide(m_pFeatureSymbol, !bVisible);
			gDLL->getEngineIFace()->MarkPlotTextureAsDirty(getViewportX(),getViewportY());
		}
	}
}


void CvPlot::updateFeatureSymbol(bool bForce)
{
	PROFILE_FUNC();

	if (!isGraphicsVisible(ECvPlotGraphics::FEATURE) && !bForce)
	{
		return;
	}

	const FeatureTypes eFeature = getFeatureType();

	gDLL->getEngineIFace()->RebuildTileArt(getViewportX(),getViewportY());

	if ( eFeature == NO_FEATURE ||
		 GC.getFeatureInfo(eFeature).getArtInfo()->isRiverArt() ||
		 GC.getFeatureInfo(eFeature).getArtInfo()->getTileArtType() != TILE_ART_TYPE_NONE ||
		 (GC.viewportsEnabled() && !isRevealed(GC.getGame().getActiveTeam(), true)) )
	{
		gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
		return;
	}

	if (bForce || (m_pFeatureSymbol == NULL) || (gDLL->getFeatureIFace()->getFeature(m_pFeatureSymbol) != eFeature))
	{
		gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
		m_pFeatureSymbol = gDLL->getFeatureIFace()->createFeature();

		FAssertMsg(m_pFeatureSymbol != NULL, "m_pFeatureSymbol is not expected to be equal with NULL");

		gDLL->getFeatureIFace()->init(m_pFeatureSymbol, 0, 0, eFeature, this);

		updateFeatureSymbolVisibility();
	}
	else
	{
		gDLL->getEntityIFace()->updatePosition((CvEntity*)m_pFeatureSymbol); //update position and contours
	}
}


CvRoute* CvPlot::getRouteSymbol() const
{
	return m_pRouteSymbol;
}


// XXX route symbols don't really exist anymore...
void CvPlot::updateRouteSymbol(bool bForce, bool bAdjacent)
{
	PROFILE_FUNC();

	if ( !isGraphicsVisible(ECvPlotGraphics::ROUTE) )
	{
		return;
	}

	if (bAdjacent)
	{
		algo::for_each(adjacent(), CvPlot::fn::updateRouteSymbol(bForce, false));
	}

	const RouteTypes eRoute = getRevealedRouteType(GC.getGame().getActiveTeam(), true);

	if (eRoute == NO_ROUTE)
	{
		gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
		return;
	}

	if (bForce || (m_pRouteSymbol == NULL) || (gDLL->getRouteIFace()->getRoute(m_pRouteSymbol) != eRoute))
	{
		gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
		m_pRouteSymbol = gDLL->getRouteIFace()->createRoute();
		FAssertMsg(m_pRouteSymbol != NULL, "m_pRouteSymbol is not expected to be equal with NULL");

		gDLL->getRouteIFace()->init(m_pRouteSymbol, 0, 0, eRoute, this);
		setLayoutDirty(true);
	}
	else
	{
		gDLL->getEntityIFace()->updatePosition((CvEntity *)m_pRouteSymbol); //update position and contours
	}
}


CvRiver* CvPlot::getRiverSymbol() const
{
	return m_pRiverSymbol;
}


CvFeature* CvPlot::getFeatureSymbol() const
{
	return m_pFeatureSymbol;
}

void CvPlot::updateRiverSymbol(bool bForce, bool bAdjacent)
{
	PROFILE_FUNC();

	if (!isGraphicsVisible(ECvPlotGraphics::RIVER))
	{
		return;
	}

	if (bAdjacent)
	{
		algo::for_each(adjacent(), CvPlot::fn::updateRiverSymbol(bForce, false));
	}

	if (!isRiverMask() ||
		(GC.viewportsEnabled() &&
		 ((!isRevealed(GC.getGame().getActiveTeam(), true) && GC.getCurrentViewport()->getSpoofHiddenGraphics() == VIEWPORT_SPOOF_ALL_UNREVEALED) ||
		  !isAdjacentRevealed(GC.getGame().getActiveTeam(), true))))
	{
		gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
		return;
	}

	if (bForce || (m_pRiverSymbol == NULL))
	{
		//create river
		gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
		m_pRiverSymbol = gDLL->getRiverIFace()->createRiver();
		FAssertMsg(m_pRiverSymbol != NULL, "m_pRiverSymbol is not expected to be equal with NULL");
		gDLL->getRiverIFace()->init(m_pRiverSymbol, 0, 0, 0, this);

		//force tree cuts for adjacent plots
		DirectionTypes affectedDirections[] = {NO_DIRECTION, DIRECTION_EAST, DIRECTION_SOUTHEAST, DIRECTION_SOUTH};
		for(int i=0;i<4;i++)
		{
			const CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), affectedDirections[i]);
			if (pAdjacentPlot != NULL && pAdjacentPlot->isInViewport())
			{
				gDLL->getEngineIFace()->ForceTreeOffsets(getViewportX(),getViewportY());
			}
		}

		//cut out canyons
		gDLL->getEngineIFace()->RebuildRiverPlotTile(getViewportX(),getViewportY(), true, false);

		//recontour adjacent rivers
		foreach_(const CvPlot* pAdjacentPlot, adjacent())
		{
			if (pAdjacentPlot->m_pRiverSymbol != NULL)
			{
				gDLL->getEntityIFace()->updatePosition((CvEntity *)pAdjacentPlot->m_pRiverSymbol); //update position and contours
			}
		}

		// update the symbol
		setLayoutDirty(true);
	}

	//recontour rivers
	gDLL->getEntityIFace()->updatePosition((CvEntity *)m_pRiverSymbol); //update position and contours
}


void CvPlot::updateRiverSymbolArt(bool bAdjacent)
{
	PROFILE_EXTRA_FUNC();
	//this is used to update floodplain features
	if ( !GC.viewportsEnabled() || isRevealed(GC.getGame().getActiveTeam(), true) )
	{
		gDLL->getEntityIFace()->setupFloodPlains(m_pRiverSymbol);
	}

	if(bAdjacent)
	{
		foreach_(const CvPlot* pAdjacentPlot, adjacent())
		{
			if (pAdjacentPlot->m_pRiverSymbol != NULL && (!GC.viewportsEnabled() || pAdjacentPlot->isRevealed(GC.getGame().getActiveTeam(), true)))
			{
				gDLL->getEntityIFace()->setupFloodPlains(pAdjacentPlot->m_pRiverSymbol);
			}
		}
	}
}


CvFlagEntity* CvPlot::getFlagSymbol() const
{
	return m_pFlagSymbol;
}

CvFlagEntity* CvPlot::getFlagSymbolOffset() const
{
	return m_pFlagSymbolOffset;
}

void CvPlot::updateFlagSymbol()
{
	PROFILE_FUNC();

	if (!isGraphicsVisible(ECvPlotGraphics::UNIT))
	{
		return;
	}
	PlayerTypes ePlayer = NO_PLAYER;
	PlayerTypes ePlayerOffset = NO_PLAYER;

	CvUnit* pCenterUnit = getCenterUnit(GC.getGame().isDebugMode());

	//get the plot's unit's flag
	//The plot check is to account for units in the delayed-death cycle
	if (pCenterUnit && pCenterUnit->plot() == this)
	{
		//OutputDebugString(CvString::format("Updating flag symbol for (%d,%d).  Center unit is %08lx\n", m_iX, m_iY, pCenterUnit).c_str());
		ePlayer = pCenterUnit->getVisualOwner();
	}

	//get moving unit's flag
	if (gDLL->getInterfaceIFace()->getSingleMoveGotoPlot() == this)
	{
		if (ePlayer == NO_PLAYER)
		{
			ePlayer = GC.getGame().getActivePlayer();
		}
		else
		{
			ePlayerOffset = GC.getGame().getActivePlayer();
		}
	}

	//don't put two of the same flags
	if (ePlayerOffset == ePlayer)
	{
		ePlayerOffset = NO_PLAYER;
	}

	//destroy old flags
	if (ePlayer == NO_PLAYER)
	{
		gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
	}
	if (ePlayerOffset == NO_PLAYER)
	{
		gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
	}

	//create and/or update unit's flag
	if (ePlayer != NO_PLAYER)
	{
		CvFlagEntity* newFlagEntity = gDLL->getFlagEntityIFace()->create(ePlayer);
		if (m_pFlagSymbol)
		{
			gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
		}
		m_pFlagSymbol = newFlagEntity;

		if (newFlagEntity)
		{
			gDLL->getFlagEntityIFace()->setPlot(newFlagEntity, this, false);
			gDLL->getFlagEntityIFace()->updateUnitInfo(newFlagEntity, this, false);
		}
	}

	//create and/or update offset flag
	if (ePlayerOffset != NO_PLAYER)
	{
		if (!m_pFlagSymbolOffset || gDLL->getFlagEntityIFace()->getPlayer(m_pFlagSymbolOffset) != ePlayerOffset)
		{
			if (m_pFlagSymbolOffset)
			{
				gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
			}
			m_pFlagSymbolOffset = gDLL->getFlagEntityIFace()->create(ePlayerOffset);
			if (m_pFlagSymbolOffset)
			{
				gDLL->getFlagEntityIFace()->setPlot(m_pFlagSymbolOffset, this, true);
			}
		}

		if (m_pFlagSymbolOffset)
		{
			gDLL->getFlagEntityIFace()->updateUnitInfo(m_pFlagSymbolOffset, this, true);
		}
	}
}


/*DllExport*/ CvUnit* CvPlot::getCenterUnit() const
{
#ifdef _DEBUG
	OutputDebugString(CvString::format("exe calls getCenterUnit() for plot at (%d, %d)\n", getX(), getY()).c_str());
#endif
	return m_pCenterUnit;
}

/*DllExport*/ CvUnit* CvPlot::getDebugCenterUnit() const
{
#ifdef _DEBUG
	OutputDebugString(CvString::format("exe calls getDebugCenterUnit() for plot at (%d, %d)\n", getX(), getY()).c_str());
#endif
	if (m_pCenterUnit || !GC.getGame().isDebugMode()) return m_pCenterUnit;

	const CLLNode<IDInfo>* pUnitNode = headUnitNode();
	return pUnitNode ? ::getUnit(pUnitNode->m_data) : NULL;
}

CvUnit* CvPlot::getCenterUnit(const bool bForced) const
{
	if (m_pCenterUnit || !bForced) return m_pCenterUnit;

	const CLLNode<IDInfo>* pUnitNode = headUnitNode();
	return pUnitNode ? ::getUnit(pUnitNode->m_data) : NULL;
}

void CvPlot::updateCenterUnit()
{
	PROFILE_FUNC();

	if (m_bInhibitCenterUnitCalculation || !isGraphicsVisible(ECvPlotGraphics::UNIT))
	{
		//	Because we are inhibitting recalculation until all the adjusting code has run
		//	(rather than have it update multiple times) the currently cached center unit
		//	might become an invalid pointer in the interim.  To protect against this we unconditionally
		//	set the recorded center unit to NULL (without marking the plot dirty which would force a repaint).
		//	This makes the interim a safe state, and the correct value will be calculated once the
		//	inhibitted section is exited
		m_pCenterUnit = NULL;
		return;
	}
	CvUnit* newCenterUnit = isActiveVisible(true) ? getPreferredCenterUnit() : NULL;
	if (!newCenterUnit && gDLL->GetWorldBuilderMode())
	{
		const CLLNode<IDInfo>* pUnitNode = headUnitNode();
		newCenterUnit = pUnitNode ? ::getUnit(pUnitNode->m_data) : NULL;
	}

	if (newCenterUnit != m_pCenterUnit)
	{
		if (newCenterUnit)
		{
			newCenterUnit->reloadEntity(true);
		}
		m_pCenterUnit = newCenterUnit;

		updateMinimapColor();
		setFlagDirty(true);

		if (newCenterUnit)
		{
			newCenterUnit->setInfoBarDirty(true);
		}
	}
}


int CvPlot::getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const
{
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	FASSERT_BOUNDS(0, GC.getNumInvisibleInfos(), eInvisible);

	if (NULL == m_apaiInvisibleVisibilityCount
	||  NULL == m_apaiInvisibleVisibilityCount[eTeam])
	{
		return 0;
	}
	return m_apaiInvisibleVisibilityCount[eTeam][eInvisible];
}


// Is plot within sight range of a eTeam owned unit with any ability to spot eInvisible
bool CvPlot::isSpotterInSight(TeamTypes eTeam, InvisibleTypes eInvisible)	const
{
	return getInvisibleVisibilityCount(eTeam, eInvisible) > 0;
}


void CvPlot::changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange)
{
	PROFILE_EXTRA_FUNC();
	FASSERT_BOUNDS(0, MAX_TEAMS, eTeam);
	FASSERT_BOUNDS(0, GC.getNumInvisibleInfos(), eInvisible);

	if (iChange != 0)
	{
		if (NULL == m_apaiInvisibleVisibilityCount)
		{
			m_apaiInvisibleVisibilityCount = new short*[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_apaiInvisibleVisibilityCount[iI] = NULL;
			}
		}

		if (NULL == m_apaiInvisibleVisibilityCount[eTeam])
		{
			m_apaiInvisibleVisibilityCount[eTeam] = new short[GC.getNumInvisibleInfos()];
			for (int iI = 0; iI < GC.getNumInvisibleInfos(); ++iI)
			{
				m_apaiInvisibleVisibilityCount[eTeam][iI] = 0;
			}
		}
		m_apaiInvisibleVisibilityCount[eTeam][eInvisible] += iChange;
	}
}

void CvPlot::setSpotIntensity(TeamTypes eTeam, InvisibleTypes eInvisible, int iUnitID, int iIntensity)
{
	PROFILE_EXTRA_FUNC();
	if (!GC.getGame().isOption(GAMEOPTION_COMBAT_HIDE_SEEK))
	{
		return;
	}
	bool bRelevant = isSpotterInSight(eTeam, eInvisible) && iIntensity != 0;
	const int iSize = getNumPlotTeamVisibilityIntensity();

	for (int iI = 0; iI < iSize; iI++)
	{
		if (getPlotTeamVisibilityIntensity(iI).eInvisibility == eInvisible
		&& getPlotTeamVisibilityIntensity(iI).eTeam == eTeam
		&& getPlotTeamVisibilityIntensity(iI).iUnitID == iUnitID)
		{
			if (bRelevant)
			{
				getPlotTeamVisibilityIntensity(iI).iIntensity = iIntensity;
				bRelevant = false;
			}
			else
			{
				FAssertMsg(iIntensity == 0, "Spot intensity set to non-zero value on plot no unit is spotting on...");

				m_aPlotTeamVisibilityIntensity.erase(m_aPlotTeamVisibilityIntensity.begin()+iI);
			}
			break;
		}
	}
	if (bRelevant)
	{
		m_aPlotTeamVisibilityIntensity.resize(iSize + 1);
		m_aPlotTeamVisibilityIntensity[iSize].eInvisibility = eInvisible;
		m_aPlotTeamVisibilityIntensity[iSize].eTeam = eTeam;
		m_aPlotTeamVisibilityIntensity[iSize].iUnitID = iUnitID;
		m_aPlotTeamVisibilityIntensity[iSize].iIntensity = iIntensity;
	}

	if (eTeam == GC.getGame().getActiveTeam())
	{
		updateCenterUnit();
	}
}


int CvPlot::getNumPlotTeamVisibilityIntensity() const
{
	return (int)m_aPlotTeamVisibilityIntensity.size();
}

PlotTeamVisibilityIntensity& CvPlot::getPlotTeamVisibilityIntensity(int iIndex)
{
	return m_aPlotTeamVisibilityIntensity[iIndex];
}

int CvPlot::getHighestPlotTeamVisibilityIntensity(InvisibleTypes eInvisibility, TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iHighest = 0;
	for (int iI = 0; iI < getNumPlotTeamVisibilityIntensity(); iI++)
	{
		if (m_aPlotTeamVisibilityIntensity[iI].eInvisibility == eInvisibility
		&&  m_aPlotTeamVisibilityIntensity[iI].eTeam == eTeam
		&&  m_aPlotTeamVisibilityIntensity[iI].iIntensity > iHighest)
		{
			iHighest = m_aPlotTeamVisibilityIntensity[iI].iIntensity;
		}
	}
	return iHighest;
}

int CvPlot::getNumUnits() const
{
	return m_units.getLength();
}


CvUnit* CvPlot::getUnitByIndex(int iIndex) const
{
	const CLLNode<IDInfo>* pUnitNode = m_units.nodeNum(iIndex);

	if (pUnitNode != NULL)
	{
		return ::getUnit(pUnitNode->m_data);
	}
	return NULL;
}


void CvPlot::addUnit(CvUnit* pUnit, bool bUpdate)
{
	PROFILE_EXTRA_FUNC();
	FAssertMsg(pUnit->at(getX(), getY()), "pUnit is expected to be at getX and getY");

	//DEBUG attempt
	if (GC.getGame().isDebugMode())
	{
		foreach_(const CvUnit* pLoopUnit, units())
		{
			FAssertMsg(pUnit != pLoopUnit, "unit being added to plot's group as a duplicate");
		}
	}

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		if (!isBeforeUnitCycle(::getUnit(pUnitNode->m_data), pUnit))
		{
			break;
		}

		pUnitNode = nextUnitNode(pUnitNode);
	}

	if (pUnitNode != NULL)
	{
		m_units.insertBefore(pUnit->getIDInfo(), pUnitNode);
	}
	else
	{
		m_units.insertAtEnd(pUnit->getIDInfo());
	}

	if (bUpdate)
	{
		updateCenterUnit();

		setFlagDirty(true);
	}

	if (pUnit->isCommanderReady())
	{
		countCommander(true, pUnit);
	}

	if (pUnit->isCommodoreReady())
    {
    	countCommodore(true, pUnit);
    }
}


void CvPlot::removeUnit(CvUnit* pUnit, bool bUpdate)
{
	PROFILE_EXTRA_FUNC();
	if (pUnit->isCommanderReady())
	{
		countCommander(false, pUnit);
	}

	if (pUnit->isCommodoreReady())
    {
    	countCommodore(false, pUnit);
    }

	for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; )
	{
		if (::getUnit(pUnitNode->m_data) == pUnit)
		{
			FAssertMsg(::getUnit(pUnitNode->m_data)->at(getX(), getY()), "The current unit instance is expected to be at getX and getY");
			pUnitNode = m_units.deleteNode(pUnitNode);
			/*break;*///TBDebug: It has been found possible that a node may have somehow duplicated itself and thus the plot may have more than 1 of the node.  If these duplicates aren't destroyed, we get a crash as well.  Best case would be to somehow catch how this gets setup but in the debugging case that discovered this, it likely happened long beforehand.  Jumptonearestvalidplot is the suspected culprit as it was an exile that showed the issue.
		}
		else
		{
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	if (bUpdate)
	{
		updateCenterUnit();
		setFlagDirty(true);
	}
}


CLLNode<IDInfo>* CvPlot::nextUnitNode(CLLNode<IDInfo>* pNode) const
{
	return m_units.next(pNode);
}


CLLNode<IDInfo>* CvPlot::prevUnitNode(CLLNode<IDInfo>* pNode) const
{
	return m_units.prev(pNode);
}


CLLNode<IDInfo>* CvPlot::headUnitNode() const
{
	return m_units.head();
}


CLLNode<IDInfo>* CvPlot::tailUnitNode() const
{
	return m_units.tail();
}


int CvPlot::getNumSymbols() const
{
	return m_symbols.size();
}


CvSymbol* CvPlot::getSymbol(int iID) const
{
	return m_symbols[iID];
}


CvSymbol* CvPlot::addSymbol()
{
	CvSymbol* pSym=gDLL->getSymbolIFace()->createSymbol();
	m_symbols.push_back(pSym);
	return pSym;
}


void CvPlot::deleteSymbol(int iID)
{
	m_symbols.erase(m_symbols.begin()+iID);
}


void CvPlot::deleteAllSymbols()
{
	PROFILE_EXTRA_FUNC();
	for (int i = 0; i < getNumSymbols(); i++)
	{
		// ?? Need gDLL->getSymbolIFace()->Hide(pLoopSymbol, true);
		gDLL->getSymbolIFace()->destroy(m_symbols[i]);
	}
	m_symbols.clear();
}

CvString CvPlot::getScriptData() const
{
	return m_szScriptData;
}

void CvPlot::setScriptData(const char* szNewValue)
{
	SAFE_DELETE_ARRAY(m_szScriptData);
	m_szScriptData = _strdup(szNewValue);
}

// Protected Functions...

void CvPlot::doFeature()
{
	PROFILE("CvPlot::doFeature()");

	if (getFeatureType() != NO_FEATURE)
	{
		if (GC.getFeatureInfo(getFeatureType()).getDisappearanceProbability() > 0
		&&  GC.getFeatureInfo(getFeatureType()).getDisappearanceProbability() >
			(
				GC.getGame().getSorenRandNum(100 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent(), "Feature Disappearance")
			)
		) setFeatureType(NO_FEATURE);

		return;
	}

	for (int iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
	{
		const CvFeatureInfo& kFeature = GC.getFeatureInfo((FeatureTypes)iI);

		if ((kFeature.getGrowthProbability() > 0 && !isUnit() || kFeature.getSpreadProbability() > 0)
		&& (getImprovementType() == NO_IMPROVEMENT || kFeature.isCanGrowAnywhere() && !isBeingWorked() && !isWater())
		&& (getBonusType() == NO_BONUS || GC.getBonusInfo(getBonusType()).isFeature(iI) || kFeature.getSpreadProbability() > 0)
		&& canHaveFeature((FeatureTypes)iI, kFeature.getSpreadProbability() > 0))
		{
			int iProbability = kFeature.isCanGrowAnywhere() ? kFeature.getGrowthProbability() : 0;

			foreach_(const CvPlot* plotX, cardinalDirectionAdjacent())
			{
				if (plotX && plotX->getFeatureType() == iI)
				{
					iProbability += (
						(
							plotX->getImprovementType() == NO_IMPROVEMENT
							?
							kFeature.getGrowthProbability()
							:
							GC.getImprovementInfo(plotX->getImprovementType()).getFeatureGrowthProbability()
						)
						+ kFeature.getSpreadProbability()
					);
				}
			}
			iProbability *= std::max(0, (GC.getFEATURE_GROWTH_MODIFIER() + 100));
			iProbability /= 100;

			if (isRoute())
			{
				iProbability *= std::max(0, (GC.getROUTE_FEATURE_GROWTH_MODIFIER() + 100));
				iProbability /= 100;
			}

			if(iProbability > 0
			&& iProbability > GC.getGame().getSorenRandNum(100 * GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent(), "Feature Growth"))
			{
				setFeatureType((FeatureTypes)iI);
				TCHAR szSound[1024] = "AS2D_FEATUREGROWTH";
				strcpy(szSound, kFeature.getGrowthSound());

				const CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);

				if (pCity && isInViewport())
				{
					AddDLLMessage(
						getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
						gDLL->getText("TXT_KEY_MISC_FEATURE_GROWN_NEAR_CITY",
						kFeature.getTextKeyWide(), pCity->getNameKey()), szSound, MESSAGE_TYPE_INFO,
						kFeature.getButton(), GC.getCOLOR_WHITE(), getViewportX(),getViewportY(), true, true
					);
				}
				break;
			}
		}
	}
}


void CvPlot::doCulture()
{
	PROFILE("CvPlot::doCulture()");

	// Toffer - ToDo: doImprovementCulture should ideally be done in owner->doTurn(),
	//	as it adds culture to plots that in one direction has already done culture decay,
	//	and plots in the other direction that has yet to do culture deccay.
	// Rewrite so player has a cached vector of all owned forts (Maybe fort should be a new object type),
	//	and have forts do a doTurn() during the players doTurn(),
	//	might be necsessary rewrite for being able to produce units in forts anyway.
	if (getOwner() != NO_PLAYER && getImprovementType() != NO_IMPROVEMENT)
	{
		doImprovementCulture(getOwner(), GC.getImprovementInfo(getImprovementType()));
	}

	// Toffer - Decay culture for players that no longer adds culture to this plot
	// Blaze - or uses equilibrium culture gameoption
	{
		const int decayPermille = GC.getTILE_CULTURE_DECAY_PERCENT() * 1000 / GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getSpeedPercent();

		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			const PlayerTypes ePlayerX = static_cast<PlayerTypes>(i);

			if (getCulture(ePlayerX) > 0)
			{
				if (GC.getGame().isOption(GAMEOPTION_CULTURE_EQUILIBRIUM))
				{
					// By limiting decay to avoid 2+ -> 0, we can ensure that putting 2 culture on a tile will always be above 1 turn decay
					const int iIsOverOne = getCulture(ePlayerX) > 1;
					if (isInCultureRangeOfCityByPlayer(ePlayerX))
					{
						int64_t iCulture64 = getCulture(ePlayerX);
						setCulture(ePlayerX, std::max(iIsOverOne, static_cast<int>(int64_t(getCulture(ePlayerX)) * (1000 - decayPermille) / 1000)), false, false, true);
					}
					// Decay 15x faster (to 45% at default speeds) if outside of city control in equilibrium, since we can't immediately set to unowned when negative
					else
					{
						setCulture(ePlayerX, std::max(iIsOverOne, static_cast<int>(int64_t(getCulture(ePlayerX)) * (1000 - 15 * decayPermille) / 1000)), false, false, true);
					}
				}
				else if (getCultureRateThisTurn(ePlayerX) < 1 && (!getPlotCity() || getOwner() != ePlayerX))
				{
					setCulture(ePlayerX, std::max(0, static_cast<int>(int64_t(getCulture(ePlayerX)) * (1000 - decayPermille) / 1000)), false, false, true);
				}
			}
		}
	}
	// Check if plot ownership should flip from culture
	if (getPlotCity())
	{
		checkCityRevolt();
	}
	else if (isActsAsCity())
	{
		checkFortRevolt();
	}
	else setOwner(calculateCulturalOwner(false), true, true);

	m_cultureRatesLastTurn.clear();
	for (std::vector< std::pair<PlayerTypes, int> >::const_iterator it = m_cultureRatesThisTurn.begin(); it != m_cultureRatesThisTurn.end(); ++it)
	{
		m_cultureRatesLastTurn.push_back(std::make_pair((*it).first, (*it).second));
	}
	m_cultureRatesThisTurn.clear();
	m_influencedByCityByPlayerLastTurn.clear();
	for (std::vector<PlayerTypes>::const_iterator it = m_influencedByCityByPlayer.begin(); it != m_influencedByCityByPlayer.end(); ++it)
	{
		m_influencedByCityByPlayerLastTurn.push_back(*it);
	}
	m_influencedByCityByPlayer.clear();
}


void CvPlot::processArea(CvArea* pArea, int iChange)
{
	PROFILE_EXTRA_FUNC();
	// XXX am not updating getBestFoundValue() or getAreaAIType()...

	pArea->changeNumTiles(iChange);

	if (isOwned())
	{
		pArea->changeNumOwnedTiles(iChange);
	}

	if (isNOfRiver())
	{
		pArea->changeNumRiverEdges(iChange);
	}
	if (isWOfRiver())
	{
		pArea->changeNumRiverEdges(iChange);
	}

	if (getBonusType() != NO_BONUS)
	{
		pArea->changeNumBonuses(getBonusType(), iChange);
	}

	if (getImprovementType() != NO_IMPROVEMENT)
	{
		pArea->changeNumImprovements(getImprovementType(), iChange);
	}

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).getStartingPlot() == this)
		{
			pArea->changeNumStartingPlots(iChange);
		}

		pArea->changePower((PlayerTypes)iI, getUnitPower((PlayerTypes)iI) * iChange);

		pArea->changeUnitsPerPlayer(((PlayerTypes)iI), (plotCount(PUF_isPlayer, iI) * iChange));
		pArea->changeAnimalsPerPlayer(((PlayerTypes)iI), (plotCount(PUF_isAnimal, -1, -1, NULL, ((PlayerTypes)iI)) * iChange));

		for (int iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
		{
			pArea->changeNumAIUnits(((PlayerTypes)iI), ((UnitAITypes)iJ), (plotCount(PUF_isUnitAIType, iJ, -1, NULL, ((PlayerTypes)iI)) * iChange));
		}
	}

	for (int iI = 0; iI < MAX_TEAMS; ++iI)
	{
		if (isRevealed((TeamTypes)iI, false))
		{
			pArea->changeNumRevealedTiles(((TeamTypes)iI), iChange);
		}
	}

	CvCity* pCity = getPlotCity();

	if (pCity)
	{
		const PlayerTypes eOwner = pCity->getOwner();
		pArea->changeCitiesPerPlayer(eOwner, iChange);
		pArea->changePopulationPerPlayer(eOwner, (pCity->getPopulation() * iChange));

		foreach_(const BuildingTypes eTypeX, pCity->getHasBuildings())
		{
			if (pCity->isDisabledBuilding(eTypeX))
			{
				continue;
			}
			const CvBuildingInfo& building = GC.getBuildingInfo(eTypeX);

			pArea->changePower(eOwner, building.getPowerValue() * iChange);

			if (!pCity->isReligiouslyLimitedBuilding(eTypeX))
			{
				if (building.getAreaHealth() > 0)
				{
					pArea->changeBuildingGoodHealth(eOwner, building.getAreaHealth() * iChange);
				}
				else pArea->changeBuildingBadHealth(eOwner, building.getAreaHealth() * iChange);

				pArea->changeBuildingHappiness(eOwner, building.getAreaHappiness() * iChange);
				pArea->changeFreeSpecialist(eOwner, building.getAreaFreeSpecialist() * iChange);

				for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					pArea->changeYieldRateModifier(eOwner, (YieldTypes)iJ, building.getAreaYieldModifier(iJ) * iChange);
				}
			}
			pArea->changeBorderObstacleCount(pCity->getTeam(), iChange * building.isAreaBorderObstacle());
		}

		for (int iI = 0; iI < NUM_UNITAI_TYPES; ++iI)
		{
			pArea->changeNumTrainAIUnits(eOwner, (UnitAITypes)iI, pCity->getNumTrainUnitAI((UnitAITypes)iI) * iChange);
		}

		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (pArea->getTargetCity((PlayerTypes)iI) == pCity)
			{
				pArea->setTargetCity(((PlayerTypes)iI), NULL);
			}
		}
	}
}


ColorTypes CvPlot::plotMinimapColor() const
{
	if (GC.getGame().getActivePlayer() != NO_PLAYER)
	{
		{
			const CvCity* pCity = getPlotCity();

			if (pCity && pCity->isRevealed(GC.getGame().getActiveTeam(), true))
			{
				return (ColorTypes) GC.getInfoTypeForString("COLOR_WHITE");
			}
		}

		if (isActiveVisible(true))
		{
			const CvUnit* pCenterUnit = getCenterUnit(GC.getGame().isDebugMode());

			if (pCenterUnit)
			{
				return (ColorTypes)GC.getPlayerColorInfo(GET_PLAYER(pCenterUnit->getVisualOwner()).getPlayerColor()).getColorTypePrimary();
			}
		}
		const PlayerTypes eRevealedOwner = getRevealedOwner(GC.getGame().getActiveTeam(), true);

		if (eRevealedOwner != NO_PLAYER && !isWater())
		{
			return (ColorTypes)GC.getPlayerColorInfo(GET_PLAYER(eRevealedOwner).getPlayerColor()).getColorTypePrimary();
		}
	}
	return (ColorTypes) GC.getInfoTypeForString("COLOR_CLEAR");
}

//
// read object from a stream
// used during load
//
void CvPlot::read(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	int iI;
	bool bVal;
	int iCount;

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// Init saved data
	reset();

	WRAPPER_READ(wrapper, "CvPlot", &m_iX);
	WRAPPER_READ(wrapper, "CvPlot", &m_iY);
	WRAPPER_READ(wrapper, "CvPlot", &m_iArea);

	int iNum = 0;
	WRAPPER_READ(wrapper, "CvPlot", &iNum);

	for (int i=0; i<iNum; i++)
	{
		int eFeature = -1;
		int iVariety = -1;
		WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_FEATURES, &eFeature);
		WRAPPER_READ(wrapper, "CvPlot", &iVariety);
	}

	WRAPPER_READ(wrapper, "CvPlot", &m_iFeatureVariety);
	WRAPPER_READ(wrapper, "CvPlot", &m_iOwnershipDuration);
	WRAPPER_READ(wrapper, "CvPlot", &m_iUpgradeProgress);
	WRAPPER_READ(wrapper, "CvPlot", &m_iCityRadiusCount);
	WRAPPER_READ(wrapper, "CvPlot", &m_iRiverID);
	WRAPPER_READ(wrapper, "CvPlot", &m_iMinOriginalStartDist);
	WRAPPER_READ(wrapper, "CvPlot", &m_iReconCount);
	WRAPPER_READ(wrapper, "CvPlot", &m_iRiverCrossingCount);

	// Super Forts
	WRAPPER_READ(wrapper, "CvPlot", &m_iCanalValue);
	WRAPPER_READ(wrapper, "CvPlot", &m_iChokeValue);
	WRAPPER_READ(wrapper, "CvPlot", &m_iDefenseDamage);
	WRAPPER_READ(wrapper, "CvPlot", &m_bBombarded);
	// ! Super Forts

	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bStartingPlot");
	m_bStartingPlot = bVal;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bHills");
	m_bHills = bVal;

	WRAPPER_READ_STRING(wrapper, "CvPlot", m_szLandmarkMessage);
	WRAPPER_READ_STRING(wrapper, "CvPlot", m_szLandmarkName);
	WRAPPER_READ(wrapper, "CvPlot", &m_eClaimingOwner);
	WRAPPER_READ(wrapper, "CvPlot", (int*)&m_eLandmarkType);
	WRAPPER_READ(wrapper, "CvPlot", &m_zobristContribution);

	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bNOfRiver");
	m_bNOfRiver = bVal;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bWOfRiver");
	m_bWOfRiver = bVal;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bIrrigated");
	m_bIrrigated = bVal;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bPotentialCityWork");
	m_bPotentialCityWork = bVal;

	WRAPPER_READ(wrapper, "CvPlot", &m_eOwner);
	WRAPPER_READ(wrapper, "CvPlot", &m_ePlotType);
	WRAPPER_READ_CLASS_ENUM(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_TERRAINS, &m_eTerrainType);
	WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_FEATURES, &m_eFeatureType);

	WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_BONUSES, &m_eBonusType);
	WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_IMPROVEMENTS, &m_eImprovementType);
	WRAPPER_READ_CLASS_ENUM_ALLOW_MISSING(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_ROUTES, &m_eRouteType);
	WRAPPER_READ(wrapper, "CvPlot", &m_eRiverNSDirection);
	WRAPPER_READ(wrapper, "CvPlot", &m_eRiverWEDirection);

	WRAPPER_READ(wrapper, "CvPlot", (int*)&m_plotCity.eOwner);
	WRAPPER_READ(wrapper, "CvPlot", &m_plotCity.iID);
	WRAPPER_READ(wrapper, "CvPlot", (int*)&m_workingCity.eOwner);
	WRAPPER_READ(wrapper, "CvPlot", &m_workingCity.iID);
	WRAPPER_READ(wrapper, "CvPlot", (int*)&m_workingCityOverride.eOwner);
	WRAPPER_READ(wrapper, "CvPlot", &m_workingCityOverride.iID);

	WRAPPER_READ_ARRAY(wrapper, "CvPlot", NUM_YIELD_TYPES, m_aiYield);

	m_iActivePlayerSafeRangeCache = -1;
	m_iActivePlayerSafeRangeCacheTestMoves = -1;
	m_bActivePlayerHasDangerCache = false;
	m_bActivePlayerHasDangerCacheTestMoves = false;
	invalidateBorderDangerCache();

	m_aiCulture.clear();

	int buffer[MAX_PLAYERS];
	char cCount = 0;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		FAssert(cCount <= MAX_PLAYERS);
		WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvPlot", cCount, buffer, "m_aiCulture");

		for (iI = 0; iI < (int)cCount; iI++)
		{
			if (buffer[iI] > 0)
			{
				m_aiCulture.push_back(std::make_pair((PlayerTypes)iI, buffer[iI]));
			}
		}
	}
	//	Force the capacity to the size() since we need to mto minimize memory usage
	//	and adding cultures is rare
	std::vector<std::pair<PlayerTypes,int> >(m_aiCulture).swap(m_aiCulture);

	SAFE_DELETE_ARRAY(m_aiFoundValue);
	char cFoundValuesPresent;
	WRAPPER_READ(wrapper, "CvPlot", &cFoundValuesPresent);
	unsigned int* m_aiFoundValue = new unsigned int[cFoundValuesPresent];
	WRAPPER_READ_ARRAY(wrapper, "CvPlot", cFoundValuesPresent, m_aiFoundValue);

	SAFE_DELETE_ARRAY(m_aiPlayerCityRadiusCount);
	cCount = 0;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aiPlayerCityRadiusCount = new char[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiPlayerCityRadiusCount);
	}

	SAFE_DELETE_ARRAY(m_aiPlotGroup);
	cCount = 0;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aiPlotGroup = new int[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiPlotGroup);
	}

	SAFE_DELETE_ARRAY(m_aiVisibilityCount);
	cCount = 0;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aiVisibilityCount = new short[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiVisibilityCount);
	}

	SAFE_DELETE_ARRAY(m_aiMountainLeaderCount);
	cCount = 0;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditionalML");
	if (cCount > 0)
	{
		m_aiMountainLeaderCount = new short[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiMountainLeaderCount);
	}

	SAFE_DELETE_ARRAY(m_aiStolenVisibilityCount);
	cCount = 0;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aiStolenVisibilityCount = new short[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiStolenVisibilityCount);
	}

	cCount = 0;	//	Must reset heer since next value was added after new format save introduction
				//	so can be exopected to be missing in some saves - hence needs sensible default
	SAFE_DELETE_ARRAY(m_aiLastSeenTurn);
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cLastSeenCount");
	if (cCount > 0)
	{
		m_aiLastSeenTurn = new short[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiLastSeenTurn);
	}

	SAFE_DELETE_ARRAY(m_aiBlockadedCount);
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aiBlockadedCount = new short[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiBlockadedCount);
		// Hack to cleanup negative blockades
		for (int i = 0; i < cCount; ++i)
		{
			m_aiBlockadedCount[i] = std::max<short>(0, m_aiBlockadedCount[i]);
		}
	}

	SAFE_DELETE_ARRAY(m_aiRevealedOwner);
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aiRevealedOwner = new char[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiRevealedOwner);
	}

	SAFE_DELETE_ARRAY(m_abRiverCrossing);
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_abRiverCrossing = new bool[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_abRiverCrossing);
	}

	SAFE_DELETE_ARRAY(m_abRevealed);
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_abRevealed = new bool[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_abRevealed);
	}

	SAFE_DELETE_ARRAY(m_aeRevealedImprovementType);
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aeRevealedImprovementType = new short[cCount];
		WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_IMPROVEMENTS, cCount, m_aeRevealedImprovementType);
	}

	SAFE_DELETE_ARRAY(m_aeRevealedRouteType);
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aeRevealedRouteType = new short[cCount];
		WRAPPER_READ_CLASS_ENUM_ARRAY_ALLOW_MISSING(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_ROUTES, cCount, m_aeRevealedRouteType);
	}

	WRAPPER_READ_STRING(wrapper, "CvPlot", &m_szScriptData);

	// @SAVEBREAK EVALUATE
	SAFE_DELETE_ARRAY(m_paiBuildProgress);

	//	Old format just saved these as a short array
	//	Unfortunately there was a screwed up intermediary format that saved as a class
	//	array if the array was non-NULL on save but DIDN'T save the count!  Dealing
	//	with all of these variants makes this next section a little complex
	iCount;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iCount, "iConditional");

	if ( wrapper.isUsingTaggedFormat() )
	{
		if (iCount > 0)
		{
			m_paiBuildProgress = new int[GC.getNumBuildInfos()];
			short* iTempArray = new short[iCount];

			//	Might be old format where it's a raw array so try to load that first
			//	but make sure all values are 0'd if it's not present
			memset(iTempArray,0,sizeof(short)*iCount);

			WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvPlot", iCount, iTempArray, "m_paiBuildProgress");

			for(int i = 0; i < iCount; i++)
			{
				int	iRemappedBuild = wrapper.getNewClassEnumValue(REMAPPED_CLASS_TYPE_BUILDS, i, true);

				if ( iRemappedBuild != -1 )
				{
					// SVN 10973 08-13-2019 Fix for already corrupted plot build-progress counter
					m_paiBuildProgress[iRemappedBuild] = std::max((short)0, iTempArray[i]);
				}
			}

			SAFE_DELETE_ARRAY(iTempArray);
		}
	}

	if (iCount > 0)
	{
		//	Correct format is a class array.  Depending on how old the save was
		//	it may or may not actually be present
		if ( m_paiBuildProgress == NULL )
		{
			m_paiBuildProgress = new int[GC.getNumBuildInfos()];
			memset(m_paiBuildProgress,0,sizeof(int)*GC.getNumBuildInfos());
		}

		WRAPPER_READ_CLASS_ARRAY_ALLOW_MISSING(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_BUILDS, GC.getNumBuildInfos(), m_paiBuildProgress);
		// SVN 10973 08-13-2019 Fix for already corrupted plot build-progress counter
		for (int i = 0; i < GC.getNumBuildInfos(); i++)
		{
			m_paiBuildProgress[i] = std::max(0, m_paiBuildProgress[i]);
		}
	}

	//	Both to cope with having allocated the array speculatively in the intermediary format
	//	and to prune it from plots that no longer have in-progress build activity check any
	//	elements are non-0
	if ( m_paiBuildProgress != NULL )
	{
		bool	inUse = false;

		for(int i = 0; i < GC.getNumBuildInfos(); i++)
		{
			if ( m_paiBuildProgress[i] != 0 )
			{
				inUse = true;
				break;
			}
		}

		if ( !inUse )
		{
			SAFE_DELETE_ARRAY(m_paiBuildProgress);
		}
	}
	// SAVEBREAK@

	//	Read the plot danger counts for all players that have any here
	int	iPlayersWithDanger = 0;

	WRAPPER_READ(wrapper, "CvPlot", &iPlayersWithDanger);
	while ( iPlayersWithDanger-- > 0 )
	{
		int	iDangerCount;

		WRAPPER_READ(wrapper, "CvPlot", (uint*)&iI);
		WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iDangerCount, "DangerCount");

		setDangerCount((PlayerTypes)iI, iDangerCount);
	}

	if (NULL != m_apaiInvisibleVisibilityCount)
	{
		for (int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount);
	}
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_apaiInvisibleVisibilityCount = new short*[cCount];
		for (iI = 0; iI < cCount; ++iI)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iCount, "InvisibilityCount");
			if (iCount > 0)
			{
				m_apaiInvisibleVisibilityCount[iI] = new short[iCount];
				WRAPPER_READ_ARRAY(wrapper, "CvPlot", iCount, m_apaiInvisibleVisibilityCount[iI]);
			}
			else
			{
				m_apaiInvisibleVisibilityCount[iI] = NULL;
			}
		}
	}

	m_units.Read(pStream);

	m_Properties.readWrapper(pStream);
	WRAPPER_READ(wrapper, "CvPlot", &m_bImprovementUpgradable);
	WRAPPER_READ(wrapper, "CvPlot", &m_iImprovementUpgradeHash);
	WRAPPER_READ(wrapper, "CvPlot", &m_iCurrentRoundofUpgradeCache);
	WRAPPER_READ(wrapper, "CvPlot", &m_iImprovementCurrentValue);

	int iSize = 0;
	WRAPPER_READ(wrapper, "CvPlot", &iSize);
	m_aPlotTeamVisibilityIntensity.resize(iSize);
	if (iSize > 0)
	{
		int iType1 = 0;
		int iType2 = 0;
		int iType3 = 0;
		int iType4 = 0;

		for (int iI = 0; iI < iSize; iI++)
		{
			WRAPPER_READ(wrapper, "CvPlot", &iType1);
			WRAPPER_READ(wrapper, "CvPlot", &iType2);
			WRAPPER_READ(wrapper, "CvPlot", &iType3);
			WRAPPER_READ(wrapper, "CvPlot", &iType4);
			m_aPlotTeamVisibilityIntensity[iI].eInvisibility = (InvisibleTypes)iType1;
			m_aPlotTeamVisibilityIntensity[iI].eTeam = (TeamTypes)iType2;
			m_aPlotTeamVisibilityIntensity[iI].iUnitID = iType3;
			m_aPlotTeamVisibilityIntensity[iI].iIntensity = iType4;
		}
	}
	WRAPPER_READ_ARRAY(wrapper, "CvPlot", NUM_YIELD_TYPES, m_baseYields);

	// Toffer - Read maps
	{
		short iSize = 0;
		uint8_t uType8 = 0;
		uint16_t uCount16 = 0;

		WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iSize, "iCommanderCountSize");
		WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iSize, "iCommodoreCountSize");
		while (iSize-- > 0)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &uType8, "iCommanderCountPlayer");
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &uCount16, "iCommanderCountCount");
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &uType8, "iCommodoreCountPlayer");
            WRAPPER_READ_DECORATED(wrapper, "CvPlot", &uCount16, "iCommodoreCountCount");

			FAssert(uCount16 > 0);
			m_commanderCount.insert(std::make_pair(uType8, uCount16));

			FAssert(uCount16 > 0);
            m_commodoreCount.insert(std::make_pair(uType8, uCount16));

			// Toffer - This is too early to set the player cache, players haven't been read in yet,
			//	so the cache will be cleared at the player reset before read.
			//GET_PLAYER(static_cast<PlayerTypes>(uType8)).setCommandFieldPlot(true, this);
		}
	}

	// Toffer - Read vectors
	{
		short iSize = 0;
		short iType = -1;
		// Building
		WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iSize, "CultureRatesThisTurnSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iType, "CultureRatesThisTurnPlayer");
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iValue, "CultureRatesThisTurnRate");

			if (iType > -1 && iType < MAX_PLAYERS)
			{
				m_cultureRatesThisTurn.push_back(std::make_pair(static_cast<PlayerTypes>(iType), iValue));
			}
		}
		WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iSize, "CultureRatesLastTurnSize");
		for (short i = 0; i < iSize; ++i)
		{
			int iValue = 0;
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iType, "CultureRatesLastTurnPlayer");
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iValue, "CultureRatesLastTurnRate");

			if (iType > -1 && iType < MAX_PLAYERS)
			{
				m_cultureRatesLastTurn.push_back(std::make_pair(static_cast<PlayerTypes>(iType), iValue));
			}
		}
		WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iSize, "InfluencedByCityByPlayerLastTurnSize");
		for (short i = 0; i < iSize; ++i)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iType, "InfluencedByCityByPlayerLastTurn");

			if (iType > -1 && iType < MAX_PLAYERS)
			{
				m_influencedByCityByPlayerLastTurn.push_back(static_cast<PlayerTypes>(iType));
			}
		}
		WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iSize, "InfluencedByCityByPlayerSize");
		for (short i = 0; i < iSize; ++i)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iType, "InfluencedByCityByPlayer");

			if (iType > -1 && iType < MAX_PLAYERS)
			{
				m_influencedByCityByPlayer.push_back(static_cast<PlayerTypes>(iType));
			}
		}
	}
	WRAPPER_READ_ARRAY(wrapper, "CvPlot", NUM_YIELD_TYPES, m_aExtraYield.c_array());
	// @SAVEBREAK delete
	foreach_(const YieldTypes eYield, YieldTypesRange())
	{
		m_aExtraYield[eYield] += GC.getGame().getPlotExtraYield(m_iX, m_iY, eYield);
	}
	// ! SAVEBREAK

	//Example of how to Skip Element
	//WRAPPER_SKIP_ELEMENT(wrapper, "CvPlot", m_bPeaks, SAVE_VALUE_ANY);
	WRAPPER_READ_OBJECT_END(wrapper);

	//	Zobrist characteristic hashes are not serialized so recalculate
	//	Right now it's just characteristics that affect what a unit might
	//	be able to move through that matter, so its unit class + certain promotions
	if ( getPlotType() != NO_PLOT )
	{
		m_movementCharacteristicsHash ^= g_plotTypeZobristHashes[getPlotType()];
	}
	if ( getFeatureType() != NO_FEATURE )
	{
		m_movementCharacteristicsHash ^= GC.getFeatureInfo(getFeatureType()).getZobristValue();
	}
	if ( getTerrainType() != NO_TERRAIN )
	{
		m_movementCharacteristicsHash ^= GC.getTerrainInfo(getTerrainType()).getZobristValue();
	}
	if ( getRouteType() != NO_ROUTE )
	{
		m_movementCharacteristicsHash ^= GC.getRouteInfo(getRouteType()).getZobristValue();
	}
	if (getRiverNSDirection() != NO_CARDINALDIRECTION)
	{
		m_movementCharacteristicsHash ^= g_riverDirectionZobristHashes[getRiverNSDirection()];
	}
	if (getRiverWEDirection() != NO_CARDINALDIRECTION)
	{
		m_movementCharacteristicsHash ^= g_riverDirectionZobristHashes[getRiverWEDirection()];
	}
	updateCenterUnit();
}

//
// write object to a stream
// used during save
//
void CvPlot::write(FDataStreamBase* pStream)
{
	PROFILE_EXTRA_FUNC();
	CvTaggedSaveFormatWrapper& wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	WRAPPER_WRITE(wrapper, "CvPlot", m_iX);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iY);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iArea);

	WRAPPER_WRITE(wrapper, "CvPlot", m_iFeatureVariety);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iOwnershipDuration);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iUpgradeProgress);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iCityRadiusCount);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iRiverID);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iMinOriginalStartDist);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iReconCount);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iRiverCrossingCount);

	// Super Forts
	WRAPPER_WRITE(wrapper, "CvPlot", m_iCanalValue);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iChokeValue);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iDefenseDamage);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bBombarded);
	// ! Super Forts

	WRAPPER_WRITE(wrapper, "CvPlot", m_bStartingPlot);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bHills);

	WRAPPER_WRITE_STRING(wrapper, "CvPlot", m_szLandmarkMessage);
	WRAPPER_WRITE_STRING(wrapper, "CvPlot", m_szLandmarkName);

	WRAPPER_WRITE(wrapper, "CvPlot", m_eClaimingOwner);
	WRAPPER_WRITE(wrapper, "CvPlot", m_eLandmarkType);
	WRAPPER_WRITE(wrapper, "CvPlot", m_zobristContribution);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bNOfRiver);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bWOfRiver);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bIrrigated);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bPotentialCityWork);

	WRAPPER_WRITE(wrapper, "CvPlot", m_eOwner);
	WRAPPER_WRITE(wrapper, "CvPlot", m_ePlotType);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_TERRAINS, m_eTerrainType);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_FEATURES, m_eFeatureType);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_BONUSES, m_eBonusType);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_IMPROVEMENTS, m_eImprovementType);
	WRAPPER_WRITE_CLASS_ENUM(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_ROUTES, m_eRouteType);
	WRAPPER_WRITE(wrapper, "CvPlot", m_eRiverNSDirection);
	WRAPPER_WRITE(wrapper, "CvPlot", m_eRiverWEDirection);

	WRAPPER_WRITE(wrapper, "CvPlot", m_plotCity.eOwner);
	WRAPPER_WRITE(wrapper, "CvPlot", m_plotCity.iID);
	WRAPPER_WRITE(wrapper, "CvPlot", m_workingCity.eOwner);
	WRAPPER_WRITE(wrapper, "CvPlot", m_workingCity.iID);
	WRAPPER_WRITE(wrapper, "CvPlot", m_workingCityOverride.eOwner);
	WRAPPER_WRITE(wrapper, "CvPlot", m_workingCityOverride.iID);

	WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", NUM_YIELD_TYPES, m_aiYield);

	if (m_aiCulture.empty())
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		int	buffer[MAX_PLAYERS];

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			buffer[iI] = 0;
		}

		for (uint iI = 0; iI < m_aiCulture.size(); iI++)
		{
			buffer[m_aiCulture[iI].first] = m_aiCulture[iI].second;
		}
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_PLAYERS, "cConditional");
		WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "CvPlot", MAX_PLAYERS, buffer, "m_aiCulture");
	}

	if (NULL == m_aiFoundValue)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cFoundValuesPresent");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_PLAYERS, "cFoundValuesPresent");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_PLAYERS, m_aiFoundValue);
	}

	if (NULL == m_aiPlayerCityRadiusCount)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_PLAYERS, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_PLAYERS, m_aiPlayerCityRadiusCount);
	}

	if (NULL == m_aiPlotGroup)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_PLAYERS, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_PLAYERS, m_aiPlotGroup);
	}

	if (NULL == m_aiVisibilityCount)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_TEAMS, m_aiVisibilityCount);
	}

	if (NULL == m_aiMountainLeaderCount)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditionalML");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditionalML");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_TEAMS, m_aiMountainLeaderCount);
	}

	if (NULL == m_aiStolenVisibilityCount)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_TEAMS, m_aiStolenVisibilityCount);
	}

	if (NULL == m_aiLastSeenTurn)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cLastSeenCount");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cLastSeenCount");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_TEAMS, m_aiLastSeenTurn);
	}

	if (NULL == m_aiBlockadedCount)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_TEAMS, m_aiBlockadedCount);
	}

	if (NULL == m_aiRevealedOwner)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_TEAMS, m_aiRevealedOwner);
	}

	if (NULL == m_abRiverCrossing)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)NUM_DIRECTION_TYPES, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", NUM_DIRECTION_TYPES, m_abRiverCrossing);
	}

	if (NULL == m_abRevealed)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_TEAMS, m_abRevealed);
	}

	if (NULL == m_aeRevealedImprovementType)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
		WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_IMPROVEMENTS, MAX_TEAMS, m_aeRevealedImprovementType);
	}

	if (NULL == m_aeRevealedRouteType)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
		WRAPPER_WRITE_CLASS_ENUM_ARRAY(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_ROUTES, MAX_TEAMS, m_aeRevealedRouteType);
	}

	WRAPPER_WRITE_STRING(wrapper, "CvPlot", m_szScriptData);

	if (NULL != m_paiBuildProgress)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (int)GC.getNumBuildInfos(), "iConditional");
		WRAPPER_WRITE_CLASS_ARRAY(wrapper, "CvPlot", REMAPPED_CLASS_TYPE_BUILDS, GC.getNumBuildInfos(), m_paiBuildProgress);
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (int)0, "iConditional");
	}

	//	Write out plot danger counts for all players that have any here
	int	iPlayersWithDanger = 0;
	for (int iI= 0; iI < MAX_PLAYERS; iI++ )
	{
		if ( getDangerCount((PlayerTypes)iI) > 0 )
		{
			iPlayersWithDanger++;
		}
	}
	WRAPPER_WRITE(wrapper, "CvPlot", iPlayersWithDanger);
	if ( iPlayersWithDanger > 0 )
	{
		for (uint iI= 0; iI < MAX_PLAYERS; iI++ )
		{
			if ( getDangerCount((PlayerTypes)iI) > 0 )
			{
				WRAPPER_WRITE(wrapper, "CvPlot", iI);
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", getDangerCount((PlayerTypes)iI), "DangerCount");
			}
		}
	}

	if (NULL == m_apaiInvisibleVisibilityCount)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
		for (int iI=0; iI < MAX_TEAMS; ++iI)
		{
			if (NULL == m_apaiInvisibleVisibilityCount[iI])
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (int)0, "InvisibilityCount");
			}
			else
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", GC.getNumInvisibleInfos(), "InvisibilityCount");
				WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", GC.getNumInvisibleInfos(), m_apaiInvisibleVisibilityCount[iI]);
			}
		}
	}

	m_units.Write(pStream);

	m_Properties.writeWrapper(pStream);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bImprovementUpgradable);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iImprovementUpgradeHash);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iCurrentRoundofUpgradeCache);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iImprovementCurrentValue);

	int iSize = (int)m_aPlotTeamVisibilityIntensity.size();

	WRAPPER_WRITE(wrapper, "CvPlot", iSize);

	for (int iI = 0; iI < iSize; iI++)
	{
		int iType1 = (int)m_aPlotTeamVisibilityIntensity[iI].eInvisibility;
		int iType2 = (int)m_aPlotTeamVisibilityIntensity[iI].eTeam;
		int iType3 = m_aPlotTeamVisibilityIntensity[iI].iUnitID;
		int iType4 = m_aPlotTeamVisibilityIntensity[iI].iIntensity;
		WRAPPER_WRITE(wrapper, "CvPlot", iType1);
		WRAPPER_WRITE(wrapper, "CvPlot", iType2);
		WRAPPER_WRITE(wrapper, "CvPlot", iType3);
		WRAPPER_WRITE(wrapper, "CvPlot", iType4);
	}
	WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", NUM_YIELD_TYPES, m_baseYields);

	// Toffer - Write maps
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (short)m_commanderCount.size(), "iCommanderCountSize");
		for (std::map<uint8_t, uint16_t>::const_iterator it = m_commanderCount.begin(), itEnd = m_commanderCount.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", it->first, "iCommanderCountPlayer");
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", it->second, "iCommanderCountCount");
		}
	}
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (short)m_commodoreCount.size(), "iCommodoreCountSize");
		for (std::map<uint8_t, uint16_t>::const_iterator it = m_commodoreCount.begin(), itEnd = m_commodoreCount.end(); it != itEnd; ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", it->first, "iCommodoreCountPlayer");
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", it->second, "iCommodoreCountCount");
		}
	}

	// Toffer - Write vectors
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (short)m_cultureRatesThisTurn.size(), "CultureRatesThisTurnSize");
		for (std::vector< std::pair<PlayerTypes, int> >::iterator it = m_cultureRatesThisTurn.begin(); it != m_cultureRatesThisTurn.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", static_cast<short>((*it).first), "CultureRatesThisTurnPlayer");
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (*it).second, "CultureRatesThisTurnRate");
		}
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (short)m_cultureRatesLastTurn.size(), "CultureRatesLastTurnSize");
		for (std::vector< std::pair<PlayerTypes, int> >::iterator it = m_cultureRatesLastTurn.begin(); it != m_cultureRatesLastTurn.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", static_cast<short>((*it).first), "CultureRatesLastTurnPlayer");
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (*it).second, "CultureRatesLastTurnRate");
		}
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (short)m_influencedByCityByPlayerLastTurn.size(), "InfluencedByCityByPlayerLastTurnSize");
		for (std::vector<PlayerTypes>::iterator it = m_influencedByCityByPlayerLastTurn.begin(); it != m_influencedByCityByPlayerLastTurn.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", static_cast<short>(*it), "InfluencedByCityByPlayerLastTurn");
		}
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (short)m_influencedByCityByPlayer.size(), "InfluencedByCityByPlayerSize");
		for (std::vector<PlayerTypes>::iterator it = m_influencedByCityByPlayer.begin(); it != m_influencedByCityByPlayer.end(); ++it)
		{
			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", static_cast<short>(*it), "InfluencedByCityByPlayer");
		}
	}
	WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", NUM_YIELD_TYPES, m_aExtraYield.c_array());

	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void CvPlot::setLayoutDirty(bool bDirty)
{
	if (!GC.IsGraphicsInitialized() || !isInViewport())
	{
		return;
	}

	if (isLayoutDirty() != bDirty)
	{
		m_bPlotLayoutDirty = bDirty;

		if (isLayoutDirty() && (m_pPlotBuilder == NULL))
		{
			if (!updatePlotBuilder())
			{
				m_bPlotLayoutDirty = false;
			}
		}
	}
}

bool CvPlot::updatePlotBuilder()
{
	if ( !isGraphicsVisible(ECvPlotGraphics::FEATURE) )
	{
		return false;
	}

	if (shouldUsePlotBuilder())
	{
		if (m_pPlotBuilder == NULL) // we need a plot builder... but it doesn't exist
		{
			m_pPlotBuilder = gDLL->getPlotBuilderIFace()->create();
			gDLL->getPlotBuilderIFace()->init(m_pPlotBuilder, this);
		}

		return true;
	}

	return false;
}

bool CvPlot::isLayoutDirty() const
{
	return m_bPlotLayoutDirty;
}

bool CvPlot::isLayoutStateDifferent() const
{
	bool bSame = true;
	// is worked
	bSame &= m_bLayoutStateWorked == isBeingWorked();

	// done
	return !bSame;
}

void CvPlot::setLayoutStateToCurrent()
{
	m_bLayoutStateWorked = isBeingWorked();
}

//------------------------------------------------------------------------------------------------

void CvPlot::getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked) const
{
	eType = NO_IMPROVEMENT;
	bWorked = false;

	if (GC.getGame().getActiveTeam() == NO_TEAM)
	{
		return;
	}

	eType = getRevealedImprovementType(GC.getGame().getActiveTeam(), true);

	if (eType == NO_IMPROVEMENT && isActiveVisible(true) && isBeingWorked() && !isCity())
	{
		if (isWater())
		{
			eType = (ImprovementTypes) GC.getDefineINT("WATER_IMPROVEMENT");
		}
		else
		{
			eType = (ImprovementTypes) GC.getDefineINT("LAND_IMPROVEMENT");
		}
	}

	// worked state
	if (isActiveVisible(false) && isBeingWorked())
	{
		bWorked = true;
	}
}

void CvPlot::getVisibleBonusState(BonusTypes& eType, bool& bImproved, bool& bWorked) const
{
	eType = NO_BONUS;
	bImproved = false;
	bWorked = false;

	if (GC.getGame().getActiveTeam() == NO_TEAM)
	{
		return;
	}

	if (GC.getGame().isDebugMode())
	{
		eType = getBonusType();
	}
	else if (isRevealed(GC.getGame().getActiveTeam(), false))
	{
		eType = getBonusType(GC.getGame().getActiveTeam());
	}

	// improved and worked states ...
	if (eType != NO_BONUS)
	{
		const ImprovementTypes eRevealedImprovement = getRevealedImprovementType(GC.getGame().getActiveTeam(), true);

		if (eRevealedImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eRevealedImprovement).isImprovementBonusTrade(eType))
		{
			bImproved = true;
			bWorked = isBeingWorked();
		}
	}
}

bool CvPlot::shouldUsePlotBuilder() const
{
	bool bBonusImproved; bool bBonusWorked; bool bImprovementWorked;
	BonusTypes eBonusType;
	ImprovementTypes eImprovementType;
	getVisibleBonusState(eBonusType, bBonusImproved, bBonusWorked);
	getVisibleImprovementState(eImprovementType, bImprovementWorked);
	if(eBonusType != NO_BONUS || eImprovementType != NO_IMPROVEMENT)
	{
		return true;
	}
	return false;
}

/* Unused...
int CvPlot::calculateMaxYield(YieldTypes eYield) const
{
	if (getTerrainType() == NO_TERRAIN)
	{
		return 0;
	}

	int iMaxYield = calculateNatureYield(eYield, NO_TEAM);

	int iImprovementYield = 0;
	for (int iImprovement = 0; iImprovement < GC.getNumImprovementInfos(); iImprovement++)
	{
		iImprovementYield = std::max(calculateImprovementYieldChange((ImprovementTypes)iImprovement, eYield, NO_PLAYER, true), iImprovementYield);
	}
	iMaxYield += iImprovementYield;

	int iRouteYield = 0;
	for (int iRoute = 0; iRoute < GC.getNumRouteInfos(); iRoute++)
	{
		iRouteYield = std::max(GC.getRouteInfo((RouteTypes)iRoute).getYieldChange(eYield), iRouteYield);
	}
	iMaxYield += iRouteYield;

	if (isWater() && !(isImpassable(getTeam())&&!isRoute()))
	{
		int iBuildingYield = 0;
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
		{
			const CvBuildingInfo& building = GC.getBuildingInfo((BuildingTypes)iBuilding);
			iBuildingYield = std::max(building.getGlobalSeaPlotYieldChange(eYield), iBuildingYield);
		}
		iMaxYield += iBuildingYield;
	}

	if (isRiver())
	{
		int iBuildingYield = 0;
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
		{
			const CvBuildingInfo& building = GC.getBuildingInfo((BuildingTypes)iBuilding);
			iBuildingYield = std::max(building.getRiverPlotYieldChange(eYield), iBuildingYield);
		}
		iMaxYield += iBuildingYield;
	}

	int iExtraYieldThreshold = 0;
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); iTrait++)
	{
		const CvTraitInfo& trait = GC.getTraitInfo((TraitTypes)iTrait);
		iExtraYieldThreshold  = std::max(trait.getExtraYieldThreshold(eYield), iExtraYieldThreshold);
	}
	if (iExtraYieldThreshold > 0 && iMaxYield > iExtraYieldThreshold)
	{
		iMaxYield += GC.getDefineINT("EXTRA_YIELD");
	}

	return iMaxYield;
}
*/

int CvPlot::getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade) const
{
	PROFILE_EXTRA_FUNC();
	int iYield = 0;

	bool bIgnoreFeature = false;
	if (getFeatureType() != NO_FEATURE)
	{
		if (GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()))
		{
			bIgnoreFeature = true;
		}
	}

	iYield += calculateNatureYield(eYield, getTeam(), bIgnoreFeature);

	ImprovementTypes eImprovement = GC.getBuildInfo(eBuild).getImprovement();

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (bWithUpgrade)
		{
			//in the case that improvements upgrade, use 2 upgrade levels higher for the
			//yield calculations.
			ImprovementTypes eUpgradeImprovement = GC.getImprovementInfo(eImprovement).getImprovementUpgrade();
			if (eUpgradeImprovement != NO_IMPROVEMENT)
			{
				//unless it's commerce on a low food tile, in which case only use 1 level higher
				if ((eYield != YIELD_COMMERCE) || (getYield(YIELD_FOOD) >= GC.getFOOD_CONSUMPTION_PER_POPULATION()))
				{
					const ImprovementTypes eUpgradeImprovement2 = GC.getImprovementInfo(eUpgradeImprovement).getImprovementUpgrade();
					if (eUpgradeImprovement2 != NO_IMPROVEMENT)
					{
						eUpgradeImprovement = eUpgradeImprovement2;
					}
				}
			}

			if ((eUpgradeImprovement != NO_IMPROVEMENT) && (eUpgradeImprovement != eImprovement))
			{
				eImprovement = eUpgradeImprovement;
			}
		}

		iYield += calculateImprovementYieldChange(eImprovement, eYield, getOwner(), false);
	}

	RouteTypes eRoute = (RouteTypes)GC.getBuildInfo(eBuild).getRoute();
	if (eRoute != NO_ROUTE)
	{
		eImprovement = getImprovementType();
		if (eImprovement != NO_IMPROVEMENT)
		{
			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				iYield += GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eRoute, iI);
				if (getRouteType() != NO_ROUTE)
				{
					iYield -= GC.getImprovementInfo(eImprovement).getRouteYieldChanges(getRouteType(), iI);
				}
			}
		}
	}


	return iYield;
}

bool CvPlot::canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();

	const CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (kTrigger.isOwnPlot() && getOwner() != ePlayer)
	{
		return false;
	}

	if (kTrigger.getPlotType() != NO_PLOT)
	{
		if (getPlotType() != kTrigger.getPlotType())
		{
			return false;
		}
	}

	if (kTrigger.getNumFeaturesRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumFeaturesRequired(); ++i)
		{
			if (kTrigger.getFeatureRequired(i) == getFeatureType())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getNumTerrainsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumTerrainsRequired(); ++i)
		{
			if (kTrigger.getTerrainRequired(i) == getTerrainType())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getNumImprovementsRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumImprovementsRequired(); ++i)
		{
			if (kTrigger.getImprovementRequired(i) == getImprovementType())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getNumBonusesRequired() > 0)
	{
		bool bFoundValid = false;

		for (int i = 0; i < kTrigger.getNumBonusesRequired(); ++i)
		{
			if (kTrigger.getBonusRequired(i) == getBonusType(kTrigger.isOwnPlot() ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM))
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.getNumRoutesRequired() > 0)
	{
		bool bFoundValid = false;

		if (NULL == getPlotCity())
		{
			for (int i = 0; i < kTrigger.getNumRoutesRequired(); ++i)
			{
				if (kTrigger.getRouteRequired(i) == getRouteType())
				{
					bFoundValid = true;
					break;
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kTrigger.isUnitsOnPlot())
	{
		bool bFoundValid = false;

		foreach_(const CvUnit* pLoopUnit, units())
		{
			if (pLoopUnit->getOwner() == ePlayer)
			{
				if (MIN_INT != pLoopUnit->getTriggerValue(eTrigger, this, false))
				{
					bFoundValid = true;
					break;
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}


	if (kTrigger.isPrereqEventCity() && kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;

		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); ++iI)
		{
			const EventTriggeredData* pTriggeredData = GET_PLAYER(ePlayer).getEventOccured((EventTypes)kTrigger.getPrereqEvent(iI));
			if (NULL == pTriggeredData || pTriggeredData->m_iPlotX != getX() || pTriggeredData->m_iPlotY != getY())
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	return true;
}

bool CvPlot::canApplyEvent(EventTypes eEvent) const
{
	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (kEvent.getFeatureChange() > 0)
	{
		if (NO_FEATURE != kEvent.getFeature()
		&& (NO_IMPROVEMENT != getImprovementType() || !canHaveFeature((FeatureTypes)kEvent.getFeature())))
		{
			return false;
		}
	}
	else if (kEvent.getFeatureChange() < 0 && NO_FEATURE == getFeatureType())
	{
		return false;
	}

	if (kEvent.getImprovementChange() > 0)
	{
		if (getImprovementType() == kEvent.getImprovement()
		|| !canHaveImprovement(kEvent.getImprovement(), getTeam(), false))
		{
			return false;
		}
	}
	else if (kEvent.getImprovementChange() < 0 && NO_IMPROVEMENT == getImprovementType())
	{
		return false;
	}

	if (kEvent.getBonusChange() > 0)
	{
		if (NO_BONUS != kEvent.getBonus() && !canHaveBonus((BonusTypes)kEvent.getBonus(), false))
		{
			return false;
		}
	}
	else if (kEvent.getBonusChange() < 0 && NO_BONUS == getBonusType())
	{
		return false;
	}

	if (kEvent.getRouteChange() < 0)
	{
		if (NO_ROUTE == getRouteType())
		{
			return false;
		}
		if (isCity())
		{
			return false;
		}
	}
	return true;
}

void CvPlot::applyEvent(EventTypes eEvent)
{
	PROFILE_EXTRA_FUNC();
	const CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (kEvent.getFeatureChange() > 0)
	{
		if (NO_FEATURE != kEvent.getFeature())
		{
			setFeatureType((FeatureTypes)kEvent.getFeature());
		}
	}
	else if (kEvent.getFeatureChange() < 0)
	{
		setFeatureType(NO_FEATURE);
	}

	if (kEvent.getImprovementChange() > 0)
	{
		if (NO_IMPROVEMENT != kEvent.getImprovement())
		{
			setImprovementType(kEvent.getImprovement());
		}
	}
	else if (kEvent.getImprovementChange() < 0)
	{
		setImprovementType(NO_IMPROVEMENT);
	}

	if (kEvent.getBonusChange() > 0)
	{
		if (NO_BONUS != kEvent.getBonus())
		{
			setBonusType((BonusTypes)kEvent.getBonus());
		}
	}
	else if (kEvent.getBonusChange() < 0)
	{
		setBonusType(NO_BONUS);
	}

	if (kEvent.getRouteChange() > 0)
	{
		if (NO_ROUTE != kEvent.getRoute())
		{
			setRouteType((RouteTypes)kEvent.getRoute(), true);
		}
	}
	else if (kEvent.getRouteChange() < 0)
	{
		setRouteType(NO_ROUTE, true);
	}

	for (int i = 0; i < NUM_YIELD_TYPES; ++i)
	{
		const int iChange = kEvent.getPlotExtraYield(i);
		if (0 != iChange)
		{
			setExtraYield((YieldTypes)i, iChange);
		}
	}
}


bool CvPlot::canTrain(UnitTypes eUnit, bool bTestVisible) const
{
	PROFILE_FUNC();

	const CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	if (isCity())
	{
		if (kUnit.getDomainType() == DOMAIN_SEA)
		{
			if (!isWater() && !isCoastalLand(kUnit.getMinAreaSize()))
			{
				return false;
			}
		}
		else if (area()->getNumTiles() < kUnit.getMinAreaSize())
		{
			return false;
		}
	}
	else
	{
		if (area()->getNumTiles() < kUnit.getMinAreaSize())
		{
			return false;
		}

		if (kUnit.getDomainType() == DOMAIN_SEA)
		{
			if (!isWater())
			{
				return false;
			}
		}
		else if (kUnit.getDomainType() == DOMAIN_LAND)
		{
			if (isWater())
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	if (!isMapCategory(*this, kUnit))
	{
		return false;
	}

	if (!bTestVisible)
	{
		const CvCity* pCity = getPlotCity();

		if (kUnit.getPrereqAndBonus() != NO_BONUS)
		{
			if (NULL == pCity)
			{
				if (!isPlotGroupConnectedBonus(getOwner(), (BonusTypes)kUnit.getPrereqAndBonus()))
				{
					return false;
				}
			}
			else if (!pCity->hasBonus((BonusTypes)kUnit.getPrereqAndBonus()))
			{
				return false;
			}
		}

		bool bValid = true;
		foreach_(const BonusTypes ePrereqBonus, kUnit.getPrereqOrBonuses())
		{
			bValid = false;

			if (NULL == pCity)
			{
				if (isPlotGroupConnectedBonus(getOwner(), ePrereqBonus))
				{
					bValid = true;
					break;
				}
			}
			else if (pCity->hasBonus(ePrereqBonus))
			{
				bValid = true;
				break;
			}
		}
		if (!bValid)
		{
			return false;
		}
	}
	return true;
}

int CvPlot::countFriendlyCulture(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iTotalCulture = 0;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		const CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			const CvTeam& kLoopTeam = GET_TEAM(kLoopPlayer.getTeam());
			if (kLoopPlayer.getTeam() == eTeam || kLoopTeam.isVassal(eTeam) || kLoopTeam.isOpenBorders(eTeam))
			{
				iTotalCulture += getCulture((PlayerTypes)iPlayer);
			}
		}
	}

	return iTotalCulture;
}

int CvPlot::countNumAirUnits(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (DOMAIN_AIR == pLoopUnit->getDomainType() && !pLoopUnit->isCargo() && pLoopUnit->getTeam() == eTeam)
		{
			iCount += pLoopUnit->getUnitInfo().getAirUnitCap();
		}
	}

	return iCount;
}

int CvPlot::countNumAirUnitCargoVolume(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (DOMAIN_AIR == pLoopUnit->getDomainType() && !pLoopUnit->isCargo() && pLoopUnit->getTeam() == eTeam)
		{
			iCount += pLoopUnit->SMCargoVolume();
		}
	}

	return iCount;
}

int CvPlot::airUnitSpaceAvailable(TeamTypes eTeam) const
{
	int iMaxUnits = 0;

	const CvCity* pCity = getPlotCity();
	if (NULL != pCity)
	{
		if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
		{
			iMaxUnits = pCity->getSMAirUnitCapacity(getTeam());
		}
		else
		{
			iMaxUnits = pCity->getAirUnitCapacity(getTeam());
		}
	}
	else
	{
		if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
		{
			iMaxUnits = GC.getCITY_AIR_UNIT_CAPACITY() * 50;
		}
		else
		{
			iMaxUnits = GC.getCITY_AIR_UNIT_CAPACITY();
		}
	}

	if (GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS))
	{
		return (iMaxUnits - countNumAirUnitCargoVolume(eTeam));
	}
	return (iMaxUnits - countNumAirUnits(eTeam));
}

/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						10/17/08		jdog5000		*/
/* 																			*/
/* 	Air AI																	*/
/********************************************************************************/
int CvPlot::countAirInterceptorsActive(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (DOMAIN_AIR == pLoopUnit->getDomainType() && !pLoopUnit->isCargo() && pLoopUnit->getTeam() == eTeam)
		{
			if( pLoopUnit->getGroup()->getActivityType() == ACTIVITY_INTERCEPT )
			{
				iCount += 1;
			}
		}
	}

	return iCount;
}
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/

bool CvPlot::isEspionageCounterSpy(TeamTypes eTeam) const
{
	const CvCity* pCity = getPlotCity();

	if (NULL != pCity && pCity->getTeam() == eTeam)
	{
		if (pCity->getEspionageDefenseModifier() > 0)
		{
			return true;
		}
	}

	if (plotCount(PUF_isCounterSpy, -1, -1, NULL, NO_PLAYER, eTeam) > 0)
	{
		return true;
	}

	return false;
}

int CvPlot::getAreaIdForGreatWall() const
{
	return getArea();
}

int CvPlot::getSoundScriptId() const
{
	int iScriptId = -1;
	if (isActiveVisible(true))
	{
		if (getImprovementType() != NO_IMPROVEMENT)
		{
			iScriptId = GC.getImprovementInfo(getImprovementType()).getWorldSoundscapeScriptId();
		}
		else if (getFeatureType() != NO_FEATURE)
		{
			iScriptId = GC.getFeatureInfo(getFeatureType()).getWorldSoundscapeScriptId();
		}
		else if (getTerrainType() != NO_TERRAIN)
		{
			iScriptId = GC.getTerrainInfo(getTerrainType()).getWorldSoundscapeScriptId();
		}
	}
	return iScriptId;
}

int CvPlot::get3DAudioScriptFootstepIndex(int iFootstepTag) const
{
	if (getFeatureType() != NO_FEATURE)
	{
		return GC.getFeatureInfo(getFeatureType()).get3DAudioScriptFootstepIndex(iFootstepTag);
	}

	if (getTerrainType() != NO_TERRAIN)
	{
		return GC.getTerrainInfo(getTerrainType()).get3DAudioScriptFootstepIndex(iFootstepTag);
	}

	return -1;
}

float CvPlot::getAqueductSourceWeight() const
{
	if (isWaterAndIsFresh() || isAsPeak()
	|| getFeatureType() != NO_FEATURE && GC.getFeatureInfo(getFeatureType()).isAddsFreshWater())
	{
		return 1.0f;
	}
	if (isHills())
	{
		return 0.67f;
	}
	return 0.0f;
}

bool CvPlot::shouldDisplayBridge(CvPlot* pToPlot, PlayerTypes ePlayer) const
{
	TeamTypes eObservingTeam = GET_PLAYER(ePlayer).getTeam();
	TeamTypes eOurTeam = getRevealedTeam(eObservingTeam, true);
	TeamTypes eOtherTeam = NO_TEAM;
	if (pToPlot != NULL)
	{
		eOtherTeam = pToPlot->getRevealedTeam(eObservingTeam, true);
	}

	if (eOurTeam == eObservingTeam || eOtherTeam == eObservingTeam || (eOurTeam == NO_TEAM && eOtherTeam == NO_TEAM))
	{
		return GET_TEAM(eObservingTeam).isBridgeBuilding();
	}

	if (eOurTeam == NO_TEAM)
	{
		return GET_TEAM(eOtherTeam).isBridgeBuilding();
	}

	if (eOtherTeam == NO_TEAM)
	{
		return GET_TEAM(eOurTeam).isBridgeBuilding();
	}

	return (GET_TEAM(eOurTeam).isBridgeBuilding() && GET_TEAM(eOtherTeam).isBridgeBuilding());
}

bool CvPlot::checkLateEra() const
{
	PROFILE_EXTRA_FUNC();
	PlayerTypes ePlayer = getOwner();
	if (ePlayer == NO_PLAYER)
	{
		//find largest culture in this plot
		ePlayer = GC.getGame().getActivePlayer();
		int maxCulture = getCulture(ePlayer);
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			int newCulture = getCulture((PlayerTypes) i);
			if (newCulture > maxCulture)
			{
				maxCulture = newCulture;
				ePlayer = (PlayerTypes) i;
			}
		}
	}

	return (GET_PLAYER(ePlayer).getCurrentEra() > GC.getNumEraInfos() / 2);
}

// UncutDragon
bool CvPlot::hasDefender(
	bool bCheckCanAttack,
	PlayerTypes eOwner,
	PlayerTypes eAttacker,
	const CvUnit* pAttacker,
	bool bTestAtWar,
	bool bTestPotentialEnemy,
	bool bTestCanMove,
	bool bTestCanFight) const
{
	PROFILE_EXTRA_FUNC();

	foreach_(const CvUnit* unitX, units())
	{
		if (!unitX->isDead()
		&& (eOwner == NO_PLAYER || unitX->getOwner() == eOwner)
		&& (eAttacker == NO_PLAYER || !unitX->isInvisible(GET_PLAYER(eAttacker).getTeam(), false))
		&& (
				!bTestAtWar
				||
				eAttacker == NO_PLAYER
				||
				unitX->isEnemy(GET_PLAYER(eAttacker).getTeam(), this)
				||
				pAttacker
				&&
				pAttacker->isEnemy(GET_PLAYER(unitX->getOwner()).getTeam(), this, unitX)
			)
		&& (!pAttacker || !unitX->canUnitCoexistWithArrivingUnit(*pAttacker))
		&& (
				!bTestPotentialEnemy
				||
				eAttacker == NO_PLAYER
				||
				unitX->isPotentialEnemy(GET_PLAYER(eAttacker).getTeam(), this)
				||
				pAttacker
				&& pAttacker->isPotentialEnemy(GET_PLAYER(unitX->getOwner()).getTeam(), this, unitX)
			)
		&& (!bTestCanMove || unitX->canMove() && !unitX->isCargo())
		&& (!pAttacker || pAttacker->getDomainType() != DOMAIN_AIR || unitX->getDamage() < pAttacker->airCombatLimit(unitX))
		&& (!bCheckCanAttack || !pAttacker || pAttacker->canAttack(*unitX))
		&& (!bTestCanFight || unitX->canFight()))
		{
			return true; // found a valid defender
		}
	}
	return false; // there are no defenders
}

bool CvPlot::hasStealthDefender(const CvUnit* victim, const bool bReveal)
{
	PROFILE_EXTRA_FUNC();
	if (!victim
	||  victim->getDomainType() == DOMAIN_AIR
	|| !GC.getGame().isOption(GAMEOPTION_COMBAT_WITHOUT_WARNING))
	{
		return false;
	}
	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	foreach_(CvUnit* unitX, units())
	{
		if (unitX->canStealthDefend(victim))
		{
			// found a valid stealth defender
			if (bReveal)
			{
				const int iValue = unitX->AI_attackOdds(this, victim);
				if (iValue >= iBestValue)
				{
					iBestValue = iValue;
					pBestUnit = unitX;
				}
			}
			else return true;
		}
	}
	if (pBestUnit)
	{
		pBestUnit->reveal();
		updateCenterUnit();

		CvWString szBuffer = gDLL->getText(
			"TXT_KEY_MISC_STEALTH_DEFENSE_OWNER",
			pBestUnit->getNameKey(),
			GET_PLAYER(victim->getOwner()).getNameKey(),
			victim->getNameKey()
		);
		AddDLLMessage(
			pBestUnit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
			szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_MINOR_EVENT,
			pBestUnit->getButton(), GC.getCOLOR_UNIT_TEXT(),
			getX(), getY(), true, true
		);
		szBuffer = gDLL->getText(
			"TXT_KEY_MISC_STEALTH_DEFENSE",
			GET_PLAYER(pBestUnit->getVisualOwner(victim->getTeam())).getNameKey(),
			pBestUnit->getNameKey(), victim->getNameKey()
		);
		AddDLLMessage(
			victim->getOwner(), false, GC.getEVENT_MESSAGE_TIME(),
			szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_MINOR_EVENT,
			pBestUnit->getButton(), GC.getCOLOR_UNIT_TEXT(),
			getX(), getY(), true, true
		);
		return true; // revealed a stealth defender
	}
	return false; // there are no stealth defenders
}


void CvPlot::doPreAttackTraps(CvUnit* pAttacker)
{
	PROFILE_EXTRA_FUNC();

	if (!pAttacker)
	{
		return;
	}
	foreach_(CvUnit* unitX, units())
	{
		if (unitX->isArmedTrap()
		&& !unitX->isDead()
		&&  unitX->isEnemy(pAttacker->getTeam(), this, pAttacker)
		&&  unitX->isTriggerBeforeAttack()
		&& !unitX->canUnitCoexistWithArrivingUnit(*pAttacker))
		{
			unitX->doTrap(pAttacker);
		}
	}
}
// /UncutDragon


PlayerTypes CvPlot::getClaimingOwner() const
{
	return (PlayerTypes) m_eClaimingOwner;
}


void CvPlot::setClaimingOwner(PlayerTypes eNewValue)
{
	m_eClaimingOwner = eNewValue;
}


bool CvPlot::isActsAsCity() const
{
	return getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType()).isActsAsCity();
}


bool CvPlot::changeBuildProgress(BuildTypes eBuild, int iChange, PlayerTypes ePlayer)
{
	const bool bResult = changeBuildProgress(eBuild, iChange, GET_PLAYER(ePlayer).getTeam());

	if (bResult)
	{
		const ImprovementTypes eImprovement = GC.getBuildInfo(eBuild).getImprovement();

		if (eImprovement != NO_IMPROVEMENT && GC.getImprovementInfo(eImprovement).isMilitaryStructure())
		{
			setOwner(ePlayer, true, true);
		}
	}
	return bResult;
}


// Called at the end of turn. Sets the owner that claimed the plot the previous turn.
// NOTE: We can't make the plot neutral at the moment of claiming,
// because the aggressor could use it immediately (i.e. travel using roads).
void CvPlot::doTerritoryClaiming()
{
	if (m_eClaimingOwner != NO_PLAYER)
	{
		setOwner((PlayerTypes)m_eClaimingOwner, false, false);

		m_eClaimingOwner = NO_PLAYER;
	}
}


int CvPlot::getRevoltProtection() const
{
	if (!isCity()) return 0;

	return algo::accumulate(units() | transformed(CvUnit::fn::revoltProtectionTotal()), 0);
}

int CvPlot::getAverageEnemyStrength(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iStrength = 0;
	int iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (GET_TEAM(pLoopUnit->getTeam()).isAtWar(eTeam))
		{
			if (pLoopUnit->baseCombatStr() > 0)
			{
				iStrength += pLoopUnit->baseCombatStr();
				iCount++;
			}
		}
	}
	if (iCount > 0)
	{
		return iStrength / iCount;
	}
	return 0;
}

int CvPlot::getAverageEnemyDamage(TeamTypes eTeam) const
{
	PROFILE_EXTRA_FUNC();
	int iDamage = 0;
	int iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		if (GET_TEAM(pLoopUnit->getTeam()).isAtWar(eTeam))
		{
			if (pLoopUnit->baseCombatStr() > 0)
			{
				iDamage += pLoopUnit->getDamagePercent();
				iCount++;
			}
		}
	}
	if (iCount > 0)
	{
		return iDamage / iCount;
	}
	return 0;
}

LandmarkTypes CvPlot::getLandmarkType() const
{
	return m_eLandmarkType;
}
void CvPlot::setLandmarkType(LandmarkTypes eLandmark)
{
	m_eLandmarkType = eLandmark;
}
CvWString CvPlot::getLandmarkName() const
{
	return m_szLandmarkName;
}
void CvPlot::setLandmarkName(CvWString szName)
{
	m_szLandmarkName = szName;
}
CvWString CvPlot::getLandmarkMessage() const
{
	return m_szLandmarkMessage;
}
void CvPlot::setLandmarkMessage(CvWString szName)
{
	m_szLandmarkMessage = szName;
}

bool CvPlot::isCountedPlot() const
{
	return m_bCounted;
}
void CvPlot::setCountedPlot(bool bNewVal)
{
	m_bCounted = bNewVal;
}


const CvPlot* CvPlot::isInFortControl(bool bIgnoreObstructions, PlayerTypes eDefendingPlayer, PlayerTypes eAttackingPlayer, const CvPlot* pFort) const
{
	PROFILE_EXTRA_FUNC();
	if (!bIgnoreObstructions)
	{
		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(getImprovementType()).isZOCSource())
			{
				if (hasDefender(true, eDefendingPlayer, eAttackingPlayer))
				{
					return this;
				}
			}
		}
	}
	foreach_(const CvPlot* pAdjacentPlot, adjacent()
	| filtered(CvPlot::fn::getImprovementType() != NO_IMPROVEMENT))
	{
		if (GC.getImprovementInfo(pAdjacentPlot->getImprovementType()).isZOCSource())
		{
			if (pAdjacentPlot->hasDefender(true, eDefendingPlayer, eAttackingPlayer))
			{
				if (pFort == NULL || pAdjacentPlot == pFort)
				{
					return pAdjacentPlot;
				}
			}
		}
	}
	return NULL;
}

//Returns the first found player that controls an adjacent fort to this plot and is at war with the attackingTeam
PlayerTypes CvPlot::controlsAdjacentZOCSource(TeamTypes eAttackingTeam) const
{
	PROFILE_EXTRA_FUNC();
	if (getImprovementType() != NO_IMPROVEMENT)
	{
		if (GC.getImprovementInfo(getImprovementType()).isZOCSource())
		{
			foreach_(const CvUnit* pLoopUnit, units())
			{
				if (GET_TEAM(pLoopUnit->getTeam()).isAtWar(eAttackingTeam))
				{
					return pLoopUnit->getOwner();
				}
			}
		}
	}

	foreach_(const CvPlot* pAdjacentPlot, adjacent()
	| filtered(CvPlot::fn::getImprovementType() != NO_IMPROVEMENT))
	{
		if (GC.getImprovementInfo(pAdjacentPlot->getImprovementType()).isZOCSource())
		{
			foreach_(const CvUnit* pLoopUnit, pAdjacentPlot->units())
			{
				if (GET_TEAM(pLoopUnit->getTeam()).isAtWar(eAttackingTeam))
				{
					return pLoopUnit->getOwner();
				}
			}
		}
	}

	return NO_PLAYER;
}

bool CvPlot::isInCityZoneOfControl(PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPlot* pAdjacentPlot, adjacent()
	| filtered(CvPlot::fn::isCity(false, NO_TEAM)))
	{
		const CvCity* pAdjacentCity = pAdjacentPlot->getPlotCity();
		if (pAdjacentCity->isZoneOfControl())
		{
			return GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(pAdjacentCity->getTeam());
		}
	}
	return false;
}


int CvPlot::getTotalTurnDamage(const CvSelectionGroup* pGroup) const
{
	PROFILE_EXTRA_FUNC();
	int iMaxDamage = 0;

	foreach_(const CvUnit* pLoopUnit, pGroup->units())
	{
		const int iDamage = getTotalTurnDamage(pLoopUnit);

		if ( iDamage > iMaxDamage )
		{
			iMaxDamage = iDamage;
		}
	}

	return iMaxDamage;
}

int CvPlot::getTerrainTurnDamage(const CvUnit* pUnit) const
{
	//PROFILE_FUNC();

	int iDamagePercent = -GC.getTerrainInfo(getTerrainType()).getHealthPercent();
	if (iDamagePercent == 0 || !GC.getGame().isModderGameOption(MODDERGAMEOPTION_TERRAIN_DAMAGE))
	{
		return 0;
	}
	if (getFeatureType() != NO_FEATURE)
	{
		// No damage on Oasis or Flood Plain or Ancient Forest
		if (GC.getFeatureInfo(getFeatureType()).getYieldChange(YIELD_FOOD) > 0)
		{
			return 0;
		}
		// No damage on Cave
		if (getFeatureType() == GC.getInfoTypeForString("FEATURE_CAVES"))
		{
			return 0;
		}
	}
	if (isCity(true))
	{
		return 0;
	}
	if (getImprovementType() != NO_IMPROVEMENT)
	{
		iDamagePercent /= 2;
	}
	if (pUnit != NULL)
	{
		if (pUnit->isTerrainProtected(getTerrainType()))
		{
			return 0;
		}
		if (pUnit->getTeam() == getTeam())
		{
			iDamagePercent /= 3;
		}
		if (pUnit->isAnimal())
		{
			return 0;
		}
		if (pUnit->isHominid())
		{
			iDamagePercent /= 3;
		}
	}

	return iDamagePercent;
}

int CvPlot::getFeatureTurnDamage() const
{
	return getFeatureType() != NO_FEATURE ? GC.getFeatureInfo(getFeatureType()).getTurnDamage() : 0;
}

int CvPlot::getTotalTurnDamage(const CvUnit* pUnit) const
{
	return getTerrainTurnDamage(pUnit) + getFeatureTurnDamage();
}

CvUnit* CvPlot::getWorstDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove, bool bAssassinate) const
{
	PROFILE_EXTRA_FUNC();
	CvUnit* pBestUnit = NULL;

	foreach_(CvUnit* pLoopUnit, units())
	{
		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
		{
			if ((eAttackingPlayer == NO_PLAYER) || !(pLoopUnit->isInvisible(GET_PLAYER(eAttackingPlayer).getTeam(), false)))
			{
				if (!bTestAtWar || eAttackingPlayer == NO_PLAYER || pLoopUnit->isEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), this, pLoopUnit)))
				{
					if (NULL == pAttacker || !pLoopUnit->canUnitCoexistWithArrivingUnit(*pAttacker))
					{
						if (!bTestPotentialEnemy || (eAttackingPlayer == NO_PLAYER) || pLoopUnit->isPotentialEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isPotentialEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), this, pLoopUnit)))
						{
							if (!bTestCanMove || (pLoopUnit->canMove() && !(pLoopUnit->isCargo())))
							{
								if ((pAttacker == NULL) || (pAttacker->getDomainType() != DOMAIN_AIR) || (pLoopUnit->getDamage() < pAttacker->airCombatLimit(pLoopUnit)))
								{
									if (pBestUnit == NULL || (pLoopUnit->canDefend() && !pLoopUnit->isBetterDefenderThan(pBestUnit, pAttacker)))
									{
										pBestUnit = pLoopUnit;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}

bool CvPlot::isInUnitZoneOfControl(PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		foreach_(const CvUnit* pLoopUnit, pAdjacentPlot->units()
		| filtered(bind(CvUnit::isZoneOfControl, _1)))
		{
			return GET_TEAM(pLoopUnit->getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam());
		}
	}
	return false;
}

bool CvPlot::isBorder(bool bIgnoreWater) const
{
	PROFILE_EXTRA_FUNC();
	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		if (pAdjacentPlot->isWater() && !bIgnoreWater)
		{
			if (!isWater())
			{
				return true;
			}
		}
		if (pAdjacentPlot->getOwner() != getOwner())
		{
			return true;
		}
	}

	return false;
}

int CvPlot::getNumVisibleAdjacentEnemyDefenders(const CvUnit* pUnit) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		iCount += pAdjacentPlot->plotCount(PUF_canDefendEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(this), NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
	}
	return iCount;
}

void CvPlot::addSign(PlayerTypes ePlayer, CvWString szMessage)
{
	Cy::call(PYCivModule, "AddSign", Cy::Args() << this << ePlayer << szMessage.GetCString());
}

void CvPlot::removeSign(PlayerTypes ePlayer)
{
	Cy::call(PYCivModule, "RemoveSign", Cy::Args() << this << ePlayer);
}

void CvPlot::removeSignForAllPlayers()
{
	PROFILE_EXTRA_FUNC();
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		removeSign((PlayerTypes)iI);
	}
}

void CvPlot::ToggleInPlotGroupsZobristContributors()
{
	PROFILE_EXTRA_FUNC();
	for (int iI= 0; iI < MAX_PLAYERS; iI++)
	{
		CvPlotGroup* p_plotGroup = getPlotGroup((PlayerTypes)iI);

		if (p_plotGroup != NULL)
		{
			//	Add the zobrist contribution of this plot to the hash
			p_plotGroup->m_zobristHashes.resourceNodesHash ^= getZobristContribution();
		}
	}
}

int CvPlot::getBorderPlotCount() const
{
	return algo::count_if(adjacent(), CvPlot::fn::getOwner() != getOwner());
}

int CvPlot::getEnemyBorderPlotCount(PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;
	foreach_(const CvPlot* pAdjacentPlot, adjacent())
	{
		const PlayerTypes ePlayerX = pAdjacentPlot->getOwner();
		if (ePlayerX != NO_PLAYER && ePlayerX != ePlayer)
		{
			const AttitudeTypes eAttitude = GET_PLAYER(ePlayerX).AI_getAttitude(ePlayer);

			if (eAttitude < ATTITUDE_CAUTIOUS)
			{
				iCount += 1 + (eAttitude < ATTITUDE_ANNOYED);
			}
		}
	}
	return iCount;
}


CvProperties* CvPlot::getProperties()
{
	return &m_Properties;
}

const CvProperties* CvPlot::getPropertiesConst() const
{
	return &m_Properties;
}

bool CvPlot::HaveCachedPathValidityResult(void* entity, bool bIsAlternateResult, bool& cachedResult) const
{
	if (m_iCachePathEpoch == m_iGlobalCachePathEpoch)
	{
		if (bIsAlternateResult)
		{
			if (m_cachedPathAlternateValidityEntity == entity)
			{
				cachedResult = m_cachedPathAlternateValidity;
				return true;
			}
			return false;
		}
		else if (m_cachedPathValidityEntity == entity)
		{
			cachedResult = m_cachedPathValidity;
			return true;
		}
		return false;
	}
	return false;
}

void CvPlot::CachePathValidityResult(void* entity, bool bIsAlternateResult, bool cachedResult)
{
	if ( bIsAlternateResult )
	{
		m_cachedPathAlternateValidity = cachedResult;
		m_cachedPathAlternateValidityEntity = entity;
	}
	else
	{
		m_cachedPathValidity = cachedResult;
		m_cachedPathValidityEntity = entity;
	}

	m_iCachePathEpoch = m_iGlobalCachePathEpoch;
}

void	CvPlot::NextCachePathEpoch()
{
	m_iGlobalCachePathEpoch++;
}

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/

void CvPlot::destroyGraphics()
{
	//gDLL->getFeatureIFace()->destroy(m_pFeatureSymbol);
	//if(m_pPlotBuilder)
	//{
	//	gDLL->getPlotBuilderIFace()->destroy(m_pPlotBuilder);
	//	m_pPlotBuilder = NULL;
	//}
	//gDLL->getRouteIFace()->destroy(m_pRouteSymbol);
	//gDLL->getRiverIFace()->destroy(m_pRiverSymbol);
	//gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
	//gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
	//m_pCenterUnit = NULL;
	//m_pFeatureSymbol = NULL;
	//m_pRouteSymbol = NULL;
	//m_pRiverSymbol = NULL;
	//m_pFlagSymbol = NULL;
	//m_pFlagSymbolOffset = NULL;

	hideGraphics(ECvPlotGraphics::ALL);

	m_bPlotLayoutDirty = false;
	m_bLayoutStateWorked = false;
	m_bFlagDirty = false;

	// deleteAllSymbols();

	// m_visibleGraphics = ECvPlotGraphics::NONE;
}

bool CvPlot::isNull() const
{
	return m_bNull;
}

void CvPlot::setNull(bool bNull)
{
	m_bNull = bNull;
}

/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

bool CvPlot::bDeferPlotGroupRecalculation = false;

void CvPlot::setDeferredPlotGroupRecalculationMode(bool bDefer)
{
	PROFILE_EXTRA_FUNC();
	if ( bDeferPlotGroupRecalculation != bDefer )
	{
		bDeferPlotGroupRecalculation = bDefer;

		if ( !bDefer )
		{
			CvPlotGroup::startBulkRecalculate();

			for(int iI = 0; iI < GC.getMap().numPlots(); iI++)
			{
				CvPlot*	pPlot = GC.getMap().plotByIndex(iI);

				if ( pPlot != NULL && pPlot->m_bPlotGroupsDirty )
				{
					pPlot->updatePlotGroup();
				}
			}

			CvPlotGroup::endBulkRecalculate();
		}
	}
}

void CvPlot::enableCenterUnitRecalc(bool bEnable)
{
	m_bInhibitCenterUnitCalculation = !bEnable;

	if (bEnable)
	{
		updateCenterUnit();
	}
}

bool CvPlot::isImprovementUpgradable() const
{
	return m_bImprovementUpgradable;
}

void CvPlot::setImprovementUpgradeCache(const int iNewValue)
{
	m_iCurrentRoundofUpgradeCache = iNewValue;

	if (iNewValue == -1)
	{
		m_iImprovementUpgradeHash = 0;
	}
}

int CvPlot::getInjuredUnitCombatsUnsupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = plotCount(PUF_isInjuredUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer);

	if (iCount > 0)
	{
		foreach_(const CvUnit* pLoopUnit, units())
		{
			if (pLoopUnit->getOwner() == ePlayer && pLoopUnit->getNumHealSupportTotal() > 0 && pLoopUnit->getHealUnitCombatTypeTotal(eUnitCombat) > 0)
			{
				iCount -=  (pLoopUnit->getNumHealSupportTotal()+1/*+1 to count itself*/);
			}
		}
	}
	return std::max(0, iCount);
}

int CvPlot::getUnitCombatsUnsupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = plotCount(PUF_isHealUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer);

	int iHighestOffset = 2;
	if (iCount > 0)
	{
		foreach_(const CvUnit* pLoopUnit, units())
		{
			if (pLoopUnit->getOwner() == ePlayer && pLoopUnit->getNumHealSupportTotal() > 0 && pLoopUnit->getHealUnitCombatTypeTotal(eUnitCombat) > 0)
			{
				const int iOffset = (pLoopUnit->getNumHealSupportTotal() +1);
				iCount -= iOffset;
				if (iOffset > iHighestOffset)
				{
					iHighestOffset = iOffset;
				}
			}
		}
		iCount -= (iHighestOffset * GET_PLAYER(ePlayer).AI_plotTargetMissionAIs(const_cast<CvPlot*>(this), MISSIONAI_HEAL_SUPPORT));
	}
	return std::max(0, (iCount/3));
}

int CvPlot::getOverloadedUnitCombatsSupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain) const
{
	PROFILE_EXTRA_FUNC();
	int iOverload = 0;

	int iCount = plotCount(PUF_isHealUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer);

	if (iCount > 0)
	{
		foreach_(const CvUnit* pLoopUnit, units())
		{
			if (pLoopUnit->getOwner() == ePlayer && pLoopUnit->getNumHealSupportTotal() > 0 && pLoopUnit->getHealUnitCombatTypeTotal(eUnitCombat) > 0)
			{
				iCount -= pLoopUnit->getNumHealSupportTotal();
				if (iCount < 0)
				{
					iOverload++;
				}
			}
		}
	}
	return std::max(0, iOverload);
}

void CvPlot::unitGameStateCorrections()
{
	PROFILE_EXTRA_FUNC();
	bool bUpdate = false;

	for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL;)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit == NULL)
		{
			pUnitNode = m_units.deleteNode(pUnitNode);
			bUpdate = true;
		}
		else
		{
			if (!pLoopUnit->atPlot(this))
			{
				removeUnit(pLoopUnit);
				bUpdate = true;
			}
			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	if (bUpdate)
	{
		updateCenterUnit();
		setFlagDirty(true);
	}
}

bool CvPlot::isMapCategoryType(MapCategoryTypes eMapCategory) const
{
	const std::vector<MapCategoryTypes>& plotMapCategories = getMapCategories();
	return plotMapCategories.empty() || algo::any_of_equal(plotMapCategories, eMapCategory);
}

const std::vector<MapCategoryTypes>& CvPlot::getMapCategories() const
{
	return GC.getTerrainInfo(getTerrainType()).getMapCategories();
}

int CvPlot::countSeeInvisibleActive(PlayerTypes ePlayer, InvisibleTypes eVisible) const
{
	PROFILE_EXTRA_FUNC();
	int iCount = 0;

	foreach_(const CvUnit* pLoopUnit, units())
	{
		const UnitAITypes eAIType = pLoopUnit->AI_getUnitAIType();
		//MissionAITypes eMissionAI = pLoopUnit->getGroup()->AI_getMissionAIType();
		
		if (eAIType == UNITAI_SEE_INVISIBLE || eAIType == UNITAI_SEE_INVISIBLE_SEA 
			|| eAIType == UNITAI_PROPERTY_CONTROL || eAIType == UNITAI_CITY_DEFENSE
			|| eAIType == UNITAI_HUNTER_ESCORT || eAIType == UNITAI_HUNTER
			|| eAIType == UNITAI_PILLAGE_COUNTER || eAIType == UNITAI_INVESTIGATOR
			|| eAIType == UNITAI_RESERVE || eAIType == UNITAI_COUNTER
			|| eAIType == UNITAI_SPY || eAIType == UNITAI_CITY_COUNTER)
		{
			if (pLoopUnit->getOwner() == ePlayer)
			{
				if (GC.getGame().isOption(GAMEOPTION_COMBAT_HIDE_SEEK))
				{
					if (GC.getUnitInfo(pLoopUnit->getUnitType()).getVisibilityIntensityType(eVisible) > 0)
					{
						iCount += 1;
					}
				}
				else
				{
					for (int iI = 0; iI < GC.getUnitInfo(pLoopUnit->getUnitType()).getNumSeeInvisibleTypes(); ++iI)
					{
						if (GC.getUnitInfo(pLoopUnit->getUnitType()).getSeeInvisibleType(iI) == (int)eVisible)
						{
							iCount += 1;
						}
					}
				}
			}
		}
	}

	return iCount;
}

#ifdef OUTBREAKS_AND_AFFLICTIONS
int CvPlot::getNumAfflictedUnits(PlayerTypes eOwner, PromotionLineTypes eAfflictionLine) const
{
	return plotCount(PUF_isAfflicted, eAfflictionLine, -1, NULL, eOwner);
}

int CvPlot::getCommunicability(PromotionLineTypes ePromotionLine, bool bWorkedTile, bool bVicinity, bool bAccessVolume) const
{
	int iCommunicability = 0;
	TerrainTypes eTerrain = getTerrainType();
	if (eTerrain != NO_TERRAIN)
	{
		iCommunicability += GC.getTerrainInfo(eTerrain).getAfflictionCommunicabilityType(ePromotionLine, bWorkedTile, bVicinity, false).iModifier;
	}
	FeatureTypes eFeature = getFeatureType();
	if (eFeature != NO_FEATURE)
	{
		iCommunicability += GC.getFeatureInfo(eFeature).getAfflictionCommunicabilityType(ePromotionLine, bWorkedTile, bVicinity, false).iModifier;
	}
	BonusTypes eBonus = getBonusType();
	if (eBonus != NO_BONUS)
	{
		iCommunicability += GC.getBonusInfo(eBonus).getAfflictionCommunicabilityType(ePromotionLine, bWorkedTile, bVicinity, bAccessVolume).iModifier;
	}

	return iCommunicability;
}
#endif // OUTBREAKS_AND_AFFLICTIONS

CvUnit* CvPlot::unit_iterator::resolve(const IDInfo& info) const
{
	return ::getUnit(info);
}

//CvPlot::const_unit_iterator::value_type* CvPlot::const_unit_iterator::resolve(const IDInfo& info) const
//{
//	return ::getUnit(info);
//}

CvPlot::adjacent_iterator::adjacent_iterator()
	: m_centerX(-1)
	, m_centerY(-1)
	, m_numPlots(0)
	, m_plotDirectionX(nullptr)
	, m_plotDirectionY(nullptr)
	, m_map(nullptr)
	, m_curr(nullptr)
	, m_idx(0)
{}

CvPlot::adjacent_iterator::adjacent_iterator(int centerX, int centerY, int numPlots, const int* plotDirectionX, const int* plotDirectionY)
	: m_centerX(centerX)
	, m_centerY(centerY)
	, m_numPlots(numPlots)
	, m_plotDirectionX(plotDirectionX)
	, m_plotDirectionY(plotDirectionY)
	, m_map(&GC.getMap())
	, m_curr(nullptr)
	, m_idx(0)
{
	increment();
}

void CvPlot::adjacent_iterator::increment()
{
	PROFILE_EXTRA_FUNC();
	m_curr = nullptr;
	while (m_curr == nullptr && m_idx < m_numPlots)
	{
		m_curr = m_map->plot(m_centerX + m_plotDirectionX[m_idx], m_centerY + m_plotDirectionY[m_idx]);
		++m_idx;
	}
}
bool CvPlot::adjacent_iterator::equal(CvPlot::adjacent_iterator const& other) const
{
	return (m_centerX == other.m_centerX
		&& m_centerY == other.m_centerY
		&& m_idx == other.m_idx)
		|| (m_curr == NULL && other.m_curr == NULL);
}

CvPlot::adjacent_iterator CvPlot::beginAdjacent(int numPlots, const int* plotDirectionX, const int* plotDirectionY) const
{
	return CvPlot::adjacent_iterator(m_iX, m_iY, numPlots, plotDirectionX, plotDirectionY);
}

CvPlot::adjacent_iterator CvPlot::endAdjacent() const
{
	return CvPlot::adjacent_iterator();
}

CvPlot::adjacent_range CvPlot::adjacent() const
{
	return CvPlot::adjacent_range(beginAdjacent(NUM_DIRECTION_TYPES, GC.getPlotDirectionX(), GC.getPlotDirectionY()), endAdjacent());
}

CvPlot::adjacent_range CvPlot::cardinalDirectionAdjacent() const
{
	return CvPlot::adjacent_range(beginAdjacent(NUM_CARDINALDIRECTION_TYPES, GC.getPlotCardinalDirectionX(), GC.getPlotCardinalDirectionY()), endAdjacent());
}


void CvPlot::countCommander(bool bNewVal, const CvUnit* com)
{
	PROFILE_EXTRA_FUNC();
	const short iRange = com->getCommanderComp()->getCommandRange();

	const PlayerTypes eComOwner = com->getOwner();

	std::map<uint8_t, uint16_t>::const_iterator itr = m_commanderCount.find(static_cast<uint8_t>(eComOwner));
	//OutputDebugString(CvString::format("countCommander (%d, %d): range=%d; iChange=%d\n", getX(), getY(), iRange, bNewVal ? 1 : -1).c_str());

	if (iRange > 0)
	{
		foreach_(CvPlot* plotX, rect(iRange, iRange))
		{
			plotX->changeCommanderCount(eComOwner, bNewVal);
		}
	}
	else changeCommanderCount(eComOwner, bNewVal);
}

void CvPlot::changeCommanderCount(const PlayerTypes ePlayer, const bool bAdd)
{
	std::map<uint8_t, uint16_t>::const_iterator itr = m_commanderCount.find(static_cast<uint8_t>(ePlayer));

	//OutputDebugString(CvString::format("changeCommanderCount plot at (%d, %d): oldCount=%d\n", getX(), getY(), (itr == m_commanderCount.end()) ? 0 : itr->second).c_str());

	if (itr == m_commanderCount.end())
	{
		if (bAdd) // Add commander count
		{
			m_commanderCount.insert(std::make_pair(static_cast<uint8_t>(ePlayer), 1));
			GET_PLAYER(ePlayer).setCommandFieldPlot(true, this);
		}
		else FErrorMsg("Expected commander addition for first player entry on this plot");
	}
	else if (!bAdd && itr->second < 2) // Remove commander count
	{
		FAssertMsg(itr->second > 0, "This change would bring the count to a negative value! Code copes with it though")
		m_commanderCount.erase(itr->first);
		GET_PLAYER(ePlayer).setCommandFieldPlot(false, this);
	}
	else // change commander count
	{
		m_commanderCount[itr->first] += bAdd ? 1 : -1;
	}
	//itr = m_commanderCount.find(static_cast<uint8_t>(ePlayer));
	//OutputDebugString(CvString::format("changeCommanderCount plot at (%d, %d): newCount=%d\n", getX(), getY(), (itr == m_commanderCount.end()) ? 0 : itr->second).c_str());
}

bool CvPlot::inCommandField(const PlayerTypes ePlayer) const
{
	return m_commanderCount.find(static_cast<uint8_t>(ePlayer)) != m_commanderCount.end();
}


void CvPlot::countCommodore(bool bNewVal, const CvUnit* com)
{
	PROFILE_EXTRA_FUNC();
	const short iRange = com->getCommodoreComp()->getCommandRange();

	const PlayerTypes eComOwner = com->getOwner();

	std::map<uint8_t, uint16_t>::const_iterator itr = m_commodoreCount.find(static_cast<uint8_t>(eComOwner));

	if (iRange > 0)
	{
		foreach_(CvPlot* plotX, rect(iRange, iRange))
		{
			plotX->changeCommodoreCount(eComOwner, bNewVal);
		}
	}
	else changeCommodoreCount(eComOwner, bNewVal);
}


void CvPlot::changeCommodoreCount(const PlayerTypes ePlayer, const bool bAdd)
{
	std::map<uint8_t, uint16_t>::const_iterator itr = m_commodoreCount.find(static_cast<uint8_t>(ePlayer));

	if (itr == m_commodoreCount.end())
	{
		if (bAdd) // Add commodore count
		{
			m_commodoreCount.insert(std::make_pair(static_cast<uint8_t>(ePlayer), 1));
			GET_PLAYER(ePlayer).setCommodoreFieldPlot(true, this);
		}
		else FErrorMsg("Expected commodore addition for first player entry on this plot");
	}
	else if (!bAdd && itr->second < 2) // Remove commodore count
	{
		FAssertMsg(itr->second > 0, "This change would bring the count to a negative value! Code copes with it though")
		m_commodoreCount.erase(itr->first);
		GET_PLAYER(ePlayer).setCommodoreFieldPlot(false, this);
	}
	else // change commodore count
	{
		m_commodoreCount[itr->first] += bAdd ? 1 : -1;
	}
}

bool CvPlot::inCommandCommodoreField(const PlayerTypes ePlayer) const
{
	return m_commodoreCount.find(static_cast<uint8_t>(ePlayer)) != m_commodoreCount.end();
}


int CvPlot::getCultureRateThisTurn(const PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	for (std::vector< std::pair<PlayerTypes, int> >::const_iterator it = m_cultureRatesThisTurn.begin(); it != m_cultureRatesThisTurn.end(); ++it)
	{
		if ((*it).first == ePlayer)
		{
			return ((*it).second);
		}
	}
	return 0;
}

int CvPlot::getCultureRateLastTurn(const PlayerTypes ePlayer) const
{
	PROFILE_EXTRA_FUNC();
	for (std::vector< std::pair<PlayerTypes, int> >::const_iterator it = m_cultureRatesLastTurn.begin(); it != m_cultureRatesLastTurn.end(); ++it)
	{
		if ((*it).first == ePlayer)
		{
			return ((*it).second);
		}
	}
	return 0;
}

void CvPlot::setInCultureRangeOfCityByPlayer(const PlayerTypes ePlayer)
{
	if (find(m_influencedByCityByPlayer.begin(), m_influencedByCityByPlayer.end(), ePlayer) == m_influencedByCityByPlayer.end())
	{
		m_influencedByCityByPlayer.push_back(ePlayer);
	}
}

bool CvPlot::isInCultureRangeOfCityByPlayer(const PlayerTypes ePlayer) const
{
	return (
		find(m_influencedByCityByPlayerLastTurn.begin(), m_influencedByCityByPlayerLastTurn.end(), ePlayer) != m_influencedByCityByPlayerLastTurn.end()
		||
		find(m_influencedByCityByPlayer.begin(), m_influencedByCityByPlayer.end(), ePlayer) != m_influencedByCityByPlayer.end()
	);
}