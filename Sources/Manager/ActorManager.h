// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 制作者 : 田中ミノル
// 制作日 : 2024/06/06 18:00
// 概要   : ActorManagerクラスのヘッダファイル	:	Actorクラスの管理クラス
// 更新日 
// 2024/06/06 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include "Manager.h"
#include <Actor/Actor.h>

// ====== クラスの定義 ======
class ActorManager : public Manager<ActorManager>
{

public:	// メンバ関数

	// 更新
	void Update();
	// 描画
	void Draw();
	// 初期化
	void Init();
	// 終了
	void Uninit();

public:	// アクセサ

	// Actorの追加
	template<class T>
	T* AddActor();
	// Actorの取得
	template<class T>
	T* GetActor();
	// Actorをタグで指定して取得
	template<class T>
	T* GetActor(const string& tag);

private:	// プライベート関数
	// Actorの破棄
	void DestroyActor();

	// ActorのリストをOrderで昇順に並べる
	void SortActor();

private:	// メンバ変数
	std::vector<Actor*> m_pActors;		// Actorリスト
	std::vector<Actor*> m_pTempActors;	// 一時的なActorリスト
	std::vector<Actor*> m_pDeadActors;	// 破棄されたActorリスト
	bool m_bLooping;					// ループフラグ

private:	// シングルトン関連
	friend class Manager<ActorManager>;
	ActorManager()
		: m_bLooping(false)
	{}

	static ActorManager* createInstance()
	{
		return NEW ActorManager();
	}

};

// インスタンスの取得
#define ACTOR_MANAGER ActorManager::manager()

template<class T>
inline T* ActorManager::AddActor()
{
	// Actorの生成
	T* buff = NEW T();

	// イテレータが操作されないよう、ループ中は一時的なリストに追加する
	if (!m_bLooping)
	{
		// Actorの追加
		m_pActors.push_back(buff);
		// Actorの並び替え
		SortActor();
	}
	else
	{
		// 一時的なリストに追加
		m_pTempActors.push_back(buff);
	}
	// 初期化
	buff->BaseInit();

	return buff;
}

template<class T>
inline T* ActorManager::GetActor()
{
	// Actorの取得
	auto buff = m_pActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		if (typeid(*(*pActor)) == typeid(T))
		{
			return (T*)(*pActor);
		}
	}
	// あれば一時的なリストからも探す
	if (m_pTempActors.size() != 0)
	{
		auto temp = m_pTempActors;
		for (auto pActor = temp.begin(), end = temp.end(); pActor != end; ++pActor)
		{
			if (typeid(*(*pActor)) == typeid(T))
			{
				return (T*)(*pActor);
			}
		}
	}

	return nullptr;
}

template<class T>
inline T* ActorManager::GetActor(const string& tag)
{
	// Actorの取得
	auto buff = m_pActors;
	for (auto pActor = buff.begin(), end = buff.end(); pActor != end; ++pActor)
	{
		if (typeid(*(*pActor)) == typeid(T) && (*pActor)->GetTag() == tag)
		{
			return (T*)(*pActor);
		}
	}

	// あれば一時的なリストからも探す
	if (m_pTempActors.size() != 0)
	{
		auto temp = m_pTempActors;
		for (auto pActor = temp.begin(), end = temp.end(); pActor != end; ++pActor)
		{
			if (typeid(*(*pActor)) == typeid(T) && (*pActor)->GetTag() == tag)
			{
				return (T*)(*pActor);
			}
		}
	}

	return nullptr;
}
