#include "../Header/BasicShaderHeader.hlsli"

float4 PS_Main(Output pin) : SV_TARGET
{
    return float4(tex.Sample(smp, pin.uv));
}