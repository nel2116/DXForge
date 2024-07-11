// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Factory.h]
// ����� : �c���~�m��
// �쐬�� : 2024/07/02
// �T�v   : PSO�ƃ��[�g�V�O�l�`���̐����N���X
// �X�V��
// 2024/07/02 : �c���~�m�� : �V�K�쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Manager/Manager.h"
#include <System/ComPtr.h>

// ====== �N���X�̒�` ======
class Factory : public Manager<Factory>
{
public:	// �p�u���b�N�֐�

	/// <summary>
	/// �p�C�v���C���X�e�[�g�̐���
	/// </summary>
	/// <param name="name">��������p�C�v���C���X�e�[�g�̖��O</param>
	/// <param name="pRootSig">���[�g�V�O�l�`��</param>
	/// <param name="pVS">���_�V�F�[�_</param>
	/// <param name="pPS">�s�N�Z���V�F�[�_</param>
	/// <param name="pInputLayout">�C���v�b�g���C�A�E�g</param>
	/// <param name="topology">�v���~�e�B�u�g�|���W</param>
	/// <param name="depthStencilState">�[�x�X�e���V���X�e�[�g</param>
	/// <param name="blendState">�u�����h�X�e�[�g</param>
	/// <param name="rasterizerState">���X�^���C�U�X�e�[�g</param>
	/// <param name="sampleMask">�T���v���}�X�N</param>
	/// <param name="sampleDesc">�T���v���ݒ�</param>
	/// <param name="numRenderTargets">�����_�[�^�[�Q�b�g�̐�</param>
	/// <param name="rtvFormats">�����_�[�^�[�Q�b�g�̃t�H�[�}�b�g</param>
	/// <param name="dsvFormat">�f�v�X�X�e���V���r���[�t�H�[�}�b�g</param>
	///	<param name="depthStencilViewFlags">�f�v�X�X�e���V���r���[�t���O</param>
	/// <returns>�p�C�v���C���X�e�[�g�̃|�C���^</returns>
	ComPtr<ID3D12PipelineState> CreatePSO(
		const std::string& name,
		const RootSignature* pRootSig,
		const ID3DBlob* pVS,
		const ID3DBlob* pPS,
		const D3D12_INPUT_ELEMENT_DESC* pElement,
		const D3D12_PRIMITIVE_TOPOLOGY_TYPE topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		const D3D12_DEPTH_STENCIL_DESC depthStencilState,
		const D3D12_BLEND_DESC blendState,
		const D3D12_RASTERIZER_DESC rasterizerState,
		const UINT sampleMask = UINT_MAX,
		const DXGI_SAMPLE_DESC sampleDesc,
		const UINT numRenderTargets = 1,
		const DXGI_FORMAT rtvFormat,
		const DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT,
		const D3D12_DSV_FLAGS depthStencilViewFlags = D3D12_DSV_FLAG_NONE
	);

	/// <summary>
	/// ���[�g�V�O�l�`���̐ݒ�
	/// </summary>
	/// <param name="name">���[�g�V�O�l�`���̖��O</param>
	/// <param name="pRootSig">���[�g�V�O�l�`��</param>
	void SetRootSignature(const std::string& name, RootSignature* pRootSig);

	/// <summary>
	/// ���[�g�V�O�l�`���̎擾
	/// </summary>
	/// <param name="name">���[�g�V�O�l�`���̖��O</param>
	/// <returns>���[�g�V�O�l�`���̃|�C���^</returns>
	RootSignature* GetRootSignature(const std::string& name);

	/// <summary>
	/// �p�C�v���C���X�e�[�g�̎擾
	/// </summary>
	/// <param name="name">�p�C�v���C���X�e�[�g�̖��O</param>
	/// <returns>�p�C�v���C���X�e�[�g�̃|�C���^</returns>
	ID3D12PipelineState* GetPSO(const std::string& name);

private:
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> m_pPSO;	// �p�C�v���C���X�e�[�g
	std::unordered_map<std::string, RootSignature*> m_pRootSig;				// ���[�g�V�O�j�`��

private:	// �V���O���g���֘A
	Factory() = default;
	friend class Manager<Factory>;
};