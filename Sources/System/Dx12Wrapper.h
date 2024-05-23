// [Dx12Wrapper.h]
// �쐬�� : 2024/05/21 17:00
// �쐬�� : �c���~�m��
// �T�v   : DirectX 12���b�p�[�N���X�̒�`
#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <map>
#include <unordered_map>
#include <DirectXTex.h>
#include <wrl.h>
#include <string.h>
#include <functional>

// 
class Dx12Wrapper
{
	// using�錾
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

private:	// �\���̒�`
	struct SceneData
	{
		DirectX::XMMATRIX view;	// �r���[�s��
		DirectX::XMMATRIX proj;	// �v���W�F�N�V�����s��
		DirectX::XMFLOAT3 eye;	// ���_���W
	};

public:		// �����o�֐�
	Dx12Wrapper(HWND hwnd);
	~Dx12Wrapper();

	void Update();
	void BeginDraw();
	void EndDraw();

	/// <summary>
	/// �e�N�X�`���p�X����K�v�ȃe�N�X�`���o�b�t�@�ւ̃|�C���^��Ԃ�
	/// @param texPath : �e�N�X�`���̃p�X
	/// </summary>
	ComPtr<ID3D12Resource> GetTextureByPath(const char* texPath);

	/// <summary>
	/// �f�o�C�X
	/// </summary>
	ComPtr<ID3D12Device> GetDevice();

	/// <summary>
	/// �R�}���h���X�g
	/// </summary>
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();

	/// <summary>
	/// �X���b�v�`�F�C��
	/// <summary>
	ComPtr<IDXGISwapChain4> GetSwapChain();

	void SetScene();


private:	// �v���C�x�[�g�֐�
	// �ŏI�I�ȃ����_�[�^�[�Q�b�g�̐���
	HRESULT CreateFinalRenderTargets();
	// �f�v�X�X�e���V���r���[�̐���
	HRESULT CreateDepthStencilView();

	// �X���b�v�`�F�C���̐���
	HRESULT CreateSwapChain(const HWND& hwnd);

	// DXGI�֘A�̏�����
	HRESULT InitDXGIDevice();

	// �R�}���h�֘A�̏�����
	HRESULT InitCommand();

	// �r���[�v���W�F�N�V�����p�r���[�̐���
	HRESULT CreateSceneView();

	// �e�N�X�`�����[�_�e�[�u���̍쐬
	void CreateTextureLoaderTable();

	// �e�N�X�`��������e�N�X�`���o�b�t�@�쐬�A���g���R�s�[
	ID3D12Resource* CreateTextureFromFile(const char* texPath);

private:	// �����o�ϐ�
	SIZE m_winSize;	// �E�B���h�E�T�C�Y

	// DXGI�֘A
	ComPtr<IDXGIFactory4> m_dxgiFactory = nullptr;	// DXGI�C���^�[�t�F�C�X
	ComPtr<IDXGISwapChain4> m_swapChain = nullptr;	// �X���b�v�`�F�C��

	// DirectX12�֘A
	ComPtr<ID3D12Device> m_device = nullptr;					// �f�o�C�X
	ComPtr<ID3D12CommandAllocator> m_cmdAllocator = nullptr;	// �R�}���h�A���P�[�^
	ComPtr<ID3D12GraphicsCommandList> m_cmdList = nullptr;		// �R�}���h���X�g
	ComPtr<ID3D12CommandQueue> m_cmndQueue = nullptr;			// �R�}���h�L���[

	// �\���Ɋւ��o�b�t�@�֘A
	ComPtr<ID3D12Resource> m_depthBuffer = nullptr;		// �[�x�o�b�t�@
	vector<ID3D12Resource*> m_backBuffers;				// �o�b�N�o�b�t�@
	ComPtr<ID3D12DescriptorHeap> m_rtvHeaps = nullptr;	// �����_�[�^�[�Q�b�g�r���[�q�[�v
	ComPtr<ID3D12DescriptorHeap> m_dsvHeap = nullptr;	// �[�x�X�e���V���r���[�q�[�v
	unique_ptr<D3D12_VIEWPORT> m_viewport;				// �r���[�|�[�g
	unique_ptr<D3D12_RECT> m_scissorRect;				// �V�U�[��`

	// �V�[�����\������o�b�t�@�֘A
	ComPtr<ID3D12Resource> m_sceneConstBuff = nullptr;	// �V�[���萔�o�b�t�@
	SceneData* m_mappedSceneData;						// �}�b�v���ꂽ�V�[���f�[�^
	ComPtr<ID3D12DescriptorHeap> m_sceneDescHeap = nullptr;	// �V�[���f�X�N���v�^�q�[�v


	// �t�F���X
	ComPtr<ID3D12Fence> m_fence = nullptr;	// �t�F���X
	UINT64 m_fenceVal = 0;					// �t�F���X�̒l

	// ���[�h�p�e�[�u��
	using LoadLambda_t = function<HRESULT(const wstring& path, DirectX::TexMetadata*, DirectX::ScratchImage&)>;
	map<string, LoadLambda_t> m_loadLambdaTable;
	// �e�N�X�`���e�[�u��
	unordered_map<string, ComPtr<ID3D12Resource>> m_textureTable;
};