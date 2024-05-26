// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : �����_���[�N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======

// ====== �N���X��` ======
class Renderer
{
private:	// �萔��`
	// GPU�̃e�B�A
	enum class E_GPU_TIER
	{
		NVIDIA,
		Amd,
		Intel,
		Arm,
		Qualcomm,
		Kind,
	};

public:		// �p�u���b�N�֐�

	/// <summary>
	/// �����_���[�̏�����
	/// </summary>
	/// <returns>�������ɐ���������true</returns>
	bool Init();

private:
	/// <summary>
	/// �t�@�N�g���[�̍쐬
	/// </summary>
	/// <returns>�쐬�ɐ���������true</returns>
	bool CreateFactory();

	/// <summary>
	/// �f�o�C�X�̍쐬
	/// </summary>
	/// <returns>�쐬�ɐ���������true</returns>
	bool CreateDevice();

private:	// �����o�ϐ�
	ComPtr<ID3D12Device8> m_pDevice = nullptr;
	ComPtr<IDXGIFactory6> m_pFactory = nullptr;
	ComPtr<IDXGISwapChain4> m_pSwapChain = nullptr;

public:
	// �V���O���g���֘A
	/// <summary>
	/// Renderer�N���X�̃C���X�^���X���擾
	/// </summary>
	/// <returns>Renderer�N���X�̃C���X�^���X�Q��</returns>
	static Renderer& Instance()
	{
		static Renderer instance;
		return instance;
	}
private:
	Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
};
