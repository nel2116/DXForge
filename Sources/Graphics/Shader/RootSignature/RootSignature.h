// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RootSignature.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/26 18:00
// �T�v   : ���[�g�V�O�l�`���[�N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Graphics/Renderer.h>

enum class RangeType
{
	CBV,
	SRV,
	UAV,
	CONSTANT,
	DESCRIPTOR_TABLE,
	SAMPLER,
};

enum class TextureAddressMode
{
	Wrap,
	Mirror,
	Clamp,
	Border,
	MirrorOnce,
};

enum class D3D12Filter
{
	Point,
	Linear,
};

class RootSignature
{
private:	// �^��`
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// �����o�֐�
	/// <summary>
	/// �쐬
	/// </summary>
	/// <param name="pDev">�����_���[�N���X�̃|�C���^</param>
	/// <param name="rangeType">�����W�^�C�v���X�g</param>
	void Create(Renderer* pDev, std::vector<RangeType>& rangeType);

	/// <summary>
	/// ���[�g�V�O�l�`���[�̎擾
	/// </summary>
	/// <returns>���[�g�V�O�l�`���[�̃|�C���^</returns>
	ID3D12RootSignature* GetRootSignature() { return m_pRootSignature.Get(); }

private:	// �v���C�x�[�g�֐�
	/// <summary>
	/// �����W�̍쐬
	/// </summary>
	/// <param name="pRange">�����W�̃|�C���^</param>
	/// <param name="type">�����W�^�C�v</param>
	/// <param name="count">�o�^��</param>
	void CreateRange(D3D12_DESCRIPTOR_RANGE& pRange, RangeType type, UINT count);

	/// <summary>
	/// �T���v���[�̍쐬
	/// </summary>
	/// <param name="pSamplerDesc">�T���v���[�f�X�N�̃|�C���^</param>
	/// <param name="addressMode">�A�h���X���[�h</param>
	/// <param name="filter">�t�B���^</param>
	/// <param name="count">�g�p�T���v���[��</param>
	void CreateStaticSampler(D3D12_STATIC_SAMPLER_DESC& pSamplerDesc, TextureAddressMode addressMode, D3D12Filter filter, UINT count);


private:	// �����o�ϐ�
	Renderer* m_pRenderer = nullptr;
	ComPtr<ID3DBlob> m_pRootBlob = nullptr;
	ComPtr<ID3D12RootSignature> m_pRootSignature = nullptr;
};
