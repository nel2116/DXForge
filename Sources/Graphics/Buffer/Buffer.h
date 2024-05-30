// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Texture.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/30 09:30
// 概要   : バッファークラスの定義
// 更新履歴
// 2024/05/30 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics\Renderer.h>


// ====== クラス定義 ======
class Buffer
{
public:		// パブリック関数
	Buffer() = default;
	~Buffer() = default;

protected:
	Renderer* m_pRenderer = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_pBuffer;
};