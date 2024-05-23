// [Application.h]
// 作成日 : 2024/05/21 17:00
// 作成者 : 田中ミノル
// 概要   : Applicationクラスの定義
#pragma once
#include "System/window.h"		// ウィンドウ関連の処理を行うため
#include <tchar.h>				// _T()マクロを使用するため
#include <d3d12.h>				// Direct3D 12 API
#include <dxgi1_6.h>			// DXGI 1.6 API
#include <DirectXMath.h>		// DirectXMath API
#include <map>					// std::mapを使用するため
#include <d3dcompiler.h>		// D3DCompile API
#include <DirectXTex.h>			// DirectXTex API
#include <DirectX/d3dx12.h>		// D3D12 APIの補助関数
#include <wrl.h>				// Microsoft::WRLを使用するため
#include <memory>				// std::unique_ptrを使用するため

class Dx12Wrapper;
class PMDRenderer;
class PMDActor;

// シングルトンクラス
class Application
{
public:
	// インスタンスの取得
	static Application& Instance();

	// 初期化
	bool Init();

	// メインループ
	void Run();

	// 終了処理
	void Terminate();

	SIZE GetWindowSize() const;
	~Application();

private:
	// ウィンドウ周り
	WNDCLASSEX m_windowClass;	// ウィンドウクラス
	HWND m_hwnd;				// ウィンドウハンドル
	shared_ptr<Dx12Wrapper> m_dx12;	// DirectX 12ラッパー
	shared_ptr<PMDRenderer> m_pmdRenderer;	// PMDレンダラー
	shared_ptr<PMDActor> m_pmdActor;	// PMDアクター

	// ゲーム用ウィンドウの生成
	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);

	// シングルトン関連
	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;
};