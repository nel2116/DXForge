// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// [Renderer.cpp]
// 作成者 : 田中ミノル
// 作成日 : 2024/05/25 17:00
// 概要   : レンダラークラスの実装
// 更新履歴
// 2024/05/25 作成
// 2024/06/24 リファクタリング
// _/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
// ====== インクルード部 ======
#include "stdafx.h"
#include "Renderer.h"
#include <System/SimpleMath.h>
#include <System/d3dx12.h>

using namespace DirectX::SimpleMath;

struct alignas(256) CbMesh
{
	Matrix   World;      //!< ワールド行列です.
};

struct alignas(256) CbTransform
{
	Matrix   View;       //!< ビュー行列です.
	Matrix   Proj;       //!< 射影行列です.
};

namespace
{
	// 色空間の種類
	enum COLOR_SPACE_TYPE
	{
		COLOR_SPACE_BT709,      // ITU-R BT.709
		COLOR_SPACE_BT2100_PQ,  // ITU-R BT.2100 PQ System
	};

	// トーンマップの種類
	enum TONEMAP_TYPE
	{
		TONEMAP_NONE = 0,   // トーンマップなし
		TONEMAP_REINHARD,   // Reinhardトーンマップ
		TONEMAP_GT,         // GTトーンマップ
	};

	// トーンマップの設定.
	struct alignas(256) CbTonemap
	{
		int     Type;               // トーンマップタイプ
		int     ColorSpace;         // 出力色空間
		float   BaseLuminance;      // 基準輝度値[nit]
		float   MaxLuminance;       // 最大輝度値[nit]
	};

	// 色度を取得
	inline UINT16 GetChromaticityCoord(double value)
	{
		return UINT16(value * 50000);
	}

	// デバッグレイヤーの有効化
	void EnableDebugLayer()
	{
		ComPtr<ID3D12Debug> debug;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(debug.ReleaseAndGetAddressOf()));

		// デバッグレイヤーの有効化
		if (SUCCEEDED(hr))
		{
			debug->EnableDebugLayer();
		}
	}

	// 交差領域の計算
	inline int ComputeIntersectionArea(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
	{
		return std::max(0, std::min(ax2, bx2) - std::max(ax1, bx1)) * std::max(0, std::min(ay2, by2) - std::max(ay1, by1));
	}

}

bool Renderer::Init(Window* window)
{
	// ウィンドウクラスのポインタを設定
	m_pWindow = window;

	m_TonemapType = TONEMAP_NONE;
	m_ColorSpace = COLOR_SPACE_BT709;

#if defined(_DEBUG) || defined(DEBUG)
	// デバッグレイヤーの有効化
	EnableDebugLayer();
#endif	// _DEBUG || DEBUG

	// デバイスの生成.
	auto hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pDevice));
	if (FAILED(hr))
	{
		return false;
	}

	// コマンドキューの生成.
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		hr = m_pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_pQueue));
		if (FAILED(hr))
		{
			return false;
		}
	}

	// スワップチェインの生成.
	{
		// DXGIファクトリーの生成.
		hr = CreateDXGIFactory2(0, IID_PPV_ARGS(m_pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			return false;
		}

		// スワップチェインの設定.
		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Width = m_pWindow->GetWidth();
		desc.BufferDesc.Height = m_pWindow->GetHeight();
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.BufferDesc.Format = m_BackBufferFormat;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = FRAME_BUFFER_COUNT;
		desc.OutputWindow = m_pWindow->GetHwnd();
		desc.Windowed = TRUE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		// スワップチェインの生成.
		ComPtr<IDXGISwapChain> pSwapChain;
		hr = m_pFactory->CreateSwapChain(m_pQueue.Get(), &desc, pSwapChain.GetAddressOf());
		if (FAILED(hr))
		{
			return false;
		}

		// IDXGISwapChain4 を取得.
		hr = pSwapChain.As(&m_pSwapChain);
		if (FAILED(hr))
		{
			return false;
		}

		// バックバッファ番号を取得.
		m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

		// 不要になったので解放.
		pSwapChain.Reset();
	}

	// ディスクリプタプールの生成.
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};

		desc.NodeMask = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = 512;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_RES]))
		{
			return false;
		}

		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.NumDescriptors = 256;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_SMP]))
		{
			return false;
		}

		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = 512;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_RTV]))
		{
			return false;
		}

		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = 512;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (!DescriptorPool::Create(&desc, &m_pPool[POOL_TYPE_DSV]))
		{
			return false;
		}
	}

	// コマンドリストの生成.
	{
		if (!m_CommandList.Init(D3D12_COMMAND_LIST_TYPE_DIRECT, FRAME_BUFFER_COUNT))
		{
			return false;
		}
	}

	// レンダーターゲットビューの生成.
	{
		// クリアカラー
		float clearColor[] = { 0.3f, 0.3f, 0.3f, 1.0f };
		for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_ColorTarget[i].InitFromBackBuffer(m_pPool[POOL_TYPE_RTV], i, true))
			{
				return false;
			}
			m_ColorTarget[i].SetClearColor(clearColor);
		}
	}

	// 深度ステンシルバッファの生成
	{
		if (!m_DepthTarget.Init(m_pPool[POOL_TYPE_DSV], nullptr, m_pWindow->GetWidth(), m_pWindow->GetHeight(), DXGI_FORMAT_D32_FLOAT, 1.0f, 0))
		{
			return false;
		}
	}

	// フェンスの生成.
	if (!m_Fence.Init())
	{
		return false;
	}

	// ビューポートの設定.
	{
		m_Viewport.TopLeftX = 0.0f;
		m_Viewport.TopLeftY = 0.0f;
		m_Viewport.Width = float(m_pWindow->GetWidth());
		m_Viewport.Height = float(m_pWindow->GetHeight());
		m_Viewport.MinDepth = 0.0f;
		m_Viewport.MaxDepth = 1.0f;
	}

	// シザー矩形の設定.
	{
		m_Scissor.left = 0;
		m_Scissor.right = m_pWindow->GetWidth();
		m_Scissor.top = 0;
		m_Scissor.bottom = m_pWindow->GetHeight();
	}

	// トーンマップ用ルートシグニチャの生成.
	{
		RootSignature::Desc desc;
		desc.Begin(4)
			.SetCBV(ShaderStage::VS, 0, 0)
			.SetCBV(ShaderStage::VS, 1, 1)
			.SetCBV(ShaderStage::PS, 2, 0)
			.SetSRV(ShaderStage::PS, 3, 0)
			.AddStaticSmp(ShaderStage::PS, 0, SamplerState::LinearWrap)
			.AllowIL()
			.End();

		if (!m_TonemapRootSig.Init(desc.GetDesc()))
		{
			ELOG("Error : RootSignature::Init() Failed.");
			return false;
		}
	}

	// トーンマップ用パイプラインステートの生成.
	{
		std::wstring vsPath;
		std::wstring psPath;

		// 頂点シェーダを検索.
		if (!SearchFilePathW(L"Assets/Shaders/VS_Tonemap.cso", vsPath))
		{
			ELOG("Error : Vertex Shader Not Found.");
			return false;
		}

		// ピクセルシェーダを検索.
		if (!SearchFilePathW(L"Assets/Shaders/PS_Tonemap.cso", psPath))
		{
			ELOG("Error : Pixel Shader Node Found.");
			return false;
		}

		ComPtr<ID3DBlob> pVSBlob;
		ComPtr<ID3DBlob> pPSBlob;

		// 頂点シェーダを読み込む.
		auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
			return false;
		}

		// ピクセルシェーダを読み込む.
		hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
		if (FAILED(hr))
		{
			ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
			return false;
		}

		D3D12_INPUT_ELEMENT_DESC elements[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		// ラスタライザステートの設定
		D3D12_RASTERIZER_DESC rsDesc = {};
		rsDesc.FillMode = D3D12_FILL_MODE_SOLID;								// 頂点によって形作られている三角形で描画
		rsDesc.CullMode = D3D12_CULL_MODE_NONE;									// 背面カリング
		rsDesc.FrontCounterClockwise = TRUE;									// 反時計回りが表面
		rsDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;							// 使用しないのでデフォルト
		rsDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;					// 使用しないのでデフォルト
		rsDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;	// 使用しないのでデフォルト
		rsDesc.DepthClipEnable = FALSE;											// 使用しないのでFALSE
		rsDesc.MultisampleEnable = FALSE;										// 使用しないのでFALSE
		rsDesc.AntialiasedLineEnable = FALSE;									// 使用しないのでFALSE
		rsDesc.ForcedSampleCount = 0;											// 0 : サンプル数が強制されない
		rsDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	// D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF : 保守的なラスタライゼーションを無効にする

		// レンダーターゲットのブレンド設定(aブレンド)
		D3D12_RENDER_TARGET_BLEND_DESC rtDesc =
		{
			TRUE,FALSE,
			D3D12_BLEND_SRC_ALPHA,D3D12_BLEND_INV_SRC_ALPHA,D3D12_BLEND_OP_ADD,
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

		// グラフィックスパイプラインステートを設定.
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.InputLayout = { elements, 2 };
		desc.pRootSignature = m_TonemapRootSig.GetPtr();
		desc.VS = { pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize() };
		desc.PS = { pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize() };
		desc.RasterizerState = rsDesc;
		desc.BlendState = bsDesc;
		desc.DepthStencilState = dsDesc;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = m_ColorTarget[0].GetRTVDesc().Format;
		desc.DSVFormat = m_DepthTarget.GetDSVDesc().Format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		// パイプラインステートを生成.
		hr = m_pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pTonemapPSO.GetAddressOf()));
		if (FAILED(hr))
		{
			ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
			return false;
		}
	}

	// 頂点バッファの生成
	{
		struct Vertex
		{
			float px;
			float py;

			float tx;
			float ty;
		};

		if (!m_QuadVB.Init<Vertex>(4))
		{
			ELOG("Error : VertexBuffer::Init() Failed.");
			return false;
		}

		auto ptr = m_QuadVB.Map<Vertex>();
		assert(ptr != nullptr);
		ptr[0].px = -1.0f;  ptr[0].py = -1.0f;  ptr[0].tx = 0.0f;   ptr[0].ty = 1.0f;
		ptr[1].px = -1.0f;  ptr[1].py = 1.0f;  ptr[1].tx = 0.0f;   ptr[1].ty = 0.0f;
		ptr[2].px = 1.0f;  ptr[2].py = -3.0f;  ptr[2].tx = 1.0f;   ptr[2].ty = 1.0f;
		ptr[3].px = 1.0f;  ptr[3].py = 1.0f;  ptr[3].tx = 1.0f;   ptr[3].ty = 0.0f;
		m_QuadVB.Unmap();
	}

	// 定数バッファの生成
	for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		if (!m_TonemapCB[i].Init(m_pPool[POOL_TYPE_RES], sizeof(CbTonemap)))
		{
			ELOG("Error : ConstantBuffer::Init() Failed.");
			return false;
		}
	}

	// 変換行列用の定数バッファの生成.
	{
		for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
		{

			// 定数バッファ初期化.
			if (!m_TransformCB[i].Init(m_pPool[POOL_TYPE_RES], sizeof(CbTransform)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
				return false;
			}

			// 変換行列を設定.
			auto ptr = m_TransformCB[i].GetPtr<CbTransform>();
			Matrix mat = Matrix::Identity;
			ptr->View = mat.Transpose();
			ptr->Proj = mat.Transpose(); //Matrix::CreateOrthographic(WIDTH, HEIGHT, 0.1f, 1000.0f);
		}
	}

	// メッシュ用バッファの生成.
	{
		for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			if (!m_MeshCB[i].Init(m_pPool[POOL_TYPE_RES], sizeof(CbMesh)))
			{
				ELOG("Error : ConstantBuffer::Init() Failed.");
				return false;
			}

			auto ptr = m_MeshCB[i].GetPtr<CbMesh>();
			ptr->World = Matrix::Identity;
		}
	}


	// 初期化成功
	return true;
}

void Renderer::Begin()
{
	// コマンドリストの記録を開始
	auto pCmd = m_CommandList.Reset();

}

void Renderer::End(ColorTarget* sceneTarget)
{
	// コマンドリストの取得
	auto pCmd = m_CommandList.Get();

	// リソースバリアの設定
	TransitionResource(m_ColorTarget[m_FrameIndex].GetResource(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// レンダーターゲットビューの取得
	auto rtv = m_ColorTarget[m_FrameIndex].GetHandleRTV();
	auto dsv = m_DepthTarget.GetHandleDSV();

	// レンダーターゲットの設定
	pCmd->OMSetRenderTargets(1, &rtv->HandleCPU, FALSE, &dsv->HandleCPU);

	// レンダーターゲットのクリア
	m_ColorTarget[m_FrameIndex].ClearView(pCmd);
	// 深度ステンシルのクリア
	m_DepthTarget.ClearView(pCmd);

	// トーンマップを適用
	DrawTonemap(sceneTarget);

	// 表示用リソースバリア設定
	TransitionResource(m_ColorTarget[m_FrameIndex].GetResource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// コマンドリストの記録を終了
	pCmd->Close();

	// コマンドリストを実行
	ID3D12CommandList* pLists[] = { pCmd };
	m_pQueue->ExecuteCommandLists(1, pLists);

	// 画面に表示
	Present(1);
}

void Renderer::TransitionResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter, UINT subresource, D3D12_RESOURCE_BARRIER_FLAGS flags)
{
	// コマンドリストの取得
	auto pCmd = m_CommandList.Get();

	// リソースバリア設定
	CD3DX12_RESOURCE_BARRIER result = {};
	D3D12_RESOURCE_BARRIER& barrier = result;
	result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	result.Flags = flags;
	barrier.Transition.pResource = pResource;
	barrier.Transition.StateBefore = stateBefore;
	barrier.Transition.StateAfter = stateAfter;
	barrier.Transition.Subresource = subresource;

	// バリアを設定
	pCmd->ResourceBarrier(1, &barrier);
}

void Renderer::WaitGpu()
{
	// GPUの処理が完了するまで待機
	m_Fence.Wait(INFINITE);
}

void Renderer::Present(uint32_t interval)
{
	// 画面に表示.
	m_pSwapChain->Present(interval, 0);

	// 完了待ち.
	m_Fence.Wait(INFINITE);

	// フレーム番号を更新.
	m_FrameIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void Renderer::CheckSupportHDR()
{
	// 何も作られていない場合は処理しない.
	if (m_pSwapChain == nullptr || m_pFactory == nullptr || m_pDevice == nullptr)
	{
		return;
	}

	HRESULT hr = S_OK;

	// ウィンドウ領域を取得.
	RECT rect;
	GetWindowRect(m_pWindow->GetHwnd(), &rect);

	if (m_pFactory->IsCurrent() == false)
	{
		m_pFactory.Reset();
		hr = CreateDXGIFactory2(0, IID_PPV_ARGS(m_pFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			return;
		}
	}

	ComPtr<IDXGIAdapter1> pAdapter;
	hr = m_pFactory->EnumAdapters1(0, pAdapter.GetAddressOf());
	if (FAILED(hr))
	{
		return;
	}

	UINT i = 0;
	ComPtr<IDXGIOutput> currentOutput;
	ComPtr<IDXGIOutput> bestOutput;
	int bestIntersectArea = -1;

	// 各ディスプレイを調べる.
	while (pAdapter->EnumOutputs(i, &currentOutput) != DXGI_ERROR_NOT_FOUND)
	{
		auto ax1 = rect.left;
		auto ay1 = rect.top;
		auto ax2 = rect.right;
		auto ay2 = rect.bottom;

		// ディスプレイの設定を取得.
		DXGI_OUTPUT_DESC desc;
		hr = currentOutput->GetDesc(&desc);
		if (FAILED(hr))
		{
			return;
		}

		auto bx1 = desc.DesktopCoordinates.left;
		auto by1 = desc.DesktopCoordinates.top;
		auto bx2 = desc.DesktopCoordinates.right;
		auto by2 = desc.DesktopCoordinates.bottom;

		// 領域が一致するかどうか調べる.
		int intersectArea = ComputeIntersectionArea(ax1, ay1, ax2, ay2, bx1, by1, bx2, by2);
		if (intersectArea > bestIntersectArea)
		{
			bestOutput = currentOutput;
			bestIntersectArea = intersectArea;
		}

		i++;
	}

	// 一番適しているディスプレイ.
	ComPtr<IDXGIOutput6> pOutput6;
	hr = bestOutput.As(&pOutput6);
	if (FAILED(hr))
	{
		return;
	}

	// 出力設定を取得.
	DXGI_OUTPUT_DESC1 desc1;
	hr = pOutput6->GetDesc1(&desc1);
	if (FAILED(hr))
	{
		return;
	}

	// 色空間が ITU-R BT.2100 PQをサポートしているかどうかチェック.
	m_IsHDR = (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
	m_MaxLuminance = desc1.MaxLuminance;
	m_BaseLuminance = desc1.MinLuminance;
}

bool Renderer::IsSupportHDR() const
{
	return m_IsHDR;
}

float Renderer::GetMaxLuminance() const
{
	return m_MaxLuminance;
}

float Renderer::GetBaseLuminance() const
{
	return m_BaseLuminance;
}

void Renderer::DrawTonemap(ColorTarget* sceneTarget)
{
	// コマンドリストの取得
	auto pCmd = m_CommandList.Get();

	// 定数バッファ更新
	{
		auto ptr = m_TonemapCB[m_FrameIndex].GetPtr<CbTonemap>();
		ptr->Type = m_TonemapType;
		ptr->ColorSpace = m_ColorSpace;
		ptr->BaseLuminance = m_BaseLuminance;
		ptr->MaxLuminance = m_MaxLuminance;
	}

	pCmd->SetGraphicsRootSignature(m_TonemapRootSig.GetPtr());
	pCmd->SetGraphicsRootDescriptorTable(0, m_TransformCB[m_FrameIndex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(1, m_MeshCB[m_FrameIndex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(2, m_TonemapCB[m_FrameIndex].GetHandleGPU());
	pCmd->SetGraphicsRootDescriptorTable(3, sceneTarget->GetHandleSRV()->HandleGPU);

	pCmd->SetPipelineState(m_pTonemapPSO.Get());
	pCmd->RSSetViewports(1, &m_Viewport);
	pCmd->RSSetScissorRects(1, &m_Scissor);

	pCmd->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	auto vbv = m_QuadVB.GetView();
	pCmd->IASetVertexBuffers(0, 1, &vbv);
	pCmd->DrawInstanced(4, 1, 0, 0);
}

void Renderer::Uninit()
{
	// GPU処理の完了を待機.
	m_Fence.Sync();

	// フェンス破棄.
	m_Fence.Uninit();

	// 頂点バッファの破棄
	m_QuadVB.Uninit();

	// 定数バッファの破棄
	for (auto i = 0; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_TonemapCB[i].Uninit();
		m_TransformCB[i].Uninit();
		m_MeshCB[i].Uninit();
	}

	// トーンマップ用パイプラインステートの破棄.
	m_pTonemapPSO.Reset();

	// トーンマップ用ルートシグニチャの破棄.
	m_TonemapRootSig.Uninit();

	// レンダーターゲットビューの破棄.
	for (auto i = 0u; i < FRAME_BUFFER_COUNT; ++i)
	{
		m_ColorTarget[i].Uninit();
	}

	// 深度ステンシルビューの破棄.
	m_DepthTarget.Uninit();

	// コマンドリストの破棄.
	m_CommandList.Uninit();

	for (auto i = 0; i < POOL_COUNT; ++i)
	{
		if (m_pPool[i] != nullptr)
		{
			m_pPool[i]->Release();
			m_pPool[i] = nullptr;
		}
	}

	// スワップチェインの破棄.
	m_pSwapChain.Reset();

	// コマンドキューの破棄.
	m_pQueue.Reset();

	// デバイスの破棄.
	m_pDevice.Reset();

	m_pWindow = nullptr;
}

