#pragma once
// ====== �C���N���[�h�� ======
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// ====== ���O��Ԓ�` ======
using namespace DirectX;

// ====== �萔��` ======
const int kInputLayoutNum = 1;	// �C���v�b�g���C�A�E�g�̐�

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
	void EnableDebugLayer();	    // �f�o�b�O���C���[�̗L����
	bool CreateDevice();	        // �f�o�C�X�̍쐬
	bool CreateCommandList();	    // �R�}���h���X�g�̍쐬
	bool CreateSwapChain();	        // �X���b�v�`�F�C���̍쐬
	bool CreateRenderTargetView();	// �����_�[�^�[�Q�b�g�r���[�̍쐬
	bool CreateFence();	            // �t�F���X�̍쐬
	bool CreateVertexBuffer();	    // ���_�o�b�t�@�̍쐬
	bool CreateShader();	        // �V�F�[�_�[�̍쐬
	bool CreateInputLayout();	    // �C���v�b�g���C�A�E�g�̍쐬
	bool CreateRootSignature();	    // ���[�g�V�O�l�`���̍쐬
	bool CreatePipelineState();	    // �p�C�v���C���X�e�[�g�̍쐬
	bool CreateViewport();	        // �r���[�|�[�g�̍쐬


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

	// ���_���
	ID3D12Resource* m_vertexBuff;	            // ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW m_vertexBuffView;	// ���_�o�b�t�@�r���[
	ID3D12Resource* m_indexBuff;	            // �C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW m_indexBuffView;	// �C���f�b�N�X�o�b�t�@�r���[

	// �V�F�[�_�[
	ID3DBlob* m_vsBlob;
	ID3DBlob* m_psBlob;

	// �C���v�b�g���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC m_inputLayout[kInputLayoutNum];

	// ���[�g�V�O�l�`��
	ID3D12RootSignature* m_rootSignature;

	// �p�C�v���C���X�e�[�g
	ID3D12PipelineState* m_pipelineState;

	// �r���[�|�[�g
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

};