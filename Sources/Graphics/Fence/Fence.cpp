// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Fence.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 12:05 : 作成
// 概要   : フェンスクラスの実装
// 更新履歴
// 2024/06/14  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Fence.h"

// コンストラクタ
Fence::Fence()
	: m_pFence(nullptr)
	, m_hEvent(nullptr)
	, m_Counter(0)
{
}

// デストラクタ
Fence::~Fence()
{
	Uninit();
}

// 初期化処理
bool Fence::Init()
{
	auto pDevice = RENDERER.GetDevice();
	if (pDevice == nullptr)
	{
		return false;
	}

	// イベントを生成.
	m_hEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_hEvent == nullptr)
	{
		return false;
	}

	// フェンスを生成.
	auto hr = pDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_pFence.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// カウンタ設定.
	m_Counter = 1;

	// 正常終了.
	return true;
}

// 終了処理
void Fence::Uninit()
{
	if (m_hEvent != nullptr)
	{
		CloseHandle(m_hEvent);
		m_hEvent = nullptr;
	}

	if (m_pFence != nullptr)
	{
		m_pFence.Reset();
	}

	m_Counter = 0;
}

// シグナル状態になるまで指定された時間待機
void Fence::Wait(UINT timeout)
{
	// デバイスの取得
	auto pCmdQueue = RENDERER.GetCmdQueue();
	if (pCmdQueue == nullptr)
	{
		return;
	}

	const auto fenceValue = m_Counter;

	// シグナル処理
	auto hr = pCmdQueue->Signal(m_pFence.Get(), fenceValue);
	if (FAILED(hr))
	{
		return;
	}

	// カウンターを増やす
	m_Counter++;

	// 次のフレームの描画準備がまだであれば待機
	if (m_pFence->GetCompletedValue() < fenceValue)
	{
		// 完了時にイベントを設定
		auto hr = m_pFence->SetEventOnCompletion(fenceValue, m_hEvent);
		if (FAILED(hr))
		{
			return;
		}

		// 待機処理
		if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_hEvent, timeout, FALSE))
		{
			return;
		}
	}
}

// シグナル状態になるまで待機
void Fence::Sync()
{
	auto pCmdQueue = RENDERER.GetCmdQueue();
	if (pCmdQueue == nullptr)
	{
		return;
	}

	// シグナル処理
	auto hr = pCmdQueue->Signal(m_pFence.Get(), m_Counter);
	if (FAILED(hr))
	{
		return;
	}

	// 完了時にイベントを設定
	hr = m_pFence->SetEventOnCompletion(m_Counter, m_hEvent);
	if (FAILED(hr))
	{
		return;
	}

	// 待機処理
	if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_hEvent, INFINITE, FALSE))
	{
		return;
	}

	// カウンターを増やす
	m_Counter++;
}

