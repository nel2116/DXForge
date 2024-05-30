// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [DepthStencil.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/30 09:30
// 概要   : 深度ステンシルクラスの定義
// 更新履歴
// 2024/05/30 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "../Buffer.h"

enum class DepthStencilFormat
{
	DepthQuality = DXGI_FORMAT_R16_TYPELESS,
	DepthHighQuality = DXGI_FORMAT_R32_TYPELESS,
	D3pthHighQualityAndStencil = DXGI_FORMAT_R24G8_TYPELESS,
};

class DepthStencil : public Buffer
{
public:		// パブリック関数
	/// <summary>
	/// 深度テクスチャ生成
	/// </summary>
	/// <param name="pDev">レンダラークラスのポインタ</param>
	/// <param name="resolution">解像度</param>
	/// <param name="format">深度テクスチャのフォーマット</param>
	bool Create(Renderer* pDev, const DirectX::XMFLOAT2& resolution, DepthStencilFormat format = DepthStencilFormat::DepthHighQuality);

	/// <summary>
	/// 深度バッファに書き込まれているデータを初期化する関数
	/// </summary>
	void ClearBuffer();

	/// <summary>
	/// DSV番号を取得
	/// </summary>
	/// <returns>DSV番号</returns>
	UINT GetDSVNumber() const { return m_dsvNumber; }

private:	// メンバ変数
	UINT m_dsvNumber = 0;	// DSV番号

};
