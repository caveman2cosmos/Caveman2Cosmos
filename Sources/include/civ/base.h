#pragma once

struct Virtual // Size: 4B, start: 0B, end: 4B
{
    void* __vptr;
};

struct NiRefObject : Virtual // Size: 4B, start: 4B, end: 8B
{
    unsigned int m_uiRefCount;
};

struct NiObject : NiRefObject // Size: 0B, start: 8B, end: 8B
{
};

struct NiFixedString
{
    char* m_kHandle; // NiGlobalStringTable::GlobalStringHandle
};

struct NiPointer_anon
{
    void* m_pObject;
};

#define DECLARE_NiPointer(T, Name) \
struct Name \
{ \
    T* m_pObject; \
};

struct NiExtraData : NiObject
{
    NiFixedString m_kName;
};

struct NiObjectNET : NiObject // Size: 16B, start: 8B, end: 24B
{
    NiFixedString m_kName; // 4B
    NiPointer_anon m_spControllers; // 4B, NiTimeControllerPtr

    NiExtraData** m_ppkExtra; // 4B
    unsigned short m_usExtraDataSize; // 2B
    unsigned short m_usMaxSize;  // 2B, number of slots in array
};

//struct NiColor
//{
//    float r;
//    float g;
//    float b;
//};