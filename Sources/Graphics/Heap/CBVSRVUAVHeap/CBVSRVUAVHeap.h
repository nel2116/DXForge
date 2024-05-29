// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [CBVSRVUAVHeap.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/29 12:00
// 概要   : CBV/SRV/UAVヒープクラスの定義
// 更新履歴
// 2024/05/29 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics\Heap\Heap.h>

// ====== クラスの定義 ======
class CBVSRVUAVHeap : public Heap<DirectX::XMFLOAT3>
{
public:
	CBVSRVUAVHeap() {}
	~CBVSRVUAVHeap() {}

	/// <summary>
	/// SRVの作成
	/// </summary>
	/// <param name="pBuffer">バッファーのポインタ</param>
	/// <returns>ヒープの紐づけられた登録番号</returns>
	int CreateSRV(ID3D12Resource* pBuffer);

	/// <summary>
	/// SRVのGPU側ハンドルを返す
	/// </summary>
	/// <param name="number">登録番号</param>
	/// <returns>GPU側ハンドル</returns>
	const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int number)override;

	/// <summary>
	/// ヒープをセットする
	/// </summary>
	void SetHeap();

	/// <summary>
/// ヒープの取得関数
/// </summary>
/// <returns></returns>
	ID3D12DescriptorHeap* GetHeap() { return m_pHeap.Get(); }

	/// <summary>
	/// 使用数を取得
	/// </summary>
	/// <returns>使用数</returns>
	const DirectX::XMFLOAT3& GetUseCount() { return m_useCount; }
private:
};




