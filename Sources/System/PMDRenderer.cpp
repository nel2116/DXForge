// [PMDRenderer.cpp]
// �쐬�� : 2024/05/23 18:00
// �쐬�� : �c���~�m��
// �T�v   : PMD�����_���[�N���X�̎���
#include "stdafx.h"
#include "PMDRenderer.h"
#include <DirectX/d3dx12.h>
#include <cassert>
#include <d3dcompiler.h>
#include "Dx12Wrapper.h"
#include <algorithm>

namespace
{
	void PrintErrorBlob(ID3DBlob* blob)
	{
		assert(blob);
		string err;
		err.resize(blob->GetBufferSize());
		copy_n((char*)blob->GetBufferPointer(), err.size(), err.begin());
	}
}

PMDRenderer::PMDRenderer(Dx12Wrapper& dx12)
	: m_dx12(dx12)
{
	assert(SUCCEEDED(CreateRootSignature()));
	assert(SUCCEEDED(CreateGraphicsPipelineForPMD()));
	m_whiteTex = CreateWhiteTexture();
	m_blackTex = CreateBlackTexture();
	m_gradTex = CreateGradationTexture();
}

PMDRenderer::~PMDRenderer()
{
}

void PMDRenderer::Update()
{
}

void PMDRenderer::Draw()
{
}

ID3D12PipelineState* PMDRenderer::GetPipelineState()
{
	return m_pipeline.Get();
}

ID3D12RootSignature* PMDRenderer::GetRootSignature()
{
	return m_rootSignature.Get();
}

ID3D12Resource* PMDRenderer::CreateDeaultTexture(size_t width, size_t height)
{
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	CD3DX12_HEAP_PROPERTIES texHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	ID3D12Resource* buff = nullptr;
	auto hr = m_dx12.GetDevice()->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&buff)
	);
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
		return nullptr;
	}
	return buff;
}

ID3D12Resource* PMDRenderer::CreateWhiteTexture()
{
	ID3D12Resource* whitebuff = CreateDeaultTexture(4, 4);

	vector<unsigned char> data(4 * 4 * 4);
	fill(data.begin(), data.end(), 0xff);

	auto hr = whitebuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	assert(SUCCEEDED(hr));
	return whitebuff;
}

ID3D12Resource* PMDRenderer::CreateBlackTexture()
{
	ID3D12Resource* blackbuff = CreateDeaultTexture(4, 4);

	vector<unsigned char> data(4 * 4 * 4);
	fill(data.begin(), data.end(), 0x00);

	auto hr = blackbuff->WriteToSubresource(0, nullptr, data.data(), 4 * 4, data.size());
	assert(SUCCEEDED(hr));
	return blackbuff;
}

ID3D12Resource* PMDRenderer::CreateGradationTexture()
{
	ID3D12Resource* gradbuff = CreateDeaultTexture(4, 256);

	// �オ�����ĉ��������e�N�X�`���f�[�^���쐬
	vector<unsigned char> data(4 * 256);
	unsigned int c = 0xff;
	for (auto it = data.begin(); it != data.end(); it += 4)
	{
		auto col = (c << 0xff) | (c << 16) | (c << 8) | c;
		fill(it, it + 4, col);
		--c;
	}

	auto hr = gradbuff->WriteToSubresource(0, nullptr, data.data(), 4 * sizeof(unsigned int), sizeof(unsigned int) * data.size());
	assert(SUCCEEDED(hr));
	return gradbuff;
}

// �p�C�v���C��������
HRESULT PMDRenderer::CreateGraphicsPipelineForPMD()
{
	// �V�F�[�_�[�t�@�C���̓ǂݍ���
	ComPtr<ID3DBlob> vsBlob = nullptr;
	ComPtr<ID3DBlob> psBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;

	auto hr = D3DCompileFromFile(
		L"Sources/HLSL/VertexShader/VS_Basic.hlsl",
		nullptr,
		nullptr,
		"VS_Main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob,
		&errBlob);

	// �G���[�`�F�b�N
	if (!CheckShaderCompileResult(hr, errBlob.Get()))
	{
		assert(0);
		return hr;
	}

	hr = D3DCompileFromFile(
		L"Sources/HLSL/PixelShader/PS_Basic.hlsl",
		nullptr,
		nullptr,
		"PS_Main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob,
		&errBlob);
	// �G���[�`�F�b�N
	if (!CheckShaderCompileResult(hr, errBlob.Get()))
	{
		assert(0);
		return hr;
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		// { "BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		// { "WEIGHT",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
		// { "EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0 },
	};

	// �p�C�v���C���X�e�[�g�̐ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	gpipeline.pRootSignature = m_rootSignature.Get();
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	// ���X�^���C�U�[�X�e�[�g�̐ݒ�
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// �J�����O�Ȃ�

	// �f�v�X�X�e���V���X�e�[�g�̐ݒ�
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;	// ���ׂď�������
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;	// �������ق����̗p
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.DepthStencilState.StencilEnable = false;

	// ���̓��C�A�E�g�ݒ�
	gpipeline.InputLayout.pInputElementDescs = inputLayout;	// ���C�A�E�g�擪�A�h���X
	gpipeline.InputLayout.NumElements = _countof(inputLayout);	// �z��v�f��

	// �v���~�e�B�u�g�|���W�ݒ�
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED; // �X�g���b�v�J�b�g�Ȃ�
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;	// �O�p�`���X�g

	// 
	gpipeline.NumRenderTargets = 1;	// ���͈�̂�
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;	// 0�`1�ɐ��K�����ꂽRGBA

	// �T���v�����O�ݒ�
	gpipeline.SampleDesc.Count = 1;	// �T���v�����O��1�s�N�Z���ɂ�1��
	gpipeline.SampleDesc.Quality = 0;	// �N�I���e�B�͍Œ�

	hr = m_dx12.GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&m_pipeline));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
	}
	return hr;
}

// ���[�g�V�O�l�`��������
HRESULT PMDRenderer::CreateRootSignature()
{
	// �����W
	CD3DX12_DESCRIPTOR_RANGE descTblRanges[4] = {};
	descTblRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);	// �萔[b0](�r���[�v���W�F�N�V�����p)
	descTblRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);	// �萔[b1](���[���h�A�{�[���s��p)
	descTblRanges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2);	// �萔[b2](�}�e���A���p)
	descTblRanges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0);	// �e�N�X�`��4��(��{��sph��spa�ƃg�D�[��)

	// ���[�g�p�����[�^
	CD3DX12_ROOT_PARAMETER rootParam[3] = {};
	rootParam[0].InitAsDescriptorTable(1, &descTblRanges[0]);	// �r���[�v���W�F�N�V�����s��
	rootParam[1].InitAsDescriptorTable(1, &descTblRanges[1]);	// ���[���h�A�{�[���s��
	rootParam[2].InitAsDescriptorTable(2, &descTblRanges[2]);	// �}�e���A��

	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};
	samplerDescs[0].Init(0);
	samplerDescs[1].Init(1, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.Init(_countof(rootParam), rootParam, _countof(samplerDescs), samplerDescs, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob = nullptr;
	ComPtr<ID3DBlob> errBlob = nullptr;

	// �V���A���C�Y
	auto hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &rootSigBlob, &errBlob);
	// �G���[�`�F�b�N
	if (!CheckShaderCompileResult(hr, errBlob.Get()))
	{
		assert(0);
		return hr;
	}

	hr = m_dx12.GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(m_rootSignature.ReleaseAndGetAddressOf()));
	// �G���[�`�F�b�N
	if (FAILED(hr))
	{
		assert(SUCCEEDED(hr));
	}
	return hr;
}

bool PMDRenderer::CheckShaderCompileResult(HRESULT hr, ID3DBlob* err)
{
	if (FAILED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else
		{
			string errstr;
			errstr.resize(err->GetBufferSize());
			copy_n((char*)err->GetBufferPointer(), errstr.size(), errstr.begin());
			errstr += "\n";
			::OutputDebugStringA(errstr.c_str());
		}
		return false;
	}
	else
	{
		return true;
	}
}
