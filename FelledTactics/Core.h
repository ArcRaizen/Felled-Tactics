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

#ifndef SMARTPOINTER_H
#include "SmartPointer.h"
#endif

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
#ifdef _DEBUG
	#define DEV_DEBUG


	//#define PERSPECTIVE_PROJECTION
	//#define LOCK_FRAMERATE
	#define DRAW_FRAMERATE

	#ifdef DEV_DEBUG
		// ~~ Time keeping and speed test tools ~~
		#include <chrono>
		using namespace std::chrono;
		//high_resolution_clock::time_point t1 = high_resolution_clock::now();
		//high_resolution_clock::time_point t2 = high_resolution_clock::now();
		//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();

		// ~~ GOD MODE DEFINES FOR TESTING ~~
		#define GOD_MODE_ALLY
		//#define GOD_MODE_ENEMY
	#endif
#endif
#pragma region

static UINT screenWidth = 1280;
static UINT screenHeight = 720;
static HRESULT hr;
static bool	CENTERED_ORIGIN = false;
static const std::string ABILITY_JSON = "../FelledTactics/Abilities.json";
static const std::string UNIT_JSON = "../FelledTactics/Saves/Units.json";
static const std::string SAVE_JSON = "../FelledTactics/Saves/Save1.json";


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
	D3DXMATRIX							gpProjectionMatrix;
};

struct Vertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
};
#endif