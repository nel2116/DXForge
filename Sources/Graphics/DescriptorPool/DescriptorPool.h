// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/13 16:30
// 作成者 : 田中ミノル
// 概要   : ディスクリプタプールクラスの定義
// 更新履歴
// 2024/06/13 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <d3d12.h>
#include <atomic>
#include <System/ComPtr.h>
#include <Graphics/Pool/Pool.h>

// ====== クラス定義 ======
class DescriptorHandle
{
public:
	D3D12_CPU_DESCRIPTOR_HANDLE HandleCPU;	// CPUディスクリプタハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE HandleGPU;	// GPUディスクリプタハンドル

	bool HasCPU() const { return HandleCPU.ptr != 0; }
	bool HasGPU() const { return HandleGPU.ptr != 0; }
};

class DescriptorPool
{
public:
	/// <summary>
	/// 生成処理
	/// </summary>
	/// <param name="pDevice">デバイス</param>
	/// <param name="pDesc">ディスクリプタヒープの構成設定</param>
	/// <param name="ppPool">生成されたディスクリプタプールの格納先</param>
	/// <returns>生成に成功したらtrue</returns>
	static bool Create(ID3D12Device* pDevice, const D3D12_DESCRIPTOR_HEAP_DESC* pDesc, DescriptorPool** ppPool);

	/// <summary>
	/// 参照カウントを増やす
	/// <summary>
	void AddRef();

	/// <summary>
	/// 開放処理を行う
	/// </summary>
	void Release();

	/// <summary>
	/// 参照カウントの取得
	/// </summary>
	/// <returns>参照カウント</returns>
	uint32_t GetCount() const;

	/// <summary>
	/// ディスクリプタハンドルを貼り当てる
	/// </summary>
	/// <returns>割り当てられたディスクリプタハンドル</returns>
	DescriptorHandle* AllocHandle();

	/// <summary>
	/// ディスクリプタハンドルを開放する
	/// </summary>
	/// <param name="pHandle">開放するディスクリプタハンドル</param>
	void FreeHandle(DescriptorHandle*& pHandle);

	/// <summary>
	///  利用可能なハンドル数を取得する
	/// </summary>
	/// <returns>利用可能なハンドル数</returns>
	uint32_t GetAvailableHandleCount() const;

	/// <summary>
	/// 割り当て済みのハンドル数を取得する
	/// </summary>
	/// <returns>割り当て済みのハンドル数</returns>
	uint32_t GetAllocatedHandleCount() const;

	/// <summary>
	/// ハンドル総数を取得する
	/// </summary>
	/// <returns>ハンドル総数</returns>
	uint32_t GetHandleCount() const;

	/// <summary>
	/// ディスクリプタヒープを取得する
	/// </summary>
	/// <returns>ディスクリプタヒープ</returns>
	ID3D12DescriptorHeap* const GetHeap() const;

private:	// プライベート関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	DescriptorPool();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DescriptorPool();

	DescriptorPool(const DescriptorPool&) = delete;	// コピーコンストラクタの削除
	void operator=(const DescriptorPool&) = delete;	// 代入演算子の削除

private:
	std::atomic<uint32_t> m_RefCount;	// 参照カウンタ
	Pool<DescriptorHandle> m_Pool;	// プール
	ComPtr<ID3D12DescriptorHeap> m_pHeap;	// ディスクリプタヒープ
	uint32_t m_DescriptorSize;	// ディスクリプタサイズ

};