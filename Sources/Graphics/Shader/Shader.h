// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Shader.h]
// 制作者 : 田中ミノル
// 作成日 : 2024/07/02
// 概要   : シェーダの定義
// 更新日
// 2024/07/02 : 田中ミノル : 新規作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <System/ComPtr.h>

// ====== 列挙体 ======
enum class ShaderType
{
	Vertex,
	Pixel,
	Geometry,
	Hull,
	Domain,
	Compute
};

// ====== クラスの定義 ======
class Shader
{
public:
	// コンストラクタ
	Shader() = default;
	// デストラクタ
	~Shader() = default;

	// シェーダの読み込み
	void Load(const std::string& path, ShaderType type);

	// シェーダのバッファサイズの取得
	size_t GetBufferSize() { return m_BufferSize; }

	// シェーダのバッファポインタの取得
	void* GetBuffer() { return m_pBuffer; }

private:
	// メンバ変数
	ShaderType m_Type;			// シェーダの種類
	ComPtr<ID3DBlob> m_pShader;	// シェーダ
	size_t m_BufferSize;		// シェーダのバッファサイズ
	void* m_pBuffer;			// シェーダのバッファ
};