// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [ShaderManager.h]
// 制作者 : 田中ミノル
// 作成日 : 2024/07/02
// 概要   : シェーダの管理クラス
// 更新日
// 2024/07/02 : 田中ミノル : 新規作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Manager.h"

// ====== クラスの定義 ======
class ShaderManager : public Manager<ShaderManager>
{
public:	// 列挙体・構造体定義
	/// <summary>
	/// シェーダの種類
	/// </summary>
	enum class ShaderType
	{
		Vertex,		// 頂点シェーダ
		Pixel,		// ピクセルシェーダ
		Geometry,	// ジオメトリシェーダ
		Hull,		// ハルシェーダ
		Domain,		// ドメインシェーダ
		Compute,	// コンピュートシェーダ
		Max			// 最大数
	};

	/// <summary>
	/// シェーダの情報
	/// </summary>
	/// <param name="type">シェーダの種類</param>
	/// <param name="blob">シェーダのバッファ</param>
	struct ShaderInfo
	{
		ShaderType type;		// シェーダの種類
		ComPtr<ID3DBlob> blob;	// シェーダのバッファ
	};

public:		//	パブリック関数

	/// <summary>
	/// シェーダを読み込んでマップに登録する関数
	/// </summary>
	/// <param name="path">ファイルパス</param>
	/// <param name="name">シェーダの名前</param>
	/// <param name="type">シェーダの種類</param>
	/// <returns>成功したかどうか</returns>
	bool LoadShader(const string& path, const string& name, const ShaderType& type);

	/// <summary>
	/// シェーダを取得する関数
	/// </summary>
	/// <param name="name">シェーダの名前</param>
	/// <returns>シェーダのバッファ</returns>
	/// <returns>失敗したらnullptr</returns>
	ID3DBlob* GetShader(const string& name);

	/// <summary>
	/// 初期化関数
	/// </summary>
	/// <returns>成功したかどうか</returns>
	bool Init();

	/// <summary>
	/// 解放関数
	/// </summary>
	/// <returns>成功したかどうか</returns>
	bool Uninit();

private:	// メンバ変数
	unordered_map<string, ShaderInfo> m_shaderMap;	// シェーダのマップ

private:	// シングルトン関連
	friend class Manager<ShaderManager>;
	ShaderManager() = default;
};
// ShaderManagerクラスのインスタンスを取得する
#define SHADER_MANAGER ShaderManager::manager()