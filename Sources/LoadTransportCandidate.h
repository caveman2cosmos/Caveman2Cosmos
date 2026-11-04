#pragma once

#include <map>
#include <algorithm>

// One read-only AI_load evaluation can inspect many transports. Reservation
// missions do not change until orders are issued after that evaluation, so
// collect them once instead of rescanning every player group for each ship.
class LoadReservationCounts
{
public:
	LoadReservationCounts(const CvPlayer& player, const CvSelectionGroup* ignored,
		const MissionAITypes* missions, int missionCount, bool cargoVolume)
		: m_player(player), m_ignored(ignored), m_missions(missions),
		m_missionCount(missionCount), m_cargoVolume(cargoVolume), m_ready(false) {}

	int reservedSpace(const CvUnit* transport)
	{
		if (!m_ready) rebuild();
		std::map<const CvUnit*, int>::const_iterator it = m_counts.find(transport);
		return it == m_counts.end() ? 0 : it->second;
	}

private:
	const CvPlayer& m_player;
	const CvSelectionGroup* m_ignored;
	const MissionAITypes* m_missions;
	int m_missionCount;
	bool m_cargoVolume, m_ready;
	std::map<const CvUnit*, int> m_counts;

	void rebuild()
	{
		foreach_(const CvSelectionGroup* group, m_player.groups())
		{
			if (group == m_ignored) continue;
			const CvUnit* target = group->AI_getMissionAIUnit();
			if (target == NULL) continue;
			for (int i = 0; i < m_missionCount; ++i)
			{
				if (m_missions[i] == NO_MISSIONAI || m_missions[i] == group->AI_getMissionAIType())
					m_counts[target] += m_cargoVolume ? group->getNumUnitCargoVolumeTotal() : group->getNumUnits();
			}
		}
		m_ready = true;
	}
};

// Score is cargo penalty + path turns. A tied/lower score keeps the earlier ship.
inline int loadTransportPathLimit(int configuredLimit, int bestScore, int cargoPenalty)
{
	if (bestScore == MAX_INT) return configuredLimit;
	if (cargoPenalty >= bestScore) return -1;
	return std::min(configuredLimit, bestScore - cargoPenalty - 1);
}

// canLoadOntoUnit is a read-only capacity/type check. Reject non-transports
// before probing reachability and build the flood only for a real load option.
inline bool reachableLoadTransport(const CvUnit* passenger, const CvUnit* transport,
	CvReachablePlotSet& reachable, int range, bool& ready, UnitAITypes requestedRole)
{
	if (transport->AI_getUnitAIType() != requestedRole) return false;
	if (!passenger->canLoadOntoUnit(transport, transport->plot())) return false;
	// A loadable ship is not necessarily a reachable boarding location. In
	// particular, ordinary land AI cannot enter open water even when a friendly
	// transport is there. Do not march to that speculative path destination and
	// then discard the load reservation to resume guarding a nearby city.
	// Already colocated units need no movement and can load even on a full tile.
	if (!passenger->atPlot(transport->plot())
		&& !passenger->canEnterPlot(transport->plot(), MoveCheck::IgnoreLocation)) return false;
	if (!ready) { reachable.Populate(range); ready = true; }
	return reachable.find(transport->plot()) != reachable.end();
}
