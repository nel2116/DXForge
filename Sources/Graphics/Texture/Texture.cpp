// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// 作成日 : 2024/06/10
// 作成者 : 田中ミノル
// 概要   : テクスチャクラスの定義
// 更新履歴 : 2024/06/10 作成
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Texture.h"
#include <System/FileUtil.h>

bool Texture::Init()
{
	// 頂点バッファの生成
	{
		// 頂点データ
		Vertex vertices[] =
		{
			{DirectX::XMFLOAT3(-1.0f, 1.0f,0.0f),DirectX::XMFLOAT2(0.0f,0.0f)},
			{DirectX::XMFLOAT3(1.0f, 1.0f,0.0f),DirectX::XMFLOAT2(1.0f,0.0f)},
			{DirectX::XMFLOAT3(1.0f,-1.0f,0.0f),DirectX::XMFLOAT2(1.0f,1.0f)},
			{DirectX::XMFLOAT3(-1.0f,-1.0f,0.0f),DirectX::XMFLOAT2(0.0f,1.0f)},
		};

		// ヒーププロパティ
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;						// D3D12_HEAP_TYPE_UPLOAD : CPUからGPUへの転送が高速
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPUページプロパティを未指定
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : メモリプール未指定
		heapProp.CreationNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定
		heapProp.VisibleNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定

		// リソースの設定
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;		// D3D12_RESOURCE_DIMENSION_BUFFER : リソースがバッファであることを示す
		resDesc.Alignment = 0;										// 0 : リソースのアライメントを指定しない
		resDesc.Width = sizeof(vertices);							// 頂点データのサイズ
		resDesc.Height = 1;											// 1 : バッファの場合は1を指定、テクスチャの場合は縦幅を指定
		resDesc.DepthOrArraySize = 1;								// 1 : バッファの場合は1を指定、テクスチャの場合は配列数を指定、ボリュームテクスチャの場合は奥行きを指定
		resDesc.MipLevels = 1;										// 1 : ミップマップレベル数を指定
		resDesc.Format = DXGI_FORMAT_UNKNOWN;						// DXGI_FORMAT_UNKNOWN : フォーマット未指定
		resDesc.SampleDesc.Count = 1;								// 1 : サンプリング数を指定
		resDesc.SampleDesc.Quality = 0;								// 0 : サンプリング品質を指定
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;			// D3D12_TEXTURE_LAYOUT_ROW_MAJOR : 行優先で保存されたテクスチャデータを指定
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;					// D3D12_RESOURCE_FLAG_NONE : オプションを特に指定しない

		// リソースの生成
		// 第1引数はHEAP_PROPERTIES構造体のポインタ
		// 第2引数はD3D12_HEAP_FLAG列挙型の値
		// 第3引数はD3D12_RESOURCE_DESC構造体のポインタ
		// 第4引数はリソースの初期状態を設定
		// 第5引数はレンダーターゲットと深度ステンシルテクスチャのためのオプション
		// 第6引数はインターフェスID
		auto hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,				// D3D12_HEAP_FLAG_NONE : ヒープのオプションを特に指定しない
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,							// D3D12_RESOURCE_DIMENSION_BUFFERを指定した場合はnullptrを指定
			IID_PPV_ARGS(m_pVB.ReleaseAndGetAddressOf()));

		if (FAILED(hr))
		{	// 失敗時
			assert(0 && "[Polygon.cpp]頂点バッファの生成に失敗しました。");
			return false;
		}

		// マッピング
		void* p = nullptr;
		// 第一引数はサブリソースのインデックス
		// 第二引数はマッピングの開始オフセットと終了オフセットを設定できる。nullptrを指定すると全体をマッピング
		// 第三引数はリソースデータへのポインタを受け取るメモリブロックへのポインタ
		hr = m_pVB->Map(0, nullptr, &p);
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]頂点バッファのマッピングに失敗しました。");
			return false;
		}

		// 頂点データをマッピング先に設定
		memcpy(p, vertices, sizeof(vertices));

		// マップング解除
		// 第一引数はサブリソースのインデックス
		// 第二引数はCPUが変更した範囲を指定、今回は全体を変更しているためnullptrを設定
		m_pVB->Unmap(0, nullptr);

		// 頂点バッファビューの設定
		m_vbView.BufferLocation = m_pVB->GetGPUVirtualAddress();	// バッファのGPU仮想アドレスを取得
		m_vbView.SizeInBytes = static_cast<UINT>(sizeof(vertices));	// 頂点バッファ全体のサイズを設定
		m_vbView.StrideInBytes = static_cast<UINT>(sizeof(Vertex));	// 1頂点のサイズを設定
	}

	// インデックスバッファの生成
	{
		uint32_t indices[] = { 0,1,2,0,2,3 };

		// ヒーププロパティ
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;						// D3D12_HEAP_TYPE_UPLOAD : CPUからGPUへの転送が高速
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPUページプロパティを未指定
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : メモリプール未指定
		heapProp.CreationNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定
		heapProp.VisibleNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定

		// リソースの設定
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;		// D3D12_RESOURCE_DIMENSION_BUFFER : リソースがバッファであることを示す
		resDesc.Alignment = 0;										// 0 : リソースのアライメントを指定しない
		resDesc.Width = sizeof(indices);							// インデックスデータのサイズ
		resDesc.Height = 1;											// 1 : バッファの場合は1を指定、テクスチャの場合は縦幅を指定
		resDesc.DepthOrArraySize = 1;								// 1 : バッファの場合は1を指定、テクスチャの場合は配列数を指定、ボリュームテクスチャの場合は奥行きを指定
		resDesc.MipLevels = 1;										// 1 : ミップマップレベル数を指定
		resDesc.Format = DXGI_FORMAT_UNKNOWN;						// DXGI_FORMAT_UNKNOWN : フォーマット未指定
		resDesc.SampleDesc.Count = 1;								// 1 : サンプリング数を指定
		resDesc.SampleDesc.Quality = 0;								// 0 : サンプリング品質を指定
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;			// D3D12_TEXTURE_LAYOUT_ROW_MAJOR : 行優先で保存されたテクスチャデータを指定
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;					// D3D12_RESOURCE_FLAG_NONE : オプションを特に指定しない

		// リソースの生成
		// 第1引数はHEAP_PROPERTIES構造体のポインタ
		// 第2引数はD3D12_HEAP_FLAG列挙型の値
		// 第3引数はD3D12_RESOURCE_DESC構造体のポインタ
		// 第4引数はリソースの初期状態を設定
		// 第5引数はレンダーターゲットと深度ステンシルテクスチャのためのオプション
		// 第6引数はインターフェスID
		auto hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,				// D3D12_HEAP_FLAG_NONE : ヒープのオプションを特に指定しない
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,							// D3D12_RESOURCE_DIMENSION_BUFFERを指定した場合はnullptrを指定
			IID_PPV_ARGS(m_pIB.ReleaseAndGetAddressOf()));

		if (FAILED(hr))
		{	// 失敗時
			assert(0 && "[Polygon.cpp]インデックスバッファの生成に失敗しました。");
			return false;
		}

		// マッピング
		void* p = nullptr;
		// 第一引数はサブリソースのインデックス
		// 第二引数はマッピングの開始オフセットと終了オフセットを設定できる。nullptrを指定すると全体をマッピング
		// 第三引数はリソースデータへのポインタを受け取るメモリブロックへのポインタ
		hr = m_pIB->Map(0, nullptr, &p);
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]インデックスバッファのマッピングに失敗しました。");
			return false;
		}

		// インデックスデータをマッピング先に設定
		memcpy(p, indices, sizeof(indices));

		// マップング解除
		m_pIB->Unmap(0, nullptr);

		// インデックスバッファビューの設定
		m_ibView.BufferLocation = m_pIB->GetGPUVirtualAddress();	// バッファのGPU仮想アドレスを取得
		m_ibView.Format = DXGI_FORMAT_R32_UINT;					// インデックスのフォーマットを設定
		m_ibView.SizeInBytes = static_cast<UINT>(sizeof(indices));	// インデックスバッファ全体のサイズを設定
	}

	// 定数バッファ用ディスクリプタヒープの生成
	// シングルバッファ状態だとGPU処理実行中に定数バッファを書き換える恐れがあるのでダブルバッファ化
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV : 定数バッファ、シェーダリソースビュー、アンオーダードアクセスビューを格納するヒープ
		desc.NumDescriptors = 4;								// 1フレームあたり1つで十分だがダブルバッファ用に2つ確保
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	// D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : シェーダから見えるヒープ
		desc.NodeMask = 0;

		auto hr = RENDERER.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapCBV_SRV_UAV.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]定数バッファ用ディスクリプタヒープの生成に失敗しました。");
			return false;
		}
	}

	// 定数バッファの生成
	{
		// ヒーププロパティ
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;						// D3D12_HEAP_TYPE_UPLOAD : CPUからGPUへの転送が高速
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPUページプロパティを未指定
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : メモリプール未指定
		heapProp.CreationNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定
		heapProp.VisibleNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定

		// リソースの設定
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Alignment = 0;
		resDesc.Width = sizeof(Transform);
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		auto incrementSize = RENDERER.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		for (int i = 0; i < 4; ++i)
		{
			// リソースの生成
			auto hr = RENDERER.GetDevice()->CreateCommittedResource(
				&heapProp,
				D3D12_HEAP_FLAG_NONE,
				&resDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(m_pCB[i].ReleaseAndGetAddressOf()));

			if (FAILED(hr))
			{
				assert(0 && "[Polygon.cpp]定数バッファの生成に失敗しました。");
				return false;
			}

			auto address = m_pCB[i]->GetGPUVirtualAddress();
			auto handleCPU = m_pHeapCBV_SRV_UAV->GetCPUDescriptorHandleForHeapStart();
			auto handleGPU = m_pHeapCBV_SRV_UAV->GetGPUDescriptorHandleForHeapStart();

			handleCPU.ptr += incrementSize * i;
			handleGPU.ptr += incrementSize * i;

			// 定数バッファビューの設定
			m_CBV[i].HandleCPU = handleCPU;
			m_CBV[i].HandleGPU = handleGPU;
			m_CBV[i].Desc.BufferLocation = address;
			m_CBV[i].Desc.SizeInBytes = sizeof(Transform);

			// 定数バッファービューの生成
			RENDERER.GetDevice()->CreateConstantBufferView(&m_CBV[i].Desc, m_CBV[i].HandleCPU);

			// マッピング
			hr = m_pCB[i]->Map(0, nullptr, reinterpret_cast<void**>(&m_CBV[i].pBuffer));
			if (FAILED(hr))
			{
				assert(0 && "[Polygon.cpp]定数バッファのマッピングに失敗しました。");
				return false;
			}

			auto eye = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
			auto focus = DirectX::XMVectorZero();
			auto up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			constexpr auto fovY = DirectX::XMConvertToRadians(37.5f);
			auto aspect = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);

			// 変換行列の設定
			m_CBV[i].pBuffer->mWorld = DirectX::XMMatrixIdentity();
			m_CBV[i].pBuffer->mView = DirectX::XMMatrixLookAtLH(eye, focus, up);
			m_CBV[i].pBuffer->mProj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, 1.0f, 1000.0f);
		}
	}

	// ルートシグネチャの生成
	{
		auto flag = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
		flag |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

		// ルートパラメータの設定
		D3D12_ROOT_PARAMETER param[2] = {};
		param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// D3D12_ROOT_PARAMETER_TYPE_CBV : 定数バッファビュー
		param[0].Descriptor.ShaderRegister = 0;
		param[0].Descriptor.RegisterSpace = 0;
		param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		D3D12_DESCRIPTOR_RANGE range = {};
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	// D3D12_DESCRIPTOR_RANGE_TYPE_CBV : 定数バッファビュー
		range.NumDescriptors = 1;
		range.BaseShaderRegister = 0;
		range.RegisterSpace = 0;
		range.OffsetInDescriptorsFromTableStart = 0;

		param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	// D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE : テーブル
		param[1].DescriptorTable.NumDescriptorRanges = 1;
		param[1].DescriptorTable.pDescriptorRanges = &range;
		param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		// スタティックサンプラーの設定
		D3D12_STATIC_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					// D3D12_FILTER_MIN_MAG_MIP_LINEAR : テクスチャの拡大縮小に線形補間を使用
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;				// D3D12_TEXTURE_ADDRESS_MODE_CLAMP : テクスチャ座標が0.0f～1.0fの範囲外になった場合、0.0fまたは1.0fにクランプ
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;				// D3D12_TEXTURE_ADDRESS_MODE_CLAMP : テクスチャ座標が0.0f～1.0fの範囲外になった場合、0.0fまたは1.0fにクランプ
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;				// D3D12_TEXTURE_ADDRESS_MODE_CLAMP : テクスチャ座標が0.0f～1.0fの範囲外になった場合、0.0fまたは1.0fにクランプ
		sampler.MipLODBias = D3D12_DEFAULT_MIP_LOD_BIAS;					// D3D12_DEFAULT_MIP_LOD_BIAS : ミップマップレベルのバイアスを設定しない
		sampler.MaxAnisotropy = 1;											// 1 : 異方性フィルタリングを使用しない
		sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;				// D3D12_COMPARISON_FUNC_NEVER : 比較を行わない
		sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK : テクスチャの境界色を透明黒に設定
		sampler.MinLOD = -D3D12_FLOAT32_MAX;								// 最小のミップマップレベルを設定
		sampler.MaxLOD = +D3D12_FLOAT32_MAX;								// 最大のミップマップレベルを設定
		sampler.ShaderRegister = 0;											// レジスタスロット0に設定
		sampler.RegisterSpace = 0;											// レジスタスペース0に設定
		sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			// ピクセルシェーダからのみアクセス可能

		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = 2;
		desc.NumStaticSamplers = 1;
		desc.pParameters = param;
		desc.pStaticSamplers = &sampler;
		desc.Flags = flag;

		ComPtr<ID3DBlob> pBlob;
		ComPtr<ID3DBlob> pErrorBlob;

		// シリアライズ
		auto hr = D3D12SerializeRootSignature(
			&desc,
			D3D_ROOT_SIGNATURE_VERSION_1,
			pBlob.ReleaseAndGetAddressOf(),
			pErrorBlob.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]ルートシグネチャのシリアライズに失敗しました。");
			return false;
		}

		// ルートシグネチャの生成
		hr = RENDERER.GetDevice()->CreateRootSignature(
			0,								// GPUノードマスク
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			IID_PPV_ARGS(m_pRootSignature.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]ルートシグネチャの生成に失敗しました。");
			return false;
		}
	}

	// パイプラインステートの生成
	{
		// 入力レイアウトの設定
		D3D12_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
			{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		};

		// ラスタライザステートの設定
		D3D12_RASTERIZER_DESC rsDesc = {};
		rsDesc.FillMode = D3D12_FILL_MODE_SOLID;								// 頂点によって形作られている三角形で描画
		rsDesc.CullMode = D3D12_CULL_MODE_NONE;									// カリングしない	
		rsDesc.FrontCounterClockwise = FALSE;									// 時計回りが表面
		rsDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;							// 使用しないのでデフォルト
		rsDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;					// 使用しないのでデフォルト
		rsDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;	// 使用しないのでデフォルト
		rsDesc.DepthClipEnable = FALSE;											// 使用しないのでFALSE
		rsDesc.MultisampleEnable = FALSE;										// 使用しないのでFALSE
		rsDesc.AntialiasedLineEnable = FALSE;									// 使用しないのでFALSE
		rsDesc.ForcedSampleCount = 0;											// 0 : サンプル数が強制されない
		rsDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF : 保守的なラスタライゼーションを無効にする

		// レンダーターゲットのブレンド設定
		D3D12_RENDER_TARGET_BLEND_DESC rtDesc =
		{
			FALSE,FALSE,
			D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE,D3D12_BLEND_ZERO,D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL
		};

		// ブレンドステートの設定
		D3D12_BLEND_DESC bsDesc = {};
		bsDesc.AlphaToCoverageEnable = FALSE;
		bsDesc.IndependentBlendEnable = FALSE;
		for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		{
			bsDesc.RenderTarget[i] = rtDesc;
		}

		// 深度ステンシルステートの設定
		D3D12_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		dsDesc.StencilEnable = FALSE;

		ComPtr<ID3DBlob> pVSBlob;
		ComPtr<ID3DBlob> pPSBlob;

		std::wstring vsPath;
		std::wstring psPath;

		if (!SearchFilePath(L"Assets/Shaders/VS_Texture.cso", vsPath))
		{
			assert(0 && "[Polygon.cpp]頂点シェーダーが見つかりませんでした");
			return false;
		}
		if (!SearchFilePath(L"Assets/Shaders/PS_Texture.cso", psPath))
		{
			assert(0 && "[Polygon.cpp]ピクセルシェーダーが見つかりませんでした");
			return false;
		}

		// 頂点シェーダーの読み込み
		auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]頂点シェーダーの読み込みに失敗しました。");
			return false;
		}
		// ピクセルシェーダーの読み込み
		hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]ピクセルシェーダーの読み込みに失敗しました。");
			return false;
		}

		// パイプラインステートの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { layout,_countof(layout) };
		desc.pRootSignature = m_pRootSignature.Get();
		desc.VS = { pVSBlob->GetBufferPointer(),pVSBlob->GetBufferSize() };
		desc.PS = { pPSBlob->GetBufferPointer(),pPSBlob->GetBufferSize() };
		desc.RasterizerState = rsDesc;
		desc.BlendState = bsDesc;
		desc.DepthStencilState = dsDesc;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// パイプラインステートの生成
		hr = RENDERER.GetDevice()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPSO.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]パイプラインステートの生成に失敗しました。");
			return false;
		}

	}

	// テクスチャの生成
	if (!CreateTexture())
	{
		assert(0 && "[Polygon.cpp]テクスチャの生成に失敗しました。");
		return false;
	}

	// ビューポートとシザー矩形の設定
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(WIDTH);
		m_viewport.Height = static_cast<float>(HEIGHT);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_scissorRect.left = 0;
		m_scissorRect.right = WIDTH;
		m_scissorRect.top = 0;
		m_scissorRect.bottom = HEIGHT;
	}
	return true;
}

void Texture::Draw()
{
	// 更新処理
	{
		m_RotateAngle += 0.025f;
		m_CBV[RENDERER.GetFrameIndex() * 2 + 0].pBuffer->mWorld = DirectX::XMMatrixRotationZ(m_RotateAngle + DirectX::XMConvertToRadians(45.0f));
		m_CBV[RENDERER.GetFrameIndex() * 2 + 1].pBuffer->mWorld = DirectX::XMMatrixRotationY(m_RotateAngle) * DirectX::XMMatrixScaling(2.0f, 0.5f, 1.0f);
	}

	// 描画処理
	{
		auto cmdList = RENDERER.GetCmdList();
		cmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
		cmdList->SetDescriptorHeaps(1, m_pHeapCBV_SRV_UAV.GetAddressOf());
		cmdList->SetPipelineState(m_pPSO.Get());

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->IASetVertexBuffers(0, 1, &m_vbView);
		cmdList->IASetIndexBuffer(&m_ibView);
		cmdList->RSSetViewports(1, &m_viewport);
		cmdList->RSSetScissorRects(1, &m_scissorRect);


		uint32_t frameIndex = RENDERER.GetFrameIndex();
		// 手前のポリゴン
		cmdList->SetGraphicsRootConstantBufferView(0, m_CBV[frameIndex * 2 + 0].Desc.BufferLocation);
		cmdList->SetGraphicsRootDescriptorTable(1, m_Texture.HandleGPU);
		// 描画
		// 第1引数はインデックス数 : 1つのインデックスが1つの頂点を指すため、頂点数と同じ6を指定
		// 第2引数はインスタンス数 : 四角形一つのため1を指定
		// 第3引数は開始インデックス : 先頭データから描画するため0を指定
		// 第4引数は開始頂点 : 0を指定
		// 第5引数はインスタンスの開始インデックス : 最初のインスタンスから描画するため0を指定
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		// 奥のポリゴン
		cmdList->SetGraphicsRootConstantBufferView(0, m_CBV[frameIndex * 2 + 1].Desc.BufferLocation);
		cmdList->SetGraphicsRootDescriptorTable(1, m_Texture.HandleGPU);
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void Texture::Uninit()
{
	// マッピング解除
	for (int i = 0; i < 2; ++i)
	{
		if (m_pCB[i])
		{
			m_pCB[i]->Unmap(0, nullptr);
			memset(&m_CBV[i], 0, sizeof(m_CBV[i]));
		}
		m_pCB[i].Reset();
	}
	m_pVB.Reset();
	m_pPSO.Reset();
}

bool Texture::CreateTexture()
{
	// ファイルパスを検索
	std::wstring texPath;
	if (!SearchFilePath(L"Assets/Texture/SampleTexture.dds", texPath))
	{
		assert(0 && "[Texture.cpp]テクスチャファイルが見つかりませんでした。");
		return false;
	}

	// テクスチャの生成
	{
		// テクスチャの読み込み
		DirectX::TexMetadata metadata = {};
		DirectX::ScratchImage scratch = {};
		auto hr = DirectX::LoadFromWICFile(texPath.c_str(), DirectX::WIC_FLAGS_NONE, &metadata, scratch);
		if (FAILED(hr))
		{
			assert(0 && "[Texture.cpp]テクスチャの読み込みに失敗しました。");
			return false;
		}

		// テクスチャの設定
		auto img = scratch.GetImage(0, 0, 0);

		// ヒーププロパティ
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;						// D3D12_HEAP_TYPE_DEFAULT : GPUからの読み書きが高速
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	// D3D12_CPU_PAGE_PROPERTY_UNKNOWN : CPUページプロパティを未指定
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;	// D3D12_MEMORY_POOL_UNKNOWN : メモリプール未指定
		heapProp.CreationNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定
		heapProp.VisibleNodeMask = 1;								// 単一GPUによる動作を前提としているため1を指定

		// リソースの設定
		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;		// D3D12_RESOURCE_DIMENSION_TEXTURE2D : リソースが2次元テクスチャであることを示す
		resDesc.Alignment = 0;										// 0 : リソースのアライメントを指定しない
		resDesc.Width = metadata.width;								// テクスチャの横幅
		resDesc.Height = metadata.height;							// テクスチャの縦幅
		resDesc.DepthOrArraySize = 1;								// 1 : テクスチャの深度または配列サイズ
		resDesc.MipLevels = 1;										// 1 : ミップマップレベル数
		resDesc.Format = metadata.format;							// テクスチャのフォーマット
		resDesc.SampleDesc.Count = 1;								// 1 : サンプリング数
		resDesc.SampleDesc.Quality = 0;								// 0 : サンプリング品質
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;				// D3D12_TEXTURE_LAYOUT_UNKNOWN : テクスチャデータのレイアウトを未指定
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;					// D3D12_RESOURCE_FLAG_NONE : オプションを特に指定しない

		// リソースの生成
		hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,				// D3D12_HEAP_FLAG_NONE : ヒープのオプションを特に指定しない
			&resDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_Texture.pResource.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Texture.cpp]テクスチャリソースの生成に失敗しました。");
			return false;
		}


		// テクスチャのアップロード
		D3D12_HEAP_PROPERTIES heapPropUpload = {};
		heapPropUpload.Type = D3D12_HEAP_TYPE_CUSTOM;							// D3D12_HEAP_TYPE_UPLOAD : CPUからGPUへの転送が高速
		heapPropUpload.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	// D3D12_CPU_PAGE_PROPERTY_WRITE_BACK : CPUページプロパティを書き戻し可能に設定
		heapPropUpload.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;				// D3D12_MEMORY_POOL_L0 : メモリプールをL0に設定
		heapPropUpload.CreationNodeMask = 0;									// 単一GPUによる動作を前提としているため0を指定
		heapPropUpload.VisibleNodeMask = 0;										// 単一GPUによる動作を前提としているため0を指定

		D3D12_RESOURCE_DESC resDescUpload = {};
		resDescUpload.Format = metadata.format;									// テクスチャのフォーマット
		resDescUpload.Width = metadata.width;									// テクスチャの横幅
		resDescUpload.Height = metadata.height;									// テクスチャの縦幅
		resDescUpload.DepthOrArraySize = metadata.arraySize;
		resDescUpload.SampleDesc.Count = 1;
		resDescUpload.SampleDesc.Quality = 0;
		resDescUpload.MipLevels = metadata.mipLevels;
		resDescUpload.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);	// D3D12_RESOURCE_DIMENSION_BUFFER : リソースがバッファであることを示す
		resDescUpload.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;								
		resDescUpload.Flags = D3D12_RESOURCE_FLAG_NONE;									// D3D12_RESOURCE_FLAG_NONE : オプションを特に指定しない

		hr = RENDERER.GetDevice()->CreateCommittedResource(
			&heapPropUpload,
			D3D12_HEAP_FLAG_NONE,
			&resDescUpload,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(m_Texture.pResource.ReleaseAndGetAddressOf()));
		if (FAILED(hr))
		{
			assert(0 && "[Texture.cpp]テクスチャアップロード用リソースの生成に失敗しました。");
			return false;
		}

		// テクスチャデータのコピー
		hr = m_Texture.pResource->WriteToSubresource(0, nullptr, img->pixels, img->rowPitch, img->slicePitch);
		if (FAILED(hr))
		{
			assert(0 && "[Texture.cpp]テクスチャデータのコピーに失敗しました。");
			return false;
		}
	}

	// インクリメントサイズを取得
	auto incrementSize = RENDERER.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// CPUハンドルとGPUハンドルを取得
	auto handleCPU = m_pHeapCBV_SRV_UAV->GetCPUDescriptorHandleForHeapStart();
	auto handleGPU = m_pHeapCBV_SRV_UAV->GetGPUDescriptorHandleForHeapStart();

	// テクスチャにディスクリプタを割り当てる
	handleCPU.ptr += incrementSize * 2;
	handleGPU.ptr += incrementSize * 2;

	m_Texture.HandleCPU = handleCPU;
	m_Texture.HandleGPU = handleGPU;

	// テクスチャの構成設定を取得
	auto desc = m_Texture.pResource->GetDesc();

	// シェーダリソースビューの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = desc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	// シェーダリソースビューの生成
	RENDERER.GetDevice()->CreateShaderResourceView(m_Texture.pResource.Get(), &srvDesc, m_Texture.HandleCPU);

	return true;
}
