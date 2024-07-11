// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 制作者 : 田中ミノル
// 制作日 : 2024/06/04 16:20
// 概要   : Actorクラスのヘッダファイル
// 更新日
// 2024/06/04 16:20 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <vector>
#include <Component/Component.h>

using namespace std;


// ====== クラスの定義 ======
/// @brief アクタークラス
/// @details ゲームオブジェクトの基底クラス
class Actor
{
public:	// パブリック関数
	/// @brief コンストラクタ
	Actor() : m_bDestroy(false), m_bActive(true), m_sTag("none"), m_nOrder(0), m_bLooping(false) {}

	/// @brief デストラクタ
	virtual ~Actor() { DestroyComponent(); }

	/// @brief 更新処理の純粋仮想関数

	virtual void Update() = 0;

	/// <summary>
	/// 描画処理の純粋仮想関数
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Init() {}

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Uninit() {}

	/// <summary>
	/// 更新処理の基底関数
	/// </summary>
	/// <remarks>コンポーネント => アクターの順番で更新</remarks>
	/// <remarks>Managerから呼び出される</remarks>
	void BaseUpdate();

	/// <summary>
	/// 描画処理の基底関数
	/// </summary>
	/// <remarks>コンポーネント => アクターの順番で描画</remarks>
	/// <remarks>Managerから呼び出される</remarks>
	void BaseDraw();

	/// <summary>
	/// 初期化処理の基底関数
	/// </summary>
	void BaseInit();

	/// <summary>
	/// 終了処理の基底関数
	/// </summary>
	void BaseUninit();

public:	// アクセサ

	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <param name="T">コンポーネントの型</param>
	/// <param name="tag">コンポーネントのタグ</param>
	/// <returns>追加したコンポーネントのポインタ</returns>
	/// <remarks>コンポーネントの生成と追加を行う</remarks>
	template<class T>
	T* AddComponent(std::string tag = "none");

	/// <summary>
	/// コンポーネントの取得
	/// </summary>
	/// <param name="T">コンポーネントの型</param>
	/// <returns>取得したコンポーネントのポインタ</returns>
	/// <remarks>コンポーネントの取得を行う</remarks>
	template<class T>
	T* GetComponent();

	/// <summary>
	/// コンポーネントの取得
	/// </summary>
	/// <param name="T">コンポーネントの型</param>
	/// <param name="tag">コンポーネントのタグ</param>
	/// <returns>取得したコンポーネントのポインタ</returns>
	/// <remarks>タグで指定してコンポーネントの取得を行う</remarks>
	template<class T>
	T* GetComponent(const string& tag);

	/// <summary>
	/// 自身の破棄フラグを立てる
	/// </summary>
	/// <remarks>自身を破棄する</remarks>
	/// <remarks>破棄されたアクターは削除される</remarks>
	/// <remarks>破棄されたアクターはコンポーネントも破棄される</remarks>
	void Destroy() { m_bDestroy = true; }

	/// <summary>
	///破棄亡フラグの取得
	/// </summary>
	/// <returns>破棄フラグ</returns>
	/// <remarks>破棄フラグの取得を行う</remarks>
	bool IsDestroy() { return m_bDestroy; }

	/// <summary>
	/// アクティブフラグの設定
	/// </summary>
	/// <param name="bIsActive">アクティブフラグ</param>
	/// <remarks>アクティブフラグの設定を行う</remarks>
	/// <remarks>アクティブフラグがfalseの場合、更新されない</remarks>
	void SetActive(bool bIsActive) { m_bActive = bIsActive; }

	/// <summary>
	/// アクティブフラグの取得
	/// </summary>
	/// <returns>アクティブフラグ</returns>
	bool IsActive() { return m_bActive; }

	/// <summary>
	/// タグの設定
	/// </summary>
	/// <param name="tag">タグ</param>
	/// <remarks>タグの設定を行う</remarks>
	/// <remarks>タグはActorの取得に使用される</remarks>
	void SetTag(const string& tag) { m_sTag = tag; }

	/// <summary>
	/// タグの取得
	/// </summary>
	/// <returns>タグ</returns>
	/// <remarks>タグの取得を行う</remarks>
	/// <remarks>タグはActorの取得に使用される</remarks>
	string GetTag() { return m_sTag; }

	/// <summary>
	/// 更新順序の設定
	/// </summary>
	/// <param name="nOrder">更新順序</param>
	/// <remarks>更新順序の設定を行う</remarks>
	/// <remarks>Orderの昇順で更新される</remarks>
	void SetOrder(int nOrder) { m_nOrder = nOrder; }

	/// <summary>
	/// 更新順序の取得
	/// </summary>
	/// <returns>更新順序</returns>
	/// <remarks>Orderの昇順で更新される</remarks>
	int GetOrder() { return m_nOrder; }

private:
	/// <summary>
	/// コンポーネントの整列
	/// </summary>
	/// <remarks>コンポーネントの整列を行う</remarks>
	/// <remarks>Orderの昇順で整列される</remarks>
	void SortComponent();

	/// <summary>
	/// コンポーネントの破棄
	/// </summary>
	/// <remarks>コンポーネントの破棄を行う</remarks>
	/// <remarks>破棄されたコンポーネントはリストから削除される</remarks>
	void DestroyComponent();

private:
	/// <summary>
	/// コンポーネントリスト
	/// </summary>
	/// <remarks>Actorが持つコンポーネントのリスト</remarks>
	vector<Component*> m_pComponents;

	/// <summary>
	/// 一時的なコンポーネントリスト
	/// </summary>
	/// <remarks>コンポーネントの追加中にイテレータが操作されないように一時的にComponentを保持する配列</remarks>
	vector<Component*> m_pTempComponents;

	/// <summary>
	/// 破棄されたコンポーネントリスト
	/// </summary>
	/// <remarks>破棄されたコンポーネントのリスト</remarks>
	/// <remarks>破棄されたコンポーネントはリストから削除される</remarks>
	vector<Component*> m_pDeadComponents;

	/// <summary>
	/// 破棄フラグ
	/// </summary>
	/// <remarks>破棄フラグがtrueの場合、Actorは削除される</remarks>
	/// <remarks>破棄されたActorはコンポーネントも破棄される</remarks>
	bool m_bDestroy;

	/// <summary>
	/// アクティブフラグ
	/// </summary>
	/// <remarks>アクティブフラグがfalseの場合、Actorは更新されない</remarks>
	/// <remarks>アクティブフラグがfalseの場合、コンポーネントも更新されない</remarks>
	bool m_bActive;

	/// <summary>
	/// タグ
	/// </summary>
	/// <remarks>Actorの取得に使用される</remarks>
	std::string m_sTag;

	/// <summary>
	/// 更新順序
	/// </summary>
	/// <remarks>Orderの昇順で更新される</remarks>
	int m_nOrder;

	/// <summary>
	/// ループフラグ
	/// </summary>
	bool m_bLooping;
};

template<class T>
inline T* Actor::AddComponent(std::string tag)
{
	// コンポーネントの生成
	T* pCmp = NEW T(tag);

	// オーナーの設定
	pCmp->SetOwner(this);

	// イテレータが操作されないよう、ループ中は一時的なリストに追加する
	if (!m_bLooping)
	{
		// コンポーネントの追加
		m_pComponents.push_back(pCmp);
		// コンポーネントの整列
		SortComponent();
	}
	else
	{
		// 一時的なリストに追加
		m_pTempComponents.push_back(pCmp);
	}

	// コンポーネントの初期化
	((Component*)pCmp)->Init();

	return pCmp;
}

template<class T>
inline T* Actor::GetComponent()
{
	// コンポーネントの取得
	auto buff = m_pComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		if (typeid(*(*cmp)) == typeid(T))
		{
			return (T*)(*cmp);
		}
	}

	// あれば一時的なリストからも探す
	if (m_pTempComponents.size() != 0)
	{
		auto temp = m_pTempComponents;
		for (auto cmp = temp.begin(), end = temp.end(); cmp != end; ++cmp)
		{
			if (typeid(*(*cmp)) == typeid(T))
			{
				return (T*)(*cmp);
			}
		}
	}

	return nullptr;
}

template<class T>
inline T* Actor::GetComponent(const string& tag)
{
	// コンポーネントの取得
	auto buff = m_pComponents;
	for (auto cmp = buff.begin(), end = buff.end(); cmp != end; ++cmp)
	{
		if (typeid(*(*cmp)) == typeid(T) && (*cmp)->GetTag() == tag)
		{
			return (T*)(*cmp);
		}
	}
	// あれば一時的なリストからも探す
	if (m_pTempComponents.size() != 0)
	{
		auto temp = m_pTempComponents;
		for (auto cmp = temp.begin(), end = temp.end(); cmp != end; ++cmp)
		{
			if (typeid(*(*cmp)) == typeid(T) && (*cmp)->GetTag() == tag)
			{
				return (T*)(*cmp);
			}
		}
	}

	return nullptr;
}

