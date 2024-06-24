// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Fence.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 12:05 : 作成
// 概要   : フェンスクラスの定義
// 更新履歴
// 2024/06/14  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <System/ComPtr.h>
#include <d3d12.h>

// ====== クラス定義 ======
class Fence
{
public:
	// コンストラクタ
	Fence();
	// デストラクタ
	~Fence();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// シグナル状態になるまで指定された時間待機
	/// </summary>
	/// <param name="time">待機時間</param>
	void Wait(UINT timeout);

	/// <summary>
	/// シグナル状態になるまで待機
	/// </summary>
	void Sync();

private:	// メンバ変数
	ComPtr<ID3D12Fence> m_pFence;	// フェンス
	HANDLE m_hEvent;				// イベントハンドル
	UINT m_Counter;					// フェンスの値

private:
	Fence(Fence const&) = delete;
	Fence& operator=(Fence const&) = delete;
};