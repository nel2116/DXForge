// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [ShaderManager.h]
// ����� : �c���~�m��
// �쐬�� : 2024/07/02
// �T�v   : �V�F�[�_�̊Ǘ��N���X
// �X�V��
// 2024/07/02 : �c���~�m�� : �V�K�쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "Manager.h"

// ====== �N���X�̒�` ======
class ShaderManager : public Manager<ShaderManager>
{
public:	// �񋓑́E�\���̒�`
	/// <summary>
	/// �V�F�[�_�̎��
	/// </summary>
	enum class ShaderType
	{
		Vertex,		// ���_�V�F�[�_
		Pixel,		// �s�N�Z���V�F�[�_
		Geometry,	// �W�I���g���V�F�[�_
		Hull,		// �n���V�F�[�_
		Domain,		// �h���C���V�F�[�_
		Compute,	// �R���s���[�g�V�F�[�_
		Max			// �ő吔
	};

	/// <summary>
	/// �V�F�[�_�̏��
	/// </summary>
	/// <param name="type">�V�F�[�_�̎��</param>
	/// <param name="blob">�V�F�[�_�̃o�b�t�@</param>
	struct ShaderInfo
	{
		ShaderType type;		// �V�F�[�_�̎��
		ComPtr<ID3DBlob> blob;	// �V�F�[�_�̃o�b�t�@
	};

public:		//	�p�u���b�N�֐�

	/// <summary>
	/// �V�F�[�_��ǂݍ���Ń}�b�v�ɓo�^����֐�
	/// </summary>
	/// <param name="path">�t�@�C���p�X</param>
	/// <param name="name">�V�F�[�_�̖��O</param>
	/// <param name="type">�V�F�[�_�̎��</param>
	/// <returns>�����������ǂ���</returns>
	bool LoadShader(const string& path, const string& name, const ShaderType& type);

	/// <summary>
	/// �V�F�[�_���擾����֐�
	/// </summary>
	/// <param name="name">�V�F�[�_�̖��O</param>
	/// <returns>�V�F�[�_�̃o�b�t�@</returns>
	/// <returns>���s������nullptr</returns>
	ID3DBlob* GetShader(const string& name);

	/// <summary>
	/// �������֐�
	/// </summary>
	/// <returns>�����������ǂ���</returns>
	bool Init();

	/// <summary>
	/// ����֐�
	/// </summary>
	/// <returns>�����������ǂ���</returns>
	bool Uninit();

private:	// �����o�ϐ�
	unordered_map<string, ShaderInfo> m_shaderMap;	// �V�F�[�_�̃}�b�v

private:	// �V���O���g���֘A
	friend class Manager<ShaderManager>;
	ShaderManager() = default;
};
// ShaderManager�N���X�̃C���X�^���X���擾����
#define SHADER_MANAGER ShaderManager::manager()