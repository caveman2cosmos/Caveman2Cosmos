#pragma once
#include <vector>

// A makeContract call assigns work but never moves/adds advertising units.
// Keep original order; assignments made during that call are still checked
// by findBestUnit. Previously assessed units cannot serve any existing request.
// This scratch list is discarded on return, so it needs no invalidation.
template<class Advertisement>
void selectContractCandidates(const std::vector<Advertisement>& units,
 bool thisPlotOnly, int latestRequest, std::vector<int>& candidates)
{
 for(size_t i=0;i<units.size();++i)
 {
  const Advertisement& u=units[i];
  if(u.iUnitId>=0 && u.iContractedWorkRequest==-1
   && (thisPlotOnly ? u.iMatchedToRequestSeqThisPlot : u.iMatchedToRequestSeqAnyPlot)<latestRequest)
   candidates.push_back((int)i);
 }
}
