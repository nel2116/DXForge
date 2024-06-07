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
#include <System/window.h>

using namespace std;

// ====== �^��` ======
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

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
	ID3D12CommandList* GetCmdList() { return m_pCmdList.Get(); }

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
	// RTV�̐���
	bool CreateRTV();
	// �t�F���X�̐���
	bool CreateFence();

private:	// �����o�ϐ�
	// �t���[���o�b�t�@��
	static const int FRAME_BUFFER_COUNT = 2;

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

	// �����_�[�^�[�Q�b�g�r���[�֘A
	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;							// RTV�q�[�v
	D3D12_CPU_DESCRIPTOR_HANDLE m_RTVHandle[FRAME_BUFFER_COUNT] = {};	// RTV�n���h��
	ComPtr<ID3D12Resource> m_pColorBuffer[FRAME_BUFFER_COUNT];			// �J���[�o�b�t�@

	// �r���[�|�[�g�֘A
	D3D12_VIEWPORT m_viewport;	// �r���[�|�[�g
	D3D12_RECT m_scissorRect;	// �V�U�[��`

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