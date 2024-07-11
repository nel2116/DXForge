// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [stdfx.h]
// �쐬�� : �c���~�m��
// �쐬�� : �s��
// �T�v   : ���ʃw�b�_�[
// �X�V����
// 2024/05/25 ���t�@�N�^�����O
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
// ------ ��{�w�b�_�[ ------
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

// ------ �v���W�F�N�g�ŗL ------
// �E�B���h�E�֌W
#include <System/window.h>
// �`��֌W�̃f�o�C�X
#include <Graphics/Renderer.h>
// �t�@�C���p�X����������֐�
#include <System/FileUtil.h>
// �f�o�b�O�֘A
#include <System/Logger.h>
// ���w�֘A
#include <System/MyMath.h>
// ���b�V��
#include <Graphics/Mesh/Mesh.h>
// �}�e���A��
#include <Graphics/Mesh/Material/Material.h>
// �萔�o�b�t�@
#include <Graphics/Buffer/ConstantBuffer/ConstantBuffer.h>
// ���[�g�V�O�l�`���[
#include <Graphics/RootSignature/RootSignature.h>
// �}�N����`
#include <System/Macro.h>

// �}�l�[�W���[�֘A
#include <Manager/SceneManager.h>
#include <Manager/ActorManager.h>
#include <Manager/TimerManager.h>
#include <Manager/ShaderManager.h>

// ====== ���O��� ======
using namespace std;

// ====== �萔��` ======
// �~����
const float PI = 3.1415926f;
