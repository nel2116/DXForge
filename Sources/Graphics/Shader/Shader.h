// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Shader.h]
// ����� : �c���~�m��
// �쐬�� : 2024/07/02
// �T�v   : �V�F�[�_�̒�`
// �X�V��
// 2024/07/02 : �c���~�m�� : �V�K�쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <System/ComPtr.h>

// ====== �񋓑� ======
enum class ShaderType
{
	Vertex,
	Pixel,
	Geometry,
	Hull,
	Domain,
	Compute
};

// ====== �N���X�̒�` ======
class Shader
{
public:
	// �R���X�g���N�^
	Shader() = default;
	// �f�X�g���N�^
	~Shader() = default;

	// �V�F�[�_�̓ǂݍ���
	void Load(const std::string& path, ShaderType type);

	// �V�F�[�_�̃o�b�t�@�T�C�Y�̎擾
	size_t GetBufferSize() { return m_BufferSize; }

	// �V�F�[�_�̃o�b�t�@�|�C���^�̎擾
	void* GetBuffer() { return m_pBuffer; }

private:
	// �����o�ϐ�
	ShaderType m_Type;			// �V�F�[�_�̎��
	ComPtr<ID3DBlob> m_pShader;	// �V�F�[�_
	size_t m_BufferSize;		// �V�F�[�_�̃o�b�t�@�T�C�Y
	void* m_pBuffer;			// �V�F�[�_�̃o�b�t�@
};