#pragma once
class CvSelectionGroup;
// Reuse a preflight only within this read-only planning call, including its
// restricted search and fallback. Never retain results across moves/queries.
class PathDestinationCheck
{
 typedef bool (*Validator)(const CvSelectionGroup*,int,int,int,bool&);
 const void* generator;const CvSelectionGroup* group;int x,y,flags;
 bool ready,allowed,war;PathDestinationCheck* previous;
 static PathDestinationCheck*& current(){static PathDestinationCheck* c=NULL;return c;}
public:
 PathDestinationCheck(const void* owner,const CvSelectionGroup* g,int tx,int ty,int f,Validator validate)
  :generator(owner),group(g),x(tx),y(ty),flags(f),ready(false),allowed(false),war(false),previous(current())
 {current()=this;allowed=validate(group,x,y,flags,war);ready=true;}
 ~PathDestinationCheck(){current()=previous;}
 bool valid()const{return allowed;}
 bool requiresWar()const{return war;}
 static bool validate(const void* owner,const CvSelectionGroup* g,int tx,int ty,int f,Validator callback,bool& requiresWar)
 {
  const PathDestinationCheck* c=current();
  if(c&&c->ready&&c->generator==owner&&c->group==g&&c->x==tx&&c->y==ty&&c->flags==f)
  {requiresWar=c->war;return c->allowed;}
  return callback(g,tx,ty,f,requiresWar);
 }
private:
 PathDestinationCheck(const PathDestinationCheck&);PathDestinationCheck& operator=(const PathDestinationCheck&);
};
