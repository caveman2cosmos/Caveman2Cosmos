#pragma once
#include <algorithm>

// Routine city support must not compete with emergency reinforcement or
// exhaust the map looking for a marginal improvement to city statistics.
namespace CitySupportSearch {
enum { MAX_TURNS=3, MAX_CANDIDATES=8, ROUTINE_PRIORITY=200, EMERGENCY_PRIORITY=500 };
template<class City> bool routine(const City* city,int owner)
{return city && city->getOwner()==owner && !city->AI_isDanger();}
inline int priority(bool routine,int requested)
{return routine?std::min(requested,(int)ROUTINE_PRIORITY):std::max(requested,(int)EMERGENCY_PRIORITY);}
inline int maxTurns(bool routine,int requested)
{return routine?(requested<0?MAX_TURNS:std::min(requested,(int)MAX_TURNS)):requested;}
inline bool maySearch(bool routine,int attempted)
{return !routine || attempted<MAX_CANDIDATES;}
}
