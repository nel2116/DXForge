#pragma once
// ====== インクルード部 ======
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "DirectXTex/DirectXTex.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DirectXTex.lib")

// ====== 名前空間定義 ======
using namespace DirectX;

// ====== 定数定義 ======
const int kInputLayoutNum = 6;	// インプットレイアウトの数
const size_t pmdvertex_size = 38;	// 頂点１つあたりのサイズ

// ====== クラス定義 ======
class Renderer
{
public:	// 構造体定義
	// 頂点情報
	struct Vertex
	{
		XMFLOAT3 pos;	// 座標
		XMFLOAT2 uv;	// テクスチャ座標
	};

	struct TexRGBA
	{
		unsigned char r, g, b, a;
	};

	// PMDヘッダー構造体
	struct PMDHeader
	{
		float version;			// 例: 00 00 80 3F == 1.00
		char model_name[20];	// モデル名
		char comment[256];		// モデルコメント
	};

	// PMD頂点構造体
	struct PMDVertex
	{
		XMFLOAT3 pos;	// 頂点座標：12バイト
		XMFLOAT3 normal;	// 法線ベクトル：12バイト
		XMFLOAT2 uv;	// UV座標：8バイト
		unsigned short bone_No[2];	// ボーン番号：2 * 2バイト
		unsigned char bone_weight;	// ボーンの重み：1バイト
		unsigned char edge_flag;	// エッジフラグ：1バイト
	};	// 合計：38バイト

public:		// パブリック関数
	Renderer();
	~Renderer();

	// 初期化
	bool Init();
	void Update();
	void Draw();

private:	// プライベート関数
	void EnableDebugLayer();			// デバッグレイヤーの有効化
	bool CreateDevice();				// デバイスの作成
	bool CreateCommandList();			// コマンドリストの作成
	bool CreateSwapChain();				// スワップチェインの作成
	bool CreateRenderTargetView();		// レンダーターゲットビューの作成
	bool CreateFence();					// フェンスの作成
	bool CreateVertexBuffer();			// 頂点バッファの作成
	bool CreateShader();				// シェーダーの作成
	bool CreateInputLayout();			// インプットレイアウトの作成
	bool CreateRootSignature();			// ルートシグネチャの作成
	bool CreatePipelineState();			// パイプラインステートの作成
	bool CreateViewport();				// ビューポートの作成
	bool CreateTestTexture();			// テストテクスチャの作成
	bool CreateShaderResourceView();	// シェーダーリソースビューの作成
	bool CreateTexture();				// テクスチャの作成
	bool CreateConstantBuffer();		// 定数バッファの作成
	bool LoadModel();					// モデルの読み込み

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

	// テストテクスチャ
	vector<TexRGBA> m_texData;
	ID3D12Resource* m_texBuff;
	ID3D12DescriptorHeap* m_pBasicDescHeap;

	// 定数バッファ
	ID3D12Resource* m_constBuff;

	// 行列
	XMMATRIX* m_pMapMat;

	// モデル
	PMDHeader m_pmdHeader;
	unsigned int vertNum;	// 頂点数
	vector<unsigned char> vertices;	// 頂点データ
};