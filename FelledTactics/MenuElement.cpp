#include "StdAfx.h"
#include "MenuElement.h"


MenuElement::MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, Level* l, void (Level::*func)(), const char* t/*=""*/) : 
	VisualElement(filename, layer, width, height, posX, posY)
{
	level = l;
	text = t;
	parameter = -1;
	mbstowcs(lpcwText, t, 20);
	activateFunction = func;
	fontColor.r = fontColor.g = fontColor.b = 0.0f;
	fontColor.a = 1.0f;

	InitializeBuffer();
}

MenuElement::MenuElement(WCHAR* filename, int layer, int width, int height, int posX, int posY, Level* l, void (Level::*func)(int), int val, const char* t/*=""*/) : 
	VisualElement(filename, layer, width, height, posX, posY)
{
	level = l;
	text = t;
	parameter = val;
	mbstowcs(lpcwText, t, 20);
	activateFunctionParameter = func;
	fontColor.r = fontColor.g = fontColor.b = 0.0f;
	fontColor.a = 1.0f;

	InitializeBuffer();
}

MenuElement::MenuElement(void)
{
	level = NULL;
	vertexBuffer->Release();
	vertexBuffer = 0;
}


MenuElement::~MenuElement(void)
{
}

void MenuElement::MouseDown(){ mouseDown = true; }
void MenuElement::MouseUp(){ if(mouseDown) { mouseDown = false; Activate(); } }
void MenuElement::MouseOver(){ mouseEntered = true; }
void MenuElement::MouseOut(){ mouseEntered = false; }

void MenuElement::Activate()
{
	if(level != NULL)
	{
		if(parameter != -1)
			((level)->*(activateFunctionParameter))(parameter);
		else
			((level)->*(activateFunction))();
	}
}

bool MenuElement::Draw()
{
	//VisualElement::Draw();

	if(drawEnabled)
	{
		D3DXMATRIX m;
		D3DXMatrixIdentity(&m);
		Direct3D::gpInfo->gpShaderWorldMatrix->SetMatrix((float*)&m);
		Direct3D::gpInfo->gpDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		Direct3D::gpInfo->gpDevice->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		Direct3D::gpInfo->gpShaderTexture->SetResource(texture);

		// Draw
		Direct3D::gpInfo->gpDevice->IASetInputLayout(Direct3D::gpInfo->gpLayout);
		Direct3D::gpInfo->gpTechnique->GetPassByIndex(0)->Apply(0);
		Direct3D::gpInfo->gpDevice->DrawIndexed(6, 0, 0);

		// Reset default Vertex Buffer
		Direct3D::gpInfo->gpDevice->IASetVertexBuffers(0, 1, &Direct3D::gpInfo->gpVertexBuffer, &stride, &offset);

		// If we have unique text to draw, do so
		if(strcmp(text, ""))
			Direct3D::GetFont()->DrawText(NULL, lpcwText, -1, &rect, DT_CENTER | DT_VCENTER, fontColor);
	}
	return true;
}

bool MenuElement::InitializeBuffer()
{
	Vertex* verts = new Vertex[4];
	memset(verts, 0, sizeof(Vertex) * 4);

	// Describve the vertex buffer we are creating
	D3D10_BUFFER_DESC vBufferDesc;
		ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));
		vBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vBufferDesc.ByteWidth = sizeof(Vertex) * 4;
		vBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vBufferDesc.MiscFlags = 0;

	// Specify the data we are initializing the vertex buffer with
	D3D10_SUBRESOURCE_DATA vInitData;
		vInitData.pSysMem = verts;

	// Create vertex buffer
	hr = Direct3D::gpInfo->gpDevice->CreateBuffer(&vBufferDesc, &vInitData, &vertexBuffer);
	if(FAILED(hr)) return hr;

	delete [] verts;
	verts = 0;
	return true;
}


bool MenuElement::UpdateBuffer(float lOffset, float rOffset, float tOffset, float bOffset)
{
	float left, right, top, bottom;		// Borders of the element
	void* vertsPtr = 0;

	if(CENTERED_ORIGIN) {}
	else
	{
		left = Left() + lOffset;
		right = Right() - rOffset;
		top = Top() - tOffset;
		bottom = Bottom() + bOffset;
	}

	
	Vertex verts[] = 
	{
		{D3DXVECTOR3(left, top, 0.0f),		D3DXVECTOR2(lOffset/Width(), tOffset/Height())},
		{D3DXVECTOR3(right, top, 0.0f),		D3DXVECTOR2(1.0f - rOffset/Width(), tOffset/Height())},
		{D3DXVECTOR3(left, bottom, 0.0f),	D3DXVECTOR2(lOffset/Width(), 1.0f - bOffset/Height())},
		{D3DXVECTOR3(right, bottom, 0.0f),	D3DXVECTOR2(1.0f - rOffset/Width(), 1.0f - bOffset/Height())},
	};

	// Set new updated vertex buffer
	hr = vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertsPtr);
	if(FAILED(hr)) return hr;
	memcpy(vertsPtr, (void*)verts, sizeof(Vertex) * 4);
	vertexBuffer->Unmap();

	return true;
}