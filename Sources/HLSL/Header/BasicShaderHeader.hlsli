struct Output
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D<float4> tex : register(t0); // 0�ԖڃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); // 0�ԖڃX���b�g�ɐݒ肳�ꂽ�T���v���[�X�e�[�g