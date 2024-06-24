// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [CommandList.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/24 13:05 : 作成
// 概要   : コマンドリストクラスの定義
// 更新履歴
// 2024/06/24  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <System/ComPtr.h>
#include <d3d12.h>
#include <cstdint>
#include <vector>

// ====== クラス定義 ======
class CommandList
{
public:
	// コンストラクタ
	CommandList();

	// デストラクタ
	~CommandList();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="type">コマンドリストの種類</param>
	/// <param name="count">アロケータの数</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(D3D12_COMMAND_LIST_TYPE type, size_t count);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// コマンドリストの取得
	/// </summary>
	/// <returns>コマンドリストのポインタ</returns>
	ID3D12GraphicsCommandList* Get() const;

	/// <summary>
	/// リセット処理を行ったコマンドリストを取得
	/// </summary>
	/// <returns>コマンドリストのポインタ</returns>
	ID3D12GraphicsCommandList* Reset();

private:	// メンバ変数
	ComPtr<ID3D12GraphicsCommandList> m_pCmdList;					// コマンドリスト
	std::vector<ComPtr<ID3D12CommandAllocator>> m_pAllocators;      // コマンドアロケータ
	uint32_t m_Index;												// 現在のアロケータのインデックス

private:	// コピー禁止
	CommandList(CommandList const&) = delete;
	CommandList& operator=(CommandList const&) = delete;
};