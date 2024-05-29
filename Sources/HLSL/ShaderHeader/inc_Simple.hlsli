struct Output
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer cbCamera : register(b0)
{
    row_major matrix c_mView;
    row_major matrix c_mProj;
};