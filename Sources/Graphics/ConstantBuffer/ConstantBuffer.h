// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/10
// 作成者 : 田中ミノル
// 概要   : 定数バッファクラスの定義
// 更新履歴
// 2024/06/13 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <d3d12.h>
#include <System/ComPtr.h>
#include <vector>


// ====== クラス定義 ======
class DescriptorHandle;
class DescriptorPool;
class ConstantBuffer
{
public:	// パブリック関数
	ConstantBuffer();
	~ConstantBuffer();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="pPool">ディスクリプタプール</param>
	/// <param name="size">サイズ</param>
	/// <returns>初期化に成功したらture</returns>
	bool Init(DescriptorPool* pPool, size_t size);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// GPU仮想アドレスを取得
	/// </summary>
	/// <returns>GPU仮想アドレス</returns>
	D3D12_GPU_VIRTUAL_ADDRESS GetAddress() const;

	/// <summary>
	/// CPUディスクリプタハンドルを取得
	/// </summary>
	/// <returns>CPUディスクリプタハンドル</returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;

	/// <summary>
	/// GPUディスクリプタハンドルを取得
	/// </summary>
	/// <returns>GPUディスクリプタハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;

	/// <summary>
	/// メモリマッピング済みポインタを取得
	/// </summary>
	/// <returns>メモリマッピング済みポインタ</returns>
	void* GetPtr() const;

	/// <summary>
	/// メモリマッピング済みポインタを取得
	/// </summary>
	/// <returns>メモリマッピング済みポインタ</returns>
	template<typename T>
	T* GetPtr()
	{
		return reinterpret_cast<T*>(GetPtr());
	}

private:	// メンバ変数
	ComPtr<ID3D12Resource> m_pCB;			// 定数バッファ.
	DescriptorHandle* m_pHandle;			// ディスクリプタハンドルです.
	DescriptorPool* m_pPool;				// ディスクリプタプールです.
	D3D12_CONSTANT_BUFFER_VIEW_DESC m_Desc;	// 定数バッファビューの構成設定.
	void* m_pMappedPtr;						// マップ済みポインタ.

private:	// プライベート関数
	ConstantBuffer(const ConstantBuffer&) = delete;       // アクセス禁止.
	void operator = (const ConstantBuffer&) = delete;       // アクセス禁止.
};


