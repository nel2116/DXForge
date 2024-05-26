// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : �����_���[�N���X�̎���
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== �C���N���[�h�� ======
#include "stdafx.h"
#include "Renderer.h"


bool Renderer::Init()
{
	if (!CreateFactory())
	{
		assert(0 && "�t�@�N�g���[�̍쐬�Ɏ��s���܂����B");
		return false;
	}
	if (!CreateDevice())
	{
		assert(0 && "D3D12�f�o�C�X�̍쐬�Ɏ��s���܂����B");
		return false;
	}

	return true;
}

bool Renderer::CreateFactory()
{
	UINT flagsDXGI = 0;

#ifdef _DEBUG
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	auto hr = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(m_pFactory.GetAddressOf()));

	if (FAILED(hr))
	{
		// �G���[���b�Z�[�W�̕\��
		MessageBox(nullptr, "DXGI�t�@�N�g���[�̍쐬�Ɏ��s���܂����B", "�G���[", MB_OK);
		return false;
	}
	return true;
}

bool Renderer::CreateDevice()
{
	// ------ �ϐ��錾 ------
	ComPtr<IDXGIAdapter1> pSelectAdapter = nullptr;
	vector<ComPtr<IDXGIAdapter1>> pAdapters;
	vector<ComPtr<DXGI_ADAPTER_DESC1>> descs;

	// ------ �A�_�v�^�[�̗� ------
	for (UINT idx = 0; 1; ++idx)
	{
		pAdapters.push_back(nullptr);
		HRESULT hr = m_pFactory->EnumAdapters(idx, &pAdapters[idx]);
	}


}
