#include "CvGameCoreDLL.h"
#include "ConnectionRequests.h"
#include "CvCityAI.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvUnitAI.h"
#include "CvSelectionGroup.h"
#include "CvPathGenerator.h"
#include "CvGameCoreUtils.h"
#include "CvDLLFAStarIFaceBase.h"
#include "UnitTravelConnectivity.h"
#include "WorkerRouteQueryMemo.h"
#include "NativeRoutePolicy.h"
#include "ReadOnlyRouteConnectivity.h"
#include <map>
#include <set>
#include <algorithm>
#include <cstdlib>

namespace ConnectionRequests {
namespace {
struct Board {
    int turn;unsigned __int64 revision;std::vector<Request> requests;
    Board():turn(-1),revision(0){}
};
struct State {
    unsigned __int64 revision;std::map<std::pair<int,int>,Board> boards;
    State():revision(1){}
};
State& state(){static State s;return s;}
int index(const CvPlot* tile){return GC.getMap().plotNum(tile->getX(),tile->getY());}
const CvCity* parent(const CvPlot* site){return site->getWorkingCity();}
struct Planning {
    int owner;bool available;ReadOnlyRouteConnectivity graph;
    explicit Planning(int player):owner(player),available(GET_PLAYER((PlayerTypes)player).getBestRoute(NULL)!=NO_ROUTE),graph(GC.getMap().numPlots()){}
    bool accepts(int node){return NativeRoutePolicy::accepts(GET_PLAYER((PlayerTypes)owner),GC.getMap().plotByIndex(node));}
    void adjacent(int node,std::vector<int>& result){
        foreach_(const CvPlot* next,GC.getMap().plotByIndex(node)->adjacent())result.push_back(index(next));
    }
    bool needsConnection(const CvPlot* from,const CvPlot* to){return available && from!=to && !graph.connected(*this,index(from),index(to));}
};
void post(int owner,const CvPlot* from,const CvPlot* to,std::vector<Request>& requests,std::set<std::pair<int,int> >& known,Planning& plan){
    if(!from||!to||from==to||from->getOwner()!=owner||to->getTeam()!=GET_PLAYER((PlayerTypes)owner).getTeam()
        ||!UnitTravelConnectivity::land(owner,from,to,true)||from->isWater()||to->isWater())return;
    const int a=index(from),b=index(to);
    const std::pair<int,int> key(std::min(a,b),std::max(a,b));
    if(known.count(key)||!plan.needsConnection(from,to))return;
    known.insert(key);requests.push_back(Request(a,b));trace("POST",owner,-1,requests.back());
}
void postCity(const CvCity* city,std::vector<Request>& requests,std::set<std::pair<int,int> >& known,Planning& plan){
    // The city chooses its nearest unmet same-landmass city connection.
    // Preserve native distance and iteration tie order; no worker pathfinding.
    std::vector<const CvCity*> cities;
    std::vector<std::pair<int,int> > order;
    for(int p=0;p<MAX_PLAYERS;++p)if(GET_PLAYER((PlayerTypes)p).getTeam()==city->getTeam())
        foreach_(const CvCity* other,GET_PLAYER((PlayerTypes)p).cities())
            if(other!=city&&UnitTravelConnectivity::land(city->getOwner(),city->plot(),other->plot(),true)){
                order.push_back(std::make_pair(plotDistance(city->getX(),city->getY(),other->getX(),other->getY()),(int)cities.size()));
                cities.push_back(other);
            }
    std::sort(order.begin(),order.end());
    for(size_t i=0;i<order.size();++i){
        const CvCity* other=cities[order[i].second];
        if(!plan.needsConnection(city->plot(),other->plot()))continue;
        post(city->getOwner(),city->plot(),other->plot(),requests,known,plan);break;
    }
}
void postSite(int owner,const CvPlot* site,std::vector<Request>& requests,std::set<std::pair<int,int> >& known,Planning& plan){
    if(!site||site->getOwner()!=owner||site->getPlotCity()||!site->isActsAsCity())return;
    // A cut road must not erase a fort's identity. Ask through the actual
    // parent, including dormant links; never attach it to a guessed city.
    const CvCity* city=parent(site);
    if(city&&city->getOwner()==owner&&city->plot()->getPlotCity()==city)
        post(owner,city->plot(),site,requests,known,plan);
}
const std::vector<Request>& prepare(int owner){
    State& s=state();Board& b=s.boards[std::make_pair((int)GC.getMap().getType(),owner)];
    if(b.turn==GC.getGame().getGameTurn()&&b.revision==s.revision)return b.requests;
    b.turn=GC.getGame().getGameTurn();b.revision=s.revision;
    std::vector<Request> requests;std::set<std::pair<int,int> > known;
    // No orders or world changes occur while publishing this board. Discard
    // the snapshot before returning to workers; needed() still uses the real
    // native finder when a worker validates a job against the current world.
    Planning plan(owner);
    foreach_(const CvCity* city,GET_PLAYER((PlayerTypes)owner).cities())postCity(city,requests,known,plan);
    const std::vector<int>& sites=GET_PLAYER((PlayerTypes)owner).getDefensivePostPlotIndices();
    for(size_t i=0;i<sites.size();++i)postSite(owner,GC.getMap().plotByIndex(sites[i]),requests,known,plan);
    b.requests.swap(requests);trace("PLAN",owner,-1,Request(-1,(int)b.requests.size()));return b.requests;
}
struct Nearer {
    const CvUnitAI& worker;
    explicit Nearer(const CvUnitAI& unit):worker(unit){}
    int distance(const Request& r)const{
        const CvPlot* tile=GC.getMap().plotByIndex(r.source);
        return stepDistance(worker.getX(),worker.getY(),tile->getX(),tile->getY());
    }
    bool operator()(const Request& a,const Request& b)const{return distance(a)<distance(b);}
};
}
void reset(){state().boards.clear();++state().revision;}
void invalidate(){++state().revision;}
void trace(const char* event,int owner,int unit,const Request& r){
#ifdef C2C_RECALCULATED_BENCHMARK
    static const char* flag=std::getenv("C2C_CIVILIAN_DECISION_TRACE");
    if(flag&&flag[0]=='1')gDLL->logMsg("CivilianDecision.log",CvString::format(
        "CONNECTION_REQUEST turn=%d map=%d player=%d unit=%d source=%d target=%d event=%s",
        GC.getGame().getGameTurn(),GC.getMap().getType(),owner,unit,r.source,r.target,event).c_str());
#endif
}
bool needed(int owner,const CvPlot* source,const CvPlot* target){
    if(!source || !target || source==target || GET_PLAYER((PlayerTypes)owner).getBestRoute(NULL)==NO_ROUTE)return false;
    WorkerRouteQueryMemo::Scope queries(owner,GC.getMap().getType(),GC.getMap().numPlots());
    // Keep performance's routeValid predicate: no logistics tiers or free roads.
    gDLL->getFAStarIFace()->ForceReset(&GC.getRouteFinder());
    return !gDLL->getFAStarIFace()->GeneratePath(&GC.getRouteFinder(),source->getX(),source->getY(),
        target->getX(),target->getY(),false,owner,true);
}
bool active(int owner,const CvPlot* source,const CvPlot* target){
    if(!source||!target||source==target||source->getOwner()!=owner
        ||target->getTeam()!=GET_PLAYER((PlayerTypes)owner).getTeam()
        ||!UnitTravelConnectivity::land(owner,source,target,true)||source->isWater()||target->isWater())return false;
    if(source->getPlotCity()&&target->getPlotCity())return true;
    const CvPlot* site=source->getPlotCity()?target:source;
    const CvPlot* city=site==source?target:source;
    const CvCity* assigned=parent(site);
    return city->getPlotCity()&&site->getOwner()==owner&&site->isActsAsCity()&&assigned
        &&assigned->getOwner()==owner&&assigned->plot()->getPlotCity()==assigned&&assigned->plot()==city;
}
bool canWork(CvUnitAI& worker,const CvPlot* source,const CvPlot* target,int flags){
    CvSelectionGroup* group=worker.getGroup();
    if(!group->generatePath(source,target,flags,true))return false;
    // A global available tier is not proof that this builder has its action,
    // resources or terrain permission. Accept only a concrete corridor on
    // which the group can actually build something. Another worker may still
    // take the same request; this negative never poisons the shared tier cache.
    const CvPath& path=group->getPath();
    for(CvPath::const_iterator i=path.begin();i!=path.end();++i)
        if(group->getBestBuildRoute(i.plot())!=NO_ROUTE)return true;
    trace("NO_BUILD",worker.getOwner(),worker.getID(),Request(index(source),index(target)));
    return false;
}
std::vector<Request> candidates(const CvUnitAI& worker){
    std::vector<Request> result=prepare(worker.getOwner());
    // A worker approaches the nearer endpoint geographically. The native
    // pathfinder validates just the shortlisted job before issuing orders.
    for(size_t i=0;i<result.size();++i){
        const CvPlot* a=GC.getMap().plotByIndex(result[i].source);
        const CvPlot* b=GC.getMap().plotByIndex(result[i].target);
        if(b->getOwner()==worker.getOwner()&&stepDistance(worker.getX(),worker.getY(),b->getX(),b->getY())
            <stepDistance(worker.getX(),worker.getY(),a->getX(),a->getY()))std::swap(result[i].source,result[i].target);
    }
    std::stable_sort(result.begin(),result.end(),Nearer(worker));return result;
}
}
