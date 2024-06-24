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
	std::vector<Actor*> m_pActors;	// Actorリスト
	std::vector<Actor*> m_pDeadActors;	// 破棄されたActorリスト


private:	// シングルトン関連
	friend class Manager<ActorManager>;
	ActorManager() = default;

	// デフォルトコンストラクタ以外でインスタンスを構築したい場合、createInstance()を上書きする
	// 例
	// Hoge(int) { std::cout << "Hoge(int)" << std::endl; }
	// static Hoge *createInstance()
	// {
	//   return NEW Hoge(0);
	// }
};

// インスタンスの取得
#define ACTOR_MANAGER ActorManager::manager()

template<class T>
inline T* ActorManager::AddActor()
{
	// Actorの追加
	T* buff = NEW T();
	m_pActors.push_back(buff);
	buff->BaseInit();
	return buff;
}

template<class T>
inline T* ActorManager::GetActor()
{
	// Actorの取得
	for (Actor* pActor : m_pActors)
	{
		if (typeid(*pActor) == typeid(T))
		{
			return (T*)pActor;
		}
	}
	return nullptr;
}

template<class T>
inline T* ActorManager::GetActor(const string& tag)
{
	// Actorの取得
	for (Actor* pActor : m_pActors)
	{
		if (typeid(*pActor) == typeid(T))
		{
			if (pActor->GetTag() == tag)
			{
				return (T*)pActor;
			}
		}
	}
	return nullptr;
}
