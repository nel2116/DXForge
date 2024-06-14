//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Mesh.cpp]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/25 19:00
// �T�v   : ���b�V���N���X�̒�`
// �X�V����
// 2024/06/11 �쐬
// 2024/06/13 �Ď���
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <string>
#include <vector>


// ====== �\���̒�` ======
struct ResMaterial
{
	DirectX::XMFLOAT3   Diffuse;        // �g�U���ː���
	DirectX::XMFLOAT3   Specular;       // ���ʔ��ː���
	float               Alpha;          // ���ߐ���
	float               Shininess;      // ���ʔ��ˋ��x
	std::wstring        DiffuseMap;     // �f�B�t���[�Y�}�b�v�t�@�C���p�X
	std::wstring        SpecularMap;    // �X�y�L�����[�}�b�v�t�@�C���p�X
	std::wstring        ShininessMap;   // �V���C�l�X�}�b�v�t�@�C���p�X
	std::wstring        NormalMap;      // �@���}�b�v�t�@�C���p�X
};

struct MeshVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 texcoord;
	DirectX::XMFLOAT3 tangent;
	MeshVertex() = default;
	MeshVertex(
		DirectX::XMFLOAT3 const& Pos,
		DirectX::XMFLOAT3 const& Normal,
		DirectX::XMFLOAT2 const& Texcoord,
		DirectX::XMFLOAT3 const& Tangent)
		: pos(Pos), normal(Normal), texcoord(Texcoord), tangent(Tangent) {}
	static const D3D12_INPUT_LAYOUT_DESC InputLayout;

private:
	static const int InputElementCount = 4;
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputElementCount];

};

struct ResMesh
{
	std::vector<MeshVertex> vertices;	// ���_�f�[�^
	std::vector<uint32_t> indices;		// �C���f�b�N�X�f�[�^
	uint32_t materialIdx;				// �}�e���A���ԍ�
};

/// <summary>
/// ���b�V�������[�h����
/// </summary>
/// <param name="fileName">�t�@�C����</param>
/// <param name="meshes">���b�V���̊i�[��</param>
/// <param name="materials">�}�e���A���̊i�[��</param>
/// <returns>���[�h�ɐ���������true</returns>
bool LoadMesh(const wchar_t* fileName, std::vector<ResMesh>& meshes, std::vector<ResMaterial>& materials);