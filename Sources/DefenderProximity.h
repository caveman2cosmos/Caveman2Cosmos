#pragma once
// Invocation-local ordering hint, never a permission or movement bound.
namespace DefenderProximity {
 struct Key {
  int band,distance,order;
  Key(bool local,bool connected,int d,int o):band(local?0:(connected?1:2)),distance(d),order(o){}
  bool operator<(const Key& k)const {
   if(band!=k.band)return band<k.band;
   if(distance!=k.distance)return distance<k.distance;
   return order<k.order;
  }
  // First try the nearest connected distance, then the next. Unknown trade
  // connections retain the old suitability scorer across all fallback units.
  bool sameBand(const Key& k)const{return band==k.band&&(band!=1||distance==k.distance);}
 };
 struct Candidate {
  int index;Key key;
  Candidate(int i,const Key& k):index(i),key(k){}
  bool operator<(const Candidate& c)const{return key<c.key;}
 };
}
