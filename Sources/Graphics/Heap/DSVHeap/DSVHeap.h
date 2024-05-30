// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [DSVHeap.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/30 09:30
// 概要   : 深度ステンシルヒープクラスの定義
// 更新履歴
// 2024/05/30 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "../Heap.h"

class DSVHeap : public Heap<int>
{
public:
	DSVHeap() {}
	~DSVHeap() {}

	/// <summary>
	/// 深度ステンシルヒープ生成
	/// </summary>
	/// <param name="pBuffer">バッファーのポインタ</param>
	/// <param name="format">フォーマット</param>
	/// <returns>ヒープの紐付けられた登録番号</returns>
	int CreateDSV(ID3D12Resource* pBuffer, DXGI_FORMAT format);
private:
};