#include "../ShaderHeader/inc_Basic.hlsli"

Texture2D baseColorTex : register(t0);
SamplerState baseColorSamp : register(s0);

Texture2D normalTex : register(t1);
SamplerState normalSamp : register(s1);

Texture2D metallicTex : register(t2);
SamplerState metallicSamp : register(s2);

Texture2D roughnessTex : register(t3);
SamplerState roughnessSamp : register(s3);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
    float3 L = normalize(ligForward);
    float3 V = normalize(CameraPos - pin.WorldPos);
    float3 H = normalize(V + L);
    float3 N = normalTex.Sample(normalSamp, pin.uv).rgb;

    N = mul(pin.InvTangentBasis, N);
    
    float NV = saturate(dot(N, V));
    float NH = saturate(dot(N, H));
    float NL = saturate(dot(N, L));
    
    float3 baseColor = baseColorTex.Sample(baseColorSamp, pin.uv).rgb;
    float metallic = metallicTex.Sample(metallicSamp, pin.uv).r;
    float roughness = roughnessTex.Sample(roughnessSamp, pin.uv).r;
    
    float3 Kd = baseColor * (1.0 - metallic);
    float3 diffuse = ComputeLambert(Kd);
    
    float3 Ks = baseColor * metallic;
    float3 specular = ComputeGGX(Ks, roughness, NH, NV, NL);
    
    float3 BRDF = diffuse + specular;
    
    float4 finalColor = float4(BRDF * NL * ligColor.rgb * ligIntensity, 1.0);
    
    return finalColor;
}