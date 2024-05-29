struct Output
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tngent : TANGENT;
    float4 Color : COLOR;
};

cbuffer cbCamera : register(b0)
{
    row_major matrix c_mView;
    row_major matrix c_mProj;
};

cbuffer cbWorld : register(b1)
{
    row_major matrix c_mWorld;
};