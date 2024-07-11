#include "stdafx.h"
#include "ShaderManager.h"

namespace
{
	// string�^��wstring�^�ɕϊ�����֐�
	std::wstring StringToWString(const string& str)
	{
		std::wstring wstr(str.length(), L' ');
		std::copy(str.begin(), str.end(), wstr.begin());
		return wstr;

	}
}

bool ShaderManager::LoadShader(const string& path, const string& name, const ShaderType& type)
{
	// ���O���o�^����Ă���΃G���[
	if (m_shaderMap.find(name) != m_shaderMap.end())
	{
		ELOG("[ShaderManager.cpp]Error : Line21 : ���ɓo�^����Ă��閼�O�ł� : %s", name.c_str());
		return false;
	}

	// �p�X���o�^����Ă���΃G���[
	if (m_shaderMap.find(path) != m_shaderMap.end())
	{
		ELOG("[ShaderManager.cpp]Error : Line28 : ���ɓo�^����Ă���p�X�ł� : %s", path.c_str());
		return false;
	}

	// �p�X����Ȃ�G���[
	if (path.empty())
	{
		ELOG("[ShaderManager.cpp]Error : Line35 : �p�X����ł��B");
		return false;
	}

	// ���O����Ȃ�G���[
	if (name.empty())
	{
		ELOG("[ShaderManager.cpp]Error : Line42 : ���O����ł��B");
		return false;
	}

	// �t�@�C���p�X�̕ϊ�
	auto wpath = StringToWString(path);
	std::wstring res;

	// �V�F�[�_������
	if (!SearchFilePathW(wpath.c_str(), res))
	{
		ELOG("[ShaderManager.cpp]Error : Line25 : �w�肳�ꂽ�V�F�[�_��������܂���ł��� : %s", wpath.c_str());
		return false;
	}

	ComPtr<ID3DBlob> pShader;

	// ���_�V�F�[�_�̓ǂݍ���
	auto hr = D3DReadFileToBlob(res.c_str(), pShader.GetAddressOf());
	if (FAILED(hr))
	{
		ELOG("[ShaderManager.cpp]Error : Line35 : �V�F�[�_�̓ǂݍ��݂Ɏ��s���܂��� : %s", res.c_str());
		return false;
	}

	// �V�F�[�_�̏���o�^
	ShaderInfo info;
	info.type = type;
	info.blob = pShader;

	m_shaderMap[name] = info;

	return true;
}

ComPtr<ID3DBlob> ShaderManager::GetShader(const string& name)
{
	// �V�F�[�_�̎擾
	auto itr = m_shaderMap.find(name);
	if (itr == m_shaderMap.end())
	{
		return nullptr;
	}
	return itr->second.blob;
}

bool ShaderManager::Init()
{
	return true;
}

bool ShaderManager::Uninit()
{
	// �o�^����Ă���V�F�[�_��S�ĉ��
	m_shaderMap.clear();

	return true;
}
