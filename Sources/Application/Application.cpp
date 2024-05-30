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
	SetDirectoryAndLoadDll();	// .dllのディレクトリのセットとロードを行う

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

	if (!Texture::Init())
	{
		assert(0 && "Textureの初期化に失敗しました。");
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
	Model model;
	model.Load("Assets/Model/Cube/Cube.gltf");

	RenderingSetting setting = {};
	setting.inputLayouts = { InputLayout::POSITION,InputLayout::TEXCOORD,InputLayout::COLOR,InputLayout::NORMAL,InputLayout::TANGENT };
	setting.Formats = { DXGI_FORMAT_R8G8B8A8_UNORM };
	setting.cullMode = CullMode::Back;
	setting.primitiveTopology = PrimitiveTopology::Triangle;

	Shader shader;
	string path = "Simple";
	shader.Create(&Renderer::Instance(), path, setting, { RangeType::CBV,RangeType::CBV,RangeType::SRV,RangeType::SRV,RangeType::SRV,RangeType::SRV });

	DirectX::XMMATRIX mWorld = DirectX::XMMatrixIdentity();
	DirectX::XMMATRIX mWorld2 = DirectX::XMMatrixIdentity();
	// カメラの設定
	DirectX::XMMATRIX mView = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	DirectX::XMMATRIX mProj = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), ASPECT_RATIO, 0.01, 1000.0f);

	// 2D用カメラの設定
	DirectX::XMMATRIX mView2 = DirectX::XMMatrixLookAtLH(DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f), DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	DirectX::XMMATRIX mProj2 = DirectX::XMMatrixOrthographicLH(WINDOW_WIDTH, WINDOW_HEIGHT, 0.01, 1000.0f);

	CBufferData::Camera cbCamera;
	CBufferData::Camera cbCamera2;
	cbCamera.mView = mView;
	cbCamera.mProj = mProj;
	cbCamera2.mView = mView2;
	cbCamera2.mProj = mProj2;

	// テクスチャの設定
	setting.inputLayouts = { InputLayout::POSITION };
	setting.isDepth = false;
	setting.isDepthMask = false;

	Shader texShader;
	path = "Texture";
	texShader.Create(&Renderer::Instance(), path, setting, { RangeType::CBV,RangeType::CBV,RangeType::SRV });

	Texture tex;
	tex.Load(&Renderer::Instance(), "Assets/Texture/field.jpg");

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
			Renderer::Instance().Begin3DDraw();		// 描画開始
			// ここに描画処理を書く
			Renderer::Instance().GetCBVSRVUAVHeap()->SetHeap();
			Renderer::Instance().GetCBufferAllocater()->ResetNumber();

			// シェーダーの設定
			shader.Begin(m_window.GetWidth(), m_window.GetHeight());
			// シェーダーにカメラのデータを渡す
			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(0, cbCamera);
			// ワールド行列の更新
			mWorld *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(1.0f));
			mWorld *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(1.0f));
			mWorld *= DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(1.0f));
			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(1, mWorld);
			// モデルの描画
			shader.DrawModel(model);

			// テクスチャの描画
			Renderer::Instance().Begin2DDraw();		// 描画開始

			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(0, cbCamera2);
			Renderer::Instance().GetCBufferAllocater()->BindAndAttachData(1, mWorld2);
			texShader.Begin(m_window.GetWidth(), m_window.GetHeight());
			texShader.Draw2D(tex);

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

void Application::SetDirectoryAndLoadDll()
{
#ifdef _DEBUG
	SetDllDirectory("Library/assimp/build/lib/Debug");
	LoadLibraryExA("assimp-vc143-mtd.dll", NULL, NULL);
#else
	SetDllDirectory("Library/assimp/build/lib/Release");
	LoadLibraryExA("assimp-vc143-mt.dll", NULL, NULL);
#endif // _DEBUG

}
