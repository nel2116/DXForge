// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// �쐬�� : 2024/06/07
// �쐬�� : �c���~�m��
// �T�v   : �|���S���N���X�̒�`
// �X�V���� : 2024/06/07 �쐬
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== �C���N���[�h�� ======
#include <wrl/client.h>

// ====== �^��` ======
template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

// ====== �N���X��` ======
class Polygon
{
public:	// �p�u���b�N�֐�
	bool Init();	// ������
	void Draw();	// �`��

private:	// �v���C�x�[�g�֐�

private:	// �����o�ϐ�
	ComPtr<ID3D12DescriptorHeap> m_pHeapCBV;		// CBV�q�[�v
	ComPtr<ID3D12Resource> m_pVB;					// ���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW m_vbView;				// ���_�o�b�t�@�r���[
	ComPtr<ID3D12Resource> m_pCB[2];				//
	ConstantBufferView<Transform> m_CBV[2];			// �萔�o�b�t�@�r���[
	ComPtr<ID3D12RootSignature> m_pRootSignature;	// ���[�g�V�O�l�`��
	ComPtr<ID3D12PipelineState> m_pPSO;				// �p�C�v���C���X�e�[�g
	float m_RotateAngle;							// ��]�p�x
};