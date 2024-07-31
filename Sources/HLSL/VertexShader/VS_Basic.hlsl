//-----------------------------------------------------------------------------
// File : BasicVS.hlsl
// Desc : Vertex Shader.
// Copyright(c) Pocol. All right reserved.
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// VSInput structure
///////////////////////////////////////////////////////////////////////////////
struct VSInput
{
    float3 Position : POSITION; // �ʒu���W�ł�.
    float3 Normal : NORMAL; // �@���x�N�g���ł�.
    float2 TexCoord : TEXCOORD; // �e�N�X�`�����W�ł�.
    float3 Tangent : TANGENT; // �ڐ��x�N�g���ł�.
};

///////////////////////////////////////////////////////////////////////////////
// VSOutput structure
///////////////////////////////////////////////////////////////////////////////
struct VSOutput
{
    float4 Position : SV_POSITION; // �ʒu���W�ł�.
    float2 TexCoord : TEXCOORD; // �e�N�X�`�����W�ł�.
    float3 Normal : NORMAL; // �@���x�N�g���ł�.
    float3 WorldPos : WORLD_POS; // ���[���h��Ԃ̈ʒu���W�ł�.
};

///////////////////////////////////////////////////////////////////////////////
// CbTransform constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbTransform : register(b0)
{
    float4x4 View : packoffset(c0); // �r���[�s��ł�.
    float4x4 Proj : packoffset(c4); // �ˉe�s��ł�.
};

///////////////////////////////////////////////////////////////////////////////
// CbMesh constant buffer
///////////////////////////////////////////////////////////////////////////////
cbuffer CbMesh : register(b1)
{
    float4x4 World : packoffset(c0); // ���[���h�s��ł�.
};

//-----------------------------------------------------------------------------
//      ���_�V�F�[�_�̃��C���G���g���[�|�C���g�ł�.
//-----------------------------------------------------------------------------
VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4 localPos = float4(input.Position, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    output.Position = projPos;
    output.TexCoord = input.TexCoord;
    output.Normal = normalize(mul((float3x3) World, input.Normal));
    output.WorldPos = worldPos.xyz;

    return output;
}