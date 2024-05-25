#include "stdafx.h"
#include "Application.h"
#include <System/Window.h>
#include <System/Dx12Wrapper.h>
#include <System/PMDRenderer.h>
#include <Actor/PMDActor.h>

Application& Application::Instance()
{
	static Application instance;
	return instance;
}

bool Application::Init()
{
	auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		return false;
	}
	CreateMainWindow(m_hwnd, m_windowClass);

	// Dx12の初期化
	m_dx12 = NEW Dx12Wrapper(m_hwnd);
	m_pmdRenderer = NEW PMDRenderer(*m_dx12);
	m_pmdActor = NEW PMDActor("Assets/Model/初音ミク.pmd", *m_pmdRenderer);

	return true;
}

void Application::Run()
{
	ShowWindow(m_hwnd, SW_SHOW);	// ウィンドウを表示する
	float angle = 0.0f;
	MSG msg = {};
	unsigned int frame = 0;
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);	// キー入力メッセージを処理する
			DispatchMessage(&msg);	// メッセージを処理する
		}
		// もうApplicationが終わるってときにメッセージがWM_QUITになる
		if (msg.message == WM_QUIT) { break; }

		// 全体の描画準備
		m_dx12->BeginDraw();

		// PMD描画のパイプラインに合わせる
		m_dx12->GetCommandList()->SetPipelineState(m_pmdRenderer->GetPipelineState());
		// ルートシグネチャーも設定
		m_dx12->GetCommandList()->SetGraphicsRootSignature(m_pmdRenderer->GetRootSignature());

		m_dx12->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_dx12->SetScene();

		// モデルの描画
		m_pmdActor->Update();
		m_pmdActor->Draw();

		// 全体の描画終了
		m_dx12->EndDraw();

		// フリップ
		m_dx12->GetSwapChain()->Present(1, 0);
	}
}

void Application::Terminate()
{
	// もうクラス使わんから登録解除してや
	UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);

	// 解放処理
	SAFE_DELETE(m_pmdActor);		// アクターの解放
	SAFE_DELETE(m_pmdRenderer);	// レンダラーの解放

	CoUninitialize();	// COMの終了処理

	ID3D12DebugDevice* pDebugDevice = nullptr;
	m_dx12->GetDevice()->QueryInterface(&pDebugDevice);
	SAFE_DELETE(m_dx12);	// Dx12の解放
	pDebugDevice->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL);
	pDebugDevice->Release();
}

SIZE Application::GetWindowSize() const
{
	SIZE ret;
	ret.cx = WINDOW_WIDTH;
	ret.cy = WINDOW_HEIGHT;
	return ret;
}

Application::Application()
{
}

Application::~Application()
{
}
