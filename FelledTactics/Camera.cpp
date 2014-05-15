#include "StdAfx.h"
#include "Camera.h"

float Camera::positionX = 0;
float Camera::positionY = 0;
float Camera::positionZ = 0;
float Camera::rotationX = 0;
float Camera::rotationY = 0;
float Camera::rotationZ = 0;
D3DXMATRIX Camera::viewMatrix;

Camera::Camera(void)
{
}


Camera::~Camera(void)
{
}

void Camera::Initialize()
{
	positionX = positionY = positionZ = 0;
	rotationX = rotationY = rotationZ = 0;
}

void Camera::Update()
{
	D3DXVECTOR3 up (0.0f, 1.0f, 0.0f); 
	D3DXVECTOR3 position(positionX, positionY, positionZ);
	D3DXVECTOR3 lookAt(0.0f, 0.0f, 1.0f);
	float pitch = rotationX * (PI / 180);
	float yaw = rotationY * (PI / 180);
	float roll = rotationZ * (PI / 180);
	D3DXMATRIX rotationMatrix;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);
	lookAt = position + lookAt;
	D3DXMatrixLookAtLH(&viewMatrix, &position, &lookAt, &up);
}

void Camera::SetPosition(float x, float y, float z) { positionX = x; positionY = y; positionZ = z; }
void Camera::SetRotation(float x, float y, float z) { rotationX = x; rotationY = y; rotationZ = z; }
D3DXVECTOR3 Camera::GetPosition() { return D3DXVECTOR3(positionX, positionY, positionZ); }
D3DXVECTOR3 Camera::GetRotation() { return D3DXVECTOR3(rotationX, rotationY, rotationZ); }
D3DXMATRIX  Camera::GetViewMatrix()  { return viewMatrix; }
void Camera::CopyViewMatrix(D3DXMATRIX& vm)	{ vm = viewMatrix; }