#pragma once
#ifndef MATRIX_H
#define MATRIX_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef QUATERNION_H
#include "Quaternion.h"
#endif
#ifndef VECTOR_H
#include "Vector.h"
#endif

#define DEG2RAD 3.14159265f/180.0f
#define RAD2DEG 180.0f/3.14159f
#define MAX(X, Y) (X >= Y ? X : Y)

// Forward Declaration
class Quaternion;
class Vector;

class Matrix
{
public:

	// Constructors
	Matrix();
	Matrix(float components[]);
	Matrix(float mat[][4]);
	Matrix(const Quaternion& q);
	Matrix(float phi, float theta, float psi);
	Matrix& operator= (const Matrix& mat);
	
	// Destructor
	~Matrix();
	
	// Operator Overloads
	float (&operator[] (int index))[4];
	const float* operator[] (int index) const;
	Matrix	operator+ (const Matrix& mat) const;
	Matrix& operator+= (const Matrix& mat);
	Matrix	operator- (const Matrix& mat) const;
	Matrix& operator-= (const Matrix& mat);
	Matrix	operator* (float scalar) const;
	Matrix&	operator*= (float scalar);
	Matrix	operator* (const Matrix& mat) const;
	Matrix&	operator*= (const Matrix& mat);
	Matrix& operator- ();
	bool	operator== (const Matrix& mat) const;
	bool	operator!= (const Matrix& mat) const;

	// Functions
	float (&GetArray())[16];
	Matrix& Identity();
	bool	IsIdentity();
	float	Determinent() const;
	Matrix& InvertEuclidean();
	Matrix& InvertAffine();
	Matrix& InvertGeneral();
	Matrix  Inverse3() const;
	Matrix	Inverse4() const;
	Matrix	InverseRotation() const;
	Matrix	InverseTranslation() const;
	Matrix	InverseScale() const;
	Matrix	Transposed() const;
	Matrix& TransposeInPlace();
	float	GetCofactor(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8) const;
	float*	EulerAngles() const;
	Quaternion ConvertToQuaternion() const;

	// Transformations
	Matrix& Translate(const Vector& v);
	Matrix&	Translate(float x, float y, float z);
	Matrix& Scale(float scale);
	Matrix& Scale(float sx, float sy, float sz);
	Matrix& Rotate(float angle, const Vector& axis);
	Matrix& Rotate(float angle, float x, float y, float z);
	Matrix&	RotateX(float angle);
	Matrix& RotateY(float angle);
	Matrix& RotateZ(float angle);
	Matrix& RotateArbitraryAxis(float angle, const Vector& start, const Vector& end);
	Matrix& ShearXY(float shx, float shy);
	Matrix& ShearYZ(float shy, float shz);
	Matrix& ShearZX(float shx, float shz);

	// Static Functions
	static Matrix MatrixAddition(const Matrix& m1, const Matrix& m2);
	static Matrix MatrixSubtraction(const Matrix& m1, const Matrix& m2);
	static Matrix MatrixScale(const Matrix& m1, float scalar);
	static Matrix MatrixMultiplication(const Matrix& m1, const Matrix& m2);
	static Quaternion MatrixToQuaternion(const Matrix& m);
	static Matrix MatrixFromXAxisRotation(float angle);
	static Matrix MatrixFromYAxisRotation(float angle);
	static Matrix MatrixFromZAxisRotation(float angle);
	static Matrix MatrixFromRotationAboutOrigin(float angle, float x, float y, float z);
	static Matrix MatrixFromRotationAboutOrigin(float angle, const Vector* vec);
	static Matrix MatrixFromArbitraryAxisRotation(float angle, const Vector& start, const Vector& end);
	static Matrix LookAt(Vector eye, Vector target, Vector up, bool lh);
	static Matrix Perspective(float fieldOfView, float aspect, float zNear, float zFar);
	static Matrix Ortho(float width, float height, float zNear, float zFar);

private:
	float matArray[16];
	float matrix[4][4];


	void SetArray();
	void SetMultiArray();
};

#endif