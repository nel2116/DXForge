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


private:	// �����o�ϐ�
	Window m_window;
	DWORD m_dwExecLastTime;
	DWORD m_dwCurrentTime;
	DWORD m_dwFrameCount;
	DWORD m_dwLsatFPSTime;
	float m_fFPS;

	// �e�X�g�p
	std::vector<Mesh*> m_pMesh;					// ���b�V��
	std::vector<ConstantBuffer*> m_Transform;	// �ϊ��s��
	ConstantBuffer* m_pLight;					// ���C�g
	Material m_Material;						// �}�e���A��
	ComPtr<ID3D12PipelineState> m_pPSO;			// �p�C�v���C���X�e�[�g
	ComPtr<ID3D12RootSignature> m_pRootSig;		// ���[�g�V�O�j�`��
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
		, m_pLight(nullptr)
		, m_pPSO(nullptr)
		, m_pRootSig(nullptr)
		, m_Material()
		, m_pMesh()
		, m_Transform()
		, m_window()
	{
	}
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
};