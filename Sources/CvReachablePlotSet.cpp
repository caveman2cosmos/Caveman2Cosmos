//	Class representing a set of plots reachable in a fixed number of tiles with given flags

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "CvCity.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvReachablePlotSet.h"
#include "CvSelectionGroup.h"

CvReachablePlotSet::const_iterator::const_iterator(const CvReachablePlotSet* parent, stdext::hash_map<CvPlot*,CvReachablePlotInfo>::const_iterator& itr) : m_parent(parent)
{
	m_itr = itr;
}

CvReachablePlotSet::const_iterator& CvReachablePlotSet::const_iterator::operator++()
{
	do
	{
		++m_itr;
	} while( m_itr != m_parent->end().m_itr && m_itr->second.iStepDistance > m_parent->m_iRange );

	return (*this);
}

bool CvReachablePlotSet::const_iterator::operator==(const const_iterator& other) const
{
	return other.m_itr == m_itr;
}

bool CvReachablePlotSet::const_iterator::operator!=(const const_iterator& other) const
{
	return other.m_itr != m_itr;
}

CvReachablePlotSet::const_iterator& CvReachablePlotSet::const_iterator::operator=(const const_iterator& other)
{
	m_itr = other.m_itr;
	m_parent = other.m_parent;

	return (*this);
}

CvPlot*	CvReachablePlotSet::const_iterator::plot() const
{
	if ( m_itr == m_parent->end().m_itr )
	{
		return NULL;
	}
	return m_itr->first;
}

int CvReachablePlotSet::const_iterator::stepDistance() const
{
	if ( m_itr == m_parent->end().m_itr )
	{
		return MAX_INT;
	}
	return m_itr->second.iStepDistance;
}

int CvReachablePlotSet::const_iterator::outsideBorderDistance() const
{
	if (m_itr == m_parent->end().m_itr)
	{
		return MAX_INT;
	}
	return m_itr->second.iOutsideOwnedDistance;
}

int CvReachablePlotSet::const_iterator::getOpaqueInfo(int iActivityId) const
{
	if (m_itr == m_parent->end().m_itr)
	{
		return 0;
	}
	return m_itr->second.iOpaqueInfo[iActivityId];
}

void CvReachablePlotSet::const_iterator::setOpaqueInfo(int iActivityId, int iValue)
{
	if ( m_itr != m_parent->end().m_itr )
	{
		*(int*)&(m_itr->second.iOpaqueInfo[iActivityId]) = iValue;
	}
}

CvReachablePlotSet::CvReachablePlotSet(const CvSelectionGroup * group, int iFlags, int iRange, bool bCachable, int iOutsideOwnedRange)
{
	m_group = group;
	//	Always include move-through-enemy flag when constructing reachable sets
	//	because otherwise any flagless move that tries to target a tile to attack
	//	(so has an enemy in its terminal node) will register as unreachable
	m_iFlags = iFlags | MOVE_THROUGH_ENEMY;

	m_proxyTo = NULL;
	m_iRange = -1;
	m_iOutsideOwnedRange = iOutsideOwnedRange;
	m_bCachable = bCachable;
	m_reachablePlots = NULL;

	if ( iRange != -1 )
	{
		Populate(iRange);
	}
}

CvReachablePlotSet::~CvReachablePlotSet()
{
	SAFE_DELETE(m_reachablePlots);
}

CvReachablePlotSet::const_iterator CvReachablePlotSet::begin() const
{
	PROFILE_EXTRA_FUNC();
	CvReachablePlotSet::const_iterator result = CvReachablePlotSet::const_iterator(this, (m_proxyTo == NULL ? m_reachablePlots->begin() : m_proxyTo->m_reachablePlots->begin()));

	while (result != end() && result.stepDistance() > m_iRange)
	{
		++result;
	}
	return result;
}

CvReachablePlotSet::const_iterator CvReachablePlotSet::end() const
{
	return CvReachablePlotSet::const_iterator(this, (m_proxyTo == NULL ? m_reachablePlots->end() : m_proxyTo->m_reachablePlots->end()));
}

CvReachablePlotSet::const_iterator CvReachablePlotSet::find(CvPlot* plot) const
{
	if ( m_proxyTo == NULL )
	{
		return CvReachablePlotSet::const_iterator(this, m_reachablePlots->find(plot));
	}
	CvReachablePlotSet::const_iterator result = m_proxyTo->find(plot);

	if (result.stepDistance() <= m_iRange)
	{
		return result;
	}
	return end();
}

int CvReachablePlotSet::getRange() const
{
	return m_proxyTo == NULL ? m_iRange : m_proxyTo->m_iRange;
}

void CvReachablePlotSet::setRange(int iRange)
{
	if (m_proxyTo == NULL)
	{
		m_iRange = iRange;
	}
	else m_proxyTo->m_iRange = iRange;
}

typedef struct CvReachablePlotSetCacheEntry
{
	CvReachablePlotSet*	plotSet;
	int					iLRUSeq;
} CvReachablePlotSetCacheEntry;

#define	PLOT_SET_CACHE_SIZE	2
typedef struct
{
	CvReachablePlotSetCacheEntry	cacheEntries[PLOT_SET_CACHE_SIZE];
	int								iNextLRU;
} PlotSetCache;

static PlotSetCache* CachedPlotSets = NULL;

void CvReachablePlotSet::ClearCache()
{
	PROFILE_EXTRA_FUNC();
	if (CachedPlotSets != NULL)
	{
		for (int iI = 0; iI < PLOT_SET_CACHE_SIZE; iI++)
		{
			SAFE_DELETE(CachedPlotSets->cacheEntries[iI].plotSet);
		}
	}
}

CvReachablePlotSet* CvReachablePlotSet::findCachedPlotSet(const CvSelectionGroup* pGroup, int iFlags, int iOutsideOwnedRange)
{
	PROFILE_EXTRA_FUNC();
	if (CachedPlotSets == NULL)
	{
		CachedPlotSets = new PlotSetCache;

		CachedPlotSets->iNextLRU = 1;

		for(int iI = 0; iI < PLOT_SET_CACHE_SIZE; iI++)
		{
			CachedPlotSets->cacheEntries[iI].iLRUSeq = 0;
			CachedPlotSets->cacheEntries[iI].plotSet = NULL;
		}
	}

	CvReachablePlotSetCacheEntry* pLRUEntry = NULL;
	int iBestLRU = MAX_INT;

	for (int iI = 0; iI < PLOT_SET_CACHE_SIZE; iI++)
	{
		CvReachablePlotSetCacheEntry* entry = &CachedPlotSets->cacheEntries[iI];

		if (entry->plotSet == NULL)
		{
			pLRUEntry = entry;
			break;
		}
		if (entry->plotSet->m_group == pGroup && entry->plotSet->m_iFlags == iFlags && entry->plotSet->m_iOutsideOwnedRange == iOutsideOwnedRange)
		{
			entry->iLRUSeq = CachedPlotSets->iNextLRU++;
			return entry->plotSet;
		}
		if (entry->iLRUSeq < iBestLRU)
		{
			iBestLRU = entry->iLRUSeq;
			pLRUEntry = entry;
		}
	}

	SAFE_DELETE(pLRUEntry->plotSet);

	pLRUEntry->plotSet = new CvReachablePlotSet(pGroup, iFlags);
	pLRUEntry->plotSet->m_reachablePlots = new stdext::hash_map<CvPlot*,CvReachablePlotInfo>();
	pLRUEntry->iLRUSeq = CachedPlotSets->iNextLRU++;

	return pLRUEntry->plotSet;
}

void CvReachablePlotSet::Populate(int iRange)
{
	PROFILE_FUNC();

	if (m_bCachable)
	{
		m_proxyTo = findCachedPlotSet(m_group, m_iFlags, m_iOutsideOwnedRange);
	}
	else if ( m_reachablePlots == NULL )
	{
		m_reachablePlots = new stdext::hash_map<CvPlot*,CvReachablePlotInfo>();
	}
	else
	{
		m_reachablePlots->clear();
	}
	m_iRange = getRange();

	if (m_iRange < iRange)
	{
		std::vector<std::pair<CvPlot*,int> > seedPlots;

		if ( m_iRange == -1 )
		{
			stdext::hash_map<CvPlot*, CvReachablePlotInfo>* reachablePlots = (m_proxyTo == NULL ? m_reachablePlots : m_proxyTo->m_reachablePlots);

			CvReachablePlotInfo	info;

			memset(&info, 0, sizeof(info));

			(*reachablePlots)[m_group->plot()] = info;
			seedPlots.push_back(std::make_pair(m_group->plot(),0));

			m_iRange = 0;
			setRange(0);
		}
		else
		{
			for(CvReachablePlotSet::const_iterator itr = begin(), itrend = end(); itr != itrend; ++itr)
			{
				if ( itr.stepDistance() == m_iRange )
				{
					seedPlots.push_back(std::make_pair(itr.plot(), itr.outsideBorderDistance()));
				}
			}
		}
		enumerateReachablePlotsInternal(iRange - getRange(), getRange()+1, seedPlots);
		setRange(iRange);
	}

	m_iRange = iRange;
}

bool CvReachablePlotSet::canMoveBetweenWithFlags(const CvSelectionGroup* group, const CvPlot* pFromPlot, const CvPlot* pToPlot, int iFlags)
{
	if (group->getDomainType() == DOMAIN_SEA)
	{
		// Can't cross diagonally across 'land'
		if (pFromPlot->isWater() && pToPlot->isWater()
		&& !GC.getMap().plot(pFromPlot->getX(), pToPlot->getY())->isWater() && !GC.getMap().plot(pToPlot->getX(), pFromPlot->getY())->isWater()
		&& !group->canMoveAllTerrain())
		{
			return false;
		}

		// If told to allow adjacent coasts to be included in the generated set, do so now.
		if ((MOVE_ALLOW_ADJACENT_COASTAL & iFlags) != 0)
		{
			if (pFromPlot->isWater() && pToPlot->isCoastalLand())
			{
				return true;
			}
			if (!pFromPlot->isWater() && !pFromPlot->isFriendlyCity(*group->getHeadUnit(), true))
			{
				return false;
			}
		}
	}

	if (GC.getGame().isOption(GAMEOPTION_UNSUPPORTED_ZONE_OF_CONTROL))
	{
		const TeamTypes eTeam = group->getTeam();
		const PlayerTypes eOwner = group->getHeadOwner();

		// ZoC don't apply into cities of the unit owner
		if (pToPlot->getPlotCity() == NULL || pToPlot->getPlotCity()->getTeam() != eTeam)
		{
			// Fort ZoC
			const PlayerTypes eDefender = pFromPlot->controlsAdjacentZOCSource(eTeam);
			if (eDefender != NO_PLAYER)
			{
				const CvPlot* pZoneOfControl = pFromPlot->isInFortControl(true, eDefender, eOwner);
				const CvPlot* pForwardZoneOfControl = pToPlot->isInFortControl(true, eDefender, eOwner);
				if (pZoneOfControl != NULL && pForwardZoneOfControl != NULL
				&& pZoneOfControl == pToPlot->isInFortControl(true, eDefender, eOwner, pZoneOfControl)
				&& !group->canIgnoreZoneofControl())
				{
					return false;
				}
			}
			// City ZoC
			if (pFromPlot->isInCityZoneOfControl(eOwner) && pToPlot->isInCityZoneOfControl(eOwner) && !group->canIgnoreZoneofControl())
			{
				return false;
			}
		}
		// Promotion ZoC
		if (GC.getGame().isAnyoneHasUnitZoneOfControl() && !group->canIgnoreZoneofControl()
		&& pFromPlot->isInUnitZoneOfControl(eOwner) && pToPlot->isInUnitZoneOfControl(eOwner))
		{
			return false;
		}
	}
	return moveToValid(group, pToPlot, iFlags);
}

void CvReachablePlotSet::enumerateReachablePlotsInternal(int iRange, int iDepth, std::vector< std::pair<CvPlot*,int> >& prevRing)
{
	PROFILE_EXTRA_FUNC();
	if (iRange > 0 && !prevRing.empty())
	{
		std::vector<std::pair<CvPlot*,int> >	nextRing;

		stdext::hash_map<CvPlot*, CvReachablePlotInfo>* reachablePlots = (m_proxyTo == NULL ? m_reachablePlots : m_proxyTo->m_reachablePlots);

		for (std::vector<std::pair<CvPlot*,int> >::const_iterator itr = prevRing.begin(), itrend = prevRing.end(); itr != itrend; ++itr)
		{
			foreach_(CvPlot* pAdjacentPlot, (*itr).first->adjacent())
			{
				if (reachablePlots->find(pAdjacentPlot) == reachablePlots->end())
				{
					bool bValidAsTerminus = false;
					bool bValid = ContextFreeNewPathValidFunc(m_group, (*itr).first->getX(), (*itr).first->getY(), pAdjacentPlot->getX(), pAdjacentPlot->getY(), m_iFlags, false, false, 0, NULL, &bValidAsTerminus);

					if (!bValid)
					{
						bool bDummy;

						bValidAsTerminus |= NewPathDestValid(m_group, pAdjacentPlot->getX(), pAdjacentPlot->getY(), m_iFlags, bDummy);
					}

					//if ( canMoveBetweenWithFlags(m_group, (*itr), pAdjacentPlot, m_iFlags) )
					if ( bValid || bValidAsTerminus )
					{
						CvReachablePlotInfo	info;

						memset(&info, 0, sizeof(info));
						info.iStepDistance = iDepth;

						if ((*itr).first->getTeam() != m_group->getTeam())
						{
							info.iOutsideOwnedDistance = (*itr).second + 1;

							if ( m_iOutsideOwnedRange != -1 && info.iOutsideOwnedDistance >= m_iOutsideOwnedRange )
							{
								bValid = false;
							}
						}
						else
						{
							info.iOutsideOwnedDistance = 0;
						}

						(*reachablePlots)[pAdjacentPlot] = info;

						if (bValid && pAdjacentPlot->isRevealed(m_group->getTeam(), false))
						{
							nextRing.push_back(std::make_pair(pAdjacentPlot, info.iOutsideOwnedDistance));
						}
					}
				}
			}
		}
		enumerateReachablePlotsInternal(iRange - 1, iDepth + 1, nextRing);
	}
}
