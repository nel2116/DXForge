// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Pipeline.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/26 18:00
// �T�v   : �p�C�v���C���N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Graphics/Renderer.h>

enum class CullMode
{
	None = D3D12_CULL_MODE_NONE,
	Front = D3D12_CULL_MODE_FRONT,
	Back = D3D12_CULL_MODE_BACK,
};

enum class BlendMode
{
	Add,
	Alpha,
};

enum class InputLayout
{
	POSITION,
	TEXCOORD,
	NORMAL,
	TANGENT,
	COLOR,
	SKININDEX,
	SKINWEIGHT,
};

enum class PrimitiveTopology
{
	Undefined = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED,
	Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
	Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
	Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
	Patch = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH,
};

// ====== �N���X�̒�` ======
class RootSignature;
class Pipeline
{
private:	// �^��`
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// �`��ݒ�̃Z�b�g
	/// </summary>
	/// <param name="pDev">�����_���[�N���X�̃|�C���^</param>
	/// <param name="pRootSignature">���[�g�V�O�l�`���N���X�̃|�C���^</param>
	/// <param name="inputLayout">���̓��C�A�E�g���</param>
	/// <param name="cullMode">�J�����O���[�h</param>
	/// <param name="blendMode">�u�����h���[�h</param>
	/// <param name="topology">�v���~�e�B�u�g�|���W�[</param>
	void SetRenderSetting(Renderer* pDev, RootSignature* pRootSignature, std::vector<InputLayout>& inputLayouts, CullMode cullMode, BlendMode blendMode, PrimitiveTopology topology);

	/// <summary>
	/// �쐬
	/// </summary>
	/// <param name="pBlob">�V�F�[�_�[�f�[�^���X�g</param>
	/// <param name="formats">�t�H�[�}�b�g���X�g</param>
	/// <param name="isDepth">�[�x���e�X�g</param>
	/// <param name="isDepthMask">�[�x��������</param>
	/// <param name="rtvCount">RTV�̐�</param>
	/// <param name="isWireFrame">���C���[�t���[�����ǂ���</param>
	void Create(std::vector<ID3DBlob*> pBlob, const std::vector<DXGI_FORMAT>formats, bool isDepth, bool isDepthMask, int rtvCount, bool isWireFrame);

	/// <summary>
	/// �p�C�v���C���̎擾
	/// </summary>
	/// <returns>�p�C�v���C���̃|�C���^</returns>
	ID3D12PipelineState* GetPipeline() { return m_pPipelineState.Get(); }

	/// <summary>
	/// �g�|���W�[�^�C�v�̎擾
	/// </summary>
	/// <returns>�g�|���W�[�^�C�v</returns>
	PrimitiveTopology GetTopology() { return m_topologyType; }

private:		// �v���C�x�[�g�֐�
	void SetInputLayout(std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElements, std::vector<InputLayout>& inputLayouts);

	/// <summary>
	/// �u�����h���[�h�̃Z�b�g
	/// </summary>
	/// <param name="blendDesc">�����_�[�e�[�Q�b�g�u�����h���</param>
	/// <param name="blendMode">�u�����h���[�h</param>
	void SetBlendMode(D3D12_RENDER_TARGET_BLEND_DESC& blendDesc, BlendMode blendMode);

private:		// �����o�ϐ�
	Renderer* m_pRenderer = nullptr;
	RootSignature* m_pRootSignature = nullptr;

	std::vector<InputLayout> m_inputLayouts;
	CullMode m_cullMode;
	BlendMode m_blendMode;
	PrimitiveTopology m_topologyType;

	ComPtr<ID3D12PipelineState> m_pPipelineState = nullptr;
};
