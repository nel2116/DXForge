// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/10
// �쐬�� : �c���~�m��
// �T�v   : �e�N�X�`���N���X�̒�`
// �X�V���� : 2024/06/10 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <wrl/client.h>

// ====== �^��` ======
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// ====== �N���X��` ======
class Texture
{
public:	// �\���̒�`
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;	// ���W
		DirectX::XMFLOAT2 uv;	// �F
	};

	struct TextureData
	{
		ComPtr<ID3D12Resource> pResource;	// �e�N�X�`�����\�[�X
		D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPU�f�B�X�N���v�^�n���h��
		D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPU�f�B�X�N���v�^�n���h��
	};

	// �萔�o�b�t�@�\����
	// alignas(256) : �A���C�����g��256�o�C�g�ɐݒ�
	struct alignas(256) Transform
	{
		DirectX::XMMATRIX mWorld;	// ���[���h�s��
		DirectX::XMMATRIX mView;	// �r���[�s��
		DirectX::XMMATRIX mProj;	// �v���W�F�N�V�����s��
	};

	template <typename T>
	struct ConstantBufferView
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc; // �萔�o�b�t�@�̍\���ݒ�
		D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPU�f�B�X�N���v�^�n���h��
		D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPU�f�B�X�N���v�^�n���h��
		T* pBuffer;	// �o�b�t�@�擪�ւ̃|�C���^
	};

public:	// �p�u���b�N�֐�
	bool Init();	// ������
	void Draw();	// �`��
	void Uninit();	// �I������

private:	// �v���C�x�[�g�֐�
	bool CreateTexture();	// �e�N�X�`���̍쐬

private:	// �����o�ϐ�

	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV_SRV_UAV;	// �f�B�X�N���v�^�q�[�v(�萔�o�b�t�@�r���[�A�V�F�[�_�[���\�[�X�r���[�A�A���I�[�_�[�h�A�N�Z�X�r���[)
	ComPtr<ID3D12Resource> m_pVB;						// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW m_vbView;					// ���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> m_pIB;						// �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW m_ibView;					// �C���f�b�N�X�o�b�t�@�r���[
	ComPtr<ID3D12Resource> m_pCB[4];					// �萔�o�b�t�@
	ConstantBufferView<Transform> m_CBV[4];				// �萔�o�b�t�@�r���[
	TextureData m_Texture;								// �e�N�X�`���f�[�^
	ComPtr<ID3D12RootSignature> m_pRootSignature;		// ���[�g�V�O�l�`��
	ComPtr<ID3D12PipelineState> m_pPSO;					// �p�C�v���C���X�e�[�g
	float m_RotateAngle;								// ��]�p�x
	// �r���[�|�[�g�֘A
	D3D12_VIEWPORT m_viewport;	// �r���[�|�[�g
	D3D12_RECT m_scissorRect;	// �V�U�[��`
};