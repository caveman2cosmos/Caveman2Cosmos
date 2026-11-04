#pragma once
#include <vector>

// One immutable planning phase only. Valid nodes have undirected geographic
// adjacency; the native finder additionally accepts its root unconditionally.
// Label a target's component lazily, evaluating each plot predicate at most once.
class ReadOnlyRouteConnectivity {
    std::vector<signed char> valid;
    std::vector<int> labels;
    int nextLabel;
    template<class World> bool accepts(World& world,int node) {
        if(valid[node]<0)valid[node]=world.accepts(node)?1:0;
        return valid[node]!=0;
    }
    template<class World> int component(World& world,int root) {
        if(labels[root]>=0)return labels[root];
        const int label=nextLabel++;
        std::vector<int> queue,neighbors;
        queue.push_back(root);labels[root]=label;neighbors.reserve(8);
        for(size_t i=0;i<queue.size();++i){
            neighbors.clear();world.adjacent(queue[i],neighbors);
            for(size_t j=0;j<neighbors.size();++j){
                const int n=neighbors[j];
                if(n>=0 && n<(int)labels.size() && labels[n]<0 && accepts(world,n)){
                    labels[n]=label;queue.push_back(n);
                }
            }
        }
        return label;
    }
public:
    explicit ReadOnlyRouteConnectivity(int size):valid(size,(signed char)-1),labels(size,-1),nextLabel(0){}
    template<class World> bool connected(World& world,int from,int to) {
        if(from==to)return true;
        if(!accepts(world,to))return false;
        const int target=component(world,to);
        if(accepts(world,from))return labels[from]==target;
        // Root exception is directional: an unqualified start can leave into
        // the valid network, but cannot be entered as a destination.
        std::vector<int> neighbors;neighbors.reserve(8);world.adjacent(from,neighbors);
        for(size_t i=0;i<neighbors.size();++i){
            const int n=neighbors[i];
            if(n>=0 && n<(int)labels.size() && labels[n]==target)return true;
        }
        return false;
    }
private:ReadOnlyRouteConnectivity(const ReadOnlyRouteConnectivity&);ReadOnlyRouteConnectivity&operator=(const ReadOnlyRouteConnectivity&);
};
