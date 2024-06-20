// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [ColorTarget.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 17:00
// 概要   : レンダーターゲットクラスの定義
// 更新履歴
// 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <d3d12.h>
#include <dxgi1_6.h>
#include <System/ComPtr.h>
#include <cstdint>

// ====== クラス定義 ======
class DescriptorHandle;
class DescriptorPool;
class ColorTarget
{
public:		// パブリック関数
	// コンストラクタ
	ColorTarget();

	// デストラクタ
	~ColorTarget();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="pPoolRTV">ディスクリプタプール</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="format">ピクセルフォーマット</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(DescriptorPool* pPoolRTV, uint32_t width, uint32_t height, DXGI_FORMAT format, bool useSRGB);

	/// <summary>
	/// バックバッファから初期化処理を行う
	/// </summary>
	/// <param name="pPoolRTV">ディスクリプタプール</param>
	/// <param name="index">バックバッファ番号</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool InitFromBackBuffer(DescriptorPool* pPoolRTV, uint32_t index, bool useSRGB);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// ディスクリプタハンドル(RTV用)を取得する
	/// </summary>
	/// <returns>ディスクリプタハンドル(RTV用)</returns>
	DescriptorHandle* GetHandleRTV() const;

	/// <summary>
	/// リソースを取得する
	/// </summary>
	/// <returns>リソース</returns>
	ID3D12Resource* GetResource() const;

	/// <summary>
	/// リソース設定を取得する
	/// </summary>
	/// <returns>リソース設定</returns>
	D3D12_RESOURCE_DESC GetDesc() const;

	/// <summary>
	/// レンダーターゲットビューの設定を取得する
	/// </summary>
	/// <returns>レンダーターゲットビューの設定</returns>
	D3D12_RENDER_TARGET_VIEW_DESC GetViewDesc() const;

private:		// メンバ変数
	ComPtr<ID3D12Resource> m_pTarget;			// リソース
	DescriptorHandle* m_pHandleRTV;				// ディスクリプタハンドル(RTV用)
	DescriptorPool* m_pPoolRTV;					// ディスクリプタプール(RTV用)
	D3D12_RENDER_TARGET_VIEW_DESC m_ViewDesc;	// レンダーターゲットビューの構成

private:		// コピー禁止
	ColorTarget(const ColorTarget&) = delete;
	void operator = (const ColorTarget&) = delete;
};