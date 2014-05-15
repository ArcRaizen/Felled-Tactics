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
public:
	VisualElement(WCHAR* filename, int layer, int width, int height, int posX, int posY);
	VisualElement();
	~VisualElement(void);

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

	__declspec(property(put=SetLayer, get=GetLayer)) int Layer;			void SetLayer(int l);			int GetLayer();
	__declspec(property(get=GetMouseDown)) bool IsMouseDown;											bool GetMouseDown();
	__declspec(property(get=GetMouseEntered)) bool MouseEntered;										bool GetMouseEntered();
	__declspec(property(put=SetWidth, get=GetWidth)) int Width;			void SetWidth(int w);			int GetWidth();
	__declspec(property(put=SetHeight, get=GetHeight)) int Height;		void SetHeight(int h);			int GetHeight();
	__declspec(property(put=SetCenter, get=GetCenter)) Position Center;	void SetCenter(Position p);		Position GetCenter();
	__declspec(property(put=SetCorner, get=GetCorner)) Position Corner;	void SetCorner(Position p);		Position GetCorner();
	__declspec(property(get=GetTexture)) ID3D10ShaderResourceView* Texture;								ID3D10ShaderResourceView* GetTexture();

protected:
	void	Translate(D3DXVECTOR3 t);
	void	Rotate(float z);
	void	Scale(D3DXVECTOR3 s);
	void	SetPosition(D3DXVECTOR3 p);

	ID3D10ShaderResourceView*	texture;		// Texture for this Visual Element
	D3DXMATRIX					world;			// World matrix for this object !!!!

	int			layer;		// Depth layer for any visual element. Used for ordering of drawing objects. 
							//	The higher the layer the closer to the screen. The lower the layer the farther back
	bool		mouseEntered, mouseDown;
	int			width, height, tileSize;
	Position	leftCorner;
	Position	center;

	D3DXVECTOR4	highlightColor;
	D3DXVECTOR2 uvScale;
};
#endif
