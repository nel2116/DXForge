//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Application.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : アプリケーションクラスの実装
// 更新履歴 : 2024/05/25 作成
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Application.h"

// ====== メンバ関数 ======
bool Application::Init()
{
	// COMの初期化
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED)))
	{
		MessageBox(nullptr, "COMの初期化に失敗しました。", "エラー", MB_OK);
		assert(0 && "COMの初期化に失敗しました。");
		return false;
	}

	if (!m_window.Create(WINDOW_WIDTH, WINDOW_HEIGHT, CLASS_NAME, WINDOW_NAME))
	{
		assert(0 && "ウィンドウの作成に失敗しました。");
		return false;
	}

	if (!Renderer::Instance().Init(&m_window))
	{
		assert(0 && "Rendererの初期化に失敗しました。");
		return false;
	}

	// タイマーの初期化
	timeBeginPeriod(1);
	m_dwExecLastTime = timeGetTime();
	m_dwCurrentTime = m_dwExecLastTime;
	return true;
}

void Application::Run()
{
	Mesh mesh;
	mesh.Create(&Renderer::Instance());

	RenderingSetting setting = {};
	setting.inputLayouts = { InputLayout::POSITION,InputLayout::TEXCOORD };
	setting.Formats = { DXGI_FORMAT_R8G8B8A8_UNORM };
	setting.isDepth = false;
	setting.isDepthMask = false;
	setting.cullMode = CullMode::None;
	setting.primitiveTopology = PrimitiveTopology::Triangle;

	Shader shader;
	string path = "Simple";
	shader.Create(&Renderer::Instance(), path, setting, { RangeType::CBV,RangeType::SRV });

	Texture sampleTex;
	sampleTex.Load(&Renderer::Instance(), "Assets/Texture/field.jpg");

	DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	DirectX::XMMATRIX mProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), WINDOW_WIDTH / WINDOW_HEIGHT, 0.01, 1000.0f);

	CBufferData::Camera cbCamera;
	cbCamera.mView = mView;
	cbCamera.mProj = mProj;

	while (true)
	{
		if (!m_window.ProcessMessage()) { break; }
		m_dwCurrentTime = timeGetTime();
		if ((m_dwCurrentTime - m_dwExecLastTime) >= FRAME_TIME)
		{
			m_dwExecLastTime = m_dwCurrentTime;
			// ====== ここにゲームの処理を書く ======
			// ------ 更新処理 ------

			// ------ 描画処理 ------
			Renderer::Instance().BeginDraw();		// 描画開始
			// ここに描画処理を書く
			Renderer::Instance().GetCBVSRVUAVHeap()->SetHeap();
			Renderer::Instance().GetCBufferAllocater()->ResetNumber();

			shader.Begin(m_window.GetWidth(), m_window.GetHeight());
			sampleTex.Set(shader.GetCBVCount() + sampleTex.GetSRVNumber());

			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(0, cbCamera);

			shader.DrawMesh(mesh);


			Renderer::Instance().EndDraw();			// 描画終了
		}
	}
}

void Application::Uninit()
{
	Renderer::Instance().Uninit();	// Rendererの終了処理
	timeEndPeriod(1);	// タイマーの終了処理
	UnregisterClass(CLASS_NAME, m_window.GetHinstance());	// ウィンドウクラスの登録解除
	CoUninitialize();	// COMの終了処理
}
