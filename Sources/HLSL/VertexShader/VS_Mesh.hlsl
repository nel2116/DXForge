#include "../ShaderHeader/inc_Mesh.hlsli"

VSOutput main(VSInput vin)
{
    VSOutput vout = (VSOutput) 0;
    
    float4 localPos = float4(vin.pos, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    vout.pos = projPos;
    vout.uv = vin.uv;
    return vout;
}