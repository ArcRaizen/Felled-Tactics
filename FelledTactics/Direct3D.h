#pragma once
#ifndef DIRECT3D_H
#define DIRECT3D_H

#ifndef CORE_H
#include "Core.h"
#endif

class Direct3D
{
public:
	Direct3D(void);
	~Direct3D(void);

	static void InitializeDirect3D(bool vsync, HWND hWnd, bool fScreen, float screenFar, float screenNear);
	static void StartFrame(D3DXMATRIX view);
	static void DrawTextReset();
	static void EndFrame();

	static ID3DX10Font* GetFont();

	static GraphicsInfo* gpInfo;
private:
	static bool DefineSwapChain(HWND hWnd);
	static bool CreateRenderTargetView();
	static bool CreateDepthStencilBuffer();
	static bool InitializeRasterizerState();
	static bool SetViewport(float screenNear, float screenFar);
	static bool InitializeShader(HWND hWnd);
	static bool InitializeBlendStates();
	static bool InitFonts();
	static bool InitializeVertexBuffer();
	static bool InitializeIndexBuffer();


	static ID3D10Device*				gpDevice;			// Chief Interface. Software controller of graphics device.
	static IDXGISwapChain*				gpSwapChain;		// Swap chain interface.
	static ID3D10Texture2D*				backBuffer;			// Back buffer of the Swap Chain
	static ID3D10RenderTargetView*		gpRenderTargetView;
	static ID3D10RasterizerState*		gpRasterizerState;

	// Shader input layout, effect and technique
	static ID3D10Effect*				gpEffect;
	static ID3D10EffectTechnique*		gpTechnique;
	static ID3D10InputLayout*			gpLayout;

	// Buffers
	static ID3D10Buffer*				gpVertexBuffer;
	static ID3D10Buffer*				gpIndexBuffer;

	// Effect variable pointers
	static ID3D10EffectMatrixVariable*				gpShaderWorldMatrix;
	static ID3D10EffectMatrixVariable*				gpShaderViewMatrix;
	static ID3D10EffectMatrixVariable*				gpShaderProjectionMatrix;
	static ID3D10EffectVectorVariable*				gpShaderHighlightColor;
	static ID3D10EffectShaderResourceVariable*		gpShaderTexture;

	static ID3D10ShaderResourceView*	gpShaderResourceView;
	static ID3D10Texture2D*				gpDepthStencilBuffer;	// Texture for depth buffer
	static ID3D10DepthStencilView*		gpDepthStencilView;
	static ID3D10DepthStencilState*		gpDepthStencilState;
	static ID3D10DepthStencilState*		gpDepthDisabledStencilState;

	static bool fullScreen;

	// Create the color that will be used to fill the backbuffer
	float ClearColor[4];// = {0.0f, 0.8f, 1.0f, 1.0f };
	float clearToAlpha[4];// = {0.0f, 0.0f, 0.0f, 0.0f };

	// Matrices
	static D3DXMATRIX projection;
	static D3DXMATRIX ortho;

	//Blendstates
	static ID3D10BlendState* blendNoBlend;
	static ID3D10BlendState* blendAlpha;
	static ID3D10BlendState* blendAdditive;
	static ID3D10BlendState* blendSubtract;

	// Camera vars
	static D3DXVECTOR3 camPos;
	static D3DXVECTOR3 targetPoint;
	static D3DXVECTOR3 worldUp;

	// Font
	static ID3DX10Font* gpFont;

	// V-Sync
	static bool vSyncEnabled;

	// Video Card Info
	static int videoCardMemory;
	static char videoCardDescription[128];
};
#endif
