// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : �����_���[�N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <wrl/client.h>
#include <array>
#include <queue>
#include <System/window.h>
#include "RTVHeap/RTVHeap.h"

using namespace std;

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

private:	// �^��`
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// �p�u���b�N�֐�

	/// <summary>
	/// �����_���[�̏�����
	/// </summary>
	/// <param name="window">�E�B���h�E�N���X�̎Q��</param>
	/// <returns>�������ɐ���������true</returns>
	bool Init(Window* window);

	/// <summary>
	/// �����_���[�̏I������
	/// </summary>
	/// <returns></returns>
	void Uninit();

	/// <summary>
	/// �����_�����O�̊J�n
	/// </summary>
	/// <returns></returns>
	void BeginDraw();

	/// <summary>
	/// �����_�����O�̏I��
	/// </summary>
	/// <returns></returns>
	void EndDraw();

	/// <summary>
	/// �R�}���h�L���[�̓����҂�
	/// </summary>
	void WaitForCmdQueue();

	/// <summary>
	/// �N���A�J���[�̐ݒ�
	/// </summary>
	/// <param name="r">�Ԑ���</param>
	/// <param name="g">�ΐ���</param>
	/// <param name="b">����</param>
	/// <param name="a">�A���t�@����</param>
	void SetClearColor(float r, float g, float b, float a);

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

	/// <summary>
	/// �R�}���h���X�g�̍쐬
	/// </summary>
	/// <returns>�쐬�ɐ���������true</returns>
	bool CreateCommandList();

	/// <summary>
	/// �X���b�v�`�F�C���̍쐬
	/// </summary>
	/// <returns>�쐬�ɐ���������true</returns>
	bool CreateSwapChain();

	/// <summary>
	/// �X���b�v�`�F�C��RTV�̍쐬
	/// </summary>
	/// <returns>�쐬�ɐ���������true</returns>
	bool CreateSwapChainRTV();

	/// <summary>
	/// Fence�̍쐬
	/// </summary>
	/// <returns>�쐬�ɐ���������true</returns>
	bool CreateFence();

	/// <summary>
	/// ���\�[�X�Ƃ��Ĉ����ɓn�����o�b�t�@�̈�����ύX����֐�
	/// </summary>
	/// <param name="pResource">���\�[�X�̎Q��</param>
	/// <param name="before">�ύX�O�̃��\�[�X�̏��</param>
	/// <param name="after">�ύX��̃��\�[�X�̏��</param>
	void TransBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);


private:	// �����o�ϐ�
	// �f�o�C�X
	D3D_FEATURE_LEVEL m_featureLevel;
	ComPtr<ID3D12Device8> m_pDevice = nullptr;
	ComPtr<IDXGIFactory6> m_pFactory = nullptr;
	// �R�}���h���X�g
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList6> m_pCmdList = nullptr;
	ComPtr<ID3D12CommandQueue> m_pCmdQueue = nullptr;
	// �X���b�v�`�F�C��
	ComPtr<IDXGISwapChain4> m_pSwapChain = nullptr;
	array<ComPtr<ID3D12Resource>, 2> m_pBackBuffers;
	unique_ptr<RTVHeap> m_pRTVHeap = nullptr;
	// Fance
	ComPtr<ID3D12Fence> m_pFence = nullptr;
	UINT64 m_fanceVal = 0;

	// �N���A�J���[
	float m_clearColor[4];
	// �E�B���h�E�N���X
	Window* m_pWindow;
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
