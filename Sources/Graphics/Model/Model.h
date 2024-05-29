// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Model.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 19:30
// 概要   : モデルクラスの定義
// 更新履歴 
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics/Mesh/Mesh.h>

class Model
{
public:		// 構造体定義
	struct Node
	{
		std::shared_ptr<Mesh> spMesh;
	};

public:		// パブリック関数
	/// <summary>
	/// モデルのロード
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>ロードに成功したらtrue</returns>
	bool Load(const std::string& filePath);

	/// <summary>
	/// ノードの取得
	/// </summary>
	/// <returns>ノード情報</returns>
	const std::vector<Node>& GetNodes() const { return m_nodes; }

private:	// メンバ変数
	std::vector<Node> m_nodes;
};