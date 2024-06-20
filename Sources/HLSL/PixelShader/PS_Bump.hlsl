#include "../ShaderHeader/inc_Bump.hlsli"

Texture2D tex : register(t0); // �e�N�X�`��
Texture2D normalTex : register(t1); // �@���}�b�v
SamplerState samp : register(s0);

float4 main(VS_OUTPUT pin) : SV_TARGET
{
#if 0   // �ڐ���ԏ�Ń��C�e�B���O����ꍇ
    // ���C�g�x�N�g��
    float3 L = normalize(ligPos - pin.WorldPos.xyz);
    L = mul(pin.TangentBasis, L);
    
    // �����x�N�g��
    float3 V = normalize(CameraPos - pin.WorldPos.xyz);
    V = mul(pin.TangentBasis, V);

    // �@���x�N�g��
    float3 N = normalTex.Sample(samp, pin.uv).rgb * 2.0f - 1.0f;
#else
    // ���C�g�x�N�g��
    float3 L = normalize(ligPos - pin.WorldPos.xyz);
    
    // �����x�N�g��
    float3 V = normalize(CameraPos - pin.WorldPos.xyz);
    
    // �@���x�N�g��
    float3 N = normalTex.Sample(samp, pin.uv).rgb * 2.0f - 1.0f;
    N = mul(pin.InvTangentBasis, N);
#endif
    
    // ���˃x�N�g��    
    float3 R = normalize(-reflect(V, N));
    
    // ���C�e�B���O�v�Z
    float4 color = tex.Sample(samp, pin.uv);
    float3 diffuse = ligColor * Diffuse * saturate(dot(L, N));
    float3 specular = ligColor * Specular * pow(saturate(dot(L, R)), Shininess);
    float4 finalColor = float4(color.rgb * (diffuse + specular), color.a * Alpha);
    
    return finalColor;
}