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

	void DrawScene();

	void DrawMesh();


private:	// �����o�ϐ�
	Window m_window;
	DWORD m_dwExecLastTime;
	DWORD m_dwCurrentTime;
	DWORD m_dwFrameCount;
	DWORD m_dwLsatFPSTime;
	float m_fFPS;

	// �e�X�g�p
	RootSignature m_SceneRootSig;				// ���[�g�V�O�j�`��
	ColorTarget m_SceneColorTarget;					// �V�[���J���[�^�[�Q�b�g
	DepthTarget m_SceneDepthTarget;					// �V�[���f�v�X�^�[�Q�b�g
	std::vector<Mesh*> m_pMesh;					// ���b�V��
	ConstantBuffer m_LightCB[FRAME_BUFFER_COUNT];          //!< ���C�g�o�b�t�@�ł�.
	ConstantBuffer m_CameraCB[FRAME_BUFFER_COUNT];         //!< �J�����o�b�t�@�ł�.
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];      //!< �ϊ��p�o�b�t�@�ł�.
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];           //!< ���b�V���p�o�b�t�@�ł�.
	Material m_Material;						// �}�e���A��
	ComPtr<ID3D12PipelineState> m_pScenePSO;	// �p�C�v���C���X�e�[�g
	float m_RotateAngle;						// ��]�p

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
	Application()
		: m_dwExecLastTime(0)
		, m_dwCurrentTime(0)
		, m_dwFrameCount(0)
		, m_dwLsatFPSTime(0)
		, m_fFPS(0.0f)
		, m_RotateAngle(0.0f)
		, m_Material()
		, m_pMesh()
		, m_window()
		, m_SceneRootSig()
		, m_SceneColorTarget()
		, m_SceneDepthTarget()
		, m_LightCB()
		, m_CameraCB()
	{
	}
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};