#include "../ShaderHeader/inc_Phong.hlsli"

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
    float3 N = normalize(pin.normal);
    float3 L = normalize(ligPos - pin.WorldPos);
    float3 V = normalize(CameraPos - pin.WorldPos);
    float3 R = normalize(-V + 2.0f * dot(N, V) * N);
    
    float NL = saturate(dot(N, L));
    float LR = saturate(dot(L, R));
    
    float4 color = tex.Sample(samp, pin.uv);
    float3 diffuse = BaseColor * (1.0f - Metallic) * (1.0f / F_PI);
    float3 specular = BaseColor * Metallic * ((Shininess + 2.0f) / (2.0f * F_PI)) * pow(LR, Shininess);
    
    float4 finalColor = float4(ligColor * color.rgb * (diffuse + specular) * NL, color.a * Alpha);
    
    return finalColor;
}