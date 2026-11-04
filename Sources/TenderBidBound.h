#pragma once

// Hypothetical units finishMoves() before path generation. The first edge to
// another plot therefore costs at least one path turn, whose score divisor is
// 1 + intSqrt(1) == 2. A local bid can still have zero distance and divisor one.
namespace TenderBidBound {
inline bool canBeat(int value,int best,bool samePlot,bool useMinimumTurn=true)
{return (useMinimumTurn && !samePlot ? value/2 : value)>best;}
}

#ifdef C2C_TENDER_BID_COMPARISON
#include <cstdlib>
#include <cstdio>
#include <float.h>
namespace TenderBidBound {
inline bool enabled(){static const char* p=getenv("C2C_TENDER_BID_BOUND");return p&&p[0]=='1';}
inline bool auditEnabled(){static const char* p=getenv("C2C_TENDER_BID_AUDIT");return enabled()&&p&&p[0]=='1';}
struct Audit {int turn,checks,invalidLength,winningSkipped;Audit():turn(-1),checks(0),invalidLength(0),winningSkipped(0){}};
inline Audit& state(){static Audit a;return a;}
inline void observe(int turn,bool samePlot,int length,int actualScore,int best)
{
    Audit& a=state();
    if(a.turn!=turn)
    {
        const char* directory=getenv("C2C_AI_BENCH_OUTPUT");
        if(directory && a.turn>=0)
        {
            const unsigned fp=_controlfp(0,0);char path[4096];
            _snprintf(path,sizeof(path),"%s/TenderBidAudit.tsv",directory);path[sizeof(path)-1]=0;
            FILE* f=fopen(path,"a");
            if(f){fprintf(f,"%d\t%d\t%d\t%d\n",a.turn,a.checks,a.invalidLength,a.winningSkipped);fclose(f);}
            _controlfp(fp,_MCW_PC|_MCW_RC|_MCW_EM|_MCW_IC);
        }
        a=Audit();a.turn=turn;
    }
    ++a.checks;
    if(!samePlot && length>=0 && length<1)++a.invalidLength;
    if(length>=0 && actualScore>best)++a.winningSkipped;
}
}
#else
namespace TenderBidBound {
inline bool enabled(){return true;}
inline bool auditEnabled(){return false;}
}
#endif
