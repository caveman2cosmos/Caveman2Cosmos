#pragma once
#include <algorithm>
#include <vector>

// Keep the existing serialized vector; only its order is derived. XML/save
// remapping may change IDs, so normalize once after reading the complete list.
namespace DisabledBuildingSet
{
 inline void normalize(std::vector<short>& buildings)
 {std::sort(buildings.begin(),buildings.end());}
 inline bool contains(const std::vector<short>& buildings,short building)
 {return std::binary_search(buildings.begin(),buildings.end(),building);}
 inline void insert(std::vector<short>& buildings,short building)
 {
  std::vector<short>::iterator it=std::lower_bound(buildings.begin(),buildings.end(),building);
  if(it==buildings.end() || *it!=building)buildings.insert(it,building);
 }
 inline void remove(std::vector<short>& buildings,short building)
 {
  // Preserve legacy erase-one semantics, including duplicate old-save entries.
  std::vector<short>::iterator it=std::lower_bound(buildings.begin(),buildings.end(),building);
  if(it!=buildings.end() && *it==building)buildings.erase(it);
 }
 // Derived membership only; never serialized. One byte per building type in
 // cities that actually query it. Mutations update one byte after callbacks.
 class Lookup
 {
  mutable bool m_ready;
  mutable std::vector<unsigned char> m_flags;
 public:
  Lookup():m_ready(false){}
  void invalidate(){m_ready=false;m_flags.clear();}
  bool get(int count,const std::vector<short>& buildings,short building) const
  {
   // Preserve legacy membership for malformed/out-of-range IDs without indexing.
   if(building<0 || building>=count)return contains(buildings,building);
   if(!m_ready || (int)m_flags.size()!=count)
   {
    m_flags.assign(count,0);
    for(size_t i=0;i<buildings.size();++i)
     if(buildings[i]>=0 && buildings[i]<count)m_flags[buildings[i]]=1;
    m_ready=true;
   }
   return m_flags[building]!=0;
  }
  void update(short building,bool present)
  {
   if(m_ready && building>=0 && building<(int)m_flags.size())m_flags[building]=present ? 1 : 0;
  }
 };
}
