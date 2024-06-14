#include "../ShaderHeader/inc_Lambert.hlsli"

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
    float3 N = normalize(pin.normal);
    float3 L = normalize(ligPos - pin.WorldPos.xyz);
    
    float4 color = tex.Sample(samp, pin.uv);
    float3 diffuse = ligColor * Diffuse * saturate(dot(L, N));
    
    float4 finalColor = float4(color.rgb * diffuse, color.a * Alpha);
    
    return finalColor;
}