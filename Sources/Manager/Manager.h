// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 制作者 : 田中ミノル
// 制作日 : 2024/06/06 18:00
// 概要   : Manager基底クラスのヘッダファイル , 参考：https://cflat-inc.hatenablog.com/entry/2014/03/04/214608
// 更新日 
// 2024/06/06 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <memory>
#include <System/Macro.h>

// ====== クラスの定義 ======
template<class T>
class Manager
{
public:
	static T& manager()
	{
		static typename T::manager_pointer_type s_manager(T::createInstance());
		return getReference(s_manager);
	}

private:
	typedef std::unique_ptr<T> manager_pointer_type;

	inline static T* createInstance()
	{
		return NEW T();
	}

	inline static T& getReference(const manager_pointer_type& manager)
	{
		return *manager;
	}

protected:
	Manager() = default;


private:
	Manager(const Manager&) = delete;
	Manager& operator=(const Manager&) = delete;
	Manager(Manager&&) = delete;
	Manager& operator=(Manager&&) = delete;

};
