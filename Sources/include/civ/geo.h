#pragma once

//struct NiPoint3
//{
//    float x;
//    float y;
//    float z;
//};

struct NiBound // 0:16:16
{
    // data specific to bounding spheres
    NiPoint3 m_kCenter; // 0:12:12
    float m_fRadius; // 12:4:16
}; // NI_DATA_ALIGNMENT(16);

struct NiMatrix3
{
    float m_pEntry[3][3];
};

struct NiTransform
{
    NiMatrix3 m_Rotate;
    NiPoint3 m_Translate;
    float m_fScale;
};

struct NiRect_float 
{
    float m_left;
    float m_right;
    float m_top;
    float m_bottom;
};
