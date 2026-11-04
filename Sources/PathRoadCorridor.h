#pragma once
#include <vector>
#include <algorithm>

// A disposable, bounded road-graph guide. It never proves that a unit can
// traverse an edge: the normal pathfinder still checks every allowed edge.
// The graph adapter exposes only owned, revealed land. No tier assumptions.
class PathRoadCorridor
{
    struct Cell
    {
        unsigned seen, allowed;
        int parent;
        Cell():seen(0),allowed(0),parent(-1){}
    };
    std::vector<Cell> cells;
    std::vector<int> queue;
    unsigned epoch;
    int examined;
    bool active;
public:
    PathRoadCorridor():epoch(0),examined(0),active(false){}
    bool allows(int plot) const
    { return active && plot>=0 && plot<(int)cells.size() && cells[plot].allowed==epoch; }
    int visits() const { return examined; }
    template<class Graph> bool build(const Graph& graph,int from,int to,int budget=2048)
    {
        examined=0;active=false;
        if(from<0 || to<0 || from>=graph.size() || to>=graph.size()
            || !graph.road(from) || !graph.road(to))return false;
        if((int)cells.size()!=graph.size()){cells.assign(graph.size(),Cell());epoch=0;}
        if(++epoch==0){std::fill(cells.begin(),cells.end(),Cell());epoch=1;}
        queue.clear();queue.push_back(from);cells[from].seen=epoch;cells[from].parent=-1;
        bool found=from==to;
        for(unsigned head=0;head<queue.size() && !found && examined<budget;++head)
        {
            const int p=queue[head];++examined;
            for(int d=0;d<8;++d)
            {
                const int n=graph.neighbor(p,d);
                if(n<0 || cells[n].seen==epoch)continue;
                cells[n].seen=epoch;
                if(!graph.road(n))continue;
                cells[n].parent=p;queue.push_back(n);
                if(n==to){found=true;break;}
            }
        }
        if(!found)return false; // Including budget exhaustion: use full A*.
        for(int p=to;p>=0;p=cells[p].parent)
        {
            mark(graph,p);
            for(int d=0;d<8;++d)
            {
                const int n=graph.neighbor(p,d);if(n<0)continue;
                mark(graph,n);
                for(int e=0;e<8;++e)mark(graph,graph.neighbor(n,e));
            }
        }
        active=true;return true;
    }
private:
    template<class Graph> void mark(const Graph& graph,int p)
    {if(p>=0 && graph.land(p))cells[p].allowed=epoch;}
};
