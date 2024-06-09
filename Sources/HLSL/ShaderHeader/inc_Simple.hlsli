struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

cbuffer Transform : register(b0)
{
    float4x4 World : packoffset(c0); // ���[���h�s��
    float4x4 View : packoffset(c4); // �r���[�s��
    float4x4 Proj : packoffset(c8); // �v���W�F�N�V�����s��
};