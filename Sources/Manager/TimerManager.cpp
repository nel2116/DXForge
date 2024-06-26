#include "stdafx.h"
#include "TimerManager.h"

void TimerManager::Update()
{
	// サンプル数の設定
	SetSampleNum(10);
	m_dwCurrentTime = timeGetTime();
	m_dwDiffTime = m_dwCurrentTime - m_dwPreExecTime;
	m_fDeltaTime = m_dwDiffTime / 1000.0f;

	// FPSの計算
	double R[50];
	// 空回しによる計測誤差の測定
	for (int i = 0; i < 50; i++)
	{
		R[i] = 1.0 / GetFPS();
	}

}

void TimerManager::UpdateFrame()
{
	m_dwPreExecTime = m_dwCurrentTime;
	m_dwFrameCount++;
}

void TimerManager::Init()
{
	timeBeginPeriod(1);
	m_dwStartTime = m_dwCurrentTime = m_dwPreExecTime = timeGetTime();
}

void TimerManager::Uninit()
{
	timeEndPeriod(1);
}

float TimerManager::UpdateFPS(double Def)
{
	// 最も古いデータを消去
	m_dwDefTimeList.pop_front();

	// データを追加
	m_dwDefTimeList.push_back(Def);

	// FPSの計算
	double FPS = 0.0;
	double AveDef = (m_dwSumTimes + Def) / m_uiNum;
	if (AveDef != 0) { FPS = 1000.0f / AveDef; }

	// 合計時間の更新
	m_dwSumTimes += Def - *m_dwDefTimeList.begin();

	// FPSの更新
	return FPS;
}

double TimerManager::GetFPS()
{
	if (m_dwDiffTime == 0.0f) return 0.0;
	return UpdateFPS(m_dwDiffTime);
}

void TimerManager::SetSampleNum(UINT num)
{
	m_uiNum = num;						//	平均を計算する個数		
	m_dwDefTimeList.resize(num, 0.0);	//	リスト初期化
	m_dwSumTimes = 0.0;					//	合計時間初期化
}


