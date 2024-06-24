// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [CommandList.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/24 13:05 : 作成
// 概要   : コマンドリストクラスの実装
// 更新履歴
// 2024/06/24  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "CommandList.h"

// コンストラクタ
CommandList::CommandList()
	: m_pCmdList(nullptr)
	, m_pAllocators()
	, m_Index(0)
{
}

// デストラクタ
CommandList::~CommandList()
{
	Uninit();
}

// 初期化処理
bool CommandList::Init(D3D12_COMMAND_LIST_TYPE type, size_t count)
{
	auto pDevice = RENDERER.GetDevice();
	if (pDevice == nullptr || count == 0)
	{
		return false;
	}

	m_pAllocators.resize(count);

	for (auto i = 0u; i < count; ++i)
	{
		auto hr = pDevice->CreateCommandAllocator(
			type, IID_PPV_ARGS(m_pAllocators[i].GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}
	}

	{
		auto hr = pDevice->CreateCommandList(
			1,
			type,
			m_pAllocators[0].Get(),
			nullptr,
			IID_PPV_ARGS(m_pCmdList.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		m_pCmdList->Close();
	}

	m_Index = 0;
	return true;
}

// 終了処理
void CommandList::Uninit()
{
	m_pCmdList.Reset();

	for (size_t i = 0; i < m_pAllocators.size(); ++i)
	{
		m_pAllocators[i].Reset();
	}

	m_pAllocators.clear();
	m_pAllocators.shrink_to_fit();
}

ID3D12GraphicsCommandList* CommandList::Get() const
{
	return m_pCmdList.Get();
}

// リセット処理を行ったコマンドリストを取得
ID3D12GraphicsCommandList* CommandList::Reset()
{
	auto hr = m_pAllocators[m_Index]->Reset();
	if (FAILED(hr))
	{
		return nullptr;
	}

	hr = m_pCmdList->Reset(m_pAllocators[m_Index].Get(), nullptr);
	if (FAILED(hr))
	{
		return nullptr;
	}

	m_Index = (m_Index + 1) % uint32_t(m_pAllocators.size());
	return m_pCmdList.Get();
}
