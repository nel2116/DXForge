// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : �����_���[�N���X�̒�`
// �X�V����
// 2024/05/25 �쐬
// 2024/06/24 ���t�@�N�^�����O
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Manager/Manager.h>
#include <System/window.h>
#include <System/ComPtr.h>
#include "Buffer/ConstantBuffer/ConstantBuffer.h"
#include "Buffer/VertexBuffer/VertexBuffer.h"
#include "RootSignature/RootSignature.h"
#include "Target/ColorTarget.h"
#include "Target/DepthTarget.h"
#include "CommandList/CommandList.h"
#include "Fence/Fence.h"

using namespace std;

#define FRAME_BUFFER_COUNT 2	// �t���[���o�b�t�@�̐�

// �񋓑�
enum DescriptorPoolType
{
	POOL_TYPE_RES = 0,     // CBV / SRV / UAV
	POOL_TYPE_SMP = 1,     // Sampler
	POOL_TYPE_RTV = 2,     // RTV
	POOL_TYPE_DSV = 3,     // DSV
	POOL_COUNT = 4,
};

// ====== �N���X��` ======
class Renderer : public Manager<Renderer>
{
public:		// �p�u���b�N�֐�
	/// <summary>
	/// �����_���[�̏�����
	/// </summary>
	/// <param name="window">�E�B���h�E�N���X�̃|�C���^</param>
	/// <returns>�������ɐ���������true</returns>
	bool Init(Window* window);

	/// <summary>
	/// �����_���[�̏I������
	/// </summary>
	/// <returns></returns>
	void Uninit();

	/// <summary>
	/// �����_���[�̕`��J�n����
	/// </summary>
	/// <returns></returns>
	void Begin();

	/// <summary>
	///	�����_���[�̕`��I������
	/// </summary>
	/// <returns></returns>
	void End(ColorTarget* sceneTarget);

	/// <summary>
	/// ���\�[�X�̏�ԑJ��
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="stateBefore"></param>
	/// <param name="stateAfter"></param>
	void TransitionResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE);

	/// <summary>
	/// GPU�̊�����҂�
	/// </summary>
	void WaitGpu();

public:		// �A�N�Z�T�֐�

	/// <summary>
	/// �f�o�C�X�̎擾
	/// </summary>
	/// <returns>�f�o�C�X</returns>
	ID3D12Device* GetDevice() const { return m_pDevice.Get(); }

	/// <summary>
	/// �R�}���h�L���[�̎擾
	/// </summary>
	/// <returns>�R�}���h�L���[</returns>
	ID3D12CommandQueue* GetCmdQueue() const { return m_pQueue.Get(); }

	/// <summary>
	/// �R�}���h���X�g�̎擾
	/// </summary>
	/// <returns>�R�}���h���X�g</returns>
	CommandList* GetCmdList() { return &m_CommandList; }

	/// <summary>
	/// �X���b�v�`�F�C���̎擾
	/// </summary>
	/// <returns>�X���b�v�`�F�C��</returns>
	IDXGISwapChain4* GetSwapChain() const { return m_pSwapChain.Get(); }

	/// <summary>
	/// �f�B�X�N���v�^�v�[���̎擾
	/// </summary>
	/// <param name="type">�f�B�X�N���v�^�v�[���̎��</param>
	/// <returns>�f�B�X�N���v�^�v�[��</returns>
	DescriptorPool* GetPool(DescriptorPoolType type) const { return m_pPool[type]; }

	/// <summary>
	/// �t���[���ԍ��̎擾
	/// </summary>
	/// <returns>�t���[���ԍ�</returns>
	uint32_t GetFrameIndex() const { return m_FrameIndex; }

	/// <summary>
	/// �J���[�^�[�Q�b�g�̎擾
	/// </summary>
	/// <param name="index">�C���f�b�N�X</param>
	/// <returns>�J���[�^�[�Q�b�g</returns>
	ColorTarget* GetColorTarget(uint32_t index) { return &m_ColorTarget[index]; }

	/// <summary>
	/// �[�x�^�[�Q�b�g�̎擾
	/// </summary>
	/// <returns>�[�x�^�[�Q�b�g</returns>
	DepthTarget* GetDepthTarget() { return &m_DepthTarget; }

	/// <summary>
	/// �r���[�|�[�g�̎擾
	/// </summary>
	/// <returns>�r���[�|�[�g</returns>
	const D3D12_VIEWPORT& GetViewport() const { return m_Viewport; }

	/// <summary>
	/// �V�U�[��`�̎擾
	/// </summary>
	/// <returns>�V�U�[��`</returns>
	const D3D12_RECT& GetScissor() const { return m_Scissor; }

	/// <summary>
	/// window�̎擾
	/// </summary>
	/// <returns>�E�B���h�E�N���X�̃|�C���^</returns>
	Window* GetWindow() const { return m_pWindow; }

	/// <summary>
	/// window�̏c���̎擾
	/// </summary>
	/// <returns>�E�B���h�E�̏c��</returns>
	float GetHeight() const { return static_cast<float>(m_pWindow->GetHeight()); }

	/// <summary>
	/// window�̉����̎擾
	/// </summary>
	/// <returns>�E�B���h�E�̉���</returns>
	float GetWidth() const { return static_cast<float>(m_pWindow->GetWidth()); }

private:	// �v���C�x�[�g�֐�
	void Present(uint32_t interval);
	void CheckSupportHDR();
	bool  IsSupportHDR() const;
	float GetMaxLuminance() const;
	float GetBaseLuminance() const;
	void DrawTonemap(ColorTarget* sceneTarget);

private:	// �����o�ϐ�
	// �E�B���h�E�֘A
	Window* m_pWindow = nullptr;					// �E�B���h�E�N���X�̃|�C���^
	ComPtr<IDXGIFactory4> m_pFactory;				// DXGI�t�@�N�g���[
	ComPtr<ID3D12Device> m_pDevice;					// �f�o�C�X
	ComPtr<ID3D12CommandQueue> m_pQueue;			// �R�}���h�L���[
	ComPtr<IDXGISwapChain4> m_pSwapChain;			// �X���b�v�`�F�C��
	ColorTarget m_ColorTarget[FRAME_BUFFER_COUNT];	// �J���[�^�[�Q�b�g
	DepthTarget m_DepthTarget;						// �[�x�^�[�Q�b�g
	DescriptorPool* m_pPool[POOL_COUNT];			// �f�B�X�N���v�^�v�[��
	CommandList m_CommandList;						// �R�}���h���X�g
	Fence m_Fence;									// �t�F���X
	uint32_t m_FrameIndex;							// �t���[���ԍ�
	D3D12_VIEWPORT m_Viewport;						// �r���[�|�[�g
	D3D12_RECT m_Scissor;							// �V�U�[��`
	DXGI_FORMAT m_BackBufferFormat;					// �o�b�N�o�b�t�@�t�H�[�}�b�g
	VertexBuffer m_QuadVB;							// ���_�o�b�t�@

	// �g�[���}�b�v�֘A
	RootSignature m_TonemapRootSig;						// �g�[���}�b�v�p���[�g�V�O�j�`��
	ConstantBuffer m_TonemapCB[FRAME_BUFFER_COUNT];		// �萔�o�b�t�@
	ComPtr<ID3D12PipelineState> m_pTonemapPSO;			// �p�C�v���C���X�e�[�g
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];	// �ϊ��p�o�b�t�@�ł�.
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];		// ���b�V���p�o�b�t�@�ł�.
	bool m_IsHDR;										// HDR���T�|�[�g���Ă��邩�ǂ���
	float m_MaxLuminance;								// �ő�P�x
	float m_BaseLuminance;								// �ŏ��P�x
	int m_TonemapType;									// �g�[���}�b�v�^�C�v
	int m_ColorSpace;									// �o�͐F���

	// �E�B���h�E�֘A
	bool m_IsFullScreen;	// �t���X�N���[�����ǂ���
	bool m_IsVSync;			// �����������s�����ǂ���

private:	// �V���O���g���֘A
	friend class Manager<Renderer>;
	Renderer(DXGI_FORMAT format)
		: m_BackBufferFormat(format)
		, m_FrameIndex(0)
		, m_IsFullScreen(false)
		, m_IsVSync(true)
		, m_IsHDR(false)
		, m_MaxLuminance(100.0f)
		, m_BaseLuminance(100.0f)
		, m_pWindow(nullptr)
		, m_pFactory(nullptr)
		, m_pDevice(nullptr)
		, m_pQueue(nullptr)
		, m_pSwapChain(nullptr)
		, m_CommandList()
		, m_Fence()
		, m_Viewport()
		, m_Scissor()
		, m_TonemapType(0)
		, m_ColorSpace(0)
		, m_QuadVB()
		, m_TonemapRootSig()
		, m_pTonemapPSO(nullptr)
		, m_TonemapCB()
		, m_TransformCB()
		, m_MeshCB()
		, m_pPool()
		, m_ColorTarget()
		, m_DepthTarget()
	{
		for (auto i = 0; i < POOL_COUNT; ++i)
		{
			m_pPool[i] = nullptr;
		}
	}

	static Renderer* createInstance()
	{
		return NEW Renderer(DXGI_FORMAT_R8G8B8A8_UNORM);
	}
};

// �C���X�^���X�̎擾
#define RENDERER Renderer::manager()
// �E�B���h�E�̏c���̎擾
#define HEIGHT RENDERER.GetHeight()
// �E�B���h�E�̉����̎擾
#define WIDTH RENDERER.GetWidth()



