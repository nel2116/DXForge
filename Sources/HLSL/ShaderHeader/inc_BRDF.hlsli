// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/24
// �T�v   : BRDF�̌v�Z���s���֐����`�����w�b�_�t�@�C��
// �X�V�� 
// 2024/06/24 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#ifndef BRDF_HLSLI
#define BRDF_HLSLI

// ====== �萔��` ======
#ifndef F_PI
#define F_PI        3.14159265358979323f   // �~����
#endif//F_PI


//-----------------------------------------------------------------------------
//      Schlick�ɂ��t���l�����̋ߎ���.
//-----------------------------------------------------------------------------
float3 SchlickFresnel(float3 specular, float VH)
{
    return specular + (1.0f - specular) * pow((1.0f - VH), 5.0f);
}

//-----------------------------------------------------------------------------
//      GGX�ɂ��@�����z�֐�.
//-----------------------------------------------------------------------------
float D_GGX(float m2, float NH)
{
    float f = (NH * m2 - NH) * NH + 1;
    return m2 / (F_PI * f * f);
}

//-----------------------------------------------------------------------------
//      Height Correlated Smith�ɂ��􉽌�����.
//-----------------------------------------------------------------------------
float G2_Smith(float NL, float NV, float m2)
{
    float NL2 = NL * NL;
    float NV2 = NV * NV;

    float Lambda_V = (-1.0f + sqrt(m2 * (1.0f - NL2) / NL2 + 1.0f)) * 0.5f;
    float Lambda_L = (-1.0f + sqrt(m2 * (1.0f - NV2) / NV2 + 1.0f)) * 0.5f;
    return 1.0f / (1.0f + Lambda_V + Lambda_V);
}

//-----------------------------------------------------------------------------
//      Lambert BRDF���v�Z���܂�.
//-----------------------------------------------------------------------------
float3 ComputeLambert(float3 Kd)
{
    return Kd / F_PI;
}

//-----------------------------------------------------------------------------
//      Phong BRDF���v�Z���܂�.
//-----------------------------------------------------------------------------
float3 ComputePhong
(
    float3 Ks,
    float shininess,
    float LdotR
)
{
    return Ks * ((shininess + 2.0f) / (2.0f * F_PI)) * pow(LdotR, shininess);
}

//-----------------------------------------------------------------------------
//      GGX BRDF���v�Z���܂�.
//-----------------------------------------------------------------------------
float3 ComputeGGX
(
    float3 Ks,
    float roughness,
    float NdotH,
    float NdotV,
    float NdotL
)
{
    float a = roughness * roughness;
    float D = D_GGX(a, NdotH);
    float G = G2_Smith(NdotL, NdotV, a);
    float3 F = SchlickFresnel(Ks, NdotL);

    return (D * G * F) / (4.0f * NdotV * NdotL);
}

#endif//BRDF_HLSLI