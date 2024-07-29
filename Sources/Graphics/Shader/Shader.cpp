#include "stdafx.h"
#include "Shader.h"

void Shader::Load(const std::string& path, ShaderType type)
{
	// �V�F�[�_�̎�ނ�ݒ�
	m_Type = type;
	std::wstring wPath = StringToWString(path);
	std::wstring res;

	// ���_�V�F�[�_������
	if (!SearchFilePathW(wPath.c_str(), res))
	{
		// �G���[���O
		ELOG("[Shader.cpp]Error : Line15 : �p�X���Ԉ���Ă��܂� : path : %s", path.c_str());
		return;
	}

	// �V�F�[�_�̓ǂݍ���
	auto hr = D3DReadFileToBlob(res.c_str(), m_pShader.GetAddressOf());
	if (FAILED(hr))
	{
		ELOG("[Shader.cpp]Error : Line33 : ���_�V�F�[�_�̓ǂݍ��݂Ɏ��s���܂��� : path : %s", path.c_str());
		return;
	}

	// �V�F�[�_�̃o�b�t�@�T�C�Y���擾
	m_BufferSize = m_pShader->GetBufferSize();

	// �V�F�[�_�̃o�b�t�@���擾
	m_pBuffer = m_pShader->GetBufferPointer();
}
