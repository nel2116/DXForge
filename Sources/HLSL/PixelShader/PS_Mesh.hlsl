#include "../ShaderHeader/inc_Mesh.hlsli"

Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 main(VSOutput pin) : SV_TARGET
{
    float4 color = tex.Sample(sam, pin.uv);
    
    return color;
}