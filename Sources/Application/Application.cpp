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
#include <Manager/ActorManager.h>

using namespace DirectX::SimpleMath;

struct LigBuffer
{
	Vector4 LigPos;	// ライトの位置
	Color LigColor;	// ライトの色
};

struct MaterialBufer
{
	Vector3 Diffuse;	// ディフューズ
	float Alpha;		// アルファ
};

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

	if (!RENDERER.Init(&m_window))
	{
		assert(0 && "Rendererの初期化に失敗しました。");
		return false;
	}

	// ====== Managerの初期化 ======
	ACTOR_MANAGER.Init();

	// メッシュをロード
	{
		std::wstring path;
		// ファイルパスを検索
		if (!SearchFilePathW(L"Assets/Models/teapot/teapot.obj", path));
		{
			ELOG("[App.cpp]Error : Line61 : ファイルが見つかりませんでした。");
			return false;
		}

		std::wstring dir = GetDirectoryPathW(path.c_str());

		// メッシュの読み込み
		vector<ResMesh> resMesh;
		vector<ResMaterial> resMaterial;

		if (!LoadMesh(path.c_str(), resMesh, resMaterial))
		{
			ELOG("[App.cpp]Error : Line72 : メッシュの読み込みに失敗しました。FilePath = %ls", path.c_str());
			return false;
		}

		// メモリを予約
		m_pMesh.reserve(resMesh.size());

		// メッシュを初期化
		for (size_t i = 0; i < resMesh.size(); ++i)
		{
			// メッシュの生成
			auto pMesh = NEW Mesh();

			// チェック
			if (!pMesh)
			{
				ELOG("[App.cpp]Error : Line88 : メモリの確保に失敗しました。");
				return false;
			}

			// 初期化処理
			if (!pMesh->Init(resMesh[i]))
			{
				ELOG("[App.cpp]Error : Line96 : メッシュの初期化に失敗しました。");
				SAFE_DELETE(pMesh);
				return false;
			}

			// メッシュの追加
			m_pMesh.push_back(pMesh);
		}

		// メモリ最適化
		resMesh.shrink_to_fit();

		DescriptorPool* pPool = RENDERER.GetDescriptorPool(POOL_TYPE_RES);

		// マテリアルの初期化
		if (!m_Material.Init(pPool, sizeof(MaterialBufer), resMaterial.size()));
		{
			ELOG("[App.cpp]Error : Line111 : マテリアルの初期化に失敗しました。");
			return false;
		}

		// テクスチャとマテリアルの設定
		for (size_t i = 0; i < resMaterial.size(); ++i)
		{
			auto ptr = m_Material.GetBufferPtr<MaterialBufer>(i);
			ptr->Diffuse = resMaterial[i].Diffuse;
			ptr->Alpha = resMaterial[i].Alpha;

			// テクスチャの設定
			std::wstring path = dir + resMaterial[i].DiffuseMap;
			m_Material.SetTexture(i, TU_DIFFUSE, path);
		}
	}

	// ライトバッファの設定
	{
		auto pCB = NEW ConstantBuffer();
		// チェック
		if (!pCB)
		{
			ELOG("[App.cpp]Error : Line136 : メモリの確保に失敗しました。");
			return false;
		}

		// 初期化
		DescriptorPool* pPool = RENDERER.GetDescriptorPool(POOL_TYPE_RES);
		if (!pCB->Init(pPool, sizeof(LigBuffer)))
		{
			ELOG("[App.cpp]Error : Line144 : ライトバッファの初期化に失敗しました。");
			SAFE_DELETE(pCB);
			return false;
		}

		auto ptr = pCB->GetPtr<LigBuffer>();
		ptr->LigPos = Vector4(0.0f, 50.0f, 100.0f, 1.0f);
		ptr->LigColor = Color(1.0f, 1.0f, 1.0f, 0.0f);

		m_pLight = pCB;
	}

	// ルートシグネチャーの生成
	{
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		// ルートパラメーターの設定
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		// ディスクリプタレンジを設定
		D3D12_DESCRIPTOR_RANGE range = {};
		range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range[0].NumDescriptors = 1;
		range[0].BaseShaderRegister = 0;
		range[0].RegisterSpace = 0;
		range[0].OffsetInDescriptorsFromTableStart = 0;

		// ルートパラメーターを設定
		D3D12_ROOT_PARAMETER param[4] = {};
		param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param[0].Descriptor.ShaderRegister = 0;
		param[0].Descriptor.RegisterSpace = 0;
		param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param[1].Descriptor.ShaderRegister = 1;
		param[1].Descriptor.RegisterSpace = 0;
		param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		param[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		param[2].Descriptor.ShaderRegister = 2;
		param[2].Descriptor.RegisterSpace = 0;
		param[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		param[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		param[3].DescriptorTable.NumDescriptorRanges = 1;
		param[3].DescriptorTable.pDescriptorRanges = &range;

	}


	// タイマーの初期化
	timeBeginPeriod(1);
	m_dwExecLastTime = timeGetTime();
	m_dwCurrentTime = m_dwExecLastTime;
	m_dwFrameCount = 0;
	m_dwLsatFPSTime = m_dwExecLastTime;
	m_fFPS = 0.0f;
	return true;
}

void Application::Run()
{
	while (true)
	{
		if (!m_window.ProcessMessage()) { break; }

		m_dwCurrentTime = timeGetTime();
		if ((m_dwCurrentTime - m_dwExecLastTime) >= FRAME_TIME)
		{
			m_dwExecLastTime = m_dwCurrentTime;
			// ====== ここにゲームの処理を書く ======
			// ------ 更新処理 ------
			ACTOR_MANAGER.Update();

			// ------ 描画処理 ------
			RENDERER.Begin();

			// --- 以下ゲームの描画処理
			ACTOR_MANAGER.Draw();

			// --- ここまでゲームの描画処理

			// ----- 描画終了 -----
			RENDERER.End();
			// ========================================
			m_dwFrameCount++;
		}
	}
}

void Application::Uninit()
{
	// ====== 終了処理 ======

	// ----- Managerの終了処理 -----
	ACTOR_MANAGER.Uninit();	// ActorManagerの終了処理

	// ----- システムの終了処理 -----
	RENDERER.Uninit();	// Rendererの終了処理
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
