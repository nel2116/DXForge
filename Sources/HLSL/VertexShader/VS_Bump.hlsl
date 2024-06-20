#include "../ShaderHeader/inc_Bump.hlsli"

VS_OUTPUT main(VS_INPUT vin)
{
    VS_OUTPUT vout = (VS_OUTPUT) 0;
    
    float4 localPos = float4(vin.pos, 1.0f);
    float4 worldPos = mul(World, localPos);
    float4 viewPos = mul(View, worldPos);
    float4 projPos = mul(Proj, viewPos);

    vout.pos = projPos;
    vout.uv = vin.uv;
    vout.WorldPos = worldPos;

    // 基底ベクトル
    float3 N = normalize(mul((float3x3) World, vin.normal));
    float3 T = normalize(mul((float3x3) World, vin.tangent));
    float3 B = normalize(cross(N, T));
    
#if 0 // 接線空間上でライティングする場合
    // 基底変換行列
    vout.TangentBasis = float3x3(T, B, N);
#else
    // 接線空間への基底変換行列の逆行列
    vout.InvTangentBasis = transpose(float3x3(T, B, N));
#endif

    return vout;
}