#include "../ShaderHeader/inc_Texture.hlsli"

Output VS_Main(float4 pos : POSITION)
{
    Output output;
    output.pos = mul(pos, c_mWorld);
    output.pos = mul(output.pos, c_mView);
    output.pos = mul(output.pos, c_mProj);
    
    return output;
}