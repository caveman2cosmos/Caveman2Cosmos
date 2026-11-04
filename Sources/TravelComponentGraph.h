#pragma once
#include <vector>
#include <map>
#include <algorithm>

// Physical areas are already connected. Only connector plots (tunnels for
// land, traversable canals/cities for sea) need their adjoining edges read.
// A rebuild is event driven, never a unit/turn refresh.
class TravelComponentGraph {
    std::vector<int> roots;
    int root(int p){int r=p;while(roots[r]!=r)r=roots[r];while(roots[p]!=p){int next=roots[p];roots[p]=r;p=next;}return r;}
    void join(int a,int b){a=root(a);b=root(b);if(a!=b)roots[std::max(a,b)]=std::min(a,b);}
public:
    // Keep connector provenance, including parallel passages. Removing one
    // bridge therefore never assumes that it was the only connection.
    std::vector<std::pair<int,int> > links;
    template<class Geography> void build(const Geography& world,int domain,bool potential){
        roots.assign(world.size(),-1);links.clear();std::map<int,int> areas;
        for(int i=0;i<world.size();++i){
            if(world.base(i,domain)){
                roots[i]=i;std::pair<std::map<int,int>::iterator,bool> slot=areas.insert(std::make_pair(world.area(i),i));
                if(!slot.second)join(i,slot.first->second);
            }else if(world.connector(i,domain,potential))roots[i]=i;
        }
        for(int i=0;i<world.size();++i)if(roots[i]>=0 && !world.base(i,domain))
            for(int d=0;d<8;++d){const int n=world.neighbor(i,d);if(n>=0&&roots[n]>=0){links.push_back(std::make_pair(i,n));join(i,n);}}
        for(int i=0;i<world.size();++i)if(roots[i]>=0)roots[i]=root(i);
    }
    bool active(int p)const{return p>=0&&p<(int)roots.size()&&roots[p]>=0;}
    bool connected(int a,int b)const{return active(a)&&active(b)&&roots[a]==roots[b];}
    size_t bytes()const{return roots.size()*sizeof(int);}
};
