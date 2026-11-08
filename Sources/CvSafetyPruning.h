#pragma once

#ifndef CV_SAFETY_PRUNING_H
#define CV_SAFETY_PRUNING_H

#include <algorithm>
#include <cstdlib>

// Pure helper functions for CvUnitAI::AI_safety and CvUnitAI::AI_safetyEval.
// Kept free of heavy engine headers so both CvGameCoreDLL (MSVC 2003) and
// standalone C++ unit tests can include and exercise the exact production logic.

namespace CvSafetyPruning
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
	//   iDiscountedScore = (iPreDistanceScore * DISTANCE_DISCOUNT_SCALE) / (iPathTurns + DISTANCE_DISCOUNT_OFFSET)
	// For iPathTurns = 0, this evaluates to 5/4 = 1.25x; for iPathTurns = 1, it evaluates to 5/5 = 1.0x.
	static const int DISTANCE_DISCOUNT_SCALE = 5;
	static const int DISTANCE_DISCOUNT_OFFSET = 4;

	// Terrain turn damage normalization constants.
	static const int FULL_HEALTH_PERCENT = 100;
	static const int TERRAIN_DAMAGE_DIVISOR = 200;

	// Penalty divisor applied when the candidate plot is marked in danger and beats iBestValue.
	static const int IN_DANGER_SCORE_DIVISOR = 2;

	// Sea movement step multiplier (naval promotions or coastal travel can at most double effective speed).
	static const int MAX_SEA_SPEED_MULTIPLIER = 2;

	// Minimum turn counts for immobilized or out-of-moves units attempting to reach a non-local plot.
	static const int MIN_EXHAUSTED_MOVES_PATH_TURNS = 1;
	static const int MIN_IMMOBILE_PATH_TURNS = 1;

	// Domain types matching DomainTypes in CvEnums.h for standalone test compatibility
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
		int iBaseMoves;
		int iMovesLeft;
		int iDomain; // SafetyPruningDomain or engine DomainTypes
		bool bFlatMovementCost;

		UnitMovementProfile()
			: iBaseMoves(1), iMovesLeft(0), iDomain(SAFETY_DOMAIN_LAND), bFlatMovementCost(false)
		{
		}

		UnitMovementProfile(int iBaseMovesIn, int iMovesLeftIn, int iDomainIn, bool bFlatMovementCostIn)
			: iBaseMoves(iBaseMovesIn), iMovesLeft(iMovesLeftIn), iDomain(iDomainIn), bFlatMovementCost(bFlatMovementCostIn)
		{
		}
	};

	inline int stepDist(int iX1, int iY1, int iX2, int iY2)
	{
		const int iDX = std::abs(iX1 - iX2);
		const int iDY = std::abs(iY1 - iY2);
		return iDX > iDY ? iDX : iDY;
	}

	inline int calcDeterministicPlotBase(
		int iDefendingUnitCount,
		bool bAnimalDanger,
		bool bOwnedByUnitOwner,
		bool bOwnedByNonHostile,
		int iDefenseModifier)
	{
		int iBase = iDefendingUnitCount * DEFENDING_UNIT_WEIGHT;
		if (bAnimalDanger)
		{
			if (bOwnedByUnitOwner)
			{
				iBase += ANIMAL_DANGER_OWNED_TERRITORY_BONUS;
			}
			else if (bOwnedByNonHostile)
			{
				iBase += ANIMAL_DANGER_NON_HOSTILE_TERRITORY_BONUS;
			}
		}
		iBase += iDefenseModifier;
		return iBase;
	}

	inline int applyTerrainTurnDamage(int iValue, int iUnitDamagePercent, int iTerrainTurnDamage)
	{
		if (iTerrainTurnDamage > 0)
		{
			const int iEffectiveHealthPercent = std::max(1, FULL_HEALTH_PERCENT - iUnitDamagePercent - iTerrainTurnDamage);
			iValue *= iEffectiveHealthPercent;
			iValue /= TERRAIN_DAMAGE_DIVISOR;
		}
		return iValue;
	}

	inline int calcFinalPlotScore(
		int iDeterministicBase,
		int iBonusOrRoll,
		int iUnitDamagePercent,
		int iTerrainTurnDamage,
		int iPathTurns,
		int iBestValue,
		bool bPlotInDanger)
	{
		int iValue = iDeterministicBase + iBonusOrRoll;
		iValue = applyTerrainTurnDamage(iValue, iUnitDamagePercent, iTerrainTurnDamage);
		iValue = (iValue * DISTANCE_DISCOUNT_SCALE) / (iPathTurns + DISTANCE_DISCOUNT_OFFSET);
		if (iValue > iBestValue && bPlotInDanger)
		{
			iValue /= IN_DANGER_SCORE_DIVISOR;
		}
		return iValue;
	}

	// Sound lower bound on path turns (iMinTurns <= iPathTurns is guaranteed):
	// - At current plot: 0 turns.
	// - Off current plot with 0 movement remaining: at least 1 turn.
	// - Air domain or flat movement cost: at most max(1, baseMoves) tiles/turn.
	// - Sea domain (no water routes; promotions at most halve step cost): at most max(1, 2 * baseMoves) tiles/turn.
	// - Land domain without flat movement cost: routes (roads/trails/railroads) and double-move
	//   promotions can reach distant tiles on the current turn (0 path turns), so iMinTurns = 0.
	inline int calcMinPossiblePathTurns(
		bool bAtPlot,
		int iStepDistance,
		const UnitMovementProfile& profile)
	{
		if (bAtPlot || iStepDistance <= 0)
		{
			return 0;
		}
		if (profile.iMovesLeft <= 0)
		{
			return MIN_EXHAUSTED_MOVES_PATH_TURNS;
		}
		if (profile.bFlatMovementCost)
		{
			const int iMoves = std::max(1, profile.iBaseMoves);
			return std::max(0, (iStepDistance - 1) / iMoves);
		}

		switch (profile.iDomain)
		{
		case SAFETY_DOMAIN_AIR:
			{
				const int iAirMoves = std::max(1, profile.iBaseMoves);
				return std::max(0, (iStepDistance - 1) / iAirMoves);
			}
		case SAFETY_DOMAIN_SEA:
			{
				const int iMaxSeaSteps = std::max(1, MAX_SEA_SPEED_MULTIPLIER * profile.iBaseMoves);
				return std::max(0, (iStepDistance - 1) / iMaxSeaSteps);
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
		int iDeterministicBase,
		bool bAtPlot,
		int iStepDistance,
		const UnitMovementProfile& profile)
	{
		const int iMaxPreDamage = iDeterministicBase + (bAtPlot ? CURRENT_PLOT_STAY_BONUS : THEORETICAL_MAX_RANDOM_ROLL);
		if (iMaxPreDamage <= 0)
		{
			return 0;
		}
		const int iMinTurns = calcMinPossiblePathTurns(bAtPlot, iStepDistance, profile);
		return (iMaxPreDamage * DISTANCE_DISCOUNT_SCALE) / (iMinTurns + DISTANCE_DISCOUNT_OFFSET);
	}

	// Returns true iff the plot's theoretical maximum score (under maximum RNG roll
	// THEORETICAL_MAX_RANDOM_ROLL and minimum possible path turns) cannot beat iBestValue.
	inline bool shouldPruneBeforePath(
		int iDeterministicBase,
		bool bAtPlot,
		int iUnitDamagePercent,
		int iTerrainTurnDamage,
		bool bPlotInDanger,
		int iStepDistance,
		const UnitMovementProfile& profile,
		int iBestValue)
	{
		if (iBestValue <= 0)
		{
			return false;
		}
		const int iMaxPreDamage = iDeterministicBase + (bAtPlot ? CURRENT_PLOT_STAY_BONUS : THEORETICAL_MAX_RANDOM_ROLL);
		if (iMaxPreDamage <= 0)
		{
			return true;
		}
		const int iMinTurns = calcMinPossiblePathTurns(bAtPlot, iStepDistance, profile);
		const int iMaxScore = calcFinalPlotScore(
			iDeterministicBase,
			bAtPlot ? CURRENT_PLOT_STAY_BONUS : THEORETICAL_MAX_RANDOM_ROLL,
			iUnitDamagePercent,
			iTerrainTurnDamage,
			iMinTurns,
			iBestValue,
			bPlotInDanger);
		return iMaxScore <= iBestValue;
	}
}

#endif // CV_SAFETY_PRUNING_H
