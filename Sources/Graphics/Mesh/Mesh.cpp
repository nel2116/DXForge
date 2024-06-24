// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 11:55 : 作成
// 概要   : メッシュクラスの実装
// 更新履歴
// 2024/06/14  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "Mesh.h"

// コンストラクタ
Mesh::Mesh()
	: m_MaterialId(UINT32_MAX)
	, m_IndexCount(0)
{}

// デストラクタ
Mesh::~Mesh()
{
	Uninit();
}

// 初期化処理
bool Mesh::Init(const ResMesh& resource)
{
	if (!m_VB.Init(sizeof(MeshVertex) * resource.vertices.size(), resource.vertices.data()))
	{
		// 頂点バッファの初期化に失敗
		return false;
	}

	if (!m_IB.Init(sizeof(uint32_t) * resource.indices.size(), resource.indices.data()))
	{
		return false;
	}

	m_MaterialId = resource.materialIdx;
	m_IndexCount = uint32_t(resource.indices.size());

	return true;
}

// 終了処理
void Mesh::Uninit()
{
	m_VB.Uninit();
	m_IB.Uninit();
	m_MaterialId = UINT32_MAX;
	m_IndexCount = 0;
}

// 描画処理
void Mesh::Draw()
{
	auto pCmdList = RENDERER.GetCmdList()->Get();
	auto VBV = m_VB.GetView();
	auto IBV = m_IB.GetView();
	pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCmdList->IASetVertexBuffers(0, 1, &VBV);
	pCmdList->IASetIndexBuffer(&IBV);
	pCmdList->DrawIndexedInstanced(m_IndexCount, 1, 0, 0, 0);
}

// マテリアルIDを取得
uint32_t Mesh::GetMaterialId() const
{
	return m_MaterialId;
}
