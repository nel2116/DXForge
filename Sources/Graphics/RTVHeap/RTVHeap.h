// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [RTVHeap.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/26 16:20
// 概要   : RTVHeapクラスの定義
// 更新履歴
// 2021/05/26 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <wrl/client.h>


// ====== クラス定義 ======
class RTVHeap
{
private:
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:		// メンバ関数
	RTVHeap() : m_useCount(0), m_incrementSize(0), m_nextIndex(0) {};
	~RTVHeap() = default;

	/// <summary>
	/// ヒープ作成
	/// </summary>
	/// <param name="pDevice">デバイスのポインタ</param>
	/// <param name="useCount">使用するRTVの数</param>
	/// <returns>成功したらtrue</returns>
	bool Create(ID3D12Device* pDevice, UINT useCount);

	/// <summary>
	/// RTVの作成
	/// </summary>
	/// <param name="pBuffer">バッファのポインタ</param>
	/// <returns>ヒープの紐づけられた登録番号</returns>
	int CreateRTV(ID3D12Resource* pBuffer);

	/// <summary>
	/// RTVのCPU側アドレスを取得
	/// </summary>
	/// <param name ="number">登録番号</param>
	/// <returns>RTVのGPU側アドレス</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle(int number);

private:
	ID3D12Device* m_pDevice = nullptr;				// デバイスのポインタ
	ComPtr<ID3D12DescriptorHeap> m_pHeap = nullptr;	// ヒープのポインタ
	int m_useCount;									// 使用するRTVの数
	int m_incrementSize;							// インクリメントサイズ
	int m_nextIndex;								// 次に作成するRTVの番号
};