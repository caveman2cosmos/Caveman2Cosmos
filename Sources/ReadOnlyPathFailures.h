#pragma once
#include <map>
#include <algorithm>
#include <limits.h>
#include "ReadOnlyLandReachability.h"

// Scratch state for ONE read-only assignment phase. The caller must destroy it
// before moving, regrouping, changing visibility or issuing any physical order.
// Only failures are reused: successful queries still publish their real path.
class ReadOnlyPathFailures
{
    struct Key
    {
        int values[8];
        bool operator<(const Key& b) const
        { return std::lexicographical_compare(values,values+8,b.values,b.values+8); }
    };
    std::map<Key,int> failures;
    unsigned int reused;
    size_t capacity;
    ReadOnlyLandReachability landReach;
    struct AllowAll {bool operator()() const{return true;}};
public:
    explicit ReadOnlyPathFailures(size_t maximumFailures=512):reused(0),capacity(maximumFailures){}
    void clear(){failures.clear();landReach.clear();}
    ReadOnlyLandReachability& landReachability(){return landReach;}
    unsigned int hits() const {return reused;}
    size_t size() const {return failures.size();}
    template<class Unit,class Plot>
    bool path(const Unit* unit,const Plot* target,int flags,int* turns,int maxTurns)
    {
        AllowAll guard;
        return path(unit,target,flags,turns,maxTurns,guard);
    }
    template<class Unit,class Plot,class Guard>
    bool path(const Unit* unit,const Plot* target,int flags,int* turns,int maxTurns,Guard& guard)
    {
        if(maxTurns<0)return unit->generatePath(target,flags,true,turns,maxTurns);
        const Key key={{unit->getID(),unit->getGroup()?unit->getGroup()->getID():-1,
            unit->getTeam(),unit->getX(),unit->getY(),target->getX(),target->getY(),flags}};
        std::map<Key,int>::iterator old=failures.find(key);
        if(old!=failures.end() && maxTurns<=old->second)
        {++reused;if(turns)*turns=INT_MAX;return false;}
        // Existing failures are cheaper than a preflight. Positive queries
        // still run the native generator and publish the actual chosen path.
        const bool possible=guard();
        if(!possible && turns)*turns=INT_MAX;
        const bool found=possible && unit->generatePath(target,flags,true,turns,maxTurns);
        if(!found && maxTurns>=0)
        {
            if(old!=failures.end())old->second=std::max(old->second,maxTurns);
            else if(failures.size()<capacity)failures.insert(std::make_pair(key,maxTurns));
        }
        return found;
    }
};
