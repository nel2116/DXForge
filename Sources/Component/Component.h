// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Component.h]
// 制作者 : 田中ミノル
// 制作日 : 2024/06/04 16:20
// 概要   : Componentクラスのヘッダファイル
// 更新日
// 2024/06/04 16:20 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <string>

// ====== クラスの定義 ======
class Actor;
class Component
{
public:	// メンバ関数
	Component(std::string tag) : m_pOwner(nullptr), m_bIsActive(true), m_bIsDead(false), m_nOrder(0), m_sTag(tag) {}
	virtual ~Component() = default;

	/// @brief 更新処理の純粋仮想関数
	/// @remarks これを継承して更新処理を記述する
	virtual void Update() = 0;
	/// @brief 描画処理の純粋仮想関数
	/// @remarks これを継承して描画処理を記述する
	virtual void Draw() = 0;
	/// @brief 初期化処理の純粋仮想関数
	/// @remarks これを継承して初期化処理を記述する
	virtual void Init() = 0;
	/// @brief 終了処理の純粋仮想関数
	/// @remarks これを継承して終了処理を記述する
	virtual void Uninit() = 0;

	/// @brief コンポーネントの破棄
	void Destroy() { m_bIsDead = true; }

public:	// アクセサ
	/// @brief 親オブジェクトの設定
	/// @param parent 親オブジェクト
	void SetOwner(Actor* parent) { m_pOwner = parent; }
	/// @brief 親オブジェクトの取得
	/// @return Actor* 親オブジェクト
	Actor* GetOwner() { return m_pOwner; }
	/// @brief 更新順序の設定
	/// @param nOrder 更新順序
	void SetOrder(int nOrder) { m_nOrder = nOrder; }
	/// @brief 更新順序の取得
	/// @return int 更新順序
	int GetOrder() { return m_nOrder; }
	/// @brief アクティブフラグの設定
	/// @param bIsActive アクティブフラグ
	void SetActive(bool bIsActive) { m_bIsActive = bIsActive; }
	/// @brief アクティブフラグの取得
	/// @return bool アクティブフラグ
	bool IsActive() { return m_bIsActive; }
	/// @brief 死亡フラグの取得
	/// @return bool 死亡フラグ
	bool IsDestroy() { return m_bIsDead; }
	/// @brief タグの設定
	/// @param tag タグ
	void SetTag(const std::string& tag) { m_sTag = tag; }
	/// @brief タグの取得
	/// @return std::string タグ
	std::string GetTag() { return m_sTag; }

protected:
	Actor* m_pOwner;	// 親オブジェクト

private:
	bool m_bIsActive;	// アクティブフラグ : falseの場合、コンポーネントは更新されない
	bool m_bIsDead;		// 死亡フラグ : trueの場合、コンポーネントは削除される
	int m_nOrder;		// 更新順序
	std::string m_sTag;	// タグ
};