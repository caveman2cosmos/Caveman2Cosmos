#include "CvGameCoreDLL.h"
#include "RailRoadRevolution.h"
#include "RailNetworkPlan.h"
#include "CvGameAI.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvImprovementInfo.h"

namespace
{
    std::vector<BuildTypes> routeBuildIndex;
    std::vector<RouteTypes> roadByTeam;
    struct RoadObsolescence { TechTypes authored, effective; RoadObsolescence():authored((TechTypes)-2),effective(NO_TECH){} };
    std::vector<RoadObsolescence> roadObsolescence;
    unsigned long roadQueries=0,roadHits=0;
    RailNetworkPlan plan;
    int planPlayer=-1,planMap=-1,planTurn=-1;
    unsigned long planBuilds=0,planQueries=0,planUseful=0;
    double planSeconds=0;

    struct RoadGrid
    {
        CvMap& map;std::vector<unsigned char> validPlots,endpoints,terminals;
        RoadGrid(PlayerTypes owner):map(GC.getMap()),validPlots(map.numPlots(),0),endpoints(map.numPlots(),0),terminals(map.numPlots(),0)
        {
            const TeamTypes team=GET_PLAYER(owner).getTeam();
            const int cottageOption=GC.getInfoTypeForString("GAMEOPTION_MAP_SUPER_COTTAGES",true);
            const bool cottages=cottageOption>=0&&GC.getGame().isOption((GameOptionTypes)cottageOption);
            for(int p=0;p<map.numPlots();++p)
            {
                const CvPlot* plot=map.plotByIndex(p);
                if(plot->getOwner()!=owner||plot->isWater()||!plot->isRevealed(team,false)||!plot->isRoute())continue;
                validPlots[p]=1;
                bool terminal=plot->isCity();
                if(!terminal&&cottages&&plot->getImprovementType()!=NO_IMPROVEMENT)
                {
                    const char* name=GC.getImprovementInfo(plot->getImprovementType()).getType();
                    terminal=!strcmp(name,"IMPROVEMENT_COTTAGE")||!strcmp(name,"IMPROVEMENT_HAMLET")||!strcmp(name,"IMPROVEMENT_VILLAGE")||!strcmp(name,"IMPROVEMENT_TOWN")||!strcmp(name,"IMPROVEMENT_SUBURBS");
                }
                terminals[p]=terminal?1:0;
                endpoints[p]=terminal||plot->getNonObsoleteBonusType(team)!=NO_BONUS;
            }
        }
        int size()const{return map.numPlots();}
        bool valid(int p)const{return validPlots[p]!=0;}
        bool endpoint(int p)const{return endpoints[p]!=0;}
        bool terminal(int p)const{return terminals[p]!=0;}
        int neighbor(int p,int d)const
        {
            const CvPlot* from=map.plotByIndex(p);
            const CvPlot* to=plotDirection(from->getX(),from->getY(),(DirectionTypes)d);
            return to ? map.plotNum(to->getX(),to->getY()) : -1;
        }
    };
}

bool RailRoadRevolution::enabled()
{
    // Do not query game options while XML infos/option arrays are being built.
    return GC.getGame().isFinalInitialized() && GC.getGame().isOption(GAMEOPTION_MAP_RAIL_ROAD_REVOLUTION);
}

const std::vector<BuildTypes>& RailRoadRevolution::routeBuilds()
{
    if(routeBuildIndex.empty())for(int i=0;i<GC.getNumBuildInfos();++i)
        if(GC.getBuildInfo((BuildTypes)i).getRoute()!=NO_ROUTE)routeBuildIndex.push_back((BuildTypes)i);
    return routeBuildIndex;
}

RouteTypes RailRoadRevolution::bestRoad(TeamTypes team)
{
    ++roadQueries;
    const int slot=team+1;
    if(roadByTeam.empty())roadByTeam.assign(MAX_TEAMS+1,(RouteTypes)-2);
    if(slot<0||slot>MAX_TEAMS)return NO_ROUTE;
    if(roadByTeam[slot]!=(RouteTypes)-2){++roadHits;return roadByTeam[slot];}
    RouteTypes best=NO_ROUTE;int value=-1;
    const std::vector<BuildTypes>& builds=routeBuilds();
    for(size_t i=0;i<builds.size();++i)
    {
        const CvBuildInfo& build=GC.getBuildInfo(builds[i]);const RouteTypes route=(RouteTypes)build.getRoute();
        const CvRouteInfo& info=GC.getRouteInfo(route);
        if(info.isRailInfrastructure()||info.isSeaTunnel()||info.getValue()<=value)continue;
        if(team!=NO_TEAM&&build.getTechPrereq()!=NO_TECH&&!GET_TEAM(team).isHasTech(build.getTechPrereq()))continue;
        // Unowned legacy rail receives Paved Road, never free advanced Highway.
        if(team==NO_TEAM&&strcmp(info.getType(),"ROUTE_PAVED_ROAD"))continue;
        best=route;value=info.getValue();
    }
    roadByTeam[slot]=best;return best;
}

TechTypes RailRoadRevolution::effectiveRoadObsoleteTech(RouteTypes route, TechTypes authoredTech)
{
    if(!enabled() || route==NO_ROUTE || authoredTech==NO_TECH)return authoredTech;
    const CvRouteInfo& info=GC.getRouteInfo(route);
    if(info.isRailInfrastructure() || info.isSeaTunnel())return authoredTech;
    if(roadObsolescence.empty())roadObsolescence.resize(GC.getNumRouteInfos());
    RoadObsolescence& cached=roadObsolescence[route];
    if(cached.authored==authoredTech)return cached.effective;
    cached.authored=authoredTech;cached.effective=NO_TECH;
    // Rail discoveries cannot retire the independent road layer. Retain the
    // authored obsolescence only if that technology unlocks a better road.
    const std::vector<BuildTypes>& builds=routeBuilds();
    for(size_t i=0;i<builds.size();++i)
    {
        const CvBuildInfo& build=GC.getBuildInfo(builds[i]);
        if(build.getTechPrereq()!=authoredTech)continue;
        const CvRouteInfo& replacement=GC.getRouteInfo((RouteTypes)build.getRoute());
        if(!replacement.isRailInfrastructure() && !replacement.isSeaTunnel() && replacement.getValue()>info.getValue())
        {cached.effective=authoredTech;break;}
    }
    return cached.effective;
}

void RailRoadRevolution::resetCaches()
{
    roadByTeam.clear();roadObsolescence.clear();routeBuildIndex.clear();planPlayer=planMap=planTurn=-1;plan.useful.clear();
}

bool RailRoadRevolution::usefulRailPlot(const CvPlot& plot,PlayerTypes owner)
{
    if(!enabled())return true;
    if(owner==NO_PLAYER||plot.getOwner()!=owner)return false;
    ++planQueries;
    if(planPlayer!=owner||planMap!=GC.getGame().getCurrentMap()||planTurn!=GC.getGame().getGameTurn())
    {
        LARGE_INTEGER start,end,frequency;QueryPerformanceCounter(&start);QueryPerformanceFrequency(&frequency);
        RoadGrid grid(owner);plan.build(grid);
        QueryPerformanceCounter(&end);planSeconds+=double(end.QuadPart-start.QuadPart)/frequency.QuadPart;
        ++planBuilds;planPlayer=owner;planMap=GC.getGame().getCurrentMap();planTurn=GC.getGame().getGameTurn();
    }
    const int p=GC.getMap().plotNum(plot.getX(),plot.getY());
    const bool useful=p>=0&&p<(int)plan.useful.size()&&plan.useful[p]!=0;
    if(useful)++planUseful;return useful;
}

void RailRoadRevolution::logTurn()
{
    if(!enabled())return;
    gDLL->logMsg("RailRoadRevolution.log",CvString::format("RAIL_PLAN turn=%d builds=%lu queries=%lu useful=%lu seconds=%.6f expanded=%lu edges=%lu retainedBytes=%lu roadQueries=%lu roadHits=%lu",GC.getGame().getGameTurn(),planBuilds,planQueries,planUseful,planSeconds,plan.expanded,plan.edges,(unsigned long)plan.useful.capacity(),roadQueries,roadHits).c_str());
    planBuilds=planQueries=planUseful=roadQueries=roadHits=0;planSeconds=0;
}

void RailRoadRevolution::migrateLegacyRoutes()
{
    if(!enabled())return;
    for(int player=0;player<MAX_PLAYERS;++player)GET_PLAYER((PlayerTypes)player).invalidateRouteChoiceCache();
    const MapTypes previous=GC.getGame().getCurrentMap();
    for(int m=0;m<NUM_MAPS;++m)
    {
        CvMap& map=GC.getMapByIndex((MapTypes)m);if(map.numPlots()<=0)continue;
        GC.getGame().setCurrentMap((MapTypes)m);
        for(int p=0;p<map.numPlots();++p)
        {
            CvPlot* plot=map.plotByIndex(p);plot->migrateRailRoadRevolution();
            if(plot->isCity())plot->updateCityRoute(false);
        }
    }
    GC.getGame().setCurrentMap(previous);CvPlot::flushMovementCostCache();
}
