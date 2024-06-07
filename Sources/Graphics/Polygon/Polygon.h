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
class Polygon
{
public:	// パブリック関数
	bool Init();	// 初期化
	void Draw();	// 描画

private:	// プライベート関数

private:	// メンバ変数
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		// CBVヒープ
	ComPtr<ID3D12Resource> m_pVB;					// 頂点バッファ
	D3D12_VERTEX_BUFFER_VIEW m_vbView;				// 頂点バッファビュー
	ComPtr<ID3D12Resource> m_pCB[2];				//
	ConstantBufferView<Transform> m_CBV[2];			// 定数バッファビュー
	ComPtr<ID3D12RootSignature> m_pRootSignature;	// ルートシグネチャ
	ComPtr<ID3D12PipelineState> m_pPSO;				// パイプラインステート
	float m_RotateAngle;							// 回転角度
};