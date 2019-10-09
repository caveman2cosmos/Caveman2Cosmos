#pragma once

enum D3DLIGHTTYPE
{
    D3DLIGHT_POINT = 1,
    D3DLIGHT_SPOT = 2,
    D3DLIGHT_DIRECTIONAL = 3,
    D3DLIGHT_FORCE_DWORD = 0x7fffffff
};

typedef struct _D3DCOLORVALUE // 16b
{
    float r;
    float g;
    float b;
    float a;
} D3DCOLORVALUE;

typedef struct D3DVECTOR // 12b
{
    float x;
    float y;
    float z;
} D3DVECTOR;

// 104b, 0x68
typedef struct D3DLIGHT9
{
    D3DLIGHTTYPE Type;              // 0b
    D3DCOLORVALUE Diffuse;          // 4b
    D3DCOLORVALUE Specular;         // 20b
    D3DCOLORVALUE Ambient;          // 36b
    D3DVECTOR Position;             // 52b
    D3DVECTOR Direction;            // 64b
    float Range;                    // 76b
    float Falloff;                  // 80b
    float Attenuation0;             // 84b
    float Attenuation1;             // 88b
    float Attenuation2;             // 92b
    float Theta;                    // 96b
    float Phi;                      // 100b
} D3DLIGHT9, *LPD3DLIGHT;           // 104b
