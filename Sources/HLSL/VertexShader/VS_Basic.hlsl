struct VS_OUT
{
    float4 pos : POSITION;
    float4 svpos : SV_POSITION;
};

VS_OUT VS_Main(float4 pos : POSITION)
{
    VS_OUT vout;
    vout.pos = pos;
    vout.svpos = pos;
    return vout;
}