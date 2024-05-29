// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Shader.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/26 18:00
// �T�v   : �V�F�[�_�[�N���X�̒�`
// �X�V���� : 2024/05/25 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Pipeline/Pipeline.h"
#include "RootSignature/RootSignature.h"

struct RenderingSetting
{
	std::vector<InputLayout> inputLayouts;
	std::vector<DXGI_FORMAT> Formats;
	CullMode cullMode = CullMode::Back;
	BlendMode blendMode = BlendMode::Alpha;
	PrimitiveTopology primitiveTopology = PrimitiveTopology::Triangle;
	bool isDepth = true;
	bool isDepthMask = true;
	int RTVCount = 1;
	bool isWireFrame = false;
};

class Mesh;
class Shader
{
public:		// �p�u���b�N�֐�
	/// <summary>
	/// �쐬
	/// </summary>
	/// <param name="pDev">�����_�[�^�[�Q�b�g�N���X�̃|�C���^</param>
	/// <param name="filePath">�t�@�C���p�X</param>
	/// <param name="setting">�����_�����O�ݒ�</param>
	/// <param name="rangeType">�����W�^�C�v</param>
	void Create(Renderer* pDev, std::string& filePath, RenderingSetting& setting, std::vector<RangeType> rangeTypes);

	/// <summary>
	/// �`��J�n
	/// </summary>	
	/// <param name="w">����</param>
	/// <param name="h">�c��</param>
	void Begin(int w, int h);

	/// <summary>
	/// ���b�V���̕`��
	/// </summary>
	/// <param name="mesh">���b�V��</param>
	void DrawMesh(const Mesh& mesh);

	/// <summary>
	/// CBV�J�E���g�擾
	/// </summary>
	/// <returns>CBV�J�E���g</returns>
	UINT GetCBVCount() const { return m_cbvCount; }

private:		// �v���C�x�[�g�֐�
	/// <summary>
	/// �V�F�[�_�[�t�@�C���̃��[�h
	/// </summary>
	/// <param name="filePath">�t�@�C���p�X</param>
	void LoadShader(const std::string& filePath);

private:		// �����o�ϐ�
	Renderer* m_pRenderer = nullptr;

	// �p�C�v���C��
	std::unique_ptr<Pipeline> m_upPipeline = nullptr;
	std::unique_ptr<RootSignature> m_upRootSignature = nullptr;

	// �V�F�[�_�[
	ID3DBlob* m_pVSBlob = nullptr;	// ���_�V�F�[�_�[�o�b�t�@
	ID3DBlob* m_pHSBlob = nullptr;	// �n���V�F�[�_�[�o�b�t�@
	ID3DBlob* m_pDSBlob = nullptr;	// �h���C���V�F�[�_�[�o�b�t�@
	ID3DBlob* m_pGSBlob = nullptr;	// �W�I���g���V�F�[�_�[�o�b�t�@
	ID3DBlob* m_pPSBlob = nullptr;	// �s�N�Z���V�F�[�_�[�o�b�t�@

	UINT m_cbvCount = 0;	// CBV�J�E���g
};