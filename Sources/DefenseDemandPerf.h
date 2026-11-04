#pragma once
// Optional benchmark observer. Ordinary DLLs have the policy enabled and no
// environment lookups, file writes or counters in the assignment loop.
namespace DefenseDemandPerf {
enum Counter { REFRESHES, POSTS, DEMANDS, INCOMING, CANDIDATES, PROTECTED,
 PATHS, FOUND, FAILED, ASSIGNED, CANCELLED, ARRIVED, EXECUTION_FAILED,
 EPOCH_SKIPS, LEGACY_SKIPS, GROUPED, RESUMED, ADJACENT_ATTACKS, TRAVEL_BLOCKED,
 CACHE_HITS, CACHE_MISSES, COMMITMENT_UPDATES, FORT_RETRY_SKIPS, CACHE_AUDITS, CACHE_MISMATCHES,
 DEFENSE_ORDER_SKIPS, NATIVE_STRENGTH_READS, NATIVE_REQUIREMENT_READS, COUNT };
}
#if defined(C2C_ASSIGNMENT_SEARCH_DIAGNOSTICS) || defined(C2C_DEFENSE_POLICY_COMPARISON)
#include <cstdlib>
namespace DefenseDemandPerf {
inline bool enabled(){static const char* p=getenv("C2C_DEFENSE_DEMAND");return p&&p[0]=='1';}
inline bool cacheEnabled(){static const char* p=getenv("C2C_DEFENSE_NEED_CACHE");return enabled()&&p&&(p[0]=='1'||p[0]=='2');}
inline bool cacheFieldsEnabled(){static const char* p=getenv("C2C_DEFENSE_NEED_CACHE");return enabled()&&p&&p[0]=='2';}
#ifdef C2C_ASSIGNMENT_SEARCH_DIAGNOSTICS
inline bool cacheAudit(){static const char* p=getenv("C2C_DEFENSE_NEED_AUDIT");return cacheEnabled()&&p&&p[0]=='1';}
#else
inline bool cacheAudit(){return false;}
#endif
inline bool fortRetriesEnabled(){static const char* p=getenv("C2C_DEFENSE_FORT_RETRIES");return enabled()&&p&&p[0]=='1';}
}
#else
#ifndef C2C_DEFENSE_NEED_CACHE
#define C2C_DEFENSE_NEED_CACHE 2
#endif
namespace DefenseDemandPerf {inline bool enabled(){return true;}inline bool cacheEnabled(){return C2C_DEFENSE_NEED_CACHE!=0;}inline bool cacheFieldsEnabled(){return C2C_DEFENSE_NEED_CACHE>1;}inline bool cacheAudit(){return false;}inline bool fortRetriesEnabled(){return true;}}
#endif
#ifdef C2C_ASSIGNMENT_SEARCH_DIAGNOSTICS
#include "CvGameAI.h"
#include <cstdio>
#include <cstring>
#include <float.h>
namespace DefenseDemandPerf {
struct State {int turn;__int64 values[COUNT];State():turn(-1){memset(values,0,sizeof(values));}};
inline State& state(){static State s;return s;}
inline void tick(){State& s=state();const int t=GC.getGame().getGameTurn();if(t==s.turn)return;
 const char* dir=getenv("C2C_AI_BENCH_OUTPUT");if(s.turn>=0&&dir){const unsigned fp=_controlfp(0,0);char path[4096];_snprintf(path,sizeof(path),"%s/DefenseDemand.tsv",dir);path[sizeof(path)-1]=0;FILE* f=fopen(path,"a");
 if(f){fprintf(f,"%d\t%d",s.turn,enabled()?1:0);for(int i=0;i<COUNT;++i)fprintf(f,"\t%I64d",s.values[i]);fprintf(f,"\n");fclose(f);}_controlfp(fp,_MCW_PC|_MCW_RC|_MCW_EM|_MCW_IC);}
 s.turn=t;memset(s.values,0,sizeof(s.values));}
inline void count(Counter c,__int64 n=1){tick();state().values[c]+=n;}
inline void mismatch(int owner,int x,int y,int kind,int extra,int any,int cached,int actual)
{
 count(CACHE_MISMATCHES);if(state().values[CACHE_MISMATCHES]>64)return;
 const char* dir=getenv("C2C_AI_BENCH_OUTPUT");if(!dir)return;const unsigned fp=_controlfp(0,0);
 char path[4096];_snprintf(path,sizeof(path),"%s/DefenseCacheMismatch.tsv",dir);path[sizeof(path)-1]=0;FILE* f=fopen(path,"a");
 if(f){fprintf(f,"%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",state().turn,owner,x,y,kind,extra,any,cached,actual);fclose(f);}
 _controlfp(fp,_MCW_PC|_MCW_RC|_MCW_EM|_MCW_IC);
}
}
#else
namespace DefenseDemandPerf {inline void count(Counter,__int64=1){}inline void mismatch(int,int,int,int,int,int,int,int){}}
#endif
