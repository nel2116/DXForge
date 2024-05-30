// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [DepthStencil.h]
// �쐬�� : �c���~�m��
// �쐬�� : 2024/05/30 09:30
// �T�v   : �[�x�X�e���V���N���X�̒�`
// �X�V����
// 2024/05/30 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include "../Buffer.h"

enum class DepthStencilFormat
{
	DepthQuality = DXGI_FORMAT_R16_TYPELESS,
	DepthHighQuality = DXGI_FORMAT_R32_TYPELESS,
	D3pthHighQualityAndStencil = DXGI_FORMAT_R24G8_TYPELESS,
};

class DepthStencil : public Buffer
{
public:		// �p�u���b�N�֐�
	/// <summary>
	/// �[�x�e�N�X�`������
	/// </summary>
	/// <param name="pDev">�����_���[�N���X�̃|�C���^</param>
	/// <param name="resolution">�𑜓x</param>
	/// <param name="format">�[�x�e�N�X�`���̃t�H�[�}�b�g</param>
	bool Create(Renderer* pDev, const DirectX::XMFLOAT2& resolution, DepthStencilFormat format = DepthStencilFormat::DepthHighQuality);

	/// <summary>
	/// �[�x�o�b�t�@�ɏ������܂�Ă���f�[�^������������֐�
	/// </summary>
	void ClearBuffer();

	/// <summary>
	/// DSV�ԍ����擾
	/// </summary>
	/// <returns>DSV�ԍ�</returns>
	UINT GetDSVNumber() const { return m_dsvNumber; }

private:	// �����o�ϐ�
	UINT m_dsvNumber = 0;	// DSV�ԍ�

};
