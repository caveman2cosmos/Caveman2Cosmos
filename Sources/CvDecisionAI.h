#pragma once

#ifndef CV_DECISION_AI
#define CV_DECISION_AI

#include "CvEnums.h"

class CvPlayerAI;

// ---------------------------------------------------------------------------
// CvDecisionAI -- per-player flavour & strategic-decision logging, the player
// analogue of CvWorkerAI / CvHunterAI. One instance per CvPlayer (m_decisionAI),
// friend of CvPlayerAI and CvCityAI so the instrumentation at the decision sites
// (which live inside those classes' methods) can read private scoring state.
//
// Purpose is observability, not behaviour: it surfaces how a leader's flavour
// values bias the major AI decisions (tech, civics, religion, strategy posture,
// city production). onTurnBegin dumps the leader's FULL flavour vector once per
// turn as a baseline ([DAI/flavors]); every later [DAI/...] line is read against
// that baseline. All output goes to DecisionAI.log via the free logDecisionAI(),
// gated by the shared gPlayerLogLevel global (no separate BUG option).
//
// The class itself is deliberately thin -- it owns only the per-turn baseline
// dump and lifecycle state. The four decision areas call the free logDecisionAI()
// inline at their sites (matching how CvWorkerAI/CvHunterAI inline their log
// calls) rather than routing wide per-decision payloads through this object.
//
// ----------------------------------------------------------------------------
// Log taxonomy (grep DecisionAI.log); gated by gPlayerLogLevel. Parallel to [WAI/*].
//
//   [DAI/begin]        per-turn header: player, leader, turn, era
//   [DAI/flavors]      baseline dump: one line per flavour type + leader value
//   [DAI/tech/cand]    per-candidate tech: a flavour's contribution to its value
//   [DAI/tech/best]    final research pick (value, start tech)
//   [DAI/civic/cand]   per-candidate civic: a flavour's contribution to its value
//   [DAI/civic/best]   revolution decision + each changed civic option
//   [DAI/strategy]     flavour-driven strategy flags set this turn (+ key flavours)
//   [DAI/religion]     religion choice + religion flavour
//   [DAI/city/build]   city building pick: flavour contribution / total
//   [DAI/city/unit]    city unit pick (value only; AI_unitValue has no flavour loop)
//
//   Levels: 1 -> begin / flavors / tech-best / strategy / religion;
//           2 -> civic-best / city-build summary / city-unit;
//           3 -> tech-cand / civic-cand / city-build per-flavour detail.
// ----------------------------------------------------------------------------
class CvDecisionAI
{
public:
	explicit CvDecisionAI(PlayerTypes owner = NO_PLAYER);

	void setOwner(PlayerTypes owner) { m_owner = owner; }
	PlayerTypes getOwner() const { return m_owner; }

	// ---- lifecycle ----
	// Emits the per-turn [DAI/begin] header and [DAI/flavors] baseline dump for
	// AI players (skipped for humans / NPCs and when gPlayerLogLevel is 0).
	void onTurnBegin(int gameTurn);

private:
	PlayerTypes m_owner;
	int         m_lastTurn;
};

#endif // CV_DECISION_AI
