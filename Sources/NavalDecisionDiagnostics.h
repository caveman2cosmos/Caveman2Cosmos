#pragma once

// Passive, compile-time diagnostics only. No cached answer is returned to AI.
// Include after the SDK header (Win32 clock declarations). Native tests provide
// the same clock API. All arguments disappear when diagnostics are disabled.
#ifdef C2C_NAVAL_DECISION_DIAGNOSTICS
#include <map>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <float.h>

namespace NavalDecisionDiagnostics
{
enum Kind { ROOT, STATUS, HEAL, ATTACK, BOMBARD, BLOCKADE, GROUP, SHADOW,
    PROTECT, SEA_AREA, GOODY, RETREAT, SAFETY, UPGRADE,
    PATH, ASTAR, FLOOD, TARGET_FLOOD, ODDS, BOMBARD_TARGET, KIND_COUNT };
enum { UNKNOWN_RESULT = -2147483647, MAX_KEYS = 4096, MAX_TRACES = 65536 };
typedef unsigned __int64 Count;
inline __int64 now() { LARGE_INTEGER t; QueryPerformanceCounter(&t); return t.QuadPart; }
struct Key
{
    int v[13];
    Key(int kind, int a=0,int b=0,int c=0,int d=0,int e=0,int f=0,
        int g=0,int h=0,int i=0,int j=0,int k=0)
    { const int values[]={kind,a,b,c,d,e,f,g,h,i,j,k,0}; for(int n=0;n<13;++n)v[n]=values[n]; }
    bool operator<(const Key& other) const
    { for(int i=0;i<13;++i) { if(v[i]<other.v[i])return true;if(v[i]>other.v[i])return false; }return false; }
};
struct Stat
{
    Count calls,repeated,crossPhase,different,unknown,processed,costed,noWork,droppedKeys,rawRepeats,rawDifferent;
    __int64 ticks,selfTicks,repeatTicks;
    Stat():calls(0),repeated(0),crossPhase(0),different(0),unknown(0),processed(0),costed(0),noWork(0),droppedKeys(0),rawRepeats(0),rawDifferent(0),ticks(0),selfTicks(0),repeatTicks(0){}
};
struct Trace
{
    int decision,owner,unit,phase,firstPhase,result,detail;
    Key key; Count processed,costed; __int64 ticks;
    Trace(const Key& k):key(k){}
};
struct Row
{
    int turn; bool flushed; unsigned nextDecision; Stat stats[KIND_COUNT];
    Count barriers,droppedTraces; __int64 bookkeeping; std::vector<Trace> traces;
    Row():turn(-1),flushed(false),nextDecision(0),barriers(0),droppedTraces(0),bookkeeping(0){}
};
struct Entry { int phase,result,detail; Entry(int p,int r,int d):phase(p),result(r),detail(d){} };
struct Decision;
struct Frame;
struct State
{
    Decision* decision; Frame* frame; __int64 frequency; Row rows[8];
    State():decision(NULL),frame(NULL) { LARGE_INTEGER f;QueryPerformanceFrequency(&f);frequency=f.QuadPart; }
    Row& row(int turn) { Row& r=rows[turn&7];if(r.turn!=turn){r=Row();r.turn=turn;}return r; }
};
inline State& state() { static State s;return s; }
struct Decision
{
    Decision* previous; Frame* previousFrame; int turn,owner,unit,id,epoch; __int64 start;
    std::map<Key,Entry> seen,rawSeen;
    Decision(int t,int o,int u,int /*group*/,int /*x*/,int /*y*/):previous(state().decision),previousFrame(state().frame),turn(t),owner(o),unit(u),epoch(0),start(now())
    { if(previous)++previous->epoch;id=++state().row(turn).nextDecision;state().decision=this;state().frame=NULL; }
    ~Decision();
};
inline bool active() { return state().decision!=NULL; }
inline void barrier() { if(active()){++state().decision->epoch;++state().row(state().decision->turn).barriers;} }
struct Frame
{
    Decision* decision; Frame* parent; Key key; int kind,phase; bool done; int result,detail;
    Count processed,costed; __int64 start,children;
    explicit Frame(const Key& k):decision(state().decision),parent(NULL),key(k),kind(k.v[0]),phase(ROOT),done(false),result(UNKNOWN_RESULT),detail(0),processed(0),costed(0),start(0),children(0)
    {
        if(!decision)return;parent=state().frame;
        if(kind<PATH)phase=kind;
        else if(parent)phase=parent->phase;
        key.v[12]=decision->epoch;start=now();state().frame=this;
    }
    void finish(int value,int extra=0)
    {
        if(done||!decision)return;done=true;const __int64 end=now(),elapsed=end-start;
        result=value;detail=extra;Row& row=state().row(decision->turn);Stat& stat=row.stats[kind];
        ++stat.calls;stat.ticks+=elapsed;stat.selfTicks+=elapsed-children;
        stat.processed+=processed;stat.costed+=costed;
        if(kind==ASTAR||kind==FLOOD||kind==TARGET_FLOOD)stat.noWork+=(processed==0&&costed==0);
        int firstPhase=-1;
        const bool matchable=kind>=PATH&&kind!=TARGET_FLOOD;
        std::map<Key,Entry>::iterator old=matchable?decision->seen.find(key):decision->seen.end();
        if(old!=decision->seen.end())
        {
            ++stat.repeated;stat.repeatTicks+=elapsed;firstPhase=old->second.phase;
            stat.crossPhase+=(firstPhase!=phase);
            if(result==UNKNOWN_RESULT||old->second.result==UNKNOWN_RESULT)++stat.unknown;
            else stat.different+=(result!=old->second.result||detail!=old->second.detail);
        }
        else if(matchable&&decision->seen.size()<MAX_KEYS)decision->seen.insert(std::make_pair(key,Entry(phase,result,detail)));
        else if(matchable)++stat.droppedKeys;
        Key raw=key;raw.v[12]=0;std::map<Key,Entry>::iterator prior=matchable?decision->rawSeen.find(raw):decision->rawSeen.end();
        if(prior!=decision->rawSeen.end())
        {++stat.rawRepeats;if(result!=UNKNOWN_RESULT&&prior->second.result!=UNKNOWN_RESULT)stat.rawDifferent+=(result!=prior->second.result||detail!=prior->second.detail);}
        else if(matchable&&decision->rawSeen.size()<MAX_KEYS)decision->rawSeen.insert(std::make_pair(raw,Entry(phase,result,detail)));
        // All phases and path/flood queries; high-volume bombard lookups only
        // when repeated or successful. Explicit drop counters bound memory.
        if(kind!=BOMBARD_TARGET||firstPhase>=0||result>=0)
        {
            if(row.traces.size()<MAX_TRACES)
            {
                Trace trace(key);trace.decision=decision->id;trace.owner=decision->owner;trace.unit=decision->unit;
                trace.phase=phase;trace.firstPhase=firstPhase;trace.result=result;trace.detail=detail;
                trace.processed=processed;trace.costed=costed;trace.ticks=elapsed;row.traces.push_back(trace);
            }
            else ++row.droppedTraces;
        }
        state().frame=parent;
        const __int64 overhead=now()-end;row.bookkeeping+=overhead;
        if(parent)parent->children+=elapsed+overhead;
    }
    template<class T> T returnValue(T value) { finish((int)value);return value; }
    template<class T> T storeValue(T value) { result=(int)value;return value; }
    void storeResult(int value,int extra) {result=value;detail=extra;}
    ~Frame() { finish(result,detail); }
};
inline Decision::~Decision()
{
    Row& row=state().row(turn);++row.stats[ROOT].calls;row.stats[ROOT].ticks+=now()-start;
    state().decision=previous;state().frame=previousFrame;
}
inline void addWork(Count processed,Count costed)
{ if(state().frame){state().frame->processed+=processed;state().frame->costed+=costed;} }
struct NodeWork
{
    Frame* frame; const int& processed; const int& costed; int p,c;
    NodeWork(const int& a,const int& b):frame(state().frame),processed(a),costed(b),p(a),c(b){}
    ~NodeWork(){if(frame){frame->processed+=processed-p;frame->costed+=costed-c;}}
};
inline const char* name(int kind)
{
    static const char* names[]={"ROOT","STATUS","HEAL","ATTACK","BOMBARD","BLOCKADE","GROUP","SHADOW","PROTECT","SEA_AREA","GOODY","RETREAT","SAFETY","UPGRADE","PATH","ASTAR","FLOOD","TARGET_FLOOD","ODDS","BOMBARD_TARGET"};
    return names[kind];
}
inline void flush(int turn)
{
    State& s=state();Row& r=s.rows[turn&7];if(r.turn!=turn||r.flushed)return;
    const char* directory=getenv("C2C_AI_BENCH_OUTPUT");if(!directory)return;
    const unsigned fp=_controlfp(0,0);char path[4096];
    _snprintf(path,sizeof(path),"%s/NavalDecision.log",directory);path[sizeof(path)-1]=0;FILE* f=fopen(path,"a");
    if(f)
    {
        for(int k=0;k<KIND_COUNT;++k)
        {const Stat& t=r.stats[k];fprintf(f,"NAVAL turn=%d kind=%s calls=%I64u ms=%.6f selfMs=%.6f repeats=%I64u repeatMs=%.6f crossPhase=%I64u different=%I64u unknown=%I64u processed=%I64u costed=%I64u noWork=%I64u droppedKeys=%I64u rawRepeats=%I64u rawDifferent=%I64u\n",turn,name(k),t.calls,1000.0*t.ticks/s.frequency,1000.0*t.selfTicks/s.frequency,t.repeated,1000.0*t.repeatTicks/s.frequency,t.crossPhase,t.different,t.unknown,t.processed,t.costed,t.noWork,t.droppedKeys,t.rawRepeats,t.rawDifferent);}
        fprintf(f,"NAVAL_OBSERVER turn=%d bookkeepingMs=%.6f barriers=%I64u droppedTraces=%I64u\n",turn,1000.0*r.bookkeeping/s.frequency,r.barriers,r.droppedTraces);fclose(f);
    }
    _snprintf(path,sizeof(path),"%s/NavalQueries.tsv",directory);path[sizeof(path)-1]=0;f=fopen(path,"a");
    if(f)
    {
        char buffer[65536];setvbuf(f,buffer,_IOFBF,sizeof(buffer));
        for(size_t i=0;i<r.traces.size();++i)
        {
            const Trace& t=r.traces[i];fprintf(f,"%d\t%d\t%d\t%d\t%s\t%s\t%d",turn,t.decision,t.owner,t.unit,name(t.key.v[0]),name(t.phase),t.firstPhase);
            for(int n=1;n<13;++n)fprintf(f,"\t%d",t.key.v[n]);
            fprintf(f,"\t%d\t%d\t%I64u\t%I64u\t%.6f\n",t.result,t.detail,t.processed,t.costed,1000.0*t.ticks/s.frequency);
        }
        fclose(f);
    }
    r.flushed=true;std::vector<Trace>().swap(r.traces);_controlfp(fp,_MCW_PC|_MCW_RC|_MCW_EM|_MCW_IC);
}
struct Turn {int turn;explicit Turn(int t):turn(t){}~Turn(){flush(turn);}};
}

#define NAVAL_DIAG_ROOT(t,o,u,g,x,y) NavalDecisionDiagnostics::Decision navalDiagnosticDecision(t,o,u,g,x,y)
#define NAVAL_DIAG_TURN(t) NavalDecisionDiagnostics::Turn navalDiagnosticTurn(t)
#define NAVAL_DIAG_PHASE(k,a,b,c,d) NavalDecisionDiagnostics::Frame navalDiagnosticPhase(NavalDecisionDiagnostics::active()?NavalDecisionDiagnostics::Key(NavalDecisionDiagnostics::k,a,b,c,d):NavalDecisionDiagnostics::Key(NavalDecisionDiagnostics::k))
#define NAVAL_DIAG_PHASE_RETURN(v) return navalDiagnosticPhase.storeValue(v)
#define NAVAL_DIAG_QUERY(k,a,b,c,d,e,f,g,h,i,j,l) NavalDecisionDiagnostics::Frame navalDiagnosticQuery(NavalDecisionDiagnostics::active()?NavalDecisionDiagnostics::Key(NavalDecisionDiagnostics::k,a,b,c,d,e,f,g,h,i,j,l):NavalDecisionDiagnostics::Key(NavalDecisionDiagnostics::k))
#define NAVAL_DIAG_QUERY_RETURN(v) return navalDiagnosticQuery.storeValue(v)
#define NAVAL_DIAG_NODE_RETURN(v) return navalDiagnosticQuery.storeValue(v)
#define NAVAL_DIAG_FINISH(v,d) navalDiagnosticQuery.storeResult(v,d)
#define NAVAL_DIAG_VOID_RETURN(v,d) do{navalDiagnosticQuery.storeResult(v,d);return;}while(0)
#define NAVAL_DIAG_BARRIER() NavalDecisionDiagnostics::barrier()
#define NAVAL_DIAG_NODES(p,c) NavalDecisionDiagnostics::NodeWork navalDiagnosticNodes(p,c)
#define NAVAL_DIAG_FLOOD_BEGIN() const bool navalDiagnosticActive=NavalDecisionDiagnostics::active();unsigned __int64 navalDiagnosticNodes=0,navalDiagnosticEdges=0
#define NAVAL_DIAG_FLOOD_NODE() do{if(navalDiagnosticActive)++navalDiagnosticNodes;}while(0)
#define NAVAL_DIAG_FLOOD_EDGE() do{if(navalDiagnosticActive)++navalDiagnosticEdges;}while(0)
#define NAVAL_DIAG_FLOOD_END() do{if(navalDiagnosticActive)NavalDecisionDiagnostics::addWork(navalDiagnosticNodes,navalDiagnosticEdges);}while(0)
#else
#define NAVAL_DIAG_ROOT(t,o,u,g,x,y)
#define NAVAL_DIAG_TURN(t)
#define NAVAL_DIAG_PHASE(k,a,b,c,d)
#define NAVAL_DIAG_PHASE_RETURN(v) return (v)
#define NAVAL_DIAG_QUERY(k,a,b,c,d,e,f,g,h,i,j,l)
#define NAVAL_DIAG_QUERY_RETURN(v) return (v)
#define NAVAL_DIAG_NODE_RETURN(v) return (v)
#define NAVAL_DIAG_FINISH(v,d)
#define NAVAL_DIAG_VOID_RETURN(v,d) return
#define NAVAL_DIAG_BARRIER()
#define NAVAL_DIAG_NODES(p,c)
#define NAVAL_DIAG_FLOOD_BEGIN()
#define NAVAL_DIAG_FLOOD_NODE()
#define NAVAL_DIAG_FLOOD_EDGE()
#define NAVAL_DIAG_FLOOD_END()
#endif
