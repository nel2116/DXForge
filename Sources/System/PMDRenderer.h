// [PMDRenderer.h]
// �쐬�� : 2024/05/23 18:00
// �쐬�� : �c���~�m��
// �T�v   : PMD�����_���[�N���X�̒�`
#pragma once

#include <d3d12.h>
#include <wrl.h>
#include <memory>

class Dx12Wrapper;
class PMDActor;
class PMDRenderer
{
	friend PMDActor;

public:	// �p�u���b�N�֐�
	PMDRenderer(Dx12Wrapper& dx12);
	~PMDRenderer();
	void Update();
	void Draw();
	ID3D12PipelineState* GetPipelineState();
	ID3D12RootSignature* GetRootSignature();

private:	// �v���C�x�[�g�֐�
	ID3D12Resource* CreateDeaultTexture(size_t width, size_t height);
	ID3D12Resource* CreateWhiteTexture();	// ���e�N�X�`���̐���
	ID3D12Resource* CreateBlackTexture();	// ���e�N�X�`���̐���
	ID3D12Resource* CreateGradationTexture();	// �O���C�X�P�[���O���f�[�V�����e�N�X�`���̐���

	// �p�C�v���C��������
	HRESULT CreateGraphicsPipelineForPMD();
	// ���[�g�V�O�l�`��������
	HRESULT CreateRootSignature();

	bool CheckShaderCompileResult(HRESULT hr, ID3DBlob* err = nullptr);

private:	// �����o�ϐ�
	Dx12Wrapper& m_dx12;
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ComPtr<ID3D12PipelineState> m_pipeline = nullptr;	// PMD�p�̃p�C�v���C��
	ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;	// ���[�g�V�O�l�`��

	// PMD�p���ʃe�N�X�`��(���A���A�O���C�X�P�[���O���f�[�V����)
	ComPtr<ID3D12Resource> m_whiteTex = nullptr;
	ComPtr<ID3D12Resource> m_blackTex = nullptr;
	ComPtr<ID3D12Resource> m_gradTex = nullptr;

};