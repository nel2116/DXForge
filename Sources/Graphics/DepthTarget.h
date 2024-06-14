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
	/// <param name="pPoolDSV">ディスクリプタプール</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="format">ピクセルフォーマット</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(DescriptorPool* pPoolDSV, uint32_t width, uint32_t height, DXGI_FORMAT format);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

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
	D3D12_DEPTH_STENCIL_VIEW_DESC GetViewDesc() const;

private:	// メンバ変数
	ComPtr<ID3D12Resource> m_pTarget;			// リソースです.
	DescriptorHandle* m_pHandleDSV;				// ディスクリプタハンドル(DSV用)です.
	DescriptorPool* m_pPoolDSV;					// ディスクリプタプール(DSV用)です.
	D3D12_DEPTH_STENCIL_VIEW_DESC m_ViewDesc;	// 深度ステンシルビューの設定です.

private:	// コピー禁止
	DepthTarget(const DepthTarget&) = delete;
	void operator = (const DepthTarget&) = delete;
};