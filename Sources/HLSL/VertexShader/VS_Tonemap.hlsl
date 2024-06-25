struct VSInput
{
    float2 Position : POSITION; // �ʒu���W�ł�.
    float2 TexCoord : TEXCOORD; // �e�N�X�`�����W�ł�.
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

cbuffer CbTransform : register(b0)
{
    float4x4 View : packoffset(c0); // �r���[�s��ł�.
    float4x4 Proj : packoffset(c4); // �ˉe�s��ł�.
};

cbuffer CbMesh : register(b1)
{
    float4x4 World : packoffset(c0); // ���[���h�s��ł�.
};

VSOutput main(const VSInput input)
{
    VSOutput output = (VSOutput) 0;

    float4 localPos = float4(input.Position, 0.0f, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);
    
    output.Position = projPos;
    output.TexCoord = input.TexCoord;

    return output;
}