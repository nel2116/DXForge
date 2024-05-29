// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RTVHeap.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 12:00
// 概要   : RTVヒープクラスの定義
// 更新履歴
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics/Heap/Heap.h>

// ====== クラス定義 ======
class RTVHeap : public Heap<int>
{
public:
	RTVHeap() {};
	~RTVHeap() {};

	/// <summary>
	/// RTVの作成
	/// </summary>
	/// <param name="pBuffer">バッファーのポインタ</param>
	/// <returns>ヒープの紐づけられた登録番号</returns>
	int CreateRTV(ID3D12Resource* pBuffer);

private:
};
