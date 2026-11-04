#pragma once

// Ordinary animal buildings and one-shot uses should not send animals across
// the empire. Heritage retains its separate, higher-priority selection.
enum { SUBDUED_ANIMAL_MAX_TRAVEL_TURNS = 3 };

inline bool preferLocalAnimalOutcome(int value, int turns, bool onSite,
	int bestValue, int bestTurns, bool bestOnSite)
{
	return value > 0 && turns >= 0 && turns <= SUBDUED_ANIMAL_MAX_TRAVEL_TURNS
		&& (onSite != bestOnSite ? onSite
			: (turns < bestTurns || (turns == bestTurns && value > bestValue)));
}
