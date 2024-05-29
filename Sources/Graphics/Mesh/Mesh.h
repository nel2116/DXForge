// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 17:00
// �T�v   : ���b�V���N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <string>
#include <memory>
#include <Graphics/Renderer.h>
#include <Graphics/Texture/Texture.h>
#include "MeshData/MeshData.h"

// ====== ���O��� ======
using namespace DirectX;

// ====== �\���̒�` ======
struct MeshFace
{
	UINT idx[3];
};

struct Material
{
	std::string name;									// �}�e���A���̖��O
	std::shared_ptr<Texture> spBaseColorTex;			// �e�N�X�`��
	XMFLOAT4 baseColor = { 1.0f,1.0f,1.0f,1.0f };		// �x�[�X�J���[
	std::shared_ptr<Texture> spMetallicRoughnessTex;	// R:���^���b�N G:���t�l�X
	float metallic = 0.0f;								// ���^���b�N
	float roughness = 0.0f;								// ���t�l�X
	std::shared_ptr<Texture> spEmissiveTex;				// ���Ȕ����e�N�X�`��
	XMFLOAT3 emissive = { 0.0f,0.0f,0.0f };				// ���Ȕ����̃X�P�[�����O�W��
	std::shared_ptr<Texture> spNormalTex;				// �@���e�N�X�`��
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
	/// <param name="vertices">���_���</param>
	/// <param name="faces">�ʏ��</param>
	/// <param name="materials">�}�e���A�����</param>
	void Create(Renderer* pDev, const std::vector<MeshVertex>& vertices, const std::vector<MeshFace>& faces, const Material& material);

	/// <summary>
	/// �C���X�^���X�`��
	/// </summary>
	/// <param name="vertexCount">���_��</param>
	void DrawInstanced(UINT vertexCount) const;

	/// <summary>
	/// �C���X�^���X�����擾
	/// </summary>
	/// <returns>�C���X�^���X��</returns>
	UINT GetInstanceCount() const { return m_instanceCount; }

	/// <summary>
	/// �}�e���A���̎擾
	/// </summary>
	/// <returns>�}�e���A�����</returns>
	const Material& GetMaterial() const { return m_material; }

private:
	Renderer* m_pRenderer = nullptr;
	ComPtr<ID3D12Resource> m_pVertexBuff = nullptr;
	ComPtr<ID3D12Resource> m_pIndexBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
	D3D12_INDEX_BUFFER_VIEW m_ibView = {};
	UINT m_instanceCount = 0;
	Material m_material = {};
};