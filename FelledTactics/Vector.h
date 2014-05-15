#pragma once
#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <assert.h>

#ifndef MATRIX_H
#include "Matrix.h"
#endif
#ifndef QUATERNION_H
#include "Quaternion.h"
#endif

class Matrix;
class Quaternion;

class Vector
{
public:
	float x,y,z;	// x, y and z components of the Vector

	// Constructors
	Vector();
	Vector(float components[]);
	Vector(float _x, float _y, float _z);
	Vector(const Vector& start, const Vector& end);
	Vector(const Vector& other);
	Vector& operator= (const Vector& other);

	// Destructor
	~Vector();

	// Operator Overloads
	float& operator[](int index);
	const float& operator[] (int index) const;
	Vector operator* (float scalar) const;
	Vector& operator*= (float scalar);
	Vector operator* (const Matrix& m) const;
	Vector& operator*= (const Matrix& m);
	Vector operator* (const Quaternion& q) const;
	Vector& operator*= (const Quaternion& q);
	Vector operator/ (float scalar) const;
	Vector& operator/= (float scalar);
	Vector operator+ (const Vector& other) const;
	Vector& operator+= (const Vector& other);
	Vector operator- (const Vector& other) const;
	Vector& operator-= (const Vector& other);
	float operator% (const Vector& other) const;
	Vector operator* (const Vector& other) const;
	float operator/ (const Vector& other) const;
	bool operator== (const Vector& other) const;
	bool operator!= (const Vector& other) const;
	operator float() const;

	// Functions
	Vector& Add(const Vector& other);
	Vector& Subtract(const Vector& other);
	Vector& Multiply(float scalar);
	Vector& Multiply(const Matrix& m);
	Vector& Divide(float scalar);
	Vector	Normalized() const;
	Vector& NormalizeInPlace();
	float	DotProduct(const Vector& other) const;
	Vector	CrossProduct(const Vector& other) const;
	Vector	Zero();
	bool	IsZero();
	Vector& Cleanse();

	// Static Functions
	static Vector VectorAddition(const Vector& v1, const Vector& v2);
	static Vector VectorSubtraction(const Vector& v1, const Vector& v2);
	static Vector VectorMatrixMultiplication(const Vector& v, const Matrix& m);
	static float VectorDotProduct(const Vector& v1, const Vector& v2);
	static Vector VectorCrossProduct(const Vector& v1, const Vector& v2);
	static float VectorMagnitude(const Vector& vec);
	static Vector VectorNormalized(const Vector& vec);
	static float AngleBetweenVectors(const Vector& v1, const Vector& v2);
	static Vector VectorProjection(const Vector& v1, const Vector& v2);
};

#endif