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

// ------ PhysX ------
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#include "PxPhysicsAPI.h"
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXCooking_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXPvdSDK_static_64.lib")
#pragma comment(lib, "PhysXTask_static_64.lib")
#pragma comment(lib, "SceneQuery_static_64.lib")
#pragma comment(lib, "SimulationController_static_64.lib")

// ------ ImGui ------
#include <imugui/imgui.h>
#include <imugui/imgui_impl_win32.h>
#include <imugui/imgui_impl_dx12.h>

// ------ プロジェクト固有 ------
// ウィンドウ関係
#include <System/window.h>
// 描画関係のデバイス
#include <Graphics/Renderer.h>
// ファイルパスを検索する関数
#include <System/FileUtil.h>
// デバッグ関連
#include <System/Logger.h>
// 数学関連
#include <System/MyMath.h>
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

// マネージャー関連
#include <Manager/SceneManager.h>
#include <Manager/ActorManager.h>
#include <Manager/TimerManager.h>
#include <Manager/ShaderManager.h>

// ====== 名前空間 ======
using namespace std;

// ====== 定数定義 ======
// 円周率
const float PI = 3.1415926f;
