// Included by DefenderDemand.inl. These wrappers memoize the exact native
// predicate and its arguments, retaining short-circuit evaluation order.
namespace {
int defenseIncomingStrength(const CvUnit* unit,const CvPlot* target)
{
    // Native city strength excludes units without a defensive bonus. It also
    // already counts assigned garrison groups within the city's radius two.
    if(!unit->canDefend() || unit->noDefensiveBonus())return 0;
    const CvCity* city=target->getPlotCity();
    if(city && stepDistance(unit->getX(),unit->getY(),target->getX(),target->getY())<=2
        && unit->getGroup()->AI_isCityGarrison(city))return 0;
    return unit->AI_genericUnitValueTimes100(UNITVALUE_FLAGS_DEFENSIVE);
}
}
void CvContractBroker::cancelDefenseAssignment(int unitId)
{
    std::map<int,std::pair<int,int> >::iterator reserved=m_defenseReservations.find(unitId);
    if(reserved==m_defenseReservations.end())return;
    for(size_t i=0;i<m_workRequests.size();++i)
        if(m_workRequests[i].iWorkRequestId==reserved->second.first)
        {
            workRequest& request=m_workRequests[i];request.bFulfilled=false;request.iDefenseAttemptEpoch=-1;
            if(reserved->second.second<0)++request.iDefenseCount;
            else request.iRequiredStrengthTimes100+=reserved->second.second;
            break;
        }
    m_pendingDefenders.erase(unitId);m_defenseReservations.erase(reserved);
}
bool CvContractBroker::defenseCacheLookup(const CvPlot* target,int kind,int extra,int any,int& value) const
{
    if(!DefenseDemandPerf::cacheEnabled() || DefenseNeedEvents::suppressed())return false;
    m_defenseNeedCache.synchronize(DefenseNeedEvents::revision(),GC.getGame().getGameTurn(),GC.getMap().getType());
    if(!m_defenseNeedCache.lookup(target->getX(),target->getY(),kind,extra,any,value))
    {DefenseDemandPerf::count(DefenseDemandPerf::CACHE_MISSES);return false;}
    DefenseDemandPerf::count(DefenseDemandPerf::CACHE_HITS);return true;
}
void CvContractBroker::defenseCacheRemember(const CvPlot* target,int kind,int extra,int any,int value) const
{
    if(!DefenseDemandPerf::cacheEnabled() || DefenseNeedEvents::suppressed())return;
    m_defenseNeedCache.synchronize(DefenseNeedEvents::revision(),GC.getGame().getGameTurn(),GC.getMap().getType());
    m_defenseNeedCache.remember(target->getX(),target->getY(),kind,extra,any,value);
}
int CvContractBroker::defenseStationed(const CvPlot* target) const
{
    int value;
    if(defenseCacheLookup(target,0,0,0,value))
    {
        if(DefenseDemandPerf::cacheAudit())
        {DefenseDemandPerf::count(DefenseDemandPerf::CACHE_AUDITS);const int actual=target->plotCount(PUF_isCityAIType,-1,-1,NULL,m_eOwner);if(value!=actual)DefenseDemandPerf::mismatch(m_eOwner,target->getX(),target->getY(),0,0,0,value,actual);}
        return value;
    }
    value=target->plotCount(PUF_isCityAIType,-1,-1,NULL,m_eOwner);
    defenseCacheRemember(target,0,0,0,value);return value;
}
bool CvContractBroker::defenseCityDefended(CvCity* city,int extra,bool any) const
{
    if(DefenseDemandPerf::cacheFieldsEnabled())
    {
        // AI_isDefended is exactly garrison strength + extra >= required.
        // Cache its native inputs so every pending-departure offset can use
        // one physical garrison scan, rather than caching each Boolean offset.
        CvCityAI* ai=static_cast<CvCityAI*>(city);int strength,required;
        if(!defenseCacheLookup(city->plot(),4,0,any?1:0,strength))
        {
            strength=ai->getGarrisonStrength(any);
            DefenseDemandPerf::count(DefenseDemandPerf::NATIVE_STRENGTH_READS);
            defenseCacheRemember(city->plot(),4,0,any?1:0,strength);
        }
        if(!defenseCacheLookup(city->plot(),5,0,0,required))
        {
            required=ai->AI_neededDefenseStrength();
            DefenseDemandPerf::count(DefenseDemandPerf::NATIVE_REQUIREMENT_READS);
            defenseCacheRemember(city->plot(),5,0,0,required);
        }
        if(DefenseDemandPerf::cacheAudit())
        {
            const int actualStrength=ai->getGarrisonStrength(any),actualRequired=ai->AI_neededDefenseStrength();
            DefenseDemandPerf::count(DefenseDemandPerf::CACHE_AUDITS,2);
            if(strength!=actualStrength)DefenseDemandPerf::mismatch(m_eOwner,city->getX(),city->getY(),4,0,any?1:0,strength,actualStrength);
            if(required!=actualRequired)DefenseDemandPerf::mismatch(m_eOwner,city->getX(),city->getY(),5,0,0,required,actualRequired);
        }
        return strength+extra>=required;
    }
    int value;
    if(defenseCacheLookup(city->plot(),1,extra,any?1:0,value))
    {
        if(DefenseDemandPerf::cacheAudit())
        {DefenseDemandPerf::count(DefenseDemandPerf::CACHE_AUDITS);const int actual=city->AI_isDefended(extra,any)?1:0;if(value!=actual)DefenseDemandPerf::mismatch(m_eOwner,city->getX(),city->getY(),1,extra,any?1:0,value,actual);}
        return value!=0;
    }
    value=city->AI_isDefended(extra,any)?1:0;
    DefenseDemandPerf::count(DefenseDemandPerf::NATIVE_STRENGTH_READS);
    DefenseDemandPerf::count(DefenseDemandPerf::NATIVE_REQUIREMENT_READS);
    defenseCacheRemember(city->plot(),1,extra,any?1:0,value);return value!=0;
}
bool CvContractBroker::defenseCityHappy(const CvCity* city,int extra) const
{
    int value;
    if(defenseCacheLookup(city->plot(),2,extra,0,value))
    {
        if(DefenseDemandPerf::cacheAudit())
        {DefenseDemandPerf::count(DefenseDemandPerf::CACHE_AUDITS);const int actual=city->AI_isAdequateHappinessMilitary(extra)?1:0;if(value!=actual)DefenseDemandPerf::mismatch(m_eOwner,city->getX(),city->getY(),2,extra,0,value,actual);}
        return value!=0;
    }
    value=city->AI_isAdequateHappinessMilitary(extra)?1:0;
    defenseCacheRemember(city->plot(),2,extra,0,value);return value!=0;
}
void CvContractBroker::noteDefenseGroupChanged(const CvSelectionGroup* group)
{
    if(!DefenseDemandPerf::cacheEnabled() || DefenseNeedEvents::suppressed() || !group)return;
    const MissionAITypes mission=group->AI_getMissionAIType();
    const bool relevant=(mission==MISSIONAI_GUARD_CITY || mission==MISSIONAI_GUARD_BONUS || mission==MISSIONAI_CONTRACT)
        && group->AI_getMissionAIPlot()!=NULL;
    m_defenseCommitments.changed(group->getID(),!DefenseDemandPerf::cacheFieldsEnabled() || relevant);
    // Mission metadata changes incoming promises, not physical garrison facts.
    // Size/role/garrison/occupancy/value changes have their own notifications.
    if(!DefenseDemandPerf::cacheFieldsEnabled())DefenseNeedEvents::changed();
}
int CvContractBroker::defensePostRequirement(const CvUnit* unit,const CvPlot* target) const
{
    int value;
    if(defenseCacheLookup(target,3,unit->getID(),unit->getGameTurnCreated(),value))
    {
        if(DefenseDemandPerf::cacheAudit())
        {DefenseDemandPerf::count(DefenseDemandPerf::CACHE_AUDITS);const int actual=static_cast<const CvUnitAI*>(unit)->AI_defenderDemandForPlot(target);if(value!=actual)DefenseDemandPerf::mismatch(m_eOwner,target->getX(),target->getY(),3,unit->getID(),unit->getGameTurnCreated(),value,actual);}
        return value;
    }
    value=static_cast<const CvUnitAI*>(unit)->AI_defenderDemandForPlot(target);
    defenseCacheRemember(target,3,unit->getID(),unit->getGameTurnCreated(),value);return value;
}
void CvContractBroker::refreshDefenseCommitments()
{
    if(!DefenseDemandPerf::cacheEnabled())return;
    const int turn=GC.getGame().getGameTurn(),map=GC.getMap().getType();
    // Rebuild once at an owner-turn boundary as a conservative fallback for
    // strategic/map/load changes. Within a turn only changed groups are read.
    if(!m_defenseCommitments.ready || m_defenseCommitments.map!=map)
    {
        m_defenseCommitments.reset();m_defenseCommitments.map=map;m_defenseCommitments.turn=turn;
        m_defenseCommitments.ready=true;
        foreach_(const CvSelectionGroup* group,GET_PLAYER(m_eOwner).groups())m_defenseCommitments.changed(group->getID());
    }
    else if(m_defenseCommitments.turn!=turn)
    {m_defenseCommitments.turn=turn;m_defenseCommitments.dirtyCommitted();}
    if(m_defenseCommitments.revision!=DefenseNeedEvents::revision())m_defenseCommitments.dirtyCommitted();
    for(std::set<int>::const_iterator i=m_defenseCommitments.dirty.begin();i!=m_defenseCommitments.dirty.end();++i)
    {
        const CvSelectionGroup* group=GET_PLAYER(m_eOwner).getSelectionGroup(*i);
        const CvPlot* target=group?group->AI_getMissionAIPlot():NULL;
        int count=0,strength=0,happy=0;
        if(target && group->getNumUnits()>0
            && (group->AI_getMissionAIType()==MISSIONAI_GUARD_CITY || group->AI_getMissionAIType()==MISSIONAI_GUARD_BONUS || group->AI_getMissionAIType()==MISSIONAI_CONTRACT)
            && group->getHeadUnit()->AI_isCityAIType())
            foreach_(const CvUnit* unit,group->units())
                if(unit->AI_isCityAIType() && !unit->isTempUnit() && !unit->isDelayedDeath() && !unit->atPlot(target))
                {++count;strength+=defenseIncomingStrength(unit,target);if(unit->isMilitaryHappiness())++happy;}
        m_defenseCommitments.replace(*i,target?target->getX():0,target?target->getY():0,count,strength,happy);
        DefenseDemandPerf::count(DefenseDemandPerf::COMMITMENT_UPDATES);
    }
    m_defenseCommitments.dirty.clear();
    m_defenseCommitments.revision=DefenseNeedEvents::revision();
    if(DefenseDemandPerf::cacheAudit() && (++m_defenseCommitments.auditReads&127)==1)
    {
        DefenseCommitments fresh;
        foreach_(const CvSelectionGroup* group,GET_PLAYER(m_eOwner).groups())
        {
            const CvPlot* target=group->AI_getMissionAIPlot();int count=0,strength=0,happy=0;
            if(target && group->getNumUnits()>0
                && (group->AI_getMissionAIType()==MISSIONAI_GUARD_CITY || group->AI_getMissionAIType()==MISSIONAI_GUARD_BONUS || group->AI_getMissionAIType()==MISSIONAI_CONTRACT)
                && group->getHeadUnit()->AI_isCityAIType())
                foreach_(const CvUnit* unit,group->units())
                    if(unit->AI_isCityAIType() && !unit->isTempUnit() && !unit->isDelayedDeath() && !unit->atPlot(target))
                    {++count;strength+=defenseIncomingStrength(unit,target);if(unit->isMilitaryHappiness())++happy;}
            fresh.replace(group->getID(),target?target->getX():0,target?target->getY():0,count,strength,happy);
        }
        DefenseDemandPerf::count(DefenseDemandPerf::CACHE_AUDITS);
        if(!m_defenseCommitments.sameContents(fresh))DefenseDemandPerf::mismatch(m_eOwner,-1,-1,4,0,0,-1,-1);
    }
}
