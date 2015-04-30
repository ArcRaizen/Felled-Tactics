/*
	Class that represents anything that is to be drawn on the screen.
	Manages the elements needed to draw an object, its layer and how it is drawn.
*/
#pragma once
#ifndef VISUAL_H
#define VISUAL_H

#ifndef DIRECT3D_H
#include "Direct3D.h"
#endif
#ifndef CORE_H
#include "Core.h"
#endif
#include "Position.h"

class VisualElement
{
protected:
	VisualElement(WCHAR* filename, int layer, int width, int height, int posX, int posY);
	VisualElement();

public:
	static SmartPointer<VisualElement> Create(WCHAR* filename, int layer, int width, int height, int posX, int posY);
	static SmartPointer<VisualElement> Create();
	virtual ~VisualElement(void);

	virtual enum FrameState {IDLE};

	virtual bool	IsPointContained(D3DXVECTOR3 pos);
	virtual void	SetCurrentMousePosition(D3DXVECTOR3 mousePos);
	virtual void	MouseDown();
	virtual void	MouseUp();
	virtual void	MouseClick();
	virtual void	MouseOver();
	virtual void	MouseOut();
	virtual int		Update(float dt);
	virtual bool	Draw();

	void			SetTexture(std::wstring filename);
	virtual void	DisableDraw();
	virtual void	EnableDraw();
	void			Delete();

	__declspec(property(put=SetLayer, get=GetLayer)) int Layer;				void SetLayer(int l);			int GetLayer();
	__declspec(property(get=GetMouseDown)) bool IsMouseDown;												bool GetMouseDown();
	__declspec(property(get=GetMouseEntered)) bool MouseEntered;											bool GetMouseEntered();
	__declspec(property(put=SetCorner, get=GetCorner)) Position Corner;		void SetCorner(Position p);		Position GetCorner();
	__declspec(property(put=SetEnabled, get=GetEnabled)) bool DrawEnabled;	void SetEnabled(bool e);		bool GetEnabled();
	__declspec(property(get=GetTexture)) ID3D10ShaderResourceView* Texture;									ID3D10ShaderResourceView* GetTexture();
																											Position GetWorldPosition();
																											RECT GetRect();

	bool		deleted;	// Mark element for deletion

protected:
	void	Translate(D3DXVECTOR3 t);
	void	Rotate(float z);
	void	Scale(D3DXVECTOR3 s);
	void	SetPosition(D3DXVECTOR3 p);
	void	CalcRect();
	bool	RectCollide(const RECT& r);

	template <typename T> friend class SmartPointer;
	unsigned int				pointerCount;	// Running count of copies of a pointer of this class. Used in conjunction with SmartPointer


	ID3D10ShaderResourceView*	texture;		// Texture for this Visual Element
	D3DXMATRIX					world;			// World matrix for this object AT ITS CENTER!

	int			layer;		// Depth layer for any visual element. Used for ordering of drawing objects. 
							//	The higher the layer the closer to the screen. The lower the layer the farther back
	bool		mouseEntered, mouseDown;
	int			tileSize;
	RECT		rect;			// Rectangle representing position on screen. Mostly used for drawing text
	Position	leftCorner;		// Position according to me. Reference point is lower-left corner of a Visual Element in the world with (0,0) being the lower-left corner of the screen
	bool		drawEnabled;	// Is this element being draw?
	float		alpha;			// space to hold alpha of element when disabling/enabling it

	D3DXVECTOR4	highlightColor;
	D3DXVECTOR2 uvScale;

	static D3DXVECTOR4 highlightNone;
	static std::wstring PNG;			// String representation of ".png" (cached for ease later)

	// Convert between in-game coordinates (Origin at bottom-left corner, X postive right, Y positive Up)
	// and DirectX drawing coordinates
	inline int ConvertCoordinatesX(int posX, int width)
	{
		return posX - (int)screenWidth/2 + width/2;
	}
	inline int ConvertCoordinatesY(int posY, int height)
	{
		return posY - (int)screenHeight/2 + height/2;
	}
	inline int ReverseConvertCoordinatesX(int posX)
	{
		return posX + (int)screenWidth/2 - (int)(world._11/2);
	}
	inline int ReverseConvertCoordinatesY(int posY)
	{
		return posY + (int)screenHeight/2 - (int)(world._22/2);
	}

	// Quick inlines for element dimensions and position (in DirectX coords) since variables for width/height were removed
	inline float Width()
	{
		return world._11;
	}
	inline float Height()
	{
		return world._22;
	}
	inline float Left()
	{
		return world._41 - (world._11 / 2);
	}
	inline float Right()
	{
		return world._41 + (world._11 / 2);
	}
	inline float Top()
	{
		return world._42 + (world._22 / 2);
	}
	inline float Bottom()
	{
		return world._42 - (world._22 / 2);
	}
};

inline SmartPointer<VisualElement> VisualElement::Create(WCHAR* filename, int layer, int width, int height, int posX, int posY)
{
	return new VisualElement(filename, layer, width, height, posX, posY);
}
inline SmartPointer<VisualElement> VisualElement::Create() { return new VisualElement(); }
typedef SmartPointer<VisualElement> VisualElementPtr;
#endif
