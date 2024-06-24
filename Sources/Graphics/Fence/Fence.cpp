// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Fence.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/06/14 12:05 : �쐬
// �T�v   : �t�F���X�N���X�̎���
// �X�V����
// 2024/06/14  �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Fence.h"

// �R���X�g���N�^
Fence::Fence()
	: m_pFence(nullptr)
	, m_hEvent(nullptr)
	, m_Counter(0)
{
}

// �f�X�g���N�^
Fence::~Fence()
{
	Uninit();
}

// ����������
bool Fence::Init()
{
	auto pDevice = RENDERER.GetDevice();
	if (pDevice == nullptr)
	{
		return false;
	}

	// �C�x���g�𐶐�.
	m_hEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
	if (m_hEvent == nullptr)
	{
		return false;
	}

	// �t�F���X�𐶐�.
	auto hr = pDevice->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(m_pFence.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// �J�E���^�ݒ�.
	m_Counter = 1;

	// ����I��.
	return true;
}

// �I������
void Fence::Uninit()
{
	if (m_hEvent != nullptr)
	{
		CloseHandle(m_hEvent);
		m_hEvent = nullptr;
	}

	if (m_pFence != nullptr)
	{
		m_pFence.Reset();
	}

	m_Counter = 0;
}

// �V�O�i����ԂɂȂ�܂Ŏw�肳�ꂽ���ԑҋ@
void Fence::Wait(UINT timeout)
{
	// �f�o�C�X�̎擾
	auto pCmdQueue = RENDERER.GetCmdQueue();
	if (pCmdQueue == nullptr)
	{
		return;
	}

	const auto fenceValue = m_Counter;

	// �V�O�i������
	auto hr = pCmdQueue->Signal(m_pFence.Get(), fenceValue);
	if (FAILED(hr))
	{
		return;
	}

	// �J�E���^�[�𑝂₷
	m_Counter++;

	// ���̃t���[���̕`�揀�����܂��ł���Αҋ@
	if (m_pFence->GetCompletedValue() < fenceValue)
	{
		// �������ɃC�x���g��ݒ�
		auto hr = m_pFence->SetEventOnCompletion(fenceValue, m_hEvent);
		if (FAILED(hr))
		{
			return;
		}

		// �ҋ@����
		if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_hEvent, timeout, FALSE))
		{
			return;
		}
	}
}

// �V�O�i����ԂɂȂ�܂őҋ@
void Fence::Sync()
{
	auto pCmdQueue = RENDERER.GetCmdQueue();
	if (pCmdQueue == nullptr)
	{
		return;
	}

	// �V�O�i������
	auto hr = pCmdQueue->Signal(m_pFence.Get(), m_Counter);
	if (FAILED(hr))
	{
		return;
	}

	// �������ɃC�x���g��ݒ�
	hr = m_pFence->SetEventOnCompletion(m_Counter, m_hEvent);
	if (FAILED(hr))
	{
		return;
	}

	// �ҋ@����
	if (WAIT_OBJECT_0 != WaitForSingleObjectEx(m_hEvent, INFINITE, FALSE))
	{
		return;
	}

	// �J�E���^�[�𑝂₷
	m_Counter++;
}

