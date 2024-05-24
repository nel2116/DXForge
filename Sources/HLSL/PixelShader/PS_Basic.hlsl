#include "../Header/BasicShaderHeader.hlsli"

float4 PS_Main(Output input) : SV_TARGET
{
    float3 light = normalize(float3(1, -1, 1)); //���̌������x�N�g��(���s����)
    float3 lightColor = float3(1, 1, 1); //���C�g�̃J���[(1,1,1�Ő^����)

	//�f�B�t���[�Y�v�Z
    float diffuseB = saturate(dot(-light, input.normal.xyz));
    float4 toonDif = toon.Sample(smpToon, float2(0, 1.0 - diffuseB));

	//���̔��˃x�N�g��
    float3 refLight = normalize(reflect(light, input.normal.xyz));
    float specularB = pow(saturate(dot(refLight, -input.ray)), specular.a);
	
	//�X�t�B�A�}�b�v�pUV
    float2 sphereMapUV = input.vnormal.xy;
    sphereMapUV = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);

    float4 texColor = tex.Sample(smp, input.uv); //�e�N�X�`���J���[

    return saturate(toonDif //�P�x(�g�D�[��)
		* diffuse //�f�B�t���[�Y�F
		* texColor //�e�N�X�`���J���[
		* sph.Sample(smp, sphereMapUV)) //�X�t�B�A�}�b�v(��Z)
		+ saturate(spa.Sample(smp, sphereMapUV) * texColor //�X�t�B�A�}�b�v(���Z)
		+ float4(specularB * specular.rgb, 1)) //�X�y�L�����[
		+ float4(texColor.rgb * ambient * 0.5, 1); //�A���r�G���g(���邭�Ȃ肷����̂�0.5�ɂ��Ă܂�)

}