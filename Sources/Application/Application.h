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

	// �e�X�g�p
	RootSignature m_SceneRootSig;						// ���[�g�V�O�j�`��
	ComPtr<ID3D12PipelineState> m_pScenePSO;			// �p�C�v���C���X�e�[�g
	std::vector<Mesh*> m_pMesh;							// ���b�V��
	ConstantBuffer m_LightCB[FRAME_BUFFER_COUNT];		// ���C�g�o�b�t�@
	ConstantBuffer m_CameraCB[FRAME_BUFFER_COUNT];		// �J�����o�b�t�@
	ConstantBuffer m_TransformCB[FRAME_BUFFER_COUNT];	// �ϊ��p�o�b�t�@
	ConstantBuffer m_MeshCB[FRAME_BUFFER_COUNT];		// ���b�V���p�o�b�t�@
	Material m_Material;								// �}�e���A��
	float m_RotateAngle;								// ��]�p

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
		: m_RotateAngle(0.0f)
		, m_Material()
		, m_pMesh()
		, m_window()
		, m_SceneRootSig()
		, m_LightCB()
		, m_CameraCB()
		, m_TransformCB()
		, m_MeshCB()
		, m_pScenePSO()
	{
	}
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};