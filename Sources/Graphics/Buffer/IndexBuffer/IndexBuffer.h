// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [IndexBuffer.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 11:30
// 概要   : インデックスバッファクラスの定義
// 更新履歴
// 2024/06/14 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <d3d12.h>
#include <System/ComPtr.h>
#include <cstdint>


// ====== クラス定義 ======
class IndexBuffer
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	IndexBuffer();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~IndexBuffer();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="count">インデックス数</param>
	/// <param name="pInitData">初期データ</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(size_t count, const uint32_t* pInitData = nullptr);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// メモリマッピング
	/// </summary>
	uint32_t* Map();

	/// <summary>
	/// メモリマッピング解除
	/// </summary>
	void Unmap();

	/// <summary>
	/// インデックスバッファビューの取得
	/// </summary>
	/// <returns>インデックスバッファビュー</returns>
	D3D12_INDEX_BUFFER_VIEW GetView() const;

	/// <summary>
	/// インデックス数を取得
	/// </summary>
	/// <returns>インデックス数</returns>
	size_t GetCount() const;

private:		// メンバ変数
	ComPtr<ID3D12Resource> m_pIB;	// インデックスバッファ
	D3D12_INDEX_BUFFER_VIEW m_View;	// インデックスバッファビュー
	size_t m_Count;					// インデックス数

private:		// コピー禁止
	IndexBuffer(const IndexBuffer&) = delete;
	void operator = (const IndexBuffer&) = delete;
};
