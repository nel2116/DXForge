#include "../ShaderHeader/inc_Phong.hlsli"

VS_OUTPUT main(VS_INPUT vin)
{
    VS_OUTPUT vout = (VS_OUTPUT) 0;
    
    float4 localPos = float4(vin.pos, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    vout.pos = projPos;
    vout.uv = vin.uv;
    vout.WorldPos = worldPos;
    vout.normal = normalize(mul((float3x3) World, vin.normal));
    return vout;
}