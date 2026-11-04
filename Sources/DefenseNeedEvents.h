#pragma once
// Pure notifications: no native AI query, allocation, RNG, or log output.
// Broader invalidation is conservative and covers radius-two garrisons too.
namespace DefenseNeedEvents {
inline unsigned __int64& revision(){static unsigned __int64 value=1;return value;}
inline int& suppression(){static int value=0;return value;}
inline bool suppressed(){return suppression()!=0;}
class IgnoreTemporary {
    bool m_active;
    IgnoreTemporary(const IgnoreTemporary&);IgnoreTemporary& operator=(const IgnoreTemporary&);
public:
    explicit IgnoreTemporary(bool active):m_active(active){if(m_active)++suppression();}
    ~IgnoreTemporary(){if(m_active)--suppression();}
};
inline void changed(){if(!suppressed())++revision();}
}
