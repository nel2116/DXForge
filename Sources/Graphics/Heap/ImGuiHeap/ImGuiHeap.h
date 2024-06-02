// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [imuGuiHeap.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/01 16:00
// 概要   : imuGuiヒープクラスの定義
// 更新履歴
// 2024/06/01 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "../Heap.h"

class ImGuiHeap : public Heap<int>
{

public:
	ImGuiHeap() {}
	~ImGuiHeap() {}

	/// <summary>
	/// imuGuiヒープ生成
	/// </summary>
	/// <param name="pBuffer">バッファーのポインタ</param>
	/// <param name="format">フォーマット</param>
	/// <returns>ヒープの紐付けられた登録番号</returns>
	int CreateimuGui(ID3D12Resource* pBuffer, DXGI_FORMAT format);

private:
};