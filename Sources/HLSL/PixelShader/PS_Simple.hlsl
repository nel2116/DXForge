#include "../ShaderHeader/inc_Simple.hlsli"

PSOutput PS_Main(VSOutput pin)
{
    PSOutput pout = (PSOutput) 0;
    pout = pin.color;
    return pout;
}