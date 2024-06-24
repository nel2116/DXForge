#include "inc_BRDF.hlsli"

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
    float2 uv : TEXCOORD; // �e�N�X�`�����W
    float3 WorldPos : WORLD_POS; // ���[���h��Ԃł̍��W
    float3x3 InvTangentBasis : INV_TANGENT_BASIS; // �ڐ���Ԃւ̊��ϊ��s��̋t�s��
};

cbuffer Transform : register(b0)
{
    float4x4 View : packoffset(c0); // �r���[�s��
    float4x4 Proj : packoffset(c4); // �v���W�F�N�V�����s��
};

cbuffer CbMesh : register(b1)
{
    float4x4 World : packoffset(c0); // ���[���h�s��ł�.
};

cbuffer LightBuff : register(b2)
{
    float3 ligColor : packoffset(c0); // ���C�g�̐F
    float ligIntensity : packoffset(c0.w); // ���C�g�̋��x
    float3 ligForward : packoffset(c1); // ���C�g�̌���
};

cbuffer CameraBuff : register(b3)
{
    float3 CameraPos : packoffset(c0); // �J�����̈ʒu
};