#pragma once
#include <vector>
#include <cstdlib>
#include "WorkerRoutePerf.h"

// routeValid is independent of its parent except for the native root
// exception. Reuse its exact result only while a selector reads the same
// world. No values survive orders, another player/map, or this scope.
namespace WorkerRouteQueryMemo {
struct Data {
    int player,map,size;unsigned long thread;
    unsigned long hits,misses;
    std::vector<signed char> values;
    Data(int p,int m,int n,unsigned long t):player(p),map(m),size(n),thread(t),hits(0),misses(0){}
};
inline Data*& current(){static Data* value=NULL;return value;}
inline bool enabled(){
#ifdef C2C_RECALCULATED_BENCHMARK
    static const char* setting=getenv("C2C_WORKER_ROUTE_QUERY_MEMO");
    static const bool value=!setting || setting[0]!='0';
    return value;
#else
    return true;
#endif
}
class Scope {
    Data data;Data* previous;bool installed;
public:
    Scope(int player,int map,int plots):data(player,map,plots,GetCurrentThreadId()),previous(current()),installed(false){
        if(!enabled())return;
        if(previous && previous->player==player && previous->map==map && previous->size==plots
            && previous->thread==GetCurrentThreadId())return;
        current()=&data;installed=true;
    }
    ~Scope(){end();}
    void end(){if(installed){
        WORKER_ROUTE_COUNT(QUERY_MEMO_HITS,data.hits);
        WORKER_ROUTE_COUNT(QUERY_MEMO_MISSES,data.misses);
        current()=previous;installed=false;
    }}
private:Scope(const Scope&);Scope&operator=(const Scope&);
};
inline signed char* entry(int player,int map,int index){
    Data* data=current();
    if(!data || data->player!=player || data->map!=map || data->thread!=GetCurrentThreadId()
        || index<0 || index>=data->size)return NULL;
    if(data->values.empty())data->values.assign(data->size,(signed char)-1);
    signed char* value=&data->values[index];
    if(*value<0)++data->misses;else ++data->hits;
    return value;
}
}
