#include "stdafx.h"
#include "Shader.h"

void Shader::Load(const std::string& path, ShaderType type)
{
	// シェーダの種類を設定
	m_Type = type;
	std::wstring wPath = StringToWString(path);
	std::wstring res;

	// 頂点シェーダを検索
	if (!SearchFilePathW(wPath.c_str(), res))
	{
		// エラーログ
		ELOG("[Shader.cpp]Error : Line15 : パスが間違っています : path : %s", path.c_str());
		return;
	}

	// シェーダの読み込み
	auto hr = D3DReadFileToBlob(res.c_str(), m_pShader.GetAddressOf());
	if (FAILED(hr))
	{
		ELOG("[Shader.cpp]Error : Line33 : 頂点シェーダの読み込みに失敗しました : path : %s", path.c_str());
		return;
	}

	// シェーダのバッファサイズを取得
	m_BufferSize = m_pShader->GetBufferSize();

	// シェーダのバッファを取得
	m_pBuffer = m_pShader->GetBufferPointer();
}
