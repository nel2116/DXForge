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
#include <Graphics\Renderer.h>

// ====== クラス定義 ======
class Texture
{
private:	// 型定義
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
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
	void Set(int index);

	/// <summary>
	/// SRV番号を取得
	/// </summary>
	/// <returns>SRV番号</returns>
	int GetSRVNumber() { return m_srvNumber; }

private:
	Renderer* m_pRenderer = nullptr;

	ComPtr<ID3D12Resource> m_pTexture = nullptr;
	int m_srvNumber = 0;
};