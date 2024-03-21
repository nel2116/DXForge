struct PS_IN
{
    float4 pos : POSITION;
    float4 svpos : SV_POSITION;
};

float4 PS_Main(PS_IN pin) : SV_TARGET
{
    return float4((float2(0, 1) + pin.pos.xy) * 0.5f, 1, 1);

}