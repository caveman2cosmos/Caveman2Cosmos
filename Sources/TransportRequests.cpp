#include "CvGameCoreDLL.h"
#include "TransportRequests.h"
#include "AIPerf.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvPlayerAI.h"
#include "CvSelectionGroupAI.h"
#include "CvUnit.h"
#include "CvPlot.h"
#include "FFreeListTrashArray.h"
#include "CvGameCoreUtils.h"
#include "UnitTravelConnectivity.h"
#include "TransportRouting.h"
#include <map>
#include <set>
#include <algorithm>
#include <cstdlib>
#include "CvTransportRouting.inl"

namespace TransportRequests {
namespace {
struct Request {
    int unit, plot, members, role, coast, retryTurn, refusedTurn;
    std::map<int,int> refusedOrigins;
    explicit Request(CvSelectionGroup* group):unit(group->getHeadUnit()->getID()),
        plot(GC.getMap().plotNum(group->getX(),group->getY())),members(group->getNumUnits()),role(group->getHeadUnitAI()),
        coast(-1),retryTurn(-1),refusedTurn(-1){}
    bool same(const Request& other)const{return unit==other.unit&&plot==other.plot&&members==other.members&&role==other.role;}
};
struct Owner {
    bool initialized, refreshAll;
    std::set<int> known, pending;
    std::map<int,Request> requests;
    std::map<int,int> ships; // ship unit ID -> requested land group ID
    Owner():initialized(false),refreshAll(false){}
};
typedef std::pair<int,int> Key;
std::map<Key,Owner>& state(){static std::map<Key,Owner> value;return value;}
Owner& ownerState(int owner){return state()[Key((int)GC.getMap().getType(),owner)];}
void trace(const char* event,int owner,int group,int unit=-1,int ship=-1){
#ifdef C2C_RECALCULATED_BENCHMARK
    static const char* flag=std::getenv("C2C_TRANSPORT_REQUEST_TRACE");
    if(flag&&flag[0]=='1')gDLL->logMsg("TransportRequests.log",CvString::format(
        "TRANSPORT_REQUEST turn=%d map=%d player=%d group=%d unit=%d ship=%d event=%s",
        GC.getGame().getGameTurn(),GC.getMap().getType(),owner,group,unit,ship,event).c_str());
#endif
}
bool eligibleOwner(int owner){return owner>=0&&owner<MAX_PLAYERS&&!GET_PLAYER((PlayerTypes)owner).isNPC();}
void cancel(Owner& s,int owner,int group,bool clearEmbark=true){
    std::map<int,Request>::iterator found=s.requests.find(group);
    const int coast=found==s.requests.end()?-1:found->second.coast;
    if(found!=s.requests.end()){trace("CANCEL",owner,group,found->second.unit);s.requests.erase(found);}
    for(std::map<int,int>::iterator i=s.ships.begin();i!=s.ships.end();)
        if(i->second==group){std::map<int,int>::iterator old=i++;s.ships.erase(old);}else ++i;
    s.known.erase(group);s.pending.erase(group);
    CvSelectionGroup* land=GET_PLAYER((PlayerTypes)owner).getSelectionGroup(group);
    if(clearEmbark&&coast>=0&&land&&land->getHeadUnit()&&land->getDomainType()==DOMAIN_LAND
        &&land->AI_getMissionAIType()==MISSIONAI_PICKUP
        &&land->AI_getMissionAIPlot()==GC.getMap().plotByIndex(coast)) {
        land->clearMissionQueue();land->AI_setMissionAI(NO_MISSIONAI,NULL,NULL);
    }
}
bool embarkLeg(CvSelectionGroup* group,const Request& request){
    if(!group||!group->getHeadUnit()||group->getHeadUnit()->getID()!=request.unit
        ||group->getHeadUnit()->isCargo()||group->getHeadUnitAI()!=request.role
        ||group->getNumUnits()!=request.members||request.coast<0
        ||group->AI_getMissionAIType()!=MISSIONAI_PICKUP
        ||group->AI_getMissionAIPlot()!=GC.getMap().plotByIndex(request.coast))return false;
    if(group->getLengthMissionQueue()==0)return true;
    if(group->getLengthMissionQueue()!=1)return false;
    const MissionData& mission=group->headMissionQueueNode()->m_data;
    const CvPlot* coast=GC.getMap().plotByIndex(request.coast);
    return coast&&((mission.eMissionType==MISSION_MOVE_TO&&mission.iData1==coast->getX()&&mission.iData2==coast->getY())
        ||(mission.eMissionType==MISSION_SKIP&&group->atPlot(coast)));
}
bool matches(CvSelectionGroup* group,const Request& request){
    return group&&group->getHeadUnit()&&group->plot()&&!group->getHeadUnit()->isCargo()
        &&group->getDomainType()==DOMAIN_LAND&&Request(group).same(request);
}
bool refused(const Request& request,const CvUnit* ship){
    if(!ship||request.refusedTurn!=GC.getGame().getGameTurn())return false;
    std::map<int,int>::const_iterator i=request.refusedOrigins.find(ship->getID());
    return i!=request.refusedOrigins.end()&&i->second==GC.getMap().plotNum(ship->getX(),ship->getY());
}
bool validShip(int owner,int ship,const Request& request){
    CvUnit* unit=GET_PLAYER((PlayerTypes)owner).getUnit(ship);
    if(!unit||unit->isDelayedDeath()||unit->isCargo()||!unit->getGroup()||unit->isFull())return false;
    CvSelectionGroup* group=unit->getGroup();
    const CvUnit* target=group->AI_getMissionAIUnit();
    return group->AI_getMissionAIType()==MISSIONAI_PICKUP&&target
        &&target->getOwner()==owner&&target->getID()==request.unit&&!target->isCargo();
}
struct NativeOrder {
    bool operator()(const CvSelectionGroup* a,const CvSelectionGroup* b)const{
        return (a->getID()&FLTA_INDEX_MASK)<(b->getID()&FLTA_INDEX_MASK);
    }
};
}
bool enabled(){
#ifdef C2C_RECALCULATED_BENCHMARK
    static const char* flag=std::getenv("C2C_TRANSPORT_REQUESTS");
    static const bool value=!flag||flag[0]!='0';return value;
#else
    return true;
#endif
}
void reset(){state().clear();}
void invalidateAll(){
    if(!enabled())return;
    for(std::map<Key,Owner>::iterator i=state().begin();i!=state().end();++i)
    {i->second.refreshAll=true;i->second.pending.insert(i->second.known.begin(),i->second.known.end());
     for(std::map<int,Request>::iterator r=i->second.requests.begin();r!=i->second.requests.end();++r)r->second.refusedOrigins.clear();}
}
void dirty(CvSelectionGroup* group){
    if(!enabled()||!group||!eligibleOwner(group->getOwner())||group->getID()==FFreeList::INVALID_INDEX)return;
    Owner& s=ownerState(group->getOwner());
    if(group->getHeadUnit()&&group->getDomainType()==DOMAIN_SEA)return;
    // Land AI publishes a new need through its own native isStranded query.
    // After reconstruction, ordinary order/move invalidations must not turn
    // the broker into another whole-army stranded scan.
    if(s.initialized&&s.requests.find(group->getID())==s.requests.end())return;
    s.known.insert(group->getID());s.pending.insert(group->getID());
}
void forget(CvSelectionGroup* group){
    if(!enabled()||!group||!eligibleOwner(group->getOwner()))return;
    Owner& s=ownerState(group->getOwner());const int id=group->getID();
    cancel(s,group->getOwner(),id,false);s.known.erase(id);s.pending.erase(id);
}
void publish(CvSelectionGroup* group,bool stranded){
    if(!enabled()||!group||!eligibleOwner(group->getOwner()))return;
    const int owner=group->getOwner(),id=group->getID();Owner& s=ownerState(owner);
    if(GET_PLAYER((PlayerTypes)owner).getSelectionGroup(id)!=group)return;
    s.known.insert(id);s.pending.erase(id);
    const CvUnit* head=group->getHeadUnit();
    if(!stranded||!head||head->isDelayedDeath()||head->isCargo()||!group->plot()||group->getDomainType()!=DOMAIN_LAND)
        {cancel(s,owner,id);return;}
    const Request request(group);std::map<int,Request>::iterator found=s.requests.find(id);
    if(found!=s.requests.end()&&found->second.same(request))return;
    if(found!=s.requests.end()&&found->second.unit==request.unit){
        found->second.refusedOrigins.clear();
        found->second.plot=request.plot;found->second.members=request.members;found->second.role=request.role;
        trace("UPDATE",owner,id,request.unit);return;
    }
    cancel(s,owner,id);s.known.insert(id);s.requests.insert(std::make_pair(id,request));trace("POST",owner,id,request.unit);
}
bool waitingOnly(CvSelectionGroup* group){
    if(!enabled()||!group||!eligibleOwner(group->getOwner()))return false;
    Owner& s=ownerState(group->getOwner());std::map<int,Request>::const_iterator i=s.requests.find(group->getID());
    if(i==s.requests.end())return false;
    if(embarkLeg(group,i->second))return true;
    if(!matches(group,i->second)||group->AI_getMissionAIType()!=NO_MISSIONAI)return false;
    return group->getLengthMissionQueue()==0||(group->getLengthMissionQueue()==1
        &&group->headMissionQueueNode()->m_data.eMissionType==MISSION_SKIP);
}
void considerIdle(CvSelectionGroup* group){
    if(!enabled()||!group||!eligibleOwner(group->getOwner())||!GC.getGame().isFinalInitialized()
        ||!group->getHeadUnit()||!group->AI_isControlled()||group->getDomainType()!=DOMAIN_LAND
        ||group->AI_getMissionAIType()!=NO_MISSIONAI||group->getLengthMissionQueue()!=0)return;
    // All land roles can ask when they actually choose to wait, including
    // roles whose main AI never calls isStranded. Assess before inserting
    // SKIP; a real job or a manually issued wait must not become a request.
    group->isStranded();
}
bool advanceLand(CvSelectionGroup* group){
    if(!enabled()||!group||!eligibleOwner(group->getOwner())||!waitingOnly(group))return false;
    const int owner=group->getOwner(),id=group->getID();Owner& s=ownerState(owner);
    if(GET_PLAYER((PlayerTypes)owner).AI_getAnyPlotDanger(group->plot(),1,false)){
        cancel(s,owner,id);return false;
    }
    if(!group->isStranded())return false;
    std::map<int,Request>::iterator found=s.requests.find(id);if(found==s.requests.end())return false;
    Request& request=found->second;
    CvPlot* coast=GC.getMap().plotByIndex(request.coast);
    if(coast&&!TransportRouting::safeCoast(group,coast)){cancel(s,owner,id);return false;}
    if(!coast&&request.retryTurn!=GC.getGame().getGameTurn()){
        request.retryTurn=GC.getGame().getGameTurn();coast=TransportRouting::chooseCoast(group);
        if(coast){request.coast=GC.getMap().plotNum(coast->getX(),coast->getY());trace("COAST",owner,id,request.unit);}
    }
    if(!coast||group->atPlot(coast)){
        group->pushMission(MISSION_SKIP);return true;
    }
    // An existing native move retains its destination and legal route flags.
    if(embarkLeg(group,request)&&group->getLengthMissionQueue()>0)return false;
    const int flags=MOVE_NO_ENEMY_TERRITORY|MOVE_WITH_CAUTION;
    const bool issued=group->pushMissionInternal(MISSION_MOVE_TO,coast->getX(),coast->getY(),flags,
        false,false,MISSIONAI_PICKUP,coast,NULL);
    if(!issued||(!group->atPlot(coast)&&group->getLengthMissionQueue()==0)){cancel(s,owner,id);return false;}
    return true;
}
void prepare(int owner){
    if(!enabled()||!eligibleOwner(owner)||!GC.getGame().isFinalInitialized())return;
    AI_PERF_SCOPE(TRANSPORT_PREPARE);
    Owner& s=ownerState(owner);
    if(!s.initialized){
        s.initialized=true;
        // One reconstruction after load/reset. Later work comes from unit/
        // group invalidations; each transport reads only posted requests.
        foreach_(CvSelectionGroup* group,GET_PLAYER((PlayerTypes)owner).groups()){
            s.known.insert(group->getID());s.pending.insert(group->getID());
        }
        // Pickup missions already store the passenger ID in saves. Rebuild
        // their transient request/claim before validating a spent SKIP wait.
        foreach_(CvSelectionGroup* group,GET_PLAYER((PlayerTypes)owner).groups()){
            const CvPlot* coast=group->AI_getMissionAIPlot();
            if(group->getHeadUnit()&&group->getDomainType()==DOMAIN_LAND
                &&group->AI_getMissionAIType()==MISSIONAI_PICKUP&&!group->AI_getMissionAIUnit()
                &&coast&&TransportRouting::safeCoast(group,coast)){
                publish(group,true);
                std::map<int,Request>::iterator land=s.requests.find(group->getID());
                if(land!=s.requests.end()){land->second.coast=GC.getMap().plotNum(coast->getX(),coast->getY());dirty(group);}
            }
            const CvUnit* target=group->AI_getMissionAIUnit();
            if(group->getHeadUnit()&&!group->getHeadUnit()->isDelayedDeath()&&group->AI_getMissionAIType()==MISSIONAI_PICKUP
                &&target&&target->getOwner()==owner&&!target->isCargo()&&target->getGroup()){
                publish(target->getGroup(),true);
                assign(owner,target->getGroup()->getID(),group->getHeadUnit()->getID());
                dirty(target->getGroup());
            }
        }
        trace("BOOTSTRAP",owner,-1);
    }
    if(s.refreshAll){
        s.refreshAll=false;
        for(std::set<int>::const_iterator i=s.known.begin();i!=s.known.end();++i){
            CvSelectionGroup* group=GET_PLAYER((PlayerTypes)owner).getSelectionGroup(*i);
            if(group)group->invalidateIsStrandedCache();
        }
    }
    while(!s.pending.empty()){
        const int id=*s.pending.begin();s.pending.erase(s.pending.begin());
        CvSelectionGroup* group=GET_PLAYER((PlayerTypes)owner).getSelectionGroup(id);
        if(!group||!group->getHeadUnit()||group->getDomainType()!=DOMAIN_LAND){cancel(s,owner,id);s.known.erase(id);continue;}
        group->isStranded(); // The land group's native assessment publishes/cancels.
    }
}
std::vector<CvSelectionGroup*> candidates(int owner,int ship){
    std::vector<CvSelectionGroup*> result;
    if(!enabled()||!eligibleOwner(owner))return result;
    prepare(owner);Owner& s=ownerState(owner);
    const CvUnit* vessel=GET_PLAYER((PlayerTypes)owner).getUnit(ship);
    for(std::map<int,Request>::iterator i=s.requests.begin();i!=s.requests.end();){
        const int id=i->first;CvSelectionGroup* group=GET_PLAYER((PlayerTypes)owner).getSelectionGroup(id);
        if(!matches(group,i->second)){++i;cancel(s,owner,id);continue;}
        bool assignedElsewhere=false;
        for(std::map<int,int>::iterator claim=s.ships.begin();claim!=s.ships.end();){
            std::map<int,Request>::iterator target=s.requests.find(claim->second);
            if(target==s.requests.end()||!validShip(owner,claim->first,target->second)){
                trace("RELEASE",owner,claim->second,-1,claim->first);std::map<int,int>::iterator old=claim++;s.ships.erase(old);
            }else {if(claim->second==id&&claim->first!=ship)assignedElsewhere=true;++claim;}
        }
        if(!assignedElsewhere&&!refused(i->second,vessel)&&(group->plot()->isCoastalLand()||(vessel&&vessel->canMoveAllTerrain()))
            &&(i->second.coast<0||group->atPlot(GC.getMap().plotByIndex(i->second.coast))))result.push_back(group);
        ++i;
    }
    std::sort(result.begin(),result.end(),NativeOrder());
    if(vessel)std::stable_sort(result.begin(),result.end(),TransportRouting::RequestOrder(vessel));
    return result;
}
bool nearestAvailable(int owner,int group,int ship){
    if(!enabled()||!eligibleOwner(owner))return false;Owner& s=ownerState(owner);
    std::map<int,Request>::iterator found=s.requests.find(group);if(found==s.requests.end())return false;
    std::map<int,int>::const_iterator claim=s.ships.find(ship);
    if(claim!=s.ships.end()&&claim->second==group&&validShip(owner,ship,found->second))return true;
    CvSelectionGroup* land=GET_PLAYER((PlayerTypes)owner).getSelectionGroup(group);
    const CvUnit* vessel=GET_PLAYER((PlayerTypes)owner).getUnit(ship);
    if(!land||!vessel)return false;
    std::set<int> blocked;
    if(found->second.refusedTurn==GC.getGame().getGameTurn())
        for(std::map<int,int>::const_iterator f=found->second.refusedOrigins.begin();f!=found->second.refusedOrigins.end();++f)
            if(refused(found->second,GET_PLAYER((PlayerTypes)owner).getUnit(f->first)))blocked.insert(f->first);
    const bool nearest=TransportRouting::nearestAvailable(land,vessel,land->plot(),blocked);
    if(!nearest)trace("NEARER_SHIP",owner,group,found->second.unit,ship);
    return nearest;
}
void rejected(int owner,int group,int ship,int maxTurns){
    if(!enabled()||!eligibleOwner(owner)||maxTurns!=MAX_INT)return;
    Owner& s=ownerState(owner);std::map<int,Request>::iterator i=s.requests.find(group);
    const CvUnit* vessel=GET_PLAYER((PlayerTypes)owner).getUnit(ship);
    if(i==s.requests.end()||!vessel)return;
    if(i->second.refusedTurn!=GC.getGame().getGameTurn()){
        i->second.refusedOrigins.clear();i->second.refusedTurn=GC.getGame().getGameTurn();
    }
    i->second.refusedOrigins[ship]=GC.getMap().plotNum(vessel->getX(),vessel->getY());
    std::map<int,int>::iterator claim=s.ships.find(ship);
    if(claim!=s.ships.end()&&claim->second==group)s.ships.erase(claim);
    trace("ROUTE_REJECT",owner,group,i->second.unit,ship);
}
void assign(int owner,int group,int ship){
    if(!enabled()||!eligibleOwner(owner))return;Owner& s=ownerState(owner);
    std::map<int,Request>::iterator i=s.requests.find(group);
    if(i==s.requests.end()||!validShip(owner,ship,i->second))return;
    s.ships[ship]=group;trace("ASSIGN",owner,group,i->second.unit,ship);
}
bool awaiting(CvSelectionGroup* group){
    if(!waitingOnly(group)||!group->isStranded())return false;
    const int owner=group->getOwner();Owner& s=ownerState(owner);
    std::map<int,Request>::iterator request=s.requests.find(group->getID());if(request==s.requests.end())return false;
    bool assigned=false;
    for(std::map<int,int>::iterator i=s.ships.begin();i!=s.ships.end();){
        std::map<int,Request>::iterator target=s.requests.find(i->second);
        if(target==s.requests.end()||!validShip(owner,i->first,target->second)){
            trace("RELEASE",owner,i->second,-1,i->first);std::map<int,int>::iterator old=i++;s.ships.erase(old);
        }else {if(i->second==group->getID())assigned=true;++i;}
    }
    if(assigned)trace("WAIT",owner,group->getID(),request->second.unit);
    return assigned;
}
}
