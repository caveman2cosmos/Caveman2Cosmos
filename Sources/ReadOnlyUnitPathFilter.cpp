#include "CvGameCoreDLL.h"
#include "ReadOnlyUnitPathFilter.h"
#include "ReadOnlyLandReachability.h"
#include "CvGameCoreUtils.h"
#include "CvTeamAI.h"
#include "CvUnit.h"
#include "CvSelectionGroup.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "AIPerf.h"
#include <limits.h>

namespace ReadOnlyUnitPathFilter {
namespace {
struct LandGraph
{
    CvMap& map;
    TeamTypes team;
    std::string rights;
    LandGraph(CvMap& m,const CvUnit& unit,int flags)
        :map(m),team(unit.getGroup()->getHeadTeam()),rights(MAX_TEAMS+1,0)
    {
        rights[MAX_TEAMS]=(flags&MOVE_OUR_TERRITORY)?1:0;
        for(int t=0;t<MAX_TEAMS;++t)
        {
            const TeamTypes other=(TeamTypes)t;
            // moveToValid forbids enemy territory. canEnterPlot also forbids
            // closed borders for AI without a ready sneak attack. Ready plans
            // remain open here; ignore plot-specific war checks conservatively.
            rights[t]=(!(flags&MOVE_SAFE_TERRITORY) || other==team)
                && (!(flags&MOVE_OUR_TERRITORY) || other==team)
                && (!(flags&MOVE_NO_ENEMY_TERRITORY) || !atWar(other,team))
                && (unit.canEnterTerritory(other,false)
                || GET_TEAM(unit.getTeam()).AI_isSneakAttackReady(other));
        }
    }
    int size() const{return map.numPlots();}
    bool passable(int index) const
    {
        const CvPlot* p=map.plotByIndex(index);
        if(p->isOwned()? !rights[(int)p->getTeam()] : rights[MAX_TEAMS]!=0)return false;
        // Admit every possible AI land step, including tunnel chains and
        // the optional water-city exception. Ignore terrain, danger and units.
        return !p->isWater() || p->isSeaTunnel()
            || (GC.getLAND_UNITS_CAN_ATTACK_WATER_CITIES()!=0 && p->isCity());
    }
    int neighbor(int index,int direction) const
    {
        const CvPlot* n=map.pathAdjacentPlot(map.plotByIndex(index),direction);
        return n?map.plotNum(n->getX(),n->getY()):-1;
    }
};

}

bool couldConnect(const CvUnit* unit,const CvPlot* target,int flags,ReadOnlyLandReachability& cache)
{
    // These cases can have movement rules outside the optimistic land graph.
    if((flags&MOVE_THROUGH_ENEMY) || !unit || !target || !unit->plot() || unit->getDomainType()!=DOMAIN_LAND
        || unit->isHuman() || unit->canMoveAllTerrain() || unit->isCargo()
        || !unit->getGroup() || unit->getGroup()->getHeadUnit()!=unit)return true;
    CvMap& map=GC.getMap();
    const CvPlot* from=unit->plot();
    if(map.plot(from->getX(),from->getY())!=from
        || map.plot(target->getX(),target->getY())!=target)return true;
    if(!cache.shouldFilterQuery())return true;
    AI_PERF_SCOPE(BROKER_REACHABILITY);
    AI_PERF_COUNT(BROKER_REACHABILITY_CHECKS,1);
    const unsigned int before=cache.builds();
    const LandGraph graph(map,*unit,flags);
    const bool possible=cache.couldConnect(graph,&map,(int)graph.team,
        map.plotNum(from->getX(),from->getY()),map.plotNum(target->getX(),target->getY()),graph.rights);
    AI_PERF_COUNT(BROKER_REACHABILITY_BUILDS,cache.builds()-before);
    if(!possible)AI_PERF_COUNT(BROKER_REACHABILITY_REJECTS,1);
    return possible;
}


bool path(const CvUnit* unit,const CvPlot* target,int flags,int* turns,int maxTurns,
    ReadOnlyLandReachability& cache)
{
    // Short tactical queries rarely repay a whole-map component build. This
    // threshold only bypasses the optimization, never limits AI target range.
    const int MIN_DISTANCE_FOR_ATTACK_FILTER=6;
    if(stepDistance(unit->getX(),unit->getY(),target->getX(),target->getY())
        >=MIN_DISTANCE_FOR_ATTACK_FILTER && !couldConnect(unit,target,flags,cache))
    {if(turns)*turns=INT_MAX;return false;}
    return unit->generatePath(target,flags,true,turns,maxTurns);
}
}
