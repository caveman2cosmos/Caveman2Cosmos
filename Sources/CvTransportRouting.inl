// Native routing queries stay separate from the transient request lifecycle.
namespace TransportRouting {
namespace {
bool available(const CvUnit* ship) {
    if(!ship||ship->isDelayedDeath()||ship->isCargo()||ship->getDomainType()!=DOMAIN_SEA)return false;
    CvSelectionGroup* group=ship->getGroup();
    return group&&group->getHeadUnit()==ship&&group->AI_isControlled()&&!group->hasCargo()
        &&group->getActivityType()==ACTIVITY_AWAKE&&!group->isBusy()&&group->canAllMove()&&group->AI_getMissionAIType()==NO_MISSIONAI
        &&group->getLengthMissionQueue()==0;
}
bool fits(const CvUnit* ship,const CvSelectionGroup* passengers) {
    const CvUnit* head=passengers->getHeadUnit();
    if(!head||!ship->getGroup())return false;
    const int required=GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS)
        ?passengers->getNumUnitCargoVolumeTotal():1;
    return ship->getGroup()->getCargoSpaceAvailable((SpecialUnitTypes)head->getSpecialUnitType(),
        (DomainTypes)head->getDomainType())>=required;
}
int distance(const CvUnit* ship,const CvPlot* coast) {
    return std::max(0,stepDistance(ship->getX(),ship->getY(),coast->getX(),coast->getY())-1);
}
int speed(const CvUnit* ship){return std::max(1,ship->getGroup()->baseMoves());}
bool hasBoat(CvSelectionGroup* group,const CvPlot* coast) {
    foreach_(const CvUnit* ship,GET_PLAYER(group->getOwner()).getUnitsByAIRole(UNITAI_SETTLER_SEA))
        if(available(ship)&&fits(ship,group)&&UnitTravelConnectivity::seaUnitCanReachPickup(ship,coast))return true;
    return false;
}
struct Coast {
    CvPlot* plot;bool boat;int distance,index;
    Coast(CvSelectionGroup* group,CvPlot* tile):plot(tile),boat(hasBoat(group,tile)),
        distance(stepDistance(group->getX(),group->getY(),tile->getX(),tile->getY())),
        index(GC.getMap().plotNum(tile->getX(),tile->getY())){}
};
struct CoastOrder {
    bool operator()(const Coast& a,const Coast& b)const {
        if(a.boat!=b.boat)return a.boat>b.boat;
        if(a.distance!=b.distance)return a.distance<b.distance;
        return a.index<b.index;
    }
};
}
bool safeCoast(CvSelectionGroup* group,const CvPlot* tile) {
    const CvUnit* head=group?group->getHeadUnit():NULL;
    return head&&tile&&tile->isCoastalLand()&&!tile->isVisibleEnemyUnit(head)
        &&(!tile->isOwned()||!head->isEnemy(tile->getTeam(),tile))
        &&UnitTravelConnectivity::landUnitCanReach(head,tile)
        &&!GET_PLAYER(group->getOwner()).AI_getAnyPlotDanger(tile,1,false);
}
CvPlot* chooseCoast(CvSelectionGroup* group) {
    AI_PERF_SCOPE(TRANSPORT_COAST);
    std::vector<Coast> choices;
    foreach_(CvPlot& tile,GC.getMap().plots())
        if(safeCoast(group,&tile))choices.push_back(Coast(group,&tile));
    std::sort(choices.begin(),choices.end(),CoastOrder());
    // Geographic ordering is deliberate: validate the first legal coast,
    // rather than running paths to compare every possible rendezvous.
    foreach_(const Coast& choice,choices) {
        int turns=0;
        if(group->atPlot(choice.plot)||group->generatePath(group->plot(),choice.plot,
            MOVE_NO_ENEMY_TERRITORY|MOVE_WITH_CAUTION,true,&turns,MAX_INT))return choice.plot;
    }
    return NULL;
}
bool nearestAvailable(CvSelectionGroup* passengers,const CvUnit* current,
    const CvPlot* coast,const std::set<int>& rejected) {
    AI_PERF_SCOPE(TRANSPORT_MATCH);
    const int ourDistance=distance(current,coast),ourSpeed=speed(current);
    foreach_(const CvUnit* ship,GET_PLAYER(passengers->getOwner()).getUnitsByAIRole(UNITAI_SETTLER_SEA)) {
        if(ship==current||rejected.count(ship->getID())||!available(ship)||!fits(ship,passengers)
            ||!UnitTravelConnectivity::seaUnitCanReachPickup(ship,coast))continue;
        const int otherDistance=distance(ship,coast);
        const __int64 otherTime=(__int64)otherDistance*ourSpeed,ourTime=(__int64)ourDistance*speed(ship);
        if(otherTime<ourTime||(otherTime==ourTime
            &&(otherDistance<ourDistance||(otherDistance==ourDistance
                &&(ship->getID()&FLTA_INDEX_MASK)<(current->getID()&FLTA_INDEX_MASK)))))return false;
    }
    return true;
}
bool RequestOrder::operator()(CvSelectionGroup* a,CvSelectionGroup* b)const {
    const bool sm=GC.getGame().isOption(GAMEOPTION_COMBAT_SIZE_MATTERS);
    const int countA=sm?a->getNumUnitCargoVolumeTotal():a->getNumUnits();
    const int countB=sm?b->getNumUnitCargoVolumeTotal():b->getNumUnits();
    const int turnsA=distance(ship,a->plot())/speed(ship),turnsB=distance(ship,b->plot())/speed(ship);
    return (__int64)countA*(turnsB+1)>(__int64)countB*(turnsA+1);
}
bool EndpointOrder::operator()(const CvPlot* a,const CvPlot* b)const {
    return stepDistance(ship->getX(),ship->getY(),a->getX(),a->getY())
        <stepDistance(ship->getX(),ship->getY(),b->getX(),b->getY());
}
}
