// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : ���b�V���N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <Graphics/Renderer.h>

// ====== ���O��� ======
using namespace DirectX;

// ====== �\���̒�` ======
struct Vertex
{
	Vertex(XMFLOAT3 position, XMFLOAT2 uv) :pos(position), UV(uv) {};
	XMFLOAT3 pos;
	XMFLOAT2 UV;
};

// ====== �N���X��` ======
class Mesh
{
private:	// �^��`
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// �쐬
	/// </summary>
	/// <param name="pDev">�����_���[�N���X�̃|�C���^</param>
	void Create(Renderer* pDev);

	/// <summary>
	/// �C���X�^���X�`��
	/// </summary>
	void DrawInstanced() const;

private:
	Renderer* m_pRenderer = nullptr;
	ComPtr<ID3D12Resource> m_pVertexBuff = nullptr;
	ComPtr<ID3D12Resource> m_pIndexBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
	D3D12_INDEX_BUFFER_VIEW m_ibView = {};
	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;
};