#pragma once
#include "DefenderProximity.h"
#include "CvCity.h"
#include "CvPlot.h"
#include "CvGameCoreUtils.h"

inline DefenderProximity::Key defenderCityProximity(const CvPlot* from,
 const CvCity* destination,int order)
{
 const CvPlot* target=destination->plot();
 const CvPlotGroup* network=from->getPlotGroup(destination->getOwner());
 const bool connected=from->area()==target->area()&&network!=NULL
  &&network==target->getPlotGroup(destination->getOwner());
 return DefenderProximity::Key(from==target,connected,
  stepDistance(from->getX(),from->getY(),target->getX(),target->getY()),order);
}
