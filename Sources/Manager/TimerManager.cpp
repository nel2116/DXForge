#include "stdafx.h"
#include "TimerManager.h"

void TimerManager::Update()
{
	// �T���v�����̐ݒ�
	SetSampleNum(10);
	m_dwCurrentTime = timeGetTime();
	m_dwDiffTime = m_dwCurrentTime - m_dwPreExecTime;
	m_fDeltaTime = m_dwDiffTime / 1000.0f;

	// FPS�̌v�Z
	double R[50];
	// ��񂵂ɂ��v���덷�̑���
	for (int i = 0; i < 50; i++)
	{
		R[i] = 1.0 / GetFPS();
	}

}

void TimerManager::UpdateFrame()
{
	m_dwPreExecTime = m_dwCurrentTime;
	m_dwFrameCount++;
}

void TimerManager::Init()
{
	timeBeginPeriod(1);
	m_dwStartTime = m_dwCurrentTime = m_dwPreExecTime = timeGetTime();
}

void TimerManager::Uninit()
{
	timeEndPeriod(1);
}

float TimerManager::UpdateFPS(double Def)
{
	// �ł��Â��f�[�^������
	m_dwDefTimeList.pop_front();

	// �f�[�^��ǉ�
	m_dwDefTimeList.push_back(Def);

	// FPS�̌v�Z
	double FPS = 0.0;
	double AveDef = (m_dwSumTimes + Def) / m_uiNum;
	if (AveDef != 0) { FPS = 1000.0f / AveDef; }

	// ���v���Ԃ̍X�V
	m_dwSumTimes += Def - *m_dwDefTimeList.begin();

	// FPS�̍X�V
	return FPS;
}

double TimerManager::GetFPS()
{
	if (m_dwDiffTime == 0.0f) return 0.0;
	return UpdateFPS(m_dwDiffTime);
}

void TimerManager::SetSampleNum(UINT num)
{
	m_uiNum = num;						//	���ς��v�Z�����		
	m_dwDefTimeList.resize(num, 0.0);	//	���X�g������
	m_dwSumTimes = 0.0;					//	���v���ԏ�����
}


