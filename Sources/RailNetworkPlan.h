#pragma once
#include <vector>
#include <algorithm>

// A deterministic road-corridor spanning forest, not a movement/path legality cache.
// Each component containing a real logistics endpoint gets useful rail trunks.
// Existing native canBuild/path checks still validate every worker mission.
class RailNetworkPlan
{
    struct Edge
    {
        int a,b,p,q,cost;
        Edge(int x,int y,int from,int to,int value):a(x),b(y),p(from),q(to),cost(value){}
        bool operator<(const Edge& rhs)const
        {
            if(cost!=rhs.cost)return cost<rhs.cost;
            if(p!=rhs.p)return p<rhs.p;
            return q<rhs.q;
        }
    };
    static int root(std::vector<int>& roots,int n)
    {
        int r=n;while(roots[r]!=r)r=roots[r];
        while(roots[n]!=n){int next=roots[n];roots[n]=r;n=next;}return r;
    }
public:
    std::vector<unsigned char> useful;
    unsigned long expanded,edges;
    RailNetworkPlan():expanded(0),edges(0){}
    template<class Grid>void build(const Grid& grid)
    {
        const int count=grid.size();useful.assign(count,0);expanded=edges=0;
        std::vector<int> label(count,-1),parent(count,-1),distance(count,0),queue,roots;
        std::vector<unsigned char> terminal;
        for(int p=0;p<count;++p)if(grid.valid(p)&&grid.endpoint(p))
        {
            label[p]=(int)roots.size();roots.push_back((int)roots.size());
            terminal.push_back(grid.terminal(p)?1:0);queue.push_back(p);
        }
        if(roots.size()<2)return;
        for(size_t head=0;head<queue.size();++head)
        {
            const int p=queue[head];++expanded;
            for(int d=0;d<8;++d)
            {
                const int q=grid.neighbor(p,d);if(q<0||!grid.valid(q)||label[q]>=0)continue;
                label[q]=label[p];parent[q]=p;distance[q]=distance[p]+1;queue.push_back(q);
            }
        }
        std::vector<Edge> candidates,selected;
        for(size_t i=0;i<queue.size();++i)
        {
            const int p=queue[i];
            for(int d=0;d<8;++d)
            {
                const int q=grid.neighbor(p,d);
                if(q<=p||q>=count||label[q]<0||label[p]==label[q])continue;
                candidates.push_back(Edge(label[p],label[q],p,q,distance[p]+distance[q]+1));
            }
        }
        std::sort(candidates.begin(),candidates.end());edges=(unsigned long)candidates.size();
        for(size_t i=0;i<candidates.size();++i)
        {
            const Edge&e=candidates[i];int a=root(roots,e.a),b=root(roots,e.b);
            if(a==b)continue;
            roots[b]=a;terminal[a]|=terminal[b];selected.push_back(e);
        }
        for(size_t i=0;i<selected.size();++i)
        {
            const Edge&e=selected[i];if(!terminal[root(roots,e.a)])continue;
            int p=e.p;while(p>=0&&!useful[p]){useful[p]=1;p=parent[p];}
            p=e.q;while(p>=0&&!useful[p]){useful[p]=1;p=parent[p];}
        }
    }
};
