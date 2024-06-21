#define F_PI (3.14159265359f)

struct VS_INPUT
{
    float3 pos : POSITION; // �ʒu
    float3 normal : NORMAL; // �@��
    float2 uv : TEXCOORD; // �e�N�X�`�����W
    float3 tangent : TANGENT; // �ڐ�
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION; // �ʒu
    float3 normal : NORMAL; // �@��
    float2 uv : TEXCOORD; // �e�N�X�`�����W
    float3 WorldPos : WORLD_POS; // ���[���h��Ԃł̍��W
};

cbuffer Transform : register(b0)
{
    float4x4 World : packoffset(c0); // ���[���h�s��
    float4x4 View : packoffset(c4); // �r���[�s��
    float4x4 Proj : packoffset(c8); // �v���W�F�N�V�����s��
};

cbuffer LightBuff : register(b1)
{
    float3 ligPos : packoffset(c0); // ���C�g�̈ʒu
    float3 ligColor : packoffset(c1); // ���C�g�̐F
    float3 CameraPos : packoffset(c2); // �J�����̈ʒu
}

cbuffer MaterialBuff : register(b2)
{
    float3 BaseColor : packoffset(c0); // �g�U���˗�
    float Alpha : packoffset(c0.w); // �����x
    float3 Metallic : packoffset(c1); // �����x
    float Shininess : packoffset(c1.y); // ���ʔ��ˋ��x
}