#pragma once
// Compile-time observers only; no evaluation of observer arguments in Release.
#ifdef C2C_TARGET_DECISION_DIAGNOSTICS
#include <cstdio>
#include <cstdlib>
#include <float.h>
namespace TargetDecisionDiagnostics
{
typedef unsigned __int64 Count;
enum Counter { INDEX_QUERIES, INDEX_BUILDS, INDEX_VISITS, INDEX_RETURNED,
    INDEX_FILTERED_OUT, INDEX_FALLBACK,
    BOMBARD_CALLS, BOMBARD_CITIES, BOMBARD_REJECTS,
    PILLAGE_CALLS, PILLAGE_PLOTS, PILLAGE_REJECTS, PILLAGE_CACHE_HITS,
    ATTACK_PREFLIGHT_PLOTS, ATTACK_PREFLIGHT_EMPTY, ATTACK_SELECTOR_PLOTS, ATTACK_SELECTOR_EMPTY,
    SEARCH_DESTROY_CALLS, ESCORT_QUERIES, ESCORT_UNIT_TYPES, ESCORT_EXISTS, COUNT };
enum TimerKind { INDEX_BUILD, BOMBARD_PREFLIGHT, PILLAGE_PREFLIGHT, SEARCH_DESTROY, ESCORT_AVAILABILITY, SD_STATUS, SD_WAIT, SD_GROUP_DAMAGE, SD_CONTRACTS, SD_EARLY_HUNT, SD_ESCORT, SD_LATE_HUNT, SD_EXPLORE, TIMERS };
struct Row
{
    int turn;Count counters[COUNT];__int64 ticks[TIMERS];
    Row():turn(-1){for(int i=0;i<COUNT;++i)counters[i]=0;for(int i=0;i<TIMERS;++i)ticks[i]=0;}
};
struct State
{
    __int64 frequency;Row rows[8];
    State(){LARGE_INTEGER f;QueryPerformanceFrequency(&f);frequency=f.QuadPart;}
    Row& row(int turn){Row& r=rows[turn&7];if(r.turn!=turn){r=Row();r.turn=turn;}return r;}
};
inline State& state(){static State s;return s;}
inline void add(int turn,Counter kind,Count value){state().row(turn).counters[kind]+=value;}
struct CounterScope
{
    int turn; Counter kind; Count value;
    CounterScope(int t,Counter k):turn(t),kind(k),value(0){}
    void operator++(){++value;}
    ~CounterScope(){add(turn,kind,value);}
};
inline __int64 now(){LARGE_INTEGER t;QueryPerformanceCounter(&t);return t.QuadPart;}
struct Timer
{
    int turn;TimerKind kind;__int64 start;
    Timer(int t,TimerKind k):turn(t),kind(k),start(now()){}
    ~Timer(){state().row(turn).ticks[kind]+=now()-start;}
};
struct SegmentTimer
{
    int turn; TimerKind kind; __int64 start;
    SegmentTimer(int t,TimerKind k):turn(t),kind(k),start(now()){}
    void next(TimerKind k){const __int64 t=now();state().row(turn).ticks[kind]+=t-start;start=t;kind=k;}
    ~SegmentTimer(){state().row(turn).ticks[kind]+=now()-start;}
};
inline void flush(int turn)
{
    State& s=state();Row& r=s.rows[turn&7];if(r.turn!=turn)return;
    const char* dir=getenv("C2C_AI_BENCH_OUTPUT");if(!dir)return;
    const unsigned fp=_controlfp(0,0);char path[4096];
    _snprintf(path,sizeof(path),"%s/TargetDecisions.log",dir);path[sizeof(path)-1]=0;
    FILE* f=fopen(path,"a");if(f)
    {
        const char* names[]={"INDEX_QUERIES","INDEX_BUILDS","INDEX_VISITS","INDEX_RETURNED","INDEX_FILTERED_OUT","INDEX_FALLBACK","BOMBARD_CALLS","BOMBARD_CITIES","BOMBARD_REJECTS",
            "PILLAGE_CALLS","PILLAGE_PLOTS","PILLAGE_REJECTS","PILLAGE_CACHE_HITS",
            "ATTACK_PREFLIGHT_PLOTS","ATTACK_PREFLIGHT_EMPTY","ATTACK_SELECTOR_PLOTS","ATTACK_SELECTOR_EMPTY",
            "SEARCH_DESTROY_CALLS","ESCORT_QUERIES","ESCORT_UNIT_TYPES","ESCORT_EXISTS"};
        for(int i=0;i<COUNT;++i)fprintf(f,"TARGET turn=%d kind=%s count=%I64u\n",turn,names[i],r.counters[i]);
        fprintf(f,"TARGET_TIME turn=%d indexBuildMs=%.6f bombardPreflightMs=%.6f\n",turn,1000.0*r.ticks[INDEX_BUILD]/s.frequency,1000.0*r.ticks[BOMBARD_PREFLIGHT]/s.frequency);
        fprintf(f,"TARGET_SEARCH_TIME turn=%d pillagePreflightMs=%.6f searchDestroyMs=%.6f escortAvailabilityMs=%.6f\n",turn,1000.0*r.ticks[PILLAGE_PREFLIGHT]/s.frequency,1000.0*r.ticks[SEARCH_DESTROY]/s.frequency,1000.0*r.ticks[ESCORT_AVAILABILITY]/s.frequency);
        const char* partNames[]={"STATUS","WAIT","GROUP_DAMAGE","CONTRACTS","EARLY_HUNT","ESCORT","LATE_HUNT","EXPLORE"};
        for(int i=SD_STATUS;i<TIMERS;++i)
            fprintf(f,"TARGET_SEARCH_PART turn=%d kind=%s ms=%.6f\n",turn,partNames[i-SD_STATUS],1000.0*r.ticks[i]/s.frequency);
        fclose(f);
    }
    _controlfp(fp,_MCW_PC|_MCW_RC|_MCW_EM|_MCW_IC);
}
struct Turn{int turn;explicit Turn(int t):turn(t){}~Turn(){flush(turn);}};
}
#define TARGET_DIAG_SEGMENTS(n,k) TargetDecisionDiagnostics::SegmentTimer n(GC.getGame().getGameTurn(),TargetDecisionDiagnostics::k)
#define TARGET_DIAG_NEXT(n,k) n.next(TargetDecisionDiagnostics::k)
#define TARGET_DIAG_ACCUM(n,k) TargetDecisionDiagnostics::CounterScope n(GC.getGame().getGameTurn(),TargetDecisionDiagnostics::k)
#define TARGET_DIAG_COUNT(k,n) TargetDecisionDiagnostics::add(GC.getGame().getGameTurn(),TargetDecisionDiagnostics::k,n)
#define TARGET_DIAG_TIMER(k) TargetDecisionDiagnostics::Timer targetDiagnosticTimer(GC.getGame().getGameTurn(),TargetDecisionDiagnostics::k)
#define TARGET_DIAG_LOCAL(n) TargetDecisionDiagnostics::Count n=0
#define TARGET_DIAG_VISIT(n) (++n)
#define TARGET_DIAG_TURN(t) TargetDecisionDiagnostics::Turn targetDiagnosticTurn(t)
#else
#define TARGET_DIAG_SEGMENTS(n,k)
#define TARGET_DIAG_NEXT(n,k)
#define TARGET_DIAG_ACCUM(n,k)
#define TARGET_DIAG_COUNT(k,n)
#define TARGET_DIAG_TIMER(k)
#define TARGET_DIAG_LOCAL(n)
#define TARGET_DIAG_VISIT(n)
#define TARGET_DIAG_TURN(t)
#endif
