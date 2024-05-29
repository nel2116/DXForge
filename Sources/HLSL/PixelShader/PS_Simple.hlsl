#include "../ShaderHeader/inc_Simple.hlsli"

Texture2D baseTex : register(t0);
Texture2D normalTex : register(t0);
Texture2D roughnessMetallicTex : register(t0);
Texture2D emissiveTex : register(t0);

SamplerState sam : register(s0);

float4 PS_Main(Output pin) : SV_TARGET
{
    float4 color = baseTex.Sample(sam, pin.uv);
    return color;
}