#pragma once
// ====== インクルード部 ======
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// ====== 名前空間定義 ======
using namespace DirectX;

// ====== 定数定義 ======
const int kInputLayoutNum = 1;	// インプットレイアウトの数

// ====== クラス定義 ======
class Renderer
{
public:		// パブリック関数
	Renderer();
	~Renderer();

	// 初期化
	bool Init();

	void Draw();

private:	// プライベート関数
	void EnableDebugLayer();	    // デバッグレイヤーの有効化
	bool CreateDevice();	        // デバイスの作成
	bool CreateCommandList();	    // コマンドリストの作成
	bool CreateSwapChain();	        // スワップチェインの作成
	bool CreateRenderTargetView();	// レンダーターゲットビューの作成
	bool CreateFence();	            // フェンスの作成
	bool CreateVertexBuffer();	    // 頂点バッファの作成
	bool CreateShader();	        // シェーダーの作成
	bool CreateInputLayout();	    // インプットレイアウトの作成
	bool CreateRootSignature();	    // ルートシグネチャの作成
	bool CreatePipelineState();	    // パイプラインステートの作成
	bool CreateViewport();	        // ビューポートの作成


private:	// メンバ変数
	// DirectX12の初期化

	// デバイス
	ID3D12Device* m_dev;
	IDXGIFactory6* m_dxgiFactory;
	IDXGISwapChain4* m_swapChain;

	// コマンドリスト
	ID3D12CommandAllocator* m_cmdAllocator;
	ID3D12GraphicsCommandList* m_cmdList;
	ID3D12CommandQueue* m_cmdQueue;

	// ディスクリプタヒープ
	ID3D12DescriptorHeap* m_pRTV;
	vector<ID3D12Resource*> m_backBuffers;

	// フェンス
	ID3D12Fence* m_fence;
	UINT64 m_fenceVal;

	// 頂点情報
	ID3D12Resource* m_vertexBuff;	            // 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW m_vertexBuffView;	// 頂点バッファビュー
	ID3D12Resource* m_indexBuff;	            // インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW m_indexBuffView;	// インデックスバッファビュー

	// シェーダー
	ID3DBlob* m_vsBlob;
	ID3DBlob* m_psBlob;

	// インプットレイアウト
	D3D12_INPUT_ELEMENT_DESC m_inputLayout[kInputLayoutNum];

	// ルートシグネチャ
	ID3D12RootSignature* m_rootSignature;

	// パイプラインステート
	ID3D12PipelineState* m_pipelineState;

	// ビューポート
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

};