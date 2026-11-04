#pragma once
#include <vector>
#include <string>

// Optimistic land components for ONE read-only broker phase. The adapter must
// admit every legal intermediate step: only disconnection can reject a path.
// Exact permission profiles prevent a specialist from borrowing another unit's
// closed-border result. Clear before orders; never keep this across a turn.
class ReadOnlyLandReachability
{
    const void* m_map;
    int m_team;
    bool m_valid;
    unsigned int m_builds;
    unsigned int m_queries;
    std::string m_permissions;
    std::vector<int> m_component, m_queue;

    template<class Graph>
    void build(const Graph& graph)
    {
        const int size=graph.size();
        m_component.assign(size,0);
        for(int i=0;i<size;++i)
            if(!graph.passable(i))m_component[i]=-1;
        int next=0;
        for(int i=0;i<size;++i)
        {
            if(m_component[i]!=0)continue;
            m_component[i]=++next;
            m_queue.clear();
            m_queue.push_back(i);
            for(size_t head=0;head<m_queue.size();++head)
                for(int d=0;d<8;++d)
                {
                    const int n=graph.neighbor(m_queue[head],d);
                    if(n>=0 && m_component[n]==0)
                    {m_component[n]=next;m_queue.push_back(n);}
                }
        }
        m_valid=true;
        ++m_builds;
    }

    template<class Graph>
    int endpoints(const Graph& graph,int plot,int* components) const
    {
        if(m_component[plot]>0)
        {components[0]=m_component[plot];return 1;}
        // Native endpoint checks can differ from intermediate-node checks.
        // Attach an exempt endpoint to every legal neighboring component.
        int count=0;
        for(int d=0;d<8;++d)
        {
            const int n=graph.neighbor(plot,d);
            if(n>=0 && m_component[n]>0)components[count++]=m_component[n];
        }
        return count;
    }
public:
    ReadOnlyLandReachability():m_map(NULL),m_team(-1),m_valid(false),m_builds(0),m_queries(0){}
    void clear(){m_valid=false;m_queries=0;}
    bool shouldFilterQuery()
    {
        // Profiling: isolated queries/ordinary eight-plot approaches do not
        // repay a map build. Amortize components over a larger read-only batch.
        // This is an optimization threshold, never a limit on AI searches.
        const unsigned int MIN_QUERIES_FOR_COMPONENT_BUILD=9;
        return ++m_queries>=MIN_QUERIES_FOR_COMPONENT_BUILD;
    }
    unsigned int builds() const{return m_builds;}

    template<class Graph>
    bool couldConnect(const Graph& graph,const void* map,int team,int from,int to,
        const std::string& permissions)
    {
        const int size=graph.size();
        if(from<0 || to<0 || from>=size || to>=size || size==0)return true;
        if(from==to)return true;
        // Preserve a direct step even if both endpoints are exempt.
        for(int d=0;d<8;++d)if(graph.neighbor(from,d)==to)return true;
        if(!m_valid || m_map!=map || m_team!=team || m_permissions!=permissions
            || (int)m_component.size()!=size)
        {
            m_map=map;m_team=team;m_permissions=permissions;
            build(graph);
        }
        int a[8],b[8];
        const int na=endpoints(graph,from,a),nb=endpoints(graph,to,b);
        for(int i=0;i<na;++i)
            for(int j=0;j<nb;++j)if(a[i]==b[j])return true;
        return false;
    }
};
