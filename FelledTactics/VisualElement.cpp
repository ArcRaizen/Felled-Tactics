#include "StdAfx.h"
#include "VisualElement.h"

D3DXVECTOR4 VisualElement::highlightNone = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
std::wstring VisualElement::PNG = L".png";

// PosX and PosY are pixel-coordinates in the game with (0,0) being the bottom-left corner
VisualElement::VisualElement(WCHAR* filename, int layer, int width, int height, int posX, int posY) : mouseEntered(false), mouseDown(false), texture(0),
	drawEnabled(true), deleted(false)
{
	// Initialize values
	highlightColor.x = highlightColor.y = highlightColor.z = highlightColor.w = 1.0f;
	uvScale.x = uvScale.y = 1.0f;

	// Save screen and visual element size, initialize previous position
	this->layer = layer;
	if(width == height)
		tileSize = width;
	else
		tileSize = -1;

	// Load Texture
	hr = D3DX10CreateShaderResourceViewFromFile(Direct3D::gpInfo->gpDevice, filename, 0, 0, &texture, 0);

	// Set initial left corner
	leftCorner.x = posX;
	leftCorner.y = posY;

	// Initialize world matrix
	D3DXMatrixIdentity(&world);
	D3DXMATRIX translation, scale;
	D3DXMatrixScaling(&scale, width, height, 0);
	D3DXMatrixTranslation(&translation, ConvertCoordinatesX(posX, width), ConvertCoordinatesY(posY, height), 0);
	world = world * scale;
	world = world * translation;
	CalcRect();
}

VisualElement::VisualElement(){}

VisualElement::~VisualElement(void)
{
	// Release texture
	if(texture)
		texture->Release();
	texture = 0;
}

// Translate Visual Element
void VisualElement::Translate(D3DXVECTOR3 t)
{
	D3DXMATRIX translate;
	D3DXMatrixTranslation(&translate, t.x, t.y, t.z);
	world *= translate;

	// Update LeftCorner and Rect
	leftCorner.x = world._41 - (int)(world._11/2) + (int)(screenWidth/2);		// DirectXPos.x - width/2 - screenWidth/2
	leftCorner.y = world._42 - (int)(world._22/2) + (int)(screenHeight/2);		// DirectXPos.y - height/2 - screenHeight/2
	CalcRect();
}

// Rotate Visual Element
void VisualElement::Rotate(float z)
{
	D3DXMATRIX rotate;
	D3DXMatrixRotationZ(&rotate, z);
	world *= rotate;
}

// Scale Visual Element
void VisualElement::Scale(D3DXVECTOR3 s)
{
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, s.x, s.y, s.z);
	world *= scale;
}
void VisualElement::SetPosition(D3DXVECTOR3 p)
{
}

// Calculate the rectangle encompasing this element in screen coordinates (mostly used for drawing text)
void VisualElement::CalcRect()	// get rekt, scrub
{
	rect.left = leftCorner.x;
	rect.right = rect.left + Width();
	rect.bottom = screenHeight - leftCorner.y;
	rect.top = rect.bottom - Height();
}

// Although rect is mostly used for drawing text, it is updated and maintained for all VisualElements
// Therefore, we can use it for overlap and collision detection calculations with other RECTS
bool VisualElement::RectCollide(const RECT& r)
{
	if(r.left > rect.right) return false;
	if(r.right < rect.left) return false;
	if(r.top > rect.bottom) return false;
	if(r.bottom < rect.top ) return false;

	return true;
}

// Is a point in the world contained inside this visual element?
bool VisualElement::IsPointContained(D3DXVECTOR3 pos)
{
	/*wchar_t msg[50];
	swprintf(msg, L"x: %.2f\n"
		L"y: %.2f\n", pos.x, pos.y);
	MessageBox(0, msg, L"Mouse Position", MB_OK);*/

	if(pos.x < (world._41 - (world._11 / 2)))
		return false;
	if(pos.x > (world._41 + (world._11 / 2)))
		return false;
	if(pos.y < (world._42 - (world._22 / 2)))
		return false;
	if(pos.y > (world._42 + (world._22 / 2)))
		return false;

	return true;
}

// Decide logic for MouseOver and MouseOut based on the position of the mouse relative to this visual element
void VisualElement::SetCurrentMousePosition(D3DXVECTOR3 mousePosition)
{
	bool mouseContained = IsPointContained(mousePosition);

	// If mouse is over this element for the first time this frame
	if(!mouseEntered && mouseContained)
	{
		MouseOver();
		mouseEntered = true;
	}
	else if(mouseEntered && !mouseContained) // or if it just left this frame
	{
		MouseOut();
		mouseEntered = false;
	}
}

#pragma region MouseEvents
void VisualElement::MouseDown() { mouseDown = true; }
void VisualElement::MouseUp() { if(mouseDown) mouseDown = false; }
void VisualElement::MouseClick(){}
void VisualElement::MouseOver() { mouseEntered = true;}
void VisualElement::MouseOut() { mouseEntered = false; mouseDown = false;}
#pragma endregion

int VisualElement::Update(float dt)
{
	return 0;
}

bool VisualElement::Draw()
{
	assert(texture);

	// Set world matrix
	Direct3D::gpInfo->gpShaderWorldMatrix->SetMatrix((float*)world);

	Direct3D::gpInfo->gpDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set highlight color for sprite (if any)
	if(highlightColor != Direct3D::gpInfo->gpShaderDefaultColor)
		Direct3D::gpInfo->gpShaderHighlightColor->SetFloatVector((float*)highlightColor);


	// Bind texture to pipeline to draw
	Direct3D::gpInfo->gpShaderTexture->SetResource(texture);

	//D3D10_TECHNIQUE_DESC techniqueDesc;
	Direct3D::gpInfo->gpDevice->IASetInputLayout(Direct3D::gpInfo->gpLayout);
	//gpInfo->gpTechnique->GetDesc(&techniqueDesc);
	//for(int i = 0; i < techniqueDesc.Passes; ++i)
	//{
		Direct3D::gpInfo->gpTechnique->GetPassByIndex(0/*i*/)->Apply(0);
		Direct3D::gpInfo->gpDevice->DrawIndexed(6, 0, 0);
	//}

	// Reset default Shader Highlight Color and UV Scale for next object
	if(highlightColor != Direct3D::gpInfo->gpShaderDefaultColor)
		Direct3D::gpInfo->gpShaderHighlightColor->SetFloatVector((float*)Direct3D::gpInfo->gpShaderDefaultColor);

	return true;
}

void VisualElement::SetTexture(std::wstring filename)
{
	// Load Texture
	hr = D3DX10CreateShaderResourceViewFromFile(Direct3D::gpInfo->gpDevice, filename.c_str(), 0, 0, &texture, 0);
}

// Turn off this element drawing by making it invisible
void VisualElement::DisableDraw()
{
	drawEnabled = false;
	alpha = highlightColor.w;
	highlightColor.w = 0.0f;
}

// Restore original visibility to the element
void VisualElement::EnableDraw()
{
	drawEnabled = true;
	highlightColor.w = alpha;
}

void VisualElement::Delete()
{
	deleted = true;
}

#pragma region Properties
int  VisualElement::GetLayer() { return layer; }
void VisualElement::SetLayer(int l) { layer = l; }
bool VisualElement::GetMouseDown() { return mouseDown; }
bool VisualElement::GetMouseEntered() { return mouseEntered; }
//int  VisualElement::GetWidth() { return width; }
//void VisualElement::SetWidth(int w) { width = w; }
//int  VisualElement::GetHeight() { return height; }
//void VisualElement::SetHeight(int h) { height = h; }
Position VisualElement::GetCorner() { return leftCorner; }
void VisualElement::SetCorner(Position p) { leftCorner = p; }
bool VisualElement::GetEnabled() { return drawEnabled; }
void VisualElement::SetEnabled(bool e) { drawEnabled = e; }
ID3D10ShaderResourceView* VisualElement::GetTexture() { return texture; }
Position VisualElement::GetWorldPosition() { return Position(world._41, world._42); }
RECT VisualElement::GetRect() { return rect; }
#pragma endregion

#pragma region Coordinates
/*
	LEFTCORNER - IN GAME COORDINATE DEFINED BY ME
		Used to position elements in the game

0, height										width, height
	+---------------------------------------------+
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	+---------------------------------------------+
  0,0                                             width, 0

				Origin:	bottom left corner
				Positive: right, up

	DIRECTX - COORDINATES USED BY DIRECTX
			Used to draw elements on screen
												width/2, height/2
	+---------------------------------------------+
	|                     |                       |
	|                     |                       |
	|                     |                       |
	|                     |                       |
	|                     |                       |
	|---------------------+-----------------------|
	|                     | \                     |
	|                     |  \                    |
	|                     |   0,0                 |
	|                     |                       |
	|                     |                       |
	+---------------------------------------------+
-width/2, -height/2

				Origin: center
				Positive: right, up
		

	RECT - SCREEN COORDINATES
		Used for mouse position and drawing directly on the screen (text)

0,0
	+---------------------------------------------+
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	|                                             |
	+---------------------------------------------+
													width/height

				Origin: top left corner
				Positive: right, down
*/
#pragma endregion