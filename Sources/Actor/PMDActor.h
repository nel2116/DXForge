// [PMDActor.h]
// 作成日 : 2024/05/21 19:30
// 作成者 : 田中ミノル
// 概要   : PMDアクタークラスの定義
#pragma once
#include <DirectX/d3dx12.h>
#include <DirectXMath.h>
#include <wrl.h>

class Dx12Wrapper;
class PMDRenderer;
class PMDActor
{
	friend PMDRenderer;
private:	// 構造体定義
	// シェーダ側に投げられるマテリアルデータ
	struct MaterialForHlsl
	{
		DirectX::XMFLOAT3 diffuse;	// ディフューズ色
		float alpha;				// α値
		DirectX::XMFLOAT3 specular;	// スペキュラ色
		float specularity;			// スペキュラの強さ
		DirectX::XMFLOAT3 ambient;	// アンビエント色
	};
	// それ以外のマテリアルデータ
	struct additionalMaterial
	{
		string texPath;	// テクスチャパス
		int toonIdx;	// トゥーン番号
		bool edgeFlg;	// マテリアルごとの輪郭線フラグ
	};
	// まとめたもの
	struct Material
	{
		unsigned int indicesNum;	// インデックス数
		MaterialForHlsl material;	// シェーダ側に投げられるマテリアルデータ
		additionalMaterial additional;	// それ以外のマテリアルデータ
	};
	struct Transform
	{
		// 内部に持っているXMMATRIXメンバが16バイトアライメントであるため、
		// Transformをnewするときには16バイト境界に確保する
		void* operator new(size_t size);
		DirectX::XMMATRIX world;
	};

public:	// パブリック関数
	PMDActor(const char* path, PMDRenderer& renderer);
	~PMDActor();
	// クローンは頂点及びマテリアルは共通のバッファを見るようにする
	PMDActor* Clone();
	void Update();
	void Draw();


private:	// プライベート関数
	// 読み込んだマテリアルを元にマテリアルバッファを作成
	HRESULT CreateMaterialData();

	// マテリアル＆テクスチャのビューを作成
	HRESULT CreateMaterialAndTextureView();

	// 座標変換行列のビューを作成
	HRESULT CreateTransformView();

	// PMDファイルの読み込み
	HRESULT LoadPMDFile(const char* path);

private:
	PMDRenderer& m_renderer;
	Dx12Wrapper& m_dx12;
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	// 頂点関連
	ComPtr<ID3D12Resource> m_vb = nullptr;
	ComPtr<ID3D12Resource> m_ib = nullptr;
	D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
	D3D12_INDEX_BUFFER_VIEW m_ibView = {};

	ComPtr<ID3D12Resource> m_transformMat = nullptr;	// 座標変換行列(今はワールドのみ)
	ComPtr<ID3D12DescriptorHeap> m_transfromHeap = nullptr;	// 座標変換ヒープ

	Transform m_transform;
	Transform* m_mappedTransform = nullptr;
	ComPtr<ID3D12Resource> m_transformBuff = nullptr;	// 座標変換行列バッファ

	// マテリアル関連
	vector<Material> m_materials;
	ComPtr<ID3D12Resource> m_materialBuff = nullptr;	// マテリアルバッファ
	vector<ComPtr<ID3D12Resource>> m_textureResources;	// テクスチャバッファ
	vector<ComPtr<ID3D12Resource>> m_sphResources;
	vector<ComPtr<ID3D12Resource>> m_spaResources;
	vector<ComPtr<ID3D12Resource>> m_toonResources;

	ComPtr<ID3D12DescriptorHeap> m_materialHeap = nullptr;	// マテリアルヒープ(5個分)

	float _angle; // テスト用y軸回転
};
