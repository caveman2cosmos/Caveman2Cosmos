#pragma once
class CvUnit;
class CvPlot;
class ReadOnlyLandReachability;

// Only use during a read-only selector, or with the broker's before-orders
// invalidation barrier. A successful preflight never supplies a native path.
namespace ReadOnlyUnitPathFilter {
    bool couldConnect(const CvUnit* unit,const CvPlot* target,int flags,
        ReadOnlyLandReachability& cache);
    bool path(const CvUnit* unit,const CvPlot* target,int flags,int* turns,int maxTurns,
        ReadOnlyLandReachability& cache);
    struct Guard {
        const CvUnit* unit;
        const CvPlot* target;
        int flags;
        ReadOnlyLandReachability& cache;
        Guard(const CvUnit* u,const CvPlot* t,int f,ReadOnlyLandReachability& c)
            :unit(u),target(t),flags(f),cache(c){}
        bool operator()() const{return couldConnect(unit,target,flags,cache);}
    };
}
