#pragma once

#include "d3d.h"
#include "base.h"
#include "geo.h"
#include "containers.h"

// // 128b (0x80). allocated as 116b (0x74)
// struct light
// {
//     D3DLIGHT9 d3d_light;
//     // D3DLIGHTTYPE d3d_type;
//     // D3DCOLORVALUE d3d_diffuse; // maybe
//     // D3DCOLORVALUE d3d_specular; // maybe
//     // D3DCOLORVALUE d3d_ambient;
//     // D3DVECTOR position;
//     // D3DVECTOR direction; // maybe
//     // unsigned int u3[7];
//     unsigned int unknown_index1;    // 104b
//     unsigned int d3d_index;         // 108b
//     unsigned int u4[2];             // 112b
//     void *p2[2];                    // 120b
// };                                  // 128b

struct NiProperty : NiObjectNET
{
};

DECLARE_NiPointer(NiProperty, NiPropertyPtr);
DECLARE_NiTPointerList(NiPropertyPtr,, NiPropertyList);

struct NiAVObject : NiObjectNET // 24:150:174
{
    unsigned char _padding[3]; // 24:3:27
    bool _bool1; // 27:1:28

    // parent pointer
    void* m_pkParent; // 28:4:32, NiNode

    // bound
    NiBound m_kWorldBound; // 32:16:48

    NiTransform m_kLocal; // 48:52:100
    NiTransform m_kWorld; // 100:52:152

    // NOTE: Variable declarations whose order effects assembly language code
    // end here.
    // properties
    NiPropertyList m_kPropertyList; // 152:16:168, NiPropertyList (NiTPointerList<NiPropertyPtr>)

    NiPointer_anon m_spCollisionObject; // 168:4:172, NiCollisionObjectPtr

    // unsigned char _padding2[2]; // 172:2:174

    // unsigned short m_uFlags; // 174:2:176 No flags used?
};

enum NiDynamicEffect_EffectType
{
    AMBIENT_LIGHT = 0,
    POINT_LIGHT = 2,
    DIR_LIGHT = 3,
    SPOT_LIGHT = 4,

    SHADOWPOINT_LIGHT = 5,
    SHADOWDIR_LIGHT = 6,
    SHADOWSPOT_LIGHT = 7,
    
    TEXTURE_EFFECT = 8,
    MAX_TYPES = TEXTURE_EFFECT + 1,

    // Note - all vertex-based lights types _must_ come before
    // the texture effects.  This allows the enum below to be
    // used for fast type checking in the low-level renderers, i.e.
    // (pE->GetEffectType() <= MAX_LIGHT_TYPE) <---> IsKindOf(NiLight, pS)
    MAX_LIGHT_TYPE = SHADOWSPOT_LIGHT,

    // Light index in provided so a NiDynamicEffect object and quickly 
    // update it's effect type when a NiShadowGenerator object is applied.
    LIGHT_INDEX = POINT_LIGHT,
    SHADOWLIGHT_INDEX = SHADOWPOINT_LIGHT,
    SHADOW_INDEX_DIFF = SHADOWPOINT_LIGHT - POINT_LIGHT
};

struct NiNode
{

};

DECLARE_NiTPointerList(NiNode, *, NiNodeList)

struct NiDynamicEffect : NiAVObject // 174:20:194
{
    bool m_bOn; // 174:1:175
    unsigned char _padding[3]; // 175:3:178
    unsigned int m_ucEffectType; // 178:4:182

    // bookkeeping
    // Not sure about next two
    int m_iIndex; // 182:4:186
    unsigned int m_uiPushCount; // 186:4:190
    // unsigned int m_uiRevID;
    // static int ms_iNextIndex;

    // void* m_pkShadowGenerator; // NiShadowGenerator

    // illuminated objects
    NiNodeList m_kAffectedNodeList; // 186:4:190
    // unaffected objects
    NiNodeList m_kUnaffectedNodeList; // 190:4:194
};


struct NiDynamicEffectState_ListItem
{
    NiDynamicEffectState_ListItem* m_pNext;
    NiDynamicEffect* m_pItem;
};

struct NiDynamicEffectState : NiRefObject
{
    bool m_bMultiTexture;
    NiDynamicEffectState_ListItem* m_pLightList;
    NiDynamicEffectState_ListItem* m_pProjLightList;
    NiDynamicEffectState_ListItem* m_pProjShadowList;
    void* m_pEnvMap; // NiTextureEffect
    void* m_pFogMap; // NiTextureEffect
};

struct NiDX9LightManager_LightEntry
{
    D3DLIGHT9 m_kD3DLight;
    unsigned int m_uiRevID;
    int m_iIndex;
    bool m_bNeedsRemoval;
    bool m_bActive;
};

struct NiLight : NiDynamicEffect // 194::
{
    float m_fDimmer; // 194:4:198
    NiColor m_kAmb; // 198:12:210
    NiColor m_kDiff; // 210:12:222
    NiColor m_kSpec; // 222:12:234
    NiDX9LightManager_LightEntry* m_pvRendererData; // 234:4:236f
};

DECLARE_NiTPointerMap(NiLight, *, NiDX9LightManager_LightEntry, *, LightPointerMap)
DECLARE_NiTPointerList(NiLight, *, LightPointerList)

// Since some cards only support 0-4095 for indices, we need to track 
// the valid indices. This will prevent overflows when loading level 
// after level. 
enum
{
    LIGHTINDEXARRAYSIZE = 32
};

struct NiDX9LightManager
{
    LightPointerMap m_kLights;
    LightPointerList m_kDeviceLights;

    void* m_pkD3DDevice9; // LPDIRECT3DDEVICE9
    void* m_pkRenderState; // NiDX9RenderState

    NiDynamicEffectState* m_pkInvalidState;
    NiDynamicEffectState* m_pkLastState;

    unsigned short _padding;

    bool m_bLastAmbientDiffuse;

    // Vertex Color Members
    bool m_bAmbientDiffuse;

    unsigned int m_eApply; // enum NiTexturingProperty::ApplyMode

    unsigned int m_eSourceMode; // enum NiVertexColorProperty::SourceVertexMode

    unsigned int m_uiNextAvailableIndex;
    unsigned int m_auiValidIndices[LIGHTINDEXARRAYSIZE];
};

struct NiDX9Renderer : /* NiRenderer : */ NiObject
{
    NiPointer_anon m_spAccum; // NiAccumulatorPtr
    void* m_pkCurrProp; // NiPropertyState
    NiDynamicEffectState* m_pkCurrEffects;

    // Default material
    NiPointer_anon m_spInitialDefaultMaterial; // NiMaterialPtr
    NiPointer_anon m_spCurrentDefaultMaterial; // NiMaterialPtr

    // Error shader
    NiPointer_anon m_spErrorShader; // NiShaderPtr

    // Safe zone rectangle
    NiRect_float m_kDisplaySafeZone; // NiRect<float>

            // // Thread synchronization primitives
            // NiCriticalSection m_kRendererLock;
            // NiCriticalSection m_kPrecacheCriticalSection; // requires m_kRendererLock

            // NiCriticalSection m_kSourceDataCriticalSection;

            // // Render target group bracketing
            // bool m_bRenderTargetGroupActive;

            // // Frame and batch rendering bracketing
            // EFrameState m_eFrameState;
            // EFrameState m_eSavedFrameState;
            // unsigned int m_uiFrameID;
            // bool m_bBatchRendering;

    unsigned char _padding[1684];

    // unsigned char _padding[1];
    // void* m_pkD3DDevice9; // LPDIRECT3DDEVICE9
    // unsigned char _padding2[2];

    // // Managers
            // void* m_pkRenderState; // NiDX9RenderState
            // void* m_pkVBManager; // NiDX9VertexBufferManager
            // void* m_pkIBManager; // NiDX9IndexBufferManager
            // void* m_pkTextureManager; // NiDX9TextureManager
    NiDX9LightManager* m_pkLightManager;
    unsigned char _padding2[2048];
};

struct NiD3DShader : /* NiD3DShaderInterface : NiShader : */ NiRefObject 
{
    unsigned char _padding[8];
    void* m_pkD3DDevice; // LPDIRECT3DDEVICE9 / D3DDevicePtr
    NiDX9Renderer* m_pkD3DRenderer;
    void* m_pkD3DRenderState; // NiDX9RenderState
    unsigned char _padding2[256];
};

// struct unknown_light_owner
// {
//     void *p1_low;
//     unsigned int unknown_count1; // ex: 1
//     void *p2;
//     unsigned int u1[3];
//     unsigned int unknown_count2; // ex: 16
//     void *p3;
//     unsigned int u2[4];
//     float f1;
//     unsigned int u3;
//     float f2[7];
//     unsigned int u4[3];
//     float f3;    // ex: 1, scale maybe?
//     float f4[9]; // rotation matrix maybe?
//     D3DVECTOR position;
//     float f5; // ex: 0.25, scale maybe?
//     void *p4_low;
//     unsigned int u5[4];
//     float f6; // ex: -1.0000001192092896
//     unsigned int unknown_count3; // ex: 7,678
//     unsigned int u6;
//     unsigned int unknown_index1; // ex: 15, 6
//     void *p5_low;
//     void *p6;
//     void *p7; // same value as p6
//     unsigned int unknown_count4; // ex: 1
//     void *p8_low;
//     unsigned int u7[3];
//     float f7;
//     unsigned int u8[3];
//     float f8[6]; // all 1
//     light *pLight;
//     D3DVECTOR direction; // maybe
//     unsigned int u9;
//     unsigned int unknown_count5; // ex: 0, 1
//     float f10; // ex: -1, 0x55555555
//     void *p10;
//     void *p11;
// };

// struct s3 {
//     signed char[120] pad120;
//     int32_t f120;
// };

// struct s2 {
//     struct s3* f0;
//     signed char[100] pad104;
//     int32_t f104;
//     int32_t f108;
//     signed char f112;
//     signed char f113;
//     signed char[58] pad172;
//     signed char f172;
//     signed char[87] pad260;
//     struct s2* f260;
// };

// struct s5 {
//     signed char[108] pad108;
//     int32_t f108;
//     signed char f112;
//     signed char f113;
// };

// struct s4 {
//     signed char[260] pad260;
//     struct s5* f260;
// };

// struct s1 {
//     struct s1* f0;
//     struct s2* f4;
//     struct s4* f8;
//     struct s1* f12;
// };

// struct s6 {
//     signed char[204] pad204;
//     int32_t f204;
//     signed char[4] pad212;
//     int32_t f212;
// };

// struct s8 {
//     signed char[228] pad228;
//     int32_t f228;
// };

// struct s7 {
//     signed char[844] pad844;
//     uint32_t f844;
//     signed char[3496] pad4344;
//     struct s8** f4344;
// };

// struct s0 {
//     signed char[20] pad20;
//     struct s1* f20;
//     signed char[4] pad28;
//     uint32_t f28;
//     struct s6** f32;
//     struct s7* f36;
//     signed char[9] pad49;
//     signed char f49;
// };
