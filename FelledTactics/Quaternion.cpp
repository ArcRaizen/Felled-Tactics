#include "stdafx.h"
#include "Quaternion.h"

// Default Constructor
Quaternion::Quaternion()
{
	x = y = z = 0.0f;
	w = 1.0f;
}

// Destructor
Quaternion::~Quaternion(){}

// Constructor from List of components
Quaternion::Quaternion(float components[])
{
	x = components[0];
	y = components[1];
	z = components[2];
	w = components[3];
}

// Constructor from each component
Quaternion::Quaternion(float _x, float _y, float _z, float _w): x(_x), y(_y), z(_z), w(_w) {}

// Constructor from Euler Angles
Quaternion::Quaternion(float x, float y, float z)
{	//						pitch		yaw			roll
	//						attitude	heading		bank
	//						phi			theta		psi

	x *= DEG2RAD;
	y *= DEG2RAD;
	z *= DEG2RAD;

	Quaternion h(0.0f, sinf(y/2.0f), 0.0f, cos(y/2.0f));
	Quaternion p(sinf(x/2.0f), 0.0f, 0.0f, cos(x/2.0f));
	Quaternion b(0.0f, 0.0f, sinf(z/2.0f), cos(z/2.0f));

	*this = h * p * b;
}

// Constructor from Rotation about an Axis
Quaternion::Quaternion(const Vector& startVec, const Vector& endVec)
{
	Vector perp = startVec * endVec;
	perp.NormalizeInPlace();

	if(float(perp) > 1.0e-5)	// if it's non-zero
	{
		float theta = startVec / endVec;	// angle between startVec and endVec
		theta *= DEG2RAD;
		w = cos(theta/2);
		// Potentially need to negate some of these based on which direction is positive for each axis
		x = perp.x * sin(theta/2);
		y = perp.y * sin(theta/2);
		z = perp.z * sin(theta/2);
	}
	else
	{
		w = 1.0f;
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
}

// Quaternion from Axis Angle
Quaternion::Quaternion(const Vector& v, float angle)
{
	Vector v2 = v.Normalized();
	angle *= DEG2RAD;

	w = cosf(angle/2);
	x = v2.x * sin(angle/2);
	y = v2.y * sin(angle/2);
	z = v2.z * sin(angle/2);
}

// Copy-Constructor
Quaternion::Quaternion(const Quaternion& quat)
{
	*this = quat;
}

// Operator= overload
Quaternion& Quaternion::operator=(const Quaternion& other)
{
	if(this != &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}

	return *this;
}

#pragma region Operator Overloads
// Indexing
float& Quaternion::operator[] (int index)
{
	assert(index > -1 && index < 3);

	if(index == 0)
		return x;
	else if(index == 1)
		return y;
	else if(index == 2)
		return z;
	else if(index == 3)
		return w;
}

const float& Quaternion::operator[] (int index) const
{
	assert(index > -1 && index < 3);

	if(index == 0)
		return x;
	else if(index == 1)
		return y;
	else if(index == 2)
		return z;
	else if(index == 3)
		return w;
}

// Quaternion Addition
Quaternion Quaternion::operator+ (const Quaternion& other) const
{
	float a = x + other.x;
	float b = y + other.y;
	float c = z + other.z;
	float d = w + other.w;

	return Quaternion(a, b, c, d);
}
Quaternion& Quaternion::operator+= (const Quaternion& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

// Quaternion Subtraction
Quaternion Quaternion::operator- (const Quaternion& other) const
{
	float a = x - other.x;
	float b = y - other.y;
	float c = z - other.z;
	float d = w - other.w;

	return Quaternion(a, b, c, d);
}
Quaternion& Quaternion::operator-= (const Quaternion& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

// Quaternion Multiplication
Quaternion Quaternion::operator* (const Quaternion& other) const
{
	float a = w*other.x + x*other.w + y*other.z - z*other.y;
	float b = w*other.y - x*other.z + y*other.w + z*other.x;
	float c = w*other.z + x*other.y - y*other.x + z*other.w;
	float d = w*other.w - x*other.x - y*other.y - z*other.z;

	return Quaternion(a, b, c ,d);
}
Quaternion& Quaternion::operator*= (const Quaternion& other)
{	// a b c d			e f g h
	float a = w*other.x + x*other.w + y*other.z - z*other.y;
	float b = w*other.y - x*other.z + y*other.w + z*other.x;
	float c = w*other.z + x*other.y - y*other.x + z*other.w;
	float d = w*other.w - x*other.x - y*other.y - z*other.z;

	x = a;
	y = b;
	z = c;
	w = d;

	return *this;
}

// Quaternion Scalar Multiplication
Quaternion Quaternion::operator* (float scalar) const
{
	float a = x * scalar;
	float b = y * scalar;
	float c = z * scalar;
	float d = w * scalar;

	return Quaternion(a, b, c, d);
}
Quaternion& Quaternion::operator*= (float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

// Quaternion Conjugate
Quaternion& Quaternion::operator~()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

// Quaternion Negation
Quaternion& Quaternion::operator-()
{
	x = -x;
	y = -y;
	z = -z;
	w = -w;

	return *this;
}

// Quaternion Dot Product
float Quaternion::operator% (const Quaternion& other) const
{
	return x*other.x + y*other.y + z*other.z;
}

// Quaternion Equality
bool Quaternion::operator== (const Quaternion& other) const
{
	return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Quaternion::operator!= (const Quaternion& other) const
{
	return !(*this == other);
}

// Quaternion Magnitude
Quaternion::operator float() const
{
	return sqrtf(x*x + y*y + z*z + w*w);
}

#pragma endregion Operator Overloads

#pragma region Functions
// Return this Quaternion + other
Quaternion& Quaternion::Add(const Quaternion& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

// Return this Quaternion - other
Quaternion& Quaternion::Subtract(const Quaternion& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;

	return *this;
}

// Return this Quaternion * other		(Quaternion * Quaternion)
Quaternion& Quaternion::Multiply(const Quaternion& other)
{
	float a = x*other.w + y*other.z - z*other.y + w*other.x;
	float b = -x*other.z + y*other.w + z*other.z + w*other.y;
	float c = x*other.y - y*other.x + z*other.w + w*other.z;
	float d = -x*other.x - y*other.y - z*other.z + w*other.w;

	x = a;
	y = b;
	z = c;
	w = d;

	return *this;
}

// Return this Quaternion * scalar		(Quaternion * scalar)
Quaternion& Quaternion::Scale(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;

	return *this;
}

// Returns the Conjugate of this Quaternion, but does not alter it
Quaternion Quaternion::Conjugated() const
{
	return Quaternion(-x, -y, -z, w);
}

// Conjugate this quaternion
Quaternion& Quaternion::ConjugateInPlace()
{
	return ~(*this);
}

Quaternion Quaternion::Inversed() const
{
	Quaternion q(x, y, z, w);

	return q.Conjugated() * (1/(float(q)*float(q)));
}

Quaternion& Quaternion::InvertInPlace()
{
	(*this).ConjugateInPlace();
	(*this) *= (1 / (float(*this) * float(*this)));

	return (*this);
}

// Returns the Negated version of this Quaternion, but does not alter it
Quaternion Quaternion::Negated() const
{
	float a = -x;
	float b = -y;
	float c = -z;

	return Quaternion(a, b, c, w);
}

// Negates this Quaternion
Quaternion& Quaternion::NegateInPlace()
{
	return -(*this);
}

// Returns to Normalized version of this Quaternion, but does not alter it.
Quaternion Quaternion::Normalized() const
{
	float mag = float(*this);

	float a = x / mag;
	float b = y / mag;
	float c = z / mag;
	float d = w / mag;

	if(mag == 0)
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	return Quaternion(a, b, c, d);
}

// Normalizes this Quaternion
Quaternion& Quaternion::NormalizeInPlace()
{
	float mag = float(*this);
	x /= mag;
	y /= mag;
	z /= mag;
	w /= mag;

	// Exception for quaternions with no magnitude
	if(mag == 0)
	{
		x = y = z = 0.0f;
		w = 1.0f;
	}

	return *this;
}

// Quaternion Dot Product
float Quaternion::DotProduct(const Quaternion& other) const
{
	return x*other.x + y*other.y + z*other.z + w*other.w;
}

// Return the Magnitude of this Quaternion
float Quaternion::Magnitude() const
{
	return float(*this);
}

// Return the corresponding Rotation Matrix of this Quaternion
Matrix Quaternion::ConvertToMatrix() const
{
	float matrix[4][4] = {0};

	matrix[0][0] = 1-2*y*y - 2*z*z;	matrix[0][1] = 2*x*y - 2*z*w;	matrix[0][2] = 2*x*z + 2*y*w;	matrix[0][3] = 0.0f;
	matrix[1][0] = 2*x*y + 2*z*w;	matrix[1][1] = 1-2*x*x - 2*z*z;	matrix[1][2] = 2*y*z - 2*x*w;	matrix[1][3] = 0.0f;
	matrix[2][0] = 2*x*z - 2*y*w;	matrix[2][1] = 2*y*z + 2*x*w;	matrix[2][2] = 1-2*x*x - 2*y*y;	matrix[2][3] = 0.0f;
	matrix[3][0] = 0.0f;			matrix[3][1] = 0.0f;			matrix[3][2] = 0.0f;			matrix[3][3] = 1.0f;

	return Matrix(matrix);
}
	
// Return an array of the Euler Angles represented by this Quaternion
float* Quaternion::EulerAngles() const
{
	//return this->ConvertToMatrix().EulerAngles();

	double heading, attitude, bank;
	double sqw = w*w;
    double sqx = x*x;
    double sqy = y*y;
    double sqz = z*z;
	double unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
	double test = x*y + z*w;
	if (test > 0.499*unit) 
	{ // singularity at north pole
		heading = 2 * atan2(x,w);
		attitude = PI/2;
		bank = 0;
		return new float[3];
	}
	if (test < -0.499*unit) 
	{ // singularity at south pole
		heading = -2 * atan2(x,w);
		attitude = -PI/2;
		bank = 0;
		return new float[3];
	}
    heading = atan2f(2*y*w-2*x*z , sqx - sqy - sqz + sqw);
	attitude = asin(2*test/unit);
	bank = atan2f(2*x*w-2*y*z , -sqx + sqy - sqz + sqw);

	float* blah = new float[3];
	blah[0] = heading * 180.0f/PI;
	blah[1] = attitude * 180.0f/PI;
	blah[2] = bank * 180.0f/PI;

	return blah;
}

// Convert this Quaternion to corresponding Axis Angle representation (returned via passed-by-reference variables)
void Quaternion::AxisAngle(Vector& v, float& angle)
{
	float scale = sqrtf(x*x + y*y + z*z);			// glRotatef(angleX, 1, 0, 0)		// convert Euler to Quaternion
													// glRotatef(angleY, 0, 1, 0)		// convert Quaternion to Axis Angle
	v.x = x / scale;								// glRotatef(angleZ, 0, 0, 1)		// glRotate(theta, v.x, v.y, v.z)
	v.y = y / scale;								// translate						// translate
	v.z = z / scale;
	angle = 2.0f * acos(w);
}

Quaternion& Quaternion::Identity()
{
	 w = 1.0f;
	 x = y = z = 0.0f;

	 return(*this);
}

bool Quaternion::IsIdentity()
{
	return x == 0.0f && y == 0.0f && z == 0.0f && w == 1.0f;
}

// Return q1 + q2
Quaternion Quaternion::QuaternionAddition(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w);
}

// Return q1 - q2
Quaternion Quaternion::QuaternionSubtraction(const Quaternion& q1, const Quaternion& q2)
{
	return Quaternion(q1.x - q2.x, q1.y - q2.y, q1.z - q2.z, q1.w - q2.w);
}

// Return q1 * q2
Quaternion Quaternion::QuaternionMultiplication(const Quaternion& q1, const Quaternion& q2)
{
	float a = q1.x*q2.w + q1.y*q2.z - q1.z*q2.y + q1.w*q2.x;
	float b = -q1.x*q2.z + q1.y*q2.w + q1.z*q2.z + q1.w*q2.y;
	float c = q1.x*q2.y - q1.y*q2.x + q1.z*q2.w + q1.w*q2.z;
	float d = -q1.x*q2.x - q1.y*q2.y - q1.z*q2.z + q1.w*q2.w;

	return Quaternion(a, b, c ,d);
}

// Return q * scalar
Quaternion Quaternion::QuaternionScale(const Quaternion& q, float scalar)
{
	return Quaternion(q.x * scalar, q.y * scalar, q.z * scalar, q.w * scalar);
}

// Return negation of q
Quaternion Quaternion::QuaternionNegate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, -q.w);
}

// Return conjugate of q
Quaternion Quaternion::QuaternionConjugate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}

// Return Matrix equivalent of q
Matrix Quaternion::QuaternionToMatrix(const Quaternion& q)
{
	float matrix[4][4] = {0};

	matrix[0][0] = 1-2*q.y*q.y - 2*q.z*q.z;	matrix[0][1] = 2*q.x*q.y - 2*q.z*q.w;	matrix[0][2] = 2*q.x*q.z + 2*q.y*q.w;
	matrix[1][0] = 2*q.x*q.y + 2*q.z*q.w;	matrix[1][1] = 1-2*q.x*q.x - 2*q.z*q.z;	matrix[1][2] = 2*q.y*q.z - 2*q.x*q.w;
	matrix[2][0] = 2*q.x*q.z - 2*q.y*q.w;	matrix[2][1] = 2*q.y*q.z + 2*q.x*q.w;	matrix[2][2] = 1-2*q.x*q.x - 2*q.y*q.y;

	return Matrix(matrix);
}

// Return Euler Angles of q
float* Quaternion::QuaternionEulerAngles(Quaternion& q)
{
	Matrix matrix = q.ConvertToMatrix();
	float eulerAngles[3] = {0};

	float phi=0, theta=0, psi=0;
	float xyDist = sqrtf(matrix[0][0]*matrix[0][0] + matrix[1][0]*matrix[1][0]);

	// Limit at theta = +/- 90. Stop that here.
	if(xyDist > 0.0001f)
	{
		// atan2f( sin(theta), abs(cos(theta))) = atan2f(tan(theta)) = theta
		theta = atan2f(-matrix[2][0], xyDist);

		// atan2f( cos(theta)sin(psi), cos(theta)cos(psi)) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
		psi =atan2f(matrix[1][0], matrix[0][0]);

		// atan2f( sin(phi)cos(theta), cos(phi)cos(theta)) = atan2f( sin(phi), cos(phi)) = atan2f(tan(phi)) = phi
		phi = atan2f(matrix[2][1], matrix[2][2]);
	}
	else
	{
		// atan2f( sin(theta), abs(cos(theta))) = atan2f( tan(theta)) = theta
		theta = atan2f(-matrix[2][0], xyDist);

		// atan2f( -(cos(phi)sin(psi)+sin(phi)sin(theta)cos(psi)), cos(phi)cos(psi)+sin(phi)sin(theta)sin(psi))
		//		in this clause, we assume thetha = 90deg or PI/4rad, at which sin(theta) = 0 - This results in
		// atan2f( cos(phi)sin(psi)-0, cos(phi)cos(psi)+0)) = atan2f(cos(phi)sin(psi), cos(phi)cos(psi)) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
		psi = atan2f(-matrix[0][1], matrix[1][1]);

		// Degree of freedom has been lost due to Gimbal Lock
		phi = 0;
	}

	eulerAngles[0] = phi;
	eulerAngles[1] = theta;
	eulerAngles[2] = psi;

	return eulerAngles;
}

// Return Quaternion representation of a rotation between 2 Vectors
Quaternion Quaternion::QuaternionFromTwoVectors(const Vector& startVec, const Vector& endVec)
{
	Vector perp = startVec * endVec;
	perp.NormalizeInPlace();

	if(float(perp) > 1.0e-5)
	{
		float angle = acos((startVec % endVec) / (float(startVec) * float(endVec)));
		return Quaternion(perp.x * sinf(angle/2), perp.y * sinf(angle/2), perp.z * sinf(angle/2), cosf(angle/2));
	}
	else	// Start and End Vectors coincide, return identity quaternion
		return Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
}

Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	Quaternion newQ;
	float dot = q1.DotProduct(q2);

	// Angle between q1 and q2 is very small, return now or omega will be 0 and there will be divide-by-zero errors and/or leave bounds of acos
	if(abs(dot) >= 1.0f)
		return q1;

	float omega = acosf(dot);
	newQ.w = q1.w * sinf((1.0f-t)*omega)/sinf(omega) + q2.w * sinf(t*omega)/sinf(omega);
	newQ.x = q1.x * sinf((1.0f-t)*omega)/sinf(omega) + q2.x * sinf(t*omega)/sinf(omega);
	newQ.y = q1.y * sinf((1.0f-t)*omega)/sinf(omega) + q2.y * sinf(t*omega)/sinf(omega);
	newQ.z = q1.z * sinf((1.0f-t)*omega)/sinf(omega) + q2.z * sinf(t*omega)/sinf(omega);

	return newQ;
}