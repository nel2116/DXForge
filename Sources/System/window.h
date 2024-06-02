// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [window.h]
// 作成者 : 田中ミノル
// 作成日 : 不明
// 概要   : ウィンドウ関連の処理
// 更新履歴 : 2024/05/25 リファクタリング
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <string>

// ====== 定数定義 ======
// ウィンドウのタイトル
#define WINDOW_NAME "DXForge"
// クラスの名前
#define CLASS_NAME "DXForgeWindow"
// 
#define WINDOW_STYLE (WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME|WS_MAXIMIZEBOX))
// ウィンドウの幅
const int WINDOW_WIDTH = 1280;
// ウィンドウの高さ
const int WINDOW_HEIGHT = 720;
// ウィンドウのアスペクト比
const float ASPECT_RATIO = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;

// ====== クラス定義 ======
class Window
{
public:		// メンバ関数
	/// <summary>
	/// ウィンドウの作成
	/// </summary>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	/// <param name="titleName">タイトル名(キャプション名)</param>
	/// <param name="className">クラス名</param>
	/// <returns>作成に成功したらtrue</returns>
	bool Create(int width, int height, const std::string& titleName, const std::string& className);

	/// <summary>
	/// ウィンドウメッセージ処理
	/// </summary>
	/// <returns>終了メッセージが来たらfalse</returns>
	bool ProcessMessage();

	HWND GetHwnd() const { return m_hwnd; }

	HINSTANCE GetHinstance() const { return m_windowClass.hInstance; }

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

private:	// プライベート関数
	static VOID SetClientSize(LONG sx, LONG sy);

private:	// メンバ変数
	static HWND m_hwnd;
	WNDCLASSEX m_windowClass;
	static int m_width;
	static int m_height;
};
