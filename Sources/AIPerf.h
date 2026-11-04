#pragma once

// Enabled only in the two AIPerf build configurations. Normal builds have no
// timing, counters, allocations or logging overhead from these macros.
#ifdef C2C_AI_PERF_ENABLE
#include "CvGameCoreDLL.h"
#include "CvGameAI.h"
#include "CvGlobals.h"
#include <map>
#include <vector>
#include <algorithm>

namespace AIPerf
{
	enum Scope
	{
		UPDATE_MOVES, UNIT_UPDATE, GROUP_PRIORITIES, GROUP_UPDATE, UNIT_DECISION,
		AUTO_MISSIONS, CONTRACT_POST, CONTRACT_MATCH, CONTRACT_PROCESS,
		UNIT_TARGET_QUERY, PATH, PATH_REUSE_VALIDATION, REACHABLE_POPULATE,
		REACHABLE_EXPANSION, CITY_PRODUCTION, CITY_CITIZENS, CITY_GUARD, ANY_ATTACK, PATROL, SAFETY, SAFETY_EVAL, SAFETY_PATH, SAFETY_SCORE, SAFETY_DANGER, SAFETY_SET, SAFETY_BOUND, CONTRACT_TENDERS, HYPOTHETICAL_PATH, WORKER_MOVE, WORKER_BONUS, WORKER_CITY, WORKER_LOCAL, WORKER_RELOCATE, WORKER_ROUTE, WORKER_CONNECT, WORKER_BUILD_EVAL, WORKER_UPGRADE, UNIT_CONSTRUCT, UNIT_OUTCOME, RETREAT_CITY, CITY_BUILD_VALUE, POPULATION_PREVIEW, BROKER_REACHABILITY, ATTACK_CHOICE, TRANSPORT_PREPARE, TRANSPORT_PICKUP, CIV_ESCORT_WAIT, CIV_SITES, CIV_ROUTE_CITY, TRANSPORT_COAST, TRANSPORT_MATCH, NUM_SCOPES
	};
	enum Counter
	{
		GROUP_PRIORITY_EVALUATIONS, UNIT_DECISIONS, REPEATED_UNIT_DECISIONS,
		CONTRACT_CANDIDATES, CONTRACT_PATHS_PRUNED, TARGET_PATHS_PRUNED,
		PATH_REQUESTS, PATH_FAILURES, PATH_REUSED, PATH_SEARCHES,
		PATH_RESET_GROUP, PATH_RESET_ORIGIN, PATH_NODES_PROCESSED,
		PATH_NODES_COSTED, REACHABLE_HITS, REACHABLE_MISSES,
		REACHABLE_EVICTIONS, SAFETY_EVALS, SAFETY_ENEMY_SKIPS, SAFETY_PATH_REQUESTS,
		SAFETY_PATH_FAILURES, SAFETY_PATH_DUPLICATES, SAFETY_SECOND_PASSES,
		SAFETY_DISTANT_REQUESTS, SAFETY_DISTANT_FAILURES, SAFETY_CANDIDATES_PRUNED, SAFETY_BOUND_APPLIED, SAFETY_FLOOD_PLOTS, SAFETY_FLOOD_FALLBACKS,
		REACHABLE_CLEARS, TENDER_PATHS_PRUNED, TENDER_PATH_CACHE_HITS, TENDER_PATH_CACHE_MISSES, UNCHANGED_UNIT_DECISIONS, PATH_LONG_REQUESTS, WORKER_PLOTS_SCANNED, WORKER_BUILD_CANDIDATES, WORKER_PATH_REJECTS, WORKER_MISSION_FAILURES, BROKER_REACHABILITY_CHECKS, BROKER_REACHABILITY_BUILDS, BROKER_REACHABILITY_REJECTS, ATTACK_STRENGTH_CACHE_HITS, ATTACK_STRENGTH_CACHE_MISSES, BUILD_ENABLEMENT_FULL_VISITS, BUILD_ENABLEMENT_INDEX_VISITS, ATTACK_MODIFIER_CACHE_HITS, ATTACK_MODIFIER_CACHE_MISSES, ATTACK_CHOICE_SCANS, ATTACK_CHOICE_EXACT_AID, NUM_COUNTERS
	};
	struct Stat
	{
		int calls;
		double totalMs, selfMs, maxMs, cpuMs;
		Stat() : calls(0), totalMs(0), selfMs(0), maxMs(0), cpuMs(0) {}
	};
	struct Round
	{
		struct RoleStat
		{
			Stat decisions, paths;
			unsigned __int64 nodesProcessed, nodesCosted;
			RoleStat() : nodesProcessed(0), nodesCosted(0) {}
		};
		std::map<int, RoleStat> roles;
		Stat scopes[NUM_SCOPES];
		unsigned __int64 counters[NUM_COUNTERS];
		// Map, owner and unit ID: repeated decisions can include legitimate moves.
		typedef std::pair<int, std::pair<int, int> > UnitKey;
		std::map<UnitKey, int> unitDecisions;
        struct UnitDetail
        {
            RoleStat work; int role,x,y,moves,mission,unchanged;
            UnitDetail():role(-1),x(-1),y(-1),moves(-1),mission(-1),unchanged(0){}
        };
        struct Actor
        {
            Stat scopes[NUM_SCOPES],callers[NUM_SCOPES];
            unsigned __int64 counters[NUM_COUNTERS];
            Actor(){memset(counters,0,sizeof(counters));}
        };
        typedef std::pair<int,int> ActorKey; // Map/player, not just player.
        std::map<ActorKey,Actor> actors;
        std::map<UnitKey,UnitDetail> units;
        Stat callers[NUM_SCOPES];
		double measuredRootMs;
		Round() : measuredRootMs(0) { memset(counters, 0, sizeof(counters)); }
	};
	class Timer;
	class SafetyTrace;
	struct State
	{
		DWORD thread;
		double frequency;
		Timer* active;
        int owner,unit;
		SafetyTrace* activeSafety;
		std::map<int, Round> rounds;
		State() : thread(GetCurrentThreadId()), frequency(0), active(NULL), owner(-1), unit(-1), activeSafety(NULL)
		{
			LARGE_INTEGER hz;
			if (QueryPerformanceFrequency(&hz)) frequency = (double)hz.QuadPart;
		}
	};
	inline State& state() { static State s; return s; }
	inline bool enabled()
	{
		return state().frequency > 0 && state().thread == GetCurrentThreadId()
			&& GC.getGame().isFinalInitialized();
	}
    // Context stores IDs only: profiling never retains game-object pointers.
    class Context
    {
        int owner,unit;
    public:
        Context(int actor,int id=-1):owner(state().owner),unit(state().unit)
        {state().owner=actor;state().unit=id;}
        ~Context(){state().owner=owner;state().unit=unit;}
    private: Context(const Context&);Context& operator=(const Context&);
    };
    inline double threadMilliseconds()
    {
#ifdef C2C_AI_PERF_CPU
        FILETIME created,exited,kernel,user;
        if(GetThreadTimes(GetCurrentThread(),&created,&exited,&kernel,&user))
        {
            ULARGE_INTEGER k,u;k.LowPart=kernel.dwLowDateTime;k.HighPart=kernel.dwHighDateTime;
            u.LowPart=user.dwLowDateTime;u.HighPart=user.dwHighDateTime;
            return (double)(k.QuadPart+u.QuadPart)/10000.0;
        }
#endif
        return 0;
    }
    inline bool expensiveRoot(Scope scope){return scope==UPDATE_MOVES || scope==UNIT_UPDATE;}
    inline void addStat(Stat& stat,double ms,double self,double cpu)
    {++stat.calls;stat.totalMs+=ms;stat.selfMs+=self;stat.cpuMs+=cpu;stat.maxMs=std::max(stat.maxMs,ms);}
	inline const char* variant()
	{
#if defined(C2C_AI_PERF_BASELINE) || defined(C2C_AI_SAFETY_BASELINE)
		return "baseline";
#else
		return "optimized";
#endif
	}
	inline const char* logFile()
	{
#if defined(C2C_AI_PERF_BASELINE) || defined(C2C_AI_SAFETY_BASELINE)
		return "AIPerf_Baseline.log";
#else
		return "AIPerf_Optimized.log";
#endif
	}
	inline int activeRole();
	inline void count(Counter counter, unsigned __int64 amount)
	{
		if (!enabled()) return;
		Round& round = state().rounds[GC.getGame().getGameTurn()];
		round.counters[counter] += amount;
        if(state().owner>=0)round.actors[Round::ActorKey(CURRENT_MAP,state().owner)].counters[counter]+=amount;
        if(state().owner>=0 && state().unit>=0 && (counter==PATH_NODES_PROCESSED || counter==PATH_NODES_COSTED))
        {
            Round::RoleStat& work=round.units[Round::UnitKey(CURRENT_MAP,std::make_pair(state().owner,state().unit))].work;
            if(counter==PATH_NODES_PROCESSED)work.nodesProcessed+=amount;else work.nodesCosted+=amount;
        }
		const int role = activeRole();
		if (role >= 0 && (counter == PATH_NODES_PROCESSED || counter == PATH_NODES_COSTED))
		{
			Round::RoleStat& stat = round.roles[role];
			if (counter == PATH_NODES_PROCESSED) stat.nodesProcessed += amount;
			else stat.nodesCosted += amount;
		}
	}
	inline void unitDecision(int owner, int unit,int x=-1,int y=-1,int moves=-1,int mission=-1)
	{
		if (!enabled()) return;
		Round& round = state().rounds[GC.getGame().getGameTurn()];
		const Round::UnitKey key(CURRENT_MAP, std::make_pair(owner, unit));
		++round.counters[UNIT_DECISIONS];
		const bool repeated=++round.unitDecisions[key]>1;
        if(repeated)++round.counters[REPEATED_UNIT_DECISIONS];
        if(x>=0)
        {
            Round::UnitDetail& detail=round.units[key];
            if(repeated && detail.x==x && detail.y==y && detail.moves==moves && detail.mission==mission)
            {++detail.unchanged;count(UNCHANGED_UNIT_DECISIONS,1);}
            detail.x=x;detail.y=y;detail.moves=moves;detail.mission=mission;
        }
	}
	inline void flush(int turn)
	{
		State& s = state();
		std::map<int, Round>::iterator found = s.rounds.find(turn);
		if (found == s.rounds.end() || s.thread != GetCurrentThreadId()) return;
		const Round& round = found->second;
		gDLL->logMsg(logFile(), CvString::format(
			"AI_PERF_ROUND variant=%s turn=%d measuredRootMs=%.3f uniqueUnits=%u",
			variant(), turn, round.measuredRootMs, (unsigned int)round.unitDecisions.size()).c_str());
		const char* scopeNames[] = {
			"UPDATE_MOVES", "UNIT_UPDATE", "GROUP_PRIORITIES", "GROUP_UPDATE", "UNIT_DECISION", "AUTO_MISSIONS", "CONTRACT_POST", "CONTRACT_MATCH", "CONTRACT_PROCESS", "UNIT_TARGET_QUERY", "PATH", "PATH_REUSE_VALIDATION", "REACHABLE_POPULATE", "REACHABLE_EXPANSION", "CITY_PRODUCTION", "CITY_CITIZENS", "CITY_GUARD", "ANY_ATTACK", "PATROL", "SAFETY", "SAFETY_EVAL", "SAFETY_PATH", "SAFETY_SCORE", "SAFETY_DANGER", "SAFETY_SET", "SAFETY_BOUND", "CONTRACT_TENDERS", "HYPOTHETICAL_PATH", "WORKER_MOVE", "WORKER_BONUS", "WORKER_CITY", "WORKER_LOCAL", "WORKER_RELOCATE", "WORKER_ROUTE", "WORKER_CONNECT", "WORKER_BUILD_EVAL", "WORKER_UPGRADE", "UNIT_CONSTRUCT", "UNIT_OUTCOME", "RETREAT_CITY", "CITY_BUILD_VALUE", "POPULATION_PREVIEW", "BROKER_REACHABILITY", "ATTACK_CHOICE", "TRANSPORT_PREPARE", "TRANSPORT_PICKUP", "CIV_ESCORT_WAIT", "CIV_SITES", "CIV_ROUTE_CITY", "TRANSPORT_COAST", "TRANSPORT_MATCH"
		};
		for (int i = 0; i < NUM_SCOPES; ++i)
		{
			const Stat& stat = round.scopes[i];
			gDLL->logMsg(logFile(), CvString::format(
				"AI_PERF_SCOPE variant=%s turn=%d scope=%s calls=%d totalMs=%.3f selfMs=%.3f maxMs=%.3f cpuMs=%.3f",
				variant(), turn, scopeNames[i], stat.calls, stat.totalMs, stat.selfMs, stat.maxMs, stat.cpuMs).c_str());
		}
		for (std::map<int, Round::RoleStat>::const_iterator role = round.roles.begin(); role != round.roles.end(); ++role)
		{
			const Round::RoleStat& stat = role->second;
			gDLL->logMsg(logFile(), CvString::format(
				"AI_PERF_ROLE variant=%s turn=%d unitAI=%d decisionCalls=%d decisionMs=%.3f pathCalls=%d pathMs=%.3f nodesProcessed=%I64u nodesCosted=%I64u",
				variant(), turn, role->first, stat.decisions.calls, stat.decisions.totalMs,
				stat.paths.calls, stat.paths.totalMs, stat.nodesProcessed, stat.nodesCosted).c_str());
		}
		const char* counterNames[] = {
			"GROUP_PRIORITY_EVALUATIONS", "UNIT_DECISIONS", "REPEATED_UNIT_DECISIONS", "CONTRACT_CANDIDATES", "CONTRACT_PATHS_PRUNED", "TARGET_PATHS_PRUNED", "PATH_REQUESTS", "PATH_FAILURES", "PATH_REUSED", "PATH_SEARCHES", "PATH_RESET_GROUP", "PATH_RESET_ORIGIN", "PATH_NODES_PROCESSED", "PATH_NODES_COSTED", "REACHABLE_HITS", "REACHABLE_MISSES", "REACHABLE_EVICTIONS", "SAFETY_EVALS", "SAFETY_ENEMY_SKIPS", "SAFETY_PATH_REQUESTS", "SAFETY_PATH_FAILURES", "SAFETY_PATH_DUPLICATES", "SAFETY_SECOND_PASSES", "SAFETY_DISTANT_REQUESTS", "SAFETY_DISTANT_FAILURES", "SAFETY_CANDIDATES_PRUNED", "SAFETY_BOUND_APPLIED", "SAFETY_FLOOD_PLOTS", "SAFETY_FLOOD_FALLBACKS", "REACHABLE_CLEARS", "TENDER_PATHS_PRUNED", "TENDER_PATH_CACHE_HITS", "TENDER_PATH_CACHE_MISSES", "UNCHANGED_UNIT_DECISIONS", "PATH_LONG_REQUESTS", "WORKER_PLOTS_SCANNED", "WORKER_BUILD_CANDIDATES", "WORKER_PATH_REJECTS", "WORKER_MISSION_FAILURES", "BROKER_REACHABILITY_CHECKS", "BROKER_REACHABILITY_BUILDS", "BROKER_REACHABILITY_REJECTS", "ATTACK_STRENGTH_CACHE_HITS", "ATTACK_STRENGTH_CACHE_MISSES", "BUILD_ENABLEMENT_FULL_VISITS", "BUILD_ENABLEMENT_INDEX_VISITS", "ATTACK_MODIFIER_CACHE_HITS", "ATTACK_MODIFIER_CACHE_MISSES", "ATTACK_CHOICE_SCANS", "ATTACK_CHOICE_EXACT_AID"
		};
		for (int i = 0; i < NUM_COUNTERS; ++i)
			gDLL->logMsg(logFile(), CvString::format(
				"AI_PERF_COUNT variant=%s turn=%d counter=%s value=%I64u",
				variant(), turn, counterNames[i], round.counters[i]).c_str());
        for(int i=0;i<NUM_SCOPES;++i)if(round.callers[i].calls)
            gDLL->logMsg(logFile(),CvString::format("AI_PERF_PATH_CALLER turn=%d caller=%s calls=%d pathMs=%.3f",turn,scopeNames[i],round.callers[i].calls,round.callers[i].totalMs).c_str());
        for(std::map<Round::ActorKey,Round::Actor>::const_iterator actor=round.actors.begin();actor!=round.actors.end();++actor)
        {
            for(int i=0;i<NUM_SCOPES;++i)
            {
                const Stat& stat=actor->second.scopes[i];if(!stat.calls)continue;
                gDLL->logMsg(logFile(),CvString::format("AI_PERF_ACTOR_SCOPE turn=%d map=%d player=%d scope=%s calls=%d totalMs=%.3f selfMs=%.3f maxMs=%.3f cpuMs=%.3f",turn,actor->first.first,actor->first.second,scopeNames[i],stat.calls,stat.totalMs,stat.selfMs,stat.maxMs,stat.cpuMs).c_str());
            }
            for(int i=0;i<NUM_SCOPES;++i)if(actor->second.callers[i].calls)
                gDLL->logMsg(logFile(),CvString::format("AI_PERF_ACTOR_CALLER turn=%d map=%d player=%d caller=%s calls=%d pathMs=%.3f",turn,actor->first.first,actor->first.second,scopeNames[i],actor->second.callers[i].calls,actor->second.callers[i].totalMs).c_str());
            for(int i=0;i<NUM_COUNTERS;++i)if(actor->second.counters[i])
                gDLL->logMsg(logFile(),CvString::format("AI_PERF_ACTOR_COUNT turn=%d map=%d player=%d counter=%s value=%I64u",turn,actor->first.first,actor->first.second,counterNames[i],actor->second.counters[i]).c_str());
        }
        // Inclusive decision time already contains its paths: rank by max,
        // not their sum. All units are counted; only the top 25 are written.
        typedef std::pair<double,Round::UnitKey> RankedUnit;
        std::vector<RankedUnit> ranked;
        for(std::map<Round::UnitKey,Round::UnitDetail>::const_iterator u=round.units.begin();u!=round.units.end();++u)
            ranked.push_back(RankedUnit(std::max(u->second.work.decisions.totalMs,u->second.work.paths.totalMs),u->first));
        std::sort(ranked.begin(),ranked.end());
        for(size_t n=0;n<ranked.size() && n<25;++n)
        {
            const Round::UnitKey& key=ranked[ranked.size()-1-n].second;
            const Round::UnitDetail& u=round.units.find(key)->second;
            gDLL->logMsg(logFile(),CvString::format("AI_PERF_UNIT turn=%d map=%d player=%d unit=%d role=%d x=%d y=%d moves=%d mission=%d decisions=%d unchanged=%d decisionMs=%.3f paths=%d pathMs=%.3f nodesProcessed=%I64u nodesCosted=%I64u",turn,key.first,key.second.first,key.second.second,u.role,u.x,u.y,u.moves,u.mission,u.work.decisions.calls,u.unchanged,u.work.decisions.totalMs,u.work.paths.calls,u.work.paths.totalMs,u.work.nodesProcessed,u.work.nodesCosted).c_str());
        }
		s.rounds.erase(found);
	}
	inline void reset()
	{
		// Called when a new game/save is loaded, before any measured work starts.
		state().rounds.clear();
	}
	class Timer
	{
		Scope m_scope;
		int m_turn;
		bool m_enabled;
		LARGE_INTEGER m_start;
		Timer* m_parent;
		double m_childrenMs;
		int m_role,m_owner,m_unit,m_map;
        double m_startCpu;
	public:
		explicit Timer(Scope scope, int role = -1) : m_scope(scope), m_turn(0), m_enabled(enabled()),
			m_parent(NULL), m_childrenMs(0), m_role(role),m_owner(state().owner),m_unit(state().unit),m_map(CURRENT_MAP),m_startCpu(0)
		{
			if (m_enabled)
			{
				m_turn = GC.getGame().getGameTurn();
				m_parent = state().active;
				if (m_role < 0 && m_parent) m_role = m_parent->m_role;
				state().active = this;
				QueryPerformanceCounter(&m_start);
                if(expensiveRoot(scope))m_startCpu=threadMilliseconds();
			}
		}
		~Timer()
		{
			if (!m_enabled) return;
			LARGE_INTEGER end;
			QueryPerformanceCounter(&end);
			State& s = state();
			const double ms = 1000.0 * (end.QuadPart - m_start.QuadPart) / s.frequency;
			Round& round = s.rounds[m_turn];
			Stat& stat = round.scopes[m_scope];
			++stat.calls;
			stat.totalMs += ms;
			stat.selfMs += std::max(0.0, ms - m_childrenMs);
			stat.maxMs = std::max(stat.maxMs, ms);
			if (m_role >= 0 && (m_scope == UNIT_DECISION || m_scope == PATH))
			{
				Round::RoleStat& role = round.roles[m_role];
				Stat& roleStat = m_scope == PATH ? role.paths : role.decisions;
				++roleStat.calls;
				roleStat.totalMs += ms;
			}
            const double cpu=expensiveRoot(m_scope) ? std::max(0.0,threadMilliseconds()-m_startCpu) : 0;
            stat.cpuMs+=cpu;
            if(m_owner>=0)addStat(round.actors[Round::ActorKey(m_map,m_owner)].scopes[m_scope],ms,std::max(0.0,ms-m_childrenMs),cpu);
            if(m_scope==PATH && m_parent)
            {
                addStat(round.callers[m_parent->m_scope],ms,0,0);
                if(m_owner>=0)addStat(round.actors[Round::ActorKey(m_map,m_owner)].callers[m_parent->m_scope],ms,0,0);
            }
            if(m_owner>=0 && m_unit>=0 && (m_scope==UNIT_DECISION || m_scope==PATH))
            {
                Round::UnitDetail& unit=round.units[Round::UnitKey(m_map,std::make_pair(m_owner,m_unit))];
                unit.role=m_role;addStat(m_scope==PATH ? unit.work.paths : unit.work.decisions,ms,std::max(0.0,ms-m_childrenMs),0);
            }
			if (m_parent) m_parent->m_childrenMs += ms;
			else round.measuredRootMs += ms;
			s.active = m_parent;
		}
		int role() const { return m_role; }
	private:
		Timer(const Timer&);
		Timer& operator=(const Timer&);
	};
	// Diagnostic lifetime is one safety decision: its origin and range stay fixed.
	// This is a query trace, not a gameplay cache.
	class SafetyTrace
	{
		SafetyTrace* m_parent;
		bool m_enabled;
		bool m_predictedUnreachable;
		std::map<std::pair<const void*, int>, int> m_queries;
	public:
		SafetyTrace() : m_parent(NULL), m_enabled(enabled()), m_predictedUnreachable(false)
		{
			if (m_enabled) { m_parent = state().activeSafety; state().activeSafety = this; }
		}
		~SafetyTrace() { if (m_enabled) state().activeSafety = m_parent; }
		void predict(bool unreachable) { m_predictedUnreachable = unreachable; }
		bool predictedUnreachable() const { return m_predictedUnreachable; }
		void query(const void* plot, int flags)
		{
			if (++m_queries[std::make_pair(plot, flags)] > 1) count(SAFETY_PATH_DUPLICATES, 1);
		}
	private:
		SafetyTrace(const SafetyTrace&);
		SafetyTrace& operator=(const SafetyTrace&);
	};
	inline void safetyQuery(const void* plot, int flags)
	{
		if (!enabled()) return;
		count(SAFETY_PATH_REQUESTS, 1);
		if (state().activeSafety) state().activeSafety->query(plot, flags);
	}

	inline void safetyPredict(bool unreachable)
	{
		if (enabled() && state().activeSafety) state().activeSafety->predict(unreachable);
	}
	inline bool safetyUnexpectedReach()
	{
		return enabled() && state().activeSafety && state().activeSafety->predictedUnreachable();
	}

	inline int activeRole() { return state().active ? state().active->role() : -1; }
}
#define AI_PERF_CONCAT_IMPL(a,b) a##b
#define AI_PERF_CONCAT(a,b) AI_PERF_CONCAT_IMPL(a,b)
#define AI_PERF_SCOPE(scope) AIPerf::Timer AI_PERF_CONCAT(aiPerfTimer_,__LINE__)(AIPerf::scope)
#define AI_PERF_UNIT_SCOPE(role) AIPerf::Timer AI_PERF_CONCAT(aiPerfUnitTimer_,__LINE__)(AIPerf::UNIT_DECISION, role)
#define AI_PERF_COUNT(counter, amount) AIPerf::count(AIPerf::counter, amount)
#define AI_PERF_UNIT_DECISION(owner, unit) AIPerf::unitDecision(owner, unit)
#define AI_PERF_CONTEXT(owner,unit) AIPerf::Context AI_PERF_CONCAT(aiPerfContext_,__LINE__)(owner,unit)
#define AI_PERF_UNIT_STATE(owner,unit,x,y,moves,mission) AIPerf::unitDecision(owner,unit,x,y,moves,mission)
#define AI_PERF_SAFETY_TRACE() AIPerf::SafetyTrace AI_PERF_CONCAT(aiPerfSafety_,__LINE__)
#define AI_PERF_SAFETY_QUERY(plot, flags) AIPerf::safetyQuery(plot, flags)
#ifdef C2C_AI_SAFETY_SHADOW
#define AI_PERF_SAFETY_PREDICT(unreachable) AIPerf::safetyPredict(unreachable)
#else
#define AI_PERF_SAFETY_PREDICT(unreachable) do {} while (0)
#endif
#define AI_PERF_FLUSH(turn) AIPerf::flush(turn)
#define AI_PERF_RESET() AIPerf::reset()
#else
#define AI_PERF_SCOPE(scope)
#define AI_PERF_UNIT_SCOPE(role)
#define AI_PERF_COUNT(counter, amount) do {} while (0)
#define AI_PERF_UNIT_DECISION(owner, unit)
#define AI_PERF_CONTEXT(owner,unit)
#define AI_PERF_UNIT_STATE(owner,unit,x,y,moves,mission)
#define AI_PERF_SAFETY_TRACE()
#define AI_PERF_SAFETY_QUERY(plot, flags) do {} while (0)
#define AI_PERF_SAFETY_PREDICT(unreachable) do {} while (0)
#define AI_PERF_FLUSH(turn)
#define AI_PERF_RESET()
#endif
