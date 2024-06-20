// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/10
// 作成者 : 田中ミノル
// 概要   : テクスチャクラスの定義
// 更新履歴 : 2024/06/10 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <System/ComPtr.h>

// ====== クラス定義 ======
class DescriptorHandle;
class DescriptorPool;
class Texture
{
public:	// パブリック関数
	Texture();	// コンストラクタ
	~Texture();	// デストラクタ
	bool Init(DescriptorPool* pPool, const wchar_t* filename, bool isSRGB);
	bool Init(DescriptorPool* pPool, const D3D12_RESOURCE_DESC* pDesc, bool isSRGB, bool isCube);	// 初期化
	void Uninit();	// 終了処理

	D3D12_CPU_DESCRIPTOR_HANDLE GetHandleCPU() const;	// CPUハンドルの取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetHandleGPU() const;	// GPUハンドルの取得

private:	// プライベート関数
	D3D12_SHADER_RESOURCE_VIEW_DESC GetViewDesc(bool isCube);
	bool CreateTexture(const wchar_t* filename, bool isSRGB);

private:	// メンバ変数
	ComPtr<ID3D12Resource> m_pTex;
	DescriptorHandle* m_pHandle;
	DescriptorPool* m_pPool;

private:
	Texture(const Texture&) = delete;
	void operator=(const Texture&) = delete;
};