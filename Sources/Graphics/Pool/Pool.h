// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/13 16:30
// �쐬�� : �c���~�m��
// �T�v   : �v�[���N���X�̒�`
// �X�V����
// 2024/06/13 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <cstdint>
#include <mutex>
#include <cassert>
#include <functional>

// ====== �N���X��` ======
template <typename T>
class Pool
{
public:	// �\���̒�`
	struct Item
	{
		T m_Value;
		uint32_t m_Index = 0;	// �C���f�b�N�X
		Item* m_pNext = nullptr;	// ���̃A�C�e��
		Item* m_pPrev = nullptr;	// �O�̃A�C�e��

		Item()
			: m_Value()
			, m_Index(0)
			, m_pNext(nullptr)
			, m_pPrev(nullptr)
		{}

		~Item() {}
	};
public:		// �p�u���b�N�֐�
	Pool()
		: m_pBuffer(nullptr)
		, m_pActive(nullptr)
		, m_pFree(nullptr)
		, m_Capacity(0)
		, m_Count(0)
	{}
	~Pool() { Uninit(); }

	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="count">�m�ۂ���A�C�e����
	/// <returns>�������ɐ���������true</returns>
	bool Init(uint32_t count)
	{
		std::lock_guard<std::mutex>gurand(m_Mutex);

		m_pBuffer = static_cast<uint32_t*>(malloc(sizeof(Item) * (count + 2)));
		// �������m�ۂɎ��s
		if (!m_pBuffer) { return false; }

		m_Capacity = count;

		// �C���f�b�N�X��U��
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
	/// �I������
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
	/// �A�C�e�����m�ۂ���
	/// </summary>
	/// <param name="func">���[�U�[�ɂ�鏉��������</param>
	/// <returns>�A�C�e���̃|�C���^�A�m�ۂɎ��s�����ꍇ��nullptr</returns>
	T* Alloc(std::function<void(uint32_t, T*)> func = nullptr)
	{
		std::lock_guard<std::mutex>gurand(m_Mutex);

		// �󂫃A�C�e�����擾
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

		// ���������蓖��
		auto val = NEW((void*)item)T();

		// ���[�U�[����������
		if (func)
		{
			func(item->m_Index, val);
		}
		return val;
	}

	/// <summary>
	/// �A�C�e�����������
	/// </summary>
	/// <param name="pValue">�������A�C�e���̃|�C���^</param>
	void Free(T* pValue)
	{
		// �|�C���^��nullptr
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
	/// ���A�C�e�������擾����
	/// </summary>
	/// <returns>���A�C�e����</returns>
	uint32_t GetSize() const
	{
		return m_Capacity;
	}

	/// <summary>
	///	�g�p���̃A�C�e�������擾����
	/// </summary>
	///	<returns>�g�p���̃A�C�e����</returns>
	uint32_t GetUsedCount() const
	{
		return m_Count;
	}

	/// <summary>
	/// �g�p�\�ȃA�C�e�������擾����
	/// </summary>
	/// <returns>�g�p�\�ȃA�C�e����</returns>
	uint32_t GetAvailableCount() const
	{
		return m_Capacity - m_Count;
	}

private:	// �v���C�x�[�g�֐�
	/// <summary>
	/// �A�C�e�����擾����
	/// </summary>
	/// <param name="index">�擾����A�C�e���̃C���f�b�N�X</param>
	/// <returns>�A�C�e���̃|�C���^</returns>
	Item* GetItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		return reinterpret_cast<Item*>(m_pBuffer + sizeof(Item) * index);
	}

	/// <summary>
	/// �A�C�e���Ƀ����������蓖�Ă�
	/// </summary>
	/// <param name="index">�擾����A�C�e���̃C���f�b�N�X</param>
	/// <returns>�A�C�e���ւ̃|�C���^
	Item* AllocItem(uint32_t index)
	{
		assert(0 <= index && index <= m_Capacity + 2);
		auto buf = (m_pBuffer + sizeof(Item) * index);
		return NEW(buf)Item();
	}
	Pool(const Pool&) = delete;
	Pool& operator=(const Pool&) = delete;

private:
	uint8_t* m_pBuffer = nullptr;	// �o�b�t�@
	Item* m_pActive = nullptr;	// �A�N�e�B�u�ȃA�C�e��

	Item* m_pFree = nullptr;	// �A�C�e���̐擪
	uint32_t m_Capacity = 0;	// ���A�C�e����
	uint32_t m_Count = 0;	// ���݂̃A�C�e����
	std::mutex m_Mutex;	// �~���[�e�b�N�X
};