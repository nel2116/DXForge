// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : �����_���[�N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <System/window.h>
#include <System/ComPtr.h>
#include <Graphics/ColorTarget.h>
#include <Graphics/DepthTarget.h>

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
class Renderer
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
	void End();

public:		// �A�N�Z�T�֐�
	/// <summary>
	/// �f�o�C�X�̎擾
	/// </summary>
	/// <returns>�f�o�C�X�̎Q��</returns>
	ID3D12Device* GetDevice() { return m_pDevice.Get(); }

	/// <summary>
	/// �R�}���h���X�g�̎擾
	///	</summary>
	/// <returns>�R�}���h���X�g�̎Q��</returns>
	ID3D12GraphicsCommandList* GetCmdList() { return m_pCmdList.Get(); }

	/// <summary>
	/// �R�}���h�L���[�̎擾
	/// </summary>
	/// <returns>�R�}���h�L���[�̎Q��</returns>
	ID3D12CommandQueue* GetCmdQueue() { return m_pCmdQueue.Get(); }

	/// <summary>
	/// �X���b�v�`�F�C���̎擾
	/// </summary>
	/// <returns>�X���b�v�`�F�C���̎Q��</returns>
	IDXGISwapChain3* GetSwapChain() { return m_pSwapChain.Get(); }

	/// <summary>
	/// �t���[���o�b�t�@�̃C���f�b�N�X���擾
	/// </summary>
	/// <returns>�t���[���o�b�t�@�̃C���f�b�N�X</returns>
	uint32_t GetFrameIndex() { return m_FrameIndex; }

	/// <summary>
	/// �J���[�^�[�Q�b�g�̎擾
	/// </summary>
	/// <param name="index">�J���[�^�[�Q�b�g�̃C���f�b�N�X</param>
	/// <returns>�J���[�^�[�Q�b�g</returns>
	ColorTarget& GetColorTarget(uint32_t index) { return m_ColorTarget[index]; }

	/// <summary>
	/// �E�B���h�E�̉������擾
	/// </summary>
	/// <returns>�E�B���h�E�̉���</returns>
	int GetWidth() { return m_pWindow->GetWidth(); }

	/// <summary>
	/// �E�B���h�E�̏c�����擾
	/// </summary>
	/// <returns>�E�B���h�E�̏c��</returns>
	int GetHeight() { return m_pWindow->GetHeight(); }

	/// <summary>
	/// �r���[�|�[�g�̎擾
	/// </summary>
	/// <returns>�r���[�|�[�g</returns>
	D3D12_VIEWPORT GetViewport() { return m_viewport; }

	/// <summary>
	/// �V�U�[��`�̎擾
	/// </summary>
	/// <returns>�V�U�[��`</returns>
	D3D12_RECT GetScissor() { return m_scissor; }

	/// <summary>
	/// �f�B�X�N���v�^�v�[���̎擾
	/// </summary>
	/// <param name="type">�f�B�X�N���v�^�v�[���̎��</param>
	/// <returns>�f�B�X�N���v�^�v�[���̎Q��</returns>
	DescriptorPool* GetDescriptorPool(DescriptorPoolType type) { return m_pPool[type]; }

	/// <summary>
	/// �f�v�X�^�[�Q�b�g�̎擾
	/// </summary>
	/// <returns>�f�v�X�^�[�Q�b�g</returns>
	DepthTarget& GetDepthTarget() { return m_DepthTarget; }

private:	// �v���C�x�[�g�֐�

	// ��ʃt���b�v
	void Present(uint32_t interval);
	// GPU�̏���������҂�
	void WaitGpu();

	// �f�o�C�X�̐���
	bool CreateDevice();
	// �R�}���h�L���[�̐���
	bool CreateCommandQueue();
	// �X���b�v�`�F�C���̐���
	bool CreateSwapChain();
	// �R�}���h�A���P�[�^�̐���
	bool CreateCommandAllocator();
	// �R�}���h���X�g�̐���
	bool CreateCommandList();
	// �t�F���X�̐���
	bool CreateFence();
	// �����_�[�^�[�Q�b�g�r���[�̐���
	bool CreateRenderTargetView();
	// �[�x�X�e���V���r���[�̐���
	bool CreateDepthStencilView();
	// �f�B�X�N���v�^�v�[���̐���
	bool CreateDescriptorPool();

private:	// �����o�ϐ�

	// �E�B���h�E�֘A
	Window* m_pWindow = nullptr;	// �E�B���h�E�N���X�̃|�C���^

	// �f�o�C�X�֘A
	ComPtr<ID3D12Device> m_pDevice;										// �f�o�C�X
	ComPtr<ID3D12CommandQueue> m_pCmdQueue;								// �R�}���h�L���[
	ComPtr<IDXGISwapChain3> m_pSwapChain;								// �X���b�v�`�F�C��
	ComPtr<ID3D12CommandAllocator> m_pCmdAllocator[FRAME_BUFFER_COUNT];	// �R�}���h�A���P�[�^
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;						// �R�}���h���X�g

	// �t�F���X�֘A
	ComPtr<ID3D12Fence> m_pFence;										// �t�F���X
	HANDLE m_fenceEvent = nullptr;										// �t�F���X�C�x���g
	uint64_t m_FenceCounter[FRAME_BUFFER_COUNT] = {};					// �t�F���X�J�E���^
	uint32_t m_FrameIndex = 0;											// �t���[���ԍ�

	// �o�b�N�o�b�t�@�֘A
	ColorTarget m_ColorTarget[FRAME_BUFFER_COUNT];	// �J���[�^�[�Q�b�g
	DepthTarget m_DepthTarget;						// �[�x�^�[�Q�b�g

	DescriptorPool* m_pPool[POOL_COUNT];	// �f�B�X�N���v�^�v�[��

	// �r���[�|�[�g�֘A
	D3D12_VIEWPORT m_viewport;	// �r���[�|�[�g
	D3D12_RECT m_scissor;	// �V�U�[��`

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

#define RENDERER Renderer::Instance()	// �����_���[�̃C���X�^���X���擾
#define WIDTH RENDERER.GetWidth()		// �E�B���h�E�̕�
#define HEIGHT RENDERER.GetHeight()		// �E�B���h�E�̍���