#include "../ShaderHeader/inc_Bump.hlsli"

Texture2D tex : register(t0); // テクスチャ
Texture2D normalTex : register(t1); // 法線マップ
SamplerState samp : register(s0);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
#if 0   // 接線空間上でライティングする場合
    // ライトベクトル
    float3 L = normalize(ligPos - pin.WorldPos.xyz);
    L = mul(pin.TangentBasis, L);
    
    // 視線ベクトル
    float3 V = normalize(CameraPos - pin.WorldPos.xyz);
    V = mul(pin.TangentBasis, V);

    // 法線ベクトル
    float3 N = normalTex.Sample(samp, pin.uv).rgb * 2.0f - 1.0f;
#else
    // ライトベクトル
    float3 L = normalize(ligPos - pin.WorldPos.xyz);
    
    // 視線ベクトル
    float3 V = normalize(CameraPos - pin.WorldPos.xyz);
    
    // 法線ベクトル
    float3 N = normalTex.Sample(samp, pin.uv).rgb * 2.0f - 1.0f;
    N = mul(pin.InvTangentBasis, N);
#endif
    
    // 反射ベクトル    
    float3 R = normalize(-reflect(V, N));
    
    // ライティング計算
    float4 color = tex.Sample(samp, pin.uv);
    float3 diffuse = ligColor * Diffuse * saturate(dot(L, N));
    float3 specular = ligColor * Specular * pow(saturate(dot(L, R)), Shininess);
    float4 finalColor = float4(color.rgb * (diffuse + specular), color.a * Alpha);
    
    return finalColor;
}