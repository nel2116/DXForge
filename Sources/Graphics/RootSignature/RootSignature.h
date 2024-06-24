// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RootSignature.h]
// �쐬�� : �c���~�m��
// �쐬�� : 20124/06/24
// �T�v   : ���[�g�V�O�l�`���N���X�̒�`
// �X�V����
// 2024/06/24  �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <System/ComPtr.h>
#include <d3d12.h>
#include <vector>

// ====== �񋓑� ======
enum ShaderStage
{
	ALL = 0, // �S�X�e�[�W
	VS = 1,  // ���_�V�F�[�_
	HS = 2,  // �n���V�F�[�_
	DS = 3,  // �h���C���V�F�[�_
	GS = 4,  // �W�I���g���V�F�[�_
	PS = 5,  // �s�N�Z���V�F�[�_
};

enum SamplerState
{
	PointWrap = 0,		// �|�C���g�T���v�����O�A���b�v
	PointClamp = 1,		// �|�C���g�T���v�����O�A�N�����v
	LinearWrap = 2,		// ���j�A�T���v�����O�A���b�v
	LinearClamp = 3,	// ���j�A�T���v�����O�A�N�����v
	AnisotropicWrap = 4,// �ٕ����t�B���^�����O�A���b�v
	AnisotropicClamp = 5,// �ٕ����t�B���^�����O�A�N�����v
};

// ====== �N���X��` ======
class RootSignature
{
public:
	// �f�B�X�N�N���X
	class Desc
	{
	public:
		Desc();
		~Desc();
		Desc& Begin(int count);
		Desc& SetCBV(ShaderStage stage, int index, uint32_t reg);
		Desc& SetSRV(ShaderStage stage, int index, uint32_t reg);
		Desc& SetUAV(ShaderStage stage, int index, uint32_t reg);
		Desc& SetSmp(ShaderStage stage, int index, uint32_t reg);
		Desc& AddStaticSmp(ShaderStage stage, uint32_t reg, SamplerState state);
		Desc& AllowIL();
		Desc& AllowSO();
		Desc& End();
		const D3D12_ROOT_SIGNATURE_DESC* GetDesc() const;
	private:
		std::vector<D3D12_DESCRIPTOR_RANGE>     m_Ranges;
		std::vector<D3D12_STATIC_SAMPLER_DESC>  m_Samplers;
		std::vector<D3D12_ROOT_PARAMETER>       m_Params;
		D3D12_ROOT_SIGNATURE_DESC               m_Desc;
		bool                                    m_DenyStage[5];
		uint32_t                                m_Flags;

		void CheckStage(ShaderStage stage);
		void SetParam(ShaderStage, int index, uint32_t reg, D3D12_DESCRIPTOR_RANGE_TYPE type);
	};

	// �p�u���b�N�֐�
	RootSignature();
	~RootSignature();
	bool Init(const D3D12_ROOT_SIGNATURE_DESC* pDesc);
	void Uninit();
	ID3D12RootSignature* GetPtr() const;

private:
	// �����o�ϐ�
	ComPtr<ID3D12RootSignature> m_pRootSignature;
};
