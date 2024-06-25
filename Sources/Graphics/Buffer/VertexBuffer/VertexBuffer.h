// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [VertexBuffer.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 11:30
// 概要   : 頂点バッファクラスの定義
// 更新履歴
// 2024/06/14 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <d3d12.h>
#include <System/ComPtr.h>

// ====== クラス定義 ======
class VertexBuffer
{
public:
	// コンストラクタ
	VertexBuffer();
	// デストラクタ
	~VertexBuffer();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="size">頂点バッファサイズ</param>
	/// <param name="stride">1頂点あたりのサイズ</param>
	/// <param name="pInitData">初期化データ</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(size_t size, size_t stride, const void* pInitData = nullptr);

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="size">頂点バッファサイズ</param>
	/// <param name="pInitData">初期化データ</param>
	/// <returns>初期化に成功したらtrue</returns>
	template<typename T>
	bool Init(size_t size, const T* pInitData = nullptr)
	{
		return Init(sizeof(T) * size, sizeof(T), pInitData);
	}

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// メモリマッピング
	/// </summary>
	void* Map() const;

	/// <summary>
	/// メモリマッピングを解除
	/// </summary>
	void Unmap();

	/// <summary>
	/// メモリマッピング
	/// </summary>
	template<typename T>
	T* Map() const
	{
		return reinterpret_cast<T*>(Map());
	}

	/// <summary>
	/// 頂点バッファビューを取得
	/// </summary>
	/// <returns>頂点バッファビュー</returns>
	D3D12_VERTEX_BUFFER_VIEW GetView() const;

private:	// メンバ変数
	ComPtr<ID3D12Resource>      m_pVB;      // 頂点バッファです.
	D3D12_VERTEX_BUFFER_VIEW    m_View;     // 頂点バッファビューです.

private:
	VertexBuffer(const VertexBuffer&) = delete;     // アクセス禁止.
	void operator = (const VertexBuffer&) = delete;     // アクセス禁止.
};
