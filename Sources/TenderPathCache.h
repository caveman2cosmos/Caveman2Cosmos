#pragma once
#include <map>

// Only the length/reachability of hypothetical bids, never a live unit path.
// Owner, map and flags are fixed by one finalizeTenderContracts() invocation.
// Discard the cache on return: no cross-turn invalidation or save data needed.
class TenderPathCache
{
 struct Key
 {
  int from, to, unit, limit;
  Key(int f,int t,int u,int l):from(f),to(t),unit(u),limit(l){}
  bool operator<(const Key& rhs)const
  {
   if(from!=rhs.from)return from<rhs.from;
   if(to!=rhs.to)return to<rhs.to;
   if(unit!=rhs.unit)return unit<rhs.unit;
   return limit<rhs.limit;
  }
 };
 std::map<Key,int> m_lengths; // -1 unreachable; zero is a valid length.
public:
 bool lookup(int from,int to,int unit,int limit,int& length)const
 {
  std::map<Key,int>::const_iterator it=m_lengths.find(Key(from,to,unit,limit));
  if(it==m_lengths.end())return false;
  length=it->second;return true;
 }
 void remember(int from,int to,int unit,int limit,int length)
 {
  // Bound transient memory in very large empires; eviction only loses speed.
  if(m_lengths.size()>=4096)m_lengths.clear();
  m_lengths[Key(from,to,unit,limit)]=length;
 }
};
