// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/10
// �쐬�� : �c���~�m��
// �T�v   : �e�N�X�`���N���X�̒�`
// �X�V���� : 2024/06/10 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <System/ComPtr.h>

// ====== �N���X��` ======
class DescriptorHandle;
class DescriptorPool;
class Texture
{
public:	// �p�u���b�N�֐�
	Texture();	// �R���X�g���N�^
	~Texture();	// �f�X�g���N�^
	bool Init(DescriptorPool* pPool, const wchar_t* filename, bool isSRGB);
	bool Init(DescriptorPool* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isSRGB, bool isCube);	// ������
	void Uninit();	// �I������

	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;	// CPU�n���h���̎擾
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;	// GPU�n���h���̎擾

private:	// �v���C�x�[�g�֐�
	D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);
	bool CreateTexture(const wchar_t* filename, bool isSRGB);

private:	// �����o�ϐ�
	ComPtr<ID3D12Resource> m_pTex;
	DescriptorHandle* m_pHandle;
	DescriptorPool* m_pPool;

private:
	Texture(const Texture&) = delete;
	void operator=(const Texture&) = delete;
};