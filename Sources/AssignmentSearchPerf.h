#pragma once

// Aggregate diagnostics and paired-control switches exist only in benchmark DLLs.
#ifdef C2C_ASSIGNMENT_SEARCH_DIAGNOSTICS
#include "CvGameAI.h"
#include <cstdio>
#include <cstdlib>
#include <float.h>
namespace AssignmentSearchPerf {
enum Kind { TENDERS, FORTS, HYPOTHETICAL, CACHE_BUILD, NO_ENEMY_QUERY, KIND_COUNT };
enum Counter { CANDIDATES, KNOWN_HOPS, UNKNOWN_HOPS, BAND_SKIPS, PATHS,
 FOUND, FAILED, CACHE_HITS, CACHE_MISSES, SELECTED, EXPANDED, BYTES,
 HOP_SNAPSHOTS, HOP_CACHE_MISSES, GENERIC_LAND_REQUESTS, FORT_TIGHT_LIMITS, COUNTER_COUNT };
inline int stage(){static const char* p=getenv("C2C_ASSIGNMENT_SEARCH_STAGE");static const int n=p?atoi(p):2;return n;}
inline bool hopsEnabled(){return stage()>=2;}
inline bool flagsEnabled(){return stage()>=1;}
struct Stat {unsigned long calls;__int64 ticks,counts[COUNTER_COUNT];Stat():calls(0),ticks(0){memset(counts,0,sizeof(counts));}};
struct State {int turn;__int64 hz;Stat rows[KIND_COUNT];State():turn(-1){LARGE_INTEGER f;QueryPerformanceFrequency(&f);hz=f.QuadPart;}};
inline State& state(){static State s;return s;}
inline void flush(){State& s=state();if(s.turn<0)return;const char* dir=getenv("C2C_AI_BENCH_OUTPUT");if(!dir)return;
 unsigned fp=_controlfp(0,0);char path[4096];_snprintf(path,sizeof(path),"%s/AssignmentSearch.tsv",dir);path[sizeof(path)-1]=0;FILE* f=fopen(path,"a");
 if(f){for(int k=0;k<KIND_COUNT;++k){const Stat& r=s.rows[k];if(!r.calls)continue;fprintf(f,"%d\t%d\t%d\t%lu\t%.6f",s.turn,stage(),k,r.calls,1000.0*r.ticks/s.hz);for(int c=0;c<COUNTER_COUNT;++c)fprintf(f,"\t%I64d",r.counts[c]);fprintf(f,"\n");}fclose(f);} _controlfp(fp,_MCW_PC|_MCW_RC|_MCW_EM|_MCW_IC);}
class Scope {Stat* row;__int64 started;public:explicit Scope(Kind k,bool enabled=true):row(NULL),started(0){if(!enabled||!GC.getGame().isFinalInitialized())return;State& s=state();const int t=GC.getGame().getGameTurn();if(t!=s.turn){flush();for(int i=0;i<KIND_COUNT;++i)s.rows[i]=Stat();s.turn=t;}row=&s.rows[k];++row->calls;LARGE_INTEGER n;QueryPerformanceCounter(&n);started=n.QuadPart;}
 ~Scope(){if(row){LARGE_INTEGER n;QueryPerformanceCounter(&n);row->ticks+=n.QuadPart-started;}}
 void count(Counter c,__int64 n=1){if(row)row->counts[c]+=n;}
private:Scope(const Scope&);Scope& operator=(const Scope&);};
}
#else
namespace AssignmentSearchPerf {
enum Kind { TENDERS, FORTS, HYPOTHETICAL, CACHE_BUILD, NO_ENEMY_QUERY };
enum Counter { CANDIDATES, KNOWN_HOPS, UNKNOWN_HOPS, BAND_SKIPS, PATHS,
 FOUND, FAILED, CACHE_HITS, CACHE_MISSES, SELECTED, EXPANDED, BYTES,
 HOP_SNAPSHOTS, HOP_CACHE_MISSES, GENERIC_LAND_REQUESTS, FORT_TIGHT_LIMITS };
inline bool hopsEnabled(){return true;}inline bool flagsEnabled(){return true;}
class Scope {public:explicit Scope(Kind,bool=true){}void count(Counter,__int64=1){}};
}
#endif
