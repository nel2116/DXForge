#include "../ShaderHeader/inc_Texture.hlsli"

Texture2D tex : register(t0);

SamplerState sam : register(s0);

float4 PS_Main(Output pin) : SV_TARGET
{
    float4 color = tex.Sample(sam, pin.uv);
    return color;
}