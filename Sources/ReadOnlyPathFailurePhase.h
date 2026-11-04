#pragma once
#include "ReadOnlyPathFailures.h"

// Consecutive unsuccessful contract queries do not move/regroup units. Keep
// their failed paths until work is handed back for execution. A successful
// query invalidates every enclosing phase BEFORE any order can mutate state.
class ReadOnlyPathFailurePhase
{
    ReadOnlyPathFailurePhase*& m_slot;
    ReadOnlyPathFailurePhase* m_previous;
    ReadOnlyPathFailures m_failures;
    bool m_readOnly;
public:
    explicit ReadOnlyPathFailurePhase(ReadOnlyPathFailurePhase*& slot)
        :m_slot(slot),m_previous(slot),m_failures(8192),m_readOnly(true)
    {m_slot=this;}
    ~ReadOnlyPathFailurePhase(){m_slot=m_previous;}
    ReadOnlyPathFailures* paths(){return m_readOnly?&m_failures:NULL;}
    void beforeOrders()
    {
        for(ReadOnlyPathFailurePhase* phase=this;phase;phase=phase->m_previous)
        {phase->m_failures.clear();phase->m_readOnly=false;}
    }
    void completeCall()
    {
        if(!m_readOnly){m_failures.clear();m_readOnly=true;}
    }
private:
    ReadOnlyPathFailurePhase(const ReadOnlyPathFailurePhase&);
    ReadOnlyPathFailurePhase& operator=(const ReadOnlyPathFailurePhase&);
};
