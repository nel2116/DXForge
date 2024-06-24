#include "inc_BRDF.hlsli"

struct VS_INPUT
{
    float3 pos : POSITION; // 位置
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // テクスチャ座標
    float3 tangent : TANGENT; // 接線
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // 位置
    float2 uv : TEXCOORD; // テクスチャ座標
    float3 WorldPos : WORLD_POS; // ワールド空間での座標
    float3x3 InvTangentBasis : INV_TANGENT_BASIS; // 接線空間への基底変換行列の逆行列
};

cbuffer Transform : register(b0)
{
    float4x4 View : packoffset(c0); // ビュー行列
    float4x4 Proj : packoffset(c4); // プロジェクション行列
};

cbuffer CbMesh : register(b1)
{
    float4x4 World : packoffset(c0); // ワールド行列です.
};

cbuffer LightBuff : register(b2)
{
    float3 ligColor : packoffset(c0); // ライトの色
    float ligIntensity : packoffset(c0.w); // ライトの強度
    float3 ligForward : packoffset(c1); // ライトの向き
};

cbuffer CameraBuff : register(b3)
{
    float3 CameraPos : packoffset(c0); // カメラの位置
};