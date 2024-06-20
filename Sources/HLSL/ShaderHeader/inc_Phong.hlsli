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
    float3 normal : NORMAL; // 法線
    float2 uv : TEXCOORD; // テクスチャ座標
    float4 WorldPos : WORLD_POS; // ワールド空間での座標
};

cbuffer Transform : register(b0)
{
    float4x4 World : packoffset(c0); // ワールド行列
    float4x4 View : packoffset(c4); // ビュー行列
    float4x4 Proj : packoffset(c8); // プロジェクション行列
};

cbuffer LightBuff : register(b1)
{
    float3 ligPos : packoffset(c0); // ライトの位置
    float3 ligColor : packoffset(c1); // ライトの色
    float3 CameraPos : packoffset(c2); // カメラの位置
}

cbuffer MaterialBuff : register(b2)
{
    float3 Diffuse : packoffset(c0); // 拡散反射率
    float Alpha : packoffset(c0.w); // 透明度
    float3 Specular : packoffset(c1); // 鏡面反射率
    float Shininess : packoffset(c1.w); // 輝き係数
}