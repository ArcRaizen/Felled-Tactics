#pragma once
#ifndef CORE_H
#define CORE_H

#include <math.h>
#include <cmath>
#include <time.h>
#include <sstream>
#include <iostream>
#include <assert.h>
#include <D3D10.h>
#include <D3DX10.h>

extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
//#include "Luna.h"
//#include "json_spirit.h"
using namespace std;

#define PI		3.141592653589793238462
#define DEG2RAD PI / 180
#define RAD2DEG 180 / PI
#define CLAMP(value, lo, hi) (val < lo ? (lo) : (val > hi ? (hi) : (val)))

#pragma region DEV BUILD
#define DEBUG
//#define LOCK_FRAMERATE
#define DRAW_FRAMERATE
#define ALLOW_ZERO_TILE_MOVEMENT
#pragma region

static UINT screenWidth = 1280;
static UINT screenHeight = 720;
static HRESULT hr;
static bool	CENTERED_ORIGIN = false;

// Structure to hold important pieces of the graphics pipeline
// We can create this structure once and pass around a pointer to it, rather than passing each part separately
struct GraphicsInfo
{
	ID3D10Device*						gpDevice;
	ID3D10EffectTechnique*				gpTechnique;
	ID3D10InputLayout*					gpLayout;
	ID3D10Buffer*						gpVertexBuffer;
	ID3D10EffectShaderResourceVariable* gpShaderTexture;
	ID3D10EffectMatrixVariable*			gpShaderWorldMatrix;
	ID3D10EffectMatrixVariable*			gpShaderViewMatrix;
	ID3D10EffectMatrixVariable*			gpShaderProjectionMatrix;
	ID3D10EffectVectorVariable*			gpShaderHighlightColor;
	D3DXVECTOR4							gpShaderDefaultColor;
	D3DXVECTOR2							gpShaderDefaultUVScale;
};

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
};
#endif