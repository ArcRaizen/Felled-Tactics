#include "stdafx.h"
#include "Vector.h"


// Default constructor
Vector::Vector()
{
	x = y = z = 0.0f;
}

// Constructor from list of coordinates
Vector::Vector(float components[])
{		
	// Set the vector array to the passed in array	
	x = components[0];
	y = components[1];
	z = components[2];

}

// Constructor from each component
Vector::Vector(float _x, float _y, float _z): x(_x), y(_y), z(_z) {}

// Constructor from 2 Vectors
Vector::Vector(const Vector& start, const Vector& end)
{
	x = end[0] - start[0];
	y = end[1] - start[1];
	z = end[2] - start[2];
}

// Copy-Constructor
Vector::Vector(const Vector& other)
{
	//cout << "copy-constructor called" << endl;
	*this = other;
}

//Operator= overload
Vector& Vector::operator= (const Vector& other)
{
	//cout << "operator= called" << endl;
	if(this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	}

	return *this;
}

// Destructor
Vector::~Vector() {}

#pragma region Operator Overloads
float& Vector::operator[] (int index)
{
	assert(index > -1 && index < 3);

	if(index == 0)
		return x;
	else if(index == 1)
		return y;
	else if(index == 2)
		return z;
}

const float& Vector::operator[] (int index) const
{
	assert(index > -1 && index < 3);

	if(index == 0)
		return x;
	else if(index == 1)
		return y;
	else if(index == 2)
		return z;
}

// Vector Scalar Multiplication
Vector Vector::operator* (float scalar) const
{
	return Vector(x * scalar, y * scalar, z * scalar);
}
Vector& Vector::operator*= (float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;

	return *this;
}

// Vector Matrix Multiplication 
Vector Vector::operator* (const Matrix& m) const
{
	float a, b, c;

	a = m[0][0] * x + m[0][1] * y + m[0][2] * z + m[0][3];
	b = m[1][0] * x + m[1][1] * y + m[1][2] * z + m[1][3];
	c = m[2][0] * x + m[2][1] * y + m[2][2] * z + m[2][3];

	return Vector(a, b, c);
}
Vector& Vector::operator*= (const Matrix& m)
{
	float a=x, b=y, c=z;

	for(int i = 0; i < 3; i++)
		(*this)[i] = m[i][0] * a + m[i][1] * b + m[i][2] * c + m[i][3];

	return *this;
}

Vector Vector::operator* (const Quaternion& q) const
{
	Vector v = this->Normalized();
	Quaternion resQuat, vecQuat;
	vecQuat.x = v.x;
	vecQuat.y = v.y;
	vecQuat.z = v.z;
	vecQuat.w = 0.0f;

	resQuat = vecQuat * q.Conjugated();
	resQuat = q * resQuat;

	return Vector(resQuat.x, resQuat.y, resQuat.z);
}

Vector& Vector::operator*= (const Quaternion& q)
{
	Quaternion vectorQuat(x, y, z, 0.0f);		// Quaternion representation of this vector
	Quaternion result = (q.Normalized() * vectorQuat * q.Normalized().Conjugated());

	x = result.x;
	y = result.y;
	z = result.z;

	return *this;
}

// Vector Scalar Division
Vector Vector::operator/ (float scalar) const
{
	return Vector(x / scalar, y / scalar, z / scalar);
}
Vector& Vector::operator/= (float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;

	return *this;
}
	 
// Vector Addition
Vector Vector::operator+ (const Vector& other) const
{
	float a = x + other[0];
	float b = y + other[1];
	float c = z + other[2];
		
	return Vector(a, b, c);
}
Vector& Vector::operator+= (const Vector& other)
{
	x += other[0];
	y += other[1];
	z += other[2];


	return *this;
}

// Vector Subtraction (this - other)
Vector Vector::operator- (const Vector& other) const
{
	float a = x - other[0];
	float b = y - other[1];
	float c = z - other[2];
		
	return Vector(a, b, c);
}
Vector& Vector::operator-= (const Vector& other)
{
	x -= other[0];
	y -= other[1];
	z -= other[2];

	return *this;
}

// Vector Dot Product
float Vector::operator% (const Vector& other) const
{
	return x*other.x + y*other.y + z*other.z;
}

// Vector Cross Product
Vector Vector::operator* (const Vector& other) const
{
	float a = y*other.z - z*other.y;
	float b = z*other.x - x*other.z;
	float c = x*other.y - y*other.x;

	return Vector(a, b, c);
}

// Angle Between Vectors
float Vector::operator/ (const Vector& other) const
{
	float dot = *this % other;
	float mag1 = VectorMagnitude(*this);
	float mag2 = VectorMagnitude(other);
	float cosTheta = dot / (mag1 * mag2) * DEG2RAD;

	return acos(cosTheta);
}

// Vector Equality
bool Vector::operator== (const Vector& other) const
{
	return x == other.x && y == other.y && z == other.z;
}

bool Vector::operator!= (const Vector& other) const
{
	return !(*this == other);
}

// Vector Magnitude
Vector::operator float() const
{
	return sqrt(x*x + y*y + z*z);
}

#pragma endregion Operator Overloads

#pragma region Functions
// Return this Vector + other
Vector& Vector::Add(const Vector& other)
{
	return (*this += other);
}

// Return this Vector - other
Vector& Vector::Subtract(const Vector& other)
{
	return (*this -= other);
}

// Return this Vector * scalar	(Vector-Scalar Multiplication)
Vector& Vector::Multiply(float scalar)
{
	return (*this *= scalar);
}

// Return this Vector * m	(Vector-Matrix Multiplication)
Vector& Vector::Multiply(const Matrix& m)
{
	return (*this *= m);
}

// Return this Vector * scalar
Vector& Vector::Divide(float scalar)
{
	return (*this /= scalar);
}

// Return Normalized form of this Vector
Vector Vector::Normalized() const
{
	float mag = float(*this);

	float a = x / mag;
	float b = y / mag;
	float c = z / mag;

	if(mag == 0)
		return Vector(0,0,0);
	return Vector(a, b, c);
}

// Normalize and return this Vector
Vector& Vector::NormalizeInPlace()
{
	float mag = float(*this);

	x /= mag;
	y /= mag;
	z /= mag;

	if(mag == 0)
		x = y = z = 0;
	return *this;
}

// Return Dot Product of this Vector and other
float Vector::DotProduct(const Vector& other) const
{
	return (x*other.x + y*other.y + z*other.z);
}

// Return Cross Product of this Vector and other	(this cross other)
Vector Vector::CrossProduct(const Vector& other) const
{
	float a = y*other.z - z*other.y;
	float b = z*other.x - x*other.z;
	float c = x*other.y - y*other.x;

	return Vector(a, b, c);
}

// Set this vector to all 0's
Vector Vector::Zero()
{
	x = y = z = 0.0f;

	return (*this);
}

// Is this Vector a Zero vector?
bool Vector::IsZero()
{
	return x == 0.0f && y == 0.0f && z == 0.0f;
}

// Eliminate any parameter that may be due to round-off error.
Vector& Vector::Cleanse()
{
	if(fabs(x) < 1.0e-3)	x = 0;
	if(fabs(y) < 1.0e-3)	y = 0;
	if(fabs(z) < 1.0e-3)	z = 0;

	return (*this);
}
#pragma endregion Functions



// Global Static Vector Functions
Vector Vector::VectorAddition(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

Vector Vector::VectorSubtraction(const Vector& v1, const Vector& v2)
{
	return Vector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

Vector Vector::VectorMatrixMultiplication(const Vector& v, const Matrix& m)
{
	float vec[3] = {0};

	for(int i = 0; i < 3; i++)
		vec[i] = m[i][0] * v[0] + m[i][1] * v[1] + m[i][2] * v[2] + m[i][3];

	return Vector(vec);
}

float Vector::VectorDotProduct(const Vector& v1, const Vector& v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vector Vector::VectorCrossProduct(const Vector& v1, const Vector& v2)
{
	float x = v1.y*v2.z - v1.z*v2.y;
	float y = v1.z*v2.x - v1.x*v2.z;
	float z = v1.x*v2.y - v1.y*v2.x;

	return Vector(x, y, z);
}

float Vector::VectorMagnitude(const Vector& vec)
{
	return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

Vector Vector::VectorNormalized(const Vector& vec)
{
	float mag = VectorMagnitude(vec);
	
	float x = vec.x / mag;
	float y = vec.y / mag;
	float z = vec.z / mag;

	return Vector(x, y, z);
}

float Vector::AngleBetweenVectors(const Vector& v1, const Vector& v2)
{
	// cos(theta) = Dot(v1,v2) / ( |v1| * |v2| )
	return acos(((VectorDotProduct(v1,v2)) / (VectorMagnitude(v1) * VectorMagnitude(v2)) * DEG2RAD));
}

// Projection of v1(b) onto v2(a)
Vector Vector::VectorProjection(const Vector& v1, const Vector& v2)
{
	float theta = AngleBetweenVectors(v2, v1);	// Angle between v2 and v1
	float mag1 = VectorMagnitude(v1);			// Magnitude of v1
	float mag2 = VectorMagnitude(v2);			// Magnitude of v2
	float componentB = mag1 * cos(theta);		// Component of v1(b) in the direction of v1(a)
	
	Vector proj(v2.x, v2.y, v2.z);
	proj /= mag2;
	proj *= componentB;

	return proj;
}