#pragma once

// Per-decision work list, not another persistent city/resource cache.
// Nearby city plots come first; the existing owned-plot index preserves other
// owned water resources outside the current city footprints.
namespace SeaWorkerBonusCandidates
{
 inline void append(const CvUnitAI& unit, const CvPlot* tile,
                    std::set<int>& seen, std::vector<int>& result)
 {
  if (tile == NULL || !tile->isWater() || tile->getOwner() != unit.getOwner()
      || tile->getNonObsoleteBonusType(unit.getTeam()) == NO_BONUS) return;
  if (tile->area() != unit.area()
      && !(tile->isWater() && unit.plot()->isAdjacentToArea(tile->area()))) return;
  const int index = GC.getMap().plotNum(tile->getX(), tile->getY());
  if (seen.insert(index).second) result.push_back(index);
 }

 inline bool collect(const CvUnitAI& unit, std::vector<int>& result)
 {
  // The old planner may consider unowned plots for defended groups. Retain
  // that search rather than silently dropping prospective foreign-border jobs.
  if (unit.getDomainType() != DOMAIN_SEA
      || (unit.getGroup()->getNumUnits() > 1 && unit.getGroup()->canDefend())) return false;
  const CvPlayerAI& owner = GET_PLAYER(unit.getOwner());
  std::multimap<int, const CvCity*> cities;
  foreach_(const CvCity* city, owner.cities())
  {
   cities.insert(std::make_pair(stepDistance(unit.getX(), unit.getY(), city->getX(), city->getY()), city));
  }
  std::set<int> seen;
  for (std::multimap<int, const CvCity*>::const_iterator it = cities.begin(); it != cities.end(); ++it)
  {
   const CvCity* city = it->second;
   for (int i = 0; i < city->getNumCityPlots(); ++i)
    append(unit, city->getCityIndexPlot(i), seen, result);
   // Inspect the small raw radius as well as the current workable footprint.
   // ResourceId-wide vicinity queries cannot identify a boat's next job.
   for (int x = -3; x <= 3; ++x)
    for (int y = -3; y <= 3; ++y)
    {
     const CvPlot* tile = plotXY(city->getX(), city->getY(), x, y);
     if (tile != NULL && plotDistance(city->getX(), city->getY(), tile->getX(), tile->getY()) <= 3)
      append(unit, tile, seen, result);
    }
  }
  const std::vector<int>& owned = owner.getOwnedPlotIndices();
  for (size_t i = 0; i < owned.size(); ++i)
   append(unit, GC.getMap().plotByIndex(owned[i]), seen, result);
  return true;
 }
}
