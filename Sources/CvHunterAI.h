#pragma once

#ifndef CV_HUNTER_AI
#define CV_HUNTER_AI

#include "CvEnums.h"
#include <map>
#include <utility>

class CvPlot;
class CvUnitAI;

// ---------------------------------------------------------------------------
// CvHunterAI -- per-player hunting AI, split out of
// CvUnitAI::AI_SearchAndDestroyMove the same way CvWorkerAI was split out of the
// worker planner. One instance per CvPlayer (m_hunterAI), friend of CvUnitAI.
//
// Two entry points, dispatched from CvUnitAI::AI_SearchAndDestroyMove:
//
//   hunterMove   -- UNITAI_HUNTER units, and great commanders escorting a hunter
//                   (bWithCommander=true, the only caller that passes it). The
//                   wildlife/recon hunter: escort merge & hunter-escort contract,
//                   subdued-animal drop-off, needed-hunter phase-out & scrapping,
//                   upgrade travel, animal-focused hunting. The bWithCommander
//                   guards inside disable the hunter-only bookkeeping for the
//                   commander case (matching the legacy AI_SearchAndDestroyMove(true)).
//
//   autoHuntMove -- any other combat unit a player toggled to AUTOMATE_HUNT
//                   (UNITAI_ATTACK, UNITAI_ATTACK_CITY, ...). Lean, aggressive
//                   enemy-seeking with none of the hunter-only bookkeeping and no
//                   animal-only target restriction.
//
// The low-level movement helpers (AI_huntRange, AI_safety, AI_heal, AI_goody,
// AI_moveToBorders, AI_patrol, AI_explore, AI_refreshExploreRange, ...) stay on
// CvUnitAI and are called through friendship -- they are shared with
// AI_borderPatrol and others, exactly as CvWorkerAI left AI_plotValid/generatePath
// on CvUnitAI.
//
// State is per-player and turn-scoped (onTurnBegin wipes it). The claim ledger is a
// backstop to MISSIONAI_HUNT for cross-hunter target dedup.
//
// ----------------------------------------------------------------------------
// Log taxonomy (grep HunterAI.log); gated by gUnitLogLevel. Parallel to [WAI/*].
//
//   [HAI/begin]        routine entry: unit, AI type, automate type, with-commander
//   [HAI/heal]         heal / safety branch taken
//   [HAI/escort]       escort merge / hunter-escort contract advertise
//   [HAI/target/skip]  candidate rejected + reason (claimed/filter/nopath/odds)
//   [HAI/target/best]  new best target during the AI_huntRange sweep
//   [HAI/engage]       attack committed: adjacent kill, or auto-hunt sea seaAreaAttack/blockade
//   [HAI/explore]      auto-hunt sea exploration: seaExplore (new heading) / seaExploreKeep
//                      (committed heading -- hysteresis) / exploreGeneric (AI_explore fallback)
//   [HAI/spread]       border / patrol / refreshExplore fallback chosen (with per-unit bias)
//   [HAI/spin]         turn-hang safety: unit re-decided 8x at the same plot -> end its turn
//   [HAI/scrap]        phase-out / scrap decision
//   [HAI/mission]      final mission push (emitted by AI_huntRange)
//   [HAI/end]          routine exit with success/failure
//
//   Levels: 1 -> begin/end/best/mission/engage/spin; 2 -> heal/escort/spread/scrap/explore;
//           3 -> target/skip (per-candidate detail).
// ----------------------------------------------------------------------------
class CvHunterAI
{
public:
	explicit CvHunterAI(PlayerTypes owner = NO_PLAYER);

	void setOwner(PlayerTypes owner) { m_owner = owner; }
	PlayerTypes getOwner() const { return m_owner; }

	// ---- lifecycle ----
	void onTurnBegin(int gameTurn);

	// ---- hunting routines (return true if a mission/decision was made) ----
	bool hunterMove(CvUnitAI* unit, bool bWithCommander);
	bool autoHuntMove(CvUnitAI* unit);

	// ---- target claim ledger (cross-hunter dedup backstop to MISSIONAI_HUNT) ----
	bool tryClaim(int plotIdx, int unitId);
	bool isClaimedByOther(int plotIdx, int unitId) const;
	void releaseAllClaimsBy(int unitId);

private:
	// Naval exploration for auto-hunt ships: open-ocean frontier + claim-based fleet spread.
	bool seaExplore(CvUnitAI* unit);

	// Turn-hang safety: ends a unit's turn (returns true) if it re-decides at the same plot
	// too many times without moving -- a hunter/explore routine pushing a mission that never
	// advances the unit would otherwise spin ~50x/turn (capped only by the engine backstop).
	bool detectSpin(CvUnitAI* unit);

	PlayerTypes m_owner;
	int         m_lastTurn;
	std::map<int, int> m_claims; // plotIdx -> unitId
	std::map<int, std::pair<int, int> > m_decisionPlot; // unitId -> (plotIdx, same-plot re-decide count this turn)
};

#endif // CV_HUNTER_AI
