// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : メッシュクラスの定義
// 更新履歴 : 2024/05/25 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics/Renderer.h>

// ====== 名前空間 ======
using namespace DirectX;

// ====== クラス定義 ======
class Mesh
{
private:	// 型定義
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="pDev">レンダラークラスのポインタ</param>
	void Create(Renderer* pDev);

	/// <summary>
	/// インスタンス描画
	/// </summary>
	void DrawInstanced() const;

private:
	Renderer* m_pRenderer = nullptr;
	ComPtr<ID3D12Resource> m_pVertexBuff = nullptr;
	ComPtr<ID3D12Resource> m_pIndexBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView;
	D3D12_INDEX_BUFFER_VIEW m_ibView;
	XMFLOAT3 m_vertices[3];
};