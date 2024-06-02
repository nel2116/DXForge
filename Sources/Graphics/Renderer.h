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

using namespace std;

// ====== �N���X��` ======
class RTVHeap;
class CBVSRVUAVHeap;
class CBufferAllocater;
class DSVHeap;
class DepthStencil;
class ImGuiHeap;
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
	/// 3D�����_�����O�̊J�n
	/// </summary>
	/// <returns></returns>
	void Begin3DDraw();

	/// <summary>
	/// 2D�����_�����O�̊J�n
	/// </summary>
	/// <returns></returns>
	void Begin2DDraw();

	/// <summary>
	/// ImGui�̕`��J�n
	/// </summary>
	/// <returns></returns>
	void BeginImGuiDraw();

	/// <summary>
	/// ImGui�̕`��I��
	/// </summary>
	/// <returns></returns>
	void EndImGuiDraw();

	/// <summary>
	/// �����_�����O�̏I��
	/// </summary>
	/// <returns></returns>
	void EndDraw();

	/// <summary>
	/// �R�}���h�L���[�̓����҂�
	/// </summary>
	void WaitForCmdQueue();

public:		// �A�N�Z�T�֐�
	/// <summary>
	/// �f�o�C�X�̎擾
	/// </summary>
	/// <returns>�f�o�C�X�̎Q��</returns>
	ID3D12Device8* GetDev()const { return m_pDevice.Get(); }

	/// <summary>
	/// �R�}���h���X�g�̎擾
	/// </summary>
	/// <returns>�R�}���h���X�g�̎Q��</returns>
	ID3D12GraphicsCommandList6* GetCmdList()const { return m_pCmdList.Get(); }

	/// <summary>
	/// CBVSRVUAV�q�[�v�̎擾
	/// </summary>
	/// <returns>CBVSRVUAV�q�[�v�̃|�C���^</returns>
	CBVSRVUAVHeap* GetCBVSRVUAVHeap()const { return m_upCBVSRVUAVHeap.get(); }

	/// <summary>
	/// CBuffer�A���P�[�^�̎擾
	/// </summary>
	/// <returns>CBuffer�A���P�[�^�̃|�C���^</returns>
	CBufferAllocater* GetCBufferAllocater()const { return m_upCBufferAllocater.get(); }

	/// <summary>
	/// DSV�q�[�v�̎擾
	/// </summary>
	/// <returns>DSV�q�[�v�̃|�C���^</returns>
	DSVHeap* GetDSVHeap()const { return m_upDSVHeap.get(); }

	/// <summary>
	/// imuGui�q�[�v�̎擾
	/// </summary>
	/// <returns>imuGui�q�[�v�̃|�C���^</returns>
	ImGuiHeap* GetImGuiHeap()const { return m_upImGuiHeap.get(); }

	/// <summary>
	/// �N���A�J���[�̐ݒ�
	/// </summary>
	/// <param name="r">�Ԑ���</param>
	/// <param name="g">�ΐ���</param>
	/// <param name="b">����</param>
	/// <param name="a">�A���t�@����</param>
	void SetClearColor(float r, float g, float b, float a);

	/// <summary>
	/// ���\�[�X�Ƃ��Ĉ����ɓn�����o�b�t�@�̈�����ύX����֐�
	/// </summary>
	/// <param name="pResource">���\�[�X�̎Q��</param>
	/// <param name="before">�ύX�O�̃��\�[�X�̏��</param>
	/// <param name="after">�ύX��̃��\�[�X�̏��</param>
	void TransBarrier(ID3D12Resource* pResource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

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
	/// ImGui�̃Z�b�e�B���O
	/// </summary>
	/// <returns>�쐬�ɐ���������true</returns>
	bool ImGuiSetting();

private:	// �����o�ϐ�
	// �f�o�C�X
	D3D_FEATURE_LEVEL m_featureLevel = {};
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
	// CBVSRVUAV�q�[�v
	unique_ptr<CBVSRVUAVHeap> m_upCBVSRVUAVHeap = nullptr;
	// CBuffer�A���P�[�^
	unique_ptr<CBufferAllocater> m_upCBufferAllocater = nullptr;
	// DSV�q�[�v
	unique_ptr<DSVHeap> m_upDSVHeap = nullptr;
	// �f�v�X�X�e���V��
	unique_ptr<DepthStencil> m_upDepthStencil = nullptr;
	// imuGui�q�[�v
	unique_ptr<ImGuiHeap> m_upImGuiHeap = nullptr;

	// Fance
	ComPtr<ID3D12Fence> m_pFence = nullptr;
	UINT64 m_fanceVal = 0;

	// �N���A�J���[
	float m_clearColor[4] = {};
	// �E�B���h�E�N���X
	Window* m_pWindow = nullptr;
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
