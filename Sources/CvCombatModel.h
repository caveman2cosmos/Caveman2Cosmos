#pragma once

#ifndef CV_COMBAT_MODEL_H
#define CV_COMBAT_MODEL_H

class CvUnit;

// ---------------------------------------------------------------------------
// CvCombatModel -- the single home for combat-probability math.
//
// Extracted from CvGameCoreUtils so combat odds live in one trackable,
// modifiable place. This is the foundation of the combat-odds unification:
//   - getCombatOdds          : attacker win probability, out of 1000.
//   - getCombatOddsSpecific  : probability of one specific outcome (n_A hits
//                              taken by attacker, n_D by defender); the ACO
//                              ("Advanced Combat Odds") distribution. win% is a
//                              sum over this.
//
// Subsequent phases fold the AI per-unit odds (CvUnitAI::AI_attackOddsAtPlot*)
// and the per-round resolution model (CvUnit::getDefenderCombatValues /
// resolveCombat) onto this same engine so prediction and resolution can never
// diverge again. See Sources/docs/combat-odds-baseline.md for the regression
// reference.
// ---------------------------------------------------------------------------

int getCombatOdds(const CvUnit* pAttacker, const CvUnit* pDefender);
float getCombatOddsSpecific(const CvUnit* pAttacker, const CvUnit* pDefender, int n_A, int n_D);

#endif // CV_COMBAT_MODEL_H
