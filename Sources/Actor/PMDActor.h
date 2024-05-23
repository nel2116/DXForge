// [PMDActor.h]
// �쐬�� : 2024/05/21 19:30
// �쐬�� : �c���~�m��
// �T�v   : PMD�A�N�^�[�N���X�̒�`
#pragma once
#include <DirectX/d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

class Dx12Wrapper;
class PMDRenderer;
class PMDActor
{
	friend PMDRenderer;
private:	// �\���̒�`
	// �V�F�[�_���ɓ�������}�e���A���f�[�^
	struct MaterialForHlsl
	{
		DirectX::XMFLOAT3 diffuse;	// �f�B�t���[�Y�F
		float alpha;				// ���l
		DirectX::XMFLOAT3 specular;	// �X�y�L�����F
		float specularity;			// �X�y�L�����̋���
		DirectX::XMFLOAT3 ambient;	// �A���r�G���g�F
	};
	// ����ȊO�̃}�e���A���f�[�^
	struct additionalMaterial
	{
		string texPath;	// �e�N�X�`���p�X
		int toonIdx;	// �g�D�[���ԍ�
		bool edgeFlg;	// �}�e���A�����Ƃ̗֊s���t���O
	};
	// �܂Ƃ߂�����
	struct Material
	{
		unsigned int indicesNum;	// �C���f�b�N�X��
		MaterialForHlsl material;	// �V�F�[�_���ɓ�������}�e���A���f�[�^
		additionalMaterial additional;	// ����ȊO�̃}�e���A���f�[�^
	};
	struct Transform
	{
		// �����Ɏ����Ă���XMMATRIX�����o��16�o�C�g�A���C�����g�ł��邽�߁A
	};


private:
	PMDRenderer& m_renderer;
	Dx12Wrapper& m_dx12;
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// ���_�֘A
	ComPtr<ID3D12Resource> m_vb = nullptr;
	ComPtr<ID3D12Resource> m_ib = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
	D3D12_INDEX_BUFFER_VIEW m_ibView = {};

	ComPtr<ID3D12Resource> m_transformMat = nullptr;	// ���W�ϊ��s��(���̓��[���h�̂�)
	ComPtr<ID3D12DescriptorHeap> m_transfromHeap = nullptr;	// ���W�ϊ��q�[�v



};
