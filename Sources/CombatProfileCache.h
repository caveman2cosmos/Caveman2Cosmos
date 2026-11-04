#pragma once
#include <map>
#include <vector>
#include <utility>
#include <cstdlib>

// Transient per-unit results, not promotions or serialized game state. A profile
// is the complete ordered set of an opponent's combat classes, not just Archer.
namespace CombatProfileCache {
 enum { MAX_SIGNATURES=4096, SMALL_SLOTS=4, LARGE_SLOTS=16 };
#ifdef COMBAT_PROFILE_CACHE_TEST
 extern int testMode;
#endif
#ifdef C2C_RECALCULATED_BENCHMARK
 inline int environmentMode(){const char*p=getenv("C2C_COMBAT_PROFILE_MODE");return p?atoi(p):3;}
#endif
 inline int mode(){
#ifdef COMBAT_PROFILE_CACHE_TEST
  return testMode;
#elif defined(C2C_RECALCULATED_BENCHMARK)
#ifdef C2C_COMBAT_PROFILE_FIXED_MODE
  // Final timing uses the ordinary build's constant-folded cache path.
  return C2C_COMBAT_PROFILE_FIXED_MODE;
#else
  static const int selected=environmentMode();return selected;
#endif
#else
  return 3; // Bounded profiles, bitmap membership and keyed neutral strength.
#endif
 }
 struct Stats {
  unsigned long long queries,hits,misses,classesBuilt,bitQueries,bitHits,bitBuilds,neutralHits,neutralMisses,bad,audits,liveUnits,liveBytes,peakBytes;
  Stats():queries(0),hits(0),misses(0),classesBuilt(0),bitQueries(0),bitHits(0),bitBuilds(0),neutralHits(0),neutralMisses(0),bad(0),audits(0),liveUnits(0),liveBytes(0),peakBytes(0){}
 };
 inline Stats&stats(){static Stats s;return s;}
 inline bool audit(){
#if defined(C2C_RECALCULATED_BENCHMARK) && !defined(C2C_COMBAT_PROFILE_FIXED_MODE)
  static const char*p=getenv("C2C_COMBAT_PROFILE_AUDIT");static unsigned serial=0;return p&&p[0]=='1'&&((++serial&63)==0);
#else
  return false;
#endif
 }
 inline void memory(long delta){
#if defined(C2C_RECALCULATED_BENCHMARK)||defined(COMBAT_PROFILE_CACHE_TEST)
  Stats&s=stats();if(delta>=0)s.liveBytes+=delta;else s.liveBytes-=unsigned(-delta);if(s.liveBytes>s.peakBytes)s.peakBytes=s.liveBytes;
#endif
 }
 struct Registry {
  std::map<std::vector<int>,unsigned> ids;
  unsigned intern(const std::vector<int>&classes){
   std::map<std::vector<int>,unsigned>::const_iterator i=ids.find(classes);if(i!=ids.end())return i->second;
   if(ids.size()>=MAX_SIGNATURES)return 0;const unsigned id=unsigned(ids.size()+1);ids.insert(std::make_pair(classes,id));return id;
  }
 };
 inline Registry&registry(){static Registry r;return r;}
 struct Entry {unsigned signature;int total,extra;Entry():signature(0),total(0),extra(0){}};
 class Unit {
  Unit(const Unit&);Unit&operator=(const Unit&);
  bool classesReady,bitsReady,neutralReady;unsigned signature;int neutralBase,neutralModifier,neutralValue;
  std::vector<int> classes;std::vector<unsigned> bits;std::vector<Entry> rows;
 public:
  Unit():classesReady(false),bitsReady(false),neutralReady(false),signature(0),neutralBase(0),neutralModifier(0),neutralValue(0){memory(sizeof(Unit));
#if defined(C2C_RECALCULATED_BENCHMARK)||defined(COMBAT_PROFILE_CACHE_TEST)
   ++stats().liveUnits;
#endif
  }
  ~Unit(){memory(-long(bytes()));
#if defined(C2C_RECALCULATED_BENCHMARK)||defined(COMBAT_PROFILE_CACHE_TEST)
   --stats().liveUnits;
#endif
  }
  size_t bytes()const{return sizeof(Unit)+classes.capacity()*sizeof(int)+bits.capacity()*sizeof(unsigned)+rows.capacity()*sizeof(Entry);}
  void invalidate(bool membership){for(size_t i=0;i<rows.size();++i)rows[i].signature=0;neutralReady=false;if(membership){classesReady=false;bitsReady=false;signature=0;}}
  bool ready()const{return classesReady;}
  const std::vector<int>&getClasses()const{return classes;}
  unsigned getSignature()const{return signature;}
  void setClasses(const std::vector<int>&values){size_t old=bytes();classes=values;signature=registry().intern(classes);classesReady=true;bitsReady=false;memory(long(bytes())-long(old));}
  const Entry*find(unsigned id)const{if(!id||rows.empty())return NULL;const Entry&e=rows[id&(rows.size()-1)];return e.signature==id?&e:NULL;}
  void store(unsigned id,int total,int extra){if(!id)return;size_t old=bytes();if(rows.empty())rows.resize(mode()==1?SMALL_SLOTS:LARGE_SLOTS);Entry&e=rows[id&(rows.size()-1)];e.signature=id;e.total=total;e.extra=extra;memory(long(bytes())-long(old));}
  bool hasReady()const{return bitsReady;}
  // Keep this ready-path small enough to inline into the common membership query.
  // Invalidating membership clears bitsReady before any class changes are used.
  bool hasCached(int type)const{return type>=0 && size_t(unsigned(type)>>5)<bits.size() && (bits[unsigned(type)>>5]&(1U<<(type&31)))!=0;}
  bool has(int type){if(type<0)return false;if(!bitsReady){size_t old=bytes();bits.assign(classes.empty()?0:size_t(classes.back()/32+1),0);for(size_t i=0;i<classes.size();++i)bits[classes[i]/32]|=1U<<(classes[i]&31);bitsReady=true;memory(long(bytes())-long(old));}return hasCached(type);}
  bool neutral(int base,int modifier,int&value)const{if(!neutralReady||base!=neutralBase||modifier!=neutralModifier)return false;value=neutralValue;return true;}
  void setNeutral(int base,int modifier,int value){neutralBase=base;neutralModifier=modifier;neutralValue=value;neutralReady=true;}
 };
}
#if (defined(C2C_RECALCULATED_BENCHMARK) && !defined(C2C_COMBAT_PROFILE_FIXED_MODE))||defined(COMBAT_PROFILE_CACHE_TEST)
#define CPC_COUNT(name) (++CombatProfileCache::stats().name)
#else
#define CPC_COUNT(name) ((void)0)
#endif
