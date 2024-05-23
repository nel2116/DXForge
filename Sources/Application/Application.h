// [Application.h]
// �쐬�� : 2024/05/21 17:00
// �쐬�� : �c���~�m��
// �T�v   : Application�N���X�̒�`
#pragma once
#include "System/window.h"		// �E�B���h�E�֘A�̏������s������
#include <tchar.h>				// _T()�}�N�����g�p���邽��
#include <d3d12.h>				// Direct3D 12 API
#include <dxgi1_6.h>			// DXGI 1.6 API
#include <DirectXMath.h>		// DirectXMath API
#include <map>					// std::map���g�p���邽��
#include <d3dcompiler.h>		// D3DCompile API
#include <DirectXTex.h>			// DirectXTex API
#include <DirectX/d3dx12.h>		// D3D12 API�̕⏕�֐�
#include <wrl.h>				// Microsoft::WRL���g�p���邽��
#include <memory>				// std::unique_ptr���g�p���邽��

class Dx12Wrapper;
class PMDRenderer;
class PMDActor;

// �V���O���g���N���X
class Application
{
public:
	// �C���X�^���X�̎擾
	static Application& Instance();

	// ������
	bool Init();

	// ���C�����[�v
	void Run();

	// �I������
	void Terminate();

	SIZE GetWindowSize() const;
	~Application();

private:
	// �E�B���h�E����
	WNDCLASSEX m_windowClass;	// �E�B���h�E�N���X
	HWND m_hwnd;				// �E�B���h�E�n���h��
	shared_ptr<Dx12Wrapper> m_dx12;	// DirectX 12���b�p�[
	shared_ptr<PMDRenderer> m_pmdRenderer;	// PMD�����_���[
	shared_ptr<PMDActor> m_pmdActor;	// PMD�A�N�^�[

	// �Q�[���p�E�B���h�E�̐���
	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);

	// �V���O���g���֘A
	Application();
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;
};