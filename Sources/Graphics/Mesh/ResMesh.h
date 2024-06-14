//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 19:00
// 概要   : メッシュクラスの定義
// 更新履歴
// 2024/06/11 作成
// 2024/06/13 再実装
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <string>
#include <vector>


// ====== 構造体定義 ======
struct ResMaterial
{
	DirectX::XMFLOAT3   Diffuse;        // 拡散反射成分
	DirectX::XMFLOAT3   Specular;       // 鏡面反射成分
	float               Alpha;          // 透過成分
	float               Shininess;      // 鏡面反射強度
	std::wstring        DiffuseMap;     // ディフューズマップファイルパス
	std::wstring        SpecularMap;    // スペキュラーマップファイルパス
	std::wstring        ShininessMap;   // シャイネスマップファイルパス
	std::wstring        NormalMap;      // 法線マップファイルパス
};

struct MeshVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texcoord;
	DirectX::XMFLOAT3 tangent;
	MeshVertex() = default;
	MeshVertex(
		DirectX::XMFLOAT3 const& Pos,
		DirectX::XMFLOAT3 const& Normal,
		DirectX::XMFLOAT2 const& Texcoord,
		DirectX::XMFLOAT3 const& Tangent)
		: pos(Pos), normal(Normal), texcoord(Texcoord), tangent(Tangent) {}
	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 4;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];

};

struct ResMesh
{
	std::vector<MeshVertex> vertices;	// 頂点データ
	std::vector<uint32_t> indices;		// インデックスデータ
	uint32_t materialIdx;				// マテリアル番号
};

/// <summary>
/// メッシュをロードする
/// </summary>
/// <param name="fileName">ファイル名</param>
/// <param name="meshes">メッシュの格納先</param>
/// <param name="materials">マテリアルの格納先</param>
/// <returns>ロードに成功したらtrue</returns>
bool LoadMesh(const wchar_t* fileName, std::vector<ResMesh>& meshes, std::vector<ResMaterial>& materials);