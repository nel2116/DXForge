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

#define _USE_MATH_DEFINES
#include <math.h>

// ------ DirectX ------
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

// DirectXTex
#pragma comment(lib, "DirectXTex.lib")
#include <DirectXTex\DirectXTex.h>

// ------ �v���W�F�N�g�ŗL ------
#include <System/window.h>
// �`��֌W�̃f�o�C�X
#include <Graphics/Renderer.h>
// �q�[�v
#include <Graphics/Heap/Heap.h>
#include <Graphics\Heap\RTVHeap\RTVHeap.h>
#include <Graphics\Heap\CBVSRVUAVHeap\CBVSRVUAVHeap.h>
// �萔�o�b�t�@�̃A���P�[�^�[
#include <Graphics/CBufferAllocater/CBufferAllocater.h>
// �萔�o�b�t�@�f�[�^
#include <Graphics\CBufferAllocater\CBufferData\CBufferData.h>
// �e�N�X�`��
#include <Graphics/Texture/Texture.h>
// ���b�V��
#include <Graphics/Mesh/Mesh.h>
// ���f��
#include <Graphics/Model/Model.h>
// �V�F�[�_�[
#include <Graphics/Shader/Shader.h>

// ====== ���O��� ======
using namespace std;

// ====== �萔��` ======
// �~����
const float PI = 3.1415926f;

// ====== �}�N����` ======
// ���������[�N���o
#if _DEBUG
#define NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW new
#endif
// �������̊J��
#define SAFE_DELETE(p) { if(p) { delete(p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[](p); (p) = nullptr; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p) = nullptr; } }

// ====== �v���g�^�C�v�錾 ======
/// <summary>
/// �R���\�[����ʂɃt�H�[�}�b�g�t���ŕ�������o�͂���
/// </summary>
/// <param name="format">�t�H�[�}�b�g</param>
/// <param name="...">�ϒ�����</param>
/// <remarks>���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�܂���</remarks>
void DebugOutputFormatString(const char* format, ...);

/// <summary>
/// �t�@�C���p�X����e�f�B���N�g���܂ł̃p�X���擾����
/// </summary>
/// <param name="path">�t�@�C���p�X</param>
/// <returns>�f�B���N�g���p�X</returns>
inline string GetDirFromPath(const string& path)
{
	const string::size_type pos = path.find_last_of("/");
	return (pos == string::npos) ? string() : path.substr(0, pos + 1);
}