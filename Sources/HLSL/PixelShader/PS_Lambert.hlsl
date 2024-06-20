#include "../ShaderHeader/inc_Lambert.hlsli"

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
    float3 N = normalize(pin.normal);
    float3 L = normalize(ligPos - pin.WorldPos.xyz);
    
    float NL = saturate(dot(N, L));
    
    float4 color = tex.Sample(samp, pin.uv);
    float3 diffuse = Diffuse * (1.0f / F_PI);
    
    float4 finalColor = float4(ligColor * color.rgb * diffuse * NL, color.a * Alpha);
    
    return finalColor;
}