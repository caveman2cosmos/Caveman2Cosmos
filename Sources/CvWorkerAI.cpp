#include "CvGameCoreDLL.h"
#include "CvWorkerAI.h"

#include "FProfiler.h"

#include "CvBonusInfo.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvImprovementInfo.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvPlot.h"
#include "CvReachablePlotSet.h"
#include "CvSelectionGroup.h"
#include "CvTeamAI.h"
#include "CvUnitAI.h"
#include "Repos/BuildsRepo.h"

// ---------------------------------------------------------------------------
// WorkerScoringWeights -- legacy defaults preserved.
// ---------------------------------------------------------------------------
WorkerScoringWeights::WorkerScoringWeights()
	: timeScoreNumerator(10000)
	, improvementYieldInCityRadius(20)
	, natureYieldInCityRadius(10)
	, defenseDivisor(10)
	, zocSourceBonus(3)
	, aiObjectiveScale(100)
	, bonusValueMultiplier(1000)
	, atPlotBonus(3)
	, cityRadiusBonus(2)
	, noTradeableBonusMultiplier(2)
{
}

// ---------------------------------------------------------------------------
// CvWorkerAI lifecycle & cache + claim ledger.
// ---------------------------------------------------------------------------
CvWorkerAI::CvWorkerAI(PlayerTypes owner)
	: m_owner(owner)
	, m_lastTurn(-1)
	, m_weights()
{
}

void CvWorkerAI::onTurnBegin(int gameTurn)
{
	m_lastTurn = gameTurn;
	m_bonusEvalCache.clear();
	m_cityEvalCache.clear();
	m_outerRejected.clear();
	m_claims.clear();
}

CvWorkerAI::OuterRejectReason CvWorkerAI::lookupOuterReject(int plotIdx, UnitTypes unitType) const
{
	std::map<EvalKey, OuterRejectReason>::const_iterator it =
		m_outerRejected.find(EvalKey(plotIdx, (int)unitType));
	return (it == m_outerRejected.end()) ? REJECT_NONE : it->second;
}

void CvWorkerAI::markOuterRejected(int plotIdx, UnitTypes unitType, OuterRejectReason reason)
{
	m_outerRejected[EvalKey(plotIdx, (int)unitType)] = reason;
}

const char* CvWorkerAI::outerRejectReasonName(OuterRejectReason reason)
{
	switch (reason)
	{
	case REJECT_OWNERSHIP:        return "ownership";
	case REJECT_PLOT_INVALID:     return "plotInvalid";
	case REJECT_AREA_MISMATCH:    return "areaMismatch";
	case REJECT_NO_BONUS:         return "noBonus";
	case REJECT_NOT_CLOSE_ENOUGH: return "notCloseEnough";
	default:                      return "none";
	}
}

const CvWorkerAI::BonusEval* CvWorkerAI::lookup(int plotIdx, UnitTypes unitType,
                                                int gameTurn, BonusTypes currentBonus) const
{
	std::map<EvalKey, BonusEval>::const_iterator it =
		m_bonusEvalCache.find(EvalKey(plotIdx, (int)unitType));
	if (it == m_bonusEvalCache.end())            return NULL;
	if (it->second.turnComputed != gameTurn)     return NULL;
	if (it->second.bonus       != currentBonus)  return NULL;
	return &it->second;
}

void CvWorkerAI::record(int plotIdx, UnitTypes unitType, const BonusEval& eval)
{
	m_bonusEvalCache[EvalKey(plotIdx, (int)unitType)] = eval;
}

const CvWorkerAI::CityPlotEval* CvWorkerAI::lookupCity(int plotIdx, UnitTypes unitType,
                                                       int gameTurn, int cityId) const
{
	std::map<EvalKey, CityPlotEval>::const_iterator it =
		m_cityEvalCache.find(EvalKey(plotIdx, (int)unitType));
	if (it == m_cityEvalCache.end())          return NULL;
	if (it->second.turnComputed != gameTurn)  return NULL;
	if (it->second.cityId       != cityId)    return NULL;
	return &it->second;
}

void CvWorkerAI::recordCity(int plotIdx, UnitTypes unitType, const CityPlotEval& eval)
{
	m_cityEvalCache[EvalKey(plotIdx, (int)unitType)] = eval;
}

bool CvWorkerAI::tryClaim(int plotIdx, int unitId)
{
	std::map<int, int>::iterator it = m_claims.find(plotIdx);
	if (it == m_claims.end())
	{
		m_claims[plotIdx] = unitId;
		return true;
	}
	return it->second == unitId;
}

bool CvWorkerAI::isClaimedByOther(int plotIdx, int unitId) const
{
	std::map<int, int>::const_iterator it = m_claims.find(plotIdx);
	return it != m_claims.end() && it->second != unitId;
}

void CvWorkerAI::releaseClaim(int plotIdx, int unitId)
{
	std::map<int, int>::iterator it = m_claims.find(plotIdx);
	if (it != m_claims.end() && it->second == unitId)
	{
		m_claims.erase(it);
	}
}

void CvWorkerAI::releaseAllClaimsBy(int unitId)
{
	for (std::map<int, int>::iterator it = m_claims.begin(); it != m_claims.end(); )
	{
		if (it->second == unitId)
		{
			m_claims.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

// ---------------------------------------------------------------------------
// improveBonus inner-loop helper.
//
// Apply the per-build filters that survive after the sparse iteration source
// has already established the build-improvement-bonus relationship, then update
// the running best-candidate state if this build wins on (yield, then time).
//
// Returns true if this candidate was qualified (passed all filters), false
// otherwise. The qualified-count is incremented by the caller.
//
// `bCultureBranch` toggles between the two iteration sources:
//   - false: bonus-trading branch (came from getProvidedByImprovementTypes).
//            Condition B requires BOTH culture > 0 AND outsideBorders on non-owned plots.
//   - true:  culture-extending branch (came from BuildsRepo::cultureBuilds()).
//            Condition B only requires outsideBorders (culture > 0 is guaranteed by
//            the iteration source).
// ---------------------------------------------------------------------------
namespace {

struct CandidateContext
{
	CvUnitAI*       unit;
	CvPlot*         plot;
	FeatureTypes    feature;
	PlayerTypes     plotOwner;
	PlayerTypes     player;
	bool            bLeaveForests;
};

struct CandidateBest
{
	int        yieldScore;
	int        timeScore;
	BuildTypes build;
	int        qualified;
};

bool considerCandidate(const CandidateContext& ctx,
                       const CvImprovementInfo& kImpr, ImprovementTypes eImpr,
                       BuildTypes eBuild, int iYieldSum, bool bCultureBranch,
                       const WorkerScoringWeights& weights,
                       CandidateBest& best)
{
	const CvBuildInfo& kBuild = GC.getBuildInfo(eBuild);

	// Filter A: leave-forests option blocks builds that would remove a feature.
	if (ctx.bLeaveForests && ctx.feature != NO_FEATURE && kBuild.isFeatureRemove(ctx.feature))
	{
		return false;
	}

	// Filter B: ownership / outside-borders eligibility.
	if (ctx.plotOwner != ctx.player)
	{
		if (bCultureBranch)
		{
			// In culture branch, getCulture() > 0 is guaranteed by the iteration source.
			if (!kImpr.isOutsideBorders()) return false;
		}
		else
		{
			if (!(kImpr.getCulture() > 0 && kImpr.isOutsideBorders())) return false;
		}
	}

	// Filter D: per-unit can-build check.
	if (!ctx.unit->canBuild(ctx.plot, eBuild)) return false;

	++best.qualified;

	const int iTimeScore = weights.timeScoreNumerator / (kBuild.getTime() + 1);

	if (gPlayerLogLevel >= 3)
	{
		logBuildEvaluation(3, "  [WAI/build/cand] build=%s impr=%s%s yield=%d time=%d timeScore=%d",
			kBuild.getType(), kImpr.getType(),
			bCultureBranch ? " (culture)" : "",
			iYieldSum, kBuild.getTime(), iTimeScore);
	}

	// Primary: improvement yield change on this plot. Tiebreaker: time score (faster wins).
	if (iYieldSum > best.yieldScore
	|| (iYieldSum == best.yieldScore && iTimeScore > best.timeScore))
	{
		best.yieldScore = iYieldSum;
		best.timeScore  = iTimeScore;
		best.build      = eBuild;
		return true;
	}
	return true;
}

// Sum of improvement yield change across all yield types on this plot.
// Yield depends only on the improvement, not on the build that creates it, so
// this is hoisted out of the per-build inner loop in the bonus-trading branch.
int computeYieldSum(CvPlot* plot, ImprovementTypes eImpr, PlayerTypes ePlayer)
{
	int iYieldSum = 0;
	for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
	{
		iYieldSum += plot->calculateImprovementYieldChange(eImpr, (YieldTypes)iK, ePlayer, false);
	}
	return iYieldSum;
}

} // namespace

// ---------------------------------------------------------------------------
// CvWorkerAI::pushBuildMission -- shared mission-push tail for the two planners.
//
// Both improveBonus and improveCity decide their best plot + build + mission
// type by different rules, then run the identical sequence: AI_betterPlotBuild
// substitution, [<section>/mission] log, atPlot fast-path, moves guard, push
// the move + build pair, log success or pushFailed. Extracted here so the
// two callers don't drift.
//
// `section` is the log-tag prefix -- "WAI" for the bonus planner (emits
// [WAI/mission] / [WAI/end]) and "WAI/city" for the city planner (emits
// [WAI/city/mission] / [WAI/city/end]). The bonus planner keeps its own
// [WAI/end] result=noTarget and result=route exits inline because they
// happen *before* eMission is decided.
//
// Returns true if a mission was pushed onto the unit's group.
// ---------------------------------------------------------------------------
bool CvWorkerAI::pushBuildMission(CvUnitAI* unit, CvPlot* pBestPlot, BuildTypes eBestBuild,
                                  MissionTypes eMission, int iBestValue, const char* section)
{
	FAssertMsg(unit != NULL && pBestPlot != NULL, "pushBuildMission requires unit+plot");
	FAssertMsg(eBestBuild != NO_BUILD, "pushBuildMission requires a build");
	FAssertMsg(eBestBuild < GC.getNumBuildInfos(), "BestBuild is assigned a corrupt value");

	// AI_betterPlotBuild can substitute the chosen improvement build with a
	// route build (typically BUILD_TRAIL) so the worker lays the road first.
	// Log both the originally chosen and the substituted build so the analyst
	// sees both the planner's decision and the mission's actual payload.
	const BuildTypes eChosenBuild = eBestBuild;
	eBestBuild = unit->AI_betterPlotBuild(pBestPlot, eBestBuild);
	const bool bSubstituted = (eBestBuild != eChosenBuild);
	const bool bAtBestPlot  = unit->atPlot(pBestPlot);

	if (gPlayerLogLevel >= 2)
	{
		const char* missionName = bAtBestPlot ? "BUILD_HERE"
			: (eMission == MISSION_ROUTE_TO ? "ROUTE_TO" : "MOVE_TO");
		if (bSubstituted)
		{
			logBuildEvaluation(2, "[%s/mission] at=(%d,%d) chosen=%s actual=%s mission=%s value=%d",
				section,
				pBestPlot->getX(), pBestPlot->getY(),
				GC.getBuildInfo(eChosenBuild).getType(),
				GC.getBuildInfo(eBestBuild).getType(),
				missionName, iBestValue);
		}
		else
		{
			logBuildEvaluation(2, "[%s/mission] at=(%d,%d) build=%s mission=%s value=%d",
				section,
				pBestPlot->getX(), pBestPlot->getY(),
				GC.getBuildInfo(eBestBuild).getType(),
				missionName, iBestValue);
		}
	}

	// Fast path: unit already standing on the chosen plot. BTS rejects
	// pushMissionInternal(MISSION_MOVE_TO|ROUTE_TO, ...) when from == to;
	// without this guard those cases surface as result=pushFailed even
	// though the plan was sound.
	if (bAtBestPlot)
	{
		unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0,
			false, false, MISSIONAI_BUILD, pBestPlot);

		if (gPlayerLogLevel >= 1)
		{
			if (bSubstituted)
			{
				logBuildEvaluation(1, "[%s/end] unit=%d result=build at=(%d,%d) chosen=%s actual=%s value=%d (atPlot)",
					section, unit->getID(), pBestPlot->getX(), pBestPlot->getY(),
					GC.getBuildInfo(eChosenBuild).getType(),
					GC.getBuildInfo(eBestBuild).getType(), iBestValue);
			}
			else
			{
				logBuildEvaluation(1, "[%s/end] unit=%d result=build at=(%d,%d) build=%s value=%d (atPlot)",
					section, unit->getID(), pBestPlot->getX(), pBestPlot->getY(),
					GC.getBuildInfo(eBestBuild).getType(), iBestValue);
			}
		}
		return true;
	}

	// Moves guard: BTS validates the first step at push time and rejects
	// when no move is possible. Converts what would otherwise surface as a
	// noisy pushFailed into a clean noMoves exit.
	if (unit->getMoves() <= 0)
	{
		if (gPlayerLogLevel >= 1)
			logBuildEvaluation(1, "[%s/end] unit=%d result=noMoves at=(%d,%d) target=(%d,%d) mission=%s",
				section, unit->getID(), unit->getX(), unit->getY(),
				pBestPlot->getX(), pBestPlot->getY(),
				eMission == MISSION_ROUTE_TO ? "ROUTE_TO" : "MOVE_TO");
		return false;
	}

	if (unit->getGroup()->pushMissionInternal(eMission, pBestPlot->getX(), pBestPlot->getY(),
		unit->isHuman() ? 0 : MOVE_WITH_CAUTION, false, false, MISSIONAI_BUILD, pBestPlot))
	{
		unit->getGroup()->pushMission(MISSION_BUILD, eBestBuild, -1, 0,
			(unit->getGroup()->getLengthMissionQueue() > 0), false, MISSIONAI_BUILD, pBestPlot);

		if (gPlayerLogLevel >= 1)
		{
			if (bSubstituted)
			{
				logBuildEvaluation(1, "[%s/end] unit=%d result=build at=(%d,%d) chosen=%s actual=%s value=%d",
					section, unit->getID(), pBestPlot->getX(), pBestPlot->getY(),
					GC.getBuildInfo(eChosenBuild).getType(),
					GC.getBuildInfo(eBestBuild).getType(), iBestValue);
			}
			else
			{
				logBuildEvaluation(1, "[%s/end] unit=%d result=build at=(%d,%d) build=%s value=%d",
					section, unit->getID(), pBestPlot->getX(), pBestPlot->getY(),
					GC.getBuildInfo(eBestBuild).getType(), iBestValue);
			}
		}
		return true;
	}

	// pushMissionInternal returned false: typically busy with a prior mission,
	// or path now invalid (enemy revealed, etc.). Log enough context to
	// diagnose without re-running the game.
	if (gPlayerLogLevel >= 1)
		logBuildEvaluation(1, "[%s/end] unit=%d result=pushFailed from=(%d,%d) to=(%d,%d) mission=%s moves=%d isBusy=%d",
			section, unit->getID(),
			unit->getX(), unit->getY(),
			pBestPlot->getX(), pBestPlot->getY(),
			eMission == MISSION_ROUTE_TO ? "ROUTE_TO" : "MOVE_TO",
			unit->getMoves(),
			unit->getGroup()->isBusy() ? 1 : 0);
	return false;
}

// ---------------------------------------------------------------------------
// CvWorkerAI::improveBonus
//
// Unified worker bonus-improvement planner. Successor to the legacy CvUnitAI::
// AI_improveBonus and the deleted CvWorkerService::ImproveBonus experiment.
//
// Returns true if a mission was pushed onto the unit's group.
//
// Log taxonomy is documented in CvWorkerAI.h above the class declaration;
// each [WAI/*] tag below corresponds to a section explained there.
// ---------------------------------------------------------------------------
bool CvWorkerAI::improveBonus(CvUnitAI* unit, int allowedMovementTurns)
{
	PROFILE_FUNC();

	// --- One-time setup: pull invariants we'll reuse across all candidate plots ---
	int iMaxDistFromBorder = -1;
	if (unit->getGroup()->getNumUnits() > 1 && unit->getGroup()->canDefend())
	{
		iMaxDistFromBorder = GC.getAI_WORKER_MAX_DISTANCE_FROM_CITY_OUT_BORDERS();
	}
	const int iBasePathFlags = MOVE_SAFE_TERRITORY | MOVE_AVOID_ENEMY_UNITS
		| (unit->isHuman() ? MOVE_OUR_TERRITORY : MOVE_IGNORE_DANGER | MOVE_RECONSIDER_ON_LEAVING_OWNED);
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();
	const PlayerTypes ePlayer    = unit->getOwner();
	const CvPlayerAI& kOwner     = GET_PLAYER(ePlayer);
	const bool bLeaveForests     = kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS);
	const bool bSafeAutomation   = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const bool bCanRoute         = unit->canBuildRoute();
	const int  iGameTurn         = GC.getGame().getGameTurn();
	const UnitTypes eUnitType    = unit->getUnitType();
	const WorkerScoringWeights& W = m_weights;

	int iPathTurns = 0;
	int iBestValue = 0;
	bool bBestBuildIsRoute = false;
	BuildTypes eBestBuild = NO_BUILD;
	CvPlot* pBestPlot = NULL;

	const int iSearchRange = (allowedMovementTurns > 0)
		? unit->AI_searchRange(allowedMovementTurns)
		: -1;

	if (gPlayerLogLevel >= 1)
	{
		logBuildEvaluation(1, "[WAI/begin] owner=%d unit=%d at=(%d,%d) allowedTurns=%d searchRange=%d canRoute=%d",
			(int)ePlayer, unit->getID(), unit->getX(), unit->getY(),
			allowedMovementTurns, iSearchRange, bCanRoute ? 1 : 0);
	}

	// Populate plotSet upfront and iterate directly. The legacy code scanned
	// numPlots() and lazy-populated the set on first match, paying for the
	// hash lookup against every plot in the world (~40k on a big map).
	CvReachablePlotSet plotSet(unit->getGroup(), iBasePathFlags, iSearchRange, true,
	                           iMaxDistFromBorder == -1 ? -1 : iMaxDistFromBorder / 2 + 1);
	plotSet.Populate(MAX_INT);

	if (plotSet.begin() == plotSet.end() && gPlayerLogLevel >= 1)
	{
		logBuildEvaluation(1, "[WAI/plotset-empty] no reachable plots for unit=%d", unit->getID());
	}

	// =======================================================================
	// Outer loop: iterate every reachable plot, score it as an improve-bonus target.
	// =======================================================================
	for (CvReachablePlotSet::const_iterator itr = plotSet.begin(); itr != plotSet.end(); ++itr)
	{
		CvPlot* pLoopPlot = itr.plot();
		const int iPlotIdx = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());

		PROFILE("CvWorkerAI::improveBonus.ConsiderPlot");

		// --- Cached outer-rejection short-circuit ---
		// If this (plot, unitType) was already rejected at the outer filter earlier
		// this turn for a STABLE reason, skip the work and the spammy level-2 log
		// line. This is the optimization that makes the cascade pattern
		// (improveBonus(2) -> improveBonus(4) -> improveBonus(-1)) cheap when no
		// candidate is viable: only the first call does the work; the next two
		// only log at level 3.
		const OuterRejectReason eCachedReject = lookupOuterReject(iPlotIdx, eUnitType);
		if (eCachedReject != REJECT_NONE)
		{
			if (gPlayerLogLevel >= 3)
				logBuildEvaluation(3, "[WAI/plot/skip] at=(%d,%d) reason=%s (cached)",
					pLoopPlot->getX(), pLoopPlot->getY(),
					outerRejectReasonName(eCachedReject));
			continue;
		}

		const PlayerTypes ePlotOwner = pLoopPlot->getOwner();

		// --- [WAI/plot/skip] outer filters: ownership/visibility/area ---
		const bool bOwnerOK = (ePlotOwner == ePlayer)
			|| (ePlotOwner == NO_PLAYER && pLoopPlot->isRevealed(unit->getTeam(), false));
		if (!bOwnerOK)
		{
			markOuterRejected(iPlotIdx, eUnitType, REJECT_OWNERSHIP);
			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/plot/skip] at=(%d,%d) reason=ownership owner=%d",
					pLoopPlot->getX(), pLoopPlot->getY(), (int)ePlotOwner);
			continue;
		}
		if (!unit->AI_plotValid(pLoopPlot))
		{
			markOuterRejected(iPlotIdx, eUnitType, REJECT_PLOT_INVALID);
			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/plot/skip] at=(%d,%d) reason=plotInvalid",
					pLoopPlot->getX(), pLoopPlot->getY());
			continue;
		}
		const bool bSameArea = (pLoopPlot->area() == unit->area())
			|| (DOMAIN_SEA == unit->getDomainType() && pLoopPlot->isWater() && unit->plot()->isAdjacentToArea(pLoopPlot->area()));
		if (!bSameArea)
		{
			markOuterRejected(iPlotIdx, eUnitType, REJECT_AREA_MISMATCH);
			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/plot/skip] at=(%d,%d) reason=areaMismatch",
					pLoopPlot->getX(), pLoopPlot->getY());
			continue;
		}

		const BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(unit->getTeam());
		if (eNonObsoleteBonus == NO_BONUS)
		{
			markOuterRejected(iPlotIdx, eUnitType, REJECT_NO_BONUS);
			// Plots without a non-obsolete bonus aren't this planner's concern -- they're
			// the city-improvement planner's domain. Logging this at level 3 only since
			// most reachable plots fall here.
			if (gPlayerLogLevel >= 3)
				logBuildEvaluation(3, "[WAI/plot/skip] at=(%d,%d) reason=noBonus",
					pLoopPlot->getX(), pLoopPlot->getY());
			continue;
		}

		// --- [WAI/plot/close] Super Forts close-enough check (non-owned plots only) ---
		bool bCloseEnough = false;
		if (ePlotOwner == ePlayer)
		{
			bCloseEnough = true;
		}
		else if (!unit->isHuman() || !kOwner.hasBonus(eNonObsoleteBonus))
		{
			// Automated human workers won't reach outside borders for bonuses they already own.
			PROFILE("CvWorkerAI::improveBonus.CheckConnectivity");

			const CvCity* pNearestCity = GC.getMap().findCity(
				pLoopPlot->getX(), pLoopPlot->getY(), ePlayer, NO_TEAM, false);

			if (pNearestCity != NULL
			&& unit->generatePath(pLoopPlot, 0, true, &iPathTurns, iMaxDistFromBorder / 2)
			&& plotDistance(pLoopPlot->getX(), pLoopPlot->getY(),
			                pNearestCity->getX(), pNearestCity->getY())
			   * (kOwner.hasBonus(eNonObsoleteBonus) ? 2 : 1) <= iMaxDistFromBorder)
			{
				bCloseEnough = true;
			}

			if (gPlayerLogLevel >= 3)
				logBuildEvaluation(3, "[WAI/plot/close] at=(%d,%d) bonus=%s closeEnough=%d",
					pLoopPlot->getX(), pLoopPlot->getY(),
					GC.getBonusInfo(eNonObsoleteBonus).getType(), bCloseEnough ? 1 : 0);
		}

		if (!bCloseEnough)
		{
			markOuterRejected(iPlotIdx, eUnitType, REJECT_NOT_CLOSE_ENOUGH);
			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/plot/skip] at=(%d,%d) reason=notCloseEnough",
					pLoopPlot->getX(), pLoopPlot->getY());
			continue;
		}

		// --- [WAI/plot/access] accessibility gate ---
		// bIsConnected is only needed when no working city and we can't route.
		// Defer the isConnectedToCapital() walk until first use.
		const bool bHasWorkingCity = pLoopPlot->getWorkingCity() != NULL;
		bool bIsConnectedKnown = false;
		bool bIsConnected = false;

		bool bAccessible;
		if (bHasWorkingCity || bCanRoute)
		{
			bAccessible = true;
		}
		else
		{
			bIsConnected = pLoopPlot->isConnectedToCapital(ePlayer);
			bIsConnectedKnown = true;
			bAccessible = bIsConnected;
		}

		if (!bAccessible || pLoopPlot->isVisibleEnemyUnit(unit))
		{
			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/plot/skip] at=(%d,%d) reason=%s",
					pLoopPlot->getX(), pLoopPlot->getY(),
					!bAccessible ? "inaccessible" : "visibleEnemy");
			continue;
		}

		PROFILE("CvWorkerAI::improveBonus.CheckImprovement");

		// --- bDoImprove decision: is the plot's current improvement (if any) replaceable? ---
		const bool bCityRadius = pLoopPlot->isCityRadius();
		const ImprovementTypes eImprovement = pLoopPlot->getImprovementType();
		bool bDoImprove = false;

		if (eImprovement == NO_IMPROVEMENT)
		{
			bDoImprove = true;
		}
		else if (bCityRadius && (GC.getImprovementInfo(eImprovement).isActsAsCity()
		         || GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus)))
		{
			// Super Forts: re-evaluate whether a fort should remain on a city-radius plot
			// vs a bonus-trading improvement.
			bDoImprove = true;
		}

		if (!bDoImprove)
		{
			bDoImprove = (eImprovement == eRuins);
		}
		else if (bSafeAutomation && eImprovement != NO_IMPROVEMENT && eImprovement != eRuins)
		{
			// PLAYEROPTION_SAFE_AUTOMATION: don't replace existing non-ruin improvements.
			bDoImprove = false;
		}

		const FeatureTypes eFeature = pLoopPlot->getFeatureType();
		BuildTypes eBestTempBuild = NO_BUILD;

		// =================================================================
		// [WAI/build/...] Inner build pick: cache lookup or sparse iteration.
		// =================================================================
		if (bDoImprove)
		{
			PROFILE("CvWorkerAI::improveBonus.CheckBuild");

			CandidateBest best;
			best.yieldScore = INT_MIN;
			best.timeScore  = 0;
			best.build      = NO_BUILD;
			best.qualified  = 0;

			const BonusEval* pCached = lookup(iPlotIdx, eUnitType, iGameTurn, eNonObsoleteBonus);
			bool bFromCache = false;

			if (pCached != NULL)
			{
				eBestTempBuild  = pCached->bestBuild;
				best.yieldScore = (pCached->qualified > 0) ? pCached->score : INT_MIN;
				best.qualified  = pCached->qualified;
				best.build      = pCached->bestBuild;
				bFromCache      = true;

				if (gPlayerLogLevel >= 2)
					logBuildEvaluation(2, "[WAI/build/hit] at=(%d,%d) bonus=%s -> %s qualified=%d yield=%d",
						pLoopPlot->getX(), pLoopPlot->getY(),
						GC.getBonusInfo(eNonObsoleteBonus).getType(),
						eBestTempBuild == NO_BUILD ? "NO_BUILD" : GC.getBuildInfo(eBestTempBuild).getType(),
						best.qualified, best.qualified ? best.yieldScore : 0);
			}
			else
			{
				// Build the context once -- the helper reuses these across all candidates.
				CandidateContext ctx;
				ctx.unit          = unit;
				ctx.plot          = pLoopPlot;
				ctx.feature       = eFeature;
				ctx.plotOwner     = ePlotOwner;
				ctx.player        = ePlayer;
				ctx.bLeaveForests = bLeaveForests;

				// --- Branch 1: builds whose improvement trades this bonus. ---
				// yieldSum is computed once per improvement and shared across all
				// builds that produce that improvement.
				const CvBonusInfo& kBonus = GC.getBonusInfo(eNonObsoleteBonus);
				foreach_(const ImprovementTypes eImpr, kBonus.getProvidedByImprovementTypes())
				{
					const CvImprovementInfo& kImpr = GC.getImprovementInfo(eImpr);
					const int iYieldSum = computeYieldSum(pLoopPlot, eImpr, ePlayer);

					foreach_(const BuildTypes eBuild, kImpr.getBuildTypes())
					{
						considerCandidate(ctx, kImpr, eImpr, eBuild, iYieldSum,
							/*bCultureBranch=*/false, W, best);
					}
				}

				// --- Branch 2: culture-extending builds outside city radius. ---
				// Skip those that also trade this bonus -- they were scored in branch 1.
				if (!bCityRadius)
				{
					foreach_(const BuildTypes eBuild, BuildsRepo::get().cultureBuilds())
					{
						const ImprovementTypes eImpr =
							(ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();
						const CvImprovementInfo& kImpr = GC.getImprovementInfo(eImpr);

						if (kImpr.isImprovementBonusTrade(eNonObsoleteBonus)) continue;

						// Each culture build may have its own improvement, so yieldSum is
						// per-build here. Negligible cost since the culture list is small.
						const int iYieldSum = computeYieldSum(pLoopPlot, eImpr, ePlayer);

						considerCandidate(ctx, kImpr, eImpr, eBuild, iYieldSum,
							/*bCultureBranch=*/true, W, best);
					}
				}

				eBestTempBuild = best.build;

				// Record into the cache: store yieldSum as the score field.
				BonusEval eval;
				eval.turnComputed = iGameTurn;
				eval.bonus        = eNonObsoleteBonus;
				eval.bestBuild    = eBestTempBuild;
				eval.score        = (eBestTempBuild == NO_BUILD) ? 0 : best.yieldScore;
				eval.qualified    = best.qualified;
				record(iPlotIdx, eUnitType, eval);

				if (gPlayerLogLevel >= 2 && best.qualified > 0)
				{
					logBuildEvaluation(2, "[WAI/build/winner] at=(%d,%d) bonus=%s -> %s qualified=%d yield=%d time=%d",
						pLoopPlot->getX(), pLoopPlot->getY(),
						GC.getBonusInfo(eNonObsoleteBonus).getType(),
						eBestTempBuild == NO_BUILD ? "NO_BUILD" : GC.getBuildInfo(eBestTempBuild).getType(),
						best.qualified, best.yieldScore, best.timeScore);
				}
				else if (gPlayerLogLevel >= 3)
				{
					logBuildEvaluation(3, "[WAI/build/winner] at=(%d,%d) bonus=%s -> NO_BUILD",
						pLoopPlot->getX(), pLoopPlot->getY(),
						GC.getBonusInfo(eNonObsoleteBonus).getType());
				}
			}

			(void)bFromCache; // referenced only by the [WAI/build/hit] log above
		}
		if (eBestTempBuild == NO_BUILD)
		{
			bDoImprove = false;
		}
		// Super Forts: if not building an improvement and we don't own the plot, skip
		// route building too -- we shouldn't route into someone else's land.
		if (!bDoImprove && ePlotOwner != ePlayer)
		{
			continue;
		}

		if (eBestTempBuild == NO_BUILD && !(bCanRoute && !bHasWorkingCity))
		{
			continue;
		}

		// Route fallback uses bIsConnected; compute lazily if not already known.
		if (eBestTempBuild == NO_BUILD && bCanRoute)
		{
			if (!bIsConnectedKnown)
			{
				bIsConnected = pLoopPlot->isConnectedToCapital(ePlayer);
				bIsConnectedKnown = true;
			}
			if (bIsConnected)
			{
				// Already connected and no improvement to build here -- nothing to do.
				continue;
			}
		}

		PROFILE("CvWorkerAI::improveBonus.Evaluate");

		// --- [WAI/score] per-plot scoring after inner pick ---
		// MOVE_IGNORE_DANGER lets the worker find a target before getting spooked;
		// it can request an escort once a target is chosen.
		if (!unit->generatePath(pLoopPlot, iBasePathFlags, true, &iPathTurns))
		{
			if (gPlayerLogLevel >= 3)
				logBuildEvaluation(3, "[WAI/plot/skip] at=(%d,%d) reason=noPath",
					pLoopPlot->getX(), pLoopPlot->getY());
			continue;
		}

		int iValue = kOwner.AI_bonusVal(eNonObsoleteBonus);
		const int iValueInitial = iValue;
		int iValueYields = 0;
		int iValueDefense = 0;
		int iValueCounter = 0;

		if (bDoImprove)
		{
			// CSE the build info / improvement info we'll touch repeatedly here.
			const CvBuildInfo&       kBestBuild = GC.getBuildInfo(eBestTempBuild);
			const ImprovementTypes   eImprovementX = kBestBuild.getImprovement();
			FAssert(eImprovementX != NO_IMPROVEMENT);
			const CvImprovementInfo& kImprovementX = GC.getImprovementInfo(eImprovementX);

			// Yield contribution (city-radius only).
			if (bCityRadius)
			{
				for (int iK = 0; iK < NUM_YIELD_TYPES; iK++)
				{
					iValueYields += W.improvementYieldInCityRadius
						* pLoopPlot->calculateImprovementYieldChange(eImprovementX, (YieldTypes)iK, ePlayer, false);
					iValueYields += W.natureYieldInCityRadius
						* pLoopPlot->calculateNatureYield((YieldTypes)iK, unit->getTeam(),
							eFeature == NO_FEATURE ? true : kBestBuild.isFeatureRemove(eFeature));
				}
				iValue += iValueYields;
			}

			// Defense contribution (fort-likes only).
			if (kImprovementX.isActsAsCity())
			{
				const int iDefenseRaw = kImprovementX.getAirBombDefense()
				                      + kImprovementX.getDefenseModifier();
				iValueDefense = iDefenseRaw / W.defenseDivisor;
				if (kImprovementX.isZOCSource()) iValueDefense += W.zocSourceBonus;

				if (!bCityRadius)
				{
					iValue += iValueDefense;
					iValue += kImprovementX.getCulture();
				}
				else if (pLoopPlot->getWorkingCity() != NULL)
				{
					// Priority to replace with fort scales with city threat.
					int iThreat = iValueDefense * pLoopPlot->getWorkingCity()->AI_cityThreat() / 100;
					iValue += iThreat;
					iValueDefense = iThreat; // for logging
				}
			}

			// Penalty for replacing an existing improvement: subtract its current value.
			if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				iValueCounter = pLoopPlot->getImprovementCurrentValue();
				if (iValueCounter == 0)
				{
					// Lazy init for plots that predate the current-value field. This is
					// a side-effect mutation; consider moving to setImprovementType().
					pLoopPlot->setImprovementCurrentValue();
					iValueCounter = pLoopPlot->getImprovementCurrentValue();
				}
				iValue -= iValueCounter;
			}
			if (iValue <= 0)
			{
				bDoImprove = false;
			}
		}

		// Hoist these out of the bDoImprove block so we can log them even when
		// bDoImprove was set to false earlier (negative-total case).
		int iAiObjective = 0;
		bool bNoTradeable = false;
		if (bDoImprove)
		{
			iAiObjective = std::max(0, W.aiObjectiveScale * GC.getBonusInfo(eNonObsoleteBonus).getAIObjective());
			iValue += iAiObjective;

			bNoTradeable = (kOwner.getNumTradeableBonuses(eNonObsoleteBonus) == 0);
			if (bNoTradeable)
			{
				iValue *= W.noTradeableBonusMultiplier;
			}
		}

		// --- [WAI/dedup] iMaxWorkers vs AI_plotTargetMissionAIs ---
		// CvPlayerAI maintains m_missionTargetCache for MISSIONAI_BUILD lookups so
		// this is amortized O(1) on the player side after the first call per turn.
		int iMaxWorkers = 1;
		if (eBestTempBuild != NO_BUILD && GC.getBuildInfo(eBestTempBuild).getTime() > 0)
		{
			iMaxWorkers = unit->AI_calculatePlotWorkersNeeded(pLoopPlot, eBestTempBuild);
			if (unit->getPathMovementRemaining() == 0)
			{
				iMaxWorkers = std::min((iMaxWorkers + 1) / 2,
				                       1 + kOwner.AI_baseBonusVal(eNonObsoleteBonus) / 20);
			}
		}

		const int iOtherBuilders = kOwner.AI_plotTargetMissionAIs(
			pLoopPlot, MISSIONAI_BUILD, unit->getGroup());
		const bool bDedupOK = iOtherBuilders < iMaxWorkers;
		const bool bBuildtimeOK = !bDoImprove
			|| pLoopPlot->getBuildTurnsLeft(eBestTempBuild, 0, 0) > 2 * iPathTurns - 1;

		// The atPlot flag is cached here so the [WAI/score] line can advertise
		// whether the atPlot multiplier will fire downstream.
		const bool bAtPlot = unit->atPlot(pLoopPlot);

		if (gPlayerLogLevel >= 2)
		{
			// total = (base + yield + def - counter + aiObj) * (noTrade ? 2 : 1)
			// final value (compared against best) = total * bonusValueMultiplier
			//      * (atPlot ? atPlotBonus : 1) / (path + 1) * (cityRad ? cityRadiusBonus : 1)
			// The flags here let an analyst recompute the final value from this line alone.
			logBuildEvaluation(2, "[WAI/score] at=(%d,%d) bonus=%s base=%d yield=%d def=%d counter=%d aiObj=%d noTrade=%d total=%d path=%d maxW=%d others=%d cityRad=%d atPlot=%d ok=%d",
				pLoopPlot->getX(), pLoopPlot->getY(),
				GC.getBonusInfo(eNonObsoleteBonus).getType(),
				iValueInitial, iValueYields, iValueDefense, iValueCounter,
				iAiObjective, bNoTradeable ? 1 : 0,
				iValue, iPathTurns, iMaxWorkers, iOtherBuilders,
				bCityRadius ? 1 : 0, bAtPlot ? 1 : 0,
				bDedupOK && bBuildtimeOK ? 1 : 0);
		}

		if (!bDedupOK || !bBuildtimeOK)
		{
			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/dedup] at=(%d,%d) skip reason=%s others=%d max=%d",
					pLoopPlot->getX(), pLoopPlot->getY(),
					!bDedupOK ? "maxWorkers" : "buildTimeVsPath",
					iOtherBuilders, iMaxWorkers);
			continue;
		}

		// --- Apply the whole-plot multipliers and compare against best so far. ---
		if (bDoImprove)
		{
			iValue *= W.bonusValueMultiplier;
			if (bAtPlot) iValue *= W.atPlotBonus;
			iValue /= (iPathTurns + 1);
			if (bCityRadius) iValue *= W.cityRadiusBonus;

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestBuild = eBestTempBuild;
				pBestPlot = pLoopPlot;
				bBestBuildIsRoute = false;

				if (gPlayerLogLevel >= 1)
					logBuildEvaluation(1, "[WAI/best] at=(%d,%d) bonus=%s build=%s value=%d (improve)",
						pLoopPlot->getX(), pLoopPlot->getY(),
						GC.getBonusInfo(eNonObsoleteBonus).getType(),
						GC.getBuildInfo(eBestBuild).getType(), iValue);
			}
		}
		else if (bCanRoute && !bIsConnected)
		{
			// Route-fallback: existing improvement traded the bonus, but plot isn't connected.
			FAssert(bCanRoute && !bIsConnected);

			if (eImprovement != NO_IMPROVEMENT
			&& GC.getImprovementInfo(eImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
			{
				iValue *= W.bonusValueMultiplier;
				iValue /= (iPathTurns + 1);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestBuild = NO_BUILD;
					pBestPlot = pLoopPlot;
					bBestBuildIsRoute = true;

					if (gPlayerLogLevel >= 1)
						logBuildEvaluation(1, "[WAI/best] at=(%d,%d) bonus=%s build=ROUTE value=%d (connect)",
							pLoopPlot->getX(), pLoopPlot->getY(),
							GC.getBonusInfo(eNonObsoleteBonus).getType(), iValue);
				}
			}
		}
	}

	// =======================================================================
	// [WAI/mission] Post-loop: push the chosen mission, if any.
	// =======================================================================
	if (pBestPlot == NULL)
	{
		if (gPlayerLogLevel >= 1)
			logBuildEvaluation(1, "[WAI/end] unit=%d result=noTarget", unit->getID());
		return false;
	}

	if (eBestBuild != NO_BUILD)
	{
		FAssertMsg(!bBestBuildIsRoute, "BestBuild should not be a route");

		// Decide MOVE_TO vs ROUTE_TO based on connectivity and path-vs-step
		// distance. The shared pushBuildMission helper handles the atPlot
		// fast-path (which makes the mission type moot when from == to).
		MissionTypes eBestMission = MISSION_MOVE_TO;
		if (!unit->atPlot(pBestPlot))
		{
			if (pBestPlot->getWorkingCity() == NULL
			|| !pBestPlot->getWorkingCity()->isConnectedToCapital())
			{
				eBestMission = MISSION_ROUTE_TO;
			}
			else
			{
				int iPathTurnsLocal;
				if (unit->generatePath(pBestPlot, iBasePathFlags, false, &iPathTurnsLocal)
				&& iPathTurnsLocal >= stepDistance(unit->getX(), unit->getY(),
				                                   pBestPlot->getX(), pBestPlot->getY()))
				{
					eBestMission = MISSION_ROUTE_TO;
				}
			}
		}

		return pushBuildMission(unit, pBestPlot, eBestBuild, eBestMission, iBestValue, "WAI");
	}

	if (bBestBuildIsRoute)
	{
		if (gPlayerLogLevel >= 2)
			logBuildEvaluation(2, "[WAI/mission] at=(%d,%d) build=ROUTE mission=connectPlot value=%d",
				pBestPlot->getX(), pBestPlot->getY(), iBestValue);

		if (unit->AI_connectPlot(pBestPlot))
		{
			if (gPlayerLogLevel >= 1)
				logBuildEvaluation(1, "[WAI/end] unit=%d result=route at=(%d,%d) value=%d",
					unit->getID(), pBestPlot->getX(), pBestPlot->getY(), iBestValue);
			return true;
		}
	}
	else
	{
		FErrorMsg("CvWorkerAI::improveBonus: pBestPlot set with neither build nor route");
	}

	if (gPlayerLogLevel >= 1)
		logBuildEvaluation(1, "[WAI/end] unit=%d result=fallthrough", unit->getID());
	return false;
}

// ---------------------------------------------------------------------------
// CvWorkerAI::improveCity
//
// Successor to the legacy CvUnitAI::AI_bestCityBuild + AI_improveCity pair.
// Mirrors improveBonus's single-pass shape: one iteration over the city's
// working plots, yield-primary scoring (from the per-city precomputed
// AI_getBestBuildValue, which is the city-AI's analog of the BonusEval
// score), path-turn + dedup multipliers, mission push at the end.
//
// The legacy code ran the path generation + dedup work twice (pass 0 picked
// a naive best, pass 1 confirmed it with path turns and might fall through
// to scan again). This version computes the path-discounted value inline for
// every candidate, so the comparator always sees the same units it ranks on.
// ---------------------------------------------------------------------------
bool CvWorkerAI::improveCity(CvUnitAI* unit, CvCity* pCity)
{
	PROFILE_FUNC();

	if (pCity == NULL || unit == NULL) return false;

	// Never service a city we don't own. Callers reach here from the tile under
    // the worker's feet (plot()->getPlotCity()/getWorkingCity()), which can be a
    // foreign city when borders overlap. canBuild would reject every foreign plot
    // anyway, but bailing up front keeps a strayed worker from committing to (and
    // pathing toward) another player's city -- see AI_workerMove / IsAbroad.
    if (pCity->getOwner() != unit->getOwner()) return false;

	const PlayerTypes ePlayer    = unit->getOwner();
	const CvPlayerAI& kOwner     = GET_PLAYER(ePlayer);
	const bool bSafeAutomation   = kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION);
	const ImprovementTypes eRuins = GC.getIMPROVEMENT_CITY_RUINS();
	const int  iGameTurn         = GC.getGame().getGameTurn();
	const UnitTypes eUnitType    = unit->getUnitType();
	const int  iCityId           = pCity->getID();
	const bool bUnitInCity       = unit->plot()->isCity();

	int        iBestValue = 0;
	int        iBestPathTurns = 0;
	CvPlot*    pBestPlot = NULL;
	BuildTypes eBestBuild = NO_BUILD;

	if (gPlayerLogLevel >= 1)
	{
		logBuildEvaluation(1, "[WAI/city/begin] owner=%d unit=%d at=(%d,%d) city=%S(%d) plots=%d",
			(int)ePlayer, unit->getID(), unit->getX(), unit->getY(),
			pCity->getName().GetCString(), iCityId, pCity->getNumCityPlots());
	}

	for (int iI = 0; iI < pCity->getNumCityPlots(); iI++)
	{
		CvPlot* pLoopPlot = plotCity(pCity->getX(), pCity->getY(), iI);
		if (pLoopPlot == NULL) continue;

		// --- [WAI/city/plot/skip] outer filters mirror AI_bestCityBuild ---
		if (pLoopPlot->getWorkingCity() != pCity) continue;
		if (!unit->AI_plotValid(pLoopPlot))      continue;

		if (bSafeAutomation)
		{
			const ImprovementTypes eImpr = pLoopPlot->getImprovementType();
			if (eImpr != NO_IMPROVEMENT && eImpr != eRuins)
			{
				if (gPlayerLogLevel >= 3)
					logBuildEvaluation(3, "[WAI/city/plot/skip] at=(%d,%d) reason=safeAutomation",
						pLoopPlot->getX(), pLoopPlot->getY());
				continue;
			}
		}

		const int iPlotIdx = GC.getMap().plotNum(pLoopPlot->getX(), pLoopPlot->getY());

		// --- Inner pick: read from cache, else materialise from the per-city precomputed best ---
		BuildTypes eBuild  = NO_BUILD;
		int        iValue  = 0;
		bool       bCanBuild = false;
		bool       bFromCache = false;

		const CityPlotEval* pCached = lookupCity(iPlotIdx, eUnitType, iGameTurn, iCityId);
		if (pCached != NULL)
		{
			eBuild    = pCached->bestBuild;
			iValue    = pCached->baseValue;
			bCanBuild = pCached->canBuild;
			bFromCache = true;

			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/city/eval/hit] at=(%d,%d) build=%s value=%d canBuild=%d",
					pLoopPlot->getX(), pLoopPlot->getY(),
					eBuild == NO_BUILD ? "NO_BUILD" : GC.getBuildInfo(eBuild).getType(),
					iValue, bCanBuild ? 1 : 0);
		}
		else
		{
			iValue = pCity->AI_getBestBuildValue(iI);
			eBuild = pCity->AI_getBestBuild(iI);
			bCanBuild = (eBuild != NO_BUILD) && unit->canBuild(pLoopPlot, eBuild);

			CityPlotEval eval;
			eval.turnComputed = iGameTurn;
			eval.cityId       = iCityId;
			eval.bestBuild    = eBuild;
			eval.baseValue    = iValue;
			eval.canBuild     = bCanBuild;
			recordCity(iPlotIdx, eUnitType, eval);

			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/city/eval/new] at=(%d,%d) build=%s value=%d canBuild=%d",
					pLoopPlot->getX(), pLoopPlot->getY(),
					eBuild == NO_BUILD ? "NO_BUILD" : GC.getBuildInfo(eBuild).getType(),
					iValue, bCanBuild ? 1 : 0);
		}
		(void)bFromCache;

		if (iValue <= 0 || eBuild == NO_BUILD || !bCanBuild) continue;

		// --- Path / dedup gate ---
		int iPathTurns = 0;
		if (pLoopPlot->isVisibleEnemyUnit(unit)) continue;
		if (!unit->generatePath(pLoopPlot, 0, true, &iPathTurns))
		{
			if (gPlayerLogLevel >= 3)
				logBuildEvaluation(3, "[WAI/city/plot/skip] at=(%d,%d) reason=noPath",
					pLoopPlot->getX(), pLoopPlot->getY());
			continue;
		}

		int iMaxWorkers = 1;
		if (unit->getPathMovementRemaining() == 0)
		{
			iPathTurns++;
		}
		else if (iPathTurns <= 1)
		{
			iMaxWorkers = unit->AI_calculatePlotWorkersNeeded(pLoopPlot, eBuild);
		}
		// Preserves the legacy "I'm already in a city and one step away" stack bonus.
		if (bUnitInCity && iPathTurns == 1 && unit->getPathMovementRemaining() > 0)
		{
			iMaxWorkers += 10;
		}

		const int iOtherBuilders = kOwner.AI_plotTargetMissionAIs(
			pLoopPlot, MISSIONAI_BUILD, unit->getGroup());
		const bool bDedupOK = iOtherBuilders < iMaxWorkers;

		const bool bAtPlot = unit->atPlot(pLoopPlot);

		// Path-turn discount + atPlot bonus, same shape as improveBonus.
		int iScored = iValue;
		if (bAtPlot)
		{
			iScored = iScored * 3 / 2;
		}
		iScored /= (1 + iPathTurns);

		if (gPlayerLogLevel >= 2)
			logBuildEvaluation(2, "[WAI/city/score] at=(%d,%d) build=%s base=%d path=%d atPlot=%d maxW=%d others=%d scored=%d ok=%d",
				pLoopPlot->getX(), pLoopPlot->getY(),
				GC.getBuildInfo(eBuild).getType(),
				iValue, iPathTurns, bAtPlot ? 1 : 0, iMaxWorkers, iOtherBuilders,
				iScored, bDedupOK ? 1 : 0);

		if (!bDedupOK)
		{
			if (gPlayerLogLevel >= 2)
				logBuildEvaluation(2, "[WAI/city/dedup] at=(%d,%d) skip others=%d max=%d",
					pLoopPlot->getX(), pLoopPlot->getY(), iOtherBuilders, iMaxWorkers);
			continue;
		}

		if (iScored > iBestValue)
		{
			iBestValue     = iScored;
			iBestPathTurns = iPathTurns;
			pBestPlot      = pLoopPlot;
			eBestBuild     = eBuild;

			if (gPlayerLogLevel >= 1)
				logBuildEvaluation(1, "[WAI/city/best] at=(%d,%d) build=%s value=%d",
					pLoopPlot->getX(), pLoopPlot->getY(),
					GC.getBuildInfo(eBestBuild).getType(), iScored);
		}
	}

	if (pBestPlot == NULL || eBestBuild == NO_BUILD)
	{
		if (gPlayerLogLevel >= 1)
			logBuildEvaluation(1, "[WAI/city/end] unit=%d result=noTarget city=%S",
				unit->getID(), pCity->getName().GetCString());
		return false;
	}

	// --- Mission decision ---
	// Mirrors the legacy AI_improveCity branching: if the unit isn't already on
	// the working-city plot (or the chosen build IS a route build), the worker
	// needs to lay road as it goes; otherwise pick MOVE_TO unless the plot has
	// a movement-cost terrain that warrants ROUTE_TO anyway.
	MissionTypes eMission;
	if (unit->plot()->getWorkingCity() != pCity
	|| GC.getBuildInfo(eBestBuild).getRoute() != NO_ROUTE)
	{
		eMission = MISSION_ROUTE_TO;
	}
	else
	{
		eMission = MISSION_MOVE_TO;
		int iPathTurns;
		if (unit->generatePath(pBestPlot, MOVE_WITH_CAUTION, false, &iPathTurns)
		&& iPathTurns == 1 && unit->getPathMovementRemaining() == 0)
		{
			if (pBestPlot->getRouteType() != NO_ROUTE)
			{
				eMission = MISSION_ROUTE_TO;
			}
		}
		else if (unit->plot()->getRouteType() == NO_ROUTE)
		{
			int iPlotMoveCost = GC.getTerrainInfo(unit->plot()->getTerrainType()).getMovementCost();
			if (unit->plot()->getFeatureType() != NO_FEATURE)
			{
				iPlotMoveCost += GC.getFeatureInfo(unit->plot()->getFeatureType()).getMovementCost();
			}
			if (unit->plot()->isHills())
			{
				iPlotMoveCost += GC.getHILLS_EXTRA_MOVEMENT();
			}
			if (unit->plot()->isAsPeak())
			{
				if (!GET_TEAM(unit->getTeam()).isMoveFastPeaks())
				{
					iPlotMoveCost += GC.getPEAK_EXTRA_MOVEMENT();
				}
				iPlotMoveCost += 3;
			}
			if (iPlotMoveCost > 1)
			{
				eMission = MISSION_ROUTE_TO;
			}
		}
	}

	(void)iBestPathTurns; // currently informational only; shared helper logs path-independent fields
	return pushBuildMission(unit, pBestPlot, eBestBuild, eMission, iBestValue, "WAI/city");
}

// ---------------------------------------------------------------------------
// Stateless worker helpers (consolidated from the former CvWorkerService).
// ---------------------------------------------------------------------------
bool CvWorkerAI::shouldImproveCity(CvCity* pCity)
{
	PROFILE_EXTRA_FUNC();
	if (pCity == NULL) return false;

	foreach_(const CvPlot * pLoopPlot, pCity->plots())
	{
		const int iPlotIndex = pCity->getCityPlotIndex(pLoopPlot);
		if (pLoopPlot
		&& pLoopPlot->getWorkingCity() == pCity
		&& pLoopPlot->getImprovementType() == NO_IMPROVEMENT
		&& pCity->AI_getBestBuildValue(iPlotIndex) > 0)
		{
			const BuildTypes eBuild = pCity->AI_getBestBuild(iPlotIndex);

			if (eBuild != NO_BUILD && GC.getBuildInfo(eBuild).getImprovement() != NO_IMPROVEMENT)
			{
				return true;
			}
		}
	}
	return false;
}

BuildTypes CvWorkerAI::getFastestBuildForImprovementType(
	const CvPlayer& player, ImprovementTypes eImprovement, const CvPlot* pPlot,
	const CvUnitAI* unit, bool includeCurrentImprovement)
{
	PROFILE_EXTRA_FUNC();
	int        iFastestTime = 10000;
	BuildTypes eFastestBuild = NO_BUILD;
	const ImprovementTypes eCurrentImprovement = pPlot->getImprovementType();
	const CvImprovementInfo& kImprovement = GC.getImprovementInfo(eImprovement);
	const bool bCheckUnitBuild = (unit != NULL);

	foreach_(const BuildTypes eBuild, kImprovement.getBuildTypes())
	{
		if (player.canBuild(pPlot, eBuild, false, false)
		|| (includeCurrentImprovement && eImprovement == eCurrentImprovement))
		{
			if (!bCheckUnitBuild || unit->canBuild(pPlot, eBuild, false))
			{
				const int iBuildTime = GC.getBuildInfo(eBuild).getTime();
				if (iFastestTime > iBuildTime)
				{
					iFastestTime = iBuildTime;
					eFastestBuild = eBuild;
				}
			}
		}
	}
	return eFastestBuild;
}
