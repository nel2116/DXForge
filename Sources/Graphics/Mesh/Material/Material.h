// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Material.h]
// 作成者 : 田中ミノル
// 作成日 : 2024/06/14 12:05 : 作成
// 概要   : マテリアルクラスの定義
// 更新履歴
// 2024/06/14  作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
#pragma once
// ====== インクルード部 ======
#include <Graphics/DescriptorPool/DescriptorPool.h>
#include <Graphics/Texture/Texture.h>
#include <Graphics/Buffer/ConstantBuffer/ConstantBuffer.h>
#include <map>

// ====== クラスの定義 ======
class Material
{
public:
	// ====== 列挙体 ======
	enum TEXTURE_USAGE
	{
		TEXTURE_USAGE_DIFFUSE = 0,  // ディフューズマップ
		TEXTURE_USAGE_SPECULAR,     // スペキュラーマップ
		TEXTURE_USAGE_SHININESS,    // シャイネスマップ
		TEXTURE_USAGE_NORMAL,       // 法線マップ

		TEXTURE_USAGE_BASE_COLOR,   // ベースカラーマップ
		TEXTURE_USAGE_METALLIC,     // メタリックマップ
		TEXTURE_USAGE_ROUGHNESS,    // ラフネスマップ

		TEXTURE_USAGE_COUNT
	};

public:	// ====== メンバ関数 ======
	// コンストラクタ
	Material();

	// デストラクタ
	~Material();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="pPool">ディスクリプタプール</param>
	/// <param name="bufferSize">1マテリアルあたりの定数バッファのサイズ</param>
	/// <param name="count">マテリアル数</param>
	/// <returns>初期化に成功したらtrue</returns>
	bool Init(DescriptorPool* pPool, size_t bufferSize, size_t count);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Uninit();

	/// <summary>
	/// テクスチャを設定します
	/// </summary>
	/// <param name="index">マテリアル番号</param>
	/// <param name="usage">テクスチャの使用用途</param>
	/// <param name="path">テクスチャパス</param>
	/// <returns>設定に成功したらtrue</returns>
	bool SetTexture(size_t index, TEXTURE_USAGE usage, const std::wstring& path);

	/// <summary>
	/// 定数バッファのポインタを取得
	/// </summary>
	/// <param name="index">マテリアル番号</param>
	/// <returns>指定された番号に一致する定数バッファのポインタ,一致するものが無い場合は nullptr</returns>
	void* GetBufferPtr(size_t index) const;

	/// <summary>
	/// 定数バッファのポインタを取得
	/// </summary>
	/// <param name="index">マテリアル番号</param>
	/// <returns>指定された番号に一致する定数バッファのポインタ,一致するものが無い場合は nullptr</returns>
	template<typename T>
	T* GetBufferPtr(size_t index) const
	{
		return reinterpret_cast<T*>(GetBufferPtr(index));
	}

	/// <summary>
	/// 定数バッファのGPU仮想アドレスを取得
	/// </summary>
	/// <param name="index">マテリアル番号</param>
	/// <returns>指定された番号に一致する定数バッファのGPU仮想アドレス</returns>
	D3D12_GPU_VIRTUAL_ADDRESS GetBufferAddress(size_t index) const;

	/// <summary>
	/// テクスチャハンドルを取得
	/// </summary>
	/// <param name="index">マテリアル番号</param>
	/// <param name="usage">テクスチャの使用用途</param>
	/// <returns>指定された番号に一致するテクスチャのディスクリプタハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(size_t index, TEXTURE_USAGE usage) const;

	/// <summary>
	/// マテリアル数を取得
	/// </summary>
	/// <returns>マテリアル数</returns>
	size_t GetCount() const;

private:	// 構造体定義
	struct Subset
	{
		ConstantBuffer* pCostantBuffer; // 定数バッファ
		D3D12_GPU_DESCRIPTOR_HANDLE TextureHandle[TEXTURE_USAGE_COUNT]; // テクスチャハンドル
	};

private:	// メンバ変数
	std::map<std::wstring, Texture*> m_pTexture;     // テクスチャ
	std::vector<Subset> m_Subset;       // サブセット
	ID3D12Device* m_pDevice; // デバイス
	DescriptorPool* m_pPool; // ディスクリプタプール

private:	// コピー禁止
	Material(const Material&) = delete;
	void operator = (const Material&) = delete;
};

// ====== インライン関数 ======
constexpr auto TU_DIFFUSE = Material::TEXTURE_USAGE_DIFFUSE;
constexpr auto TU_SPECULAR = Material::TEXTURE_USAGE_SPECULAR;
constexpr auto TU_SHININESS = Material::TEXTURE_USAGE_SHININESS;
constexpr auto TU_NORMAL = Material::TEXTURE_USAGE_NORMAL;

constexpr auto TU_BASE_COLOR = Material::TEXTURE_USAGE_BASE_COLOR;
constexpr auto TU_METALLIC = Material::TEXTURE_USAGE_METALLIC;
constexpr auto TU_ROUGHNESS = Material::TEXTURE_USAGE_ROUGHNESS;
