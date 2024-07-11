#include "stdafx.h"
#include "ShaderManager.h"

namespace
{
	// string型をwstring型に変換する関数
	std::wstring StringToWString(const string& str)
	{
		std::wstring wstr(str.length(), L' ');
		std::copy(str.begin(), str.end(), wstr.begin());
		return wstr;

	}
}

bool ShaderManager::LoadShader(const string& path, const string& name, const ShaderType& type)
{
	// 名前が登録されていればエラー
	if (m_shaderMap.find(name) != m_shaderMap.end())
	{
		ELOG("[ShaderManager.cpp]Error : Line21 : 既に登録されている名前です : %s", name.c_str());
		return false;
	}

	// パスが登録されていればエラー
	if (m_shaderMap.find(path) != m_shaderMap.end())
	{
		ELOG("[ShaderManager.cpp]Error : Line28 : 既に登録されているパスです : %s", path.c_str());
		return false;
	}

	// パスが空ならエラー
	if (path.empty())
	{
		ELOG("[ShaderManager.cpp]Error : Line35 : パスが空です。");
		return false;
	}

	// 名前が空ならエラー
	if (name.empty())
	{
		ELOG("[ShaderManager.cpp]Error : Line42 : 名前が空です。");
		return false;
	}

	// ファイルパスの変換
	auto wpath = StringToWString(path);
	std::wstring res;

	// シェーダを検索
	if (!SearchFilePathW(wpath.c_str(), res))
	{
		ELOG("[ShaderManager.cpp]Error : Line25 : 指定されたシェーダが見つかりませんでした : %s", wpath.c_str());
		return false;
	}

	ComPtr<ID3DBlob> pShader;

	// 頂点シェーダの読み込み
	auto hr = D3DReadFileToBlob(res.c_str(), pShader.GetAddressOf());
	if (FAILED(hr))
	{
		ELOG("[ShaderManager.cpp]Error : Line35 : シェーダの読み込みに失敗しました : %s", res.c_str());
		return false;
	}

	// シェーダの情報を登録
	ShaderInfo info;
	info.type = type;
	info.blob = pShader;

	m_shaderMap[name] = info;

	return true;
}

ComPtr<ID3DBlob> ShaderManager::GetShader(const string& name)
{
	// シェーダの取得
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
	// 登録されているシェーダを全て解放
	m_shaderMap.clear();

	return true;
}
