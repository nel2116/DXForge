struct Output
{
    float4 pos : SV_POSITION;
};

cbuffer cbCamera : register(b0)
{
    row_major matrix c_mView;
    row_major matrix c_mProj;
};

cbuffer cbWorld : register(b1)
{
    row_major matrix c_mWorld;
};