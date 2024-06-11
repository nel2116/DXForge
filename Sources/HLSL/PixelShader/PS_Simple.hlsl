#include "../ShaderHeader/inc_Simple.hlsli"

float4 main(VSOutput pin) : SV_TARGET
{
    float4 color = pin.color;
    return color;
}