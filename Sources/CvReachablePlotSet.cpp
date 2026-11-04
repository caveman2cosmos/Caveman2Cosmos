//	Class representing a set of plots reachable in a fixed number of tiles with given flags

#include "FProfiler.h"

#include "CvGameCoreDLL.h"
#include "NavalDecisionDiagnostics.h"
#include "AIPerf.h"
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
	PROFILE_EXTRA_FUNC();
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

// C2C Optimization: Increased cache size from 2 to 16 to reduce cache eviction rate
// during AI pathing and group movement evaluations across large mod maps.
#define	PLOT_SET_CACHE_SIZE	16
typedef struct
{
	CvReachablePlotSetCacheEntry	cacheEntries[PLOT_SET_CACHE_SIZE];
	int								iNextLRU;
} PlotSetCache;

static PlotSetCache* CachedPlotSets = NULL;

void CvReachablePlotSet::ClearCache()
{
    NAVAL_DIAG_BARRIER();

	AI_PERF_COUNT(REACHABLE_CLEARS, 1);
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
			AI_PERF_COUNT(REACHABLE_HITS, 1);
			return entry->plotSet;
		}
		if (entry->iLRUSeq < iBestLRU)
		{
			iBestLRU = entry->iLRUSeq;
			pLRUEntry = entry;
		}
	}

	AI_PERF_COUNT(REACHABLE_MISSES, 1);
	if (pLRUEntry->plotSet != NULL) AI_PERF_COUNT(REACHABLE_EVICTIONS, 1);
	SAFE_DELETE(pLRUEntry->plotSet);

#ifdef C2C_AI_PERF_BASELINE
	pLRUEntry->plotSet = new CvReachablePlotSet(pGroup, iFlags);
#else
	// Preserve the full lookup key. Population uses the caller's border limit;
	// forgetting it here makes every limited-border lookup miss the cache.
	pLRUEntry->plotSet = new CvReachablePlotSet(pGroup, iFlags, -1, true, iOutsideOwnedRange);
#endif
	pLRUEntry->plotSet->m_reachablePlots = new stdext::hash_map<CvPlot*,CvReachablePlotInfo>();
	pLRUEntry->iLRUSeq = CachedPlotSets->iNextLRU++;

	return pLRUEntry->plotSet;
}

bool CvReachablePlotSet::containsWithinRange(CvPlot* plot, int iRange)
{
 if (m_iRange < iRange) Populate(iRange);
 return find(plot) != end();
}

bool CvReachablePlotSet::tryPopulateFromCache(int iRange)
{
	if (iRange < 0) return false;
	if (m_iRange >= iRange) return true;
	if (!m_bCachable || CachedPlotSets == NULL) return false;
	for (int i = 0; i < PLOT_SET_CACHE_SIZE; ++i)
	{
		CvReachablePlotSetCacheEntry& entry = CachedPlotSets->cacheEntries[i];
		if (entry.plotSet != NULL && entry.plotSet->m_group == m_group
			&& entry.plotSet->m_iFlags == m_iFlags
			&& entry.plotSet->m_iOutsideOwnedRange == m_iOutsideOwnedRange)
		{
			if (entry.plotSet->m_iRange < iRange) return false;
			m_proxyTo = entry.plotSet;
			m_iRange = iRange;
			entry.iLRUSeq = CachedPlotSets->iNextLRU++;
			return true;
		}
	}
	return false;
}

void CvReachablePlotSet::PopulateForTargets(const std::vector<CvPlot*>& targets, int iRange)
{
    NAVAL_DIAG_QUERY(TARGET_FLOOD,m_group->getOwner(),m_group->getID(),m_group->plot()->getX(),m_group->plot()->getY(),m_iFlags,iRange,m_iOutsideOwnedRange,m_bCachable,((int)targets.size()),0,0);

 if (targets.empty())
 {
  if (m_iRange == -1 && m_reachablePlots == NULL && m_proxyTo == NULL)
  {m_bCachable=false;m_reachablePlots=new stdext::hash_map<CvPlot*,CvReachablePlotInfo>();m_iRange=0;}
  NAVAL_DIAG_VOID_RETURN(m_iRange,m_proxyTo!=NULL);
 }
 if (tryPopulateFromCache(iRange)) NAVAL_DIAG_VOID_RETURN(m_iRange,m_proxyTo!=NULL);
 // Existing populated objects retain the ordinary expansion contract. The
 // optimized consumers create a fresh view and call this only once.
 if (m_iRange != -1 || m_reachablePlots != NULL || m_proxyTo != NULL)
 {
  if (m_bCachable || m_proxyTo != NULL) {Populate(iRange);NAVAL_DIAG_VOID_RETURN(m_iRange,m_proxyTo!=NULL);}
  // A previous private target view may be incomplete. Rebuild privately;
  // never extend it as though it were the ordinary complete-range cache.
  SAFE_DELETE(m_reachablePlots);m_iRange=-1;
 }
 PlotDangerCacheScope dangerCache(m_group->getOwner());
 AI_PERF_SCOPE(REACHABLE_POPULATE);
 m_bCachable = false;
 m_reachablePlots = new stdext::hash_map<CvPlot*,CvReachablePlotInfo>();
 CvReachablePlotInfo info;memset(&info,0,sizeof(info));
 (*m_reachablePlots)[m_group->plot()] = info;
 std::vector<std::pair<CvPlot*,int> > seeds;
 seeds.push_back(std::make_pair(m_group->plot(),0));
 m_iRange = 0;
 enumerateReachablePlotsInternal(iRange,1,seeds,&targets,&m_iRange);
    NAVAL_DIAG_FINISH(m_iRange,m_proxyTo!=NULL);
}

namespace {
// VC71 hash_map is an append-only linear hash here. Its iteration order is
// bucket then key order. Reconstruct that order for the small boundary only;
// do not change BFS tie order or depend on private STL layout. Other toolchains
// retain the original full-map scan until separately validated.
#if defined(_MSC_VER) && _MSC_VER == 1310
struct ReachableFrontierOrder
{
 typedef stdext::hash_map<CvPlot*,CvReachablePlotInfo> Map;
 Map::key_compare compare;size_t mask,maximum;
 ReachableFrontierOrder(const Map& map):compare(map.key_comp()),mask(1),maximum(std::max((size_t)1,(map.size()+Map::bucket_size-1)/Map::bucket_size))
 {while(mask<maximum-1)mask=(mask<<1)+1;}
 size_t bucket(CvPlot* p)const{size_t b=compare(p)&mask;return b<maximum?b:b-((mask>>1)+1);}
 bool operator()(const std::pair<CvPlot*,int>& a,const std::pair<CvPlot*,int>& b)const
 {const size_t x=bucket(a.first),y=bucket(b.first);return x!=y?x<y:compare(a.first,b.first);}
};
#endif
bool orderReachableFrontier(const stdext::hash_map<CvPlot*,CvReachablePlotInfo>& map,std::vector<std::pair<CvPlot*,int> >& frontier)
{
#if defined(_MSC_VER) && _MSC_VER == 1310
 if (map.size()<512) return false;
 std::sort(frontier.begin(),frontier.end(),ReachableFrontierOrder(map));return true;
#else
 return false;
#endif
}
}

void CvReachablePlotSet::Populate(int iRange)
{
    NAVAL_DIAG_QUERY(FLOOD,m_group->getOwner(),m_group->getID(),m_group->plot()->getX(),m_group->plot()->getY(),m_iFlags,iRange,m_iOutsideOwnedRange,m_bCachable,0,0,0);

	// The flood only reads gameplay state. Share danger answers between its
	// repeated destination tests, but never across moves or separate floods.
	PlotDangerCacheScope dangerCache(m_group->getOwner());
	AI_PERF_SCOPE(REACHABLE_POPULATE);
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
        m_savedFrontier.erase(m_savedFrontier.begin(),m_savedFrontier.end());
	}
	m_iRange = getRange();

	if (m_iRange < iRange)
	{
		AI_PERF_SCOPE(REACHABLE_EXPANSION);
		std::vector<std::pair<CvPlot*,int> > seedPlots;
        CvReachablePlotSet* storage=m_proxyTo==NULL?this:m_proxyTo;

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
            seedPlots=storage->m_savedFrontier;
            if (m_iRange<16 || !orderReachableFrontier(*storage->m_reachablePlots,seedPlots))
            {
                seedPlots.erase(seedPlots.begin(),seedPlots.end());
                for(CvReachablePlotSet::const_iterator itr=begin(),itrend=end();itr!=itrend;++itr)
                {
                    if (itr.stepDistance()==m_iRange)
                        seedPlots.push_back(std::make_pair(itr.plot(),itr.outsideBorderDistance()));
                }
            }
		}
		enumerateReachablePlotsInternal(iRange - getRange(), getRange()+1, seedPlots);
		setRange(iRange);
	}

	m_iRange = iRange;
    NAVAL_DIAG_FINISH(m_iRange,m_proxyTo!=NULL);
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

void CvReachablePlotSet::enumerateReachablePlotsInternal(int iRange, int iDepth, std::vector< std::pair<CvPlot*,int> >& prevRing, const std::vector<CvPlot*>* targets, int* completedDepth)
{
    NAVAL_DIAG_FLOOD_BEGIN();

	PROFILE_EXTRA_FUNC();
	if (completedDepth) *completedDepth = iDepth - 1;
	if (iRange <= 0 || prevRing.empty())
	{
		return;
	}

	stdext::hash_map<CvPlot*, CvReachablePlotInfo>* reachablePlots = (m_proxyTo == NULL ? m_reachablePlots : m_proxyTo->m_reachablePlots);
	const int iWidth = GC.getMap().getGridWidth();
	// Membership is needed for every adjacent edge. Keep the original hash map
	// for metadata and iteration order, but use a flood-local bit per map plot
	// for these repeated lookups. No answers survive this read-only flood.
	std::vector<unsigned int> visited((GC.getMap().numPlots() + 31) / 32, 0);
	// Rejected terminal predicates are independent of the incoming edge.
	// Every edge still runs its normal validation; reuse only within this flood.
	std::vector<unsigned int> rejectedTerminals(visited.size(), 0);
	for (stdext::hash_map<CvPlot*, CvReachablePlotInfo>::const_iterator itr = reachablePlots->begin(); itr != reachablePlots->end(); ++itr)
	{
		const int iPlot = itr->first->getY() * iWidth + itr->first->getX();
		visited[iPlot >> 5] |= 1U << (iPlot & 31);
	}
    CvReachablePlotSet* storage=m_proxyTo==NULL?this:m_proxyTo;
    std::vector<std::pair<CvPlot*,int> >& saved=storage->m_savedFrontier;
    saved.erase(saved.begin(),saved.end());
	std::vector<std::pair<CvPlot*,int> > nextRing;
	while (iRange > 0 && !prevRing.empty())
	{
        if (targets != NULL)
        {
            bool complete = true;
            for (size_t t=0;t<targets->size();++t)
            {
                const CvPlot* p=(*targets)[t];
                const int id=p->getY()*iWidth+p->getX();
                if (!(visited[id>>5] & (1U<<(id&31)))) {complete=false;break;}
            }
            if (complete) break;
        }
		// VC71 clear() releases capacity; erase keeps this frontier reusable.
		nextRing.erase(nextRing.begin(), nextRing.end());

		for (std::vector<std::pair<CvPlot*,int> >::const_iterator itr = prevRing.begin(), itrend = prevRing.end(); itr != itrend; ++itr)
		{
            NAVAL_DIAG_FLOOD_NODE();
			foreach_(CvPlot* pAdjacentPlot, (*itr).first->adjacent())
			{
				NAVAL_DIAG_FLOOD_EDGE();
				const int iPlot = pAdjacentPlot->getY() * iWidth + pAdjacentPlot->getX();
				const unsigned int iMask = 1U << (iPlot & 31);
				if (!(visited[iPlot >> 5] & iMask))
				{
					bool bValidAsTerminus = false;
					bool bValid = ContextFreeNewPathValidFunc(m_group, (*itr).first->getX(), (*itr).first->getY(), pAdjacentPlot->getX(), pAdjacentPlot->getY(), m_iFlags, false, false, 0, NULL, &bValidAsTerminus);

					// A failed edge may already be legal as a terminal node. The
					// fallback only adds that fact; do not re-evaluate it when known.
					if (!bValid && !bValidAsTerminus && !(rejectedTerminals[iPlot >> 5] & iMask))
					{
						bool bDummy;

						bValidAsTerminus |= NewPathDestValid(m_group, pAdjacentPlot->getX(), pAdjacentPlot->getY(), m_iFlags, bDummy);
						if (!bValidAsTerminus) rejectedTerminals[iPlot >> 5] |= iMask;
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

						visited[iPlot >> 5] |= iMask;
						reachablePlots->insert(std::make_pair(pAdjacentPlot, info));
                        if (iRange==1 && iDepth>=16) saved.push_back(std::make_pair(pAdjacentPlot,info.iOutsideOwnedDistance));

						if (bValid && pAdjacentPlot->isRevealed(m_group->getTeam(), false))
						{
							nextRing.push_back(std::make_pair(pAdjacentPlot, info.iOutsideOwnedDistance));
						}
					}
				}
			}
		}
		// Reuse the two frontiers without changing discovery or insertion order.
		prevRing.swap(nextRing);
		--iRange;
		++iDepth;
		if (completedDepth) *completedDepth = iDepth - 1;
	}
    NAVAL_DIAG_FLOOD_END();
}
