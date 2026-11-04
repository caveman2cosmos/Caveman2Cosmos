#include "CvGameCoreDLL.h"
#include "UnitTravelConnectivity.h"
#include "TransportRequests.h"
#include "ConnectionRequests.h"
#include "TravelComponentGraph.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include "CvInfos.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvImprovementInfo.h"
#include "CvUnit.h"
#include "CvSelectionGroup.h"
#include <set>

namespace UnitTravelConnectivity {
namespace {
    struct Layer {unsigned __int64 revision;bool dirty,potential;std::set<int> dependencies;TravelComponentGraph graph;Layer():revision(0),dirty(true),potential(false){}};
    struct Entry {Layer layers[4];};
    struct State {unsigned __int64 revision;std::map<std::pair<int,int>,Entry> players;std::map<std::pair<int,int>,std::set<Layer*> > watching;State():revision(1){}};
    State& state(){static State s;return s;}
    struct Geography {
        CvMap& map;TeamTypes team;
        Geography(CvMap&m,int owner):map(m),team(GET_PLAYER((PlayerTypes)owner).getTeam()){}
        int size()const{return map.numPlots();}
        int area(int i)const{return map.plotByIndex(i)->getArea();}
        bool base(int i,int domain)const{return map.plotByIndex(i)->isWater()==(domain==1);}
        bool access(const CvPlot* p,bool enemy)const{
            const TeamTypes other=p->getTeam();
            return other==NO_TEAM || other==team || GET_TEAM(team).isOpenBorders(other)
                || GET_TEAM(other).isVassal(team) || (enemy && GET_TEAM(team).isAtWar(other));
        }
        bool connector(int i,int domain,bool potential)const{
            const CvPlot* p=map.plotByIndex(i);
            if(domain==0)return p->isWater() && p->isSeaTunnel() && (potential || access(p,false));
            if(p->isWater())return false;
            // Explicit sea passages also work inland. Ordinary friendly city
            // gates retain the native coastal restriction.
            if(p->isCanMoveSeaUnits())return potential || access(p,true);
            return p->isCity(true) && p->isCoastalLand() && (potential || access(p,false));
        }
        bool possibleConnector(int i,int domain)const{return connector(i,domain,true);}
        int neighbor(int i,int d)const{
            const CvPlot* p=map.plotByIndex(i);
            const CvPlot* n=plotDirection(p->getX(),p->getY(),(DirectionTypes)d);
            return n?map.plotNum(n->getX(),n->getY()):-1;
        }
    };
    bool query(int player,const CvPlot* from,const CvPlot* to,int domain,bool potential,bool adjacent){
        if(!from || !to || player<0 || player>=MAX_PLAYERS)return true;
        CvMap& map=GC.getMap();State&s=state();
        Layer& layer=s.players[std::make_pair(player,(int)map.getType())].layers[domain+(potential?2:0)];
        if(layer.revision!=s.revision || layer.dirty){
            const int mapId=(int)map.getType();
            for(std::set<int>::const_iterator i=layer.dependencies.begin();i!=layer.dependencies.end();++i)s.watching[std::make_pair(mapId,*i)].erase(&layer);
            layer.dependencies.clear();Geography world(map,player);layer.graph.build(world,domain,potential);
            for(int i=0;i<world.size();++i)if(world.possibleConnector(i,domain)){
                layer.dependencies.insert(i);s.watching[std::make_pair(mapId,i)].insert(&layer);
            }
            layer.potential=potential;layer.revision=s.revision;layer.dirty=false;
        }
        const int a=map.plotNum(from->getX(),from->getY()),b=map.plotNum(to->getX(),to->getY());
        // Special units can already stand outside this generic movement graph.
        // Unknown origins must fall through to their real movement rules.
        if(!layer.graph.active(a))return true;
        if(layer.graph.connected(a,b))return true;
        if(adjacent){
            foreach_(const CvPlot* n,to->adjacent())
                if(layer.graph.connected(a,map.plotNum(n->getX(),n->getY())))return true;
        }
        return false;
    }
}
void invalidate(){ConnectionRequests::invalidate();++state().revision;TransportRequests::invalidateAll();}
void reset(){ConnectionRequests::reset();State&s=state();s.watching.clear();s.players.clear();++s.revision;}
void invalidatePlot(const CvPlot* plot,int newKinds,bool accessOnly){
    if(!plot)return;ConnectionRequests::invalidate();TransportRequests::invalidateAll();State&s=state();const int mapId=(int)GC.getMap().getType(),id=GC.getMap().plotNum(plot->getX(),plot->getY());
    std::map<std::pair<int,int>,std::set<Layer*> >::iterator watched=s.watching.find(std::make_pair(mapId,id));
    if(watched!=s.watching.end())for(std::set<Layer*>::iterator i=watched->second.begin();i!=watched->second.end();++i)
        if(!accessOnly || !(*i)->potential)(*i)->dirty=true;
    // A newly created connector was not a dependency of the previous graph.
    // Announce it explicitly from its construction event; no polling is used.
    if(newKinds)for(std::map<std::pair<int,int>,Entry>::iterator e=s.players.begin();e!=s.players.end();++e)if(e->first.second==mapId)
        for(int k=0;k<4;++k)if(newKinds&(k%2==0?LAND:SEA))e->second.layers[k].dirty=true;
}
bool relevantImprovement(int type){return type!=NO_IMPROVEMENT && (GC.getImprovementInfo((ImprovementTypes)type).isActsAsCity() || GC.getImprovementInfo((ImprovementTypes)type).isCanMoveSeaUnits());}
bool relevant(const CvPlot* p){return p && (p->isCity(true) || p->isSeaTunnel() || (!p->isWater() && p->isCanMoveSeaUnits()));}
bool relevantRoute(int type){return type!=NO_ROUTE && GC.getRouteInfo((RouteTypes)type).isSeaTunnel();}
bool land(int player,const CvPlot* from,const CvPlot* to,bool potential){return query(player,from,to,0,potential,false);}
bool sea(int player,const CvPlot* from,const CvPlot* to,bool potential){return query(player,from,to,1,potential,false);}
bool landOrAdjacent(int player,const CvPlot* from,const CvPlot* to,bool potential){return query(player,from,to,0,potential,true);}
bool landUnitCanReach(const CvUnit* unit,const CvPlot* target,bool potential){
    if(!unit || !target || unit->getDomainType()!=DOMAIN_LAND
        || (unit->getGroup()?unit->getGroup()->canMoveAllTerrain():unit->canMoveAllTerrain()))return true;
    const bool physical=potential || unit->isRivalTerritory();
    if(target->isWater() && target->isCity() && !target->isSeaTunnel())
        return GC.getLAND_UNITS_CAN_ATTACK_WATER_CITIES()>0 && landOrAdjacent(unit->getOwner(),unit->plot(),target,physical);
    return land(unit->getOwner(),unit->plot(),target,physical);
}
bool seaUnitCanReach(const CvUnit* unit,const CvPlot* target,bool potential){
    return !unit || !target || unit->getDomainType()!=DOMAIN_SEA
        || (unit->getGroup()?unit->getGroup()->canMoveAllTerrain():unit->canMoveAllTerrain())
        || sea(unit->getOwner(),unit->plot(),target,potential || unit->isRivalTerritory());
}
bool seaUnitCanReachPickup(const CvUnit* unit,const CvPlot* target){
    // The reachable-set search can include a coastal endpoint or pick up
    // from one of its neighbors. Reject only disconnected physical water
    // components; access, enemies, range and real entry rules remain native.
    if(seaUnitCanReach(unit,target,true))return true;
    foreach_(const CvPlot* neighbor,target->adjacent())
        if(seaUnitCanReach(unit,neighbor,true))return true;
    return false;
}
}
