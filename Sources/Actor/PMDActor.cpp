#include "stdafx.h"
#include "PMDActor.h"
#include <System/PMDRenderer.h>
#include <System/Dx12Wrapper.h>
#include <DirectX/d3dx12.h>
using namespace Microsoft::WRL;
using namespace DirectX;

namespace
{
	/// <summary>
	/// テクスチャのパスをセパレータ文字で分離する
	/// @param[in] path テクスチャのパス
	/// @param[in] sepitter セパレータ文字
	/// @return 分離前後の文字列ペア
	/// </summary>
	pair<string, string> SplitFileName(const string& path, const char sepitter = '*')
	{
		int idx = path.find(sepitter);
		pair<string, string> ret;
		ret.first = path.substr(0, idx);
		ret.second = path.substr(idx + 1, path.length() - idx - 1);
		return ret;
	}

	/// <summary>
	/// ファイル名から拡張子を取得する
	/// @param filePath : ファイルパス
	/// @return : ファイルの拡張子
	/// </summary>
	string GetExtension(const string& filePath)
	{
		int idx = filePath.rfind('.');
		return filePath.substr(idx + 1, filePath.length() - idx - 1);
	}

	/// <summary>
	/// モデルのパスとテクスチャのパスから合成パスを得る
	/// @param modelPath : アプリケーションから見たPMDモデルのパス
	/// @param texPath : PMDモデルから見たテクスチャのパス
	/// @return : アプリケーションから見たテクスチャのパス
	/// </summary>
	string GetTexturePathFromModelAndTexPath(const string& modelPath, const char* texPath)
	{
		// ファイルのフォルダ区切りは\と/のに種類が使用される可能性があり
		// ともかく末尾の\か/を得られればいいので、双方のrfindを取り、比較する
		// int型二代入しているのは見つからなかった場合はrfindがepos(-1→0xffffffff)を返すため
		int pathIndex1 = modelPath.rfind('/');
		int pathIndex2 = modelPath.rfind('\\');
		auto pathIndex = max(pathIndex1, pathIndex2);
		auto folderPath = modelPath.substr(0, pathIndex + 1);
		return folderPath + texPath;
	}
}

void* PMDActor::Transform::operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

PMDActor::PMDActor(const char* path, PMDRenderer& renderer)
	: m_renderer(renderer)
	, m_dx12(renderer.m_dx12)
	, _angle(0.0f)
{
	m_transform.world = XMMatrixIdentity();
	assert(SUCCEEDED(LoadPMDFile(path)));
	assert(SUCCEEDED(CreateTransformView()));
	assert(SUCCEEDED(CreateMaterialData()));
	assert(SUCCEEDED(CreateMaterialAndTextureView()));
}

PMDActor::~PMDActor()
{
}

PMDActor* PMDActor::Clone()
{
	return nullptr;
}

void PMDActor::Update()
{
	_angle += 0.03f;
	m_transform.world = XMMatrixRotationY(_angle);
}

void PMDActor::Draw()
{
	m_dx12.GetCommandList()->IASetVertexBuffers(0, 1, &m_vbView);
	m_dx12.GetCommandList()->IASetIndexBuffer(&m_ibView);

	ID3D12DescriptorHeap* transheaps[] = { m_transfromHeap.Get() };
	// トランスフォームヒープをセット
	m_dx12.GetCommandList()->SetDescriptorHeaps(1, transheaps);
	m_dx12.GetCommandList()->SetGraphicsRootDescriptorTable(1, m_transfromHeap->GetGPUDescriptorHandleForHeapStart());

	ID3D12DescriptorHeap* mdh[] = { m_materialHeap.Get() };
	// マテリアルヒープをセット
	m_dx12.GetCommandList()->SetDescriptorHeaps(1, mdh);

	auto materialH = m_materialHeap->GetGPUDescriptorHandleForHeapStart();
	unsigned int idxOffset = 0;

	auto cbvsrvIncSize = m_dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 5;
	for (auto& m : m_materials)
	{
		m_dx12.GetCommandList()->SetGraphicsRootDescriptorTable(2, materialH);
		m_dx12.GetCommandList()->DrawIndexedInstanced(m.indicesNum, 1, idxOffset, 0, 0);
		materialH.ptr += cbvsrvIncSize;
		idxOffset += m.indicesNum;
	}
}

HRESULT PMDActor::CreateMaterialData()
{
	// マテリアルバッファを作成
	auto materialBuffSize = sizeof(MaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(materialBuffSize * m_materials.size());
	auto hr = m_dx12.GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_materialBuff.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	// マップマテリアルにコピー
	char* mapMaterial = nullptr;
	hr = m_materialBuff->Map(0, nullptr, (void**)&mapMaterial);
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	for (auto& m : m_materials)
	{
		*((MaterialForHlsl*)mapMaterial) = m.material;	// コピー
		mapMaterial += materialBuffSize;				// ポインタを進める
	}
	m_materialBuff->Unmap(0, nullptr);

	return S_OK;
}

HRESULT PMDActor::CreateMaterialAndTextureView()
{
	D3D12_DESCRIPTOR_HEAP_DESC materialDescHeapDesc = {};
	materialDescHeapDesc.NumDescriptors = m_materials.size() * 5;	// マテリアル数分(定数1つ、テクスチャ3つ)
	materialDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	materialDescHeapDesc.NodeMask = 0;
	materialDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// デスクリプタヒープの種類
	auto hr = m_dx12.GetDevice()->CreateDescriptorHeap(&materialDescHeapDesc, IID_PPV_ARGS(m_materialHeap.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}
	auto materialBuffSize = sizeof(MaterialForHlsl);
	materialBuffSize = (materialBuffSize + 0xff) & ~0xff;
	D3D12_CONSTANT_BUFFER_VIEW_DESC matCBVDesc = {};
	matCBVDesc.BufferLocation = m_materialBuff->GetGPUVirtualAddress();
	matCBVDesc.SizeInBytes = materialBuffSize;

	// マテリアルのビューを作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;						// ミップマップ数
	CD3DX12_CPU_DESCRIPTOR_HANDLE matDescHeapH(m_materialHeap->GetCPUDescriptorHandleForHeapStart());
	auto incSize = m_dx12.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	for (int i = 0; i < m_materials.size(); ++i)
	{
		// マテリアル固定バッファビュー
		m_dx12.GetDevice()->CreateConstantBufferView(&matCBVDesc, matDescHeapH);
		matDescHeapH.ptr += incSize;
		matCBVDesc.BufferLocation += materialBuffSize;
		if (m_textureResources[i] == nullptr)
		{
			srvDesc.Format = m_renderer.m_whiteTex->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_renderer.m_whiteTex.Get(), &srvDesc, matDescHeapH);
		}
		else
		{
			srvDesc.Format = m_textureResources[i]->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_textureResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.Offset(incSize);

		if (m_sphResources[i] == nullptr)
		{
			srvDesc.Format = m_renderer.m_whiteTex->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_renderer.m_whiteTex.Get(), &srvDesc, matDescHeapH);
		}
		else
		{
			srvDesc.Format = m_sphResources[i]->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_sphResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;

		if (m_spaResources[i] == nullptr)
		{
			srvDesc.Format = m_renderer.m_whiteTex->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_renderer.m_blackTex.Get(), &srvDesc, matDescHeapH);
		}
		else
		{
			srvDesc.Format = m_spaResources[i]->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_spaResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;

		if (m_toonResources[i] == nullptr)
		{
			srvDesc.Format = m_renderer.m_whiteTex->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_renderer.m_gradTex.Get(), &srvDesc, matDescHeapH);
		}
		else
		{
			srvDesc.Format = m_toonResources[i]->GetDesc().Format;
			m_dx12.GetDevice()->CreateShaderResourceView(m_toonResources[i].Get(), &srvDesc, matDescHeapH);
		}
		matDescHeapH.ptr += incSize;
	}

	return S_OK;
}

HRESULT PMDActor::CreateTransformView()
{
	// GPUバッファ作成
	auto buffSize = sizeof(Transform);
	buffSize = (buffSize + 0xff) & ~0xff;
	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	auto hr = m_dx12.GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_transformBuff.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	// マップとコピー
	hr = m_transformBuff->Map(0, nullptr, (void**)&m_mappedTransform);
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	*m_mappedTransform = m_transform;

	// ビューの作成
	D3D12_DESCRIPTOR_HEAP_DESC transformHeapDesc = {};
	transformHeapDesc.NumDescriptors = 1;	// 
	transformHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformHeapDesc.NodeMask = 0;
	transformHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	// デスクリプタヒープの種類
	hr = m_dx12.GetDevice()->CreateDescriptorHeap(&transformHeapDesc, IID_PPV_ARGS(m_transfromHeap.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return hr;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = m_transformBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = buffSize;
	m_dx12.GetDevice()->CreateConstantBufferView(&cbvDesc, m_transfromHeap->GetCPUDescriptorHandleForHeapStart());

	return S_OK;
}

HRESULT PMDActor::LoadPMDFile(const char* path)
{
	// PMDヘッダ構造体
	struct PMDHeader
	{
		float version;			// バージョン (例:00 00 80 3F == 1.00)
		char model_name[20];	// モデル名
		char comment[256];		// モデルコメント
	};
	char signature[3];
	PMDHeader pmdheader = {};

	string strModelPath = path;

	auto fp = fopen(strModelPath.c_str(), "rb");
	if (fp == nullptr)
	{	// エラー処理
		assert(0);
		return ERROR_FILE_NOT_FOUND;
	}

	// シグネチャの読み込み
	fread(signature, sizeof(signature), 1, fp);
	// ヘッダーの読み込み
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	unsigned int vertNum = 0;	// 頂点数
	fread(&vertNum, sizeof(vertNum), 1, fp);

#pragma pack(1)	// ここから1バイトパッキング・・・アライメント発生しない
	// PMDマテリアル構造体
	struct PMDMaterial
	{
		DirectX::XMFLOAT3 diffuse;	// ディフューズ色
		float alpha;				// α値
		float specularity;			// スペキュラの強さ(乗算値)
		DirectX::XMFLOAT3 specular;	// スペキュラ色
		DirectX::XMFLOAT3 ambient;	// アンビエント色
		unsigned char toonIdx;		// トゥーン番号
		unsigned char edgeFlg;		// マテリアルごとの輪郭フラグ
		unsigned int indicesNum;	// 面頂点数
		char texFilePath[20];		// テクスチャファイル名
	};
#pragma pack()	// ・・・1バイトパッキング解除

	constexpr unsigned int pmdvertSize = 38;	// PMD頂点構造体のサイズ
	vector<unsigned char> vertices(vertNum * pmdvertSize);	// 頂点データ
	fread(vertices.data(), vertices.size(), 1, fp);			// 一気に読み込み

	unsigned int indicesNum = 0;	// インデックス数
	fread(&indicesNum, sizeof(indicesNum), 1, fp);

	D3D12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(vertices[0]));

	// UPLOAD(確保は可能)
	auto hr = m_dx12.GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_vb.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr))
	{	// メッセージボックス
		MessageBox(nullptr, "頂点バッファの生成に失敗しました", "PMDファイル読み込みエラー", MB_OK);
		return hr;
	}

	unsigned char* vertMap = nullptr;
	hr = m_vb->Map(0, nullptr, (void**)&vertMap);
	// エラーチェック
	if (FAILED(hr))
	{	// メッセージボックス
		MessageBox(nullptr, "頂点バッファのマップに失敗しました", "PMDファイル読み込みエラー", MB_OK);
		return hr;
	}
	copy(vertices.begin(), vertices.end(), vertMap);
	m_vb->Unmap(0, nullptr);

	m_vbView.BufferLocation = m_vb->GetGPUVirtualAddress();	// バッファの仮想アドレス
	m_vbView.SizeInBytes = vertices.size();					// 全バイト数
	m_vbView.StrideInBytes = pmdvertSize;					// 1頂点あたりのバイト数

	vector<unsigned short> indices(indicesNum);
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);	// 一気に読み込み

	D3D12_RESOURCE_DESC resDescBuff = CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0]));

	// 設定は、バッファのサイズ以外は頂点バッファの設定を使い回す
	hr = m_dx12.GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDescBuff,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_ib.ReleaseAndGetAddressOf()));
	// エラーチェック
	if (FAILED(hr))
	{	// メッセージボックス
		MessageBox(nullptr, "インデックスバッファの生成に失敗しました", "PMDファイル読み込みエラー", MB_OK);
		return hr;
	}

	// 作ったバッファにインデックスデータをコピー
	unsigned short* idxMap = nullptr;
	hr = m_ib->Map(0, nullptr, (void**)&idxMap);
	// エラーチェック
	if (FAILED(hr))
	{	// メッセージボックス
		MessageBox(nullptr, "インデックスバッファのマップに失敗しました", "PMDファイル読み込みエラー", MB_OK);
		return hr;
	}
	copy(indices.begin(), indices.end(), idxMap);
	m_ib->Unmap(0, nullptr);

	// インデックスバッファビューの設定
	m_ibView.BufferLocation = m_ib->GetGPUVirtualAddress();	// バッファの仮想アドレス
	m_ibView.Format = DXGI_FORMAT_R16_UINT;					// インデックスのフォーマット
	m_ibView.SizeInBytes = indices.size() * sizeof(indices[0]);	// 全バイト数

	unsigned int materialNum = 0;	// マテリアル数
	fread(&materialNum, sizeof(materialNum), 1, fp);
	m_materials.resize(materialNum);
	m_textureResources.resize(materialNum);
	m_sphResources.resize(materialNum);
	m_spaResources.resize(materialNum);
	m_toonResources.resize(materialNum);

	vector<PMDMaterial> pmdMaterials(materialNum);
	fread(pmdMaterials.data(), pmdMaterials.size() * sizeof(PMDMaterial), 1, fp);
	// コピー
	for (int i = 0; i < pmdMaterials.size(); ++i)
	{
		m_materials[i].indicesNum = pmdMaterials[i].indicesNum;
		m_materials[i].material.diffuse = pmdMaterials[i].diffuse;
		m_materials[i].material.alpha = pmdMaterials[i].alpha;
		m_materials[i].material.specular = pmdMaterials[i].specular;
		m_materials[i].material.specularity = pmdMaterials[i].specularity;
		m_materials[i].material.ambient = pmdMaterials[i].ambient;
		m_materials[i].additional.toonIdx = pmdMaterials[i].toonIdx;
	}

	for (int i = 0; i < pmdMaterials.size(); ++i)
	{
		// トゥーンリソースの読み込み
		char toonFilePath[32];
		sprintf(toonFilePath, "toon/toon%02d.bmp", pmdMaterials[i].toonIdx + 1);
		m_toonResources[i] = m_dx12.GetTextureByPath(toonFilePath);

		if (strlen(pmdMaterials[i].texFilePath) == 0)
		{
			ID3D12Resource* tmp = nullptr;
			m_textureResources[i].Swap(tmp);
			SAFE_RELEASE(tmp);
			continue;
		}

		string texFileName = pmdMaterials[i].texFilePath;
		string sphFileName = "";
		string spaFileName = "";
		if (count(texFileName.begin(), texFileName.end(), '*') > 0)
		{	// スプリッタがある
			auto namepair = SplitFileName(texFileName);
			if (GetExtension(namepair.first) == "sph")
			{
				texFileName = namepair.second;
				sphFileName = namepair.first;
			}
			else if (GetExtension(namepair.first) == "spa")
			{
				texFileName = namepair.second;
				spaFileName = namepair.first;
			}
			else
			{
				texFileName = namepair.first;
				if (GetExtension(namepair.second) == "sph")
				{
					sphFileName = namepair.second;
				}
				else if (GetExtension(namepair.second) == "spa")
				{
					spaFileName = namepair.second;
				}
			}
		}
		else
		{
			if (GetExtension(pmdMaterials[i].texFilePath) == "sph")
			{
				sphFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
			else if (GetExtension(pmdMaterials[i].texFilePath) == "spa")
			{
				spaFileName = pmdMaterials[i].texFilePath;
				texFileName = "";
			}
			else
			{
				texFileName = pmdMaterials[i].texFilePath;
			}
		}
		// モデルとテクスチャパスからアプリケーションからのテクスチャパスを得る
		if (texFileName != "")
		{
			auto texFilePath = GetTexturePathFromModelAndTexPath(strModelPath, texFileName.c_str());
			m_textureResources[i] = m_dx12.GetTextureByPath(texFilePath.c_str());
		}
		if (sphFileName != "")
		{
			auto sphFilePath = GetTexturePathFromModelAndTexPath(strModelPath, sphFileName.c_str());
			m_sphResources[i] = m_dx12.GetTextureByPath(sphFilePath.c_str());
		}
		if (spaFileName != "")
		{
			auto spaFilePath = GetTexturePathFromModelAndTexPath(strModelPath, spaFileName.c_str());
			m_spaResources[i] = m_dx12.GetTextureByPath(spaFilePath.c_str());
		}
	}
	fclose(fp);
}
