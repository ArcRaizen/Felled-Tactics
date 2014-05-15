#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <D3DX10math.h>
#ifndef CORE_H
#include "Core.h"
#endif

class Camera
{
public:
	Camera(void);
	~Camera(void);

	static void Initialize();
	static void Update();
	static void CopyViewMatrix(D3DXMATRIX& vm);

	static void SetPosition(float x, float y, float z);	static D3DXVECTOR3 GetPosition();
	static void SetRotation(float x, float y, float z);	static D3DXVECTOR3 GetRotation();
														static D3DXMATRIX  GetViewMatrix();

private:
	static float positionX, positionY, positionZ;
	static float rotationX, rotationY, rotationZ;
	static D3DXMATRIX viewMatrix;
};
#endif
