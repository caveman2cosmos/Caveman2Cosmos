// plot.cpp

#include "CvGameCoreDLL.h"
#include "CvDLLSymbolIFaceBase.h"
#include "CvDLLPlotBuilderIFaceBase.h"
#include "CvDLLFlagEntityIFaceBase.h"

#include "CvPlotPaging.h"

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
	if ( m_resultHashMap == NULL )
	{
		m_resultHashMap = new stdext::hash_map<int,int>();
	}

	if ( g_bestDefenderCache == NULL )
	{
		g_bestDefenderCache = new DefenderScoreCache();
	}

	m_aiYield = new short[NUM_YIELD_TYPES];
#ifdef UNUSED_SUPER_FORTS_METHODS
	m_aiCultureRangeForts = NULL; // Super Forts *culture*
#endif
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
	// Plot danger cache
	m_abIsTeamBorderCache = new bool[MAX_TEAMS];
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	m_aiFoundValue = NULL;
	m_aiPlayerCityRadiusCount = NULL;
	m_aiPlotGroup = NULL;
	m_aiVisibilityCount = NULL;
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
	m_apaiCultureRangeCities = NULL;
	m_apaiInvisibleVisibilityCount = NULL;
	//m_apaiCachedHighestTeamInvisibilityIntensity = NULL;
/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_aiOccupationCultureRangeCities = NULL;
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	m_aiMountainLeaderCount = NULL;	//	Koshling - mountain mod efficiency
	m_pFeatureSymbol = NULL;
	m_pPlotBuilder = NULL;
	m_pRouteSymbol = NULL;
	m_pRiverSymbol = NULL;
	m_pFlagSymbol = NULL;
	m_pFlagSymbolOffset = NULL;
	m_pCenterUnit = NULL;
	m_bInhibitCenterUnitCalculation = false;
	m_bIgnoringImprovementUpgrade = false;
	m_iCanHaveImprovementAsUpgradeCache = -1;
	m_iCurrentRoundofUpgradeCache = 0;
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

	SAFE_DELETE_ARRAY(m_aiYield);

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
	// Plot danger cache
	SAFE_DELETE_ARRAY(m_abIsTeamBorderCache);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
}

void CvPlot::init(int iX, int iY)
{
	//--------------------------------
	// Init saved data
	reset(iX, iY);
	m_aPlotTeamVisibilityIntensity.clear();
	//--------------------------------
	// Init non-saved data

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
#ifdef UNUSED_SUPER_FORTS_METHODS
	SAFE_DELETE_ARRAY(m_aiCultureRangeForts); // Super Forts *culture*
#endif
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
	SAFE_DELETE_ARRAY(m_aiOccupationCultureRangeCities);
	SAFE_DELETE_ARRAY(m_paiBuildProgress);
	SAFE_DELETE_ARRAY2(m_apaiCultureRangeCities, MAX_PLAYERS);
	SAFE_DELETE_ARRAY2(m_apaiInvisibleVisibilityCount, MAX_TEAMS);
	//	SAFE_DELETE_ARRAY2(m_apaiCachedHighestTeamInvisibilityIntensity, MAX_TEAMS);

	m_aPlotTeamVisibilityIntensity.clear();
	m_units.clear();
}

// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlot::reset(int iX, int iY, bool bConstructorCall)
{
	int iI;

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

/************************************************************************************************/
/* DCM	                  Start		 05/31/10                                Johnny Smith       */
/*                                                                           Afforess           */
/* Battle Effects                                                                               */
/************************************************************************************************/
	// Dale - BE: Battle Effect START
	m_iBattleCountdown = 0;
	//Not saved intentionally
	m_eBattleEffect = NO_EFFECT;
	// Dale - BE: Battle Effect END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
	m_iX = iX;
	m_iY = iY;
	m_iArea = FFreeList::INVALID_INDEX;
	m_pPlotArea = NULL;
	m_iFeatureVariety = 0;
	m_iOwnershipDuration = 0;
	m_iImprovementDuration = 0;
	m_iUpgradeProgress = 0;
	m_iForceUnownedTimer = 0;
	m_iCityRadiusCount = 0;
	m_iRiverID = -1;
	m_iMinOriginalStartDist = -1;
	m_iReconCount = 0;
	m_iRiverCrossingCount = 0;

	// Super Forts begin *canal* *choke*
	m_iCanalValue = 0;
	m_iChokeValue = 0;
	// Super Forts end
	// Super Forts begin *bombard*
	m_iDefenseDamage = 0;
	m_bBombarded = false;
	// Super Forts end

	m_bStartingPlot = false;
	m_bHills = false;
/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	m_bDepletedMine = false; // deleteMe
	m_eClaimingOwner = NO_PLAYER;
	m_bCounted = false;
	m_eLandmarkType = NO_LANDMARK;
	m_szLandmarkMessage = "";
	m_szLandmarkName = "";
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
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

	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		m_aiYield[iI] = 0;
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
	// Plot danger cache
	m_bIsActivePlayerNoDangerCache = false;
	m_bIsActivePlayerHasDangerCache = false;

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_abIsTeamBorderCache[iI] = false;
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	m_pathGenerationSeq = -1;

	m_Properties.clear();

	m_bPlotGroupsDirty = false;
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
	if (!isGraphicPagingEnabled())
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
	FAssertMsg(isGraphicPagingEnabled(), "Graphics paging is not enabled");
	// Show all graphics, as we aren't paging any more
	setRequireGraphicsVisible(ECvPlotGraphics::ALL, true);
	// Disable paging
	pageGraphicsOut();
}

//bool	CvPlot::shouldHaveFullGraphics() const
//{
//	return (!GC.getGraphicalDetailPagingEnabled() || m_iGraphicsPageIndex != -1) && shouldHaveGraphics();
//}

bool CvPlot::shouldHaveGraphics() const
{
	return GC.IsGraphicsInitialized() && isInViewport(); // && isRevealed(GC.getGame().getActiveTeam(), false);
}

//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvPlot::setupGraphical()
{
	//MEMORY_TRACE_FUNCTION();
	PROFILE_FUNC();

	showRequiredGraphics();

	//if ( !shouldHaveGraphics() )
	//{
	//	return;
	//}

	////updateSymbols();
	//
	//updateFeatureSymbol();
	//updateRiverSymbol();
	//updateMinimapColor();

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
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pCity;
	CvUnit* pLoopUnit;
	CLinkList<IDInfo> oldUnits;

	// kill units
	oldUnits.clear();

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		oldUnits.insertAtEnd(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
	}

	pUnitNode = oldUnits.head();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = oldUnits.next(pUnitNode);

		if (pLoopUnit != NULL)
		{
			pLoopUnit->kill(false, NO_PLAYER, true);
		}
	}

	// kill cities
	pCity = getPlotCity();
	if (pCity != NULL)
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

float CvPlot::getPointX() const
{
	CvViewport* pCurrentViewport = GC.getCurrentViewport();

	FAssert(pCurrentViewport != NULL);

	return pCurrentViewport->plotXToPointX(pCurrentViewport->getViewportXFromMapX(getX()));
}


float CvPlot::getPointY() const
{
	CvViewport* pCurrentViewport = GC.getCurrentViewport();

	FAssert(pCurrentViewport != NULL);

	return pCurrentViewport->plotYToPointY(pCurrentViewport->getViewportYFromMapY(getY()));
}


NiPoint3 CvPlot::getPoint() const
{
	NiPoint3 pt3Point;

	memset(&pt3Point, 0, sizeof(pt3Point));

	if (this == NULL)
	{
		FAssert(false);
	}
	else
	{
		pt3Point.x = getPointX();
		pt3Point.y = getPointY();
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
		else
		{
			return 1.2f;
		}
	}
	else
	{
		if (isShowCitySymbols())
		{
			return 1.2f;
		}
		else
		{
			return 0.8f;
		}
	}
}


float CvPlot::getSymbolOffsetX(int iOffset) const
{
	return ((40.0f + (((float)iOffset) * 28.0f * getSymbolSize())) - (GC.getPLOT_SIZE() / 2.0f));
}


float CvPlot::getSymbolOffsetY(int iOffset) const
{
	return (-(GC.getPLOT_SIZE() / 2.0f) + 50.0f);
}


TeamTypes CvPlot::getTeam() const
{
	if (isOwned())
	{
		return GET_PLAYER(getOwner()).getTeam();
	}
	else
	{
		return NO_TEAM;
	}
}


void CvPlot::doTurn()
{
	PROFILE_FUNC();

	if (getForceUnownedTimer() > 0)
	{
		changeForceUnownedTimer(-1);
	}

	if (isOwned())
	{
		changeOwnershipDuration(1);
	}

	if (getImprovementType() != NO_IMPROVEMENT)
	{
		changeImprovementDuration(1);
		if (isOwned())
		{
			doImprovement();
		}
	}

	// Super Forts begin *bombard*
	if (!isBombarded() && getDefenseDamage() > 0)
	{
		changeDefenseDamage(-(GC.getDefineINT("CITY_DEFENSE_DAMAGE_HEAL_RATE")));
	}
	setBombarded(false);
	// Super Forts end

	doFeature();

	doCulture();

	verifyUnitValidPlot();
/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	doTerritoryClaiming();

	if (getOwner() != NO_PLAYER)
	{
		if (!GET_PLAYER(getOwner()).isAlive())
		{
			setOwner(NO_PLAYER, false, false);
		}
	}

/*	doImprovementSpawn();

void CvPlot::doImprovementSpawn()
{
	if (getImprovementType() != NO_IMPROVEMENT)
	{
		if (getOwner() == NO_PLAYER)
		{
			changeImprovementDecay(-1);
			if (getImprovementDecay() == 0)
			{
				if (GC.getImprovementInfo(getImprovementType()).getImprovementPillage() != NO_IMPROVEMENT)
				{
					int iTimer = 10;
					changeImprovementDecay(iTimer);
					setImprovementType((ImprovementTypes)GC.getImprovementInfo(getImprovementType()).getImprovementPillage());
				}
				else if (getImprovementType() != (ImprovementTypes)(GC.getDefineINT("RUINS_IMPROVEMENT")))
				{
					int iTimer = 10;
					changeImprovementDecay(iTimer);
					setImprovementType((ImprovementTypes)(GC.getDefineINT("RUINS_IMPROVEMENT")));
				}
				else
				{
					setImprovementType(NO_IMPROVEMENT);
				}
			}
			return;
		}
	}
	if (getOwner() != NO_PLAYER)
	{
		int iBestValue = 0;
		ImprovementTypes eBestImprovement = NO_IMPROVEMENT;
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			int iValue = GET_PLAYER(getOwner()).getImprovementValue();
			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestImprovement = (ImprovementTypes)iI;
			}
		}
		if (eBestImprovement != NO_IMPROVEMENT)
		{
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	// XXX
#ifdef _DEBUG
	{
		foreach_ (CvUnit* unit, units())
		{
			FAssertMsg(unit->atPlot(this), "pLoopUnit is expected to be at the current plot instance");
			if (!unit->atPlot(this))
			{
				removeUnit(unit);
			}
		}
	}
#endif
	// XXX
}


void CvPlot::doImprovement()
{
	PROFILE_FUNC();

	ImprovementTypes eType = getImprovementType();
	CvImprovementInfo& pInfo = GC.getImprovementInfo(eType);

	if (isBeingWorked() || pInfo.isUpgradeRequiresFortify())
	{
		doImprovementUpgrade();
		if (eType != getImprovementType())
		{ // We won't do bonus discovery/depletion if it just upgraded into a new improvement.
			return;
		}
	}
	// Discover bonus
	if (getBonusType() == NO_BONUS)
	{
		for (int iI = 0; iI < GC.getNumBonusInfos(); ++iI)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes) iI).getTechReveal())))
			{
				if (!canHaveBonus((BonusTypes)iI))
				{
					continue;
				}
				int iOdds = pInfo.getImprovementBonusDiscoverRand(iI);

				if (iOdds > 0)
				{
					iOdds *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getConstructPercent();
					iOdds /= 100;

					if( GC.getGame().getSorenRandNum(iOdds, "Bonus Discovery") == 0)
					{
						setBonusType((BonusTypes)iI);

						CvCity* pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);

						if (pCity != NULL && isInViewport())
						{
							MEMORY_TRACK_EXEMPT();

							CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_DISCOVERED_NEW_RESOURCE", GC.getBonusInfo((BonusTypes) iI).getTextKeyWide(), pCity->getNameKey());
							AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DISCOVERBONUS", MESSAGE_TYPE_MINOR_EVENT, GC.getBonusInfo((BonusTypes) iI).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getViewportX(), getViewportY(), true, true);
						}
						break;
					}
				}
			}
		}
	}
	// Deplete Bonus
	else if (GC.getGame().isModderGameOption(MODDERGAMEOPTION_RESOURCE_DEPLETION))
	{
		BonusTypes eBonus = getNonObsoleteBonusType(getTeam());
		PlayerTypes ePlayer = getOwner();

		// We know it's owned by a player because this function is only called if it is.
		if (eBonus == NO_BONUS || !pInfo.isImprovementBonusTrade(eBonus) || !GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo(eBonus).getTechCityTrade())))
		{
			return;
		}

		int iBonusOdds = pInfo.getImprovementBonusDepletionRand(eBonus);
		iBonusOdds *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getConstructPercent();
		iBonusOdds /= 100;

		iBonusOdds *= 12 * ((int)GC.getMap().getWorldSize() + 1);
		iBonusOdds /= GC.getMap().getNumBonuses(eBonus);

		if (GET_PLAYER(ePlayer).getResourceConsumption(eBonus) > 0)
		{
			iBonusOdds *= (100 * GET_PLAYER(ePlayer).getNumCities());
			iBonusOdds /= GET_PLAYER(ePlayer).getResourceConsumption(eBonus);
		}

		if (iBonusOdds > 0 && GC.getGame().getSorenRandNum(iBonusOdds, "Bonus Depletion") == 0)
		{
			{
				MEMORY_TRACK_EXEMPT();

				CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_RESOURCE_DEPLETED", GC.getBonusInfo(eBonus).getTextKeyWide(), pInfo.getDescription());
				AddDLLMessage(ePlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, GC.getBonusInfo(eBonus).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX(), getY(), true, true);
			}
			GC.getGame().logMsg("Resource Depleted! Resource was %d, The odds were 1 in %d", eBonus, iBonusOdds);

			setBonusType(NO_BONUS);

			CvCity* pCity = GC.getMap().findCity(getX(), getY(), ePlayer, NO_TEAM, false);

			if (pCity != NULL) { pCity->AI_setAssignWorkDirty(true); }

			if (!canHaveImprovement(eType, getTeam()))
			{
				setImprovementType(NO_IMPROVEMENT);
				return;
			}
		}
	}
}

void CvPlot::doImprovementUpgrade()
{
	if (getImprovementType() != NO_IMPROVEMENT  && !isPlotIgnoringImprovementUpgrade())
	{
/************************************************************************************************/
/* Afforess	                  Start		 05/23/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
		ImprovementTypes eImprovementUpdrade = (ImprovementTypes)GC.getImprovementInfo(getImprovementType()).getImprovementUpgrade();*/
		ImprovementTypes eImprovement = getImprovementType();
		ImprovementTypes eImprovementUpdrade = GET_TEAM(getTeam()).getImprovementUpgrade(getImprovementType());
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		if (eImprovementUpdrade != NO_IMPROVEMENT && (GC.getImprovementInfo(eImprovementUpdrade).getPrereqTech() == NO_TECH || (GC.getImprovementInfo(eImprovementUpdrade).getPrereqTech() != NO_TECH && GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getImprovementInfo(eImprovementUpdrade).getPrereqTech()))))
		{
			if (GC.getImprovementInfo(eImprovementUpdrade).getHighestCost() < GET_PLAYER(getOwner()).getEffectiveGold())
			{
				// Super Forts begin *upgrade* - added if-else statement
				if(GC.getImprovementInfo(getImprovementType()).isUpgradeRequiresFortify())
				{
					bool bDefenderFound = false;
					CLinkList<IDInfo> oldUnits;
					CLLNode<IDInfo>* pUnitNode = headUnitNode();
					CvUnit* pLoopUnit;


					while (pUnitNode != NULL)
					{
						oldUnits.insertAtEnd(pUnitNode->m_data);
						pUnitNode = nextUnitNode(pUnitNode);
					}

					pUnitNode = oldUnits.head();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = nextUnitNode(pUnitNode);
						if(pLoopUnit->getFortifyTurns() > 0)
						{
							if (pLoopUnit->getTeam() == getTeam() && pLoopUnit->canDefend())
							{
								bDefenderFound = true;
								break;
							}
						}
					}

					if(bDefenderFound)
					{
						changeUpgradeProgressHundredths(GET_PLAYER(getOwner()).getImprovementUpgradeRateTimes100(getImprovementType()));

						if (getUpgradeProgressHundredths() >= 100*GC.getGame().getImprovementUpgradeTime(getImprovementType()))
						{
							//here: new function - setImprovementUpgrade
							setImprovementUpgrade();
							//setImprovementType(eImprovementUpdrade);
						}
					}
				}
				else
				{
					if (isBeingWorked())
					{
						changeUpgradeProgressHundredths(GET_PLAYER(getOwner()).getImprovementUpgradeRateTimes100(getImprovementType()));

						if (getUpgradeProgressHundredths() >= 100*GC.getGame().getImprovementUpgradeTime(getImprovementType()))
						{
							//here: new function - setImprovementUpgrade
							setImprovementUpgrade();
							//setImprovementType(eImprovementUpdrade);
						}
					}
				} // Super Forts end
			}
		}
	}
}

void CvPlot::updateCulture(bool bBumpUnits, bool bUpdatePlotGroups)
{
	//	Koshling - C2C code left largely unchanged on Super Forts merge as it already worked essentially the same way here
	if (!isCity() && (!isActsAsCity() || getUnitPower(getOwner()) == 0))
	{
		PlayerTypes eNewOwner = calculateCulturalOwner();

		if ( eNewOwner != NO_PLAYER || !isActsAsCity() )
		{
			setOwner(eNewOwner, bBumpUnits, bUpdatePlotGroups);
		}
	}
}


void CvPlot::updateFog()
{
	//MEMORY_TRACE_FUNCTION();
	PROFILE_FUNC();

	if ( !shouldHaveGraphics() )
	{
		return;
	}

	FAssert(GC.getGame().getActiveTeam() != NO_TEAM);

	if (isRevealed(GC.getGame().getActiveTeam(), false))
	{
		if (gDLL->getInterfaceIFace()->isBareMapMode())
		{
			gDLL->getEngineIFace()->LightenVisibility(getFOWIndex());
		}
		else
		{
			int cityScreenFogEnabled = GC.getDefineINT("CITY_SCREEN_FOG_ENABLED");
			if (cityScreenFogEnabled && gDLL->getInterfaceIFace()->isCityScreenUp() && (gDLL->getInterfaceIFace()->getHeadSelectedCity() != getWorkingCity()))
			{
				gDLL->getEngineIFace()->DarkenVisibility(getFOWIndex());
			}
			else if (isActiveVisible(false))
			{
				gDLL->getEngineIFace()->LightenVisibility(getFOWIndex());
			}
			else
			{
				gDLL->getEngineIFace()->DarkenVisibility(getFOWIndex());
			}
		}
	}
	else
	{
		gDLL->getEngineIFace()->BlackenVisibility(getFOWIndex());
	}
}


void CvPlot::updateVisibility()
{
	//MEMORY_TRACE_FUNCTION();
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


void CvPlot::updateSymbolDisplay()
{
	PROFILE_FUNC();

	CvSymbol* pLoopSymbol;
	int iLoop;

	if ( !isGraphicsVisible(ECvPlotGraphics::SYMBOLS))
	{
		return;
	}

	for (iLoop = 0; iLoop < getNumSymbols(); iLoop++)
	{
		pLoopSymbol = getSymbol(iLoop);

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

	CvSymbol* pLoopSymbol;
	int iLoop;

	if ( !isGraphicsVisible(ECvPlotGraphics::SYMBOLS) )
	{
		return;
	}

	if ( updateSymbolsInternal() )
	{
		updateSymbolDisplay();

		for (iLoop = 0; iLoop < getNumSymbols(); iLoop++)
		{
			pLoopSymbol = getSymbol(iLoop);

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
	MEMORY_TRACK_EXEMPT();
	PROFILE_FUNC();

	if (!isGraphicsVisible(ECvPlotGraphics::SYMBOLS))
	{
		return false;
	}
	deleteAllSymbols();

	if (isRevealed(GC.getGame().getActiveTeam(), true) && (isShowCitySymbols() || (gDLL->getInterfaceIFace()->isShowYields() && !(gDLL->getInterfaceIFace()->isCityScreenUp()))))
	{
		int yieldAmounts[NUM_YIELD_TYPES];
		int maxYield = 0;
		int iYield = 0;
		for (int iYieldType = 0; iYieldType < NUM_YIELD_TYPES; iYieldType++)
		{
			iYield = calculateYield(((YieldTypes)iYieldType), true);
			yieldAmounts[iYieldType] = iYield;
			if (iYield > maxYield)
			{
				maxYield = iYield;
			}
		}

		if (maxYield)
		{
			int maxYieldStack = GC.getDefineINT("MAX_YIELD_STACK");

			CvSymbol *pSymbol5x = addSymbol();
			CvSymbol *pSymbol= addSymbol();


			for (int iYieldType = 0; iYieldType < NUM_YIELD_TYPES; iYieldType++)
			{
				iYield = yieldAmounts[iYieldType];

				if (iYield)
				{
					if (iYield > 29)
					{
						gDLL->getSymbolIFace()->setTypeYield(pSymbol5x, iYieldType, maxYieldStack-1);
						gDLL->getSymbolIFace()->setTypeYield(pSymbol, iYieldType, 4);
					}
					else if (iYield > 4)
					{
						int iYieldBig = iYield / 5;
						iYield -= 5 * iYieldBig;
						gDLL->getSymbolIFace()->setTypeYield(pSymbol5x, iYieldType, iYieldBig + 4);

						if (iYield)
						{
							gDLL->getSymbolIFace()->setTypeYield(pSymbol, iYieldType, iYield);
						}
						else
						{
							gDLL->getSymbolIFace()->setTypeYield(pSymbol, iYieldType, maxYieldStack);
						}
					}
					else
					{
						gDLL->getSymbolIFace()->setTypeYield(pSymbol5x, iYieldType, maxYieldStack);
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
	else { return false; }
}


void CvPlot::updateMinimapColor()
{
	PROFILE_FUNC();

	if ( !shouldHaveGraphics() )
	{
		return;
	}

	gDLL->getInterfaceIFace()->setMinimapColor(MINIMAPMODE_TERRITORY, getViewportX(),getViewportY(), plotMinimapColor(), STANDARD_MINIMAP_ALPHA);
}

bool CvPlot::unitHere(const CvUnit* pUnit) const
{
	return algo::contains(units(), pUnit);
}

CvUnit* CvPlot::getPreferredCenterUnit() const
{
	CvUnit*  pNewCenterUnit = getSelectedUnit();

	FAssertMsg(pNewCenterUnit == NULL || pNewCenterUnit->atPlot(this), "Selected unit isn't in the plot that owns it");
	if (pNewCenterUnit != NULL)
	{
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(GC.getGame().getActivePlayer(), NO_PLAYER, NULL, false, false, true);
	if (pNewCenterUnit != NULL)
	{
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(GC.getGame().getActivePlayer());
	if (pNewCenterUnit != NULL)
	{
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(NO_PLAYER, GC.getGame().getActivePlayer(), gDLL->getInterfaceIFace()->getHeadSelectedUnit(), true);
	if (pNewCenterUnit != NULL)
	{
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(NO_PLAYER, GC.getGame().getActivePlayer(), gDLL->getInterfaceIFace()->getHeadSelectedUnit());
	if (pNewCenterUnit != NULL)
	{
		return pNewCenterUnit;
	}
	pNewCenterUnit = getBestDefender(NO_PLAYER, GC.getGame().getActivePlayer());
	return pNewCenterUnit;
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
		//	This makes the interim safe state, and the correct value will be calculated once the
		//	inhibitted section is exited
		m_pCenterUnit = NULL;
		return;
	}

	CvUnit* pOldCenterUnit = getCenterUnit();
	CvUnit* pNewCenterUnit = NULL;

	if(isActiveVisible(true) /*&& isGraphicsVisible(ECvPlotGraphics::UNIT)*/)
	{
		pNewCenterUnit = getPreferredCenterUnit();
	}

	if (pNewCenterUnit != pOldCenterUnit)
	{
#if 0
		OutputDebugString(CvString::format("Center unit change for plot (%d,%d) from %08lx to %08lx\n", m_iX, m_iY, pOldCenterUnit, pNewCenterUnit).c_str());
		if ( pOldCenterUnit != NULL )
		{
			//	Does the old center unit still exist?
			if ( unitHere(pOldCenterUnit) )
			{
				pOldCenterUnit->reloadEntity();
			}
			else
			{
				OutputDebugString("Old center unit no longer here\n");
			}
		}
#endif

		//if (pOldCenterUnit != NULL)
		//{
		//	pOldCenterUnit->reloadEntity(true);
		//}

		if (pNewCenterUnit != NULL)
		{
			pNewCenterUnit->reloadEntity(true);
		}
		setCenterUnit(pNewCenterUnit);
	}
	else
	{
		setCenterUnit(pOldCenterUnit);
	}
}


void CvPlot::verifyUnitValidPlot()
{
	PROFILE_FUNC();

	bool bAnyMoved = false;
	std::vector<CvUnit*> aUnits;

	for (unit_iterator unitItr = beginUnits(); unitItr != endUnits(); ++unitItr)
	{
		aUnits.push_back(*unitItr);
	}

	for (std::vector<CvUnit*>::iterator it = aUnits.begin(); it != aUnits.end(); )
	{
		CvUnit* pLoopUnit = *it;
		bool bErased = false;

		if (pLoopUnit->atPlot(this) &&
			!(pLoopUnit->isCargo()) &&
			!(pLoopUnit->isCombat()) &&
			(!isValidDomainForLocation(*pLoopUnit) || !pLoopUnit->canEnterArea(getTeam(), area()))
			)
		{
			bAnyMoved = true;
			if (!pLoopUnit->jumpToNearestValidPlot())
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

	//TB: Disabled due to other game rules making this at times an incompatible check with logical game flow.
	//if (isOwned())
	//{
	//	it = aUnits.begin();
	//	while (it != aUnits.end())
	//	{
	//		CvUnit* pLoopUnit = *it;
	//		bool bErased = false;

	//		if (pLoopUnit != NULL)
	//		{
	//			if (pLoopUnit->atPlot(this))
	//			{
	//				if (!(pLoopUnit->isCombat()))
	//				{
	//					//if (pLoopUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam())))
	//					if (getTeam() != NO_TEAM &&
	//						GET_PLAYER(pLoopUnit->getCombatOwner(getTeam(), this)).getTeam() != BARBARIAN_TEAM &&
	//						pLoopUnit->getTeam() != getTeam() &&
	//						!GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam()))
	//					{
	//						if (isVisibleEnemyUnit(pLoopUnit))
	//						{
	//							if (!(pLoopUnit->isInvisible(getTeam(), false)))
	//							{
	//								if (!(isCity(true) && pLoopUnit->isBlendIntoCity()))
	//								{
	//									bAnyMoved = true;
	//									if (!pLoopUnit->jumpToNearestValidPlot())
	//									{
	//										bErased = true;
	//									}
	//								}
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}

	//		if (bErased)
	//		{
	//			it = aUnits.erase(it);
	//		}
	//		else
	//		{
	//			++it;
	//		}
	//	}
	//}

	//	Check groups are not broken by only some of their constituent units
	//	having been moved
	if ( bAnyMoved )
	{
		for (std::vector<CvUnit*>::iterator it = aUnits.begin(); it != aUnits.end(); ++it)
		{
			CvUnit* pLoopUnit = *it;
			CvSelectionGroup* pGroup = pLoopUnit->getGroup();

			if ( pGroup != NULL && !pGroup->isMidMove() )
			{
				CvUnit*	pHeadUnit = pGroup->getHeadUnit();

				if ( pHeadUnit != NULL && pHeadUnit->plot() != pLoopUnit->plot() )
				{
					pLoopUnit->joinGroup(NULL);
				}
			}
		}
	}
}


void CvPlot::nukeExplosion(int iRange, CvUnit* pNukeUnit)
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pLoopCity;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	CLinkList<IDInfo> oldUnits;
	CvWString szBuffer;
	int iNukeDamage;
	int iNukedPopulation;
	int iDX, iDY;
	int iI;

	GC.getGame().changeNukesExploded(1);

	for (iDX = -(iRange); iDX <= iRange; iDX++)
	{
		for (iDY = -(iRange); iDY <= iRange; iDY++)
		{
			pLoopPlot	= plotXY(getX(), getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				// if we remove roads, don't remove them on the city... XXX

				pLoopCity = pLoopPlot->getPlotCity();

				if (pLoopCity == NULL)
				{
					if (!(pLoopPlot->isWater()) && !(pLoopPlot->isImpassable()))
					{
						if (NO_FEATURE == pLoopPlot->getFeatureType() || !GC.getFeatureInfo(pLoopPlot->getFeatureType()).isNukeImmune())
						{
							if (GC.getGame().getSorenRandNum(100, "Nuke Fallout") < GC.getDefineINT("NUKE_FALLOUT_PROB"))
							{
								pLoopPlot->setImprovementType(NO_IMPROVEMENT);
								pLoopPlot->setFeatureType((FeatureTypes)(GC.getDefineINT("NUKE_FEATURE")));
							}
						}
					}
				}

				oldUnits.clear();

				pUnitNode = pLoopPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					oldUnits.insertAtEnd(pUnitNode->m_data);
					pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);
				}

				pUnitNode = oldUnits.head();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = oldUnits.next(pUnitNode);

					if (pLoopUnit != NULL)
					{
						// < M.A.D. Nukes Start >
						if (pLoopUnit != pNukeUnit && !pLoopUnit->isMADEnabled())
						//if (pLoopUnit != pNukeUnit)
						// < M.A.D. Nukes End   >
						{
							if (!pLoopUnit->isNukeImmune() && !pLoopUnit->isDelayedDeath())
							{
								iNukeDamage = (GC.getDefineINT("NUKE_UNIT_DAMAGE_BASE") + GC.getGame().getSorenRandNum(GC.getDefineINT("NUKE_UNIT_DAMAGE_RAND_1"), "Nuke Damage 1") + GC.getGame().getSorenRandNum(GC.getDefineINT("NUKE_UNIT_DAMAGE_RAND_2"), "Nuke Damage 2"));

								if (pLoopCity != NULL)
								{
									iNukeDamage *= std::max(0, (pLoopCity->getNukeModifier() + 100));
									iNukeDamage /= 100;
								}

								if (pLoopUnit->canFight() || pLoopUnit->airBaseCombatStr() > 0)
								{
									pLoopUnit->changeDamage(iNukeDamage, ((pNukeUnit != NULL) ? pNukeUnit->getOwner() : NO_PLAYER));
								}
								else if (iNukeDamage >= GC.getDefineINT("NUKE_NON_COMBAT_DEATH_THRESHOLD"))
								{
									pLoopUnit->kill(true, ((pNukeUnit != NULL) ? pNukeUnit->getOwner() : NO_PLAYER));
								}
							}
						}
					}
				}

				if (pLoopCity != NULL)
				{
					for (iI = 0; iI < GC.getNumBuildingInfos(); ++iI)
					{
						if (pLoopCity->getNumRealBuilding((BuildingTypes)iI) > 0)
						{
							if (!(GC.getBuildingInfo((BuildingTypes) iI).isNukeImmune()))
							{
								if (GC.getGame().getSorenRandNum(100, "Building Nuked") < GC.getDefineINT("NUKE_BUILDING_DESTRUCTION_PROB"))
								{
									pLoopCity->setNumRealBuilding(((BuildingTypes)iI), pLoopCity->getNumRealBuilding((BuildingTypes)iI) - 1);
								}
							}
						}
					}

					iNukedPopulation = ((pLoopCity->getPopulation() * (GC.getDefineINT("NUKE_POPULATION_DEATH_BASE") + GC.getGame().getSorenRandNum(GC.getDefineINT("NUKE_POPULATION_DEATH_RAND_1"), "Population Nuked 1") + GC.getGame().getSorenRandNum(GC.getDefineINT("NUKE_POPULATION_DEATH_RAND_2"), "Population Nuked 2"))) / 100);

					iNukedPopulation *= std::max(0, (pLoopCity->getNukeModifier() + 100));
					iNukedPopulation /= 100;

					pLoopCity->changePopulation(-(std::min((pLoopCity->getPopulation() - 1), iNukedPopulation)));
				}
			}
		}
	}

	CvEventReporter::getInstance().nukeExplosion(this, pNukeUnit);
}


bool CvPlot::isConnectedTo(const CvCity* pCity) const
{
	// Super Forts begin *AI_worker* (had to remove the assert and replace it with an if-else)
	//FAssert(isOwned());
	if(isOwned())
	{
		return ((getPlotGroup(getOwner()) == pCity->plotGroup(getOwner())) || (getPlotGroup(pCity->getOwner()) == pCity->plotGroup(pCity->getOwner())));
	}
	else
	{
		return false;
	}
	// Super Forts end
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

		if (pCapitalCity != NULL)
		{
			return isConnectedTo(pCapitalCity);
		}
	}

	return false;
}


int CvPlot::getPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	FAssertMsg(ePlayer != NO_PLAYER, "Player is not assigned a valid value");
	FAssertMsg(eBonus != NO_BONUS, "Bonus is not assigned a valid value");

	const CvPlotGroup* pPlotGroup = getPlotGroup(ePlayer);

	if (pPlotGroup != NULL)
	{
		return pPlotGroup->getNumBonuses(eBonus);
	}
	else
	{
		return 0;
	}
}


bool CvPlot::isPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	return (getPlotGroupConnectedBonus(ePlayer, eBonus) > 0);
}


bool CvPlot::isAdjacentPlotGroupConnectedBonus(PlayerTypes ePlayer, BonusTypes eBonus) const
{
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		const CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isPlotGroupConnectedBonus(ePlayer, eBonus))
			{
				return true;
			}
		}
	}

	return false;
}


void CvPlot::updatePlotGroupBonus(bool bAdd)
{
	PROFILE_FUNC();

	CvCity* pPlotCity;
	CvPlotGroup* pPlotGroup;
	BonusTypes eNonObsoleteBonus;
	int iI;

	if (!isOwned())
	{
		return;
	}

	pPlotGroup = getPlotGroup(getOwner());

	if (pPlotGroup != NULL)
	{
		pPlotCity = getPlotCity();

		if (pPlotCity != NULL)
		{
			PROFILE("CvPlot::updatePlotGroupBonus.PlotCity");

			for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
			{
				if (!GET_TEAM(getTeam()).isBonusObsolete((BonusTypes)iI))
				{
					pPlotGroup->changeNumBonuses(((BonusTypes)iI), (pPlotCity->getFreeBonus((BonusTypes)iI) * ((bAdd) ? 1 : -1)));
				}
			}

			if (pPlotCity->isCapital())
			{
				PROFILE("CvPlot::updatePlotGroupBonus.Capital");

				for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					pPlotGroup->changeNumBonuses(((BonusTypes)iI), (GET_PLAYER(getOwner()).getBonusExport((BonusTypes)iI) * ((bAdd) ? -1 : 1)));
					pPlotGroup->changeNumBonuses(((BonusTypes)iI), (GET_PLAYER(getOwner()).getBonusImport((BonusTypes)iI) * ((bAdd) ? 1 : -1)));
				}
			}
		}

		eNonObsoleteBonus = getNonObsoleteBonusType(getTeam());

		if (eNonObsoleteBonus != NO_BONUS)
		{
			PROFILE("CvPlot::updatePlotGroupBonus.NonObsoletePlotBonus");

			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo(eNonObsoleteBonus).getTechCityTrade())))
			{
// Super forts C2C adaptation
				//	Now that improvements are separately tagged as to whether they are universal resource providers
				//	the 'fort' check here is not needed
				if (isCity(false, getTeam()) ||
// Super forts C2C adaptation end
					((getImprovementType() != NO_IMPROVEMENT) && GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus)))
				{
					if ((pPlotGroup != NULL) && isBonusNetwork(getTeam()))
					{
						pPlotGroup->changeNumBonuses(eNonObsoleteBonus, ((bAdd) ? 1 : -1));
					}
				}
			}
		}
	}
}


bool CvPlot::isAdjacentToArea(int iAreaID) const
{
	PROFILE_FUNC();

	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getArea() == iAreaID)
			{
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::isAdjacentToArea(const CvArea* pArea) const
{
	return isAdjacentToArea(pArea->getID());
}


bool CvPlot::shareAdjacentArea(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	int iCurrArea;
	int iLastArea;
	CvPlot* pAdjacentPlot;
	int iI;

	iLastArea = FFreeList::INVALID_INDEX;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			iCurrArea = pAdjacentPlot->getArea();

			if (iCurrArea != iLastArea)
			{
				if (pPlot->isAdjacentToArea(iCurrArea))
				{
					return true;
				}

				iLastArea = iCurrArea;
			}
		}
	}

	return false;
}


bool CvPlot::isAdjacentToLand() const
{
	PROFILE_FUNC();

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL && !pAdjacentPlot->isWater())
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::isCoastalLand(int iMinWaterSize) const
{
	PROFILE_FUNC();

	if (isWater())
	{
		return false;
	}

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isWater())
			{
				if (pAdjacentPlot->area()->getNumTiles() >= iMinWaterSize)
				{
					return true;
				}
			}
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
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
			{
				if ((eIgnorePlayer == NO_PLAYER) || (((PlayerTypes)iI) != eIgnorePlayer))
				{
					if (isPlayerCityRadius((PlayerTypes)iI))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvPlot::isLake() const
{
	const CvArea* pArea = area();
	return pArea ? pArea->isLake() : false;
}


// XXX if this changes need to call updateIrrigated() and pCity->updateFreshWaterHealth()
// XXX precalculate this???
/************************************************************************************************/
/* Afforess	                  Start		 07/22/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
bool CvPlot::isFreshWater() const
*/
bool CvPlot::isFreshWater(bool bIgnoreJungle) const
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
{
	CvCity* pCity = getPlotCity();
	if (pCity != NULL && pCity->hasFreshWater())
	{
		return true;
	}

	if (isLake())
	{
		return true;
	}

	TeamTypes eTeam = getTeam();

	if (isWater() || isImpassable(eTeam))
	{
		return false;
	}

	if (isRiver())
	{
		return true;
	}

	for (int iDX = -1; iDX <= 1; iDX++)
	{
		for (int iDY = -1; iDY <= 1; iDY++)
		{
			CvPlot* pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isLake())
				{
					return true;
				}

				if (pLoopPlot->getFeatureType() != NO_FEATURE)
				{
					if (GC.getFeatureInfo(pLoopPlot->getFeatureType()).isAddsFreshWater() && (!bIgnoreJungle || GC.getFeatureInfo(pLoopPlot->getFeatureType()).getHealthPercent() >= 0))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvPlot::isPotentialIrrigation() const
{
//===NM=====Mountain Mod===0X=====
	//TB Debug: Why should it be necessary for cities to require Not being on hills or alternative peak types (like volcanoes) for them to be potentially irrigated?  Seems to be a strange requirement for wells to function.
	if ((isCity() /*&& !(isHills() || isPeak2(true))*/) || ((getImprovementType() != NO_IMPROVEMENT) && (GC.getImprovementInfo(getImprovementType()).isCarriesIrrigation())))
	{
		if ((getTeam() != NO_TEAM) && GET_TEAM(getTeam()).isIrrigation())
		{
			return true;
		}
	}

	return false;
}


bool CvPlot::canHavePotentialIrrigation() const
{
	int iI;

//===NM=====Mountain Mod===0X=====
	//TB Debug: Why should it be necessary for cities to require Not being on either hills or alternative peak types (like volcanoes) for them to be potentially irrigated?  Seems to be a strange requirement for wells to function.
	if (isCity() /*&& !(isHills() || isPeak2(true))*/)
	{
		return true;
	}

	for (iI = 0; iI < GC.getNumImprovementInfos(); ++iI)
	{
		if (GC.getImprovementInfo((ImprovementTypes)iI).isCarriesIrrigation())
		{
			if (canHaveImprovement(((ImprovementTypes)iI), NO_TEAM, true))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isIrrigationAvailable(bool bIgnoreSelf) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	if (!bIgnoreSelf && isIrrigated())
	{
		return true;
	}

	if (isFreshWater())
	{
		return true;
	}

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isIrrigated())
			{
				return true;
			}
		}
	}

	return false;
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
		FAssert(false);
		break;
	}

	return false;
}


bool CvPlot::isRiverSide() const
{
	CvPlot* pLoopPlot;
	int iI;

	for (iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
	{
		pLoopPlot = plotCardinalDirection(getX(), getY(), ((CardinalDirectionTypes)iI));

		if (pLoopPlot != NULL)
		{
			if (isRiverCrossing(directionXY(this, pLoopPlot)))
			{
				return true;
			}
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
		FAssert(false);
		break;
	}

	return false;
}


CvPlot* CvPlot::getNearestLandPlotInternal(int iDistance) const
{
	if (iDistance > GC.getMap().getGridHeight() && iDistance > GC.getMap().getGridWidth())
	{
		return NULL;
	}

	for (int iDX = -iDistance; iDX <= iDistance; iDX++)
	{
		for (int iDY = -iDistance; iDY <= iDistance; iDY++)
		{
			if (abs(iDX) + abs(iDY) == iDistance)
			{
				CvPlot* pPlot = plotXY(getX(), getY(), iDX, iDY);
				if (pPlot != NULL)
				{
					if (!pPlot->isWater())
					{
						return pPlot;
					}
				}
			}
		}
	}
	return getNearestLandPlotInternal(iDistance + 1);
}


int CvPlot::getNearestLandArea() const
{
	CvPlot* pPlot = getNearestLandPlot();
	return pPlot ? pPlot->getArea() : -1;
}


CvPlot* CvPlot::getNearestLandPlot() const
{
	return getNearestLandPlotInternal(0);
}


int CvPlot::seeFromLevel(TeamTypes eTeam) const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	int iLevel = GC.getTerrainInfo(getTerrainType()).getSeeFromLevel();

	if (getImprovementType() != NO_IMPROVEMENT)
	{
		iLevel += GC.getImprovementInfo(getImprovementType()).getSeeFrom();
	}

	if (isPeak2(true))
	{
		iLevel += GC.getPEAK_SEE_FROM_CHANGE();
	}
	else if (isHills())
	{
		iLevel += GC.getHILLS_SEE_FROM_CHANGE();
	}
	else if (isWater())
	{
		iLevel += GC.getSEAWATER_SEE_FROM_CHANGE();

		if (GET_TEAM(eTeam).isExtraWaterSeeFrom())
		{
			iLevel++;
		}
	}
	return iLevel;
}


int CvPlot::seeThroughLevel() const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	int iLevel = GC.getTerrainInfo(getTerrainType()).getSeeThroughLevel();

	if (getFeatureType() != NO_FEATURE)
	{
		iLevel += GC.getFeatureInfo(getFeatureType()).getSeeThroughChange();
	}

	if (isPeak2(true))
	{
		iLevel += GC.getPEAK_SEE_THROUGH_CHANGE();
	}
	else if (isHills())
	{
		iLevel += GC.getHILLS_SEE_THROUGH_CHANGE();
	}
	else if (isWater())
	{
		iLevel += GC.getSEAWATER_SEE_FROM_CHANGE();
	}
	return iLevel;
}



void CvPlot::changeAdjacentSight(TeamTypes eTeam, int iRange, bool bIncrement, CvUnit* pUnit, bool bUpdatePlotGroups)
{
	bool bAerial = (pUnit != NULL && pUnit->getDomainType() == DOMAIN_AIR);
	int iUnitID = 0;

	DirectionTypes eFacingDirection = NO_DIRECTION;
	if (!bAerial && NULL != pUnit)
	{
		eFacingDirection = pUnit->getFacingDirection(true);
	}

	//fill invisible types
	std::vector<InvisibleTypes> aSeeInvisibleTypes;
	if (NULL != pUnit)
	{
		iUnitID = pUnit->getID();
		if (!GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
		{
			for(int i=0;i<pUnit->getNumSeeInvisibleTypes();i++)
			{
				aSeeInvisibleTypes.push_back(pUnit->getSeeInvisibleType(i));
			}
		}
		else
		{
			for(int i=0; i < GC.getNumInvisibleInfos(); i++)
			{
				if (pUnit->hasVisibilityType((InvisibleTypes)i))
				{
					aSeeInvisibleTypes.push_back((InvisibleTypes)i);
				}
			}
		}
	}

	if(aSeeInvisibleTypes.empty() || GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
	{
		aSeeInvisibleTypes.push_back(NO_INVISIBLE);
	}

	//check one extra outer ring
	if (!bAerial)
	{
		iRange++;
	}

	int iDistance = 0;
	int iIntensity = 0;
	int iFinalIntensity = 0;
	int iIncrement = 0;
	int iAdjustment = 0;
	int ix = 0;
	int iy = 0;
	bool bSameTile = false;
	for(int i=0;i<(int)aSeeInvisibleTypes.size();i++)
	{
		for (int dx = -iRange; dx <= iRange; dx++)
		{
			for (int dy = -iRange; dy <= iRange; dy++)
			{
				if (GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK) && pUnit != NULL)
				{
					ix = dx;
					iy = dy;
					if (ix < 0)
					{
						ix *= -1;
					}
					if (iy < 0)
					{
						iy *= -1;
					}
					iDistance = std::max(ix,iy);
					bSameTile = (iDistance == 0);
					if (aSeeInvisibleTypes[i] != NO_INVISIBLE)
					{
						iIntensity = pUnit->visibilityIntensityTotal(aSeeInvisibleTypes[i]);
						iDistance -= std::max(0, pUnit->visibilityIntensityRangeTotal(aSeeInvisibleTypes[i]));
						iDistance = std::max(0, (iDistance -1));
						iIncrement = 1 + (-1 * std::min(0, pUnit->visibilityIntensityRangeTotal(aSeeInvisibleTypes[i])));
						iAdjustment = iIncrement * std::max(0, iDistance);
						iIntensity = std::max(0, (iIntensity - iAdjustment));
						if (bSameTile)
						{
							iIntensity += pUnit->visibilityIntensitySameTileTotal(aSeeInvisibleTypes[i]);
						}
						iFinalIntensity = std::max(0, iIntensity);
					}
					else
					{
						iFinalIntensity = 0;
					}
				}
				//check if in facing direction
				if (bAerial || shouldProcessDisplacementPlot(dx, dy, iRange - 1, eFacingDirection))
				{
					bool outerRing = false;
					if ((abs(dx) == iRange) || (abs(dy) == iRange))
					{
						outerRing = true;
					}

					//check if anything blocking the plot
					if (bAerial || canSeeDisplacementPlot(eTeam, dx, dy, dx, dy, true, outerRing))
					{
						CvPlot* pPlot = plotXY(getX(), getY(), dx, dy);
						if (NULL != pPlot)
						{
							pPlot->changeVisibilityCount(eTeam, ((bIncrement) ? 1 : -1), aSeeInvisibleTypes[i], bUpdatePlotGroups, iFinalIntensity, iUnitID);
						}
					}
				}
				//TBSITEDEBUG - Probably should remove this whole next section.
				//If it works it's actually likely problematic since it ultimately does nothing but process in then process out without any adjustments to the game state inbetween.
				//if (eFacingDirection != NO_DIRECTION)
				//{
				//	if((abs(dx) <= 1) && (abs(dy) <= 1)) //always reveal adjacent plots when using line of sight
				//	{
				//		CvPlot* pPlot = plotXY(getX(), getY(), dx, dy);
				//		if (NULL != pPlot)
				//		{
				//			pPlot->changeVisibilityCount(eTeam, 1, aSeeInvisibleTypes[i], bUpdatePlotGroups, iFinalIntensity);
				//			pPlot->changeVisibilityCount(eTeam, -1, aSeeInvisibleTypes[i], bUpdatePlotGroups, iFinalIntensity);
				//		}
				//	}
				//}
			}
		}
	}
}

bool CvPlot::canSeePlot(CvPlot *pPlot, TeamTypes eTeam, int iRange, DirectionTypes eFacingDirection) const
{
	iRange++;

	if (pPlot == NULL)
	{
		return false;
	}

	//find displacement
	int dx = pPlot->getX() - getX();
	int dy = pPlot->getY() - getY();
	dx = dxWrap(dx); //world wrap
	dy = dyWrap(dy);

	//check if in facing direction
	if (shouldProcessDisplacementPlot(dx, dy, iRange - 1, eFacingDirection))
	{
		bool outerRing = false;
		if ((abs(dx) == iRange) || (abs(dy) == iRange))
		{
			outerRing = true;
		}

		//check if anything blocking the plot
		if (canSeeDisplacementPlot(eTeam, dx, dy, dx, dy, true, outerRing))
		{
			return true;
		}
	}

	return false;
}

bool CvPlot::canSeeDisplacementPlot(TeamTypes eTeam, int dx, int dy, int originalDX, int originalDY, bool firstPlot, bool outerRing) const
{
	CvPlot *pPlot = plotXY(getX(), getY(), dx, dy);
	if (pPlot == NULL)
		return false;

	// Base case is current plot
	if (dx == 0 && dy == 0)
	{
		return true;
	}

	// Find closest of three points (1, 2, 3) to original line from Start (S) to End (E)
	// The diagonal is computed first as that guarantees a change in position
	// -------------
	// |   | 2 | S |
	// -------------
	// | E | 1 | 3 |
	// -------------
	int displacements[3][2] = { {dx - getSign(dx), dy - getSign(dy)}, {dx - getSign(dx), dy}, {dx, dy - getSign(dy)} };
	int allClosest[3];
	int closest = -1;
	for (int i = 0; i < 3; i++)
	{
		// int tempClosest = abs(displacements[i][0] * originalDX - displacements[i][1] * originalDY); //more accurate, but less structured on a grid
		allClosest[i] = abs(displacements[i][0] * dy - displacements[i][1] * dx); // cross product
		if((closest == -1) || (allClosest[i] < closest))
		{
			closest = allClosest[i];
		}
	}

	// iterate through all minimum plots to see if any of them are passable
	for (int i = 0; i < 3; i++)
	{
		int nextDX = displacements[i][0];
		int nextDY = displacements[i][1];

		// Make sure we change plots
		if((nextDX != dx || nextDY != dy)
			&& allClosest[i] == closest
			&& canSeeDisplacementPlot(eTeam, nextDX, nextDY, originalDX, originalDY, false, false)
			)
		{
			int fromLevel = seeFromLevel(eTeam);
			int throughLevel = pPlot->seeThroughLevel();

			// Check strictly higher level
			if(outerRing)
			{
				CvPlot *passThroughPlot = plotXY(getX(), getY(), nextDX, nextDY);
				if(passThroughPlot)
				{
					int passThroughLevel = passThroughPlot->seeThroughLevel();
					if (fromLevel >= passThroughLevel
						// Either we can see through to it or it is high enough to see from far
						&& (fromLevel > passThroughLevel || pPlot->seeFromLevel(eTeam) > fromLevel))
					{
						return true;
					}
				}
			}
			// we can clearly see this level or
			// we can also see it if it is the first plot that is too tall
			else if (fromLevel >= throughLevel || firstPlot)
			{
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::shouldProcessDisplacementPlot(int dx, int dy, int range, DirectionTypes eFacingDirection) const
{
	if(eFacingDirection == NO_DIRECTION)
	{
		return true;
	}
	else if((dx == 0) && (dy == 0)) //always process this plot
	{
		return true;
	}
	else
	{
		//							N		NE		E		SE			S		SW		W			NW
		int displacements[8][2] = {{0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

		int directionX = displacements[eFacingDirection][0];
		int directionY = displacements[eFacingDirection][1];

		//compute angle off of direction
		int crossProduct = directionX * dy - directionY * dx; //cross product
		int dotProduct = directionX * dx + directionY * dy; //dot product

		float theta = atan2((float) crossProduct, (float) dotProduct);
		float spread = 60 * (float) M_PI / 180;
		if((abs(dx) <= 1) && (abs(dy) <= 1)) //close plots use wider spread
		{
			spread = 90 * (float) M_PI / 180;
		}

		if((theta >= -spread / 2) && (theta <= spread / 2))
		{
			return true;
		}
		else
		{
			return false;
		}

		/*
		DirectionTypes leftDirection = GC.getTurnLeftDirection(eFacingDirection);
		DirectionTypes rightDirection = GC.getTurnRightDirection(eFacingDirection);

		//test which sides of the line equation (cross product)
		int leftSide = displacements[leftDirection][0] * dy - displacements[leftDirection][1] * dx;
		int rightSide = displacements[rightDirection][0] * dy - displacements[rightDirection][1] * dx;
		if((leftSide <= 0) && (rightSide >= 0))
			return true;
		else
			return false;
		*/
	}
}

void CvPlot::updateSight(bool bIncrement, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvCity* pCity;
//	CvCity* pHolyCity;
	CvUnit* pLoopUnit;
	int iLoop;
	int iI;

	pCity = getPlotCity();

	if (pCity != NULL)
	{
		// Religion - Disabled with new Espionage System
/*		for (iI = 0; iI < GC.getNumReligionInfos(); ++iI)
		{
			if (pCity->isHasReligion((ReligionTypes)iI))
			{
				pHolyCity = GC.getGame().getHolyCity((ReligionTypes)iI);

				if (pHolyCity != NULL)
				{
					if (GET_PLAYER(pHolyCity->getOwner()).getStateReligion() == iI)
					{
						changeAdjacentSight(pHolyCity->getTeam(), GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
					}
				}
			}
		}*/

		// Vassal
		for (iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if (GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
			{
				changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
			}
		}

		// EspionageEffect
		for (iI = 0; iI < MAX_PC_TEAMS; ++iI)
		{
			if (pCity->getEspionageVisibility((TeamTypes)iI))
			{
				// Passive Effect: enough EPs gives you visibility into someone's cities
				changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
			}
		}

		/************************************************************************************************/
		/* Afforess	                  Start		 02/03/10                                              */
		/*                                                                                              */
		/* Embassy Allows Players to See Capitals                                                       */
		/************************************************************************************************/
		//Removed 3/7/10 due to bugs
		//fixed 12/5/12 by damgo and reinstated by ls612
		if (pCity->isCapital())
		{
			TeamTypes pTeam = pCity->getTeam();
			for (iI = 0; iI < MAX_PC_TEAMS; ++iI)
			{
				if (GET_TEAM(pTeam).isHasEmbassy((TeamTypes)iI))
				{
					changeAdjacentSight((TeamTypes)iI, GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
				}
			}
		}
		/************************************************************************************************/
		/* Afforess	                     END                                                            */
		/************************************************************************************************/
	}

	// Owned
	if (isOwned())
	{
		changeAdjacentSight(getTeam(), GC.getDefineINT("PLOT_VISIBILITY_RANGE"), bIncrement, NULL, bUpdatePlotGroups);
	}

	pUnitNode = headUnitNode();

	// Unit
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			if (bIncrement)
			{
				pLoopUnit->changeDebugCount(1);
			}
			else
			{
				pLoopUnit->changeDebugCount(-1);
			}

			changeAdjacentSight(pLoopUnit->getTeam(), pLoopUnit->visibilityRange(), bIncrement, pLoopUnit, bUpdatePlotGroups);
		}
		else
		{
			verifyUnitValidPlot();
			pUnitNode = NULL;
		}
	}

	if (getReconCount() > 0)
	{
		int iRange = GC.getDefineINT("RECON_VISIBILITY_RANGE");
		for (iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			for(pLoopUnit = GET_PLAYER((PlayerTypes)iI).firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = GET_PLAYER((PlayerTypes)iI).nextUnit(&iLoop))
			{
				if (pLoopUnit->getReconPlot() == this)
				{
					if (bIncrement)
					{
						pLoopUnit->changeDebugCount(1);
					}
					else
					{
						pLoopUnit->changeDebugCount(-1);
					}
					changeAdjacentSight(pLoopUnit->getTeam(), iRange, bIncrement, pLoopUnit, bUpdatePlotGroups);
				}
			}
		}
	}
}


void CvPlot::updateSeeFromSight(bool bIncrement, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iDX, iDY;

	int iRange = GC.getMAX_UNIT_VISIBILITY_RANGE() + 1;

	for (iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (iDY = -iRange; iDY <= iRange; iDY++)
		{
			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

			if (pLoopPlot != NULL)
			{
				pLoopPlot->updateSight(bIncrement, bUpdatePlotGroups);
			}
		}
	}
}


bool CvPlot::canHaveBonus(BonusTypes eBonus, bool bIgnoreLatitude) const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (eBonus == NO_BONUS)
	{
		return true;
	}

	if (getBonusType() != NO_BONUS)
	{
		return false;
	}


	if (getFeatureType() != NO_FEATURE)
	{
		if (!(GC.getBonusInfo(eBonus).isFeature(getFeatureType())))
		{
			return false;
		}

		if (!(GC.getBonusInfo(eBonus).isFeatureTerrain(getTerrainType())))
		{
			return false;
		}
	}
	else
	{
		if (!(GC.getBonusInfo(eBonus).isTerrain(getTerrainType())))
		{
			return false;
		}
	}

	if (isHills())
	{
		if (!(GC.getBonusInfo(eBonus).isHills()))
		{
			return false;
		}
	}
	else if (isPeak2(true))
	{
		if (!(GC.getBonusInfo(eBonus).isPeaks()))
		{
			return false;
		}
	}
	else if (isFlatlands())
	{
		if (!(GC.getBonusInfo(eBonus).isFlatlands()))
		{
			return false;
		}
	}

	if (GC.getBonusInfo(eBonus).isNoRiverSide())
	{
		if (isRiverSide())
		{
			return false;
		}
	}

	if (GC.getBonusInfo(eBonus).getMinAreaSize() > 1)
	{
		if (area()->getNumTiles() < GC.getBonusInfo(eBonus).getMinAreaSize())
		{
			return false;
		}
	}

	if (!bIgnoreLatitude)
	{
		if (getLatitude() > GC.getBonusInfo(eBonus).getMaxLatitude())
		{
			return false;
		}

		if (getLatitude() < GC.getBonusInfo(eBonus).getMinLatitude())
		{
			return false;
		}
	}

	if (!isPotentialCityWork())
	{
		return false;
	}

	int iCount = GC.getBonusInfo(eBonus).getNumMapCategoryTypes();
	bool bFound = (iCount < 1);
	for (int iI = 0; iI < iCount; iI++)
	{
		if (isMapCategoryType((MapCategoryTypes)GC.getBonusInfo(eBonus).getMapCategoryType(iI)))
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		return false;
	}

	return true;
}

bool CvPlot::canBuildImprovement(ImprovementTypes eImprovement, TeamTypes eTeam) const
{
	bool bReturn = false;
	for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
	{
		if ((ImprovementTypes)GC.getBuildInfo((BuildTypes)iI).getImprovement() == eImprovement)
		{
			BuildTypes eBuild = ((BuildTypes)iI);
			if (GET_TEAM(eTeam).isHasTech((TechTypes)GC.getBuildInfo(eBuild).getTechPrereq()))
			{
				bReturn = true;
				break;
			}
		}
	}
	return bReturn;
}

bool CvPlot::canHaveImprovementAsUpgrade(ImprovementTypes eImprovement, TeamTypes eTeam, bool bPotential, bool bOver) const
{
	if (eTeam == NO_TEAM)
	{
		return false;
	}

	if (m_iCurrentRoundofUpgradeCache > -1)
	{
		if (GET_TEAM(eTeam).getLastRoundOfValidImprovementCacheUpdate() > m_iCurrentRoundofUpgradeCache)
		{
			m_iCurrentRoundofUpgradeCache = GC.getGame().getGameTurn();
		}
		else
		{
			return (m_iCurrentRoundofUpgradeCache == 1);
		}
	}

	bool bCanHave = canHaveImprovement(eImprovement, eTeam, bPotential, bOver, true);

	if (bCanHave && !canBuildImprovement(eImprovement, eTeam))
	{
		bCanHave = false;
	}

	if (bCanHave)
	{
		m_iCurrentRoundofUpgradeCache = 1;
		return true;
	}

	m_iCurrentRoundofUpgradeCache = 0;
	return false;
}


// Needs to be improved a bit. What if no builds account for the ability to generate the improvement?
// Hmm... maybe a whole nother check that along with this one gets cached significantly?
bool CvPlot::canHaveImprovement(ImprovementTypes eImprovement, TeamTypes eTeam, bool bPotential, bool bOver, bool bUpgradeCheck) const
{
	FAssertMsg(eImprovement != NO_IMPROVEMENT, "Improvement is not assigned a valid value");
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (isCity())
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isNotOnAnyBonus() && getBonusType() != NO_BONUS)
	{
		return false;
	}
	// Don't upgrade if old imp is specialized for the plot bonus and the upgrade isn't.
	if (getBonusType() != NO_BONUS  && bUpgradeCheck && getImprovementType() != NO_IMPROVEMENT
	&& GC.getImprovementInfo(getImprovementType()).isImprovementBonusMakesValid(getBonusType())
	&& !GC.getImprovementInfo(eImprovement).isImprovementBonusMakesValid(getBonusType()))
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isWater() != isWater())
	{
		return false;
	}
	// Overide old?
	if (!bOver && eImprovement != NO_IMPROVEMENT && getImprovementType() == eImprovement)
	{
		return false;
	}
	// Feature forbids
	if (getFeatureType() != NO_FEATURE && GC.getFeatureInfo(getFeatureType()).isNoImprovement())
	{
		return false;
	}
	// Irrigation req
	if (!bPotential && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isIgnoreIrrigation())
	&& GC.getImprovementInfo(eImprovement).isRequiresIrrigation() && !isIrrigationAvailable())
	{
		return false;
	}

	bool bValid = false;

	if (isPeak2(true))
	{
		if (!GC.getImprovementInfo(eImprovement).isPeakMakesValid())
		{
			return false;
		}
		bValid = true;
	}
	else if (getBonusType(eTeam) != NO_BONUS
	&& GC.getImprovementInfo(eImprovement).isImprovementBonusMakesValid(getBonusType(eTeam))
	&& !GC.getImprovementInfo(eImprovement).isRequiresPeak())
	{
		return true;
	}

	if (getBonusType(eTeam) != NO_BONUS && GC.getImprovementInfo(eImprovement).isImprovementObsoleteBonusMakesValid(getBonusType(eTeam)))
	{
		if (GET_TEAM(eTeam).isHasTech((TechTypes)GC.getBonusInfo(getBonusType(eTeam)).getTechObsolete()))
		{
			return true;
		}
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isNoFreshWater() && isFreshWater())
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isRequiresFlatlands() && !isFlatlands())
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isCanMoveSeaUnits() && !isCoastalLand())
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isRequiresFeature() && (getFeatureType() == NO_FEATURE))
	{
		return false;
	}

	if (GC.getImprovementInfo(eImprovement).isHillsMakesValid() && isHills())
	{
		bValid = true;
	}

	if (GC.getImprovementInfo(eImprovement).isFreshWaterMakesValid() && isFreshWater())
	{
		bValid = true;
	}

	if (GC.getImprovementInfo(eImprovement).isRiverSideMakesValid() && isRiverSide())
	{
		bValid = true;
	}

	if (GC.getImprovementInfo(eImprovement).getTerrainMakesValid(getTerrainType()))
	{
		bValid = true;
	}

	if ((getFeatureType() != NO_FEATURE) && GC.getImprovementInfo(eImprovement).getFeatureMakesValid(getFeatureType()))
	{
		bValid = true;
	}

	//Desert has negative defense
	bool bIgnoreNatureYields = false;
	if (GC.getTerrainInfo(getTerrainType()).getDefenseModifier() < 0 && GC.getImprovementInfo(eImprovement).isRequiresIrrigation()
	&& (eTeam != NO_TEAM && GET_TEAM(eTeam).isCanFarmDesert() || getTeam() != NO_TEAM && GET_TEAM(getTeam()).isCanFarmDesert()))
	{
		bValid = true;
		bIgnoreNatureYields = true; // This is a min yield override for desert
	}

	if (!bValid)
	{
		return false;
	}

	// Minimum yield
	if (!bIgnoreNatureYields)
	{
		for (int iI = 0; iI < NUM_YIELD_TYPES; ++iI)
		{
			if (calculateNatureYield(((YieldTypes)iI), eTeam) < GC.getImprovementInfo(eImprovement).getPrereqNatureYield(iI))
			{
				return false;
			}
		}
	}
	// Riverside
	if (GC.getImprovementInfo(eImprovement).isRequiresRiverSide())
	{
		bValid = false;
		for (int iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
		{
			const CvPlot* pLoopPlot = plotCardinalDirection(getX(), getY(), ((CardinalDirectionTypes)iI));

			if (pLoopPlot != NULL && isRiverCrossing(directionXY(this, pLoopPlot)))
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
				FAssertMsg(false, "Can build value cache verification failure");
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

	struct canBuildCacheEntry* entry;

	//	If this player does not own, and cannot build a unit that can perform this build
	//	no need to check with the Python.
	if ( !GET_PLAYER(ePlayer).canHaveBuilder(eBuild) )
	{
		return 0L;
	}

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

	ImprovementTypes eImprovement;
	ImprovementTypes eFinalImprovementType;
	RouteTypes eRoute;
	bool bValid;

	if (eBuild == NO_BUILD)
	{
		return false;
	}

	//	Tech requirements are not checked here - they are checked in CvPlayer::canBuild() which will also be called
	//	when necessary (which is why the enabling tech is not checked here)

	if(bIncludePythonOverrides && GC.getUSE_CAN_BUILD_CALLBACK(eBuild))
	{
		long lResult = canBuildFromPython(eBuild, ePlayer);

		if (lResult >= 1)
		{
			return true;
		}
		else if (lResult == 0)
		{
			return false;
		}
	}

	bValid = false;

	if (GC.getBuildInfo(eBuild).getNumPlaceBonusTypes() > 0)
	{
		if (getBonusType() != NO_BONUS)
		{
			return false;
		}

		bool bAccessFound = false;
		for (int iI = 0; iI < GC.getBuildInfo(eBuild).getNumPlaceBonusTypes(); iI++)
		{
			BonusTypes eBonus = GC.getBuildInfo(eBuild).getPlaceBonusType(iI).eBonus;
			bAccessFound = false;
			if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).bRequiresAccess)
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
				if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqMapCategory != NO_MAPCATEGORY)
				{
					bAccessFound = false;
					if (isMapCategoryType(GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqMapCategory))
					{
						bAccessFound = true;
					}
				}
			}
			if (bAccessFound)
			{
				if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqTech != NO_TECH)
				{
					bAccessFound = false;
					if (GET_TEAM(getTeam()).isHasTech(GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqTech))
					{
						bAccessFound = true;
					}
				}
			}
			if (bAccessFound)
			{
				if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqTerrain != NO_TERRAIN)
				{
					bAccessFound = false;
					if (getTerrainType() == GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqTerrain)
					{
						bAccessFound = true;
					}
				}
			}
			if (bAccessFound)
			{
				if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqFeature != NO_FEATURE)
				{
					bAccessFound = false;
					if (getFeatureType() == GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqFeature)
					{
						bAccessFound = true;
					}
				}
			}
			if (bAccessFound)
			{
				bAccessFound = false;
				if(canHaveBonus(GC.getBuildInfo(eBuild).getPlaceBonusType(iI).eBonus))
				{
					bAccessFound = true;
				}
			}
			if (bAccessFound)
			{
				break;
			}
		}
		if (!bAccessFound)
		{
			return false;
		}
	}



	for (int iI = 0; iI < GC.getBuildInfo(eBuild).getNumPrereqBonusTypes(); iI++)
	{
		BonusTypes ePrereqBonus = ((BonusTypes)GC.getBuildInfo(eBuild).getPrereqBonusType(iI));

		if (!isAdjacentPlotGroupConnectedBonus(ePlayer, ePrereqBonus))
		{
			return false;
		}
	}

	int iCount = GC.getBuildInfo(eBuild).getNumMapCategoryTypes();
	bool bFound = (iCount < 1);
	for (int iI = 0; iI < iCount; iI++)
	{
		if (isMapCategoryType((MapCategoryTypes)GC.getBuildInfo(eBuild).getMapCategoryType(iI)))
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		return false;
	}

	eImprovement = ((ImprovementTypes)(GC.getBuildInfo(eBuild).getImprovement()));

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (!canHaveImprovement(eImprovement, GET_PLAYER(ePlayer).getTeam(), bTestVisible))
		{
			return false;
		}

		int iCount = GC.getImprovementInfo(eImprovement).getNumMapCategoryTypes();
		bool bFound = (iCount < 1);
		for (int iI = 0; iI < iCount; iI++)
		{
			if (isMapCategoryType((MapCategoryTypes)GC.getImprovementInfo(eImprovement).getMapCategoryType(iI)))
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			return false;
		}

		// Super Forts begin *build*
		if (GC.getImprovementInfo(eImprovement).getUniqueRange() > 0)
		{
			int iUniqueRange = GC.getImprovementInfo(eImprovement).getUniqueRange();
			for (int iDX = -iUniqueRange; iDX <= iUniqueRange; iDX++)
			{
				for (int iDY = -iUniqueRange; iDY <= iUniqueRange; iDY++)
				{
					CvPlot *pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
					if (pLoopPlot != NULL && pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
					{
						if (finalImprovementUpgrade(pLoopPlot->getImprovementType()) == finalImprovementUpgrade(eImprovement))
						{
							return false;
						}
					}
				}
			}
		}
		// Super Forts end

		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(getImprovementType()).isPermanent())
			{
				return false;
			}

			//	Koshling - unable to merge this check because backward compatbilioty with existing
			//	save states requires the use of a unified cities-and-forts cultural range structure
			//	(which we already had in a slightly more limitted manner, courtesy of AND).  Since I also
			//	canno se how getCultureRangeForts() could ever be > 1 anyway (for forts defined with iCultureRange of 1
			//	at least) I cannot really see what this limitation was achieving.
			//	THIS MAY NEED TWEAKING
			// Super Forts begin *AI_worker* - prevent forts from being built over when outside culture range
			//if (GC.getImprovementInfo(getImprovementType()).isActsAsCity())
			//{
			//	if (!isWithinCultureRange(ePlayer) && !(getCultureRangeForts(ePlayer) > 1))
			//	{
			//		return false;
			//	}
			//}
			// Super Forts end

			if (getImprovementType() == eImprovement)
			{
				return false;
			}

			eFinalImprovementType = finalImprovementUpgrade(getImprovementType());

			//TB Commented out to allow for units to build over current improvements with their upgrads (if the build to do so is defined.)
			//if (eFinalImprovementType != NO_IMPROVEMENT)
			//{
			//	if (eFinalImprovementType == finalImprovementUpgrade(eImprovement))
			//	{
			//		return false;
			//	}
			//}
		}

		if (!bTestVisible)
		{
			if (GET_PLAYER(ePlayer).getTeam() != getTeam())
			{
				//outside borders can't be built in other's culture
				if (GC.getImprovementInfo(eImprovement).isOutsideBorders())
				{
					if (getTeam() != NO_TEAM)
					{
						return false;
					}
				}
				else //only buildable in own culture
				{
					return false;
				}
			}
			// Super Forts begin *AI_worker* - prevent workers from two different players from building a fort in the same plot
			if(GC.getImprovementInfo(eImprovement).isActsAsCity())
			{
				CLinkList<IDInfo> oldUnits;
				CLLNode<IDInfo>* pUnitNode = headUnitNode();
				CvUnit* pLoopUnit;

				while (pUnitNode != NULL)
				{
					oldUnits.insertAtEnd(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);
				}

				pUnitNode = oldUnits.head();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);
					if(pLoopUnit->getOwner() != ePlayer)
					{
						if(pLoopUnit->getBuildType() != NO_BUILD)
						{
							ImprovementTypes eImprovementBuild = (ImprovementTypes)(GC.getBuildInfo(pLoopUnit->getBuildType()).getImprovement());
							if(eImprovementBuild != NO_IMPROVEMENT)
							{
								if(GC.getImprovementInfo(eImprovementBuild).isActsAsCity())
								{
									return false;
								}
							}
						}
					}
				}
			}
			// Super Forts end
		}

		bValid = true;
	}

	eRoute = ((RouteTypes)(GC.getBuildInfo(eBuild).getRoute()));

	if (eRoute != NO_ROUTE)
	{
		if (getRouteType() != NO_ROUTE)
		{
			if (GC.getRouteInfo(getRouteType()).getValue() >= GC.getRouteInfo(eRoute).getValue())
			{
				return false;
			}
			/************************************************************************************************/
			/* Afforess	                  Start		 12/8/09                                                */
			/*                                                                                              */
			/*    The AI should not replace sea tunnels with non-sea tunnels                                */
			/************************************************************************************************/
			else if (GC.getRouteInfo(getRouteType()).isSeaTunnel() && (!GC.getRouteInfo(eRoute).isSeaTunnel()))
			{
				return false;
			}
			/************************************************************************************************/
			/* Afforess	                     END                                                            */
			/************************************************************************************************/
		}

		if (!bTestVisible)
		{
			if (GC.getRouteInfo(eRoute).getPrereqBonus() != NO_BONUS)
			{
				if (!isAdjacentPlotGroupConnectedBonus(ePlayer, ((BonusTypes)(GC.getRouteInfo(eRoute).getPrereqBonus()))))
				{
					return false;
				}
			}

			bool bFoundValid = true;
			for (int i = 0; i < GC.getNUM_ROUTE_PREREQ_OR_BONUSES(); ++i)
			{
				if (NO_BONUS != GC.getRouteInfo(eRoute).getPrereqOrBonus(i))
				{
					bFoundValid = false;

					if (isAdjacentPlotGroupConnectedBonus(ePlayer, ((BonusTypes)(GC.getRouteInfo(eRoute).getPrereqOrBonus(i)))))
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

		bValid = true;
	}

	if (getFeatureType() != NO_FEATURE)
	{
		if (GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()))
		{
			if (isOwned() && (GET_PLAYER(ePlayer).getTeam() != getTeam()) && !atWar(GET_PLAYER(ePlayer).getTeam(), getTeam()))
			{
				return false;
			}

			bValid = true;
		}
	}
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/13/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	TerrainTypes eTerrain;
	eTerrain = ((TerrainTypes)(GC.getBuildInfo(eBuild).getTerrainChange()));

	if (eTerrain != NO_TERRAIN)
	{
		if (getTerrainType() == eTerrain)
		{
			return false;
		}

		bValid = true;
	}

	FeatureTypes eFeature;
	eFeature = ((FeatureTypes)(GC.getBuildInfo(eBuild).getFeatureChange()));

	if (eFeature != NO_FEATURE)
	{
		if (getFeatureType() == eFeature)
		{
			return false;
		}

		int iCount = GC.getFeatureInfo(eFeature).getNumMapCategoryTypes();
		bool bFound = (iCount < 1);
		for (int iI = 0; iI < iCount; iI++)
		{
			if (isMapCategoryType((MapCategoryTypes)GC.getFeatureInfo(eFeature).getMapCategoryType(iI)))
			{
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{
			return false;
		}

		bValid = true;
	}
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/

	return bValid;
}


int CvPlot::getBuildTime(BuildTypes eBuild) const
{
	int iTime;

	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");
	CvBuildInfo& kBuild = GC.getBuildInfo(eBuild);
	iTime = kBuild.getTime();

	if (getFeatureType() != NO_FEATURE)
	{
		iTime += kBuild.getFeatureTime(getFeatureType());
	}

	for (int iI = 0; iI < kBuild.getNumTerrainStructs(); iI++)
	{
		if (kBuild.getTerrainStruct(iI).eTerrain == getTerrainType())
		{
			iTime += kBuild.getTerrainStruct(iI).iTime;
		}
	}

/************************************************************************************************/
/* Afforess	                  Start		 07/18/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (isPeak2(true))
	{
		iTime *= std::max(0, (GC.getDefineINT("PEAK_BUILD_TIME_MODIFIER") + 100));
		iTime /= 100;
	}

	RouteTypes eCurrentRoute = getRouteType();
	if (kBuild.getRoute() != NO_ROUTE)
	{
		if (eCurrentRoute != NO_ROUTE)
		{
			if (GC.getDefineINT("ROUTES_UPGRADE") > 0)
			{
				for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
				{
					BuildTypes eCurrentBuild = (BuildTypes)iI;
					if (GC.getBuildInfo(eCurrentBuild).getRoute() == eCurrentRoute)
					{
						iTime = std::max(1, iTime - GC.getBuildInfo(eCurrentBuild).getTime());
						break;
					}
				}
			}
		}
	}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	iTime *= std::max(0, (GC.getTerrainInfo(getTerrainType()).getBuildModifier() + 100));
	iTime /= 100;

	iTime *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getBuildPercent();
	iTime /= 100;

	iTime *= GC.getEraInfo(GC.getGame().getStartEra()).getBuildPercent();
	iTime /= 100;

	return iTime;
}


// BUG - Partial Builds - start
int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, PlayerTypes ePlayer) const
{
	int iWorkRate = getBuildTurnsLeft(eBuild, 0, 0);
	if (iWorkRate == 0)
	{
		return GET_PLAYER(ePlayer).getWorkRate(eBuild);
	}
	else
	{
		return iWorkRate;
	}
}
// BUG - Partial Builds - end

// BUG - Partial Builds - start
int CvPlot::getBuildTurnsLeft(BuildTypes eBuild, int iNowExtra, int iThenExtra, bool bIncludeUnits) const
// BUG - Partial Builds - end
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iNowBuildRate;
	int iThenBuildRate;
	int iBuildLeft;
	int iTurnsLeft;

	iNowBuildRate = iNowExtra;
	iThenBuildRate = iThenExtra;

// BUG - Partial Builds - start
	if (bIncludeUnits)
	{
// BUG - Partial Builds - end
// RevolutionDCM - start BUG extra changes
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->getBuildType() == eBuild)
			{
				if (pLoopUnit->canMove())
				{
					iNowBuildRate += pLoopUnit->workRate(false);
				}
				iThenBuildRate += pLoopUnit->workRate(true);
			}
		}
// RevolutionDCM - end BUG extra changes
// BUG - Partial Builds - start
	}
// BUG - Partial Builds - end

	if (iThenBuildRate == 0)
	{
		//this means it will take forever under current circumstances
		return MAX_INT;
	}

	iBuildLeft = getBuildTime(eBuild);

	iBuildLeft -= getBuildProgress(eBuild);
	iBuildLeft -= iNowBuildRate;

	iBuildLeft = std::max(0, iBuildLeft);

	iTurnsLeft = (iBuildLeft / iThenBuildRate);

	if ((iTurnsLeft * iThenBuildRate) < iBuildLeft)
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

	int iProduction = GC.getBuildInfo(eBuild).getFeatureProduction(getFeatureType());
	iProduction -= std::max(0, plotDistance(getX(), getY(), (*ppCity)->getX(), (*ppCity)->getY()) - 2) * 5;

	iProduction *= std::max(0, (GET_PLAYER((*ppCity)->getOwner()).getFeatureProductionModifier() + 100));
	iProduction /= 100;

	iProduction *= GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getFeatureProductionPercent();
	iProduction /= 100;

	iProduction *= std::min((GC.getDefineINT("BASE_FEATURE_PRODUCTION_PERCENT") + (GC.getDefineINT("FEATURE_PRODUCTION_PERCENT_MULTIPLIER") * (*ppCity)->getPopulation())), 100);
	iProduction /= 100;

	if (getTeam() != eTeam)
	{
		iProduction *= GC.getDefineINT("DIFFERENT_TEAM_FEATURE_PRODUCTION_PERCENT");
		iProduction /= 100;
	}

/************************************************************************************************/
/* Afforess	                  Start		 05/25/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (!GET_TEAM(eTeam).isHasTech((TechTypes)GC.getBuildInfo(eBuild).getFeatureTech(getFeatureType())))
	{
		return 0;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

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
	int getDefenderScore(const CvPlot* plot, const CvUnit* pLoopUnit, PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove, bool bAssassinate, ECacheAccess::flags cacheAccess)
	{
		// Check the unit is valid
		if (
			// In delayed death cycle
			pLoopUnit->plot() == NULL
			// Going to be dead
			|| pLoopUnit->AI_getPredictedHitPoints() == 0
			// Already dead
			|| pLoopUnit->isDead()
			// Doesn't belong to the player we are interested in
			|| (eOwner != NO_PLAYER && pLoopUnit->getOwner() != eOwner)
			)
		{
			return 0;
		}

		CvChecksum checksum;
		if (cacheAccess != ECacheAccess::None)
		{
			// Work out our cache key (todo: change this to just be a proper hash key)
			if (pAttacker != NULL)
			{
				checksum.add(pAttacker->getID());
			}
			checksum.add((int)pLoopUnit->getOwner());
			checksum.add(pLoopUnit->getID());
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
					&& itr->second.iHealth == pLoopUnit->currHitPoints())
				{
					return itr->second.iValue;
				}
			}
		}

		int iValue = 0;

		if (
			// Assassination target, if we have one
				(!bAssassinate || (pAttacker && pLoopUnit->isTargetOf(*pAttacker)))
			// Not invisible to the player, if any
			&& (eAttackingPlayer == NO_PLAYER || !pLoopUnit->isInvisible(GET_PLAYER(eAttackingPlayer).getTeam(), false))
			// War enemy (todo: we should just assert that attacking player is valid if we are testing for enemy)
			&& (!bTestAtWar
				|| eAttackingPlayer == NO_PLAYER
				|| pLoopUnit->isEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), plot)
				|| (pAttacker != NULL && pAttacker->isEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), plot, pLoopUnit)))
			// Units cannot coexist together
			&& (pAttacker == NULL || !pLoopUnit->canUnitCoexistWithArrivingUnit(*pAttacker))
			// Potential enemy (todo: we should just assert that attacking player is valid if we are testing for potential enemy)
			&& (!bTestPotentialEnemy
				|| eAttackingPlayer == NO_PLAYER
				|| pLoopUnit->isPotentialEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), plot)
				|| (pAttacker != NULL && pAttacker->isPotentialEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), plot, pLoopUnit))
				)
			// If we are testing movement, can the unit move?
			&& (!bTestCanMove || (pLoopUnit->canMove() && !pLoopUnit->isCargo()))
			// If the attacker is an air unit then is the units current damage less than the attackers damage limit?
			&& (pAttacker == NULL
				|| pAttacker->getDomainType() != DOMAIN_AIR
				|| pLoopUnit->getDamage() < pAttacker->airCombatLimit(pLoopUnit)
				)
			)
		{
			iValue = pAttacker ? pLoopUnit->defenderValue(pAttacker) : 0;
			iValue += (pLoopUnit->tauntTotal() * iValue) / 100;
			// It should be greater than 0 as this target is at least valid as per the checks above
			iValue = std::max(1, iValue);
		}

		if (cacheAccess & ECacheAccess::Write)
		{
			MEMORY_TRACK_EXEMPT();

			unitDefenderInfo info;
			info.iHealth = pLoopUnit->currHitPoints();
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

/************************************************************************************************/
/* Afforess	                  Start		 06/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	//TB - Commented out due to suspicion that this will lead to some of the strange problems we have with bad defense decisions or poor group evaluations changing once attack is launched.
	//if (isCity() && getPlotCity()->isInvaded() && !bAssassinate)
	//{
	//	return getWorstDefender(eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove);
	//}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	//	Heavily cache this routine as during large stack fights the question is asked over and over, with the
	//	same attacker cropping up repeatedly

	if ( g_bestDefenderCachePlot != this )
	{
		g_bestDefenderCachePlot = this;
		g_bestDefenderCache->clear();
	}

	int iBestValue = 0;
	CvUnit* pBestUnit = nullptr;

	// Can't use this as it requires more than 9 args, and bind only supports 9
	//CvUnit* pBestUnit = scoring::max_score(units(),
	//	bst::bind(&CvSelectionGroup::getDefenderScore, this, _1, eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove, bAssassinate, bClearCache ? ECacheAccess::Write : ECacheAccess::ReadWrite)
	//).get_value_or(nullptr);

	foreach_ (CvUnit* unit, units())
	{
		int iValue = getDefenderScore(this, unit, eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove, bAssassinate, bClearCache ? ECacheAccess::Write : ECacheAccess::ReadWrite);

		if (iValue > iBestValue)
		{
			pBestUnit = unit;
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
	bool bFound = false;

	CvUnit* pFirstUnit = NULL;
	int iValue = 0;

	for (unit_iterator unit = beginUnits(); unit != endUnits(); ++unit)
	{
		CvUnit* pLoopUnit = *unit;

		int iValue = getDefenderScore(this, pLoopUnit, eOwner, eAttackingPlayer, pAttacker, bTestAtWar, bTestPotentialEnemy, bTestCanMove, false, ECacheAccess::Write);

		if (iValue > 0)
		{
			pFirstUnit = pLoopUnit;
			break;
		}
	}

	return pFirstUnit;
}

// returns a sum of the strength (adjusted by firepower) of all the units on a plot
int CvPlot::AI_sumStrength(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, DomainTypes eDomainType, StrengthFlags::flags flags /*= StrengthFlags::DefensiveBonuses*/, int iRange) const
{
	const int COLLATERAL_COMBAT_DAMAGE = GC.getDefineINT("COLLATERAL_COMBAT_DAMAGE"); // K-Mod. (currently this number is "10")

	const bool bTestAtWar = flags & StrengthFlags::TestAtWar;
	const bool bTestPotentialEnemy = flags & StrengthFlags::TestPotentialEnemy;
	const bool bDefensiveBonuses = flags & StrengthFlags::DefensiveBonuses;
	const bool bCollatoral = flags & StrengthFlags::CollatoralDamage;
	FAssertMsg(!bTestPotentialEnemy || eAttackingPlayer != NO_PLAYER, "Need to specify the attacking player to filter by enemies");

	int	strSum = 0;

	for (int iDX = -iRange; iDX <= iRange; iDX++)
	{
		for (int iDY = -iRange; iDY <= iRange; iDY++)
		{
			const CvPlot* pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

			if (pLoopPlot == NULL)
				continue;

			for(unit_iterator unitItr = pLoopPlot->beginUnits(); unitItr != pLoopPlot->endUnits(); ++unitItr)
			{
				CvUnit* pLoopUnit = *unitItr;

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
	}

	return strSum;
}


CvUnit* CvPlot::getSelectedUnit() const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			if (pLoopUnit->IsSelected())
			{
				return pLoopUnit;
			}
		}
	}

	return NULL;
}

//ls612: Advanced Nuke Interception

//int CvPlot::getUnitNukeIntercept(PlayerTypes eOwner) const
//{
//	CLLNode<IDInfo>* pUnitNode;
//	CvUnit* pLoopUnit;
//	int iChance;
//	int iTempValue = 0;
//	iChance = 0;
//
//	pUnitNode = headUnitNode();
//
//	while (pUnitNode != NULL)
//	{
//		pLoopUnit = ::getUnit(pUnitNode->m_data);
//		pUnitNode = nextUnitNode(pUnitNode);
//
//		if (((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner)) && (pLoopUnit->getUnitInfo().getNukeInterceptionProbability() > iTempValue))
//		{
//			iChance = pLoopUnit->getUnitInfo().getNukeInterceptionProbability();
//			iTempValue = iChance;
//		}
//	}
//
//	return iChance;
//}

int CvPlot::getUnitPower(PlayerTypes eOwner) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount;

	iCount = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
		{
			iCount += pLoopUnit->getUnitInfo().getPowerValue();
		}
	}

	return iCount;
}

// Super Forts begin *bombard*
bool CvPlot::isBombardable(const CvUnit* pUnit) const
{
	ImprovementTypes eImprovement;

	if (NULL != pUnit && !pUnit->isEnemy(getTeam()))
	{
		return false;
	}

	eImprovement = getImprovementType();
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
// Super Forts end

// Super Forts begin *culture*
#ifdef UNUSED_SUPER_FORTS_METHODS
int CvPlot::getCultureRangeForts(PlayerTypes ePlayer) const
{
	if (NULL == m_aiCultureRangeForts)
	{
		return 0;
	}

	return m_aiCultureRangeForts[ePlayer];
}

void CvPlot::setCultureRangeForts(PlayerTypes ePlayer, int iNewValue)
{
	if (getCultureRangeForts(ePlayer) != iNewValue)
	{
		if(NULL == m_aiCultureRangeForts)
		{
			m_aiCultureRangeForts = new short[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_aiCultureRangeForts[iI] = 0;
			}
		}

		m_aiCultureRangeForts[ePlayer] = iNewValue;

		if(getCulture(ePlayer) == 0)
		{
			changeCulture(ePlayer, 1, false);
		}
	}
}

void CvPlot::changeCultureRangeForts(PlayerTypes ePlayer, int iChange)
{
	if (0 != iChange)
	{
		setCultureRangeForts(ePlayer, (getCultureRangeForts(ePlayer) + iChange));
	}
}

bool CvPlot::isWithinFortCultureRange(PlayerTypes ePlayer) const
{
	return (getCultureRangeForts(ePlayer) > 0);
}
#endif

void CvPlot::changeCultureRangeFortsWithinRange(PlayerTypes ePlayer, int iChange, int iRange, bool bUpdate)
{
	CvPlot* pLoopPlot;
	int iDX, iDY;
	int iCultureDistance;

	if ((0 != iChange) && (iRange >= 0))
	{
		for (iDX = -iRange; iDX <= iRange; iDX++)
		{
			for (iDY = -iRange; iDY <= iRange; iDY++)
			{
				iCultureDistance = plotDistance(0, 0, iDX, iDY);

				if(iCultureDistance <= iRange)
				{
					pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

					if (pLoopPlot != NULL)
					{
						pLoopPlot->changeCultureRangeCities(ePlayer, iCultureDistance, iChange, false, false);

						//	Super Forts adaptation to C2C - make sure that if a plot is newly
						//	in range it gets at least 1 culture
						if ( iChange > 0 )
						{
							if(pLoopPlot->getCulture(ePlayer) == 0)
							{
								pLoopPlot->changeCulture(ePlayer, 1, false);
							}
						}

						if(bUpdate)
						{
							pLoopPlot->updateCulture(true,true);
						}
					}
				}
			}
		}
	}
}

void CvPlot::doImprovementCulture()
{
	CvPlot* pLoopPlot;
	int iDX, iDY;
	int iCultureDistance, iCulture, iCultureRange;
	ImprovementTypes eImprovement;
	PlayerTypes ePlayer;

	eImprovement = getImprovementType();
	if (eImprovement != NO_IMPROVEMENT)
	{
		ePlayer = getOwner();
		if(ePlayer != NO_PLAYER)
		{
			iCulture = GC.getImprovementInfo(eImprovement).getCulture();
			if(iCulture > 0)
			{
				iCultureRange = GC.getImprovementInfo(eImprovement).getCultureRange();

				if(iCultureRange > 0)
				{
					for (iDX = -iCultureRange; iDX <= iCultureRange; iDX++)
					{
						for (iDY = -iCultureRange; iDY <= iCultureRange; iDY++)
						{
							iCultureDistance = plotDistance(0, 0, iDX, iDY);

							if(iCultureDistance <= iCultureRange)
							{
								pLoopPlot = plotXY(getX(), getY(), iDX, iDY);

								if (pLoopPlot != NULL)
								{
									int iChange = ((iCultureRange - ((iCultureDistance == 0) ? 1 : iCultureDistance))*iCulture) + iCulture;
									pLoopPlot->changeCulture(ePlayer,iChange,false);
								}
							}
						}
					}
				}
				else
				{
					changeCulture(ePlayer,iCulture,false);
				}
			}
		}
	}
}
// Super Forts end

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
	CvPlot* pAdjacentPlot;
	int iCount = 0;
	for(int iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotCardinalDirection(getX(), getY(), ((CardinalDirectionTypes)iI));
		if(pAdjacentPlot != NULL)
		{
			if(pAdjacentPlot->isImpassable() || (area() != pAdjacentPlot->area()))
			{
				++iCount;
			}
		}
	}
	return iCount;
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
	else if (isPeak2(true))
	{
		iModifier += GC.getPEAK_EXTRA_DEFENSE();
	}

	ImprovementTypes eImprovement;
	if (bHelp)
	{
		eImprovement = getRevealedImprovementType(GC.getGame().getActiveTeam(), false);
	}
	else
	{
		eImprovement = getImprovementType();
	}

	if (eImprovement != NO_IMPROVEMENT && eDefender != NO_TEAM
	&& (getTeam() == NO_TEAM || GET_TEAM(eDefender).isFriendlyTerritory(getTeam())))
	{
		// Super Forts begin *bombard*
		iModifier += GC.getImprovementInfo(eImprovement).getDefenseModifier() - getDefenseDamage();
	}

	if (!bHelp)
	{
		CvCity* pCity = getPlotCity();

		if (pCity != NULL)
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
	//	ones and visa versa.  That not only results in incorrect path generation, but also to OOS errors in multiplayer
	//	due to the cached values being used at different times (so on one m,achine the plot may be first seen as revealed, while on
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
			CvRouteInfo& fromRoute = GC.getRouteInfo(fromRouteType);
			const RouteTypes toRouteType = getRouteType();
			CvRouteInfo& toRoute = GC.getRouteInfo(toRouteType);

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

				/************************************************************************************************/
				/* Afforess	Mountains Start		 09/20/09                                           		 */
				/*                                                                                              */
				/*                                                                                              */
				/************************************************************************************************/
				if (isPeak2(true))
				{
					if (!GET_TEAM(pUnit->getTeam()).isMoveFastPeaks())
					{
						iRegularCost += GC.getPEAK_EXTRA_MOVEMENT();
					}
					else
					{
						iRegularCost += 1;
					}
				}
				/************************************************************************************************/
				/* Afforess	Mountains End       END        		                                             */
				/************************************************************************************************/
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
		MEMORY_TRACK_EXEMPT();

		(*m_resultHashMap)[iResultKeyHash] = iResult;

}

	return iResult;
}

int CvPlot::getExtraMovePathCost() const
{
	return GC.getGame().getPlotExtraCost(getX(), getY());
}


void CvPlot::changeExtraMovePathCost(int iChange)
{
	GC.getGame().changePlotExtraCost(getX(), getY(), iChange);
}

int	CvPlot::getHasMountainLeader(TeamTypes eTeam) const
{
	int iCount = plotCount(PUF_isMountainLeader, -1, -1, NULL, NO_PLAYER, eTeam);
	return (iCount > 0);
}

bool CvPlot::isAdjacentOwned() const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isOwned())
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isAdjacentPlayer(PlayerTypes ePlayer, bool bLandOnly) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getOwner() == ePlayer)
			{
				if (!bLandOnly || !(pAdjacentPlot->isWater()))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvPlot::isAdjacentTeam(TeamTypes eTeam, bool bLandOnly) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getTeam() == eTeam)
			{
				if (!bLandOnly || !(pAdjacentPlot->isWater()))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvPlot::isWithinCultureRange(PlayerTypes ePlayer, int* iFoundRange) const
{
	int iI;

	for (iI = 0; iI < GC.getNumCultureLevelInfos(); ++iI)
	{
		if (isCultureRangeCity(ePlayer, iI))
		{
			if ( iFoundRange != NULL )
			{
				*iFoundRange = iI;
			}
			return true;
		}
	}

	return false;
}


int CvPlot::getNumCultureRangeCities(PlayerTypes ePlayer) const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < GC.getNumCultureLevelInfos(); ++iI)
	{
		iCount += getCultureRangeCities(ePlayer, iI);
	}

	return iCount;
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/10/10                                jdog5000      */
/*                                                                                              */
/* General AI                                                                                   */
/************************************************************************************************/
bool CvPlot::isHasPathToEnemyCity( TeamTypes eAttackerTeam, bool bIgnoreBarb ) const
{
	PROFILE_FUNC();

	int iI;
	CvCity* pLoopCity = NULL;
	int iLoop;

	FAssert(eAttackerTeam != NO_TEAM);

	if( (area()->getNumCities() - GET_TEAM(eAttackerTeam).countNumCitiesByArea(area())) == 0 )
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
	for (iI = 0; !bFound && iI < MAX_PC_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_TEAM(eAttackerTeam).AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_WARPLAN )
		{
			if( !bIgnoreBarb || !(GET_PLAYER((PlayerTypes)iI).isNPC() || GET_PLAYER((PlayerTypes)iI).isMinorCiv()) )
			{
				pLoopCity = GET_PLAYER((PlayerTypes)iI).getCapitalCity();

				if( pLoopCity != NULL )
				{
					if( (pLoopCity->area() == area()) )
					{
						bFound = gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, getX(), getY(), pLoopCity->getX(), pLoopCity->getY(), false, 0, true);

						if( bFound )
						{
							break;
						}
					}
				}
			}
		}
	}

	// Check all other cities
	for (iI = 0; !bFound && iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_TEAM(eAttackerTeam).AI_getWarPlan(GET_PLAYER((PlayerTypes)iI).getTeam()) != NO_WARPLAN )
		{
			if( !bIgnoreBarb || !(GET_PLAYER((PlayerTypes)iI).isNPC() || GET_PLAYER((PlayerTypes)iI).isMinorCiv()) )
			{
				for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); !bFound && pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
				{
					if( (pLoopCity->area() == area()) && !(pLoopCity->isCapital()) )
					{
						bFound = gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, getX(), getY(), pLoopCity->getX(), pLoopCity->getY(), false, 0, true);

						if( bFound )
						{
							break;
						}
					}
				}
			}
		}
	}

	gDLL->getFAStarIFace()->destroy(pTeamStepFinder);

	return bFound;
}

bool CvPlot::isHasPathToPlayerCity( TeamTypes eMoveTeam, PlayerTypes eOtherPlayer ) const
{
	PROFILE_FUNC();

	CvCity* pLoopCity = NULL;
	int iLoop;

	FAssert(eMoveTeam != NO_TEAM);

	if( (area()->getCitiesPerPlayer(eOtherPlayer) == 0) )
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

	for (pLoopCity = GET_PLAYER(eOtherPlayer).firstCity(&iLoop); !bFound && pLoopCity != NULL; pLoopCity = GET_PLAYER(eOtherPlayer).nextCity(&iLoop))
	{
		if( pLoopCity->area() == area() )
		{
			bFound = gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, getX(), getY(), pLoopCity->getX(), pLoopCity->getY(), false, 0, true);

			if( bFound )
			{
				break;
			}
		}
	}

	gDLL->getFAStarIFace()->destroy(pTeamStepFinder);

	return bFound;
}

int CvPlot::calculatePathDistanceToPlot( TeamTypes eTeam, CvPlot* pTargetPlot ) const
{
	PROFILE_FUNC();

	FAssert(eTeam != NO_TEAM);

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
	FAStarNode* pNode;

	int iPathDistance = -1;
	gDLL->getFAStarIFace()->GeneratePath(pTeamStepFinder, getX(), getY(), pTargetPlot->getX(), pTargetPlot->getY(), false, 0, true);

	pNode = gDLL->getFAStarIFace()->GetLastNode(&GC.getStepFinder());

	if (pNode != NULL)
	{
		iPathDistance = pNode->m_iData1;
	}

	gDLL->getFAStarIFace()->destroy(pTeamStepFinder);

	return iPathDistance;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
// Plot danger cache
bool CvPlot::isActivePlayerNoDangerCache() const
{
	return m_bIsActivePlayerNoDangerCache;
}

bool CvPlot::isActivePlayerHasDangerCache() const
{
	return m_bIsActivePlayerHasDangerCache;
}

bool CvPlot::isTeamBorderCache( TeamTypes eTeam ) const
{
	return m_abIsTeamBorderCache[eTeam];
}

void CvPlot::setIsActivePlayerNoDangerCache( bool bNewValue ) const
{
	m_bIsActivePlayerNoDangerCache = bNewValue;
}

void CvPlot::setIsActivePlayerHasDangerCache( bool bNewValue ) const
{
	m_bIsActivePlayerHasDangerCache = bNewValue;
}

void CvPlot::setIsTeamBorderCache( TeamTypes eTeam, bool bNewValue ) const
{
	PROFILE_FUNC();
	m_abIsTeamBorderCache[eTeam] = bNewValue;
}

void CvPlot::invalidateIsTeamBorderCache() const
{
	PROFILE_FUNC();

	for( int iI = 0; iI < MAX_TEAMS; iI++ )
	{
		m_abIsTeamBorderCache[iI] = false;
	}
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


/*** Dexy - Fixed Borders START ****/
/* returns the city adjacent to this plot or NULL if none exists. more than one can't exist, because of the 2-tile spacing btwn cities limit. */
//Alberts2: added eplayer parameter to only return the city if the owner == eplayer
CvCity* CvPlot::getAdjacentCity(PlayerTypes eplayer) const
{
	int iDX, iDY;
	CvPlot* pLoopPlot;
	CvCity* pLoopCity;

	for (iDX = -1; iDX <= 1; iDX++)
	{
		for (iDY = -1; iDY <= 1; iDY++)
		{
			pLoopPlot = plotXY(getX(), getY(), iDX, iDY);
			if (pLoopPlot != NULL)
			{
				pLoopCity = pLoopPlot->getPlotCity();
				if (pLoopCity != NULL)
				{
					if(eplayer != NO_PLAYER)
					{
						return (pLoopCity->getOwner() == eplayer ? pLoopCity : NULL);
					}

					return pLoopCity;
				}
			}
		}
	}

	return NULL;
}
/*** Dexy - Fixed Borders  END  ****/


PlayerTypes CvPlot::calculateCulturalOwner() const
{
	PROFILE("CvPlot::calculateCulturalOwner()")

	CvCity* pLoopCity;
	CvCity* pBestCity;
	CvPlot* pLoopPlot;
	PlayerTypes eBestPlayer;
	bool bValid;
	int iCulture;
	int iBestCulture;
	int iBestCultureRange;
	int iPriority;
	int iBestPriority;
	int iI;

	if (isForceUnowned())
	{
		return NO_PLAYER;
	}

	iBestCulture = 0;
	iBestCultureRange = MAX_INT;
	eBestPlayer = NO_PLAYER;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iCulture = getCulture((PlayerTypes)iI);

			if (iCulture > 0)
			{
				int iCultureRange;

				// Super Forts begin *culture* - modified if statement
				//	Koshling - modified back since C2C uses a unified city/fort culture state
				//	inheritted from AND
				//if (isWithinCultureRange((PlayerTypes)iI) || isWithinFortCultureRange((PlayerTypes)iI))
				if (isWithinCultureRange((PlayerTypes)iI, &iCultureRange)) // Original Code
				// Super Forts end
				{
					//	Koshling - modified so that equal culture victory goes to the player with the closest cultuer source as
					//	first tie-breaker
					if (iCulture > iBestCulture ||
						(iCulture == iBestCulture &&
						 (iCultureRange < iBestCultureRange || (getOwner() == iI && iCultureRange == iBestCultureRange))))
					{
						iBestCulture = iCulture;
						eBestPlayer = ((PlayerTypes)iI);
						iBestCultureRange = iCultureRange;
					}
				}
			}
		}
	}
	iCulture = 0;
	if (getOwner() != NO_PLAYER && eBestPlayer == NO_PLAYER)
	{
		iCulture = getCulture(getOwner());
		if (iCulture < 1)
		{
			return NO_PLAYER;
		}
	}
/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	bool bOwnerHasUnit;

	/* plots that are not forts and are adjacent to cities always belong to those cities' owners */
	if (GC.getGame().isOption(GAMEOPTION_MIN_CITY_BORDER) && !isActsAsCity())
	{
		CvCity* adjacentCity = getAdjacentCity();
		if (adjacentCity != NULL)
		{
			return adjacentCity->getOwner();
		}
	}


	//	Have to check for the current owner being alive for this to work correctly
	//	in the cultural re-assignment that takes place as he dies during processing of the capture
	//	of his last city
	if (getOwner() != NO_PLAYER && GET_PLAYER(getOwner()).isAlive())
	{
		if (GET_PLAYER(getOwner()).hasFixedBorders())
		{
			//Can not steal land from other, more dominant fixed border civilization
			bool bValidCulture = true;
			if (eBestPlayer != NO_PLAYER && getOwner() != eBestPlayer)
			{
				bValidCulture = !GET_PLAYER(eBestPlayer).hasFixedBorders();
			}

			//	Koshling - changed Fixed Borders to not unconditionally hold territory, but only
			//	to hold it against a natural owner with less than twice the FB player's culture
			if (bValidCulture &&
				getCulture(getOwner()) > iBestCulture/2 &&
				(isWithinCultureRange(getOwner()) || isWithinOccupationRange(getOwner())))
			{
				return getOwner();
			}

			bOwnerHasUnit = false;

			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->getTeam() == getTeam())
				{
					if (pLoopUnit->canClaimTerritory(NULL))
					{
						bOwnerHasUnit = true;
						break;
					}
				}
			}

			if (bOwnerHasUnit)
			{
				return getOwner();
			}

			//This is a fort, outside of our normal borders, but we have fixed borders, and it has not been claimed
			if (isActsAsCity())
			{
				return getOwner();
			}
		}
	}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	if (!isCity())
	{
		if (eBestPlayer != NO_PLAYER)
		{
			iBestPriority = MAX_INT;
			pBestCity = NULL;

			for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				pLoopPlot = plotCity(getX(), getY(), iI);

				if (pLoopPlot != NULL)
				{
					pLoopCity = pLoopPlot->getPlotCity();

					if (pLoopCity != NULL)
					{
						if (pLoopCity->getTeam() == GET_PLAYER(eBestPlayer).getTeam() || GET_TEAM(GET_PLAYER(eBestPlayer).getTeam()).isVassal(pLoopCity->getTeam()))
						{
							if (getCulture(pLoopCity->getOwner()) > 0)
							{
								if (isWithinCultureRange(pLoopCity->getOwner()))
								{
									iPriority = GC.getCityPlotPriority()[iI];

									if (pLoopCity->getTeam() == GET_PLAYER(eBestPlayer).getTeam())
									{
										iPriority += 5; // priority ranges from 0 to 4 -> give priority to Masters of a Vassal
									}

									if ((iPriority < iBestPriority) || ((iPriority == iBestPriority) && (pLoopCity->getOwner() == eBestPlayer)))
									{
										iBestPriority = iPriority;
										pBestCity = pLoopCity;
									}
								}
							}
						}
					}
				}
			}

			if (pBestCity != NULL)
			{
				eBestPlayer = pBestCity->getOwner();
			}
		}
	}

	if (eBestPlayer == NO_PLAYER)
	{
		bValid = true;

		for (iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
		{
			pLoopPlot = plotCardinalDirection(getX(), getY(), ((CardinalDirectionTypes)iI));

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->isOwned())
				{
					if (eBestPlayer == NO_PLAYER)
					{
						eBestPlayer = pLoopPlot->getOwner();
					}
					else if (eBestPlayer != pLoopPlot->getOwner())
					{
						bValid = false;
						break;
					}
				}
				else
				{
					bValid = false;
					break;
				}
			}
		}

		if (!bValid
			// Super Forts begin *culture*
			|| !GET_PLAYER(eBestPlayer).isAlive())
			// Super Forts end
		{
			eBestPlayer = NO_PLAYER;
		}
	}

	return eBestPlayer;
}


void CvPlot::plotAction(PlotUnitFunc func, int iData1, int iData2, PlayerTypes eOwner, TeamTypes eTeam)
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
		{
			if ((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
			{
				func(pLoopUnit, iData1, iData2, NULL);
			}
		}
	}
}


int CvPlot::plotCount(ConstPlotUnitFunc funcA, int iData1A, int iData2A, const CvUnit* pUnit, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B, int iRange) const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount;

	iCount = 0;

	if ( iRange == 0 )
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL)
			{
				if ( pLoopUnit->isDead() )
				{
					continue;
				}

				if ((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner))
				{
					if ((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
					{
						if ((funcA == NULL) || funcA(pLoopUnit, iData1A, iData2A, pUnit))
						{
							if ((funcB == NULL) || funcB(pLoopUnit, iData1B, iData2B, NULL))
							{
								iCount++;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		//	Recurse for each plot in the specified range
		for(int iX = -iRange; iX <= iRange; iX++)
		{
			for(int iY = -iRange; iY <= iRange; iY++)
			{
				CvPlot* pLoopPlot = plotXY(getX(), getY(), iX, iY);

				if ( pLoopPlot != NULL )
				{
					iCount += pLoopPlot->plotCount(funcA, iData1A, iData2A, pUnit, eOwner, eTeam, funcB, iData1B, iData2B);
				}
			}
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

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iStrength;

	iStrength = 0;

	if ( iRange == 0 )
	{
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit != NULL && ((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner)))
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
		for(int iX = -iRange; iX <= iRange; iX++)
		{
			for(int iY = -iRange; iY <= iRange; iY++)
			{
				CvPlot* pLoopPlot = plotXY(getX(), getY(), iX, iY);

				if ( pLoopPlot != NULL )
				{
					iStrength += pLoopPlot->plotStrengthTimes100(eFlags, funcA, iData1A, iData2A, eOwner, eTeam, funcB, iData1B, iData2B, 0);
				}
			}
		}
	}

	return iStrength;
}


CvUnit* CvPlot::plotCheck(ConstPlotUnitFunc funcA, int iData1A, int iData2A, const CvUnit* pUnit, PlayerTypes eOwner, TeamTypes eTeam, ConstPlotUnitFunc funcB, int iData1B, int iData2B) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL && ((eOwner == NO_PLAYER) || (pLoopUnit->getOwner() == eOwner)))
		{
			if ((eTeam == NO_TEAM) || (pLoopUnit->getTeam() == eTeam))
			{
				if (funcA(pLoopUnit, iData1A, iData2A, pUnit))
				{
					if ((funcB == NULL) || funcB(pLoopUnit, iData1B, iData2B, NULL))
					{
						return pLoopUnit;
					}
				}
			}
		}
	}

	return NULL;
}


bool CvPlot::isOwned() const
{
	return (getOwner() != NO_PLAYER);
}


bool CvPlot::isBarbarian() const
{
	return (getOwner() == BARBARIAN_PLAYER);
}

bool CvPlot::isNPC() const
{
	return (getOwner() >= MAX_PC_PLAYERS);
}

bool CvPlot::isHominid() const
{
	return (getOwner() == BARBARIAN_PLAYER || getOwner() == NEANDERTHAL_PLAYER);
}


bool CvPlot::isRevealedBarbarian() const
{
	return (getRevealedOwner(GC.getGame().getActiveTeam(), true) == BARBARIAN_PLAYER);
}


bool CvPlot::isVisible(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
	{
		return true;
	}
	else
	{
		if (eTeam == NO_TEAM)
		{
			return false;
		}

		return ((getVisibilityCount(eTeam) > 0) || (getStolenVisibilityCount(eTeam) > 0));
	}
}


bool CvPlot::isActiveVisible(bool bDebug) const
{
	return isVisible(GC.getGame().getActiveTeam(), bDebug);
}


bool CvPlot::isVisibleToCivTeam() const
{
	int iI;

	for (iI = 0; iI < MAX_PC_TEAMS; ++iI)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (isVisible(((TeamTypes)iI), false))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isVisibleToWatchingHuman() const
{
	int iI;

	for (iI = 0; iI < MAX_PC_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).isHuman())
			{
				if (isVisible(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
				{
					return true;
				}
			}
		}
	}

	return false;
}


bool CvPlot::isAdjacentVisible(TeamTypes eTeam, bool bDebug) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isVisible(eTeam, bDebug))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::isAdjacentNonvisible(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (!pAdjacentPlot->isVisible(eTeam, false))
			{
				return true;
			}
		}
	}

	return false;
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

	return ((getRevealedImprovementType(eTeam, false) == NO_IMPROVEMENT) ? false : GC.getImprovementInfo(getRevealedImprovementType(eTeam, false)).isGoody());
}


void CvPlot::removeGoody()
{
	setImprovementType(NO_IMPROVEMENT);
}


bool CvPlot::isCity(bool bCheckImprovement, TeamTypes eForTeam) const
{
	if (bCheckImprovement && NO_IMPROVEMENT != getImprovementType() && GC.getImprovementInfo(getImprovementType()).isActsAsCity()
	&& (NO_TEAM == eForTeam || NO_TEAM == getTeam() && GC.getImprovementInfo(getImprovementType()).isOutsideBorders() || GET_TEAM(eForTeam).isFriendlyTerritory(getTeam())))
	{
		return true;
	}
	return (getPlotCity() != NULL);
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
/************************************************************************************************/
/* Afforess	Vicinity Bonus Start		 07/29/09                                            */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
bool CvPlot::isHasValidBonus() const
{
	if (getBonusType() == NO_BONUS)
	{
		return false;
	}
	if (getImprovementType() == NO_IMPROVEMENT)
	{
		return false;
	}
	if (!isBonusNetwork(getTeam()))
	{
		return false;
	}
	if (!isWithinTeamCityRadius(getTeam()))
	{
		return false;
	}
	if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)m_eBonusType).getTechReveal())))
	{
		if (GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(getBonusType()))
		{
			return true;
		}
	}
	if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)m_eBonusType).getTechReveal())) && GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)m_eBonusType).getTechObsolete())))
	{
		if (GC.getImprovementInfo(getImprovementType()).isImprovementBonusTrade(getBonusType()))
		{
			return true;
		}
	}

	return false;
}
/************************************************************************************************/
/* Afforess	Vicinity Bonus End       END                                                     */
/************************************************************************************************/
bool CvPlot::isEnemyCity(const CvUnit& kUnit, bool bOnlyRealCities) const
{
	// Super Forts begin *culture*
	TeamTypes ePlotTeam = getTeam();
	if (isCity(!bOnlyRealCities) && (ePlotTeam != NO_TEAM))
	{
		return kUnit.isEnemy(ePlotTeam, this);
	}
	/* Original Code
	CvCity* pCity = getPlotCity();

	if (pCity != NULL)
	{
		return kUnit.isEnemy(pCity->getTeam(), this);
	} */
	// Super Forts end

	return false;
}


bool CvPlot::isOccupation() const
{
	CvCity* pCity;

	pCity = getPlotCity();

	if (pCity != NULL)
	{
		return pCity->isOccupation();
	}

	return false;
}


bool CvPlot::isBeingWorked() const
{
	CvCity* pWorkingCity;

	pWorkingCity = getWorkingCity();

	if (pWorkingCity != NULL)
	{
		return pWorkingCity->isWorkingPlot(this);
	}

	return false;
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

	return (plotCheck(PUF_canDefendEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(this), NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner()) != NULL);
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

	if (pUnit != NULL && isVisible(pUnit->getTeam(), false))
	{
		return plotCount(PUF_canDefendPotentialEnemyAgainst, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
	}
	return 0;
}

int CvPlot::getNumVisiblePotentialEnemyDefenderless(const CvUnit* pUnit) const
{
	PROFILE_FUNC();

	if (pUnit != NULL && isVisible(pUnit->getTeam(), false))
	{
		return plotCount(PUF_canDefenselessPotentialEnemyAgainst, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
	}
	return 0;
}

bool CvPlot::isVisiblePotentialEnemyDefender(const CvUnit* pUnit) const
{
	PROFILE_FUNC();

	if (pUnit != NULL && isVisible(pUnit->getTeam(), false))
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
	return (plotCheck(PUF_isEnemy, ePlayer, 0, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer) != NULL);
}

int CvPlot::getNumVisibleUnits(PlayerTypes ePlayer) const
{
	return plotCount(PUF_isVisibleDebug, ePlayer);
}

/************************************************************************************************/
/* Afforess	                  Start		 6/22/11                                                */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
int CvPlot::getNumVisibleEnemyUnits(PlayerTypes ePlayer) const
{
	return plotCount(PUF_isEnemy, ePlayer, 0, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer);
}

int CvPlot::getNumVisibleEnemyCombatUnits(PlayerTypes ePlayer) const
{
	PROFILE_FUNC();

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iCount;

	iCount = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if ( pLoopUnit->isDead() )
		{
			continue;
		}

		if (! (pLoopUnit->canAttack() || pLoopUnit->canDefend()))
		{
			continue;
		}

		if (PUF_isEnemy(pLoopUnit, ePlayer, 0))
		{
			if (PUF_isVisible(pLoopUnit, ePlayer))
			{
				iCount++;
			}
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
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

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
	return (plotCheck(PUF_isEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(this), pUnit, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner()) != NULL);
}

bool CvPlot::isVisibleOtherUnit(PlayerTypes ePlayer) const
{
	return (plotCheck(PUF_isOtherTeam, ePlayer, -1, NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, ePlayer) != NULL);
}


bool CvPlot::isFighting() const
{
	return (plotCheck(PUF_isFighting) != NULL);
}


bool CvPlot::canHaveFeature(FeatureTypes eFeature, bool bOverExistingFeature) const
{
	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");

	if (eFeature == NO_FEATURE)
	{
		return true;
	}
	if (!bOverExistingFeature && getFeatureType() != NO_FEATURE)
	{
		return false;
	}
	if (getImprovementType() != NO_IMPROVEMENT && !GC.getImprovementInfo(getImprovementType()).getFeatureMakesValid(eFeature))
	{
		return false;
	}
	if (isCity() || isPeak())
	{
		return false;
	}

	const CvFeatureInfo& kFeature = GC.getFeatureInfo(eFeature);

	if (!kFeature.isTerrain(getTerrainType()))
	{
		return false;
	}
	if (kFeature.isNoCoast() && isCoastalLand())
	{
		return false;
	}
	if (kFeature.isNoRiver() && isRiver())
	{
		return false;
	}
	if (kFeature.isRequiresFlatlands() && isHills())
	{
		return false;
	}
	if (kFeature.isNoAdjacent())
	{
		CvPlot* pAdjacentPlot;

		for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL && pAdjacentPlot->getFeatureType() == eFeature)
			{
				return false;
			}
		}
	}
	if (kFeature.isRequiresRiver() && !isRiver())
	{
		return false;
	}
	return true;
}


bool CvPlot::isRoute() const
{
	return (getRouteType() != NO_ROUTE);
}

/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
bool CvPlot::isCanMoveAllUnits() const
{
	return false;
}

bool CvPlot::isCanMoveLandUnits() const
{
	//Check is only to determine if there's a tunnel.
	if (isSeaTunnel())
	{
		return true;
	}

	return isCanMoveAllUnits();
}

bool CvPlot::isCanMoveSeaUnits() const
{
	//Check is to determine if there's a way for a sea unit to be on the plot.
	if (getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(getImprovementType()).isCanMoveSeaUnits())
	{
		return true;
	}
	if (isWater())
	{
		return true;
	}
	return isCanMoveAllUnits();
}

bool CvPlot::isCanUseRouteLandUnits() const
{
	return true;
}

bool CvPlot::isCanUseRouteSeaUnits() const
{
	if (isSeaTunnel())
	{
		return false;
	}

	return true;
}

bool CvPlot::isSeaTunnel() const
{
	if (isRoute())
	{
		if (GC.getRouteInfo(getRouteType()).isSeaTunnel())
		{
			return true;
		}
	}

	return false;
}
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/

bool CvPlot::isValidRoute(const CvUnit* pUnit) const
{
	if (isRoute())
	{
		TeamTypes eTeam = getTeam();

		if (eTeam == NO_TEAM || GET_TEAM(pUnit->getTeam()).isFriendlyTerritory(eTeam) || GET_TEAM(pUnit->getTeam()).isOpenBorders(eTeam) || pUnit->isEnemyRoute())
		//if (!pUnit->isEnemy(getTeam(), this) || pUnit->isEnemyRoute())
		{
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			if (pUnit->getDomainType() == DOMAIN_LAND && !isCanUseRouteLandUnits())
			{
				return false;
			}

			if (pUnit->getDomainType() == DOMAIN_SEA && !isCanUseRouteSeaUnits())
			{
				return false;
			}
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
			return true;
		}
	}

	return false;
}


bool CvPlot::isTradeNetworkImpassable(TeamTypes eTeam) const
{
/************************************************************************************************/
/* Afforess	Mountains Start		 09/18/09                                                  		*/
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*Original Code:
	return (isImpassable() && !isRiverNetwork(eTeam));
	*/
	return (isImpassable(eTeam) && !isRiverNetwork(eTeam) && !isRoute() );
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                                */
/************************************************************************************************/
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

	if (!isOwned())
	{
		if (!isRevealed(eTeam, false))
		{
			return false;
		}
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

	if (!isOwned())
	{
		if (!isRevealed(eTeam, false) || !(pPlot->isRevealed(eTeam, false)))
		{
			return false;
		}
	}

	if (isRoute())
	{
		if (pPlot->isRoute())
		{
			return true;
		}
	}

	if (isCity(true, eTeam))
	{
		if (pPlot->isNetworkTerrain(eTeam))
		{
			return true;
		}
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

		if (pPlot->isRiverNetwork(eTeam))
		{
			if (pPlot->isRiverConnection(directionXY(pPlot, this)))
			{
				return true;
			}
		}
	}

	if (isRiverNetwork(eTeam))
	{
		if (pPlot->isNetworkTerrain(eTeam))
		{
			if (isRiverConnection(directionXY(this, pPlot)))
			{
				return true;
			}
		}

		if (isRiverConnection(directionXY(this, pPlot)) || pPlot->isRiverConnection(directionXY(pPlot, this)))
		{
			if (pPlot->isRiverNetwork(eTeam))
			{
				return true;
			}
		}
	}

	return false;
}


bool CvPlot::isValidDomainForLocation(const CvUnit& unit) const
{
	if (isValidDomainForAction(unit))
	{
		return true;
	}

	return isCity(true, unit.getTeam());
}


bool CvPlot::isValidDomainForAction(const CvUnit& unit) const
{
	switch (unit.getDomainType())
	{
	case DOMAIN_SEA:
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		return (isWater() || unit.canMoveAllTerrain() || isCanMoveSeaUnits());
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
		break;

	case DOMAIN_AIR:
		return false;
		break;

	case DOMAIN_LAND:
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		return (!isWater() || unit.canMoveAllTerrain() || isCanMoveLandUnits());
		break;
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
	case DOMAIN_IMMOBILE:
		return (!isWater() || unit.canMoveAllTerrain());
		break;

	default:
		FAssert(false);
		break;
	}

	return false;
}


bool CvPlot::isImpassable(TeamTypes eTeam) const
{
	if (isPeak2(true))
	{
		if (eTeam != NO_TEAM && GET_TEAM(eTeam).isCanPassPeaks())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	if (getTerrainType() == NO_TERRAIN)
	{
		return false;
	}

	return ((getFeatureType() == NO_FEATURE) ? GC.getTerrainInfo(getTerrainType()).isImpassable() : GC.getFeatureInfo(getFeatureType()).isImpassable());
}

int CvPlot::getViewportX() const
{
	CvViewport*	pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);

	return pCurrentViewPort->getViewportXFromMapX(m_iX);
}


int CvPlot::getViewportY() const
{
	CvViewport*	pCurrentViewPort = GC.getCurrentViewport();
	FAssert(pCurrentViewPort != NULL);

	return pCurrentViewPort->getViewportYFromMapY(m_iY);
}

bool CvPlot::isInViewport(int comfortBorderSize) const
{
	CvViewport*	pCurrentViewPort = GC.getCurrentViewport();
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
	CvMap& kMap = GC.getMap();

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
	CvMap& kMap = GC.getMap();

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
	CvViewport* pCurrentViewport = GC.getCurrentViewport();

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
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						01/02/09		jdog5000		*/
/* 																			*/
/* 	General AI																*/
/********************************************************************************/
/* original BTS code
CvArea* CvPlot::waterArea() const
*/
CvArea* CvPlot::waterArea(bool bNoImpassable) const
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/
{
	CvArea* pBestArea;
	CvPlot* pAdjacentPlot;
	int iValue;
	int iBestValue;
	int iI;

	if (isWater())
	{
		return area();
	}

	iBestValue = 0;
	pBestArea = NULL;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						01/02/09		jdog5000		*/
/* 																			*/
/* 	General AI																*/
/********************************************************************************/
/* original BTS code
			if (pAdjacentPlot->isWater())
*/
			if (pAdjacentPlot->isWater() && (!bNoImpassable || !(pAdjacentPlot->isImpassable())))
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/
			{
				iValue = pAdjacentPlot->area()->getNumTiles();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pAdjacentPlot->area();
				}
			}
		}
	}

	return pBestArea;
}

CvArea* CvPlot::secondWaterArea() const
{

	CvArea* pWaterArea = waterArea();
	CvArea* pBestArea;
	CvPlot* pAdjacentPlot;
	int iValue;
	int iBestValue;
	int iI;

	FAssert(!isWater());

	iBestValue = 0;
	pBestArea = NULL;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isWater() && (pAdjacentPlot->getArea() != pWaterArea->getID()))
			{
				iValue = pAdjacentPlot->area()->getNumTiles();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					pBestArea = pAdjacentPlot->area();
				}
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
	bool bOldLake;

	if (getArea() != iNewValue)
	{
		bOldLake = isLake();

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
	FAssert(m_iFeatureVariety >= 0);
	return m_iFeatureVariety;
}


int CvPlot::getOwnershipDuration() const
{
	return m_iOwnershipDuration;
}


bool CvPlot::isOwnershipScore() const
{
	return (getOwnershipDuration() >= GC.getDefineINT("OWNERSHIP_SCORE_DURATION_THRESHOLD"));
}


void CvPlot::setOwnershipDuration(int iNewValue)
{
	bool bOldOwnershipScore;

	if (getOwnershipDuration() != iNewValue)
	{
		bOldOwnershipScore = isOwnershipScore();

		m_iOwnershipDuration = iNewValue;
		FAssert(getOwnershipDuration() >= 0);

		if (bOldOwnershipScore != isOwnershipScore())
		{
			if (isOwned())
			{
				if (!isWater())
				{
					GET_PLAYER(getOwner()).changeTotalLandScored((isOwnershipScore()) ? 1 : -1);
				}
			}
		}
	}
}


void CvPlot::changeOwnershipDuration(int iChange)
{
	setOwnershipDuration(getOwnershipDuration() + iChange);
}


int CvPlot::getImprovementDuration() const
{
	return m_iImprovementDuration;
}


void CvPlot::setImprovementDuration(int iNewValue)
{
	m_iImprovementDuration = iNewValue;
	FAssert(getImprovementDuration() >= 0);
}


void CvPlot::changeImprovementDuration(int iChange)
{
	setImprovementDuration(getImprovementDuration() + iChange);
}


int CvPlot::getUpgradeProgressHundredths() const
{
	return m_iUpgradeProgress;
}


int CvPlot::getUpgradeTimeLeft(ImprovementTypes eImprovement, PlayerTypes ePlayer) const
{
	int iUpgradeLeftTimes100;
	int iUpgradeRateTimes100;
	int iTurnsLeft;

	iUpgradeLeftTimes100 = (100*GC.getGame().getImprovementUpgradeTime(eImprovement) - ((getImprovementType() == eImprovement) ? getUpgradeProgressHundredths() : 0));

	if (ePlayer == NO_PLAYER)
	{
		return iUpgradeLeftTimes100/100;
	}

	iUpgradeRateTimes100 = GET_PLAYER(ePlayer).getImprovementUpgradeRateTimes100(eImprovement);

	if (iUpgradeRateTimes100 == 0)
	{
		return iUpgradeLeftTimes100;
	}

	iTurnsLeft = (iUpgradeLeftTimes100) / iUpgradeRateTimes100;

	if ((iTurnsLeft * iUpgradeRateTimes100) < iUpgradeLeftTimes100)
	{
		iTurnsLeft++;
	}

	return std::max(1, iTurnsLeft);
}


void CvPlot::setUpgradeProgressHundredths(int iNewValue)
{
	m_iUpgradeProgress = iNewValue;
	FAssert(getUpgradeProgressHundredths() >= 0);
}


void CvPlot::changeUpgradeProgressHundredths(int iChange)
{
	int iMax = (MAX_INT/2);
	int iToNormalSet = getUpgradeProgressHundredths() + iChange;
	int iSet = std::min(iMax, iToNormalSet);
	if (iSet == iMax)
	{
		setPlotIgnoringImprovementUpgrade(false);
	}
	setUpgradeProgressHundredths(iSet);
}


int CvPlot::getForceUnownedTimer() const
{
	return m_iForceUnownedTimer;
}


bool CvPlot::isForceUnowned() const
{
	return (getForceUnownedTimer() > 0);
}


void CvPlot::setForceUnownedTimer(int iNewValue)
{
	m_iForceUnownedTimer = iNewValue;
	FAssert(getForceUnownedTimer() >= 0);
}


void CvPlot::changeForceUnownedTimer(int iChange)
{
	setForceUnownedTimer(getForceUnownedTimer() + iChange);
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
	m_iCityRadiusCount = (m_iCityRadiusCount + iChange);
	FAssert(getCityRadiusCount() >= 0);
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
	CvPlot* pAdjacentPlot;
	int iI;

	if ((isNOfRiver() != bNewValue) || (eRiverDir != m_eRiverWEDirection))
	{
		if (isNOfRiver() != bNewValue)
		{
			updatePlotGroupBonus(false);
			m_bNOfRiver = bNewValue;
			updatePlotGroupBonus(true);

			updateRiverCrossing();
			updateYield();

			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL)
				{
					pAdjacentPlot->updateRiverCrossing();
					pAdjacentPlot->updateYield();
				}
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
	CvPlot* pAdjacentPlot;
	int iI;

	if ((isWOfRiver() != bNewValue) || (eRiverDir != m_eRiverNSDirection))
	{
		if (isWOfRiver() != bNewValue)
		{
			updatePlotGroupBonus(false);
			m_bWOfRiver = bNewValue;
			updatePlotGroupBonus(true);

			updateRiverCrossing();
			updateYield();

			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL)
				{
					pAdjacentPlot->updateRiverCrossing();
					pAdjacentPlot->updateYield();
				}
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
	CvPlot* pLoopPlot;
	int iDX, iDY;

	if (isIrrigated() != bNewValue)
	{
		m_bIrrigated = bNewValue;

		for (iDX = -1; iDX <= 1; iDX++)
		{
			for (iDY = -1; iDY <= 1; iDY++)
			{
				pLoopPlot	= plotXY(getX(), getY(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updateYield();
					pLoopPlot->setLayoutDirty(true);
				}
			}
		}
	}
}


void CvPlot::updateIrrigated()
{
	PROFILE("CvPlot::updateIrrigated()");

	CvPlot* pLoopPlot;
	FAStar* pIrrigatedFinder;
	bool bFoundFreshWater;
	bool bIrrigated;
	int iI;

	if (area() == NULL)
	{
		return;
	}

	if (!(GC.getGame().isFinalInitialized()))
	{
		return;
	}

	pIrrigatedFinder = gDLL->getFAStarIFace()->create();

	if (isIrrigated())
	{
		if (!isPotentialIrrigation())
		{
			setIrrigated(false);

			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					bFoundFreshWater = false;
					gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), NULL, NULL, NULL, potentialIrrigation, NULL, checkFreshWater, &bFoundFreshWater);
					gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, pLoopPlot->getX(), pLoopPlot->getY(), -1, -1);

					if (!bFoundFreshWater)
					{
						bIrrigated = false;
						gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), NULL, NULL, NULL, potentialIrrigation, NULL, changeIrrigated, &bIrrigated);
						gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, pLoopPlot->getX(), pLoopPlot->getY(), -1, -1);
					}
				}
			}
		}
	}
	else
	{
		if (isPotentialIrrigation() && isIrrigationAvailable(true))
		{
			bIrrigated = true;
			gDLL->getFAStarIFace()->Initialize(pIrrigatedFinder, GC.getMap().getGridWidth(), GC.getMap().getGridHeight(), GC.getMap().isWrapX(), GC.getMap().isWrapY(), NULL, NULL, NULL, potentialIrrigation, NULL, changeIrrigated, &bIrrigated);
			gDLL->getFAStarIFace()->GeneratePath(pIrrigatedFinder, getX(), getY(), -1, -1);
		}
	}

	gDLL->getFAStarIFace()->destroy(pIrrigatedFinder);
}


bool CvPlot::isPotentialCityWork() const
{
	return m_bPotentialCityWork;
}


bool CvPlot::isPotentialCityWorkForArea(CvArea* pArea) const
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	int iI;

	for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		pLoopPlot = plotCity(getX(), getY(), iI);

		if (pLoopPlot != NULL)
		{
			if (!(pLoopPlot->isWater()) || GC.getWATER_POTENTIAL_CITY_WORK_FOR_AREA())
			{
				if (pLoopPlot->area() == pArea)
				{
					return true;
				}
			}
		}
	}

	return false;
}


void CvPlot::updatePotentialCityWork()
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	bool bValid;
	int iI;

	bValid = false;

	for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		pLoopPlot = plotCity(getX(), getY(), iI);

		if (pLoopPlot != NULL)
		{
			if (!(pLoopPlot->isWater()))
			{
				bValid = true;
				break;
			}
		}
	}

	if (isPotentialCityWork() != bValid)
	{
		m_bPotentialCityWork = bValid;

		updateYield();
	}
}


bool CvPlot::isShowCitySymbols() const
{
	return m_bShowCitySymbols;
}


void CvPlot::updateShowCitySymbols()
{
	PROFILE_FUNC();

	CvCity* pLoopCity;
	CvPlot* pLoopPlot;
	bool bNewShowCitySymbols;
	int iI;

	bNewShowCitySymbols = false;

	for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
	{
		pLoopPlot = plotCity(getX(), getY(), iI);

		if (pLoopPlot != NULL)
		{
			pLoopCity = pLoopPlot->getPlotCity();

			if (pLoopCity != NULL)
			{
				if (pLoopCity->isCitySelected() && gDLL->getInterfaceIFace()->isCityScreenUp())
				{
					if (pLoopCity->canWork(this))
					{
						bNewShowCitySymbols = true;
						break;
					}
				}
			}
		}
	}

	if (isShowCitySymbols() != bNewShowCitySymbols)
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
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pOldCity;
	CvCity* pNewCity;
	CvUnit* pLoopUnit;
	CvWString szBuffer;
	UnitTypes eBestUnit;
	int iFreeUnits;
	int iI;

	if (getOwner() != eNewValue)
	{
		PROFILE("CvPlot::setOwner.changed");

		GC.getGame().addReplayMessage(REPLAY_MESSAGE_PLOT_OWNER_CHANGE, eNewValue, (char*)NULL, getX(), getY());

		pOldCity = getPlotCity();

		if (pOldCity != NULL)
		{
			{
				MEMORY_TRACK_EXEMPT();

				if ( isInViewport() )
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_REVOLTED_JOINED", pOldCity->getNameKey(), GET_PLAYER(eNewValue).getCivilizationDescriptionKey());
					AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_MAJOR_EVENT,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getViewportX(),getViewportY(), true, true);
					AddDLLMessage(eNewValue, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_MAJOR_EVENT,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getViewportX(),getViewportY(), true, true);
				}
			}

			szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_REVOLTS_JOINS", pOldCity->getNameKey(), GET_PLAYER(eNewValue).getCivilizationDescriptionKey());
			GC.getGame().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getOwner(), szBuffer, getX(), getY(), (ColorTypes)GC.getInfoTypeForString("COLOR_ALT_HIGHLIGHT_TEXT"));

			FAssertMsg(pOldCity->getOwner() != eNewValue, "pOldCity->getOwner() is not expected to be equal with eNewValue");
			GET_PLAYER(eNewValue).acquireCity(pOldCity, false, false, bUpdatePlotGroup); // will delete the pointer
			pOldCity = NULL;
			pNewCity = getPlotCity();
			FAssertMsg(pNewCity != NULL, "NewCity is not assigned a valid value");

			if (pNewCity != NULL)
			{
				CLinkList<IDInfo> oldUnits;

				pUnitNode = headUnitNode();

				while (pUnitNode != NULL)
				{
					oldUnits.insertAtEnd(pUnitNode->m_data);
					pUnitNode = nextUnitNode(pUnitNode);
				}

				pUnitNode = oldUnits.head();

				while (pUnitNode != NULL)
				{
					pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = oldUnits.next(pUnitNode);

					if (pLoopUnit)
					{
						if (pLoopUnit->isEnemy(GET_PLAYER(eNewValue).getTeam()))
						{
							FAssert(pLoopUnit->getTeam() != GET_PLAYER(eNewValue).getTeam());
							pLoopUnit->jumpToNearestValidPlot();
						}
					}
				}

				int iDummyValue;

				eBestUnit = pNewCity->AI_bestUnitAI(UNITAI_CITY_DEFENSE, iDummyValue);

				if (eBestUnit == NO_UNIT)
				{
					eBestUnit = pNewCity->AI_bestUnitAI(UNITAI_ATTACK, iDummyValue);
				}

				if (eBestUnit != NO_UNIT)
				{
					iFreeUnits = (GC.getDefineINT("BASE_REVOLT_FREE_UNITS") + ((pNewCity->getHighestPopulation() * GC.getDefineINT("REVOLT_FREE_UNITS_PERCENT")) / 100));

					for (iI = 0; iI < iFreeUnits; ++iI)
					{
						GET_PLAYER(eNewValue).initUnit(eBestUnit, getX(), getY(), UNITAI_CITY_DEFENSE, NO_DIRECTION, GC.getGame().getSorenRandNum(10000, "AI Unit Birthmark"));
					}
				}
			}
		}
		else
		{
			setOwnershipDuration(0);

			if (isOwned())
			{
				changeAdjacentSight(getTeam(), GC.getDefineINT("PLOT_VISIBILITY_RANGE"), false, NULL, bUpdatePlotGroup);

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
					// Super Forts begin *culture* - adapted to C2C
					if (GC.getImprovementInfo(getImprovementType()).getCulture() > 0 )
					{
						changeCultureRangeFortsWithinRange(getOwner(), -1, GC.getImprovementInfo(getImprovementType()).getCultureRange(), false);
					}
					// Super Forts end
				}

				updatePlotGroupBonus(false);
			}

			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

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
#ifdef OLD_VERSION_PRE_SUPER_FORTS
/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			/* if the plot contains fort, its influence (actsAsCity) must be transferred to the new owner */
			if (isActsAsCity())
			{
				if (getOwner() != NO_PLAYER)
				{
					changeActsAsCity(getOwner(), -1);
				}
				if (eNewValue != NO_PLAYER)
				{
					changeActsAsCity(eNewValue, 1);
				}
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
#endif

			m_eOwner = eNewValue;

			setWorkingCityOverride(NULL);
			updateWorkingCity();

			if (isOwned())
			{
				changeAdjacentSight(getTeam(), GC.getDefineINT("PLOT_VISIBILITY_RANGE"), true, NULL, bUpdatePlotGroup);

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
					// Super Forts begin *culture* - adapted to C2C
					if (GC.getImprovementInfo(getImprovementType()).getCulture() > 0)
					{
						changeCultureRangeFortsWithinRange(getOwner(), 1, GC.getImprovementInfo(getImprovementType()).getCultureRange(), true);
					}
					// Super Forts end
				}

				updatePlotGroupBonus(true);
			}

			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->getTeam() != getTeam() && (getTeam() == NO_TEAM || !GET_TEAM(getTeam()).isVassal(pLoopUnit->getTeam())))
				{
					GET_PLAYER(pLoopUnit->getOwner()).changeNumOutsideUnits(1);
				}
			}

			for (iI = 0; iI < MAX_TEAMS; ++iI)
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

			if (isOwned())
			{
				if (isGoody())
				{
					GET_PLAYER(getOwner()).doGoody(this, NULL);
				}

				for (iI = 0; iI < MAX_PC_TEAMS; ++iI)
				{
					if (GET_TEAM((TeamTypes)iI).isAlive())
					{
						if (isVisible((TeamTypes)iI, false))
						{
							GET_TEAM((TeamTypes)iI).meet(getTeam(), true);
						}
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


/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
		// Plot danger cache
		CvPlot* pLoopPlot;
		for (int iDX = -(DANGER_RANGE); iDX <= DANGER_RANGE; iDX++)
		{
			for (int iDY = -(DANGER_RANGE); iDY <= (DANGER_RANGE); iDY++)
			{
				pLoopPlot	= plotXY(getX(), getY(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->invalidateIsTeamBorderCache();
				}
			}
		}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

		updateSymbols();
	}
}


PlotTypes CvPlot::getPlotType() const
{
	return (PlotTypes)m_ePlotType;
}


bool CvPlot::isWaterExternal() const
{
	return (getPlotType() == PLOT_OCEAN && (!GC.viewportsEnabled() || isRevealed(GC.getGame().getActiveTeam(), true) || isAdjacentRevealed(GC.getGame().getActiveTeam(), true)));
}

bool CvPlot::isWater() const
{
	return (getPlotType() == PLOT_OCEAN);
}


bool CvPlot::isFlatlands() const
{
	return (getPlotType() == PLOT_LAND);
}


bool CvPlot::isHills() const
{
	return (getPlotType() == PLOT_HILLS);
}


bool CvPlot::isPeak() const
{
	return (isPeak2(false));
}

bool CvPlot::isPeak2(bool bCountsAs) const
{
	if (bCountsAs && getFeatureType() != NO_FEATURE)
	{
		return (getPlotType() == PLOT_PEAK || GC.getFeatureInfo(getFeatureType()).isCountsAsPeak());
	}
	return (getPlotType() == PLOT_PEAK);
}


void CvPlot::setPlotType(PlotTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	CvArea* pNewArea;
	CvArea* pCurrArea;
	CvArea* pLastArea;
	CvPlot* pLoopPlot;
	bool bWasWater;
	bool bRecalculateAreas;
	int iAreaCount;
	int iI;
	PlotTypes eOldPlotType = getPlotType();

	if (eOldPlotType != eNewValue)
	{
		if ((eOldPlotType == PLOT_OCEAN) || (eNewValue == PLOT_OCEAN))
		{
			erase();
		}

		bWasWater = isWater();

		updateSeeFromSight(false, true);

		if ( eOldPlotType != NO_PLOT )
		{
			m_movementCharacteristicsHash ^= g_plotTypeZobristHashes[eOldPlotType];
		}
		if ( eNewValue != NO_PLOT )
		{
			m_movementCharacteristicsHash ^= g_plotTypeZobristHashes[eNewValue];
		}

		m_ePlotType = eNewValue;

		updateYield();
		updatePlotGroup();

		updateSeeFromSight(true, true);

		if ((getTerrainType() == NO_TERRAIN) || (GC.getTerrainInfo(getTerrainType()).isWater() != isWater()))
		{
			if (isWater())
			{
				if (isAdjacentToLand())
				{
					setTerrainType(((TerrainTypes)(GC.getDefineINT("SHALLOW_WATER_TERRAIN"))), bRecalculate, bRebuildGraphics);
				}
				else
				{
					setTerrainType(((TerrainTypes)(GC.getDefineINT("DEEP_WATER_TERRAIN"))), bRecalculate, bRebuildGraphics);
				}
			}
			else
			{
				TerrainTypes eTerrain = (TerrainTypes)GC.getDefineINT("LAND_TERRAIN");

#ifdef EXPERIMENTAL_FEATURE_ON_PEAK
				if ( eNewValue == PLOT_PEAK )
				{
					TerrainTypes eTerrainPeak = (TerrainTypes)GC.getInfoTypeForString("TERRAIN_PEAK");
					if ( eTerrainPeak != -1 )
					{
						eTerrain = eTerrainPeak;
					}
				}
#endif
				setTerrainType(eTerrain, bRecalculate, bRebuildGraphics);
			}
		}

		GC.getMap().resetPathDistance();

		if (bWasWater != isWater())
		{
			if (bRecalculate)
			{
				for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->isWater())
						{
							if (pLoopPlot->isAdjacentToLand())
							{
								pLoopPlot->setTerrainType(((TerrainTypes)(GC.getDefineINT("SHALLOW_WATER_TERRAIN"))), bRecalculate, bRebuildGraphics);
							}
							else
							{
								pLoopPlot->setTerrainType(((TerrainTypes)(GC.getDefineINT("DEEP_WATER_TERRAIN"))), bRecalculate, bRebuildGraphics);
							}
						}
					}
				}
			}

			for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updateYield();
					pLoopPlot->updatePlotGroup();
				}
			}

			for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				pLoopPlot = plotCity(getX(), getY(), iI);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->updatePotentialCityWork();
				}
			}

			GC.getMap().changeLandPlots((isWater()) ? -1 : 1);

			if (getBonusType() != NO_BONUS)
			{
				GC.getMap().changeNumBonusesOnLand(getBonusType(), ((isWater()) ? -1 : 1));
			}

			if (isOwned())
			{
				GET_PLAYER(getOwner()).changeTotalLand((isWater()) ? -1 : 1);
				GET_TEAM(getTeam()).changeTotalLand((isWater()) ? -1 : 1);
			}

			if (bRecalculate)
			{
				pNewArea = NULL;
				bRecalculateAreas = false;

				// XXX might want to change this if we allow diagonal water movement...
				if (isWater())
				{
					for (iI = 0; iI < NUM_CARDINALDIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotCardinalDirection(getX(), getY(), ((CardinalDirectionTypes)iI));

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->area()->isWater())
							{
								if (pNewArea == NULL)
								{
									pNewArea = pLoopPlot->area();
								}
								else if (pNewArea != pLoopPlot->area())
								{
									bRecalculateAreas = true;
									break;
								}
							}
						}
					}
				}
				else
				{
					for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

						if (pLoopPlot != NULL)
						{
							if (!(pLoopPlot->area()->isWater()))
							{
								if (pNewArea == NULL)
								{
									pNewArea = pLoopPlot->area();
								}
								else if (pNewArea != pLoopPlot->area())
								{
									bRecalculateAreas = true;
									break;
								}
							}
						}
					}
				}

				if (!bRecalculateAreas)
				{
					pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)(NUM_DIRECTION_TYPES - 1)));

					if (pLoopPlot != NULL)
					{
						pLastArea = pLoopPlot->area();
					}
					else
					{
						pLastArea = NULL;
					}

					iAreaCount = 0;

					for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

						if (pLoopPlot != NULL)
						{
							pCurrArea = pLoopPlot->area();
						}
						else
						{
							pCurrArea = NULL;
						}

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

				if (bRecalculateAreas)
				{
					GC.getMap().recalculateAreas();
				}
				else
				{
					setArea(FFreeList::INVALID_INDEX);

					if ((area() != NULL) && (area()->getNumTiles() == 1))
					{
						GC.getMap().deleteArea(getArea());
					}

					if (pNewArea == NULL)
					{
						pNewArea = GC.getMap().addArea();
						pNewArea->init(pNewArea->getID(), isWater());
					}

					setArea(pNewArea->getID());
				}
			}
		}

		if (bRebuildGraphics && GC.IsGraphicsInitialized())
		{
			if ( shouldHaveGraphics() )
			{
				//Update terrain graphical
				gDLL->getEngineIFace()->RebuildPlot(getViewportX(), getViewportY(), true, true);
				//gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true); //minimap does a partial update
				//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);

				updateFeatureSymbol();
				setLayoutDirty(true);
				updateRouteSymbol(false, true);
				updateRiverSymbol(false, true);
			}
		}
	}
}


TerrainTypes CvPlot::getTerrainTypeExternal() const
{
#ifdef EXTREME_PAGING
	if ( !shouldHaveFullGraphics() )
	{
		if ( isWater() )
		{
			return (TerrainTypes)GC.getInfoTypeForString("TERRAIN_OCEAN");
		}
		else
		{
			return (TerrainTypes)GC.getInfoTypeForString("TERRAIN_GRASSLAND");
		}
	}
	else
#endif
	{
		if ( !GC.viewportsEnabled() || isRevealed(GC.getGame().getActiveTeam(), true) )
		{
			return (TerrainTypes)m_eTerrainType;
		}
		else
		{
			return (TerrainTypes)GC.getInfoTypeForString("TERRAIN_NONE");
		}
	}
}

TerrainTypes CvPlot::getTerrainType() const
{
	return (TerrainTypes)m_eTerrainType;
}


void CvPlot::setTerrainType(TerrainTypes eNewValue, bool bRecalculate, bool bRebuildGraphics)
{
	bool bUpdateSight;
	TerrainTypes eOldTerrain = getTerrainType();

	if ( eOldTerrain != eNewValue)
	{
		if ((getTerrainType() != NO_TERRAIN) &&
			  (eNewValue != NO_TERRAIN) &&
			  ((GC.getTerrainInfo(getTerrainType()).getSeeFromLevel() != GC.getTerrainInfo(eNewValue).getSeeFromLevel()) ||
				 (GC.getTerrainInfo(getTerrainType()).getSeeThroughLevel() != GC.getTerrainInfo(eNewValue).getSeeThroughLevel())))
		{
			bUpdateSight = true;
		}
		else
		{
			bUpdateSight = false;
		}

		if (bUpdateSight)
		{
			updateSeeFromSight(false, true);
		}

		if ( eOldTerrain != NO_TERRAIN )
		{
			m_movementCharacteristicsHash ^= GC.getTerrainInfo(eOldTerrain).getZobristValue();
		}
		if ( eNewValue != NO_TERRAIN )
		{
			m_movementCharacteristicsHash ^= GC.getTerrainInfo(eNewValue).getZobristValue();
		}

		m_eTerrainType = eNewValue;

		updateYield();
		updatePlotGroup();

		if (bUpdateSight)
		{
			updateSeeFromSight(true, true);
		}

		if (bRebuildGraphics)
		{
			if ( shouldHaveGraphics() )
			{
				//Update terrain graphics
				gDLL->getEngineIFace()->RebuildPlot(getViewportX(), getViewportY(),false,true);
				//gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true); //minimap does a partial update
				//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
			}
		}

		if (GC.getTerrainInfo(getTerrainType()).isWater() != isWater())
		{
			setPlotType(((GC.getTerrainInfo(getTerrainType()).isWater()) ? PLOT_OCEAN : PLOT_LAND), bRecalculate, bRebuildGraphics);
		}
	}
}


FeatureTypes CvPlot::getFeatureTypeExternal() const
{
	if ( !GC.viewportsEnabled() || isRevealed(GC.getGame().getActiveTeam(), true) )
	{
		return (FeatureTypes)m_eFeatureType;
	}
	else
	{
		return NO_FEATURE;
	}
}

FeatureTypes CvPlot::getFeatureType() const
{
	return (FeatureTypes)m_eFeatureType;
}


void CvPlot::setFeatureType(FeatureTypes eNewValue, int iVariety, bool bImprovementSet)
{
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
	if (GC.getGame().isOption(GAMEOPTION_PERSONALIZED_MAP))
	{
		if (getOwner() != NO_PLAYER
		&& (eNewValue == NO_FEATURE || GC.getFeatureInfo(eNewValue).getHealthPercent() < 0)
		&& (getLandmarkType() == LANDMARK_FOREST || getLandmarkType() == LANDMARK_JUNGLE))
		{
			for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);
				if (pLoopPlot != NULL)
				{
					CvCity* pLoopCity = pLoopPlot->getPlotCity();
					if (pLoopCity != NULL && pLoopCity->getOwner() == getOwner())
					{
						pLoopCity->changeLandmarkAngerTimer(GC.getDefineINT("LANDMARK_ANGER_DIVISOR") * 2);
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
		updateYield();
		updateFeatureSymbol();

		if (eOldFeature != NO_FEATURE && GC.getFeatureInfo(eOldFeature).getArtInfo()->isRiverArt()
		|| getFeatureType() != NO_FEATURE && GC.getFeatureInfo(getFeatureType()).getArtInfo()->isRiverArt())
		{
			updateRiverSymbolArt(true);
		}

		for (int iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);
			if (pLoopPlot != NULL)
			{
				CvCity* pLoopCity = pLoopPlot->getPlotCity();
				if (pLoopCity != NULL)
				{
					pLoopCity->updateFeatureHealth();
					pLoopCity->updateFeatureHappiness();
				}
			}
		}

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
			CLLNode<IDInfo>* pUnitNode;
			CvUnit* pLoopUnit;
			pUnitNode = headUnitNode();
			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
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
	if (eTeam != NO_TEAM)
	{
		if (m_eBonusType != NO_BONUS)
		{
			if (!GET_TEAM(eTeam).isHasTech((TechTypes)(GC.getBonusInfo((BonusTypes)m_eBonusType).getTechReveal())) && !GET_TEAM(eTeam).isForceRevealedBonus((BonusTypes)m_eBonusType))
			{
				return NO_BONUS;
			}
		}
	}

	return (BonusTypes)m_eBonusType;
}


BonusTypes CvPlot::getNonObsoleteBonusType(TeamTypes eTeam) const
{
	FAssert(eTeam != NO_TEAM);

	BonusTypes eBonus = getBonusType(eTeam);
	if (eBonus != NO_BONUS)
	{
		if (GET_TEAM(eTeam).isBonusObsolete(eBonus))
		{
			return NO_BONUS;
		}
	}

	return eBonus;
}


void CvPlot::setBonusType(BonusTypes eNewValue)
{
	if (getBonusType() != eNewValue)
	{
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
		if (GC.getImprovementInfo(eImprovement).getCulture() > 0 || GC.getImprovementInfo(eImprovement).isActsAsCity())
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


void CvPlot::setImprovementType(ImprovementTypes eNewValue)
{
	int iI;
	ImprovementTypes eOldImprovement = getImprovementType();
	bool bUpgrade = false;

	if (getImprovementType() != eNewValue)
	{
		if (getImprovementType() != NO_IMPROVEMENT && eNewValue != NO_IMPROVEMENT)
		{
			//Charge for Upgrade but be careful not to charge for a downgrade.
			if ((ImprovementTypes)GC.getImprovementInfo(getImprovementType()).getImprovementUpgrade() == eNewValue)
			{
				bUpgrade = true;
			}
			if (!bUpgrade && GC.getImprovementInfo(getImprovementType()).getNumAlternativeImprovementUpgradeTypes() > 0)
			{
				for (iI = 0; iI < GC.getImprovementInfo(getImprovementType()).getNumAlternativeImprovementUpgradeTypes(); iI++)
				{
					if ((ImprovementTypes)GC.getImprovementInfo(getImprovementType()).getAlternativeImprovementUpgradeType(iI) == eNewValue)
					{
						bUpgrade = true;
					}
				}
			}
			if (bUpgrade)
			{
				GET_PLAYER(getOwner()).changeGold(-GC.getImprovementInfo(eNewValue).getHighestCost());
			}

			if (area())
			{
				area()->changeNumImprovements(getImprovementType(), -1);
			}
			if (isOwned())
			{
				GET_PLAYER(getOwner()).changeImprovementCount(getImprovementType(), -1);
				// Super Forts begin *culture* - adapted to C2C
				if (GC.getImprovementInfo(getImprovementType()).getCulture() > 0)
				{
					changeCultureRangeFortsWithinRange(getOwner(), -1, GC.getImprovementInfo(getImprovementType()).getCultureRange(), true);
				}
				// Super Forts end
			}
		}
		if (eNewValue != NO_IMPROVEMENT)
		{
			for (iI = 0; iI < GC.getImprovementInfo(eNewValue).getNumFeatureChangeTypes(); iI++)
			{
				FeatureTypes eAddingFeature = (FeatureTypes)GC.getImprovementInfo(eNewValue).getFeatureChangeType(iI);
				if (canHaveFeature(eAddingFeature, true))
				{
						setFeatureType(eAddingFeature, -1, true);
						break;
				}
			}
			if (GC.getImprovementInfo(eNewValue).getBonusChange() != NO_BONUS)
			{
				BonusTypes eAddingBonus = (BonusTypes)GC.getImprovementInfo(eNewValue).getBonusChange();
				if (canHaveBonus(eAddingBonus, false))
				{
					setBonusType(eAddingBonus);
				}
			}
		}

		// Super Forts begin *vision*
		updateSight(false, true);
		// Super Forts end
#ifdef OLD_VERSION_PRE_SUPER_FORTS
/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		bool oldActAsCity = (eOldImprovement != NO_IMPROVEMENT) && GC.getImprovementInfo(eOldImprovement).isActsAsCity();
		bool newActAsCity = (eNewValue != NO_IMPROVEMENT) && GC.getImprovementInfo(eNewValue).isActsAsCity();

		if (isOwned())
		{
			/* fort removed, pillaged, destroyed */
			if (oldActAsCity && !newActAsCity)
			{
				changeActsAsCity(getOwner(), -1);
			}

			/* fort created */
			if (!oldActAsCity && newActAsCity)
			{
				changeActsAsCity(getOwner(), 1);
			}
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
#endif
		if (eNewValue != NO_IMPROVEMENT && GC.getImprovementInfo(eNewValue).isChangeRemove())
		{
			eNewValue = NO_IMPROVEMENT;
		}
		if ( isOwned() )
		{
			GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();
			updatePlotGroupBonus(false);
		}
		m_eImprovementType = eNewValue;
		if ( isOwned() )
		{
			updatePlotGroupBonus(true);
			GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();
		}

		if (getImprovementType() == NO_IMPROVEMENT)
		{
			setImprovementDuration(0);
		}

		setUpgradeProgressHundredths(0);
		setPlotIgnoringImprovementUpgrade(false);

		for (iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (isVisible((TeamTypes)iI, false))
				{
					setRevealedImprovementType((TeamTypes)iI, getImprovementType());
				}
			}
		}

		if (getImprovementType() != NO_IMPROVEMENT)
		{
			if (area())
			{
				area()->changeNumImprovements(getImprovementType(), 1);
			}
			if (isOwned())
			{
				GET_PLAYER(getOwner()).changeImprovementCount(getImprovementType(), 1);
				// Super Forts begin *culture* - adapted to C2C
				if (GC.getImprovementInfo(getImprovementType()).getCulture() > 0)
				{
					changeCultureRangeFortsWithinRange(getOwner(), 1, GC.getImprovementInfo(getImprovementType()).getCultureRange(), true);
				}
				// Super Forts end
			}
		}
		// Super Forts begin *vision*
		updateSight(true, true);
		// Super Forts end

		if ( (getImprovementType() == NO_IMPROVEMENT) != (eOldImprovement == NO_IMPROVEMENT) &&
			 getBonusType() != NO_BONUS)
		{
			//	Newly added or removed improvement on a bonus - add/remove Zobrist contributuns
			//	for all local plot groups
			ToggleInPlotGroupsZobristContributors();
		}

		updateIrrigated();
		updateYield();

		for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			CvPlot* pLoopPlot = plotCity(getX(), getY(), iI);

			if (pLoopPlot != NULL)
			{
				CvCity* pLoopCity = pLoopPlot->getPlotCity();

				if (pLoopCity != NULL)
				{
					pLoopCity->updateFeatureHappiness();
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/19/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
					pLoopCity->updateImprovementHealth();
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/

					//	Changed improvement status might change city best build opinions
					pLoopCity->AI_markBestBuildValuesStale();
				}
			}
		}

		// Building or removing a fort will now force a plotgroup update to verify resource connections.
		if ( (NO_IMPROVEMENT != getImprovementType() && GC.getImprovementInfo(getImprovementType()).isUniversalTradeBonusProvider()) !=
			 (NO_IMPROVEMENT != eOldImprovement && GC.getImprovementInfo(eOldImprovement).isUniversalTradeBonusProvider()) )
		{
			updatePlotGroup();
		}

		if (NO_IMPROVEMENT != eOldImprovement && GC.getImprovementInfo(eOldImprovement).isActsAsCity())
		{
			verifyUnitValidPlot();
		}

		if (GC.getGame().isDebugMode())
		{
			setLayoutDirty(true);
		}

		if (getImprovementType() != NO_IMPROVEMENT)
		{
			CvEventReporter::getInstance().improvementBuilt(getImprovementType(), getX(), getY());
		}

		if (getImprovementType() == NO_IMPROVEMENT)
		{
			CvEventReporter::getInstance().improvementDestroyed(eOldImprovement, getOwner(), getX(), getY());
		}

		CvCity* pWorkingCity = getWorkingCity();
		if (NULL != pWorkingCity)
		{
			if ((NO_IMPROVEMENT != eNewValue && pWorkingCity->getImprovementFreeSpecialists(eNewValue) > 0)	||
				(NO_IMPROVEMENT != eOldImprovement && pWorkingCity->getImprovementFreeSpecialists(eOldImprovement) > 0))
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
	bool bOldRoute;
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool bOldSeaRoute;
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
	int iI;

	RouteTypes eOldRoute = getRouteType();

	if (eOldRoute != eNewValue)
	{
		bOldRoute = isRoute(); // XXX is this right???
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		bOldSeaRoute = isCanMoveLandUnits();
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/

		if ( isOwned() )
		{
			GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();
			updatePlotGroupBonus(false);
		}

		if ( eOldRoute != NO_ROUTE )
		{
			m_movementCharacteristicsHash ^= GC.getRouteInfo(eOldRoute).getZobristValue();
		}
		if ( eNewValue != NO_ROUTE )
		{
			m_movementCharacteristicsHash ^= GC.getRouteInfo(eNewValue).getZobristValue();
		}

		m_eRouteType = eNewValue;
		if ( isOwned() )
		{
			updatePlotGroupBonus(true);
			GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();
		}

		for (iI = 0; iI < MAX_TEAMS; ++iI)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (isVisible((TeamTypes)iI, false))
				{
					setRevealedRouteType((TeamTypes)iI, getRouteType());
				}
			}
		}

		updateYield();

		if (bUpdatePlotGroups)
		{
			if (bOldRoute != isRoute())
			{
				updatePlotGroup();
			}
		}

		if (GC.getGame().isDebugMode())
		{
			updateRouteSymbol(true, true);
		}

		if (getRouteType() != NO_ROUTE)
		{
			CvEventReporter::getInstance().routeBuilt(getRouteType(), getX(), getY());
		}
		// < M.A.D. Nukes Start >
		else
		{
			setLayoutDirty(true);
		}
		// < M.A.D. Nukes End   >
/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 07/07/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		if (eNewValue == NO_ROUTE && bOldSeaRoute)
		{
			CLLNode<IDInfo>* pUnitNode;
			CvUnit* pLoopUnit;
			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);

				if (pLoopUnit->getDomainType() == DOMAIN_LAND)
				{
					if (!pLoopUnit->isCargo() && !pLoopUnit->canMoveAllTerrain())
					{
						pLoopUnit->kill(true);//MIGHT need to also specify if the unit isn't cargo of a land unit that isn't cargo itself.  Shouldn't have to though because if the carrier would die, then that would dunk the carrier's units, likely in a fatal manner.
					}
				}
			}
		}
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
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
	CvPlotGroup* pPlotGroup;
	CvPlot* pLoopPlot;
	int iI;

	if (getPlotCity() != pNewValue)
	{
		if (isCity())
		{
			for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				pLoopPlot = plotCity(getX(), getY(), iI);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->changeCityRadiusCount(-1);
					pLoopPlot->changePlayerCityRadiusCount(getPlotCity()->getOwner(), -1);
				}
			}
		}

		GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();

		updatePlotGroupBonus(false);
		if (isCity())
		{
			pPlotGroup = getPlotGroup(getOwner());

			if (pPlotGroup != NULL)
			{
				FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::setPlotCity");
				for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					getPlotCity()->changeNumBonuses(((BonusTypes)iI), -(pPlotGroup->getNumBonuses((BonusTypes)iI)));
				}
			}
		}
		if (pNewValue != NULL)
		{
			m_plotCity = pNewValue->getIDInfo();
		}
		else
		{
			m_plotCity.reset();
		}
		if (isCity())
		{
			pPlotGroup = getPlotGroup(getOwner());

			if (pPlotGroup != NULL)
			{
				FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::setPlotCity");
				for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
				{
					getPlotCity()->changeNumBonuses(((BonusTypes)iI), pPlotGroup->getNumBonuses((BonusTypes)iI));
				}
			}
		}
		updatePlotGroupBonus(true);

		GET_PLAYER(getOwner()).endDeferredPlotGroupBonusCalculation();

		if (isCity())
		{
			for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
			{
				pLoopPlot = plotCity(getX(), getY(), iI);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->changeCityRadiusCount(1);
					pLoopPlot->changePlayerCityRadiusCount(getPlotCity()->getOwner(), 1);
				}
			}
		}

		updateIrrigated();
		updateYield();

		updateMinimapColor();
	}
}


CvCity* CvPlot::getWorkingCity() const
{
	return getCity(m_workingCity);
}


void CvPlot::updateWorkingCity()
{
	PROFILE_FUNC();

	CvCity* pOldWorkingCity;
	CvCity* pLoopCity;
	CvCity* pBestCity;
	CvPlot* pLoopPlot;
	int iBestPlot;
	int iI;

	pBestCity = getPlotCity();

	if (pBestCity == NULL)
	{
		pBestCity = getWorkingCityOverride();
		FAssertMsg((pBestCity == NULL) || (pBestCity->getOwner() == getOwner()), "pBest city is expected to either be NULL or the current plot instance's");
	}

	if ((pBestCity == NULL) && isOwned())
	{
		iBestPlot = 0;

		for (iI = 0; iI < NUM_CITY_PLOTS; ++iI)
		{
			pLoopPlot = plotCity(getX(), getY(), iI);

			if (pLoopPlot != NULL)
			{
				pLoopCity = pLoopPlot->getPlotCity();

				if (pLoopCity != NULL)
				{
					if (pLoopCity->getOwner() == getOwner())
					{
						// XXX use getGameTurnAcquired() instead???
						if ((pBestCity == NULL) ||
							  (GC.getCityPlotPriority()[iI] < GC.getCityPlotPriority()[iBestPlot]) ||
							  ((GC.getCityPlotPriority()[iI] == GC.getCityPlotPriority()[iBestPlot]) &&
							   ((pLoopCity->getGameTurnFounded() < pBestCity->getGameTurnFounded()) ||
								((pLoopCity->getGameTurnFounded() == pBestCity->getGameTurnFounded()) &&
								 (pLoopCity->getID() < pBestCity->getID())))))
						{
							iBestPlot = iI;
							pBestCity = pLoopCity;
						}
					}
				}
			}
		}
	}

	pOldWorkingCity = getWorkingCity();

	if (pOldWorkingCity != pBestCity)
	{
		if (pOldWorkingCity != NULL)
		{
			pOldWorkingCity->setWorkingPlot(this, false);
		}

		if (pBestCity != NULL)
		{
			FAssertMsg(isOwned(), "isOwned is expected to be true");
			FAssertMsg(!isBeingWorked(), "isBeingWorked did not return false as expected");
			m_workingCity = pBestCity->getIDInfo();
		}
		else
		{
			m_workingCity.reset();
		}

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

		if (getOwner() == GC.getGame().getActivePlayer())
		{
			if (gDLL->getGraphicOption(GRAPHICOPTION_CITY_RADIUS))
			{
				if (gDLL->getInterfaceIFace()->canSelectionListFound())
				{
					gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
				}
			}
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
	m_iReconCount = (m_iReconCount + iChange);
	FAssert(getReconCount() >= 0);
}


int CvPlot::getRiverCrossingCount() const
{
	return m_iRiverCrossingCount;
}


void CvPlot::changeRiverCrossingCount(int iChange)
{
	m_iRiverCrossingCount = (m_iRiverCrossingCount + iChange);
	FAssert(getRiverCrossingCount() >= 0);
}


short* CvPlot::getYield()
{
	return m_aiYield;
}


int CvPlot::getYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiYield[eIndex];
}


int CvPlot::calculateNatureYield(YieldTypes eYield, TeamTypes eTeam, bool bIgnoreFeature) const
{
	PROFILE_FUNC();

	BonusTypes eBonus;
	int iYield;

/************************************************************************************************/
/* Afforess	Mountains Start		 08/03/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/

	if (isImpassable(getTeam()))
	{
		if (isPeak2(true))
		{
			if (eTeam != NO_TEAM && !isRoute())
			{// It makes sense to only require a route for a city to work the peak even if the player doesn't have the mounteneering tech.
			// Perhaps add a landslide event that destroys route and improvement on peaks, so that players without the mountaneering tech
			// can't rebuild easily mountain improvements conquered from a more advanced civilization.
				return 0;
			}
		}
		else
		{
			return 0;
		}
	}
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/


	FAssertMsg(getTerrainType() != NO_TERRAIN, "TerrainType is not assigned a valid value");
/************************************************************************************************/
/* Afforess	                  Start		 08/29/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (isPeak2(true)) iYield = 0; else
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	iYield = GC.getTerrainInfo(getTerrainType()).getYield(eYield);

	if (isHills())
	{
		iYield += GC.getYieldInfo(eYield).getHillsChange();
	}

	if (isPeak2(true))
	{
		iYield += GC.getYieldInfo(eYield).getPeakChange();
	}

	if (isLake())
	{
		iYield += GC.getYieldInfo(eYield).getLakeChange();
	}

	if (eTeam != NO_TEAM)
	{
		eBonus = getBonusType(eTeam);

		if (eBonus != NO_BONUS)
		{
			iYield += GC.getBonusInfo(eBonus).getYieldChange(eYield);
		}
	}

	if (isRiver())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType()).getRiverYieldChange(eYield) : GC.getFeatureInfo(getFeatureType()).getRiverYieldChange(eYield));
	}

	if (isHills())
	{
		iYield += ((bIgnoreFeature || (getFeatureType() == NO_FEATURE)) ? GC.getTerrainInfo(getTerrainType()).getHillsYieldChange(eYield) : GC.getFeatureInfo(getFeatureType()).getHillsYieldChange(eYield));
	}

	if (!bIgnoreFeature)
	{
		if (getFeatureType() != NO_FEATURE)
		{
			iYield += GC.getFeatureInfo(getFeatureType()).getYieldChange(eYield);
		}
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

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/06/09                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
int CvPlot::calculateImprovementYieldChange(ImprovementTypes eImprovement, YieldTypes eYield, PlayerTypes ePlayer, bool bOptimal, bool bBestRoute) const
{
	PROFILE_FUNC();

	BonusTypes eBonus;
	int iBestYield;
	int iYield;
	int iI;

/************************************************************************************************/
/* Afforess	                  Start		 06/24/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	//TB note: currently returns nothing but false so is wasted evaluation
	//if (isOvergrown())
	//{
	//	return 0;
	//}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

	iYield = GC.getImprovementInfo(eImprovement).getYieldChange(eYield);

	if (isRiverSide())
	{
		iYield += GC.getImprovementInfo(eImprovement).getRiverSideYieldChange(eYield);
	}

	if (isHills())
	{
		iYield += GC.getImprovementInfo(eImprovement).getHillsYieldChange(eYield);
	}

	if ((bOptimal) ? true : isIrrigationAvailable())
	{
		iYield += GC.getImprovementInfo(eImprovement).getIrrigatedYieldChange(eYield);
	}

	if (bOptimal)
	{
		iBestYield = 0;

		for (iI = 0; iI < GC.getNumRouteInfos(); ++iI)
		{
			iBestYield = std::max(iBestYield, GC.getImprovementInfo(eImprovement).getRouteYieldChanges(iI, eYield));
		}

		iYield += iBestYield;
	}
	else
	{
		RouteTypes eRoute = getRouteType();

		if( bBestRoute && ePlayer != NO_PLAYER )
		{
			eRoute = GET_PLAYER(ePlayer).getBestRoute(GC.getMap().plotSorenINLINE(getX(), getY()));
		}

		if (eRoute != NO_ROUTE)
		{
			iYield += GC.getImprovementInfo(eImprovement).getRouteYieldChanges(eRoute, eYield);
		}
	}

	if (bOptimal || ePlayer == NO_PLAYER)
	{
		for (iI = 0; iI < GC.getNumTechInfos(); ++iI)
		{
			iYield += GC.getImprovementInfo(eImprovement).getTechYieldChanges(iI, eYield);
		}

		for (iI = 0; iI < GC.getNumCivicInfos(); ++iI)
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
		eBonus = getBonusType(GET_PLAYER(ePlayer).getTeam());

		if (eBonus != NO_BONUS)
		{
			iYield += GC.getImprovementInfo(eImprovement).getImprovementBonusYield(eBonus, eYield);
		}
	}

/*************************************************************************************************/
/* UNOFFICIAL_PATCH                       06/02/10                     Afforess & jdog5000       */
/*                                                                                               */
/* Bugfix                                                                                        */
/*************************************************************************************************/
/* original bts code
	return iYield;
*/
	// Improvement cannot actually produce negative yield
	int iCurrYield = calculateNatureYield(eYield, (ePlayer == NO_PLAYER) ? NO_TEAM : GET_PLAYER(ePlayer).getTeam(), bOptimal);

	return std::max( -iCurrYield, iYield );
/*************************************************************************************************/
/* UNOFFICIAL_PATCH                         END                                                  */
/*************************************************************************************************/
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

int CvPlot::calculateYield(YieldTypes eYield, bool bDisplay) const
{
	PROFILE_FUNC();

	CvCity* pCity;
	CvCity* pWorkingCity;
	ImprovementTypes eImprovement;
	RouteTypes eRoute;
	PlayerTypes ePlayer;
	bool bCity;
	int iYield;

	if (bDisplay && GC.getGame().isDebugMode())
	{
		return getYield(eYield);
	}

	if (getTerrainType() == NO_TERRAIN)
	{
		return 0;
	}

	if (!isPotentialCityWork())
	{
		return 0;
	}

	bCity = false;

	if (bDisplay)
	{
		ePlayer = getRevealedOwner(GC.getGame().getActiveTeam(), false);
		eImprovement = getRevealedImprovementType(GC.getGame().getActiveTeam(), false);
		eRoute = getRevealedRouteType(GC.getGame().getActiveTeam(), false);

		if (ePlayer == NO_PLAYER)
		{
			ePlayer = GC.getGame().getActivePlayer();
		}
	}
	else
	{
		ePlayer = getOwner();
		eImprovement = getImprovementType();
		eRoute = getRouteType();
	}

	iYield = calculateNatureYield(eYield, ((ePlayer != NO_PLAYER) ? GET_PLAYER(ePlayer).getTeam() : NO_TEAM));

	if (eImprovement != NO_IMPROVEMENT)
	{
		iYield += calculateImprovementYieldChange(eImprovement, eYield, ePlayer);
	}

	if (eRoute != NO_ROUTE)
	{
		iYield += GC.getRouteInfo(eRoute).getYieldChange(eYield);
	}

	if (ePlayer != NO_PLAYER)
	{
		pCity = getPlotCity();

		if (pCity != NULL)
		{
			if (!bDisplay || pCity->isRevealed(GC.getGame().getActiveTeam(), false))
			{
				iYield += GC.getYieldInfo(eYield).getCityChange();
				if (GC.getYieldInfo(eYield).getPopulationChangeDivisor() != 0)
				{
					iYield += ((pCity->getPopulation() + GC.getYieldInfo(eYield).getPopulationChangeOffset()) / GC.getYieldInfo(eYield).getPopulationChangeDivisor());
				}
				bCity = true;
			}
		}

		if (isWater())
		{
			if (!(isImpassable(GET_PLAYER(ePlayer).getTeam())&&!isRoute())) //Added Team Parameter, Afforess
			{
				iYield += GET_PLAYER(ePlayer).getSeaPlotYield(eYield);

				pWorkingCity = getWorkingCity();

				if (pWorkingCity != NULL)
				{
					if (!bDisplay || pWorkingCity->isRevealed(GC.getGame().getActiveTeam(), false))
					{
						iYield += pWorkingCity->getSeaPlotYield(eYield);
					}
				}
			}
		}

		if (isRiver())
		{
			if (!(isImpassable(GET_PLAYER(ePlayer).getTeam())&&!isRoute())) //Added Team Parameter, Afforess))
			{
				pWorkingCity = getWorkingCity();

				if (NULL != pWorkingCity)
				{
					if (!bDisplay || pWorkingCity->isRevealed(GC.getGame().getActiveTeam(), false))
					{
						iYield += pWorkingCity->getRiverPlotYield(eYield);
					}
				}
			}
		}
	}

	if (bCity)
	{
		//
		// Mongoose CityTileCommerceMod BEGIN
		//
		if (eYield == YIELD_COMMERCE)
		{
			iYield += GC.getYieldInfo(eYield).getMinCity();
		}
		else
		{
			iYield = std::max(iYield, GC.getYieldInfo(eYield).getMinCity());
		}
		//
		// Mongoose CityTileCommerceMod END
		//
	}

	iYield += GC.getGame().getPlotExtraYield(m_iX, m_iY, eYield);

	if (ePlayer != NO_PLAYER)
	{
		if (GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield) > 0)
		{
			if (iYield >= GET_PLAYER(ePlayer).getExtraYieldThreshold(eYield))
			{
				iYield += GC.getDefineINT("EXTRA_YIELD");
			}
		}

		if (GET_PLAYER(ePlayer).getLessYieldThreshold(eYield) > 0)
		{
			if (iYield >= GET_PLAYER(ePlayer).getLessYieldThreshold(eYield))
			{
				iYield -= GC.getDefineINT("EXTRA_YIELD");
			}
		}

		if (GET_PLAYER(ePlayer).isGoldenAge())
		{
			if (iYield >= GC.getYieldInfo(eYield).getGoldenAgeYieldThreshold())
			{
				iYield += GC.getYieldInfo(eYield).getGoldenAgeYield();
			}
		}
/************************************************************************************************/
/* Afforess	                  Start		 02/02/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		iYield += GET_PLAYER(ePlayer).getTerrainYieldChange(getTerrainType(), eYield);

		if (getLandmarkType() != NO_LANDMARK)
		{
			if (GC.getGame().isOption(GAMEOPTION_PERSONALIZED_MAP))
			{
				iYield += GET_PLAYER(ePlayer).getLandmarkYield(eYield);
			}
		}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}

/************************************************************************************************/
/* RevolutionDCM	                  Start		 05/31/10                        Afforess       */
/*                                                                                              */
/* Battle Effects                                                                               */
/************************************************************************************************/
	if (isBattle())
	{
		if (eYield == YIELD_FOOD)
		{
			iYield += GC.getBATTLE_EFFECT_LESS_FOOD();
		}
		else if (eYield == YIELD_PRODUCTION)
		{
			iYield += GC.getBATTLE_EFFECT_LESS_PRODUCTION();
		}
		else
		{
			iYield += GC.getBATTLE_EFFECT_LESS_COMMERCE();
		}
	}
/************************************************************************************************/
/* RevolutionDCM	             Battle Effects END                                             */
/************************************************************************************************/

	return std::max(0, iYield);
}


bool CvPlot::hasYield() const
{
	int iI;

	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		if (getYield((YieldTypes)iI) > 0)
		{
			return true;
		}
	}

	return false;
}

void CvPlot::updateYield()
{
	PROFILE_FUNC();

	CvCity* pWorkingCity;
	bool bChange;
	int iNewYield;
	int iOldYield;
	int iI;

	if (area() == NULL)
	{
		return;
	}

	bChange = false;

	for (iI = 0; iI < NUM_YIELD_TYPES; ++iI)
	{
		iNewYield = calculateYield((YieldTypes)iI);

		if (getYield((YieldTypes)iI) != iNewYield)
		{
			iOldYield = getYield((YieldTypes)iI);

			m_aiYield[iI] = iNewYield;
			FAssert(getYield((YieldTypes)iI) >= 0);

			pWorkingCity = getWorkingCity();

			if (pWorkingCity != NULL)
			{
				if (isBeingWorked())
				{
					pWorkingCity->changeBaseYieldRate(((YieldTypes)iI), (getYield((YieldTypes)iI) - iOldYield));
				}

				pWorkingCity->AI_setAssignWorkDirty(true);
			}

			bChange = true;
		}
	}

	if (bChange)
	{
		updateSymbols();
	}
}


int CvPlot::getCulture(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");

	for(std::vector<std::pair<PlayerTypes,int> >::const_iterator itr = m_aiCulture.begin(); itr != m_aiCulture.end(); ++itr)
	{
		if ( (*itr).first == eIndex )
		{
			return (*itr).second;
		}
	}

	return 0;
}


int CvPlot::countTotalCulture() const
{
	int iTotalCulture;
	int iI;

	iTotalCulture = 0;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			iTotalCulture += getCulture((PlayerTypes)iI);
		}
	}

	return iTotalCulture;
}


TeamTypes CvPlot::findHighestCultureTeam() const
{
	PlayerTypes eBestPlayer = findHighestCulturePlayer();

	if (NO_PLAYER == eBestPlayer)
	{
		return NO_TEAM;
	}

	return GET_PLAYER(eBestPlayer).getTeam();
}


PlayerTypes CvPlot::findHighestCulturePlayer() const
{
	PlayerTypes eBestPlayer = NO_PLAYER;
	int iBestValue = 0;

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			int iValue = getCulture((PlayerTypes)iI);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestPlayer = (PlayerTypes)iI;
			}
		}
	}

	return eBestPlayer;
}


int CvPlot::calculateCulturePercent(PlayerTypes eIndex) const
{
	PROFILE_FUNC();

	int iTotalCulture;

	iTotalCulture = countTotalCulture();

	if (iTotalCulture > 0)
	{
		int iResult;

		if (getCulture(eIndex) > MAX_INT/1000)
		{
			iResult = (getCulture(eIndex) / (iTotalCulture/100));
		}
		else
		{
			iResult = ((getCulture(eIndex) * 100) / iTotalCulture);
		}
		return iResult;
	}

	return 0;
}


int CvPlot::calculateTeamCulturePercent(TeamTypes eIndex) const
{
	int iTeamCulturePercent;
	int iI;

	iTeamCulturePercent = 0;

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eIndex)
			{
				iTeamCulturePercent += calculateCulturePercent((PlayerTypes)iI);
			}
		}
	}

	return iTeamCulturePercent;
}


void CvPlot::setCulture(PlayerTypes eIndex, int iNewValue, bool bUpdate, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	CvCity* pCity;

	FAssertMsg(eIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "iIndex is expected to be within maximum bounds (invalid Index)");

	if (getCulture(eIndex) != iNewValue)
	{
		std::vector<std::pair<PlayerTypes,int> >::iterator itr;

		for(itr = m_aiCulture.begin(); itr != m_aiCulture.end(); ++itr)
		{
			if ( (*itr).first == eIndex )
			{
				break;
			}
		}

		if ( itr == m_aiCulture.end() )
		{
			m_aiCulture.push_back(std::make_pair(eIndex,iNewValue));
			//	Force the capacity to the size() since we need to mto minimize memory usage
			//	and adding cultures is rare
			std::vector<std::pair<PlayerTypes,int> >(m_aiCulture).swap(m_aiCulture);
		}
		else
		{
			(*itr).second = iNewValue;
		}
		FAssert(getCulture(eIndex) >= 0);

		if (bUpdate)
		{
			updateCulture(true, bUpdatePlotGroups);
		}

		pCity = getPlotCity();

		if (pCity != NULL)
		{
			pCity->AI_setAssignWorkDirty(true);
		}
	}
}


void CvPlot::changeCulture(PlayerTypes eIndex, int iChange, bool bUpdate)
{
	if (0 != iChange)
	{
		const int iNonNegativeCulture = std::max(getCulture(eIndex) + iChange, 0);		
		setCulture(eIndex, iNonNegativeCulture, bUpdate, true);
	}
}

int CvPlot::getFoundValue(PlayerTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiFoundValue || (!isRevealed(GET_PLAYER(eIndex).getTeam(), false) && GET_PLAYER(eIndex).getNumCities() > 0))
	{
		return 0;
	}

	//	Note - m_aiFoundValue is scaled by a factor of 10 to prevent wrapping since it's
	//	only a short (extra policing is present on the set)
	if (m_aiFoundValue[eIndex] == INVALID_FOUND_VALUE)
	{
		long lResult = -1;
		if(GC.getUSE_GET_CITY_FOUND_VALUE_CALLBACK())
		{
			lResult = Cy::call<long>(PYGameModule, "getCityFoundValue", Cy::Args()
				<< eIndex
				<< getX()
				<< getY()
			);
		}

		if (lResult == -1)
		{
			setFoundValue(eIndex,GET_PLAYER(eIndex).AI_foundValue(getX(), getY(), -1, true));
		}
		else
		{
			setFoundValue(eIndex, lResult);
		}

		if ( area()->hasBestFoundValue(eIndex) && (int) m_aiFoundValue[eIndex] > area()->getBestFoundValue(eIndex))
		{
			area()->setBestFoundValue(eIndex, m_aiFoundValue[eIndex]);
		}
	}

	return m_aiFoundValue[eIndex];
}


bool CvPlot::isBestAdjacentFound(PlayerTypes eIndex)
{
	CvPlot* pAdjacentPlot;
	int iI;

	int iPlotValue = GET_PLAYER(eIndex).AI_foundValue(getX(), getY());

	if (iPlotValue == 0)
	{
		return false;
	}

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if ((pAdjacentPlot != NULL) && pAdjacentPlot->isRevealed(GET_PLAYER(eIndex).getTeam(), false))
		{
			//if (pAdjacentPlot->getFoundValue(eIndex) >= getFoundValue(eIndex))
			if (GET_PLAYER(eIndex).AI_foundValue(pAdjacentPlot->getX(), pAdjacentPlot->getY()) > iPlotValue)
			{
				return false;
			}
		}
	}

	return true;
}

void CvPlot::clearFoundValue(PlayerTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

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
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssert(iNewValue >= 0);

	MEMORY_TRACK_EXEMPT();

	if (NULL == m_aiFoundValue && 0 != iNewValue)
	{
		m_aiFoundValue = new unsigned int[MAX_PLAYERS];
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			m_aiFoundValue[iI] = 0;
		}
	}

	if (NULL != m_aiFoundValue)
	{
		FAssert(iNewValue >= 0);
		m_aiFoundValue[eIndex] = (unsigned int)iNewValue;
	}
}


int CvPlot::getPlayerCityRadiusCount(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiPlayerCityRadiusCount)
	{
		return 0;
	}

	return m_aiPlayerCityRadiusCount[eIndex];
}


bool CvPlot::isPlayerCityRadius(PlayerTypes eIndex) const
{
	return (getPlayerCityRadiusCount(eIndex) > 0);
}


void CvPlot::changePlayerCityRadiusCount(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (0 != iChange)
	{
		if (NULL == m_aiPlayerCityRadiusCount)
		{
			m_aiPlayerCityRadiusCount = new char[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_aiPlayerCityRadiusCount[iI] = 0;
			}
		}

		m_aiPlayerCityRadiusCount[eIndex] += iChange;
		FAssert(getPlayerCityRadiusCount(eIndex) >= 0);
	}
}


int CvPlot::getPlotGroupId(PlayerTypes ePlayer) const
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiPlotGroup)
	{
		return FFreeList::INVALID_INDEX;
	}

	return m_aiPlotGroup[ePlayer];
}

CvPlotGroup* CvPlot::getPlotGroup(PlayerTypes ePlayer) const
{
	FAssertMsg(ePlayer >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

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

	int iI;

	CvPlotGroup* pOldPlotGroup = getPlotGroup(ePlayer);

	//	The id-level check is to handle correction of an old buggy state where
	//	invalid ids could be present (which map to a NULL group if resolved through getPlotGroup())
	if (pOldPlotGroup != pNewValue || (pNewValue == NULL && getPlotGroupId(ePlayer) != -1) )
	{
		CvCity* pCity;

		if (NULL ==  m_aiPlotGroup)
		{
			m_aiPlotGroup = new int[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_aiPlotGroup[iI] = FFreeList::INVALID_INDEX;
			}
		}

		if ( bRecalculateEffect )
		{
			pCity = getPlotCity();

			if (ePlayer == getOwner())
			{
				GET_PLAYER(getOwner()).startDeferredPlotGroupBonusCalculation();
				updatePlotGroupBonus(false);
			}

			if (pOldPlotGroup != NULL)
			{
				if (pCity != NULL)
				{
					if (pCity->getOwner() == ePlayer)
					{
						FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::setPlotGroup");
						for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
						{
							pCity->changeNumBonuses(((BonusTypes)iI), -(pOldPlotGroup->getNumBonuses((BonusTypes)iI)));
						}
					}
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

		if ( bRecalculateEffect )
		{
			if (getPlotGroup(ePlayer) != NULL)
			{
				if (pCity != NULL)
				{
					if (pCity->getOwner() == ePlayer)
					{
						FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlot::setPlotGroup");
						for (iI = 0; iI < GC.getNumBonusInfos(); ++iI)
						{
							pCity->changeNumBonuses(((BonusTypes)iI), getPlotGroup(ePlayer)->getNumBonuses((BonusTypes)iI));
						}
					}
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

	int iI;

	if ( bDeferPlotGroupRecalculation )
	{
		m_bPlotGroupsDirty = true;
	}
	else
	{
		for (iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				updatePlotGroup((PlayerTypes)iI);
			}
		}

		m_bPlotGroupsDirty = false;
	}
}

static bool PlotSetContains(CvPlot* pPlot, std::vector<CvPlot*>& set)
{
	for(int iJ = 0; iJ < (int)set.size(); iJ++)
	{
		if ( set[iJ] == pPlot )
		{
			return true;
		}
	}

	return false;
}

static void CalculateClosePlotGroupConnectSet(PlayerTypes ePlayer, CvPlot* pSeedPlot, std::vector<CvPlot*>& set, int iRange)
{
	CvPlotGroup* seedPlotGroup = pSeedPlot->getPlotGroup(ePlayer);

	if ( !PlotSetContains(pSeedPlot, set) )
	{
		set.push_back(pSeedPlot);

		if ( iRange > 0 )
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				CvPlot* pAdjacentPlot = plotDirection(pSeedPlot->getX(), pSeedPlot->getY(), ((DirectionTypes)iI));

				if (pAdjacentPlot != NULL)
				{
					if (pAdjacentPlot->getPlotGroup(ePlayer) == seedPlotGroup)
					{
						CalculateClosePlotGroupConnectSet(ePlayer, pAdjacentPlot, set, iRange-1);
					}
				}
			}
		}
	}
}

void CvPlot::updatePlotGroup(PlayerTypes ePlayer, bool bRecalculate, bool bRecalculateBonuses)
{
	//PROFILE("CvPlot::updatePlotGroup(Player)");

	CvPlotGroup* pPlotGroup;
	CvPlotGroup* pAdjacentPlotGroup;
	CvPlot* pAdjacentPlot;
	bool bConnected;
	bool bEmpty;
	int iI;

#ifdef USE_INTERNAL_PROFILER
	if ( !IFPIsMainThread() )
	{
		::MessageBox(NULL, "Unexpected plotGroup chnage on background thread", "CvGameCoreDLL", MB_OK);
	}
#endif

	if (!(GC.getGame().isFinalInitialized()))
	{
		return;
	}

	pPlotGroup = getPlotGroup(ePlayer);

	if (pPlotGroup != NULL)
	{
		if (bRecalculate)
		{
			bConnected = false;
			FAssert(bRecalculateBonuses);	//	Should never call without recalc on a plot that already has a group

			if (isTradeNetwork(GET_PLAYER(ePlayer).getTeam()))
			{
				bConnected = true;
#if 0
				for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

					if (pAdjacentPlot != NULL)
					{
						if (pAdjacentPlot->getPlotGroup(ePlayer) == pPlotGroup)
						{
							if (!isTradeNetworkConnected(pAdjacentPlot, GET_PLAYER(ePlayer).getTeam()))
							{
								bConnected = false;
								break;
							}
						}
					}
				}
#endif
			}

			if (!bConnected)
			{
				bEmpty = (pPlotGroup->getLengthPlots() == 1);
				FAssertMsg(pPlotGroup->getLengthPlots() > 0, "pPlotGroup should have more than 0 plots");

				pPlotGroup->removePlot(this);

				if (!bEmpty)
				{
					bool	bIsSamePlotGroup[NUM_DIRECTION_TYPES];
					int		iSamePlotGroupAdjacentCount = 0;
					CvPlot* pAdjacentInstance = NULL;

					for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						bIsSamePlotGroup[iI] = false;
					}

					for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
					{
						pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

						if (pAdjacentPlot != NULL)
						{
							if (pAdjacentPlot->getPlotGroup(ePlayer) == pPlotGroup)
							{
								iSamePlotGroupAdjacentCount++;
								bIsSamePlotGroup[iI] = true;

								pAdjacentInstance = pAdjacentPlot;
							}
						}
					}

					if ( iSamePlotGroupAdjacentCount > 1 && iSamePlotGroupAdjacentCount < 7 )
					{
						PROFILE("CvPlot::updatePlotGroup.PotentialRemove");

						std::vector<CvPlot*> closeConnected;

						CalculateClosePlotGroupConnectSet(ePlayer, pAdjacentInstance, closeConnected, 3);

						int	iStillConnectedCount = 0;

						for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
						{
							pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

							if (pAdjacentPlot != NULL)
							{
								if (pAdjacentPlot->getPlotGroup(ePlayer) == pPlotGroup)
								{
									if ( PlotSetContains(pAdjacentPlot, closeConnected) )
									{
										iStillConnectedCount++;
									}
								}
							}
						}

						if ( iSamePlotGroupAdjacentCount != iStillConnectedCount )
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
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				pAdjacentPlotGroup = pAdjacentPlot->getPlotGroup(ePlayer);

				if ((pAdjacentPlotGroup != NULL) && (pAdjacentPlotGroup != pPlotGroup))
				{
					if (isTradeNetworkConnected(pAdjacentPlot, GET_PLAYER(ePlayer).getTeam()))
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
							GC.getMap().combinePlotGroups(ePlayer, pPlotGroup, pAdjacentPlotGroup, bRecalculateBonuses);
							pPlotGroup = getPlotGroup(ePlayer);
							FAssertMsg(pPlotGroup != NULL, "PlotGroup is not assigned a valid value");
						}
					}
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
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiVisibilityCount)
	{
		return 0;
	}

	return m_aiVisibilityCount[eTeam];
}

int CvPlot::getDangerCount(int /*PlayerTypes*/ ePlayer) const
{
	FAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

	return (m_aiDangerCount == NULL) ? 0 : m_aiDangerCount[ePlayer];
}

void CvPlot::setDangerCount(int /*PlayerTypes*/ ePlayer, int iNewCount)
{
	FAssertMsg(ePlayer >= 0, "ePlayer is expected to be non-negative (invalid Index)");
	FAssertMsg(ePlayer < MAX_PLAYERS, "ePlayer is expected to be within maximum bounds (invalid Index)");

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
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if ( isVisible(eTeam, false) )
	{
		return GC.getGame().getGameTurn();
	}

	if (NULL == m_aiLastSeenTurn)
	{
		return 0;
	}

	return m_aiLastSeenTurn[eTeam];
}

void CvPlot::setLastVisibleTurn(TeamTypes eTeam, short turn)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

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
	SAFE_DELETE(m_aiVisibilityCount);
	SAFE_DELETE(m_aiStolenVisibilityCount);
	if (NULL != m_apaiInvisibleVisibilityCount)
	{
		for (int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiInvisibleVisibilityCount);
	}

	//if (NULL != m_apaiCachedHighestTeamInvisibilityIntensity)
	//{
	//	for (int iI = 0; iI < MAX_TEAMS; ++iI)
	//	{
	//		SAFE_DELETE_ARRAY(m_apaiCachedHighestTeamInvisibilityIntensity[iI]);
	//	}
	//	SAFE_DELETE_ARRAY(m_apaiCachedHighestTeamInvisibilityIntensity);
	//}
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

void CvPlot::changeVisibilityCount(TeamTypes eTeam, int iChange, InvisibleTypes eSeeInvisible, bool bUpdatePlotGroups, int iIntensity, int iUnitID)
{
	CvCity* pCity;
	CvPlot* pAdjacentPlot;
	bool bOldVisible;
	int iI;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		if (NULL == m_aiVisibilityCount)
		{
			m_aiVisibilityCount = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aiVisibilityCount[iI] = 0;
			}
		}

		bOldVisible = isVisible(eTeam, false);
		if (eSeeInvisible == NO_INVISIBLE || !GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
		{
			m_aiVisibilityCount[eTeam] += iChange;
			FAssert(getVisibilityCount(eTeam) >= 0);
		}

		//	Should never happen, but if it does better to cap it below at 0
		if ( m_aiVisibilityCount[eTeam] < 0 )
		{
			m_aiVisibilityCount[eTeam] = 0;
		}

		bool bIntensityCheck = true;
		if (GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
		{
			if (iIntensity < 1)
			{
				bIntensityCheck = false;
			}
		}
		if (eSeeInvisible != NO_INVISIBLE && bIntensityCheck)
		{
			changeInvisibleVisibilityCount(eTeam, eSeeInvisible, iChange, iIntensity, iUnitID);//add unitid
		}

		if (bOldVisible != isVisible(eTeam, false))
		{
			if (isVisible(eTeam, false))
			{
				setRevealed(eTeam, true, false, NO_TEAM, bUpdatePlotGroups);

				for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

					if (pAdjacentPlot != NULL)
					{
						pAdjacentPlot->updateRevealedOwner(eTeam);
					}
				}

				if (getTeam() != NO_TEAM)
				{
					GET_TEAM(getTeam()).meet(eTeam, true);
				}
			}
			else
			{
				setLastVisibleTurn( eTeam, GC.getGame().getGameTurn() - 1 );
			}

			pCity = getPlotCity();

			if (pCity != NULL)
			{
				pCity->setInfoDirty(true);
			}

			for (iI = 0; iI < MAX_TEAMS; ++iI)
			{
				if (GET_TEAM((TeamTypes)iI).isAlive())
				{
					if (GET_TEAM((TeamTypes)iI).isStolenVisibility(eTeam))
					{
						changeStolenVisibilityCount(((TeamTypes)iI), ((isVisible(eTeam, false)) ? 1 : -1));
					}
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
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiStolenVisibilityCount)
	{
		return 0;
	}

	return m_aiStolenVisibilityCount[eTeam];
}


void CvPlot::changeStolenVisibilityCount(TeamTypes eTeam, int iChange)
{
	CvCity* pCity;
	bool bOldVisible;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		if (NULL == m_aiStolenVisibilityCount)
		{
			m_aiStolenVisibilityCount = new short[MAX_TEAMS];
			for (int iI = 0; iI < MAX_TEAMS; ++iI)
			{
				m_aiStolenVisibilityCount[iI] = 0;
			}
		}

		bOldVisible = isVisible(eTeam, false);

		m_aiStolenVisibilityCount[eTeam] += iChange;
		FAssert(getStolenVisibilityCount(eTeam) >= 0);

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

			pCity = getPlotCity();

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
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_aiBlockadedCount)
	{
		return 0;
	}

	return std::max<int>(0, m_aiBlockadedCount[eTeam]);
}

void CvPlot::resetBlockadedCounts()
{
	SAFE_DELETE_ARRAY(m_aiBlockadedCount);
}

void CvPlot::changeBlockadedCount(TeamTypes eTeam, int iChange)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

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
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
		FAssertMsg(m_aiBlockadedCount[eTeam] >= 0, CvString::format("Blockaded count on a plot should not go lower than 0, it is now %d", getBlockadedCount(eTeam)).c_str());
		FAssertMsg(m_aiBlockadedCount[eTeam] == 0 || isWater(), "Non water tiles cannot have a non-zero Blockaded count");

		// Hack so that never get negative blockade counts as a result of fixing issue causing
		// rare permanent blockades.
		// TB Note: this can throw off counts and we have false additional amounts soooo, the negative may be necessary based on the order of things taking place and when you try to 'fix' it like this you create a permanent blockade.
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
	else
	{
		FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		if (NULL == m_aiRevealedOwner)
		{
			return NO_PLAYER;
		}

		return (PlayerTypes)m_aiRevealedOwner[eTeam];
	}
}


TeamTypes CvPlot::getRevealedTeam(TeamTypes eTeam, bool bDebug) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	PlayerTypes eRevealedOwner = getRevealedOwner(eTeam, bDebug);

	if (eRevealedOwner != NO_PLAYER)
	{
		return GET_PLAYER(eRevealedOwner).getTeam();
	}
	else
	{
		return NO_TEAM;
	}
}


void CvPlot::setRevealedOwner(TeamTypes eTeam, PlayerTypes eNewValue)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

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

	CvPlot* pAdjacentPlot;
	bool bRevealed;
	int iI;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	bRevealed = false;

	if (!bRevealed)
	{
		if (isVisible(eTeam, false))
		{
			bRevealed = true;
		}
	}

	if (!bRevealed)
	{
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				if (pAdjacentPlot->isVisible(eTeam, false))
				{
					bRevealed = true;
					break;
				}
			}
		}
	}

	if (bRevealed)
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
	CvPlot* pCornerPlot;
	CvPlot* pPlot;
	bool bValid;

	FAssertMsg(eIndex >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_DIRECTION_TYPES, "eTeam is expected to be within maximum bounds (invalid Index)");

	pCornerPlot = NULL;
	bValid = false;
	pPlot = plotDirection(getX(), getY(), eIndex);

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
			FAssert(false);
			break;
		}

		if (pCornerPlot != NULL)
		{
			pNorthEastPlot = plotDirection(pCornerPlot->getX(), pCornerPlot->getY(), DIRECTION_EAST);
			pSouthEastPlot = plotDirection(pCornerPlot->getX(), pCornerPlot->getY(), DIRECTION_SOUTHEAST);
			pSouthWestPlot = plotDirection(pCornerPlot->getX(), pCornerPlot->getY(), DIRECTION_SOUTH);
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
		if (NULL == m_abRiverCrossing)
		{
			m_abRiverCrossing = new bool[NUM_DIRECTION_TYPES];
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				m_abRiverCrossing[iI] = false;
			}
		}

		m_abRiverCrossing[eIndex] = bValid;

		changeRiverCrossingCount((isRiverCrossing(eIndex)) ? 1 : -1);
	}
}


void CvPlot::updateRiverCrossing()
{
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		updateRiverCrossing((DirectionTypes)iI);
	}
}


bool CvPlot::isRevealed(TeamTypes eTeam, bool bDebug) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (bDebug && GC.getGame().isDebugMode())
	{
		return true;
	}

	if (NULL == m_abRevealed)
	{
		return false;
	}

	return m_abRevealed[eTeam];
}


void CvPlot::setRevealed(TeamTypes eTeam, bool bNewValue, bool bTerrainOnly, TeamTypes eFromTeam, bool bUpdatePlotGroup)
{
	CvCity* pCity;
	int iI;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

/*********************************/
/***** Parallel Maps - Begin *****/
/*********************************/
	if (isNull() && !isRevealed(eTeam, false))
		return;
/*******************************/
/***** Parallel Maps - End *****/
/*******************************/

	pCity = getPlotCity();

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
			area()->changeNumRevealedTiles(eTeam, ((isRevealed(eTeam, false)) ? 1 : -1));
		}

		if (bUpdatePlotGroup)
		{
			for (iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
					{
						updatePlotGroup((PlayerTypes)iI);
					}
				}
			}
		}

		if (eTeam == GC.getGame().getActiveTeam())
		{
			if ( GC.viewportsEnabled() && shouldHaveGraphics() )
			{
				CvPlot* pAdjacentPlot;
				int iI;

				for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
				{
					pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

					if (pAdjacentPlot != NULL)
					{
						//	Unrevealed plots adjacent to newly revealed ones, which are ocean plots
						//	also need redrawing to prevent artifacting
						if (pAdjacentPlot->isWater() && !pAdjacentPlot->isRevealed(eTeam, false) && pAdjacentPlot->isInViewport())
						{
							gDLL->getEngineIFace()->RebuildPlot(pAdjacentPlot->getViewportX(), pAdjacentPlot->getViewportY(),true,true);
							pAdjacentPlot->setLayoutDirty(true);
						}
					}
				}

				if ( isWater() )
				{
					GC.getGame().noteGraphicRebuildNeeded();
				}

				//Update terrain graphics
				gDLL->getEngineIFace()->RebuildPlot(getViewportX(), getViewportY(),true,true);
				//updateFeatureSymbol();
				//updateRiverSymbol(false, true);
			}

			hideGraphics(ECvPlotGraphics::ALL);
			if (bNewValue)
			{
				//updateFeatureSymbol(true);
				updateGraphics();
				//gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true); //minimap does a partial update
				//gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
			}

			//updateSymbols();
			updateFog();
			updateVisibility();

			gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);

			// showRequiredGraphics();
			// hideNonRequiredGraphics();
		}

		if (isRevealed(eTeam, false))
		{
			// ONEVENT - PlotRevealed
			CvEventReporter::getInstance().plotRevealed(this, eTeam);
/************************************************************************************************/
/* Afforess	                  Start		 03/32/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			if (getLandmarkType() != NO_LANDMARK)
			{
				for (int iJ = 0; iJ < MAX_PLAYERS; ++iJ)
				{
					if (GET_PLAYER((PlayerTypes)iJ).isAlive() && !GET_PLAYER((PlayerTypes)iJ).isNPC())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getTeam() == eTeam)
						{
							addSign((PlayerTypes)iJ, getLandmarkMessage());
							CvString szIcon;

							bool bFirstToDiscover = true;
							for (int iI = 0; iI < MAX_TEAMS; ++iI)
							{
								if(!GET_TEAM((TeamTypes)iI).isNPC() && GET_TEAM((TeamTypes)iI).isAlive())
								{
									if (isRevealed((TeamTypes)iI, false))
									{
										bFirstToDiscover = false;
										break;
									}
								}
							}

							if (bFirstToDiscover)
							{
								MEMORY_TRACK_EXEMPT();

								if (getLandmarkType() == LANDMARK_FOREST || getLandmarkType() == LANDMARK_JUNGLE)
									szIcon = GC.getFeatureInfo(getFeatureType()).getButton();
								else if (getLandmarkType() == LANDMARK_MOUNTAIN_RANGE || getLandmarkType() == LANDMARK_PEAK)
									szIcon = GC.getTerrainInfo((TerrainTypes)GC.getInfoTypeForString("TERRAIN_PEAK")).getButton();
								else
									szIcon = GC.getTerrainInfo(getTerrainType()).getButton();
								AddDLLMessage((PlayerTypes)iJ, false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_DISCOVERED_LANDMARK"), "AS2D_TECH_GENERIC", MESSAGE_TYPE_MINOR_EVENT, szIcon, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getX(), getY(), true, true);
							}
						}
					}
				}
			}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
		}
	}

	if (!bTerrainOnly)
	{
		if (isRevealed(eTeam, false))
		{
			if (eFromTeam == NO_TEAM)
			{
				setRevealedOwner(eTeam, getOwner());
				setRevealedImprovementType(eTeam, getImprovementType());
				setRevealedRouteType(eTeam, getRouteType());

				if (pCity != NULL)
				{
					pCity->setRevealed(eTeam, true);
				}
			}
			else
			{
				if (getRevealedOwner(eFromTeam, false) == getOwner())
				{
					setRevealedOwner(eTeam, getRevealedOwner(eFromTeam, false));
				}

				if (getRevealedImprovementType(eFromTeam, false) == getImprovementType())
				{
					setRevealedImprovementType(eTeam, getRevealedImprovementType(eFromTeam, false));
				}

				if (getRevealedRouteType(eFromTeam, false) == getRouteType())
				{
					setRevealedRouteType(eTeam, getRevealedRouteType(eFromTeam, false));
				}

				if (pCity != NULL)
				{
					if (pCity->isRevealed(eFromTeam, false))
					{
						pCity->setRevealed(eTeam, true);
					}
				}
			}
		}
		else
		{
			setRevealedOwner(eTeam, NO_PLAYER);
			setRevealedImprovementType(eTeam, NO_IMPROVEMENT);
			setRevealedRouteType(eTeam, NO_ROUTE);

			if (pCity != NULL)
			{
				pCity->setRevealed(eTeam, false);
			}
		}
	}
}

bool CvPlot::isAdjacentRevealed(TeamTypes eTeam, bool bDebug) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isRevealed(eTeam, bDebug))
			{
				return true;
			}
		}
	}

	return false;
}

bool CvPlot::isAdjacentNonrevealed(TeamTypes eTeam) const
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (!pAdjacentPlot->isRevealed(eTeam, false))
			{
				return true;
			}
		}
	}

	return false;
}


ImprovementTypes CvPlot::getRevealedImprovementType(TeamTypes eTeam, bool bDebug) const
{
	if (bDebug && GC.getGame().isDebugMode())
	{
		return getImprovementType();
	}
	else
	{
		FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		if (NULL == m_aeRevealedImprovementType)
		{
			return NO_IMPROVEMENT;
		}

		return (ImprovementTypes)m_aeRevealedImprovementType[eTeam];
	}
}


void CvPlot::setRevealedImprovementType(TeamTypes eTeam, ImprovementTypes eNewValue)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

	if (getRevealedImprovementType(eTeam, false) != eNewValue)
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
	else
	{
		FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
		FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

		if (NULL == m_aeRevealedRouteType)
		{
			return NO_ROUTE;
		}

		return (RouteTypes)m_aeRevealedRouteType[eTeam];
	}
}


void CvPlot::setRevealedRouteType(TeamTypes eTeam, RouteTypes eNewValue)
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");

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
	if (NULL == m_paiBuildProgress)
	{
		return 0;
	}

	return m_paiBuildProgress[eBuild];
}


// Returns true if build finished...
bool CvPlot::changeBuildProgress(BuildTypes eBuild, int iChange, TeamTypes eTeam)
{
	CvCity* pCity = NULL;
	CvWString szBuffer;
	int iProduction;
	bool bFinished;

	bFinished = false;

	if (iChange != 0)
	{
		if (NULL == m_paiBuildProgress)
		{
			m_paiBuildProgress = new int[GC.getNumBuildInfos()];
			for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				m_paiBuildProgress[iI] = 0;
			}
		}

		m_paiBuildProgress[eBuild] += iChange;
		FAssert(getBuildProgress(eBuild) >= 0);

		BonusTypes eBonusPlaced = NO_BONUS;
		if (getBuildProgress(eBuild) >= getBuildTime(eBuild))
		{
			m_paiBuildProgress[eBuild] = 0;

			//TB Bonus Placement
			if (GC.getBuildInfo(eBuild).getNumPlaceBonusTypes() > 0)
			{
				std::vector<int> m_aBonusResult;
				m_aBonusResult.clear();
				bool bAccessFound = false;
				for (int iI = 0; iI < GC.getBuildInfo(eBuild).getNumPlaceBonusTypes(); iI++)
				{
					BonusTypes eBonus = GC.getBuildInfo(eBuild).getPlaceBonusType(iI).eBonus;
					bAccessFound = false;
					if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).bRequiresAccess)
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
						if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqMapCategory != NO_MAPCATEGORY)
						{
							bAccessFound = false;
							if (isMapCategoryType(GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqMapCategory))
							{
								bAccessFound = true;
							}
						}
					}
					if (bAccessFound)
					{
						if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqTerrain != NO_TERRAIN)
						{
							bAccessFound = false;
							if (getTerrainType() == GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqTerrain)
							{
								bAccessFound = true;
							}
						}
					}
					if (bAccessFound)
					{
						if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqTech != NO_TECH)
						{
							bAccessFound = false;
							if (GET_TEAM(getTeam()).isHasTech(GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqTech))
							{
								bAccessFound = true;
							}
						}
					}
					if (bAccessFound)
					{
						if (GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqFeature != NO_FEATURE)
						{
							bAccessFound = false;
							if (getFeatureType() == GC.getBuildInfo(eBuild).getPlaceBonusType(iI).ePrereqFeature)
							{
								bAccessFound = true;
							}
						}
					}
					if (bAccessFound)
					{
						bAccessFound = false;
						if(canHaveBonus(GC.getBuildInfo(eBuild).getPlaceBonusType(iI).eBonus))
						{
							bAccessFound = true;
						}
					}
					if (bAccessFound)
					{
						for (int iK = 0; iK < GC.getBuildInfo(eBuild).getPlaceBonusType(iI).iProbability; iK++)
						{
							m_aBonusResult.push_back((int)GC.getBuildInfo(eBuild).getPlaceBonusType(iI).eBonus);
						}
					}
				}
				if (m_aBonusResult.size()>0)
				{
					int iPossible = (int)m_aBonusResult.size();
					iPossible = std::max(iPossible, 100);
					unsigned iResult = GC.getGame().getSorenRandNum(iPossible, "Select Bonus Placement Type");

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
					MEMORY_TRACK_EXEMPT();

					if ( isInViewport() )
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_PLACED_BONUS", GC.getBonusInfo(eBonusPlaced).getTextKeyWide());
						AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, GC.getBonusInfo(getBonusType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getViewportX(),getViewportY(), true, true);
					}
				}
				else
				{
					MEMORY_TRACK_EXEMPT();

					if ( isInViewport() )
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_PLACED_BONUS_FAIL");
						AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, 0, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getViewportX(),getViewportY(), true, true);
					}
				}
			}



			if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
			{
				setImprovementType((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement());
			}

			if (GC.getBuildInfo(eBuild).getRoute() != NO_ROUTE)
			{
				setRouteType((RouteTypes)GC.getBuildInfo(eBuild).getRoute(), true);
			}

/************************************************************************************************/
/* JOOYO_ADDON, Added by Jooyo, 06/13/09                                                        */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			if (GC.getBuildInfo(eBuild).getTerrainChange() != NO_TERRAIN)
			{
				setTerrainType ((TerrainTypes) GC.getBuildInfo(eBuild).getTerrainChange());
				//setImprovementType((ImprovementTypes) -1);
			}

			if (GC.getBuildInfo(eBuild).getFeatureChange() != NO_FEATURE)
			{
				setFeatureType ((FeatureTypes) GC.getBuildInfo(eBuild).getFeatureChange());
				//setImprovementType((ImprovementTypes) -1);
			}
/************************************************************************************************/
/* JOOYO_ADDON                          END                                                     */
/************************************************************************************************/
			//TB Note: Apparently isFeatureRemove is based entirely on the BUILD definition - changing it to the improvement definition would enable us to restructure where this takes place so that a 'natural' upgrade can have the same effect.
			if (getFeatureType() != NO_FEATURE)
			{
				if (GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()))
				{
					FAssertMsg(eTeam != NO_TEAM, "eTeam should be valid");

					iProduction = getFeatureProduction(eBuild, eTeam, &pCity);

					if (iProduction > 0)
					{
						pCity->changeFeatureProduction(iProduction);

						MEMORY_TRACK_EXEMPT();

						if ( isInViewport() )
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_CLEARING_FEATURE_BONUS", GC.getFeatureInfo(getFeatureType()).getTextKeyWide(), iProduction, pCity->getNameKey());
							AddDLLMessage(pCity->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, GC.getFeatureInfo(getFeatureType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getViewportX(),getViewportY(), true, true);
						}
					}

					// Python Event
					CvEventReporter::getInstance().plotFeatureRemoved(this, getFeatureType(), pCity);

					setFeatureType(NO_FEATURE);

/************************************************************************************************/
/* phunny_pharmer                Start		 04/28/10                                           */
/*   the cache of culture distances should be recomputed now that this feature has been removed */
/************************************************************************************************/
					if ( pCity != NULL )
					{
						pCity->clearCultureDistanceCache();
					}
/************************************************************************************************/
/* phunny_pharmer                    END                                                        */
/************************************************************************************************/
				}
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
	//MEMORY_TRACE_FUNCTION();
	PROFILE_FUNC();

	FeatureTypes eFeature;

	if (!isGraphicsVisible(ECvPlotGraphics::FEATURE) && !bForce)
	{
		return;
	}

	eFeature = getFeatureType();

	{
		//CMemoryTrace __memoryTrace("RebuildTileArt");

		gDLL->getEngineIFace()->RebuildTileArt(getViewportX(),getViewportY());
	}

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

	CvPlot* pAdjacentPlot;
	RouteTypes eRoute;
	int iI;

	if ( !isGraphicsVisible(ECvPlotGraphics::ROUTE) )
	{
		return;
	}

	if (bAdjacent)
	{
		for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

			if (pAdjacentPlot != NULL)
			{
				pAdjacentPlot->updateRouteSymbol(bForce, false);
				//pAdjacentPlot->setLayoutDirty(true);
			}
		}
	}

	eRoute = getRevealedRouteType(GC.getGame().getActiveTeam(), true);

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

	CvPlot* pAdjacentPlot;

	if (!isGraphicsVisible(ECvPlotGraphics::RIVER))
	{
		return;
	}

	if (bAdjacent)
	{
		for(int i=0;i<NUM_DIRECTION_TYPES;i++)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)i));
			if (pAdjacentPlot != NULL)
			{
				pAdjacentPlot->updateRiverSymbol(bForce, false);
				//pAdjacentPlot->setLayoutDirty(true);
			}
		}
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
			pAdjacentPlot = plotDirection(getX(), getY(), affectedDirections[i]);
			if (pAdjacentPlot != NULL && pAdjacentPlot->isInViewport())
			{
				gDLL->getEngineIFace()->ForceTreeOffsets(getViewportX(),getViewportY());
			}
		}

		//cut out canyons
		gDLL->getEngineIFace()->RebuildRiverPlotTile(getViewportX(),getViewportY(), true, false);

		//recontour adjacent rivers
		for(int i=0;i<NUM_DIRECTION_TYPES;i++)
		{
			pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)i));
			if((pAdjacentPlot != NULL) && (pAdjacentPlot->m_pRiverSymbol != NULL))
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
	//this is used to update floodplain features
	if ( !GC.viewportsEnabled() || isRevealed(GC.getGame().getActiveTeam(), true) )
	{
		gDLL->getEntityIFace()->setupFloodPlains(m_pRiverSymbol);
	}

	if(bAdjacent)
	{
		for(int i=0;i<NUM_DIRECTION_TYPES;i++)
		{
			CvPlot *pAdjacentPlot = plotDirection(getX(), getY(), (DirectionTypes) i);
			if((pAdjacentPlot != NULL) && (pAdjacentPlot->m_pRiverSymbol != NULL) && (!GC.viewportsEnabled() || pAdjacentPlot->isRevealed(GC.getGame().getActiveTeam(), true)))
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

	if ( !isGraphicsVisible(ECvPlotGraphics::UNIT) )
	{
		return;
	}

	PlayerTypes ePlayer = NO_PLAYER;
	PlayerTypes ePlayerOffset = NO_PLAYER;

	CvUnit* pCenterUnit = getCenterUnit();

	//get the plot's unit's flag
	//The plot check is to account for units in the delayed-death cycle
	if (pCenterUnit != NULL && pCenterUnit->plot() == this)
	{
		//OutputDebugString(CvString::format("Updating flag symbol for (%d,%d).  Center unit is %08lx\n", m_iX, m_iY, pCenterUnit).c_str());
		ePlayer = pCenterUnit->getVisualOwner();
	}

	//get moving unit's flag
	if (gDLL->getInterfaceIFace()->getSingleMoveGotoPlot() == this)
	{
		if(ePlayer == NO_PLAYER)
		{
			ePlayer = GC.getGame().getActivePlayer();
		}
		else
		{
			ePlayerOffset = GC.getGame().getActivePlayer();
		}
	}

	//don't put two of the same flags
	if(ePlayerOffset == ePlayer)
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
		//if ((m_pFlagSymbol == NULL) || (gDLL->getFlagEntityIFace()->getPlayer(m_pFlagSymbol) != ePlayer))
		{
			CvFlagEntity* newFlagEntity = gDLL->getFlagEntityIFace()->create(ePlayer);
			if (m_pFlagSymbol != NULL)
			{
				gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbol);
			}
			else
			{
				CvFlagEntity* extraFlagEntity = gDLL->getFlagEntityIFace()->create(ePlayer);
				gDLL->getFlagEntityIFace()->destroy(newFlagEntity);
				newFlagEntity = extraFlagEntity;
			}
			m_pFlagSymbol = newFlagEntity;
			if (m_pFlagSymbol != NULL)
			{
				gDLL->getFlagEntityIFace()->setPlot(m_pFlagSymbol, this, false);
			}
		}

		if (m_pFlagSymbol != NULL)
		{
			gDLL->getFlagEntityIFace()->updateUnitInfo(m_pFlagSymbol, this, false);
		}
	}

	//create and/or update offset flag
	if (ePlayerOffset != NO_PLAYER)
	{
		if ((m_pFlagSymbolOffset == NULL) || (gDLL->getFlagEntityIFace()->getPlayer(m_pFlagSymbolOffset) != ePlayerOffset))
		{
			if (m_pFlagSymbolOffset != NULL)
			{
				gDLL->getFlagEntityIFace()->destroy(m_pFlagSymbolOffset);
			}
			m_pFlagSymbolOffset = gDLL->getFlagEntityIFace()->create(ePlayerOffset);
			if (m_pFlagSymbolOffset != NULL)
			{
				gDLL->getFlagEntityIFace()->setPlot(m_pFlagSymbolOffset, this, true);
			}
		}

		if (m_pFlagSymbolOffset != NULL)
		{
			gDLL->getFlagEntityIFace()->updateUnitInfo(m_pFlagSymbolOffset, this, true);
		}
	}
}


CvUnit* CvPlot::getCenterUnit() const
{
	return m_pCenterUnit;
}


CvUnit* CvPlot::getDebugCenterUnit() const
{
	CvUnit* pCenterUnit;

	pCenterUnit = getCenterUnit();

	if (pCenterUnit == NULL)
	{
		if (GC.getGame().isDebugMode())
		{
			CLLNode<IDInfo>* pUnitNode = headUnitNode();
			if(pUnitNode == NULL)
				pCenterUnit = NULL;
			else
				pCenterUnit = ::getUnit(pUnitNode->m_data);
		}
	}

	return pCenterUnit;
}


void CvPlot::setCenterUnit(CvUnit* pNewValue)
{
	CvUnit* pOldValue;

	pOldValue = getCenterUnit();

	if (pOldValue != pNewValue)
	{
		m_pCenterUnit = pNewValue;

		updateMinimapColor();

		setFlagDirty(true);

		if (getCenterUnit() != NULL)
		{
			getCenterUnit()->setInfoBarDirty(true);
		}
	}
}


int CvPlot::getCultureRangeCities(PlayerTypes eOwnerIndex, int iRangeIndex) const
{
//	FAssert(eOwnerIndex >= 0);
	FAssert(eOwnerIndex < MAX_PLAYERS);
	FAssert(iRangeIndex >= 0);
	FAssert(iRangeIndex < GC.getNumCultureLevelInfos());

	if (NULL == m_apaiCultureRangeCities)
	{
		return 0;
	}
/************************************************************************************************/
/* Afforess	                  Start		 02/27/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	else if (eOwnerIndex == NO_PLAYER)
	{
		return 0;
	}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	else if (NULL == m_apaiCultureRangeCities[eOwnerIndex])
	{
		return 0;
	}

	return m_apaiCultureRangeCities[eOwnerIndex][iRangeIndex];
}


bool CvPlot::isCultureRangeCity(PlayerTypes eOwnerIndex, int iRangeIndex) const
{
	return (getCultureRangeCities(eOwnerIndex, iRangeIndex) > 0);
}

/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvPlot::changeCultureRangeCities(PlayerTypes eOwnerIndex, int iRangeIndex, int iChange, bool bUpdatePlotGroups, bool bUpdateCulture)
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
{
	bool bOldCultureRangeCities;

	FAssert(eOwnerIndex >= 0);
	FAssert(eOwnerIndex < MAX_PLAYERS);
	FAssert(iRangeIndex >= 0);
	FAssert(iRangeIndex < GC.getNumCultureLevelInfos());

	if (0 != iChange)
	{
		bOldCultureRangeCities = isCultureRangeCity(eOwnerIndex, iRangeIndex);

		if (NULL == m_apaiCultureRangeCities)
		{
			m_apaiCultureRangeCities = new char*[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_apaiCultureRangeCities[iI] = NULL;
			}
		}

		if (NULL == m_apaiCultureRangeCities[eOwnerIndex])
		{
			m_apaiCultureRangeCities[eOwnerIndex] = new char[GC.getNumCultureLevelInfos()];
			for (int iI = 0; iI < GC.getNumCultureLevelInfos(); ++iI)
			{
				m_apaiCultureRangeCities[eOwnerIndex][iI] = 0;
			}
		}

		m_apaiCultureRangeCities[eOwnerIndex][iRangeIndex] += iChange;
/************************************************************************************************/
/* Afforess	                  Start		 02/15/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		if (bUpdateCulture)
		{
			if (bOldCultureRangeCities != isCultureRangeCity(eOwnerIndex, iRangeIndex))
			{
				updateCulture(true, bUpdatePlotGroups);
			}
		}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/
	}
}


int CvPlot::getInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible) const
{
	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eInvisible >= 0, "eInvisible is expected to be non-negative (invalid Index)");
	FAssertMsg(eInvisible < GC.getNumInvisibleInfos(), "eInvisible is expected to be within maximum bounds (invalid Index)");

	if (NULL == m_apaiInvisibleVisibilityCount)
	{
		return 0;
	}
	else if (NULL == m_apaiInvisibleVisibilityCount[eTeam])
	{
		return 0;
	}

	return m_apaiInvisibleVisibilityCount[eTeam][eInvisible];
}


bool CvPlot::isInvisibleVisible(TeamTypes eTeam, InvisibleTypes eInvisible)	const
{
	return (getInvisibleVisibilityCount(eTeam, eInvisible) > 0);
}


void CvPlot::changeInvisibleVisibilityCount(TeamTypes eTeam, InvisibleTypes eInvisible, int iChange, int iIntensity, int iUnitID)
{
	bool bOldInvisibleVisible;

	FAssertMsg(eTeam >= 0, "eTeam is expected to be non-negative (invalid Index)");
	FAssertMsg(eTeam < MAX_TEAMS, "eTeam is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eInvisible >= 0, "eInvisible is expected to be non-negative (invalid Index)");
	FAssertMsg(eInvisible < GC.getNumInvisibleInfos(), "eInvisible is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(iIntensity >= 0, "iIntensity is expected to be non-negative (invalid Index)");

	if (iChange != 0)
	{
		bOldInvisibleVisible = isInvisibleVisible(eTeam, eInvisible);

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

		if (GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
		{
			bool bFound = false;
			if (getInvisibleVisibilityCount(eTeam, eInvisible) != 0)
			{
				int iSize = getNumPlotTeamVisibilityIntensity();
				for (int iI = 0; iI < iSize; iI++)
				{
					if (getPlotTeamVisibilityIntensity(iI).eInvisibility == eInvisible &&
						getPlotTeamVisibilityIntensity(iI).eTeam == eTeam &&
						getPlotTeamVisibilityIntensity(iI).iUnitID == iUnitID)
					{
						int iNewIntensity = std::max(0, (iIntensity * iChange));
						if (iNewIntensity == 0)
						{
							m_aPlotTeamVisibilityIntensity.erase(m_aPlotTeamVisibilityIntensity.begin()+iI);
						}
						else
						{
							getPlotTeamVisibilityIntensity(iI).iIntensity = iNewIntensity;
						}
						bFound = true;
						break;
					}
				}
				if (!bFound && iChange > 0 && iIntensity > 0)
				{
					int iISize = iSize;
					iSize++;
					m_aPlotTeamVisibilityIntensity.resize(iSize);
					m_aPlotTeamVisibilityIntensity[iISize].eInvisibility = eInvisible;
					m_aPlotTeamVisibilityIntensity[iISize].eTeam = eTeam;
					m_aPlotTeamVisibilityIntensity[iISize].iUnitID = iUnitID;
					m_aPlotTeamVisibilityIntensity[iISize].iIntensity = iIntensity;
				}
			}
			//setHighestPlotTeamVisibilityIntensity(eInvisible, eTeam);
		}
		if (bOldInvisibleVisible != isInvisibleVisible(eTeam, eInvisible))
		{
			if (eTeam == GC.getGame().getActiveTeam())
			{
				updateCenterUnit();
			}
		}
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

int CvPlot::getNumPlotTeamVisibilityIntensityCount(InvisibleTypes eInvisibility, TeamTypes eTeam, int iIntensity) const
{
	int iResult = 0;
	for (int iI = 0; iI < getNumPlotTeamVisibilityIntensity(); iI++)
	{
		if (m_aPlotTeamVisibilityIntensity[iI].eInvisibility == eInvisibility &&
			m_aPlotTeamVisibilityIntensity[iI].eTeam == eTeam &&
			m_aPlotTeamVisibilityIntensity[iI].iIntensity == iIntensity)
		{
			iResult++;
			break;
		}
	}
	return iResult;
}

bool CvPlot::hasPlotTeamVisibilityIntensity(InvisibleTypes eInvisibility, TeamTypes eTeam, int iIntensity) const
{
	return (getNumPlotTeamVisibilityIntensityCount(eInvisibility, eTeam, iIntensity) > 0 );
}

int CvPlot::getHighestPlotTeamVisibilityIntensity(InvisibleTypes eInvisibility, TeamTypes eTeam) const
{
	int iResult = 0;
	//if (m_apaiCachedHighestTeamInvisibilityIntensity != NULL &&
	//	m_apaiCachedHighestTeamInvisibilityIntensity[eTeam] != NULL)
	//{
	//	iResult = m_apaiCachedHighestTeamInvisibilityIntensity[eTeam][eInvisibility];
	//}
	//return iResult;
	int iHighestResult = 0;

	for (int iI = 0; iI < getNumPlotTeamVisibilityIntensity(); iI++)
	{
		if (m_aPlotTeamVisibilityIntensity[iI].eInvisibility == eInvisibility &&
			m_aPlotTeamVisibilityIntensity[iI].eTeam == eTeam)
		{
			iResult = m_aPlotTeamVisibilityIntensity[iI].iIntensity;
			if (iResult > iHighestResult)
			{
				iHighestResult = iResult;
			}
		}
	}
	return iHighestResult;
}
//
//void CvPlot::setHighestPlotTeamVisibilityIntensity(InvisibleTypes eInvisibility, TeamTypes eTeam)
//{
//	if (NULL == m_apaiCachedHighestTeamInvisibilityIntensity)
//	{
//		m_apaiCachedHighestTeamInvisibilityIntensity = new short*[MAX_TEAMS];
//		for (int iI = 0; iI < MAX_TEAMS; ++iI)
//		{
//			m_apaiCachedHighestTeamInvisibilityIntensity[iI] = NULL;
//		}
//	}
//
//	if (NULL == m_apaiCachedHighestTeamInvisibilityIntensity[eTeam])
//	{
//		m_apaiCachedHighestTeamInvisibilityIntensity[eTeam] = new short[GC.getNumInvisibleInfos()];
//		for (int iI = 0; iI < GC.getNumInvisibleInfos(); ++iI)
//		{
//			m_apaiCachedHighestTeamInvisibilityIntensity[eTeam][iI] = -1;
//		}
//	}
//
//	int iHighestResult = 0;
//	int iResult = 0;
//
//	//if (m_apaiCachedHighestTeamInvisibilityIntensity[eTeam][eInvisibility] == -1)
//	//{
//		//int iHighestResult = 0;
//		//int iResult = 0;
//		for (int iI = 0; iI < getNumPlotTeamVisibilityIntensity(); iI++)
//		{
//			if (m_aPlotTeamVisibilityIntensity[iI].eInvisibility == eInvisibility &&
//				m_aPlotTeamVisibilityIntensity[iI].eTeam == eTeam &&
//				m_aPlotTeamVisibilityIntensity[iI].iCount > 0)
//			{
//				iResult = m_aPlotTeamVisibilityIntensity[iI].iIntensity;
//				if (iResult > iHighestResult)
//				{
//					iHighestResult = iResult;
//				}
//			}
//		}
//		m_apaiCachedHighestTeamInvisibilityIntensity[eTeam][eInvisibility] = iHighestResult;
//	//}
//}
int CvPlot::getNumUnits() const
{
	return m_units.getLength();
}


CvUnit* CvPlot::getUnitByIndex(int iIndex) const
{
	CLLNode<IDInfo>* pUnitNode;

	pUnitNode = m_units.nodeNum(iIndex);

	if (pUnitNode != NULL)
	{
		return ::getUnit(pUnitNode->m_data);
	}
	else
	{
		return NULL;
	}
}


void CvPlot::addUnit(CvUnit* pUnit, bool bUpdate)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	FAssertMsg(pUnit->at(getX(), getY()), "pUnit is expected to be at getX and getY");

	//DEBUG attempt
	if (GC.getGame().isDebugMode())
	{
		pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);

			FAssertMsg(pUnit != pLoopUnit, "unit being added to plot's group as a duplicate");

			pUnitNode = nextUnitNode(pUnitNode);
		}
	}

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);

		if (pLoopUnit != NULL)
		{
			if (!isBeforeUnitCycle(pLoopUnit, pUnit))
			{
				break;
			}
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
}


void CvPlot::removeUnit(CvUnit* pUnit, bool bUpdate)
{
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
	int i;
	for(i=0;i<getNumSymbols();i++)
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
	PROFILE("CvPlot::doFeature()")

	CvCity* pCity;
	CvPlot* pLoopPlot;
	CvWString szBuffer;
	int iProbability;
	int iI, iJ;
/************************************************************************************************/
/* Afforess	                  Start		 2/9/10                                         */
/*   Feature Sound Effect                                                                       */
/*  (Default Sound Effect is Forest Growth if no XML value is set)                              */
/************************************************************************************************/
	TCHAR szSound[1024] = "AS2D_FEATUREGROWTH";
/************************************************************************************************/
/* Afforess	                     END                                                        */
/************************************************************************************************/

/************************************************************************************************/
/* DCM	                  Start		 05/31/10                        Johnny Smith               */
/*                                                                   Afforess                   */
/* Battle Effects                                                                               */
/************************************************************************************************/
	// Dale - BE: Battle Effect START
	if (GC.isDCM_BATTLE_EFFECTS())
	{
		if(getBattleCountdown() > 0)
		{
			changeBattleCountdown(-1);
		}
	}
	// Dale - BE: Battle Effect END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
	if (getFeatureType() != NO_FEATURE)
	{
		iProbability = GC.getFeatureInfo(getFeatureType()).getDisappearanceProbability();

		if (iProbability > 0)
		{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       03/04/10                                jdog5000      */
/*                                                                                              */
/* Gamespeed scaling                                                                            */
/************************************************************************************************/
/* original bts code
			if (GC.getGame().getSorenRandNum(10000, "Feature Disappearance") < iProbability)
*/
			int iOdds = (10000*GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getVictoryDelayPercent())/100;
			if (GC.getGame().getSorenRandNum(iOdds, "Feature Disappearance") < iProbability)
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
			{
				setFeatureType(NO_FEATURE);
			}
		}
	}
	else
	{
/************************************************************************************************/
/* Afforess	                  Start		 05/21/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
		int iStorm = GC.getInfoTypeForString("FEATURE_STORM", true);
		bool bNoStorms = GC.getGame().isModderGameOption(MODDERGAMEOPTION_NO_STORMS);

		for (iI = 0; iI < GC.getNumFeatureInfos(); ++iI)
		{
			CvFeatureInfo& kFeature = GC.getFeatureInfo((FeatureTypes)iI);
			if ((kFeature.getGrowthProbability() > 0 && !isUnit()) || kFeature.getSpreadProbability() > 0)
			{
				if (getImprovementType() == NO_IMPROVEMENT || (kFeature.isCanGrowAnywhere() && !isBeingWorked() && !isWater()))
				{
					if (!((iStorm == iI) && bNoStorms))
					{
						if (canHaveFeature((FeatureTypes)iI, kFeature.getSpreadProbability() > 0))
						{
							if ((getBonusType() == NO_BONUS) || (GC.getBonusInfo(getBonusType()).isFeature(iI)) || kFeature.getSpreadProbability() > 0)
							{
								iProbability = kFeature.isCanGrowAnywhere() ? kFeature.getGrowthProbability() : 0;

								for (iJ = 0; iJ < NUM_CARDINALDIRECTION_TYPES; iJ++)
								{
									pLoopPlot = plotCardinalDirection(getX(), getY(), ((CardinalDirectionTypes)iJ));

									if (pLoopPlot != NULL)
									{
										if (pLoopPlot->getFeatureType() == ((FeatureTypes)iI))
										{
											if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
											{
												iProbability += kFeature.getGrowthProbability();
											}
											else
											{
												iProbability += GC.getImprovementInfo(pLoopPlot->getImprovementType()).getFeatureGrowthProbability();
											}
											iProbability += kFeature.getSpreadProbability();
										}
									}
								}

								iProbability *= std::max(0, (GC.getFEATURE_GROWTH_MODIFIER() + 100));
								iProbability /= 100;

								if (isRoute())
								{
									iProbability *= std::max(0, (GC.getROUTE_FEATURE_GROWTH_MODIFIER() + 100));
									iProbability /= 100;
								}

								if (iProbability > 0)
								{
									int iOdds = (10000*GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getVictoryDelayPercent())/100;
									if (GC.getGame().getSorenRandNum(iOdds, "Feature Growth") < iProbability)
									{
										setFeatureType((FeatureTypes)iI);
										strcpy(szSound, kFeature.getGrowthSound());
										pCity = GC.getMap().findCity(getX(), getY(), getOwner(), NO_TEAM, false);
										//if (GC.getGame().getElapsedGameTurns() > 1)
										//{
											/*if (kFeature.isCanGrowAnywhere() && getImprovementType() != NO_IMPROVEMENT && !isWater())
											{
												setOvergrown(true);
												if (pCity != NULL)
												{
													szBuffer = gDLL->getText("TXT_KEY_MISC_FEATURE_OVERGROWN_NEAR_CITY", kFeature.getTextKeyWide(), GC.getImprovementInfo(getImprovementType()).getTextKeyWide(), pCity->getNameKey());
													AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, szSound, MESSAGE_TYPE_INFO, kFeature.getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX(), getY(), true, true);
												}
											}*/
										//}

										if (pCity != NULL && isInViewport())
										{
											MEMORY_TRACK_EXEMPT();

											// Tell the owner of this city.
											if (iI == iStorm)
											{
												szBuffer = gDLL->getText("TXT_KEY_MISC_STORM_GROWN_NEAR_CITY", pCity->getNameKey());
											}
											else
											{
												szBuffer = gDLL->getText("TXT_KEY_MISC_FEATURE_GROWN_NEAR_CITY", kFeature.getTextKeyWide(), pCity->getNameKey());
											}

											AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, szSound, MESSAGE_TYPE_INFO, kFeature.getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getViewportX(),getViewportY(), true, true);
										}
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

void CvPlot::doCulture()
{
	PROFILE("CvPlot::doCulture()")

	CLLNode<IDInfo>* pUnitNode;
	CvCity* pCity;
	CvUnit* pLoopUnit;
	CvWString szBuffer;
	PlayerTypes eCulturalOwner;
	int iCityStrength;

	// Super Forts begin *culture*
	doImprovementCulture();

	ImprovementTypes eImprovement = getImprovementType();
	if(eImprovement != NO_IMPROVEMENT)
	{
		// Check for a fort culture flip
		if(GC.getImprovementInfo(eImprovement).isActsAsCity() && (getOwnershipDuration() > GC.getDefineINT("SUPER_FORTS_DURATION_BEFORE_REVOLT")))
		{
			eCulturalOwner = calculateCulturalOwner();
			if(eCulturalOwner != NO_PLAYER)
			{
				if(GET_PLAYER(eCulturalOwner).getTeam() != getTeam())
				{
					bool bDefenderFound = false;
					CLinkList<IDInfo> oldUnits;
					pUnitNode = headUnitNode();

					while (pUnitNode != NULL)
					{
						oldUnits.insertAtEnd(pUnitNode->m_data);
						pUnitNode = nextUnitNode(pUnitNode);
					}

					pUnitNode = oldUnits.head();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = nextUnitNode(pUnitNode);
						if(pLoopUnit->canDefend(this))
						{
							if(pLoopUnit->getOwner() == getOwner())
							{
								bDefenderFound = true;
								break;
							}
						}
					}
					if(!bDefenderFound)
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_REVOLTED_JOINED", GC.getImprovementInfo(getImprovementType()).getText(), GET_PLAYER(eCulturalOwner).getCivilizationDescriptionKey());
						AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_INFO, GC.getImprovementInfo(getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getX(), getY(), true, true);
						AddDLLMessage(eCulturalOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTUREFLIP", MESSAGE_TYPE_INFO, GC.getImprovementInfo(getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getX(), getY(), true, true);
						setOwner(eCulturalOwner,true,true);
					}
				}
			}
		}
	}
	// Super Forts end

	pCity = getPlotCity();

	if (pCity != NULL)
	{
		eCulturalOwner = calculateCulturalOwner();

		if (eCulturalOwner != NO_PLAYER)
		{
			if (GET_PLAYER(eCulturalOwner).getTeam() != getTeam())
			{
				if (!(pCity->isOccupation()))
				{
					if (GC.getGame().getSorenRandNum(100, "Revolt #1") < pCity->getRevoltTestProbability())
					{
						int iOriginal = 0;
						iCityStrength = pCity->cultureStrength(eCulturalOwner, iOriginal);

						if ((GC.getGame().getSorenRandNum(100, "Revolt #2") < iCityStrength) || pCity->isNPC())
						{
							CLinkList<IDInfo> oldUnits;

							pUnitNode = headUnitNode();

							while (pUnitNode != NULL)
							{
								oldUnits.insertAtEnd(pUnitNode->m_data);
								pUnitNode = nextUnitNode(pUnitNode);
							}

							pUnitNode = oldUnits.head();

							while (pUnitNode != NULL)
							{
								pLoopUnit = ::getUnit(pUnitNode->m_data);
								pUnitNode = nextUnitNode(pUnitNode);

								if (pLoopUnit)
								{
									if (pLoopUnit->isNPC())
									{
										pLoopUnit->kill(false, eCulturalOwner);
									}
									else if (pLoopUnit->canDefend())
									{
										pLoopUnit->changeDamage((pLoopUnit->currHitPoints() / 2), eCulturalOwner);
									}
								}

							}

							// Disable classic city flip by culture when Revolution is running
							if (pCity->isNPC() || (!GC.getGame().isOption(GAMEOPTION_NO_CITY_FLIPPING)
								&& (GC.getGame().isOption(GAMEOPTION_FLIPPING_AFTER_CONQUEST) || !pCity->isEverOwned(eCulturalOwner))
								&& pCity->getNumRevolts(eCulturalOwner) >= GC.getDefineINT("NUM_WARNING_REVOLTS")))
							{
								if (GC.getGame().isOption(GAMEOPTION_ONE_CITY_CHALLENGE))
								{
									pCity->kill(true);
								}
								else
								{
									setOwner(eCulturalOwner, true, true); // Will invalidate pCity pointer.
								}
								pCity = NULL;
							}
							else
							{
								pCity->changeNumRevolts(eCulturalOwner, 1);
								pCity->changeOccupationTimer(GC.getDefineINT("BASE_REVOLT_OCCUPATION_TURNS") + ((iCityStrength * GC.getDefineINT("REVOLT_OCCUPATION_TURNS_PERCENT")) / 100));

								// XXX announce for all seen cities?
								MEMORY_TRACK_EXEMPT();

								if (isInViewport())
								{
									szBuffer = gDLL->getText("TXT_KEY_MISC_REVOLT_IN_CITY", GET_PLAYER(eCulturalOwner).getCivilizationAdjective(), pCity->getNameKey());
									AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_RESISTANCE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), getViewportX(),getViewportY(), true, true);
									AddDLLMessage(eCulturalOwner, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITY_REVOLT", MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_RESISTANCE")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), getViewportX(),getViewportY(), true, true);
								}
							}
						}
					}
				}
			}
		}
	}


	//Decay if you've lost any source of culture on the plot
	if (getOwner() != NO_PLAYER)
	{
		if (!isWithinCultureRange(getOwner()) && !isWithinOccupationRange(getOwner()))
		{
			int iCultureBase = getCulture(getOwner());
			if (iCultureBase >0)
			{
				iCultureBase /= 2;
				iCultureBase -= 10;
				iCultureBase = std::max(iCultureBase, 0);
				setCulture(getOwner(), iCultureBase, true, true);
			}
		}
	}

	updateCulture(true, true);
}


void CvPlot::processArea(CvArea* pArea, int iChange)
{
	CvCity* pCity;
	int iI, iJ;

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

	for (iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		if (GET_PLAYER((PlayerTypes)iI).getStartingPlot() == this)
		{
			pArea->changeNumStartingPlots(iChange);
		}

		pArea->changePower(((PlayerTypes)iI), (getUnitPower((PlayerTypes)iI) * iChange));

		pArea->changeUnitsPerPlayer(((PlayerTypes)iI), (plotCount(PUF_isPlayer, iI) * iChange));
		pArea->changeAnimalsPerPlayer(((PlayerTypes)iI), (plotCount(PUF_isAnimal, -1, -1, NULL, ((PlayerTypes)iI)) * iChange));

		for (iJ = 0; iJ < NUM_UNITAI_TYPES; iJ++)
		{
			pArea->changeNumAIUnits(((PlayerTypes)iI), ((UnitAITypes)iJ), (plotCount(PUF_isUnitAIType, iJ, -1, NULL, ((PlayerTypes)iI)) * iChange));
		}
	}

	for (iI = 0; iI < MAX_TEAMS; ++iI)
	{
		if (isRevealed(((TeamTypes)iI), false))
		{
			pArea->changeNumRevealedTiles(((TeamTypes)iI), iChange);
		}
	}

	pCity = getPlotCity();

	if (pCity != NULL)
	{
		// XXX make sure all of this (esp. the changePower()) syncs up...
		pArea->changePower(pCity->getOwner(), (getPopulationPower(pCity->getPopulation()) * iChange));

		pArea->changeCitiesPerPlayer(pCity->getOwner(), iChange);
		pArea->changePopulationPerPlayer(pCity->getOwner(), (pCity->getPopulation() * iChange));

		for (iI = 0; iI < GC.getNumBuildingInfos(); ++iI)
		{
			if (pCity->getNumActiveBuilding((BuildingTypes)iI) > 0)
			{
				pArea->changePower(pCity->getOwner(), ((GC.getBuildingInfo((BuildingTypes)iI).getPowerValue()/100) * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
				//Team Project (5)
				if (!pCity->isReligiouslyDisabledBuilding((BuildingTypes)iI))
				{
					if (GC.getBuildingInfo((BuildingTypes) iI).getAreaHealth() > 0)
					{
						pArea->changeBuildingGoodHealth(pCity->getOwner(), (GC.getBuildingInfo((BuildingTypes)iI).getAreaHealth() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
					}
					else
					{
						pArea->changeBuildingBadHealth(pCity->getOwner(), (GC.getBuildingInfo((BuildingTypes)iI).getAreaHealth() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
					}
					pArea->changeBuildingHappiness(pCity->getOwner(), (GC.getBuildingInfo((BuildingTypes)iI).getAreaHappiness() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
					pArea->changeFreeSpecialist(pCity->getOwner(), (GC.getBuildingInfo((BuildingTypes)iI).getAreaFreeSpecialist() * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
				}

				pArea->changeCleanPowerCount(pCity->getTeam(), ((GC.getBuildingInfo((BuildingTypes)iI).isAreaCleanPower()) ? iChange * pCity->getNumActiveBuilding((BuildingTypes)iI) : 0));

				pArea->changeBorderObstacleCount(pCity->getTeam(), ((GC.getBuildingInfo((BuildingTypes)iI).isAreaBorderObstacle()) ? iChange * pCity->getNumActiveBuilding((BuildingTypes)iI) : 0));

				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					//Team Project (5)
					if (!pCity->isReligiouslyDisabledBuilding((BuildingTypes)iI))
					{
						pArea->changeYieldRateModifier(pCity->getOwner(), ((YieldTypes)iJ), (GC.getBuildingInfo((BuildingTypes)iI).getAreaYieldModifier(iJ) * iChange * pCity->getNumActiveBuilding((BuildingTypes)iI)));
					}
				}
			}
		}

		for (iI = 0; iI < NUM_UNITAI_TYPES; ++iI)
		{
			pArea->changeNumTrainAIUnits(pCity->getOwner(), ((UnitAITypes)iI), (pCity->getNumTrainUnitAI((UnitAITypes)iI) * iChange));
		}

		for (iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			if (pArea->getTargetCity((PlayerTypes)iI) == pCity)
			{
				pArea->setTargetCity(((PlayerTypes)iI), NULL);
			}
		}
	}
}


ColorTypes CvPlot::plotMinimapColor()
{
	CvUnit* pCenterUnit;

	if (GC.getGame().getActivePlayer() != NO_PLAYER)
	{
		CvCity* pCity;

		pCity = getPlotCity();

		if ((pCity != NULL) && pCity->isRevealed(GC.getGame().getActiveTeam(), true))
		{
			return (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE");
		}

		if (isActiveVisible(true))
		{
			pCenterUnit = getDebugCenterUnit();

			if (pCenterUnit != NULL)
			{
				return ((ColorTypes)(GC.getPlayerColorInfo(GET_PLAYER(pCenterUnit->getVisualOwner()).getPlayerColor()).getColorTypePrimary()));
			}
		}

		if ((getRevealedOwner(GC.getGame().getActiveTeam(), true) != NO_PLAYER) && !isRevealedBarbarian())
		{
			return ((ColorTypes)(GC.getPlayerColorInfo(GET_PLAYER(getRevealedOwner(GC.getGame().getActiveTeam(), true)).getPlayerColor()).getColorTypePrimary()));
		}
	}

	return (ColorTypes)GC.getInfoTypeForString("COLOR_CLEAR");
}

//
// read object from a stream
// used during load
//
void CvPlot::read(FDataStreamBase* pStream)
{
	int iI;
	bool bVal;
	char cCount;
	int iCount;

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_READ_OBJECT_START(wrapper);

	// Init saved data
	reset();

	uint uiFlag=0;
	WRAPPER_READ(wrapper, "CvPlot", &uiFlag);	// flags for expansion

/************************************************************************************************/
/* DCM	                  Start		 05/31/10                        Johnny Smith               */
/*                                                                   Afforess                   */
/* Battle Effects                                                                               */
/************************************************************************************************/
	// Dale - BE: Battle Effect START
	WRAPPER_READ(wrapper, "CvPlot", &m_iBattleCountdown);
	// Dale - BE: Battle Effect END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
	WRAPPER_READ(wrapper, "CvPlot", &m_iX);
	WRAPPER_READ(wrapper, "CvPlot", &m_iY);
	WRAPPER_READ(wrapper, "CvPlot", &m_iArea);
	// m_pPlotArea not saved

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
	WRAPPER_READ(wrapper, "CvPlot", &m_iImprovementDuration);
	WRAPPER_READ(wrapper, "CvPlot", &m_iUpgradeProgress);
	m_iUpgradeProgress *= 100;	//	Convert legacy granularity to hundredths
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &m_iUpgradeProgress, "m_iUpdateProgressHundredths");
	WRAPPER_READ(wrapper, "CvPlot", &m_iForceUnownedTimer);
	WRAPPER_READ(wrapper, "CvPlot", &m_iCityRadiusCount);
	WRAPPER_READ(wrapper, "CvPlot", &m_iRiverID);
	WRAPPER_READ(wrapper, "CvPlot", &m_iMinOriginalStartDist);
	WRAPPER_READ(wrapper, "CvPlot", &m_iReconCount);
	WRAPPER_READ(wrapper, "CvPlot", &m_iRiverCrossingCount);

	// Super Forts begin *canal* *choke*
	WRAPPER_READ(wrapper, "CvPlot", &m_iCanalValue);
	WRAPPER_READ(wrapper, "CvPlot", &m_iChokeValue);
	// Super Forts end
	// Super Forts begin *bombard*
	WRAPPER_READ(wrapper, "CvPlot", &m_iDefenseDamage);
	WRAPPER_READ(wrapper, "CvPlot", &m_bBombarded);
	// Super Forts end

	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bStartingPlot");
	m_bStartingPlot = bVal;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bHills");
	m_bHills = bVal;
/************************************************************************************************/
/* Afforess	Mountains Start		 08/03/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/

	WRAPPER_READ(wrapper, "CvPlot", &m_bDepletedMine); // deleteMe
	WRAPPER_READ_STRING(wrapper, "CvPlot", m_szLandmarkMessage);
	WRAPPER_READ_STRING(wrapper, "CvPlot", m_szLandmarkName);
	WRAPPER_READ(wrapper, "CvPlot", &m_eClaimingOwner);
	WRAPPER_READ(wrapper, "CvPlot", (int*)&m_eLandmarkType);

	SAFE_DELETE_ARRAY(m_aiOccupationCultureRangeCities);
	cCount = 0;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_aiOccupationCultureRangeCities = new char[cCount];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCount, m_aiOccupationCultureRangeCities);
	}

	WRAPPER_READ(wrapper, "CvPlot", &m_zobristContribution);
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bNOfRiver");
	m_bNOfRiver = bVal;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bWOfRiver");
	m_bWOfRiver = bVal;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bIrrigated");
	m_bIrrigated = bVal;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &bVal, "m_bPotentialCityWork");
	m_bPotentialCityWork = bVal;
	// m_bShowCitySymbols not saved
	// m_bFlagDirty not saved
	// m_bPlotLayoutDirty not saved
	// m_bLayoutStateWorked not saved

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

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
	// Plot danger cache
	m_bIsActivePlayerNoDangerCache = false;
	m_bIsActivePlayerHasDangerCache = false;
	invalidateIsTeamBorderCache();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	m_aiCulture.clear();

	int buffer[MAX_PLAYERS];
	cCount = 0;
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		FAssert(cCount <= MAX_PLAYERS);
		WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvPlot", cCount, buffer, "m_aiCulture");

		for(iI = 0; iI < (int)cCount; iI++)
		{
			if ( buffer[iI] != 0 )
			{
				m_aiCulture.push_back(std::make_pair((PlayerTypes)iI, buffer[iI]));
			}
		}
	}
	//	Force the capacity to the size() since we need to mto minimize memory usage
	//	and adding cultures is rare
	std::vector<std::pair<PlayerTypes,int> >(m_aiCulture).swap(m_aiCulture);

	// Super Forts begin *culture*
#ifdef UNUSED_SUPER_FORTS_METHODS
	SAFE_DELETE_ARRAY(m_aiCultureRangeForts);
	char cCultureRangeFortValuesPresent = (char)-1;
	WRAPPER_READ(wrapper, "CvPlot", &cCultureRangeFortValuesPresent);
	if ( cCultureRangeFortValuesPresent != (char)-1 )
	{
		short* m_aiCultureRangeForts = new short[cCultureRangeFortValuesPresent];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cCultureRangeFortValuesPresent, m_aiCultureRangeForts);
	}
#endif
	// Super Forts end

	SAFE_DELETE_ARRAY(m_aiFoundValue);
	char cFoundValuesPresent = (char)-1;
	WRAPPER_READ(wrapper, "CvPlot", &cFoundValuesPresent);
	if ( cFoundValuesPresent != (char)-1 )
	{
		unsigned int* m_aiFoundValue = new unsigned int[cFoundValuesPresent];
		WRAPPER_READ_ARRAY(wrapper, "CvPlot", cFoundValuesPresent, m_aiFoundValue);
	}
	else
	{
		//	Handle old format
		cCount = 0;
		WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
		if (cCount > 0)
		{
			unsigned short* tempFoundValue = new unsigned short[cCount];
			WRAPPER_READ_ARRAY_DECORATED(wrapper, "CvPlot", cCount, (short*)tempFoundValue, "m_aiFoundValue");

			//	Note - the m_aiFoundValue values were scaled by a factor of 10 to
			//	address observed overflow issues, so the loaded values could be out
			//	by factor.  However, they are regenerated each turn and essentially only
			//	compared to each other, so this should not cause significant issues
			m_aiFoundValue = new unsigned int[cCount];

			for(int iI = 0; iI < (int)cCount; iI++ )
			{
				m_aiFoundValue[iI] = 10*tempFoundValue[iI];
			}

			SAFE_DELETE_ARRAY(tempFoundValue);
		}
	}

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

	SAFE_DELETE_ARRAY(m_paiBuildProgress);

	//	Old format just saved these as a short array
	//	Unfortunately there was a screwed up intermediary format that saved as a class
	//	array if the array was non-NULL on save but DIDN'T save the count!  Dealing
	//	with all of these variants makes this next section a little complex
	iCount = -1;
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

	if ( iCount > 0 || (iCount == -1 && wrapper.isUsingTaggedFormat()) )
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

	if (NULL != m_apaiCultureRangeCities)
	{
		for (int iI = 0; iI < MAX_PLAYERS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiCultureRangeCities[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiCultureRangeCities);
	}
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_apaiCultureRangeCities = new char*[cCount];
		for (iI = 0; iI < cCount; ++iI)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iCount, "CultureLevelCount");
			if (iCount > 0)
			{
				m_apaiCultureRangeCities[iI] = new char[iCount >= GC.getNumCultureLevelInfos()? iCount : GC.getNumCultureLevelInfos()];
				WRAPPER_READ_ARRAY(wrapper, "CvPlot", iCount, m_apaiCultureRangeCities[iI]);

				for(int iJ = iCount; iJ < GC.getNumCultureLevelInfos(); iJ++)
				{
					m_apaiCultureRangeCities[iI][iJ] = 0;
				}
			}
			else
			{
				m_apaiCultureRangeCities[iI] = NULL;
			}
		}
	}

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
	WRAPPER_READ(wrapper, "CvPlot", &m_bIgnoringImprovementUpgrade);
	WRAPPER_READ(wrapper, "CvPlot", &m_iCanHaveImprovementAsUpgradeCache);
	WRAPPER_READ(wrapper, "CvPlot", &m_iCurrentRoundofUpgradeCache);
	WRAPPER_READ(wrapper, "CvPlot", &m_iImprovementCurrentValue);

	int iSize = 0;
	WRAPPER_READ(wrapper, "CvPlot", &iSize);
	m_aPlotTeamVisibilityIntensity.resize(iSize);
	int iType1 = 0;
	int iType2 = 0;
	int iType3 = 0;
	int iType4 = 0;
	if (iSize > 0)
	{
		for (iI = 0; iI < iSize; iI++)
		{
			WRAPPER_READ(wrapper, "CvPlot", &iType1);
			WRAPPER_READ(wrapper, "CvPlot", &iType2);
			WRAPPER_READ(wrapper, "CvPlot", &iType3);
			WRAPPER_READ(wrapper, "CvPlot", &iType4);
			m_aPlotTeamVisibilityIntensity[iI].eInvisibility = ((InvisibleTypes)iType1);
			m_aPlotTeamVisibilityIntensity[iI].eTeam = ((TeamTypes)iType2);
			m_aPlotTeamVisibilityIntensity[iI].iUnitID = iType3;
			m_aPlotTeamVisibilityIntensity[iI].iIntensity = iType4;
		}
	}
/*
	if (NULL != m_apaiCachedHighestTeamInvisibilityIntensity)
	{
		for (int iI = 0; iI < MAX_TEAMS; ++iI)
		{
			SAFE_DELETE_ARRAY(m_apaiCachedHighestTeamInvisibilityIntensity[iI]);
		}
		SAFE_DELETE_ARRAY(m_apaiCachedHighestTeamInvisibilityIntensity);
	}
	WRAPPER_READ_DECORATED(wrapper, "CvPlot", &cCount, "cConditional");
	if (cCount > 0)
	{
		m_apaiCachedHighestTeamInvisibilityIntensity = new short*[cCount];
		for (iI = 0; iI < cCount; ++iI)
		{
			WRAPPER_READ_DECORATED(wrapper, "CvPlot", &iCount, "CachedHighestTeamInvisibilityIntensity");
			if (iCount > 0)
			{
				m_apaiCachedHighestTeamInvisibilityIntensity[iI] = new short[iCount];
				WRAPPER_READ_ARRAY(wrapper, "CvPlot", iCount, m_apaiCachedHighestTeamInvisibilityIntensity[iI]);
			}
			else
			{
				m_apaiCachedHighestTeamInvisibilityIntensity[iI] = NULL;
			}
		}
	}*///skipped when this was the last element - you can safely do that.
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
	uint iI;

	CvTaggedSaveFormatWrapper&	wrapper = CvTaggedSaveFormatWrapper::getSaveFormatWrapper();

	wrapper.AttachToStream(pStream);

	WRAPPER_WRITE_OBJECT_START(wrapper);

	uint uiFlag=0;
	WRAPPER_WRITE(wrapper, "CvPlot", uiFlag);		// flag for expansion

/************************************************************************************************/
/* DCM	                  Start		 05/31/10                        Johnny Smith               */
/*                                                                   Afforess                   */
/* Battle Effects                                                                               */
/************************************************************************************************/
	// Dale - BE: Battle Effect START
	WRAPPER_WRITE(wrapper, "CvPlot", m_iBattleCountdown);
	// Dale - BE: Battle Effect END
/************************************************************************************************/
/* DCM                                     END                                                  */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvPlot", m_iX);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iY);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iArea);
	// m_pPlotArea not saved

	WRAPPER_WRITE(wrapper, "CvPlot", m_iFeatureVariety);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iOwnershipDuration);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iImprovementDuration);
	WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", m_iUpgradeProgress, "m_iUpdateProgressHundredths");
	WRAPPER_WRITE(wrapper, "CvPlot", m_iForceUnownedTimer);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iCityRadiusCount);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iRiverID);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iMinOriginalStartDist);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iReconCount);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iRiverCrossingCount);

	// Super Forts begin *canal* *choke*
	WRAPPER_WRITE(wrapper, "CvPlot", m_iCanalValue);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iChokeValue);
	// Super Forts end
	// Super Forts begin *bombard*
	WRAPPER_WRITE(wrapper, "CvPlot", m_iDefenseDamage);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bBombarded);
	// Super Forts end

	WRAPPER_WRITE(wrapper, "CvPlot", m_bStartingPlot);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bHills);
/************************************************************************************************/
/* Afforess	Mountains Start		 08/03/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvPlot", m_bDepletedMine); // deleteMe
	WRAPPER_WRITE_STRING(wrapper, "CvPlot", m_szLandmarkMessage);
	WRAPPER_WRITE_STRING(wrapper, "CvPlot", m_szLandmarkName);
	WRAPPER_WRITE(wrapper, "CvPlot", m_eClaimingOwner);
	WRAPPER_WRITE(wrapper, "CvPlot", m_eLandmarkType);

	if (NULL == m_aiOccupationCultureRangeCities)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_PLAYERS, "cConditional");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_PLAYERS, m_aiOccupationCultureRangeCities);
	}

	WRAPPER_WRITE(wrapper, "CvPlot", m_zobristContribution);
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/
	WRAPPER_WRITE(wrapper, "CvPlot", m_bNOfRiver);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bWOfRiver);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bIrrigated);
	WRAPPER_WRITE(wrapper, "CvPlot", m_bPotentialCityWork);
	// m_bShowCitySymbols not saved
	// m_bFlagDirty not saved
	// m_bPlotLayoutDirty not saved
	// m_bLayoutStateWorked not saved

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

		for(iI = 0; iI < MAX_PLAYERS; iI++)
		{
			buffer[iI] = 0;
		}

		for(iI = 0; iI < m_aiCulture.size(); iI++)
		{
			buffer[m_aiCulture[iI].first] = m_aiCulture[iI].second;
		}
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_PLAYERS, "cConditional");
		WRAPPER_WRITE_ARRAY_DECORATED(wrapper, "CvPlot", MAX_PLAYERS, buffer, "m_aiCulture");
	}

	// Super Forts begin *culture*
#ifdef UNUSED_SUPER_FORTS_METHODS
	if (NULL == m_aiCultureRangeForts)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cCultureRangeFortValuesPresent");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_PLAYERS, "cCultureRangeFortValuesPresent");
		WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", MAX_PLAYERS, m_aiCultureRangeForts);
	}
#endif
	// Super Forts end

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

	if (NULL == m_apaiCultureRangeCities)
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	}
	else
	{
		WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_PLAYERS, "cConditional");
		for (iI=0; iI < MAX_PLAYERS; ++iI)
		{
			if (NULL == m_apaiCultureRangeCities[iI])
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (int)0, "CultureLevelCount");
			}
			else
			{
				WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (int)GC.getNumCultureLevelInfos(), "CultureLevelCount");
				WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", GC.getNumCultureLevelInfos(), m_apaiCultureRangeCities[iI]);
			}
		}
	}

	//	Write out plot danger counts for all players that have any here
	int	iPlayersWithDanger = 0;
	for( iI= 0; iI < MAX_PLAYERS; iI++ )
	{
		if ( getDangerCount((PlayerTypes)iI) > 0 )
		{
			iPlayersWithDanger++;
		}
	}
	WRAPPER_WRITE(wrapper, "CvPlot", iPlayersWithDanger);
	if ( iPlayersWithDanger > 0 )
	{
		for( iI= 0; iI < MAX_PLAYERS; iI++ )
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
		for (iI=0; iI < MAX_TEAMS; ++iI)
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
	WRAPPER_WRITE(wrapper, "CvPlot", m_bIgnoringImprovementUpgrade);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iCanHaveImprovementAsUpgradeCache);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iCurrentRoundofUpgradeCache);
	WRAPPER_WRITE(wrapper, "CvPlot", m_iImprovementCurrentValue);

	int iSize = (int)m_aPlotTeamVisibilityIntensity.size();
	int iType1 = 0;
	int iType2 = 0;
	int iType3 = 0;
	int iType4 = 0;
	WRAPPER_WRITE(wrapper, "CvPlot", iSize);
	if (iSize > 0)
	{
		for (iI = 0; iI < (uint)iSize; iI++)
		{
			iType1 = (int)m_aPlotTeamVisibilityIntensity[iI].eInvisibility;
			iType2 = (int)m_aPlotTeamVisibilityIntensity[iI].eTeam;
			iType3 = m_aPlotTeamVisibilityIntensity[iI].iUnitID;
			iType4 = m_aPlotTeamVisibilityIntensity[iI].iIntensity;
			WRAPPER_WRITE(wrapper, "CvPlot", iType1);
			WRAPPER_WRITE(wrapper, "CvPlot", iType2);
			WRAPPER_WRITE(wrapper, "CvPlot", iType3);
			WRAPPER_WRITE(wrapper, "CvPlot", iType4);
		}
	}

	//if (NULL == m_apaiCachedHighestTeamInvisibilityIntensity)
	//{
	//	WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)0, "cConditional");
	//}
	//else
	//{
	//	WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (char)MAX_TEAMS, "cConditional");
	//	for (iI=0; iI < MAX_TEAMS; ++iI)
	//	{
	//		if (NULL == m_apaiCachedHighestTeamInvisibilityIntensity[iI])
	//		{
	//			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", (int)0, "CachedHighestTeamInvisibilityIntensity");
	//		}
	//		else
	//		{
	//			WRAPPER_WRITE_DECORATED(wrapper, "CvPlot", GC.getNumInvisibleInfos(), "CachedHighestTeamInvisibilityIntensity");
	//			WRAPPER_WRITE_ARRAY(wrapper, "CvPlot", GC.getNumInvisibleInfos(), m_apaiCachedHighestTeamInvisibilityIntensity[iI]);
	//		}
	//	}
	//}


	WRAPPER_WRITE_OBJECT_END(wrapper);
}

void CvPlot::setLayoutDirty(bool bDirty)
{
	if (!GC.IsGraphicsInitialized())
	{
		return;
	}

	if ( !isInViewport() )
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

void CvPlot::getVisibleImprovementState(ImprovementTypes& eType, bool& bWorked)
{
	eType = NO_IMPROVEMENT;
	bWorked = false;

	if (GC.getGame().getActiveTeam() == NO_TEAM)
	{
		return;
	}

	eType = getRevealedImprovementType(GC.getGame().getActiveTeam(), true);

	if (eType == NO_IMPROVEMENT)
	{
		if (isActiveVisible(true))
		{
			if (isBeingWorked() && !isCity())
			{
				if (isWater())
				{
					eType = ((ImprovementTypes)(GC.getDefineINT("WATER_IMPROVEMENT")));
				}
				else
				{
					eType = ((ImprovementTypes)(GC.getDefineINT("LAND_IMPROVEMENT")));
				}
			}
		}
	}

	// worked state
	if (isActiveVisible(false) && isBeingWorked())
	{
		bWorked = true;
	}
}

void CvPlot::getVisibleBonusState(BonusTypes& eType, bool& bImproved, bool& bWorked)
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
		ImprovementTypes eRevealedImprovement = getRevealedImprovementType(GC.getGame().getActiveTeam(), true);

		if ((eRevealedImprovement != NO_IMPROVEMENT) && GC.getImprovementInfo(eRevealedImprovement).isImprovementBonusTrade(eType))
		{
			bImproved = true;
			bWorked = isBeingWorked();
		}
	}
}

bool CvPlot::shouldUsePlotBuilder()
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

/************************************************************************************************/
/* Afforess	Mountains Start		 09/18/09                                           		 */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	if (isWater() && !(isImpassable(getTeam())&&!isRoute()))
/************************************************************************************************/
/* Afforess	Mountains End       END        		                                             */
/************************************************************************************************/

	{
		int iBuildingYield = 0;
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
		{
			CvBuildingInfo& building = GC.getBuildingInfo((BuildingTypes)iBuilding);
			iBuildingYield = std::max(building.getSeaPlotYieldChange(eYield) + building.getGlobalSeaPlotYieldChange(eYield), iBuildingYield);
		}
		iMaxYield += iBuildingYield;
	}

	if (isRiver())
	{
		int iBuildingYield = 0;
		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); iBuilding++)
		{
			CvBuildingInfo& building = GC.getBuildingInfo((BuildingTypes)iBuilding);
			iBuildingYield = std::max(building.getRiverPlotYieldChange(eYield), iBuildingYield);
		}
		iMaxYield += iBuildingYield;
	}

	int iExtraYieldThreshold = 0;
	for (int iTrait = 0; iTrait < GC.getNumTraitInfos(); iTrait++)
	{
		CvTraitInfo& trait = GC.getTraitInfo((TraitTypes)iTrait);
		iExtraYieldThreshold  = std::max(trait.getExtraYieldThreshold(eYield), iExtraYieldThreshold);
	}
	if (iExtraYieldThreshold > 0 && iMaxYield > iExtraYieldThreshold)
	{
		iMaxYield += GC.getDefineINT("EXTRA_YIELD");
	}

	return iMaxYield;
}

int CvPlot::getYieldWithBuild(BuildTypes eBuild, YieldTypes eYield, bool bWithUpgrade) const
{
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

	ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (bWithUpgrade)
		{
			//in the case that improvements upgrade, use 2 upgrade levels higher for the
			//yield calculations.
			ImprovementTypes eUpgradeImprovement = (ImprovementTypes)GC.getImprovementInfo(eImprovement).getImprovementUpgrade();
			if (eUpgradeImprovement != NO_IMPROVEMENT)
			{
				//unless it's commerce on a low food tile, in which case only use 1 level higher
				if ((eYield != YIELD_COMMERCE) || (getYield(YIELD_FOOD) >= GC.getFOOD_CONSUMPTION_PER_POPULATION()))
				{
					ImprovementTypes eUpgradeImprovement2 = (ImprovementTypes)GC.getImprovementInfo(eUpgradeImprovement).getImprovementUpgrade();
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
	FAssert(::isPlotEventTrigger(eTrigger));

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

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

		CLLNode<IDInfo>* pUnitNode = headUnitNode();

		while (NULL != pUnitNode)
		{
			CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->getOwner() == ePlayer)
			{
				if (-1 != pLoopUnit->getTriggerValue(eTrigger, this, false))
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
		if (NO_IMPROVEMENT != kEvent.getImprovement()
		&& !canHaveImprovement((ImprovementTypes)kEvent.getImprovement(), getTeam(), false, false))
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
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

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
			setImprovementType((ImprovementTypes)kEvent.getImprovement());
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
		int iChange = kEvent.getPlotExtraYield(i);
		if (0 != iChange)
		{
			GC.getGame().setPlotExtraYield(m_iX, m_iY, (YieldTypes)i, iChange);
		}
	}
}


bool CvPlot::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible) const
{
	PROFILE_FUNC();

	CvUnitInfo& kUnit = GC.getUnitInfo(eUnit);

	if (kUnit.isPrereqBonuses())
	{
		if (kUnit.getDomainType() == DOMAIN_SEA)
		{
			bool bValid = false;
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
			{
				CvPlot* pLoopPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

				if (pLoopPlot != NULL && pLoopPlot->isWater() && pLoopPlot->area()->getNumTotalBonuses() > 0)
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
		else if (area()->getNumTotalBonuses() > 0)
		{
			return false;
		}
	}

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

	const int iCount = kUnit.getNumMapCategoryTypes();
	bool bFound = (iCount < 1);
	for (int iI = 0; iI < iCount; iI++)
	{
		if (isMapCategoryType((MapCategoryTypes)kUnit.getMapCategoryType(iI)))
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		return false;
	}

	if (!bTestVisible)
	{
		CvCity* pCity = getPlotCity();

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
		for (int iI = 0; iI < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); ++iI)
		{
			if (kUnit.getPrereqOrBonuses(iI) != NO_BONUS)
			{
				bValid = false;

				if (NULL == pCity)
				{
					if (isPlotGroupConnectedBonus(getOwner(), (BonusTypes)kUnit.getPrereqOrBonuses(iI)))
					{
						bValid = true;
						break;
					}
				}
				else if (pCity->hasBonus((BonusTypes)kUnit.getPrereqOrBonuses(iI)))
				{
					bValid = true;
					break;
				}
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
	int iTotalCulture = 0;

	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive())
		{
			CvTeam& kLoopTeam = GET_TEAM(kLoopPlayer.getTeam());
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
	int iCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (DOMAIN_AIR == pLoopUnit->getDomainType() && !pLoopUnit->isCargo() && pLoopUnit->getTeam() == eTeam)
		{
			iCount += GC.getUnitInfo(pLoopUnit->getUnitType()).getAirUnitCap();
		}
	}

	return iCount;
}

int CvPlot::countNumAirUnitCargoVolume(TeamTypes eTeam) const
{
	int iCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

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

	CvCity* pCity = getPlotCity();
	if (NULL != pCity)
	{
		if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
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
		if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
		{
			iMaxUnits = GC.getDefineINT("CITY_AIR_UNIT_CAPACITY") * 50;
		}
		else
		{
			iMaxUnits = GC.getDefineINT("CITY_AIR_UNIT_CAPACITY");
		}
	}

	if (GC.getGame().isOption(GAMEOPTION_SIZE_MATTERS))
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
	int iCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

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
	CvCity* pCity = getPlotCity();

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
	float fWeight = 0.0f;

	if (isLake() || isPeak2(true) || (getFeatureType() != NO_FEATURE && GC.getFeatureInfo(getFeatureType()).isAddsFreshWater()))
	{
		fWeight = 1.0f;
	}
	else if (isHills())
	{
		fWeight = 0.67f;
	}

	return fWeight;
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
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
/*                                                                                              */
/* Lead From Behind                                                                             */
/************************************************************************************************/
// From Lead From Behind by UncutDragon
void CvPlot::changeBattleCountdown(int iValue)
{
	if (iValue != 0)
	{
		m_iBattleCountdown += iValue;
	}
}

void CvPlot::setBattleCountdown(int iValue)
{
	m_iBattleCountdown = iValue;
}

int CvPlot::getBattleCountdown() const
{
	return (GC.isDCM_BATTLE_EFFECTS() == true ? m_iBattleCountdown : 0);
}

bool CvPlot::isBattle() const
{
	return getBattleCountdown() > 0;
}

bool CvPlot::canHaveBattleEffect(const CvUnit* pAttacker, const CvUnit* pDefender) const
{
	if (pAttacker != NULL)
	{
		if (pAttacker->isAnimal())
		{
			return false;
		}
		if (pAttacker->getUnitInfo().getDomainType() == DOMAIN_SEA)
		{
			return false;
		}
	}
	if (pDefender != NULL)
	{
		if (pDefender->isAnimal())
		{
			return false;
		}
		if (pDefender->getUnitInfo().getDomainType() == DOMAIN_SEA)
		{
			return false;
		}
	}

	if (isWater())
	{
		return false;
	}

	return true;

}

EffectTypes CvPlot::getBattleEffect()
{
	if (m_eBattleEffect == NO_EFFECT)
	{
		setBattleEffect();
	}

	return m_eBattleEffect;
}

void CvPlot::setBattleEffect()
{
	const int effectsCount = GC.getNumEffectInfos();
	int iRandOffset = GC.getASyncRand().get(effectsCount);
	for (int iI = 0; iI < effectsCount; iI++)
	{
		//Randomly loop over the effect types, so that the chosen effect is different each time
		EffectTypes eLoopEffect = (EffectTypes)((iI + iRandOffset) % effectsCount);
		if (GC.getEffectInfo(eLoopEffect).isBattleEffect())
		{
			m_eBattleEffect = eLoopEffect;
			return;
		}
	}
}
/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

// UncutDragon
bool CvPlot::hasDefender(bool bCheckCanAttack, PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove, bool bTestCanFight) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (pLoopUnit != NULL)
		{
			if (!pLoopUnit->isDead())
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
											if (!bCheckCanAttack || (pAttacker == NULL) || (pAttacker->canAttack(*pLoopUnit)))
											{
												if (!bTestCanFight || pLoopUnit->canFight())
												{
													// found a valid defender
													return true;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// there are no defenders
	return false;
}
bool CvPlot::hasStealthDefender(const CvUnit* pAttacker) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (pAttacker == NULL || !GC.getGame().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return false;
	}
	PlayerTypes eAttackingPlayer = pAttacker->getOwner();
	pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		if (pLoopUnit != NULL)
		{
			if (!pLoopUnit->isDead() && pLoopUnit->hasStealthDefense() && pLoopUnit->isInvisible(pAttacker->getTeam(), false, false) && !pLoopUnit->isCargo())
			{
				if (pLoopUnit->isEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), this, pLoopUnit)))
				{
					if (NULL == pAttacker || !pLoopUnit->canUnitCoexistWithArrivingUnit(*pAttacker))
					{
						if (pLoopUnit->getImmobileTimer() < 1)
						{
							if (pAttacker->getDomainType() != DOMAIN_AIR)
							{
								if (pLoopUnit->canAttack(*pAttacker))
								{
									// found a valid defender
									return true;
								}
							}
						}
					}
				}
			}
		}
	}

	// there are no stealth defenders
	return false;
}
void CvPlot::revealBestStealthDefender(const CvUnit* pAttacker)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iValue = 0;
	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	if (pAttacker == NULL || !GC.getGame().isOption(GAMEOPTION_WITHOUT_WARNING))
	{
		return;
	}
	PlayerTypes eAttackingPlayer = pAttacker->getOwner();
	pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		iValue = 0;
		if (!pLoopUnit->isDead() && pLoopUnit->hasStealthDefense() && pLoopUnit->isInvisible(pAttacker->getTeam(), false, false) && !pLoopUnit->isCargo())
		{
			if (pLoopUnit->isEnemy(GET_PLAYER(eAttackingPlayer).getTeam(), this) || (NULL != pAttacker && pAttacker->isEnemy(GET_PLAYER(pLoopUnit->getOwner()).getTeam(), this, pLoopUnit)))
			{
				if (NULL == pAttacker || !pLoopUnit->canUnitCoexistWithArrivingUnit(*pAttacker))
				{
					if (pLoopUnit->getImmobileTimer() < 1)
					{
						if (pAttacker->getDomainType() != DOMAIN_AIR)
						{
							if (pLoopUnit->canAttack(*pAttacker))
							{
								iValue = pLoopUnit->AI_attackOdds(this, pAttacker);
								if (iValue >= iBestValue)
								{
									iBestValue = iValue;
									pBestUnit = pLoopUnit;
								}
							}
						}
					}
				}
			}
		}
	}
	if (pBestUnit != NULL)
	{
		pBestUnit->reveal();
		updateCenterUnit();

		CvWString szBuffer;
		PlayerTypes eVisualDefender = pBestUnit->getVisualOwner(pAttacker->getTeam());
		MEMORY_TRACK_EXEMPT();

		szBuffer = gDLL->getText("TXT_KEY_MISC_STEALTH_DEFENSE_OWNER", pBestUnit->getNameKey(), GET_PLAYER(eAttackingPlayer).getNameKey(), pAttacker->getNameKey());
		AddDLLMessage(pBestUnit->getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_MINOR_EVENT, pBestUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX(), getY(), true, true);

		szBuffer = gDLL->getText("TXT_KEY_MISC_STEALTH_DEFENSE", GET_PLAYER(eVisualDefender).getNameKey(), pBestUnit->getNameKey(), pAttacker->getNameKey());
		AddDLLMessage(eAttackingPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_EXPOSED", MESSAGE_TYPE_MINOR_EVENT, pBestUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), getX(), getY(), true, true);
	}
}

void CvPlot::doPreAttackTraps(CvUnit* pAttacker)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvUnit* pTrap = NULL;

	if (pAttacker == NULL)
	{
		return;
	}
	pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		if (!pLoopUnit->isDead() && pLoopUnit->isArmedTrap() && pLoopUnit->isEnemy(pAttacker->getTeam(), this, pAttacker) && pLoopUnit->isTriggerBeforeAttack())
		{
			if (NULL == pAttacker || !pLoopUnit->canUnitCoexistWithArrivingUnit(*pAttacker))
			{
				pLoopUnit->doTrap(pAttacker);
			}
		}
	}
}
// /UncutDragon

/************************************************************************************************/
/* Afforess	                  Start		 01/20/10                                               */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/

PlayerTypes CvPlot::getClaimingOwner() const
{
	return (PlayerTypes) m_eClaimingOwner;
}


void CvPlot::setClaimingOwner(PlayerTypes eNewValue)
{
	m_eClaimingOwner = eNewValue;
}

#ifdef OLD_VERSION_PRE_SUPER_FORTS
void CvPlot::changeActsAsCity(PlayerTypes ePlayer, int iChange)
{
	CvPlot* pAdjacentPlot;
	int iI;

	for (iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (iChange > 0)
			{
				pAdjacentPlot->changeCulture(ePlayer, iChange, false);
			}
			pAdjacentPlot->changeCultureRangeCities(ePlayer, 1, iChange, false, false);
		}
	}

	if (iChange > 0)
	{
		changeCulture(ePlayer, iChange, false);
	}
	changeCultureRangeCities(ePlayer, 1, iChange, false, false);
}
#endif

bool CvPlot::isActsAsCity() const
{
	return (getImprovementType() != NO_IMPROVEMENT) && GC.getImprovementInfo(getImprovementType()).isActsAsCity();
}

bool CvPlot::changeBuildProgress(BuildTypes eBuild, int iChange, PlayerTypes ePlayer)
{
	bool bResult = changeBuildProgress(eBuild, iChange, GET_PLAYER(ePlayer).getTeam());

	if (bResult)
	{
		if (GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo((ImprovementTypes)(GC.getBuildInfo(eBuild).getImprovement())).isActsAsCity())
			{
				setOwner(ePlayer, true, false);
			}
		}
	}
	return bResult;
}

int CvPlot::getOccupationCultureRangeCities(PlayerTypes eOwnerIndex) const
{
	FAssert(eOwnerIndex >= 0);
	FAssert(eOwnerIndex < MAX_PLAYERS);

	if (NULL == m_aiOccupationCultureRangeCities)
	{
		return 0;
	}

	return m_aiOccupationCultureRangeCities[eOwnerIndex];
}


bool CvPlot::isWithinOccupationRange(PlayerTypes eOwnerIndex) const
{
	return (getOccupationCultureRangeCities(eOwnerIndex) > 0);
}


void CvPlot::changeOccupationCultureRangeCities(PlayerTypes eOwnerIndex,int iChange)
{
	FAssert(eOwnerIndex >= 0);
	FAssert(eOwnerIndex < MAX_PLAYERS);

	if (0 != iChange)
	{
		if (NULL == m_aiOccupationCultureRangeCities)
		{
			m_aiOccupationCultureRangeCities = new char[MAX_PLAYERS];
			for (int iI = 0; iI < MAX_PLAYERS; ++iI)
			{
				m_aiOccupationCultureRangeCities[iI] = 0;
			}
		}

		m_aiOccupationCultureRangeCities[eOwnerIndex] += iChange;
	}
}

// Called at the end of turn. Sets the owner that claimed the plot the previous turn.
// NOTE: We can't make the plot neutral at the moment of claiming (setForceUnowned()), because the aggressor could use it imidiatelly (i.e. travel using roads).
void CvPlot::doTerritoryClaiming()
{
	if (m_eClaimingOwner != NO_PLAYER)
	{
		setOwner((PlayerTypes)m_eClaimingOwner, false, false);

		m_eClaimingOwner = NO_PLAYER;
	}
}

int CvPlot::getRevoltProtection()
{
	if (!isCity()) return 0;

	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iProtection = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
		iProtection += pLoopUnit->revoltProtectionTotal();
	}
	return iProtection;
}
int CvPlot::getAverageEnemyStrength(TeamTypes eTeam)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iStrength = 0;
	int iCount = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
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
int CvPlot::getAverageEnemyDamage(TeamTypes eTeam)
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int iDamage = 0;
	int iCount = 0;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);
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
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getImprovementType() != NO_IMPROVEMENT)
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
		}
	}
	return NULL;
}

//Returns the first found player that controls an adjacent fort to this plot and is at war with the attackingTeam
PlayerTypes CvPlot::controlsAdjacentZOCSource(TeamTypes eAttackingTeam) const
{
	if (getImprovementType() != NO_IMPROVEMENT)
	{
		if (GC.getImprovementInfo(getImprovementType()).isZOCSource())
		{
			CLLNode<IDInfo>* pUnitNode;
			CvUnit* pLoopUnit;

			pUnitNode = headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
				if (GET_TEAM(pLoopUnit->getTeam()).isAtWar(eAttackingTeam))
				{
					return pLoopUnit->getOwner();
				}
			}
		}
	}

	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));

		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				if (GC.getImprovementInfo(pAdjacentPlot->getImprovementType()).isZOCSource())
				{
					CLLNode<IDInfo>* pUnitNode;
					CvUnit* pLoopUnit;

					pUnitNode = pAdjacentPlot->headUnitNode();

					while (pUnitNode != NULL)
					{
						pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pAdjacentPlot->nextUnitNode(pUnitNode);
						if (GET_TEAM(pLoopUnit->getTeam()).isAtWar(eAttackingTeam))
						{
							return pLoopUnit->getOwner();
						}
					}
				}
			}
		}
	}

	return NO_PLAYER;
}

bool CvPlot::isInCityZoneOfControl(PlayerTypes ePlayer) const
{
	//Check adjacent tiles
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->isCity())
			{
				CvCity* pAdjacentCity = pAdjacentPlot->getPlotCity();
				if (pAdjacentCity->isZoneOfControl())
				{
					return GET_TEAM(GET_PLAYER(ePlayer).getTeam()).isAtWar(pAdjacentCity->getTeam());
				}
			}
		}
	}
	return false;
}


int CvPlot::getTotalTurnDamage(const CvSelectionGroup* pGroup) const
{
	CLLNode<IDInfo>* pUnitNode = pGroup->headUnitNode();
	int		iMaxDamage = 0;

	FAssertMsg(pUnitNode != NULL, "headUnitNode() expected to be non-NULL");

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = pGroup->nextUnitNode(pUnitNode);

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
		//Oasis or Flood Plain
		if (GC.getFeatureInfo(getFeatureType()).getYieldChange(YIELD_FOOD) > 0)
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
	int iDamage = 0;

	if (getFeatureType() != NO_FEATURE)
	{
		iDamage = GC.getFeatureInfo(getFeatureType()).getTurnDamage();
	}
	return iDamage;
}

int CvPlot::getTotalTurnDamage(const CvUnit* pUnit) const
{
	return getTerrainTurnDamage(pUnit) + getFeatureTurnDamage();
}

CvUnit* CvPlot::getWorstDefender(PlayerTypes eOwner, PlayerTypes eAttackingPlayer, const CvUnit* pAttacker, bool bTestAtWar, bool bTestPotentialEnemy, bool bTestCanMove) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	int iBestUnitRank = -1;

	pBestUnit = NULL;

	pUnitNode = headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

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
									if (pBestUnit == NULL || (pLoopUnit->canDefend() && !pLoopUnit->isBetterDefenderThan(pBestUnit, pAttacker, &iBestUnitRank)))
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
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
		{
			CLLNode<IDInfo>* pUnitNode;
			CvUnit* pLoopUnit;

			pUnitNode = pAdjacentPlot->headUnitNode();

			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pAdjacentPlot->nextUnitNode(pUnitNode);
				if (pLoopUnit->isZoneOfControl())
				{
					return GET_TEAM(pLoopUnit->getTeam()).isAtWar(GET_PLAYER(ePlayer).getTeam());
				}
			}
		}
	}
	return false;
}

bool CvPlot::isBorder(bool bIgnoreWater) const
{
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
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
	}

	return false;
}

int CvPlot::getNumVisibleAdjacentEnemyDefenders(const CvUnit* pUnit) const
{
	int iCount = 0;
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
		{
			iCount += pAdjacentPlot->plotCount(PUF_canDefendEnemy, pUnit->getOwner(), pUnit->isAlwaysHostile(this), NULL, NO_PLAYER, NO_TEAM, PUF_isVisible, pUnit->getOwner());
		}
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
	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		removeSign((PlayerTypes)iI);
	}
}

void CvPlot::ToggleInPlotGroupsZobristContributors()
{
	int	iI;

	for( iI= 0; iI < MAX_PLAYERS; iI++ )
	{
		CvPlotGroup* p_plotGroup = getPlotGroup((PlayerTypes)iI);

		if ( p_plotGroup != NULL )
		{
			//	Add the zobrist contribution of this plot to the hash
			p_plotGroup->m_zobristHashes.resourceNodesHash ^= getZobristContribution();
		}
	}
}

int CvPlot::getBorderPlotCount() const
{
	int iCount = 0;
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getOwner() != getOwner())
			{
				iCount++;
			}
		}
	}
	return iCount;
}

int CvPlot::getEnemyBorderPlotCount(PlayerTypes ePlayer) const
{
	int iCount = 0;
	for (int iI = 0; iI < NUM_DIRECTION_TYPES; ++iI)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX(), getY(), ((DirectionTypes)iI));
		if (pAdjacentPlot != NULL)
		{
			if (pAdjacentPlot->getOwner() != NO_PLAYER)
			{
				if (GET_PLAYER(pAdjacentPlot->getOwner()).AI_getAttitude(ePlayer) >= ATTITUDE_ANNOYED)
				{
					iCount++;
				}
			}
		}
	}
	return iCount;
}

/************************************************************************************************/
/* Afforess	                     END                                                            */
/************************************************************************************************/

CvProperties* CvPlot::getProperties()
{
	return &m_Properties;
}

const CvProperties* CvPlot::getPropertiesConst() const
{
	return &m_Properties;
}

bool CvPlot::HaveCachedPathValidityResult(void* entity, bool bIsAlternateResult, bool& cachedResult)
{
	if ( m_iCachePathEpoch == m_iGlobalCachePathEpoch )
	{
		if ( bIsAlternateResult )
		{
			if ( m_cachedPathAlternateValidityEntity == entity )
			{
				cachedResult = m_cachedPathAlternateValidity;
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if ( m_cachedPathValidityEntity == entity )
			{
				cachedResult = m_cachedPathValidity;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
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

void	CvPlot::setDeferredPlotGroupRecalculationMode(bool bDefer)
{
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

int CvPlot::getNumAfflictedUnits(PlayerTypes eOwner, PromotionLineTypes eAfflictionLine) const
{
	return plotCount(PUF_isAfflicted, eAfflictionLine, -1, NULL, eOwner);
}

void CvPlot::enableCenterUnitRecalc(bool bEnable)
{
	m_bInhibitCenterUnitCalculation = !bEnable;

	if ( bEnable )
	{
		hideGraphics(ECvPlotGraphics::UNIT);
		// updateCenterUnit();
	}
}

bool CvPlot::isPlotIgnoringImprovementUpgrade() const
{
	return m_bIgnoringImprovementUpgrade;
}

void CvPlot::setPlotIgnoringImprovementUpgrade(bool bNewValue)
{
	m_bIgnoringImprovementUpgrade = bNewValue;
}

void CvPlot::setImprovementUpgrade()
{
	PlayerTypes ePlayer = getOwner();
	TeamTypes eTeam = GET_PLAYER(ePlayer).getTeam();
	ImprovementTypes ePrimaryUpgrade = GET_TEAM(getTeam()).getImprovementUpgrade(getImprovementType());
	ImprovementTypes eUpgrade = NO_IMPROVEMENT;
	//canHaveImprovement can be dramatically improved for processing
	if (canHaveImprovementAsUpgrade(ePrimaryUpgrade, eTeam, false, false))
	{
		if (GC.getImprovementInfo(ePrimaryUpgrade).getHighestCost() < GET_PLAYER(getOwner()).getEffectiveGold())
		{
			eUpgrade = ePrimaryUpgrade;
		}
	}
	else if (GC.getImprovementInfo(getImprovementType()).getNumAlternativeImprovementUpgradeTypes() > 0)
	{
		for (int iI = 0; iI < GC.getImprovementInfo(getImprovementType()).getNumAlternativeImprovementUpgradeTypes(); ++iI)
		{
			ImprovementTypes ePotentialUpgrade = (ImprovementTypes)GC.getImprovementInfo(getImprovementType()).getAlternativeImprovementUpgradeType(iI);
			if (canHaveImprovementAsUpgrade(ePotentialUpgrade, eTeam, false, false))
			{
				if (GC.getImprovementInfo(ePotentialUpgrade).getHighestCost() < GET_PLAYER(getOwner()).getEffectiveGold())
				{
					eUpgrade = ePotentialUpgrade;
					break;
				}
			}
		}
	}

	if (eUpgrade == NO_IMPROVEMENT)
	{
		setPlotIgnoringImprovementUpgrade(true);
		return;
	}

	BuildTypes eBestBuild = NO_BUILD;
	int iBestProduction = 0;
	int iProduction = 0;
	CvCity* pCity = NULL;
	CvWString szBuffer;
	if (getFeatureType() != NO_FEATURE)
	{
		FAssertMsg(eTeam != NO_TEAM, "eTeam should be valid");

		for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			if ((ImprovementTypes)GC.getBuildInfo((BuildTypes)iI).getImprovement() == eUpgrade)
			{
				BuildTypes eBuild = ((BuildTypes)iI);
				if (GET_TEAM(eTeam).isHasTech((TechTypes)GC.getBuildInfo(eBuild).getTechPrereq()))
				{
					if (GC.getBuildInfo(eBuild).isFeatureRemove(getFeatureType()))
					{
						iProduction = getFeatureProduction(eBuild, eTeam, &pCity);
						if (iProduction >= iBestProduction && iProduction != -1)
						{
							iBestProduction = iProduction;
							eBestBuild = eBuild;
						}

					}
					else
					{
						iBestProduction = -1;
						eBestBuild = eBuild;
					}
				}
				else
				{
					return;
				}
			}
		}
		if (iBestProduction == -1)
		{
			if (GC.getImprovementInfo(eUpgrade).getHighestCost() < GET_PLAYER(getOwner()).getEffectiveGold())
			{
				setImprovementType(eUpgrade);
			}
			else
			{
				return;
			}
		}
		else
		{
			if (GET_PLAYER(getOwner()).isHuman())
			{
				GET_PLAYER(getOwner()).upgradePlotPopup(getImprovementType(), getX(), getY());
				//send popup if player is human
			}
			else if (GC.getImprovementInfo(eUpgrade).getHighestCost() < GET_PLAYER(getOwner()).getEffectiveGold())
			{
				if (pCity != NULL)
				{
					pCity->changeFeatureProduction(iBestProduction);
				}

				MEMORY_TRACK_EXEMPT();

				if ( isInViewport() && pCity != NULL)
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_CLEARING_FEATURE_BONUS", GC.getFeatureInfo(getFeatureType()).getTextKeyWide(), iBestProduction, pCity->getNameKey());
					AddDLLMessage(getOwner(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer,  ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), MESSAGE_TYPE_INFO, GC.getFeatureInfo(getFeatureType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), getViewportX(),getViewportY(), true, true);
				}

				// Python Event
				CvEventReporter::getInstance().plotFeatureRemoved(this, getFeatureType(), pCity);

				setFeatureType(NO_FEATURE);

				if ( pCity != NULL )
				{
					pCity->clearCultureDistanceCache();
				}

				setImprovementType(eUpgrade);
			}
			else
			{
				return;
			}
		}
	}
	else if (eUpgrade != NO_IMPROVEMENT)
	{
		if (GC.getImprovementInfo(eUpgrade).getHighestCost() < GET_PLAYER(getOwner()).getEffectiveGold())
		{
			setImprovementType(eUpgrade);
		}
		else
		{
			return;
		}
	}
}

int CvPlot::getInjuredUnitCombatsUnsupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain) const
{
	int iCount = 0;
	if (eDomain != NO_DOMAIN)
	{
		iCount = plotCount(PUF_isInjuredUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer);
	}
	else
	{
		iCount = plotCount(PUF_isInjuredUnitCombatType, eUnitCombat, NO_DOMAIN, NULL, ePlayer);
	}
	if (iCount > 0)
	{
		CLLNode<IDInfo>* pUnitNode;
		CvUnit* pLoopUnit;
		pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

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
	int iCount = 0;
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	if (eDomain != NO_DOMAIN)
	{
		iCount = plotCount(PUF_isHealUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer);
	}
	else
	{
		iCount = plotCount(PUF_isHealUnitCombatType, eUnitCombat, NO_DOMAIN, NULL, ePlayer);
	}
	int iHighestOffset = 2;
	int iOffset = 0;
	if (iCount > 0)
	{
		CLLNode<IDInfo>* pUnitNode;
		CvUnit* pLoopUnit;
		pUnitNode = headUnitNode();
		CvPlot* pPlot;
		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			pPlot = pLoopUnit->plot();

			if (pLoopUnit->getOwner() == ePlayer && pLoopUnit->getNumHealSupportTotal() > 0 && pLoopUnit->getHealUnitCombatTypeTotal(eUnitCombat) > 0)
			{
				iOffset = (pLoopUnit->getNumHealSupportTotal() +1);
				iCount -= iOffset;
				if (iOffset > iHighestOffset)
				{
					iHighestOffset = iOffset;
				}
			}
		}
		iCount -= (iHighestOffset * kPlayer.AI_plotTargetMissionAIs(pPlot, MISSIONAI_HEAL_SUPPORT));
	}
	return std::max(0, (iCount/3));
}

int CvPlot::getOverloadedUnitCombatsSupportedByHealer(PlayerTypes ePlayer, UnitCombatTypes eUnitCombat, DomainTypes eDomain) const
{
	int iCount = 0;
	CvPlayerAI& kPlayer = GET_PLAYER(ePlayer);
	if (eDomain != NO_DOMAIN)
	{
		iCount = plotCount(PUF_isHealUnitCombatType, eUnitCombat, eDomain, NULL, ePlayer);
	}
	else
	{
		iCount = plotCount(PUF_isHealUnitCombatType, eUnitCombat, NO_DOMAIN, NULL, ePlayer);
	}
	int iHighestOffset = 2;
	int iOffset = 0;
	int iOverload = 0;
	if (iCount > 0)
	{
		CvUnit* pLoopUnit;
		CLLNode<IDInfo> * pUnitNode = headUnitNode();
		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);

			if (pLoopUnit->getOwner() == ePlayer && pLoopUnit->getNumHealSupportTotal() > 0 && pLoopUnit->getHealUnitCombatTypeTotal(eUnitCombat) > 0)
			{
				iOffset = (pLoopUnit->getNumHealSupportTotal());
				iCount -= iOffset;
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
	bool bUpdate = false;

	for (CLLNode<IDInfo>* pUnitNode = headUnitNode(); pUnitNode != NULL; )
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

bool CvPlot::isMapCategoryType(MapCategoryTypes eIndex) const
{
	CvTerrainInfo& kTerrain = GC.getTerrainInfo(getTerrainType());
	int iNumTypes = kTerrain.getNumMapCategoryTypes();
	if (iNumTypes > 0)
	{
		if (!kTerrain.isMapCategoryType((int)eIndex))
		{
			return false;
		}
	}
	return true;
}

int CvPlot::countSeeInvisibleActive(PlayerTypes ePlayer, InvisibleTypes eVisible) const
{
	int iCount = 0;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		UnitAITypes eAIType = pLoopUnit->AI_getUnitAIType();
		MissionAITypes eMissionAI = pLoopUnit->getGroup()->AI_getMissionAIType();

		if (eAIType == UNITAI_SEE_INVISIBLE || eAIType == UNITAI_SEE_INVISIBLE_SEA)
		{
			if (pLoopUnit->getOwner() == ePlayer)
			{
				if (GC.getGame().isOption(GAMEOPTION_HIDE_AND_SEEK))
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

CvUnit* CvPlot::unit_iterator::resolve(const IDInfo& info) const
{
	return ::getUnit(info);
}

//CvPlot::const_unit_iterator::value_type* CvPlot::const_unit_iterator::resolve(const IDInfo& info) const
//{
//	return ::getUnit(info);
//}
