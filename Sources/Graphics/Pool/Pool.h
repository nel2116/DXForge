// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/13 16:30
// 作成者 : 田中ミノル
// 概要   : プールクラスの定義
// 更新履歴
// 2024/06/13 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>

// ====== クラス定義 ======
template <typename T>
class Pool
{
public:	// 構造体定義
	struct Item
	{
		T m_Value;
		uint32_t m_Index = 0;	// インデックス
		Item* m_pNext = nullptr;	// 次のアイテム
		Item* m_pPrev = nullptr;	// 前のアイテム

		Item()
			: m_Value()
			, m_Index(0)
			, m_pNext(nullptr)
			, m_pPrev(nullptr)
		{}

		~Item() {}
	};
public:		// パブリック関数
	Pool()
		: m_pBuffer(nullptr)
		, m_pActive(nullptr)
		, m_pFree(nullptr)
		, m_Capacity(0)
		, m_Count(0)
	{}
	~Pool() { Uninit(); }

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="count">確保するアイテム数
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(uint32_t count)
	{
		std::lock_guard<std::mutex>gurand(m_Mutex);

		m_pBuffer = static_cast<uint32_t*>(malloc(sizeof(Item) * (count + 2)));
		// メモリ確保に失敗
		if (!m_pBuffer) { return false; }

		m_Capacity = count;

		// インデックスを振る
		for (auto i = 2u, j = 0u; i < m_Capacity + 2; ++i, ++j)
		{
			auto item = GetItem(i);
			item->m_Index = j;
		}

		m_pActive = GetItem(0);
		m_pActive->m_pPrev = m_pActive->m_pNext = m_pActive;
		m_pActive->m_Index = uint32_t(-1);

		m_pFree = GetItem(1);
		m_pFree->m_Index = uint32_t(-2);

		for (auto i = 1u; i < m_Capacity + 2; ++i)
		{
			GetItem(i)->m_pPrev = nullptr;
			GetItem(i)->m_pNext = GetItem(i + 1);
		}

		GetItem(m_Capacity + 1)->m_pPrev = m_pFree;
		m_Count = 0;

		return true;
	}

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit()
	{
		std::lock_guard<std::mutex>gurand(m_Mutex);

		if (m_pBuffer)
		{
			free(m_pBuffer);
			m_pBuffer = nullptr;
		}
		m_pActive = nullptr;
		m_pFree = nullptr;
		m_Capacity = 0;
		m_Count = 0;
	}

	/// <summary>
	/// アイテムを確保する
	/// </summary>
	/// <param name="func">ユーザーによる初期化処理</param>
	/// <returns>アイテムのポインタ、確保に失敗した場合はnullptr</returns>
	T* Alloc(std::function<void(uint32_t, T*)> func = nullptr)
	{
		std::lock_guard<std::mutex>gurand(m_Mutex);

		// 空きアイテムを取得
		if (m_pFree->m_pNext == m_pFree || m_Count + 1 > m_Capacity)
		{
			return nullptr;
		}

		auto item = m_pFree->m_pNext;
		m_pFree->m_pNext = item->m_pNext;

		item->m_pPrev = m_pActive->m_pPrev;
		item->m_pNext = m_pActive;
		item->m_pPrev->m_pNext = item->m_pNext->m_pPrev = item;

		m_Count++;

		// メモリ割り当て
		auto val = NEW((void*)item)T();

		// ユーザー初期化処理
		if (func)
		{
			func(item->m_Index, val);
		}
		return val;
	}

	/// <summary>
	/// アイテムを解放する
	/// </summary>
	/// <param name="pValue">解放するアイテムのポインタ</param>
	void Free(T* pValue)
	{
		// ポインタがnullptr
		if (!pValue) { return; }

		std::lock_guard<std::mutex>gurand(m_Mutex);

		auto item = reinterpret_cast<Item*>(pValue);

		item->m_pPrev->m_pNext = item->m_pNext;
		item->m_pNext->m_pPrev = item->m_pPrev;

		item->m_pPrev = nullptr;
		item->m_pNext = m_pFree->m_pNext;

		m_pFree->m_pNext = item;
		m_Count--;
	}

	/// <summary>
	/// 総アイテム数を取得する
	/// </summary>
	/// <returns>総アイテム数</returns>
	uint32_t GetSize() const
	{
		return m_Capacity;
	}

	/// <summary>
	///	使用中のアイテム数を取得する
	/// </summary>
	///	<returns>使用中のアイテム数</returns>
	uint32_t GetUsedCount() const
	{
		return m_Count;
	}

	/// <summary>
	/// 使用可能なアイテム数を取得する
	/// </summary>
	/// <returns>使用可能なアイテム数</returns>
	uint32_t GetAvailableCount() const
	{
		return m_Capacity - m_Count;
	}

private:	// プライベート関数
	/// <summary>
	/// アイテムを取得する
	/// </summary>
	/// <param name="index">取得するアイテムのインデックス</param>
	/// <returns>アイテムのポインタ</returns>
	Item* GetItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		return reinterpret_cast<Item*>(m_pBuffer + sizeof(Item) * index);
	}

	/// <summary>
	/// アイテムにメモリを割り当てる
	/// </summary>
	/// <param name="index">取得するアイテムのインデックス</param>
	/// <returns>アイテムへのポインタ
	Item* AllocItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		auto buf = (m_pBuffer + sizeof(Item) * index);
		return NEW(buf)Item();
	}
	Pool(const Pool&) = delete;
	Pool& operator=(const Pool&) = delete;

private:
	uint8_t* m_pBuffer = nullptr;	// バッファ
	Item* m_pActive = nullptr;	// アクティブなアイテム

	Item* m_pFree = nullptr;	// アイテムの先頭
	uint32_t m_Capacity = 0;	// 総アイテム数
	uint32_t m_Count = 0;	// 現在のアイテム数
	std::mutex m_Mutex;	// ミューテックス
};