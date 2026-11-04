#pragma once
#include <vector>

// A read-only target-selection helper, never a world/turn cache. Create a new
// instance for each selector and do not use it after issuing any command.
// Combat itself continues to call CvUnit::bestInterceptor with live state.
class AirInterceptionCandidates
{
public:
	explicit AirInterceptionCandidates(const CvUnit& attacker) : m_attacker(attacker), m_ready(false) {}

	CvUnit* bestInterceptor(const CvPlot* target)
	{
		if (!m_ready)
		{
			m_ready = true;
			for (int i = 0; i < MAX_PLAYERS; ++i)
			{
				const CvPlayer& player = GET_PLAYER((PlayerTypes)i);
				if (!player.isAlive() || m_attacker.isInvisible(player.getTeam(), false, false)) continue;
				foreach_(CvUnit* unit, player.getAirInterceptionCandidates())
				{
					if (unit->canAirDefend() && !unit->isMadeInterception()
						&& m_attacker.isEnemy(player.getTeam(), NULL, unit)
						&& (unit->getDomainType() != DOMAIN_AIR
							|| (!unit->hasMoved() && unit->getGroup()->getActivityType() == ACTIVITY_INTERCEPT)))
					{
						m_units.push_back(unit);
					}
				}
			}
		}
		int bestValue = 0;
		CvUnit* best = NULL;
		for (size_t i = 0; i < m_units.size(); ++i)
		{
			CvUnit* unit = m_units[i];
			if (plotDistance(unit->getX(), unit->getY(), target->getX(), target->getY()) <= unit->airRange())
			{
				const int value = unit->currInterceptionProbability();
				if (value > bestValue) { bestValue = value; best = unit; }
			}
		}
		return best;
	}

private:
	const CvUnit& m_attacker;
	bool m_ready;
	std::vector<CvUnit*> m_units;
};
