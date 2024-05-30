// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Texture.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/29 12:00
// �T�v   : �e�N�X�`���N���X�̒�`
// �X�V����
// 2024/05/29 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "../Buffer.h"

// ====== �N���X��` ======
class Texture : public Buffer
{
public:
	/// <summary>
	/// �e�N�X�`���̃��[�h
	/// </summary>
	/// <param name="pDev">�����_���[�N���X�̃|�C���^</param>
	/// <param name="filePath">�t�@�C���p�X</param>
	/// <returns>����������true</returns>
	bool Load(Renderer* pDev, const string& filePath);

	/// <summary>
	/// �V�F�[�_�[���\�[�X�Ƃ��ăZ�b�g
	/// </summary>
	/// <param name="index">�C���f�b�N�X</param>
	void Set(int index);

	/// <summary>
	/// SRV�ԍ����擾
	/// </summary>
	/// <returns>SRV�ԍ�</returns>
	int GetSRVNumber() { return m_srvNumber; }

private:
	int m_srvNumber = 0;
};