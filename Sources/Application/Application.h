// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Application.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 15:00
// �T�v   : �A�v���P�[�V�����N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <System/window.h>

// ====== �萔��` ======
const float FPS = 120.0f;				// �t���[�����[�g
const float FRAME_TIME = 1000.0f / FPS;	// �t���[������

// ====== �N���X��` ======
class Polygon;
class Application
{
public:		// �p�u���b�N�֐�
	/// <summary>
	/// �A�v���P�[�V�����̏�����
	/// </summary>
	/// <returns>�������ɐ���������true</returns>
	bool Init();

	/// <summary>
	/// �A�v���P�[�V�����̎��s
	/// </summary>
	void Run();

	/// <summary>
	/// �A�v���P�[�V�����̏I������
	/// </summary>
	void Uninit();

private:	// �v���C�x�[�g�֐�	
	/// <summary>
	///  .dll�̃f�B���N�g���̃Z�b�g�ƃ��[�h���s��
	/// </summary>
	void SetDirectoryAndLoadDll();


private:	// �����o�ϐ�
	Window m_window;
	DWORD m_dwExecLastTime;
	DWORD m_dwCurrentTime;
	DWORD m_dwFrameCount;
	DWORD m_dwLsatFPSTime;
	float m_fFPS;
	MyPolygon m_Polygon;

public:		// �V���O���g���֘A
	/// <summary>
	/// Application�N���X�̃C���X�^���X���擾
	/// </summary>
	/// <returns></returns>
	static Application& Instance()
	{
		static Application instance;
		return instance;
	}
private:	// �V���O���g���֘A
	Application() = default;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};