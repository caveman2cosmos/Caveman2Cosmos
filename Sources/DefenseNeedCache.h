#pragma once
#include <map>
#include <set>
#include <algorithm>

// Unknown is absence, never a stored zero. Events invalidate observations;
// pending departures remain a separate input to each native predicate query.
class DefenseNeedCache
{
    struct Key {
        int values[5];
        bool operator<(const Key& b) const
        {return std::lexicographical_compare(values,values+5,b.values,b.values+5);}
    };
    std::map<Key,int> m_values;
    unsigned __int64 m_revision;
    int m_turn,m_map;
public:
    DefenseNeedCache():m_revision(0),m_turn(-1),m_map(-1){}
    void reset(){m_values.clear();m_revision=0;m_turn=m_map=-1;}
    void synchronize(unsigned __int64 revision,int turn,int map)
    {
        // Native strategic inputs are not all event driven. A conservative
        // turn boundary is required until that event coverage is complete.
        if(revision!=m_revision || turn!=m_turn || map!=m_map)
        {m_values.clear();m_revision=revision;m_turn=turn;m_map=map;}
    }
    bool lookup(int x,int y,int kind,int extra,int any,int& value) const
    {
        const Key k={{x,y,kind,extra,any}};
        std::map<Key,int>::const_iterator i=m_values.find(k);
        if(i==m_values.end())return false;
        value=i->second;return true;
    }
    void remember(int x,int y,int kind,int extra,int any,int value)
    {
        // A bounded transient cache; eviction can only lose speed.
        if(m_values.size()>=8192)m_values.clear();
        const Key k={{x,y,kind,extra,any}};m_values[k]=value;
    }
};

// Group membership is the authority. Replacing a snapshot subtracts the old
// contribution before adding the new one, including partial arrival/death.
class DefenseCommitments
{
    struct Entry {int x,y,count,strength,happy;Entry(int a=0,int b=0,int n=0,int s=0,int h=0):x(a),y(b),count(n),strength(s),happy(h){}};
    std::map<int,Entry> m_groups;
    std::map<std::pair<int,int>,int> m_incoming;
    std::map<std::pair<int,int>,int> m_strength,m_happy;
public:
    bool ready;
    int map,turn,auditReads;
    unsigned __int64 revision;
    std::set<int> dirty;
    DefenseCommitments():ready(false),map(-1),turn(-1),auditReads(0),revision(0){}
    void reset(){m_groups.clear();m_incoming.clear();m_strength.clear();m_happy.clear();dirty.clear();ready=false;map=turn=-1;auditReads=0;revision=0;}
    void changed(int id,bool relevant=true){if(ready&&(relevant||m_groups.find(id)!=m_groups.end()))dirty.insert(id);}
    void dirtyCommitted(){for(std::map<int,Entry>::const_iterator i=m_groups.begin();i!=m_groups.end();++i)dirty.insert(i->first);}
    void replace(int id,int x,int y,int count,int strength=0,int happy=0)
    {
        std::map<int,Entry>::iterator old=m_groups.find(id);
        if(old!=m_groups.end())
        {
            const std::pair<int,int> key(old->second.x,old->second.y);
            m_incoming[key]-=old->second.count;
            m_strength[key]-=old->second.strength;m_happy[key]-=old->second.happy;
            if(!m_incoming[key])m_incoming.erase(key);
            if(!m_strength[key])m_strength.erase(key);
            if(!m_happy[key])m_happy.erase(key);
            m_groups.erase(old);
        }
        if(count>0){m_groups.insert(std::make_pair(id,Entry(x,y,count,strength,happy)));const std::pair<int,int> key(x,y);m_incoming[key]+=count;if(strength)m_strength[key]+=strength;if(happy)m_happy[key]+=happy;}
    }
    int incoming(int x,int y)const
    {
        std::map<std::pair<int,int>,int>::const_iterator i=m_incoming.find(std::make_pair(x,y));
        return i==m_incoming.end()?0:i->second;
    }
    int strength(int x,int y)const{std::map<std::pair<int,int>,int>::const_iterator i=m_strength.find(std::make_pair(x,y));return i==m_strength.end()?0:i->second;}
    int happy(int x,int y)const{std::map<std::pair<int,int>,int>::const_iterator i=m_happy.find(std::make_pair(x,y));return i==m_happy.end()?0:i->second;}
    bool sameContents(const DefenseCommitments& other)const{return m_incoming==other.m_incoming&&m_strength==other.m_strength&&m_happy==other.m_happy;}
};

// This is an AI search budget, not a claim that reachability cannot change.
// A failed fort candidate is reconsidered next turn or after its own state
// changes; another advertiser alone does not repeat every old failure.
class DefenseFortRetries
{
    struct Key {
        int v[10];
        bool operator<(const Key& b)const{return std::lexicographical_compare(v,v+10,b.v,b.v+10);}
    };
    std::set<Key> m_failed;
    int m_turn,m_map;
public:
    DefenseFortRetries():m_turn(-1),m_map(-1){}
    void reset(){m_failed.clear();m_turn=m_map=-1;}
    void synchronize(int turn,int map){if(turn!=m_turn||map!=m_map){reset();m_turn=turn;m_map=map;}}
    template<class Unit,class Plot> bool failed(const Unit* u,const Plot* p,int flags,int limit)const
    {
        const Key k={{u->getID(),u->getGameTurnCreated(),u->getGroup()->getID(),u->getX(),u->getY(),u->getMoves(),p->getX(),p->getY(),flags,limit}};
        return m_failed.find(k)!=m_failed.end();
    }
    template<class Unit,class Plot> void remember(const Unit* u,const Plot* p,int flags,int limit)
    {
        const Key k={{u->getID(),u->getGameTurnCreated(),u->getGroup()->getID(),u->getX(),u->getY(),u->getMoves(),p->getX(),p->getY(),flags,limit}};
        // Budget storage cannot grow without bound on enormous maps.
        if(m_failed.size()<262144)m_failed.insert(k);
    }
};
