// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [TimerManager.h]
// ����� : �c���~�m��
// ����� : 2024/06/26 21:30
// �T�v   : TimerManager�N���X�̒�`	:	�^�C�}�[�}�l�[�W���N���X
// �X�V��
// 2024/06/26 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Manager.h"

// ====== �萔��` ======
const float FPS = 60.0f;				// �t���[�����[�g
const float FRAME_TIME = 1000.0f / FPS;	// �t���[������

// ====== �N���X�̒�` ======
class TimerManager : public Manager<TimerManager>
{
public:	// �����o�֐�
	// �t���[�����s�O�̍X�V
	void Update();
	// �t���[�����s��̍X�V
	void UpdateFrame();
	// ������
	void Init();
	// �I��
	void Uninit();

	// �t���[�����[�g�̍X�V
	float UpdateFPS(double Def);

	/// <summary>
	/// �f���^�^�C���̎擾
	/// </summary>
	/// <returns>�f���^�^�C��</returns>
	float GetDeltaTime() { return m_fDeltaTime; }

	/// <summary>
	/// �t���[���J�E���g�̎擾
	/// </summary>
	/// <returns>�t���[���J�E���g</returns>
	DWORD GetFrameCount() { return m_dwFrameCount; }

	/// <summary>
	/// ���s�J�n���Ԃ̎擾
	/// </summary>
	/// <returns>���s�J�n����</returns>
	DWORD GetStartTime() { return m_dwStartTime; }

	/// <summary>
	/// ���ݎ��Ԃ̎擾
	/// </summary>
	/// <returns>���ݎ���</returns>
	DWORD GetCurrentTime() { return m_dwCurrentTime; }

	/// <summary>
	/// �O����s���Ԃ̎擾
	/// </summary>
	/// <returns>�O����s����</returns>
	DWORD GetPreExecTime() { return m_dwPreExecTime; }

	/// <summary>
	/// �������Ԃ̎擾
	/// </summary>
	/// <returns>��������</returns>
	DWORD GetDiffTime() { return m_dwDiffTime; }

	/// <summary>
	/// �t���[�����i�ނ��ǂ����̎擾
	/// </summary>
	/// <returns>�t���[�����i�ނ��ǂ���</returns>
	bool IsFrameGo() { return m_dwDiffTime >= FRAME_TIME; }

	/// <summary>
	/// �t���[�����[�g�̎擾
	/// </summary>
	/// <returns>�t���[�����[�g</returns>
	double GetFPS();

	/// <summary>
	/// �T���v������ύX
	/// </summary>
	void SetSampleNum(UINT num);

private:
	DWORD m_dwStartTime;			// �J�n����
	DWORD m_dwCurrentTime;			// ���ݎ���
	DWORD m_dwPreExecTime;			// �O����s����
	DWORD m_dwDiffTime;				// �O��t���[������̌o�ߎ���(�~���b)
	float m_fDeltaTime;				// �O��t���[������̌o�ߎ���(�b)
	DWORD m_dwFrameCount;			// �Q�[���J�n���猻�݂܂ł̑��t���[����

	// �t���[�����[�g�֘A
	float m_fFrameRate;				// �t���[�����[�g
	list<double> m_dwDefTimeList;	// �t���[�����Ԃ̃��X�g
	UINT m_uiNum;					// �ړ����όv�Z���̃f�[�^
	double m_dwSumTimes;			// ���ʕ����̍��v

private: // �V���O���g���֘A
	friend class Manager<TimerManager>;
	TimerManager()
		: m_dwStartTime(0)
		, m_dwCurrentTime(0)
		, m_dwPreExecTime(0)
		, m_dwDiffTime(0)
		, m_fDeltaTime(0.0f)
		, m_dwFrameCount(0)
		, m_fFrameRate(0.0f)
		, m_uiNum(0)
		, m_dwSumTimes(0.0)
		, m_dwDefTimeList()
	{}

	static TimerManager* createInstance()
	{
		return NEW TimerManager();
	}
};
// �C���X�^���X���擾
#define TIMER_MANAGER TimerManager::manager()
