// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/07
// 作成者 : 田中ミノル
// 概要   : ポリゴンクラスの定義
// 更新履歴 : 2024/06/07 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <wrl/client.h>

// ====== 型定義 ======
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// ====== クラス定義 ======
class MyPolygon
{
public:	// 構造体定義
	// 頂点構造体
	struct Vertex
	{
		DirectX::XMFLOAT3 pos;	// 座標
		DirectX::XMFLOAT4 color;	// 色
	};

	// 定数バッファ構造体
	// alignas(256) : アライメントを256バイトに設定
	struct alignas(256) Transform
	{
		DirectX::XMMATRIX mWorld;	// ワールド行列
		DirectX::XMMATRIX mView;	// ビュー行列
		DirectX::XMMATRIX mProj;	// プロジェクション行列
	};

	template <typename T>
	struct ConstantBufferView
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC Desc; // 定数バッファの構成設定
		D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPUディスクリプタハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPUディスクリプタハンドル
		T* pBuffer;	// バッファ先頭へのポインタ
	};

public:	// パブリック関数
	bool Init();	// 初期化
	void Draw();	// 描画
	void Uninit();	// 終了処理

private:	// プライベート関数

private:	// メンバ変数
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		// CBVヒープ
	ComPtr<ID3D12Resource> m_pVB;					// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW m_vbView;				// 頂点バッファビュー
	ComPtr<ID3D12Resource> m_pIB;					// インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW m_ibView;				// インデックスバッファビュー
	ComPtr<ID3D12Resource> m_pCB[4];				// 定数バッファ
	ConstantBufferView<Transform> m_CBV[4];			// 定数バッファビュー
	ComPtr<ID3D12RootSignature> m_pRootSignature;	// ルートシグネチャ
	ComPtr<ID3D12PipelineState> m_pPSO;				// パイプラインステート
	float m_RotateAngle;							// 回転角度
	// ビューポート関連
	D3D12_VIEWPORT m_viewport;	// ビューポート
	D3D12_RECT m_scissorRect;	// シザー矩形
};