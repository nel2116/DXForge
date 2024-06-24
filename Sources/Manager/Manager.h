// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ����� : �c���~�m��
// ����� : 2024/06/06 18:00
// �T�v   : Manager���N���X�̃w�b�_�t�@�C�� , �Q�l�Fhttps://cflat-inc.hatenablog.com/entry/2014/03/04/214608
// �X�V�� 
// 2024/06/06 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <memory>
#include <System/Macro.h>

// ====== �N���X�̒�` ======
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
