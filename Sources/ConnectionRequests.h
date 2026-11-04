#pragma once
#include <vector>
class CvPlot;
class CvUnitAI;
namespace ConnectionRequests {
    struct Request {
        int source,target;
        Request(int a,int b):source(a),target(b){}
    };
    void reset();
    void invalidate();
    // Producers plan once per owner/map/turn or infrastructure invalidation.
    // Consumers inspect only published unmet endpoint pairs, never all cities.
    std::vector<Request> candidates(const CvUnitAI& worker);
    bool needed(int owner,const CvPlot* source,const CvPlot* target);
    bool active(int owner,const CvPlot* source,const CvPlot* target);
    bool canWork(CvUnitAI& worker,const CvPlot* source,const CvPlot* target,int flags);
    void trace(const char* event,int owner,int unit,const Request& request);
}
