#pragma once
#ifndef QUATERNION_H
#define QUATERNION_H

#include <math.h>
#include <assert.h>

#ifndef MATRIX_H
#include "Matrix.h"
#endif
#ifndef VECTOR_H
#include "Vector.h"
#endif

#define PI	3.14159
#define CLAMP(val, high, low) ((val < low) ? low : ((val > high) ? high : val))

// Forward Declaration
class Matrix;
class Vector;

class Quaternion
{
public:
	float x, y, z, w;	//x, y and z are the imaginary components and w is the real component of the Quaternion

	// Constructors
	Quaternion();
	Quaternion(float components[]);
	Quaternion(float _x, float _y, float _z, float _w);
	Quaternion(float phi, float theta, float psi);
	Quaternion(const Vector& startVec, const Vector& endVec);
	Quaternion(const Vector& v, float angle);
	Quaternion(const Quaternion& quat);
	Quaternion& operator=(const Quaternion& other);

	// Destructor
	~Quaternion();

	// Operator Overloads
	float& operator[] (int index);
	const float& operator[] (int index) const;
	Quaternion operator+ (const Quaternion& other) const;
	Quaternion& operator+= (const Quaternion& other);
	Quaternion operator- (const Quaternion& other) const;
	Quaternion& operator-= (const Quaternion& other);
	Quaternion operator* (const Quaternion& other) const;
	Quaternion& operator*= (const Quaternion& other);
	Quaternion operator* (float scalar) const;
	Quaternion& operator*= (float scalar);
	Quaternion& operator~();
	Quaternion& operator-();
	float operator% (const Quaternion& other) const;
	bool operator== (const Quaternion& other) const;
	bool operator!= (const Quaternion& other) const;
	operator float() const;

	// Functions
	Quaternion& Add(const Quaternion& other);
	Quaternion& Subtract(const Quaternion& other);
	Quaternion& Multiply(const Quaternion& other);
	Quaternion& Scale(float scalar);
	Quaternion	Conjugated() const;
	Quaternion&	ConjugateInPlace();
	Quaternion	Inversed() const;
	Quaternion& InvertInPlace();
	Quaternion	Negated() const;
	Quaternion& NegateInPlace();
	Quaternion	Normalized() const;
	Quaternion& NormalizeInPlace();
	float		DotProduct(const Quaternion& other) const;
	float		Magnitude() const;
	Matrix		ConvertToMatrix() const;	
	float*		EulerAngles() const;
	void		AxisAngle(Vector& v, float& angle);
	Quaternion& Identity();
	bool		IsIdentity();

	// Static Functions
	static Quaternion QuaternionAddition(const Quaternion& q1, const Quaternion& q2);
	static Quaternion QuaternionSubtraction(const Quaternion& q1, const Quaternion& q2);
	static Quaternion QuaternionMultiplication(const Quaternion& q1, const Quaternion& q2);
	static Quaternion QuaternionScale(const Quaternion& q, float scalar);
	static Quaternion QuaternionNegate(const Quaternion& q);
	static Quaternion QuaternionConjugate(const Quaternion& q);
	static Matrix QuaternionToMatrix(const Quaternion& q);
	static float* QuaternionEulerAngles(Quaternion& q);
	static Quaternion QuaternionFromTwoVectors(const Vector& startVec, const Vector& endVec);
	static Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t);
};

#endif