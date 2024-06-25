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

// ------ ImGui ------
#include <imugui/imgui.h>
#include <imugui/imgui_impl_win32.h>
#include <imugui/imgui_impl_dx12.h>

// ------ �v���W�F�N�g�ŗL ------
// �E�B���h�E�֌W
#include <System/window.h>
// �`��֌W�̃f�o�C�X
#include <Graphics/Renderer.h>
// DirectXMath�̃��b�p�[
#include <System/SimpleMath.h>
// �t�@�C���p�X����������֐�
#include <System/FileUtil.h>
// �f�o�b�O�֘A
#include <System/Logger.h>

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

// ====== ���O��� ======
using namespace std;

// ====== �萔��` ======
// �~����
const float PI = 3.1415926f;
