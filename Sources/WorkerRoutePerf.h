#pragma once

// Aggregate targeted diagnostics. Normal playable builds have no clock reads,
// counters, environment overrides or logging from these probes.
#ifdef C2C_WORKER_ROUTE_DIAGNOSTICS
#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include <cstdlib>
#include <float.h>
namespace WorkerRoutePerf
{
    enum Scope { SELECTOR, CERTIFICATE, COMPONENT, FINDER, SCOPE_COUNT };
    enum Counter { CANDIDATES, CERTIFIED, SEARCHES, COMPONENT_PLOTS, GATE_ZERO, GATE_PRESENT, QUERY_MEMO_HITS, QUERY_MEMO_MISSES, COUNTER_COUNT };
    struct Row
    {
        int turn;
        bool flushed;
        unsigned long calls[SCOPE_COUNT], counters[COUNTER_COUNT];
        __int64 ticks[SCOPE_COUNT];
        Row():turn(-1),flushed(false)
        {
            for(int i=0;i<SCOPE_COUNT;++i){calls[i]=0;ticks[i]=0;}
            for(int i=0;i<COUNTER_COUNT;++i)counters[i]=0;
        }
    };
    struct State
    {
        Row rows[8];
        __int64 frequency;
        State(){LARGE_INTEGER f;QueryPerformanceFrequency(&f);frequency=f.QuadPart;}
        Row& row(int turn){Row& r=rows[turn&7];if(r.turn!=turn){r=Row();r.turn=turn;}return r;}
    };
    inline State& state(){static State s;return s;}
    inline bool cacheEnabled()
    {
        static const bool useCache=getenv("C2C_WORKER_ROUTE_CACHE_DISABLE")==NULL
            || atoi(getenv("C2C_WORKER_ROUTE_CACHE_DISABLE"))==0;
        return useCache;
    }
    inline void count(Counter counter,unsigned long n=1)
    {if(GC.getGame().isFinalInitialized())state().row(GC.getGame().getGameTurn()).counters[counter]+=n;}
    class Timer
    {
        Scope scope;int turn;__int64 start;bool active;
    public:
        explicit Timer(Scope kind):scope(kind),turn(GC.getGame().getGameTurn()),start(0),active(GC.getGame().isFinalInitialized())
        {if(active){LARGE_INTEGER t;QueryPerformanceCounter(&t);start=t.QuadPart;}}
        ~Timer()
        {
            if(active){LARGE_INTEGER t;QueryPerformanceCounter(&t);Row& r=state().row(turn);++r.calls[scope];r.ticks[scope]+=t.QuadPart-start;}
        }
    private:
        Timer(const Timer&);Timer& operator=(const Timer&);
    };
    inline void flush(int turn)
    {
        State& s=state();Row& r=s.row(turn);if(r.flushed)return;r.flushed=true;
        const unsigned int control=_controlfp(0,0);
        static bool configured=false;
        if(!configured){configured=true;gDLL->logMsg("TradeNetwork.log",CvString::format("WORKER_ROUTE_CONFIG cacheEnabled=%d aggregateDiagnostics=1",cacheEnabled()?1:0).c_str());}
        const char* names[]={"SELECTOR","CERTIFICATE","COMPONENT","FINDER"};
        for(int i=0;i<SCOPE_COUNT;++i)
            gDLL->logMsg("TradeNetwork.log",CvString::format("WORKER_ROUTE_SCOPE turn=%d scope=%s calls=%lu totalMs=%.6f",turn,names[i],r.calls[i],s.frequency>0?1000.0*r.ticks[i]/s.frequency:0).c_str());
        gDLL->logMsg("TradeNetwork.log",CvString::format("WORKER_ROUTE_COUNTS turn=%d candidates=%lu certified=%lu searches=%lu componentPlots=%lu gateZero=%lu gatePresent=%lu",
            turn,r.counters[CANDIDATES],r.counters[CERTIFIED],r.counters[SEARCHES],r.counters[COMPONENT_PLOTS],r.counters[GATE_ZERO],r.counters[GATE_PRESENT]).c_str());
        gDLL->logMsg("TradeNetwork.log",CvString::format("WORKER_ROUTE_MEMO turn=%d hits=%lu misses=%lu",turn,r.counters[QUERY_MEMO_HITS],r.counters[QUERY_MEMO_MISSES]).c_str());
        _controlfp(control,_MCW_PC|_MCW_RC|_MCW_EM|_MCW_IC);
    }
}
#define WORKER_ROUTE_TIMER(name,kind) WorkerRoutePerf::Timer name(WorkerRoutePerf::kind)
#define WORKER_ROUTE_COUNT(kind,n) WorkerRoutePerf::count(WorkerRoutePerf::kind,n)
#else
namespace WorkerRoutePerf { inline bool cacheEnabled(){return true;} inline void flush(int){} }
#define WORKER_ROUTE_TIMER(name,kind)
#define WORKER_ROUTE_COUNT(kind,n)
#endif
