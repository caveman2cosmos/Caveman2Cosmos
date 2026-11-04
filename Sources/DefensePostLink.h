#pragma once
#include "UnitTravelConnectivity.h"

// Native performance forts have no routeRev site-parent ledger. Require an
// actual trade-network link to an owned city on connected physical ground.
// Two null plot groups, sea-only trade and geographic proximity are no proof.
namespace DefensePostLink {
template<class Plot,class City,class Owner>
bool connected(const Plot* plot,Owner owner,int map) {
    if(!plot || plot->getOwner()!=owner || plot->getPlotGroup(owner)==NULL)return false;
    foreach_(const City* city,GET_PLAYER(owner).cities())
        if(city->getOwner()==owner && GC.getMap().getType()==map && city->plot()->getPlotCity()==city
            && plot->isConnectedTo(city)
            && UnitTravelConnectivity::land(owner,plot,city->plot()))return true;
    return false;
}
}
