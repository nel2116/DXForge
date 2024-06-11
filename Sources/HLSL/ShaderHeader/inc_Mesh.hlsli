struct VSInput
{
    float3 pos : POSITION; // 頂点座標
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // UV座標
    float3 tangent : TANGENT; // 接線
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer Transform : register(b0)
{
    float4x4 World : packoffset(c0); // ワールド行列
    float4x4 View : packoffset(c4); // ビュー行列
    float4x4 Proj : packoffset(c8); // プロジェクション行列
};