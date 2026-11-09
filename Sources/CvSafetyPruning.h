#pragma once

#ifndef CV_SAFETY_PRUNING_H
#define CV_SAFETY_PRUNING_H

#include <algorithm>
#include <cstdlib>

// Pure helper functions for CvUnitAI::AI_safety and CvUnitAI::AI_safetyEval.
// Kept free of heavy engine headers so both CvGameCoreDLL and standalone C++
// tests (see Test/CvSafetyPruning/) can include and exercise the exact
// production logic: scoring bit-identical to the pre-pruning formula, and
// admissibility of every pruning bound (no plot the old code would have
// chosen is ever pruned).

namespace safety
{
	// Score weight awarded per defending unit already stationed at the candidate plot.
	static const int DEFENDING_UNIT_WEIGHT = 100;

	// Defensive bonus weights when running from animal danger.
	static const int ANIMAL_DANGER_OWNED_TERRITORY_BONUS = 75;
	static const int ANIMAL_DANGER_NON_HOSTILE_TERRITORY_BONUS = 50;

	// Plot evaluation bonus and random roll parameters.
	static const int CURRENT_PLOT_STAY_BONUS = 50;
	static const int RANDOM_SAFETY_ROLL_MAX = 50;
	// When drawing an integer in [0, RANDOM_SAFETY_ROLL_MAX - 1], the theoretical maximum value is 49.
	static const int THEORETICAL_MAX_RANDOM_ROLL = RANDOM_SAFETY_ROLL_MAX - 1;

	// Scale and offset constants used in the turn distance discount formula:
	//   discountedScore = (preDistanceScore * DISTANCE_DISCOUNT_SCALE) / (pathTurns + DISTANCE_DISCOUNT_OFFSET)
	// For pathTurns = 0, this evaluates to 5/4 = 1.25x; for pathTurns = 1, it evaluates to 5/5 = 1.0x.
	static const int DISTANCE_DISCOUNT_SCALE = 5;
	static const int DISTANCE_DISCOUNT_OFFSET = 4;

	// Terrain turn damage normalization constants.
	static const int FULL_HEALTH_PERCENT = 100;
	static const int TERRAIN_DAMAGE_DIVISOR = 200;

	// Penalty divisor applied when the candidate plot is marked in danger and beats bestValue.
	static const int IN_DANGER_SCORE_DIVISOR = 2;

	// Sea movement step multiplier (naval promotions or coastal travel can at most double effective speed).
	static const int MAX_SEA_SPEED_MULTIPLIER = 2;

	// Minimum turn counts for immobilized or out-of-moves units attempting to reach a non-local plot.
	static const int MIN_EXHAUSTED_MOVES_PATH_TURNS = 1;
	static const int MIN_IMMOBILE_PATH_TURNS = 1;

	// Mirror of DomainTypes in CvEnums.h (C2C ordering: SEA=0, AIR=1, LAND=2,
	// IMMOBILE=3 — deliberately NOT the vanilla BtS ordering). Duplicated here so
	// this header stays engine-free for the standalone tests in Test/CvSafetyPruning/.
	// Pinned at compile time by STATIC_ASSERT checks in CvUnitAI.cpp; if DomainTypes
	// is ever reshuffled or extended, those asserts fail the build.
	enum SafetyPruningDomain
	{
		SAFETY_DOMAIN_SEA = 0,
		SAFETY_DOMAIN_AIR = 1,
		SAFETY_DOMAIN_LAND = 2,
		SAFETY_DOMAIN_IMMOBILE = 3
	};

	// Grouped movement characteristics of the unit seeking safety.
	// Passing this struct replaces multiple scalar/boolean parameters across pruning routines.
	struct UnitMovementProfile
	{
		int baseMoves;
		int movesLeft;
		int domain; // SafetyPruningDomain or engine DomainTypes
		bool flatMovementCost;

		UnitMovementProfile()
			: baseMoves(1), movesLeft(0), domain(SAFETY_DOMAIN_LAND), flatMovementCost(false)
		{
		}

		UnitMovementProfile(int baseMovesIn, int movesLeftIn, int domainIn, bool flatMovementCostIn)
			: baseMoves(baseMovesIn), movesLeft(movesLeftIn), domain(domainIn), flatMovementCost(flatMovementCostIn)
		{
		}
	};

	// Note: callers must supply wrap-aware step distance (stepDistance() from
	// CvGameCoreUtils.h). A plain Chebyshev distance without map wrapping would
	// overestimate distance, which would overestimate min path turns and make
	// the pruning bounds unsound.

	inline int calcDeterministicPlotBase(
		int defendingUnitCount,
		bool animalDanger,
		bool ownedByUnitOwner,
		bool ownedByNonHostile,
		int defenseModifier)
	{
		int base = defendingUnitCount * DEFENDING_UNIT_WEIGHT;
		if (animalDanger)
		{
			if (ownedByUnitOwner)
			{
				base += ANIMAL_DANGER_OWNED_TERRITORY_BONUS;
			}
			else if (ownedByNonHostile)
			{
				base += ANIMAL_DANGER_NON_HOSTILE_TERRITORY_BONUS;
			}
		}
		base += defenseModifier;
		return base;
	}

	inline int applyTerrainTurnDamage(int value, int unitDamagePercent, int terrainTurnDamage)
	{
		if (terrainTurnDamage > 0)
		{
			const int effectiveHealthPercent = std::max(1, FULL_HEALTH_PERCENT - unitDamagePercent - terrainTurnDamage);
			value *= effectiveHealthPercent;
			value /= TERRAIN_DAMAGE_DIVISOR;
		}
		return value;
	}

	inline int calcFinalPlotScore(
		int deterministicBase,
		int bonusOrRoll,
		int unitDamagePercent,
		int terrainTurnDamage,
		int pathTurns,
		int bestValue,
		bool plotInDanger)
	{
		int value = deterministicBase + bonusOrRoll;
		value = applyTerrainTurnDamage(value, unitDamagePercent, terrainTurnDamage);
		value = (value * DISTANCE_DISCOUNT_SCALE) / (pathTurns + DISTANCE_DISCOUNT_OFFSET);
		if (value > bestValue && plotInDanger)
		{
			value /= IN_DANGER_SCORE_DIVISOR;
		}
		return value;
	}

	// Sound lower bound on path turns (minTurns <= pathTurns is guaranteed):
	// - At current plot: 0 turns.
	// - Off current plot with 0 movement remaining: at least 1 turn.
	// - Air domain or flat movement cost: at most max(1, baseMoves) tiles/turn.
	// - Sea domain (no water routes; promotions at most halve step cost): at most max(1, 2 * baseMoves) tiles/turn.
	// - Land domain without flat movement cost: routes (roads/trails/railroads) and double-move
	//   promotions can reach distant tiles on the current turn (0 path turns), so minTurns = 0.
	inline int calcMinPossiblePathTurns(
		bool atPlot,
		int stepDistance,
		const UnitMovementProfile& profile)
	{
		if (atPlot || stepDistance <= 0)
		{
			return 0;
		}
		if (profile.movesLeft <= 0)
		{
			return MIN_EXHAUSTED_MOVES_PATH_TURNS;
		}
		if (profile.flatMovementCost)
		{
			const int moves = std::max(1, profile.baseMoves);
			return std::max(0, (stepDistance - 1) / moves);
		}

		switch (profile.domain)
		{
		case SAFETY_DOMAIN_AIR:
			{
				const int airMoves = std::max(1, profile.baseMoves);
				return std::max(0, (stepDistance - 1) / airMoves);
			}
		case SAFETY_DOMAIN_SEA:
			{
				const int maxSeaSteps = std::max(1, MAX_SEA_SPEED_MULTIPLIER * profile.baseMoves);
				return std::max(0, (stepDistance - 1) / maxSeaSteps);
			}
		case SAFETY_DOMAIN_IMMOBILE:
			return MIN_IMMOBILE_PATH_TURNS;
		case SAFETY_DOMAIN_LAND:
		default:
			return 0;
		}
	}

	// Fast upper bound without querying terrain turn damage or plot danger.
	inline int calcFastUpperScore(
		int deterministicBase,
		bool atPlot,
		int stepDistance,
		const UnitMovementProfile& profile)
	{
		const int maxPreDamage = deterministicBase + (atPlot ? CURRENT_PLOT_STAY_BONUS : THEORETICAL_MAX_RANDOM_ROLL);
		if (maxPreDamage <= 0)
		{
			return 0;
		}
		const int minTurns = calcMinPossiblePathTurns(atPlot, stepDistance, profile);
		return (maxPreDamage * DISTANCE_DISCOUNT_SCALE) / (minTurns + DISTANCE_DISCOUNT_OFFSET);
	}

	// Returns true iff the plot's theoretical maximum score (under maximum RNG roll
	// THEORETICAL_MAX_RANDOM_ROLL and minimum possible path turns) cannot beat bestValue.
	inline bool shouldPruneBeforePath(
		int deterministicBase,
		bool atPlot,
		int unitDamagePercent,
		int terrainTurnDamage,
		bool plotInDanger,
		int stepDistance,
		const UnitMovementProfile& profile,
		int bestValue)
	{
		if (bestValue <= 0)
		{
			return false;
		}
		const int maxPreDamage = deterministicBase + (atPlot ? CURRENT_PLOT_STAY_BONUS : THEORETICAL_MAX_RANDOM_ROLL);
		if (maxPreDamage <= 0)
		{
			return true;
		}
		const int minTurns = calcMinPossiblePathTurns(atPlot, stepDistance, profile);
		const int maxScore = calcFinalPlotScore(
			deterministicBase,
			atPlot ? CURRENT_PLOT_STAY_BONUS : THEORETICAL_MAX_RANDOM_ROLL,
			unitDamagePercent,
			terrainTurnDamage,
			minTurns,
			bestValue,
			plotInDanger);
		return maxScore <= bestValue;
	}
}

// Backward-compatibility alias for callers using the CvSafetyPruning namespace
namespace CvSafetyPruning = safety;

#endif // CV_SAFETY_PRUNING_H
