#pragma once
#include <vector>
#include <algorithm>
#include <iterator>
// Both XML indexes are sorted, unique and rebuilt on XML/replacement refresh.
// Include ALL conditions: dynamic expressions must still execute in old order.
template<class Building> std::vector<Building> buildingEnablementCandidates(
    const std::vector<Building>& enabled,const std::vector<Building>& conditional){
    std::vector<Building> result;result.reserve(enabled.size()+conditional.size());
    std::set_union(enabled.begin(),enabled.end(),conditional.begin(),conditional.end(),std::back_inserter(result));
    return result;
}
