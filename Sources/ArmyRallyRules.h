#pragma once

// Keep a rally a directed merge into a stronger available army. Never turn a
// garrison, transport embarkation or support assignment into a rally target.
inline bool armyRallyRole(UnitAITypes role)
{
	return role == UNITAI_ATTACK_CITY || role == UNITAI_ATTACK
		|| role == UNITAI_COUNTER || role == UNITAI_RESERVE;
}

inline bool armyRallyMission(MissionAITypes mission)
{
	return mission == NO_MISSIONAI || mission == MISSIONAI_GROUP
		|| mission == MISSIONAI_ASSAULT || mission == MISSIONAI_REGROUP
		|| mission == MISSIONAI_WAIT_FOR_ESCORT || mission == MISSIONAI_WAIT_FOR_SEE_INVISIBLE;
}

inline bool armyRallyStronger(int ours, int ourID, int other, int otherID)
{
	// A stable direction for equal strengths prevents mutually chasing groups.
	return other > ours || (other > 0 && other == ours && otherID < ourID);
}

struct ArmyRallyCandidate
{
	CvSelectionGroup* group;
	int distance, order;
	ArmyRallyCandidate(CvSelectionGroup* value, int d, int n) : group(value), distance(d), order(n) {}
	bool operator<(const ArmyRallyCandidate& other) const
	{
		return distance < other.distance || (distance == other.distance && order < other.order);
	}
};
