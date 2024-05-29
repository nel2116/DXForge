#include "../ShaderHeader/inc_Simple.hlsli"

Output VS_Main(float4 pos : POSITION,
               float2 uv : TEXCOORD0,
               float3 normal : NORMAL,
               float4 color : COLOR,
               float3 tangent : TANGENT)
{
    Output output;
    output.pos = mul(pos, c_mWorld);
    output.pos = mul(output.pos, c_mView);
    output.pos = mul(output.pos, c_mProj);
    output.uv = uv;
    output.Color = color;
    output.Normal = normal;
    output.Tngent = tangent;
    
    return output;
}