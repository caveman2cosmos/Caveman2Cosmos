// Included in CvContractBroker.cpp. Cities and city-like posts publish their
// own requirements once per owner turn. Only available units answer them.
#include "DefenseNeedEvents.h"
#include "DefensePostLink.h"
#include "CitySupportSearch.h"
#include "DefenseNeedCache.inl"
namespace {
bool isDefenseDemandPost(const CvPlot* target,PlayerTypes owner)
{
    if(!target || target->getOwner()!=owner)return false;
    const ImprovementTypes type=target->getMilitaryImprovementType();
    return type!=NO_IMPROVEMENT && (GC.getImprovementInfo(type).isMilitaryStructure()
        || GC.getImprovementInfo(type).isUpgradeRequiresFortify() || GC.getImprovementInfo(type).isActsAsCity());
}
}
void CvContractBroker::noteDefenderAvailability(const CvUnit* unit)
{
    if(DefenseDemandPerf::enabled() && unit->getDomainType()==DOMAIN_LAND && unit->AI_isCityAIType())
        ++m_defenseAvailabilityEpoch;
}

bool CvContractBroker::defenseNeedsDefender(const CvPlot* target)
{
    if(!target || target->getOwner()!=m_eOwner)return false;
    CvCity* city=target->getPlotCity();
    const int stationed=defenseStationed(target);
    int incoming=0,strength=0,happy=0;
    if(DefenseDemandPerf::cacheEnabled())
    {
        refreshDefenseCommitments();incoming=m_defenseCommitments.incoming(target->getX(),target->getY());
        strength=m_defenseCommitments.strength(target->getX(),target->getY())/100;
        happy=m_defenseCommitments.happy(target->getX(),target->getY());
    }
    if(city)return stationed+incoming<city->AI_minDefenders() || !defenseCityDefended(city,strength,false)
        || !defenseCityHappy(city,happy);
    if(!isDefenseDemandPost(target,m_eOwner) || !DefensePostLink::connected<CvPlot,CvCity>(target,m_eOwner,GC.getMap().getType()))return false;
    const int id=GC.getMap().plotNum(target->getX(),target->getY());
    const std::map<int,int>::const_iterator desired=m_defenseRequirements.find(id);
    return stationed+incoming<(desired==m_defenseRequirements.end()?1:desired->second);
}

void CvContractBroker::refreshDefenderRequests()
{
    if(!DefenseDemandPerf::enabled() || GET_PLAYER(m_eOwner).isNPC())return;
    const int turn=GC.getGame().getGameTurn();if(m_defenseDemandTurn==turn)return;
    m_defenseDemandTurn=turn;DefenseDemandPerf::count(DefenseDemandPerf::REFRESHES);
    std::map<int,int> incoming;
    refreshDefenseCommitments();
    if(!DefenseDemandPerf::cacheEnabled())foreach_(const CvSelectionGroup* group,GET_PLAYER(m_eOwner).groups())
    {
        const CvPlot* target=group->AI_getMissionAIPlot();
        if(target && !group->atPlot(target) && group->getNumUnits()>0
            && (group->AI_getMissionAIType()==MISSIONAI_GUARD_BONUS || group->AI_getMissionAIType()==MISSIONAI_GUARD_CITY || group->AI_getMissionAIType()==MISSIONAI_CONTRACT)
            && group->getHeadUnit()->AI_isCityAIType())
            foreach_(const CvUnit* unit,group->units())
                if(unit->AI_isCityAIType())++incoming[GC.getMap().plotNum(target->getX(),target->getY())];
    }
    // Native city strength requests already enter this broker during city AI.
    // Add only missing count/happiness requirements that they did not cover.
    foreach_(const CvCity* city,GET_PLAYER(m_eOwner).cities())
    {
        // performance's cities() range is already restricted to CURRENT_MAP.
        const CvPlot* target=city->plot();const int id=GC.getMap().plotNum(target->getX(),target->getY());
        if(DefenseDemandPerf::cacheEnabled())incoming[id]=m_defenseCommitments.incoming(target->getX(),target->getY());
        bool requested=false;
        for(size_t r=0;r<m_workRequests.size();++r)
            if(m_workRequests[r].bDefenseDemand && !m_workRequests[r].bFulfilled
                && m_workRequests[r].iAtX==target->getX() && m_workRequests[r].iAtY==target->getY())requested=true;
        if(requested)continue;
        const int have=defenseStationed(target);
        int missing=std::max(0,city->AI_minDefenders()-have-incoming[id]);
        if(missing==0 && incoming[id]==0 && !defenseCityHappy(city,0))missing=1;
        for(int n=0;n<missing;++n)
        {
            advertiseWork(city->isCapital()?MINIMUM_CAPITAL_DEFENDER_PRIORITY:MINIMUM_CITY_DEFENDER_PRIORITY,
                DEFENSIVE_UNITCAPABILITIES,target->getX(),target->getY(),NULL,UNITAI_CITY_DEFENSE,-1,NULL,10,true,1);
            DefenseDemandPerf::count(DefenseDemandPerf::DEMANDS);
        }
    }
    const std::vector<CvUnit*>& defenders=GET_PLAYER(m_eOwner).getUnitsByAIRole(UNITAI_CITY_DEFENSE);
    const CvUnitAI* observer=defenders.empty()?NULL:static_cast<const CvUnitAI*>(defenders.front());
    const std::vector<int>& posts=GET_PLAYER(m_eOwner).getDefensivePostPlotIndices();
    for(size_t i=0;i<posts.size();++i)
    {
        DefenseDemandPerf::count(DefenseDemandPerf::POSTS);
        const CvPlot* target=GC.getMap().plotByIndex(posts[i]);
        if(!isDefenseDemandPost(target,m_eOwner) || !DefensePostLink::connected<CvPlot,CvCity>(target,m_eOwner,GC.getMap().getType()))continue;
        const int required=observer?observer->AI_defenderDemandForPlot(target):1;
        m_defenseRequirements[posts[i]]=required;
        const int stationed=defenseStationed(target);
        if(DefenseDemandPerf::cacheEnabled())incoming[posts[i]]=m_defenseCommitments.incoming(target->getX(),target->getY());
        DefenseDemandPerf::count(DefenseDemandPerf::INCOMING,incoming[posts[i]]);
        const int missing=std::max(0,required-stationed-incoming[posts[i]]);
        if(missing>0)
        {
            // City minimums and high-priority escorts retain precedence.
            // Non-city requests cannot create unit-production tenders.
            advertiseWork(FLOATING_DEFENDER_REQUEST_PRIORITY_MAX+1,DEFENSIVE_UNITCAPABILITIES,
                target->getX(),target->getY(),NULL,NO_UNITAI,-1,NULL,4,true,missing);
            DefenseDemandPerf::count(DefenseDemandPerf::DEMANDS,missing);
        }
    }
}

bool CvContractBroker::canReleaseDefender(const CvUnit* unit) const
{
    if(!unit || unit->getOwner()!=m_eOwner || !unit->AI_isCityAIType() || !unit->canDefend()
        || unit->getDomainType()!=DOMAIN_LAND || isTransportCommitted(unit)
        || !unit->getGroup() || unit->getGroup()->getHeadUnit()!=unit || unit->getGroup()->getNumUnits()!=1
        || unit->getGroup()->isBusy() || !unit->canMove()
        || !static_cast<const CvUnitAI*>(unit)->AI_isAvailableForDefenseDemand())return false;
    const CvPlot* origin=unit->plot();if(!origin)return false;
    int pending=0,pendingStrength=0;
    for(std::set<int>::const_iterator i=m_pendingDefenders.begin();i!=m_pendingDefenders.end();++i)
    {
        const CvUnit* other=findUnit(*i);
        if(other && other!=unit && other->plot()==origin)
        {
            ++pending;
            if(origin->getPlotCity() && other->AI_isCityGarrison(origin->getPlotCity()))
                pendingStrength+=other->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE)/100;
        }
    }
    CvCity* city=origin->getPlotCity();
    if(city && city->getOwner()==m_eOwner)
    {
        const int defenders=defenseStationed(origin);
        if(defenders-pending-1<city->AI_minDefenders())return false;
        if(unit->isMilitaryHappiness() && !defenseCityHappy(city,-pending-1))return false;
        const int strength=unit->AI_isCityGarrison(city)?unit->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE)/100:0;
        if(!defenseCityDefended(city,-strength-pendingStrength,true))return false;
    }
    if(isDefenseDemandPost(origin,m_eOwner))
    {
        const int required=defensePostRequirement(unit,origin);
        if(defenseStationed(origin)-pending-1<required)return false;
    }
    return true;
}

advertisingUnit* CvContractBroker::findDemandDefender(const workRequest& request,bool thisPlotOnly,ReadOnlyPathFailures& failures)
{
    if(thisPlotOnly)return NULL;
    m_defenseFortRetries.synchronize(GC.getGame().getGameTurn(),GC.getMap().getType());
    if(request.iDefenseAttemptEpoch==m_defenseAvailabilityEpoch)
    {DefenseDemandPerf::count(DefenseDemandPerf::EPOCH_SKIPS);return NULL;}
    request.iDefenseAttemptEpoch=m_defenseAvailabilityEpoch;
    const CvPlot* target=GC.getMap().plot(request.iAtX,request.iAtY);
    if(!defenseNeedsDefender(target))return NULL;
    const bool routineDefense=CitySupportSearch::routine(target->getPlotCity(),m_eOwner);
    const int maxPath=CitySupportSearch::maxTurns(routineDefense,request.iMaxPath);
    int supportAttempts=0;
    if(!m_defenseCandidatesReady)
    {
        // Assignment changes reservations, not physical units. Test the
        // origin's garrison once per advertiser in this dispatch pass,
        // instead of once for every outstanding destination.
        m_defenseCandidatesReady=true;
        for(size_t i=0;i<m_advertisingUnits.size();++i)
        {
            const advertisingUnit& info=m_advertisingUnits[i];
            if(info.iUnitId<0 || info.iContractedWorkRequest!=-1 || info.iDefensiveValue<=0)continue;
            DefenseDemandPerf::count(DefenseDemandPerf::CANDIDATES);
            if(canReleaseDefender(findUnit(info.iUnitId)))m_defenseCandidates.push_back((int)i);
            else DefenseDemandPerf::count(DefenseDemandPerf::PROTECTED);
        }
    }
    const CvPlot* anchor=target;
    // No route-tier graph in performance: order by distance to the actual
    // destination, without scanning cities for an unavailable logistics hint.
    AssignmentHopSearch proximity(m_eOwner,true,false);
    std::vector<AssignmentHopCandidate> order;
    for(size_t n=0;n<m_defenseCandidates.size();++n)
    {
        const int i=m_defenseCandidates[n];
        const advertisingUnit& info=m_advertisingUnits[i];
        if(info.iUnitId<0 || info.iContractedWorkRequest!=-1 || info.iDefensiveValue<=0 || info.iMinPriority>request.iPriority)continue;
        const CvUnit* unit=findUnit(info.iUnitId);
        // Ground reinforcements are local to this landmass. Hop ordering is
        // only a ranking hint and must not send A* across other continents.
        // A direct city airlift already has its own real legality check and
        // does not perform a ground-path search.
        const CvPlot* origin=unit?unit->plot():NULL;
        if(!origin || (!UnitTravelConnectivity::landUnitCanReach(unit,target)
            && !(target->getPlotCity() && unit->canAirliftAt(origin,target->getX(),target->getY()))))continue;
        if(DefenseDemandPerf::fortRetriesEnabled() && !target->getPlotCity()
            && m_defenseFortRetries.failed(unit,target,MOVE_NO_ENEMY_TERRITORY|MOVE_AVOID_ENEMY_UNITS,request.iMaxPath))
        {DefenseDemandPerf::count(DefenseDemandPerf::FORT_RETRY_SKIPS);continue;}
        if(!unit->meetsUnitSelectionCriteria(&request.criteria) || target->isVisibleEnemyUnit(unit))continue;
        const DefenderProximity::Key key=target->getPlotCity() && unit->canAirliftAt(unit->plot(),target->getX(),target->getY())
            ? DefenderProximity::Key(true,0,0,0)
            : proximity.key(unit->plot(),anchor,m_eOwner,0,stepDistance(unit->getX(),unit->getY(),target->getX(),target->getY()));
        order.push_back(AssignmentHopCandidate((int)i,key));
    }
    proximity.order(order);
    // Closest available logistics/geometry candidate first. Only a real path
    // confirms availability; unknown hops fall through instead of rejecting.
    for(size_t i=0;i<order.size();++i)
    {
        advertisingUnit& info=m_advertisingUnits[order[i].index];const CvUnit* unit=findUnit(info.iUnitId);int turns;
        const int flags=MOVE_NO_ENEMY_TERRITORY|MOVE_AVOID_ENEMY_UNITS;
        // Earlier matches in this same pass can reserve another defender
        // from this origin. Never let the shared pool drain its garrison.
        if(!canReleaseDefender(unit)){DefenseDemandPerf::count(DefenseDemandPerf::PROTECTED);continue;}
        if(!CitySupportSearch::maySearch(routineDefense,supportAttempts))break;
        ++supportAttempts;
        DefenseDemandPerf::count(DefenseDemandPerf::PATHS);
        ReadOnlyUnitPathFilter::Guard reachability(unit,target,flags,failures.landReachability());
        if((target->getPlotCity() && unit->canAirliftAt(unit->plot(),target->getX(),target->getY()))
            || failures.path(unit,target,flags,&turns,maxPath,reachability))
        {DefenseDemandPerf::count(DefenseDemandPerf::FOUND);return &info;}
        DefenseDemandPerf::count(DefenseDemandPerf::FAILED);
        if(DefenseDemandPerf::fortRetriesEnabled() && !target->getPlotCity())m_defenseFortRetries.remember(unit,target,flags,request.iMaxPath);
    }
    return NULL;
}
