#pragma once
#include <map>
// One best-attacker scan only. Predicted HP can change during odds evaluation;
// key it explicitly and never carry this cache into the next simulated battle.
template<class Unit,class Plot> class AttackSelectionStrengthCache {
    struct Entry {
        int hp,value,modifiers;bool strengthValid,modifiersValid;
        Entry():hp(0),value(0),modifiers(0),strengthValid(false),modifiersValid(false){}
    };
    const Plot* m_plot;bool m_enabled;
    std::map<const Unit*,Entry> m_values;
    unsigned m_hits,m_misses,m_modifierHits,m_modifierMisses;
public:
    AttackSelectionStrengthCache(const Plot* plot,bool enabled):m_plot(plot),m_enabled(enabled),m_hits(0),m_misses(0),m_modifierHits(0),m_modifierMisses(0){}
    int get(const Unit* unit){
        if(!m_enabled){++m_misses;return unit->currEffectiveStr(m_plot,unit);}
        const int hp=unit->getHP();
        Entry& entry=m_values[unit];
        if(entry.strengthValid&&entry.hp==hp){++m_hits;return entry.value;}
        const int value=unit->currEffectiveStr(m_plot,unit);
        entry.hp=hp;entry.value=value;entry.strengthValid=true;++m_misses;return value;
    }
    // Promotions and commander membership cannot change inside this read-only
    // candidate scan. Odds can change predicted HP, which does not enter this sum.
    int modifiers(const Unit* unit,int (*calculate)(const Unit*)){
        if(!m_enabled){++m_modifierMisses;return calculate(unit);}
        Entry& entry=m_values[unit];
        if(entry.modifiersValid){++m_modifierHits;return entry.modifiers;}
        entry.modifiers=calculate(unit);entry.modifiersValid=true;++m_modifierMisses;return entry.modifiers;
    }
    unsigned hits()const{return m_hits;}unsigned misses()const{return m_misses;}
    unsigned modifierHits()const{return m_modifierHits;}unsigned modifierMisses()const{return m_modifierMisses;}
};
