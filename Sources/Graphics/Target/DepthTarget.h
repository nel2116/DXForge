// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [DepthTarget.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 17:00
// 概要   : 深度ターゲットクラスの定義
// 更新履歴
// 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <d3d12.h>
#include <System/ComPtr.h>
#include <cstdint>


// ====== クラス定義 ======
class DescriptorHandle;
class DescriptorPool;
class DepthTarget
{
public:	// パブリック関数
	// コンストラクタ
	DepthTarget();

	// デストラクタ
	~DepthTarget();

	/// <summary>
	/// 初期化処理
	///	</summary>
	/// <param name="pPoolDSV">ディスクリプタプール(DSV用)</param>
	/// <param name="pPoolSRV">ディスクリプタプール(SRV用)</param>
	/// <param name="width">幅</param>
	/// <param name="height">高さ</param>
	/// <param name="format">フォーマット</param>
	/// <param name="clearDepth">クリアする深度値</param>
	/// <param name="clearStencil">クリアするステンシル値</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(DescriptorPool* pPoolDSV, DescriptorPool* pPoolSRV, uint32_t width, uint32_t height, DXGI_FORMAT format, float clearDepth, uint8_t clearStencil);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// ビューをクリア
	/// </summary>
	void ClearView(ID3D12GraphicsCommandList* pCmdList);

public:	// アクセサ関数
	/// <summary>
	/// ディスクリプタハンドルを取得
	/// </summary>
	/// <returns>ディスクリプタハンドル</returns>
	DescriptorHandle* GetHandleDSV() const;

	/// <summary>
	/// リソースを取得
	/// </summary>
	/// <returns>リソース</returns>
	ID3D12Resource* GetResource() const;

	/// <summary>
	/// リソース設定を取得
	/// </summary>
	/// <returns>リソース設定</returns>
	D3D12_RESOURCE_DESC GetDesc() const;

	/// <summary>
	/// 深度ステンシルビューの設定を取得
	/// </summary>
	/// <returns>深度ステンシルビューの設定</returns>
	D3D12_DEPTH_STENCIL_VIEW_DESC GetDSVDesc() const;

	/// <summary>
	/// シェーダーリソースビューの設定を取得
	/// </summary>
	/// <returns>シェーダーリソースビューの設定</returns>
	D3D12_SHADER_RESOURCE_VIEW_DESC GetSRVDesc() const;

private:	// メンバ変数
	ComPtr<ID3D12Resource> m_pTarget;			// リソース
	DescriptorHandle* m_pHandleDSV;				// ディスクリプタハンドル(DSV用)
	DescriptorHandle* m_pHandleSRV;				// ディスクリプタハンドル(SRV用)
	DescriptorPool* m_pPoolDSV;					// ディスクリプタプール(DSV用)
	DescriptorPool* m_pPoolSRV;					// ディスクリプタプール(SRV用)
	D3D12_DEPTH_STENCIL_VIEW_DESC m_DSVDesc;	// 深度ステンシルビューの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC m_SRVDesc;	// シェーダリソースビューの設定
	float m_ClearDepth;
	uint8_t m_ClearStencil;

private:	// コピー禁止
	DepthTarget(const DepthTarget&) = delete;
	void operator = (const DepthTarget&) = delete;
};