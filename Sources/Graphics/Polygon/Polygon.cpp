#include "stdafx.h"
#include "Polygon.h"

bool MyPolygon::Init()
{
	// 頂点バッファの生成
	{
		// 頂点データ
		Vertex vertices[] =
		{
			{DirectX::XMFLOAT3(-1.0f, 1.0f,0.0f),DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)},
			{DirectX::XMFLOAT3(1.0f, 1.0f,0.0f),DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
			{DirectX::XMFLOAT3(1.0f,-1.0f,0.0f),DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
			{DirectX::XMFLOAT3(-1.0f,-1.0f,0.0f),DirectX::XMFLOAT4(1.0f,0.0f,1.0f,1.0f)},
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

		auto hr = RENDERER.GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_pHeapCBV.ReleaseAndGetAddressOf()));
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
			auto handleCPU = m_pHeapCBV->GetCPUDescriptorHandleForHeapStart();
			auto handleGPU = m_pHeapCBV->GetGPUDescriptorHandleForHeapStart();

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
		D3D12_ROOT_PARAMETER param = {};
		param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// D3D12_ROOT_PARAMETER_TYPE_CBV : 定数バッファビュー
		param.Descriptor.ShaderRegister = 0;
		param.Descriptor.RegisterSpace = 0;
		param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		// ルートシグネチャの設定
		D3D12_ROOT_SIGNATURE_DESC desc = {};
		desc.NumParameters = 1;
		desc.NumStaticSamplers = 0;
		desc.pParameters = &param;
		desc.pStaticSamplers = nullptr;
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
			{ "COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
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

		// 頂点シェーダーの読み込み
		auto hr = D3DReadFileToBlob(L"Assets/Shaders/VS_Simple.cso", pVSBlob.ReleaseAndGetAddressOf());
		if (FAILED(hr))
		{
			assert(0 && "[Polygon.cpp]頂点シェーダーの読み込みに失敗しました。");
			return false;
		}
		// ピクセルシェーダーの読み込み
		hr = D3DReadFileToBlob(L"Assets/Shaders/PS_Simple.cso", pPSBlob.ReleaseAndGetAddressOf());
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

	// ビューポートとシザー矩形の設定
	{
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.Width = static_cast<float>(WIDTH);
		m_viewport.Height = static_cast<float>(HEIGHT);
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		m_scissorRect.left = 0;
		m_scissorRect.right = static_cast<LONG>(WIDTH);
		m_scissorRect.top = 0;
		m_scissorRect.bottom = static_cast<LONG>(HEIGHT);
	}
	return true;
}

void MyPolygon::Draw()
{
	// 更新処理
	{
		m_RotateAngle += 0.025f;
		m_CBV[RENDERER.GetFrameIndex() * 2 + 0].pBuffer->mWorld = DirectX::XMMatrixRotationZ(m_RotateAngle + DirectX::XMConvertToRadians(45.0f));
		m_CBV[RENDERER.GetFrameIndex() * 2 + 1].pBuffer->mWorld = DirectX::XMMatrixRotationY(m_RotateAngle) * DirectX::XMMatrixScaling(2.0f, 0.5f, 1.0f);
	}

	// 描画処理
	{
		auto cmdList = RENDERER.GetCmdList()->Get();
		cmdList->SetGraphicsRootSignature(m_pRootSignature.Get());
		cmdList->SetDescriptorHeaps(1, m_pHeapCBV.GetAddressOf());
		uint32_t frameIndex = RENDERER.GetFrameIndex();
		cmdList->SetPipelineState(m_pPSO.Get());

		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->IASetVertexBuffers(0, 1, &m_vbView);
		cmdList->IASetIndexBuffer(&m_ibView);
		cmdList->RSSetViewports(1, &m_viewport);
		cmdList->RSSetScissorRects(1, &m_scissorRect);

		// 手前のポリゴン
		cmdList->SetGraphicsRootConstantBufferView(0, m_CBV[frameIndex * 2 + 0].Desc.BufferLocation);
		// 描画
		// 第1引数はインデックス数 : 1つのインデックスが1つの頂点を指すため、頂点数と同じ6を指定
		// 第2引数はインスタンス数 : 四角形一つのため1を指定
		// 第3引数は開始インデックス : 先頭データから描画するため0を指定
		// 第4引数は開始頂点 : 0を指定
		// 第5引数はインスタンスの開始インデックス : 最初のインスタンスから描画するため0を指定
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);

		// 奥のポリゴン
		cmdList->SetGraphicsRootConstantBufferView(0, m_CBV[frameIndex * 2 + 1].Desc.BufferLocation);
		cmdList->DrawIndexedInstanced(6, 1, 0, 0, 0);
	}
}

void MyPolygon::Uninit()
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
