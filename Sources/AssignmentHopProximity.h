#pragma once
#include "DefenderProximity.h"
#include "CvPlot.h"
#include "CvCity.h"
#include <vector>
#include <algorithm>

// Performance has no route-tier graph. Retain production iteration and use
// geometry only for new defender requests; no hint proves unit reachability.
inline const CvCity* assignmentCityAnchor(const CvPlot* plot,PlayerTypes owner)
{
 if(!plot)return NULL;
 const CvCity* city=plot->getPlotCity();
 if(!city)city=plot->getWorkingCity();
 return city&&city->getOwner()==owner?city:NULL;
}
struct AssignmentHopCandidate
{
 int index;DefenderProximity::Key key;
 AssignmentHopCandidate(int i,const DefenderProximity::Key& k):index(i),key(k){}
 bool operator<(const AssignmentHopCandidate& b)const
 {if(key<b.key)return true;if(b.key<key)return false;return index<b.index;}
};
class AssignmentHopSearch
{
public:
 explicit AssignmentHopSearch(PlayerTypes,bool=true,bool=false){}
 bool hasCache()const{return false;}
 DefenderProximity::Key key(const CvPlot* from,const CvPlot* to,PlayerTypes,int fallbackBand,int distance)const
 {return DefenderProximity::Key(from==to,false,distance,fallbackBand);}
 void order(std::vector<AssignmentHopCandidate>& candidates)const
 {std::sort(candidates.begin(),candidates.end());}
 bool maySearch(const AssignmentHopCandidate&)const{return true;}
 void select(const AssignmentHopCandidate&){}
};
