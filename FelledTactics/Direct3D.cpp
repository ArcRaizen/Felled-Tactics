#include "StdAfx.h"
#include "Direct3D.h"

#pragma region Static Declarations
GraphicsInfo* Direct3D::gpInfo = new GraphicsInfo();
ID3D10Device* Direct3D::gpDevice = 0;
IDXGISwapChain* Direct3D::gpSwapChain = 0;
ID3D10Texture2D* Direct3D::backBuffer = 0;
ID3D10RenderTargetView* Direct3D::gpRenderTargetView = 0;
ID3D10RasterizerState* Direct3D::gpRasterizerState = 0;
ID3D10Effect* Direct3D::gpEffect = 0;
ID3D10EffectTechnique* Direct3D::gpTechnique = 0;
ID3D10InputLayout* Direct3D::gpLayout = 0;
ID3D10Buffer* Direct3D::gpVertexBuffer = 0;
ID3D10Buffer* Direct3D::gpIndexBuffer = 0;
ID3D10EffectMatrixVariable* Direct3D::gpShaderWorldMatrix = 0;
ID3D10EffectMatrixVariable* Direct3D::gpShaderViewMatrix = 0;
ID3D10EffectMatrixVariable* Direct3D::gpShaderProjectionMatrix = 0;
ID3D10EffectVectorVariable* Direct3D::gpShaderHighlightColor = 0;
ID3D10EffectShaderResourceVariable* Direct3D::gpShaderTexture = 0;
ID3D10ShaderResourceView* Direct3D::gpShaderResourceView = 0;
ID3D10Texture2D* Direct3D::gpDepthStencilBuffer = 0;
ID3D10DepthStencilView* Direct3D::gpDepthStencilView = 0;
ID3D10DepthStencilState* Direct3D::gpDepthStencilState = 0;
ID3D10DepthStencilState* Direct3D::gpDepthDisabledStencilState = 0;
bool  Direct3D::fullScreen = false;
D3DXMATRIX Direct3D::projection;
D3DXMATRIX Direct3D::ortho;
ID3D10BlendState*  Direct3D::blendNoBlend = 0;
ID3D10BlendState*  Direct3D::blendAlpha = 0;
ID3D10BlendState*  Direct3D::blendAdditive = 0;
ID3D10BlendState*  Direct3D::blendSubtract = 0;
ID3DX10Font*  Direct3D::gpFont = 0;
bool  Direct3D::vSyncEnabled = false;
int  Direct3D::videoCardMemory = 0;
char  Direct3D::videoCardDescription[] = {'0'};
#pragma endregion

Direct3D::Direct3D(void){}

Direct3D::~Direct3D(void)
{
	gpDevice->Release(); delete gpDevice; gpDevice = 0;
	gpSwapChain->Release(); delete gpSwapChain; gpSwapChain = 0;
	backBuffer->Release(); delete backBuffer; backBuffer = 0;
	gpRenderTargetView->Release(); delete gpRenderTargetView; gpRenderTargetView = 0;
	gpRasterizerState->Release(); delete gpRasterizerState; gpRasterizerState = 0;
	gpEffect->Release(); delete gpEffect; gpEffect = 0;
	delete gpTechnique; gpTechnique = 0;
	gpLayout->Release(); delete gpLayout; gpLayout = 0;
	gpVertexBuffer->Release(); delete gpVertexBuffer; gpVertexBuffer = 0;
	gpIndexBuffer->Release(); delete gpIndexBuffer; gpIndexBuffer = 0;
	delete gpShaderWorldMatrix; gpShaderWorldMatrix = 0;
	delete gpShaderViewMatrix; gpShaderViewMatrix = 0;
	delete gpShaderProjectionMatrix; gpShaderProjectionMatrix = 0;
	delete gpShaderHighlightColor; gpShaderHighlightColor = 0;
	delete gpShaderTexture; gpShaderTexture = 0;
	gpShaderResourceView->Release(); delete gpShaderResourceView; gpShaderResourceView = 0;
	gpDepthStencilBuffer->Release(); delete gpDepthStencilBuffer; gpDepthStencilBuffer = 0;
	gpDepthStencilView->Release(); delete gpDepthStencilView; gpDepthStencilView = 0;
	gpDepthStencilState->Release(); delete gpDepthStencilState; gpDepthStencilState = 0;
	gpDepthDisabledStencilState->Release(); delete gpDepthDisabledStencilState; gpDepthDisabledStencilState = 0;
	blendNoBlend->Release(); delete blendNoBlend; blendNoBlend = 0;
	blendAlpha->Release(); delete blendAlpha; blendAlpha = 0;
	blendAdditive->Release(); delete blendAdditive; blendAdditive = 0;
	blendSubtract->Release(); delete blendSubtract; blendSubtract = 0;
	gpFont->Release(); delete gpFont; gpFont = 0;
}

void Direct3D::InitializeDirect3D(bool vsync, HWND hWnd, bool fScreen, float screenFar, float screenNear)
{
	vSyncEnabled = vsync;
	fullScreen = fScreen;

	hr = S_OK;	// Used for debugging
	assert(DefineSwapChain(hWnd));
	assert(CreateRenderTargetView());
	assert(CreateDepthStencilBuffer());
	assert(SetViewport(screenNear, screenFar));
	assert(InitializeShader(hWnd));
	assert(InitializeBlendStates());
	assert(InitFonts());
	assert(InitializeVertexBuffer());
	assert(InitializeIndexBuffer());

	// Save the graphics info struct so we can pass these things around
	gpInfo->gpDevice = gpDevice;
	gpInfo->gpTechnique = gpTechnique;
	gpInfo->gpLayout = gpLayout;
	gpInfo->gpVertexBuffer = gpVertexBuffer;
	gpInfo->gpShaderTexture = gpShaderTexture;
	gpInfo->gpShaderWorldMatrix = gpShaderWorldMatrix;
	gpInfo->gpShaderViewMatrix= gpShaderViewMatrix;
	gpInfo->gpShaderProjectionMatrix = gpShaderProjectionMatrix;
	gpInfo->gpShaderHighlightColor = gpShaderHighlightColor;
	gpInfo->gpShaderDefaultColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
	gpInfo->gpShaderDefaultUVScale = D3DXVECTOR2(1.0f, 1.0f);

	// Set initial global drawing states
	gpDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gpDevice->IASetInputLayout(gpLayout);
}

bool Direct3D::DefineSwapChain(HWND hWnd)
{
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	UINT numModes, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;

	// Create DirectX graphics interface factory
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(hr)) return hr;

	// Create adapter for primary video card
	hr = factory->EnumAdapters(0, &adapter);
	if(FAILED(hr)) return hr;

	// Enumerate primary adapter output
	hr = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(hr)) return hr;

	// Get number of modes that fit the appropriate format for the adapter output
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(hr)) return hr;
	
	// Create list of all possible display modes and fill it
	displayModeList = new DXGI_MODE_DESC[numModes];
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(hr)) return hr;

	// Find and save refresh rate
	for(int i = 0; i < numModes; i++)
	{
		if(displayModeList[i].Width == (UINT)screenWidth && displayModeList[i].Height == (UINT)screenHeight)
		{
			numerator = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// Get and save information about the video card
	hr = adapter->GetDesc(&adapterDesc);
	if(FAILED(hr)) return hr;
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0) return false;

	// Clear/Release all data just used
	delete [] displayModeList;
	displayModeList = 0;
	adapterOutput->Release();
	adapterOutput = 0;
	adapter->Release();
	adapter = 0;
	factory->Release();
	factory = 0;

	// Define the Swap Chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = vSyncEnabled ? numerator : 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = vSyncEnabled ? denominator : 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = !fullScreen;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// Debug mode
	UINT createDeviceFlag = 0;
#if defined(DEBUG)
	//createDeviceFlag |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	// Create device and swap chain
	hr = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL,
		createDeviceFlag, D3D10_SDK_VERSION, &swapChainDesc, &gpSwapChain,	&gpDevice);
	if(FAILED(hr))	return hr;

	return true;
}

// Create Render Target View
bool Direct3D::CreateRenderTargetView()
{
	// Get pointer to back buffer
	hr = gpSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBuffer);
	if(FAILED(hr)) return hr;

	// Create render target view with back buffer
	hr = gpDevice->CreateRenderTargetView(backBuffer, NULL, &gpRenderTargetView);
	if(FAILED(hr)) return hr;

	backBuffer->Release();
	backBuffer = 0;

	//gpDevice->OMSetRenderTargets(1, &gpRenderTargetView, 0);
	return true;
}
bool Direct3D::CreateDepthStencilBuffer()
{
	// Create the description of depth buffer
	D3D10_TEXTURE2D_DESC depthBufferDesc;
		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
		depthBufferDesc.Width = screenWidth;
		depthBufferDesc.Height = screenHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

	// Create depth buffer texture
	hr = gpDevice->CreateTexture2D(&depthBufferDesc, 0, &gpDepthStencilBuffer);
	if(FAILED(hr)) return hr;

	// Create description of stencil state
	D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		depthStencilDesc.DepthEnable = true;	
		depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;
		depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	// Create depth stencil state
	hr = gpDevice->CreateDepthStencilState(&depthStencilDesc, &gpDepthStencilState);
	if(FAILED(hr)) return hr;

	// Set depth stencil state
	gpDevice->OMSetDepthStencilState(gpDepthStencilState, 1);

	// Create description of depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create depth stencil view
	hr = gpDevice->CreateDepthStencilView(gpDepthStencilBuffer, &depthStencilViewDesc, &gpDepthStencilView);
	if(FAILED(hr)) return hr;
	
	// Bind render target view and depth stencil buffer to output render pipeline
	gpDevice->OMSetRenderTargets(1, &gpRenderTargetView, gpDepthStencilView);
	return true;
}

bool Direct3D::InitializeRasterizerState()
{
	// Create raster description (determine how/what polygons will be drawn)
	D3D10_RASTERIZER_DESC rasterizerStateDesc;
		ZeroMemory(&rasterizerStateDesc, sizeof(rasterizerStateDesc));
		rasterizerStateDesc.AntialiasedLineEnable = false;	// true;
		rasterizerStateDesc.CullMode = D3D10_CULL_BACK;		// D3D10_CULL_NONE
		rasterizerStateDesc.DepthBias = 0;
		rasterizerStateDesc.DepthBiasClamp = 0.0f;
		rasterizerStateDesc.DepthClipEnable = true;
		rasterizerStateDesc.FillMode = D3D10_FILL_SOLID;
		rasterizerStateDesc.FrontCounterClockwise = false;
		rasterizerStateDesc.MultisampleEnable = false;
		rasterizerStateDesc.ScissorEnable = false;
		rasterizerStateDesc.SlopeScaledDepthBias = 0.0f;

	// Create rasterizer state
	hr = gpDevice->CreateRasterizerState(&rasterizerStateDesc, &gpRasterizerState);
	if(FAILED(hr)) return hr;

	gpDevice->RSSetState(gpRasterizerState);
	return true;
}

bool Direct3D::SetViewport(float screenNear, float screenFar)
{
	// Set the Viewport (currently the same size as the window)
	D3D10_VIEWPORT vp;
	vp.Width = screenWidth;
	vp.Height = screenHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	// Create viewport
	gpDevice->RSSetViewports(1, &vp);

	float fieldOfView = (float)D3DX_PI / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// Projection Matrix for 3D rendering
	D3DXMatrixPerspectiveFovLH(&projection, fieldOfView, screenAspect, screenNear, screenFar);
	
	// Set World Matrix to Identity for now
//	D3DXMatrixIdentity(&world);

	// Ortographic Projection Matrix for 2D rendering
	D3DXMatrixOrthoLH(&ortho, (float)screenWidth, (float)screenHeight, screenNear, screenFar);

	// Create description for depth stencil state that turns off Z Buffer for 2D rendering
	D3D10_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
		ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
		depthDisabledStencilDesc.DepthEnable = false;
		depthDisabledStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
		depthDisabledStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
		depthDisabledStencilDesc.StencilEnable = true;
		depthDisabledStencilDesc.StencilReadMask = 0xFF;
		depthDisabledStencilDesc.StencilWriteMask = 0xFF;
		depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
		depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		depthDisabledStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		depthDisabledStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		depthDisabledStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

	hr = gpDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &gpDepthDisabledStencilState);
	if(FAILED(hr)) return hr;

	// Turn Z Buffer Off
	gpDevice->OMSetDepthStencilState(gpDepthDisabledStencilState, 1);
	return true;
}

bool Direct3D::InitializeShader(HWND hWnd)
{
	ID3D10Blob* errorMessage = 0;
	D3D10_INPUT_ELEMENT_DESC vertexDesc[2];
	UINT numElements;
	
	D3D10_PASS_DESC passDesc;

	hr = D3DX10CreateEffectFromFile(L"../FelledTactics/texture.fx", NULL, NULL, "fx_4_0", D3D10_SHADER_ENABLE_STRICTNESS,
										0, gpDevice, NULL, NULL, &gpEffect, &errorMessage, NULL);
	if(FAILED(hr))
	{
		MessageBox(hWnd,L"../FelledTactics/texture.fx", L"Done fucked up", MB_OK);
		return hr;
	}

	gpTechnique = gpEffect->GetTechniqueByName("TextureTechnique");

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].SemanticIndex = 0;
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].InputSlot = 0;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	vertexDesc[0].InstanceDataStepRate = 0;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].SemanticIndex = 0;
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].InputSlot = 0;
	vertexDesc[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	vertexDesc[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	vertexDesc[1].InstanceDataStepRate = 0;

	numElements = sizeof(vertexDesc) / sizeof(vertexDesc[0]);
	
	gpTechnique->GetPassByIndex(0)->GetDesc(&passDesc);
	hr = gpDevice->CreateInputLayout(vertexDesc, numElements, passDesc.pIAInputSignature, 
										passDesc.IAInputSignatureSize, &gpLayout);
	if(FAILED(hr)) return hr;

	// Get pointers to matrices from the shader
	gpShaderWorldMatrix = gpEffect->GetVariableByName("worldMatrix")->AsMatrix();
	gpShaderViewMatrix = gpEffect->GetVariableByName("viewMatrix")->AsMatrix();
	gpShaderProjectionMatrix = gpEffect->GetVariableByName("projectionMatrix")->AsMatrix();
	gpShaderHighlightColor = gpEffect->GetVariableByName("highlightColor")->AsVector();

	// Get pointer to texture from the shader
	gpShaderTexture = gpEffect->GetVariableByName("shaderTexture")->AsShaderResource();
	return true;
}

bool Direct3D::InitializeBlendStates()
{
	// No Blend
	D3D10_BLEND_DESC blendStateDesc;
		ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));
		blendStateDesc.BlendEnable[0] = false;
		blendStateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	hr = gpDevice->CreateBlendState(&blendStateDesc, &blendNoBlend);
	if(FAILED(hr)) return hr;

	// Alpha Blend
	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));
		blendStateDesc.BlendEnable[0] = true;
		blendStateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
		blendStateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
		blendStateDesc.BlendOp = D3D10_BLEND_OP_ADD;
		blendStateDesc.SrcBlendAlpha = D3D10_BLEND_ONE;
		blendStateDesc.DestBlendAlpha = D3D10_BLEND_ONE;
		blendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		blendStateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	hr = gpDevice->CreateBlendState(&blendStateDesc, &blendAlpha);
	if(FAILED(hr)) return hr;

	// Additive Blend
	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));
		blendStateDesc.BlendEnable[0] = TRUE;
		blendStateDesc.SrcBlend = D3D10_BLEND_ONE;
		blendStateDesc.DestBlend = D3D10_BLEND_ONE;
		blendStateDesc.BlendOp = D3D10_BLEND_OP_ADD;
		blendStateDesc.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
		blendStateDesc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
		blendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
		blendStateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	hr = gpDevice->CreateBlendState(&blendStateDesc, &blendAdditive);
	if(FAILED(hr)) return hr;

	// Subtractive Blend
	ZeroMemory(&blendStateDesc, sizeof(blendStateDesc));
		blendStateDesc.BlendEnable[0] = TRUE;
		blendStateDesc.SrcBlend = D3D10_BLEND_ONE;
		blendStateDesc.DestBlend = D3D10_BLEND_ONE;
		blendStateDesc.BlendOp = D3D10_BLEND_OP_REV_SUBTRACT;
		blendStateDesc.SrcBlendAlpha = D3D10_BLEND_ZERO;
		blendStateDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
		blendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_SUBTRACT;
		blendStateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;
	hr = gpDevice->CreateBlendState(&blendStateDesc, &blendSubtract);
	if(FAILED(hr)) return hr;

	// Set default blend state
	gpDevice->OMSetBlendState(blendAlpha, 0, 0xffffffff);
	return true;
}

bool Direct3D::InitFonts()
{
	D3DX10_FONT_DESC fontDesc;
	fontDesc.Height = 24;
	fontDesc.Width = 0;
	fontDesc.Weight = 0;
	fontDesc.MipLevels = 1;
	fontDesc.Italic = false;
	fontDesc.CharSet = DEFAULT_CHARSET;
	fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontDesc.Quality = DEFAULT_QUALITY;
	fontDesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	wcscpy_s(fontDesc.FaceName, L"Times New Roman");

	hr = D3DX10CreateFontIndirect(gpDevice, &fontDesc, &gpFont);
	if(FAILED(hr)) return hr;
	return true;
}

// Every object drawn in this game is a square, so we can create
// one Vertex Buffer and re-used it for every object drawn
bool Direct3D::InitializeVertexBuffer()
{
	Vertex verts[] =	// 0 1
	{					// 2 3
		{D3DXVECTOR3(-0.5f, 0.5f, 0.0f),	D3DXVECTOR2(0.0f, 0.0f)},			
		{D3DXVECTOR3(0.5f, 0.5f, 0.0f),		D3DXVECTOR2(1.0f, 0.0f)},			
		{D3DXVECTOR3(-0.5f, -0.5f, 0.0f),	D3DXVECTOR2(0.0f, 1.0f)},			
		{D3DXVECTOR3(0.5f, -0.5f, 0.0f),	D3DXVECTOR2(1.0f, 1.0f)}
	};

	// Describe the vertex buffer we are creating
	D3D10_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * 4;
		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

	// Specify the data we are initializing the vertex buffer with
	D3D10_SUBRESOURCE_DATA vertexInitData;
		vertexInitData.pSysMem = verts;

	// Create vertex buffer
	hr = gpDevice->CreateBuffer(&vertexBufferDesc, &vertexInitData, &gpVertexBuffer);
	if(FAILED(hr)) return hr;

	// Bind vertex buffer to the pipeline
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	gpDevice->IASetVertexBuffers(0, 1, &gpVertexBuffer, &stride, &offset);
	return true;
}

bool Direct3D::InitializeIndexBuffer()
{
	UINT indices[6] = {0, 1, 2, 1, 3, 2};

	// Describe the index buffer we are creating
	D3D10_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage = D3D10_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		indexBufferDesc.ByteWidth = sizeof(UINT) * 6;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

	// Specify the data we are initializing the index buffer with
	D3D10_SUBRESOURCE_DATA indexInitData;
		indexInitData.pSysMem = indices;

	// Create the index buffer
	hr = gpDevice->CreateBuffer(&indexBufferDesc, &indexInitData, &gpIndexBuffer);
	if(FAILED(hr)) return hr;

	// Bind index buffer to the pipeline
	gpDevice->IASetIndexBuffer(gpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	return true;
}

void Direct3D::StartFrame(D3DXMATRIX view)
{
	// Clear background to black
	gpDevice->ClearRenderTargetView(gpRenderTargetView, D3DXCOLOR(0.392f, 0.584f, 0.929f, 0.0f));

	// Clear depth buffer
	gpDevice->ClearDepthStencilView(gpDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);

	// Update WVP matrices for rendering
//	gpShaderWorldMatrix->SetMatrix((float*)&world);
	gpShaderViewMatrix->SetMatrix((float*)&view);
	gpShaderProjectionMatrix->SetMatrix((float*)&ortho);

//	gpInfo->gpShaderWorldMatrix = gpShaderWorldMatrix;
//	gpInfo->gpShaderViewMatrix= gpShaderViewMatrix;
//	gpInfo->gpShaderProjectionMatrix = gpShaderProjectionMatrix;
}

// Reset drawing structure after DrawText alters it
void Direct3D::DrawTextReset()
{
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	gpDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	gpDevice->IASetInputLayout(gpLayout);
	gpDevice->IASetVertexBuffers(0, 1, &gpVertexBuffer, &stride, &offset);
	gpDevice->IASetIndexBuffer(gpIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Direct3D::EndFrame()
{
	if(vSyncEnabled)
		gpSwapChain->Present(1, 0);
	else
		gpSwapChain->Present(0, 0);
}

#pragma region Properties
ID3DX10Font* Direct3D::GetFont() { return gpFont; }
#pragma endregion
