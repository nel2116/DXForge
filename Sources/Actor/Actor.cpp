#include "stdafx.h"
#include "Actor.h"

void Actor::BaseUpdate()
{
	// ループ中は一時的なリストに追加
	m_bLooping = true;
	// 先にコンポーネントを更新
	auto buff = m_pComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		// 破棄されている場合はリストに移動させて次へ
		if ((*cmp)->IsDead()) { m_pDeadComponents.push_back(*cmp); continue; }
		// 使われていない場合は次へ
		if (!(*cmp)->IsActive()) { continue; }
		// 更新
		(*cmp)->Update();
	}
	m_bLooping = false;

	// 一時的なリストのコンポーネントがあれば
	if (m_pTempComponents.size() != 0)
	{
		// 一時的なリストのコンポーネントを本リストに移動
		auto temp = m_pTempComponents;
		for (auto cmp = temp.begin(), end = temp.end(); cmp != end; ++cmp)
		{
			m_pComponents.push_back(*cmp);
		}
		// コンポーネントの並び替え
		SortComponent();
		// 一時的なリストをクリア
		m_pTempComponents.clear();
	}

	// 自身の更新
	Update();
}

void Actor::BaseDraw()
{
	// ループ中は一時的なリストに追加
	m_bLooping = true;
	// 先にコンポーネントを描画
	auto buff = m_pComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		// 破棄されている場合はリストに移動させて次へ
		if ((*cmp)->IsDead()) { m_pDeadComponents.push_back(*cmp); continue; }
		// 使われていない場合は次へ
		if (!(*cmp)->IsActive()) { continue; }
		// 描画
		(*cmp)->Draw();
	}
	m_bLooping = false;

	// 自身の描画
	Draw();

	// 破棄フラグのたったコンポーネントがあれば削除
	if (m_pDeadComponents.size() != 0) { DestroyComponent(); SortComponent(); }
}

void Actor::BaseInit()
{
	Init();
}

void Actor::BaseUninit()
{
	// コンポーネントの破棄
	for (Component* cmp : m_pComponents)
	{
		cmp->Uninit();
		SAFE_DELETE(cmp);
	}
	m_pComponents.clear();

	// 一時的なリストも破棄
	for (Component* cmp : m_pTempComponents)
	{
		cmp->Uninit();
		SAFE_DELETE(cmp);
	}
	m_pTempComponents.clear();

	// 破棄フラグのたったコンポーネントも破棄
	for (Component* cmp : m_pDeadComponents)
	{
		cmp->Uninit();
		SAFE_DELETE(cmp);
	}
	m_pDeadComponents.clear();

	// 自身の終了処理
	Uninit();
}

void Actor::SortComponent()
{
	// Orderの順番で昇順に並べる
	std::sort(m_pComponents.begin(), m_pComponents.end(), [](Component* a, Component* b) { return a->GetOrder() < b->GetOrder(); });
}

void Actor::DestroyComponent()
{
	// 破棄フラグのたったコンポーネントを削除
	auto buff = m_pDeadComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		// コンポーネント配列からも削除
		auto itr = std::find(m_pComponents.begin(), m_pComponents.end(), (*cmp));
		if (itr != m_pComponents.end())
		{
			m_pComponents.erase(itr);
		}
		(*cmp)->Uninit();
		SAFE_DELETE(*cmp);
	}
	m_pDeadComponents.clear();
}


