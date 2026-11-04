#pragma once
#include <vector>
class CvSelectionGroup;

// Transient demand index. All long-lived references are map/owner/group/unit
// IDs; native pickup missions serialize their own target as before.
namespace TransportRequests {
    bool enabled();
    void reset();
    void invalidateAll();
    void dirty(CvSelectionGroup* group);
    void forget(CvSelectionGroup* group);
    void publish(CvSelectionGroup* group, bool stranded);
    void considerIdle(CvSelectionGroup* group);
    bool advanceLand(CvSelectionGroup* group);
    bool nearestAvailable(int owner,int group,int ship);
    void rejected(int owner,int group,int ship,int maxTurns);
    bool waitingOnly(CvSelectionGroup* group);
    void prepare(int owner);
    std::vector<CvSelectionGroup*> candidates(int owner, int ship = -1);
    void assign(int owner, int group, int ship);
    bool awaiting(CvSelectionGroup* group);
}
