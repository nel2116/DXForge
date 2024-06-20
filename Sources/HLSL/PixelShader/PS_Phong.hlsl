#include "../ShaderHeader/inc_Phong.hlsli"

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
    float3 N = normalize(pin.normal);
    float3 L = normalize(ligPos - pin.WorldPos.xyz);
    float3 V = normalize(CameraPos - pin.WorldPos.xyz);
    
    float3 R = normalize(-reflect(V, N));
    
    float4 color = tex.Sample(samp, pin.uv);
    float3 diffuse = ligColor * Diffuse * saturate(dot(L, N));
    float3 specular = ligColor * Specular * pow(saturate(dot(L, R)), Shininess);
    
    float4 finalColor = float4(color.rgb * (diffuse + specular), color.a * Alpha);
    
    return finalColor;
}