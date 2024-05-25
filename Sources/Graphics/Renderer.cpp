// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : レンダラークラスの実装
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Renderer.h"


bool Renderer::Init()
{
	if (!CreateFactory())
	{
		assert(0 && "ファクトリーの作成に失敗しました。");
		return false;
	}
	if (!CreateDevice())
	{
		assert(0 && "D3D12デバイスの作成に失敗しました。");
		return false;
	}

	return true;
}

bool Renderer::CreateFactory()
{
	UINT flagsDXGI = 0;

#ifdef _DEBUG
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	auto hr = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_pFactory.GetAddressOf()));

	if (FAILED(hr))
	{
		// エラーメッセージの表示
		MessageBox(nullptr, "DXGIファクトリーの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}
	return true;
}

bool Renderer::CreateDevice()
{
	// ------ 変数宣言 ------
	ComPtr<IDXGIAdapter1> pSelectAdapter = nullptr;
	vector<ComPtr<IDXGIAdapter1>> pAdapters;
	vector<ComPtr<DXGI_ADAPTER_DESC1>> descs;

	// ------ アダプターの列挙 ------
	for (UINT idx = 0; 1; ++idx)
	{
		pAdapters.push_back(nullptr);
		HRESULT hr = m_pFactory->EnumAdapters(idx, &pAdapters[idx]);
	}


}
