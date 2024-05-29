// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Model.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 19:30
// 概要   : モデルクラスの定義
// 更新履歴 
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Model.h"
#include "ModelLoader.h"

bool Model::Load(const std::string& filePath)
{
	ModelLoader modelLoader;
	if (!modelLoader.Load(filePath, m_nodes))
	{
		assert(0 && "モデルのロードに失敗しました。");
		return false;
	}
	return true;
}
