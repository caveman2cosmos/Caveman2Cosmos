#pragma once
#include <set>
class CvUnit;
class CvPlot;
class CvSelectionGroup;
namespace TransportRouting {
    struct EndpointOrder {
        const CvUnit* ship;
        explicit EndpointOrder(const CvUnit* vessel):ship(vessel){}
        bool operator()(const CvPlot* a,const CvPlot* b)const;
    };
    struct RequestOrder {
        const CvUnit* ship;
        explicit RequestOrder(const CvUnit* vessel):ship(vessel){}
        bool operator()(CvSelectionGroup* a,CvSelectionGroup* b)const;
    };
    bool safeCoast(CvSelectionGroup* group,const CvPlot* plot);
    CvPlot* chooseCoast(CvSelectionGroup* group);
    bool nearestAvailable(CvSelectionGroup* passengers,const CvUnit* current,
        const CvPlot* coast,const std::set<int>& rejected);
}
