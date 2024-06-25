// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [stdfx.h]
// 作成者 : 田中ミノル
// 作成日 : 不明
// 概要   : 共通ヘッダー
// 更新履歴
// 2024/05/25 リファクタリング
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
// ------ 基本ヘッダー ------
#pragma comment(lib, "winmm.lib")

#define NOMINMAX
#include <iostream>
#include <crtdbg.h>
#include <cassert>
#include <wrl/client.h>

// ------ SHL ------
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <list>
#include <iterator>
#include <queue>
#include <algorithm>
#include <memory>
#include <random>
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <filesystem>
#include <chrono>
#include <typeinfo>
#include <new>

#define _USE_MATH_DEFINES
#include <math.h>

// ------ DirectX ------
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

// ------ DirectXTex ------
#include <DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

// ------ ImGui ------
#include <imugui/imgui.h>
#include <imugui/imgui_impl_win32.h>
#include <imugui/imgui_impl_dx12.h>

// ------ プロジェクト固有 ------
// ウィンドウ関係
#include <System/window.h>
// 描画関係のデバイス
#include <Graphics/Renderer.h>
// DirectXMathのラッパー
#include <System/SimpleMath.h>
// ファイルパスを検索する関数
#include <System/FileUtil.h>
// デバッグ関連
#include <System/Logger.h>

// メッシュ
#include <Graphics/Mesh/Mesh.h>
// マテリアル
#include <Graphics/Mesh/Material/Material.h>
// 定数バッファ
#include <Graphics/Buffer/ConstantBuffer/ConstantBuffer.h>
// ルートシグネチャー
#include <Graphics/RootSignature/RootSignature.h>
// マクロ定義
#include <System/Macro.h>

// ====== 名前空間 ======
using namespace std;

// ====== 定数定義 ======
// 円周率
const float PI = 3.1415926f;
