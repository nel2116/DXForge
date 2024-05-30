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
#include <vector>

// ====== �N���X��` ======
struct Vertex
{
	Vertex(DirectX::XMFLOAT3 Pos, DirectX::XMFLOAT2 UV) : pos(Pos), uv(UV) {}
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

class Texture : public Buffer
{
public:		// �p�u���b�N�֐�
	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����������true</returns>
	static bool Init();

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
	void Set(int index) const;

	/// <summary>
	/// SRV�ԍ����擾
	/// </summary>
	/// <returns>SRV�ԍ�</returns>
	const int GetSRVNumber() const { return m_srvNumber; }

	/// <summary>
	/// �`��
	/// </summary>
	void Draw() const;

private:
	int m_srvNumber = 0;	// SRV�ԍ�

	static Microsoft::WRL::ComPtr<ID3D12Resource> m_pVBuffer;	// �o�b�t�@
	static Microsoft::WRL::ComPtr<ID3D12Resource> m_pIBuffer;	// �o�b�t�@
	static D3D12_VERTEX_BUFFER_VIEW m_vbView;					// ���_�o�b�t�@�r���[
	static D3D12_INDEX_BUFFER_VIEW m_ibView;					// �C���f�b�N�X�o�b�t�@�r���[
	static std::vector<Vertex> m_vertex;						// ���_���W
	static std::vector<UINT> m_index;							// �C���f�b�N�X	
};