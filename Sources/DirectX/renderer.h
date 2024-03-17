#pragma once
// ====== �C���N���[�h�� ======
#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

// ====== �N���X��` ======
class Renderer
{
public:		// �p�u���b�N�֐�
	Renderer();
	~Renderer();

	// ������
	bool Init();

	void Draw();

private:	// �v���C�x�[�g�֐�
	void EnableDebugLayer();	// �f�o�b�O���C���[�̗L����
	bool CreateDevice();	// �f�o�C�X�̍쐬
	bool CreateCommandList();	// �R�}���h���X�g�̍쐬
	bool CreateSwapChain();	// �X���b�v�`�F�C���̍쐬
	bool CreateRenderTargetView();	// �����_�[�^�[�Q�b�g�r���[�̍쐬
	bool CreateFence();	// �t�F���X�̍쐬


private:	// �����o�ϐ�
	// DirectX12�̏�����

	// �f�o�C�X
	ID3D12Device* m_dev;
	IDXGIFactory6* m_dxgiFactory;
	IDXGISwapChain4* m_swapChain;

	// �R�}���h���X�g
	ID3D12CommandAllocator* m_cmdAllocator;
	ID3D12GraphicsCommandList* m_cmdList;
	ID3D12CommandQueue* m_cmdQueue;

	// �f�B�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* m_pRTV;
	vector<ID3D12Resource*> m_backBuffers;

	// �t�F���X
	ID3D12Fence* m_fence;
	UINT64 m_fenceVal;

};