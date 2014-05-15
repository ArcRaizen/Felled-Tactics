////=============================================================================
//// color.fx by Frank Luna (C) 2008 All Rights Reserved.
////
//// Transforms and colors geometry.
////=============================================================================
//
//
//
//void VS(float3 iPosL : POSITION,
//        float4 iColor : COLOR,
//        out float4 oPosH  : SV_POSITION,
//        out float4 oColor : COLOR)
//{
//	// Transform to homogeneous clip space.
//	oPosH = mul(float4(iPosL, 1.0f), gWVP);
//	
//	// Just pass vertex color into the pixel shader.
//    oColor = iColor;
//}
//
//float4 PS(float4 posH  : SV_POSITION,
//          float4 color : COLOR) : SV_Target
//{
//    return color;
//}

//--------------------------------------------------------------------------------------
// File: omgD10Triangle.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

Texture2D txDiffuse;
float2 uvScale = float2(1, 1);
float2 uvOffset = float2(0, 0);
float2 dir = float2(1.0f, 0.0f); //right
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbPerObject
{
	float4x4 gWVP; 
};

cbuffer cbNeverChanges
{
    //matrix View;
	float4 vMeshColor;
};

/*cbuffer cbChangeOnResize
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame
{
    matrix World;
};*/

struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct GS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, gWVP);
	
	output.Tex = input.Tex;

	output.Tex = input.Tex * uvScale;
	output.Tex = input.Tex * uvScale +uvOffset;
	if(dir.x<0.0f)
	{
		output.Tex.x = 1 - output.Tex.x;
	}
    return output;
}


//[maxvertexcount(4)]
//void GS( point GS_INPUT point[1], inout TriangleStream<PS_INPUT> triStream )
//{
//	PS_INPUT v;
//	triStream.Append(v);
//}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex ) * float4(1.0f, 1.0f, 1.0f, 1.0f);    // White, with Alpha = 1
}


technique10 ColorTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
