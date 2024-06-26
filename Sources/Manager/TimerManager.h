// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [TimerManager.h]
// 制作者 : 田中ミノル
// 制作日 : 2024/06/26 21:30
// 概要   : TimerManagerクラスの定義	:	タイマーマネージャクラス
// 更新日
// 2024/06/26 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Manager.h"

// ====== 定数定義 ======
const float FPS = 60.0f;				// フレームレート
const float FRAME_TIME = 1000.0f / FPS;	// フレーム時間

// ====== クラスの定義 ======
class TimerManager : public Manager<TimerManager>
{
public:	// メンバ関数
	// フレーム実行前の更新
	void Update();
	// フレーム実行後の更新
	void UpdateFrame();
	// 初期化
	void Init();
	// 終了
	void Uninit();

	// フレームレートの更新
	float UpdateFPS(double Def);

	/// <summary>
	/// デルタタイムの取得
	/// </summary>
	/// <returns>デルタタイム</returns>
	float GetDeltaTime() { return m_fDeltaTime; }

	/// <summary>
	/// フレームカウントの取得
	/// </summary>
	/// <returns>フレームカウント</returns>
	DWORD GetFrameCount() { return m_dwFrameCount; }

	/// <summary>
	/// 実行開始時間の取得
	/// </summary>
	/// <returns>実行開始時間</returns>
	DWORD GetStartTime() { return m_dwStartTime; }

	/// <summary>
	/// 現在時間の取得
	/// </summary>
	/// <returns>現在時間</returns>
	DWORD GetCurrentTime() { return m_dwCurrentTime; }

	/// <summary>
	/// 前回実行時間の取得
	/// </summary>
	/// <returns>前回実行時間</returns>
	DWORD GetPreExecTime() { return m_dwPreExecTime; }

	/// <summary>
	/// 差分時間の取得
	/// </summary>
	/// <returns>差分時間</returns>
	DWORD GetDiffTime() { return m_dwDiffTime; }

	/// <summary>
	/// フレームが進むかどうかの取得
	/// </summary>
	/// <returns>フレームが進むかどうか</returns>
	bool IsFrameGo() { return m_dwDiffTime >= FRAME_TIME; }

	/// <summary>
	/// フレームレートの取得
	/// </summary>
	/// <returns>フレームレート</returns>
	double GetFPS();

	/// <summary>
	/// サンプル数を変更
	/// </summary>
	void SetSampleNum(UINT num);

private:
	DWORD m_dwStartTime;			// 開始時間
	DWORD m_dwCurrentTime;			// 現在時間
	DWORD m_dwPreExecTime;			// 前回実行時間
	DWORD m_dwDiffTime;				// 前回フレームからの経過時間(ミリ秒)
	float m_fDeltaTime;				// 前回フレームからの経過時間(秒)
	DWORD m_dwFrameCount;			// ゲーム開始から現在までの総フレーム数

	// フレームレート関連
	float m_fFrameRate;				// フレームレート
	list<double> m_dwDefTimeList;	// フレーム時間のリスト
	UINT m_uiNum;					// 移動平均計算時のデータ
	double m_dwSumTimes;			// 共通部分の合計

private: // シングルトン関連
	friend class Manager<TimerManager>;
	TimerManager()
		: m_dwStartTime(0)
		, m_dwCurrentTime(0)
		, m_dwPreExecTime(0)
		, m_dwDiffTime(0)
		, m_fDeltaTime(0.0f)
		, m_dwFrameCount(0)
		, m_fFrameRate(0.0f)
		, m_uiNum(0)
		, m_dwSumTimes(0.0)
		, m_dwDefTimeList()
	{}

	static TimerManager* createInstance()
	{
		return NEW TimerManager();
	}
};
// インスタンスを取得
#define TIMER_MANAGER TimerManager::manager()
