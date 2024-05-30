// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Texture.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 12:00
// 概要   : テクスチャクラスの定義
// 更新履歴
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "../Buffer.h"
#include <vector>

// ====== クラス定義 ======
struct Vertex
{
	Vertex(DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT2 UV) : pos(Pos), uv(UV) {}
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

class Texture : public Buffer
{
public:		// パブリック関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成功したらtrue</returns>
	static bool Init();

	/// <summary>
	/// テクスチャのロード
	/// </summary>
	/// <param name="pDev">レンダラークラスのポインタ</param>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>成功したらtrue</returns>
	bool Load(Renderer* pDev, const string& filePath);

	/// <summary>
	/// シェーダーリソースとしてセット
	/// </summary>
	/// <param name="index">インデックス</param>
	void Set(int index) const;

	/// <summary>
	/// SRV番号を取得
	/// </summary>
	/// <returns>SRV番号</returns>
	const int GetSRVNumber() const { return m_srvNumber; }

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() const;

private:
	int m_srvNumber = 0;	// SRV番号

	static Microsoft::WRL::ComPtr<ID3D12Resource> m_pVBuffer;	// バッファ
	static Microsoft::WRL::ComPtr<ID3D12Resource> m_pIBuffer;	// バッファ
	static D3D12_VERTEX_BUFFER_VIEW m_vbView;					// 頂点バッファビュー
	static D3D12_INDEX_BUFFER_VIEW m_ibView;					// インデックスバッファビュー
	static std::vector<Vertex> m_vertex;						// 頂点座標
	static std::vector<UINT> m_index;							// インデックス	
};