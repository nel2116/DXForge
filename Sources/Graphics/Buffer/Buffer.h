// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Texture.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/30 09:30
// �T�v   : �o�b�t�@�[�N���X�̒�`
// �X�V����
// 2024/05/30 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Graphics\Renderer.h>


// ====== �N���X��` ======
class Buffer
{
public:		// �p�u���b�N�֐�
	Buffer() = default;
	~Buffer() = default;

protected:
	Renderer* m_pRenderer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pBuffer;
};