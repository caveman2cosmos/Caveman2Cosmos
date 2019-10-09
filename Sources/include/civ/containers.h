#pragma once

#define DECLARE_NiTPointerList(T, PTR, Name) \
struct NiTListItem_##T \
{ \
    NiTListItem_##T* m_pkNext; \
    NiTListItem_##T* m_pkPrev; \
    T PTR m_element; \
}; \
struct Name : Virtual \
{ \
    NiTListItem_##T* m_pkHead; \
    NiTListItem_##T* m_pkTail; \
    /* allocator is empty */ \
    unsigned int m_uiCount; \
};
// #define NiTPointerList(T) NiTPointerList_##T

#define DECLARE_NiTPointerMap(TKEY, TKEYPTR, TVAL, TVALPTR, Name) \
struct NiTMapItem_##TKEY##_##TVAL \
{ \
    NiTMapItem_##TKEY##_##TVAL* m_pkNext; \
    TKEY TKEYPTR m_key; \
    TVAL TVALPTR m_val; \
}; \
struct Name : Virtual \
{ \
    /* maximum slots in hash table */ \
    unsigned int m_uiHashSize; \
    NiTMapItem_##TKEY##_##TVAL** m_ppkHashTable; /* NiTMapItem<TKEY,TVAL> */ \
    /* empty allocator here */ \
    unsigned int m_uiCount; \
};
// #define NiTPointerMap(TKEY, TVAL) NiTPointerMap_##TKEY##_##TVAL
