// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [ModelLoader.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 19:30
// 概要   : モデルローダークラスの定義
// 更新履歴 
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#ifdef _DEBUG
#pragma comment(lib,"assimp-vc143-mtd.lib")
#else
#pragma comment(lib,"assimp-vc143-mt.lib")	
#endif // _DEBUG

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"

class ModelLoader
{
public:		// パブリック関数
	/// <summary>
	/// モデルのロード
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="nodes">ノード情報</param>
	/// <returns>ロードに成功したらtrue</returns>
	bool Load(const std::string& filePath, std::vector<Model::Node>& nodes);

private:	// プライベート関数
	/// <summary>
	/// 解析
	/// </summary>
	/// <param name="pScene">モデルシーンのポインタ</param>
	/// <param name="pMesh">メッシュのポインタ</param>
	/// <param name="pMaterial">マテリアルのポインタ</param>
	/// <param name="dirPath">ディレクトリパス</param>
	/// <returns>メッシュポインタ</returns>
	std::shared_ptr<Mesh> Parse(const aiScene* pScene, const aiMesh* pMesh, const aiMaterial* pMaterial, const std::string& dirPath);

	/// <summary>
	/// マテリアルの解析
	/// </summary>
	/// <param name="pMaterial">マテリアルのポインタ</param>
	/// <param name="dirPath">ディレクトリパス</param>
	/// <returns>マテリアル情報</returns>
	const Material ParseMaterial(const aiMaterial* pMaterial, const std::string& dirPath);

};