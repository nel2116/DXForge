// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [MeshData.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 19:00
// 概要   : メッシュデータの宣言
// 更新履歴
// 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once

// メッシュの頂点情報
struct MeshVertex
{
	DirectX::XMFLOAT3 Pos;				// 位置座標
	DirectX::XMFLOAT2 UV;				// テクスチャ座標
	DirectX::XMFLOAT3 Normal;			// 法線
	UINT Color = 0xFFFFFFFF;	// 色
	DirectX::XMFLOAT3 Tangent;			// 接線
};