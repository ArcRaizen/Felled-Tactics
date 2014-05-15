#include "stdafx.h"
#include "Matrix.h"

// Default Constructor - Creates Identity Matrix
Matrix::Matrix()
{	
	int count = 0;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			matrix[i][j] = 0;
			matArray[count++] = 0;
		}
	}

	matArray[0] = matArray[5] = matArray[10] = matArray[15] = 
	matrix[0][0] = matrix[1][1] = matrix[2][2] = matrix[3][3] = 1;
}

// Destructor
Matrix::~Matrix()
{

}

// Matrix from array
Matrix::Matrix(float components[])
{
	for(int i = 0; i < 16; i++)
		matArray[i] = components[i];
			
	this->SetMultiArray();
}

// Matrix from 2d array
Matrix::Matrix(float mat[][4])
{
	int count = 0;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			matrix[i][j] = mat[i][j];
			matArray[count++] = mat[i][j];
		}
	}
}

// Matrix from Quaternion
Matrix::Matrix(const Quaternion& q)
{
	matrix[0][0] = 1-2*q.y*q.y - 2*q.z*q.z;	matrix[0][1] = 2*q.x*q.y - 2*q.z*q.w;	matrix[0][1] = 2*q.x*q.z + 2*q.y*q.w;
	matrix[1][0] = 2*q.x*q.y + 2*q.z*q.w;	matrix[1][1] = 1-2*q.x*q.x - 2*q.z*q.z;	matrix[1][2] = 2*q.y*q.z - 2*q.x*q.w;
	matrix[2][0] = 2*q.x*q.z - 2*q.y*q.w;	matrix[2][1] = 2*q.y*q.z + 2*q.x*q.w;	matrix[2][1] = 1-2*q.x*q.x - 2*q.y*q.y;

	this->SetArray();
}

// Matrix from Euler Angles
Matrix::Matrix(float phi, float theta, float psi)
{

	matrix[0][0] = cosf(theta)*cosf(psi);	matrix[0][1] = -cosf(phi)*sinf(psi)-sinf(phi)*sinf(theta)*cosf(psi);	matrix[0][2] = sinf(phi)*sinf(psi)+cosf(phi)*sinf(theta)*cosf(psi);		matrix[0][3] = 0.0f;
	matrix[1][0] = cosf(theta)*sinf(psi);	matrix[1][1] = cosf(phi)*cosf(psi)+sinf(phi)*sinf(theta)*sinf(psi);		matrix[1][2] = -sinf(phi)*cosf(psi)+cosf(phi)*sinf(theta)*sinf(psi);	matrix[1][3] = 0.0f;
	matrix[2][0] = -sinf(theta);			matrix[2][1] = sinf(phi)*cosf(theta);									matrix[2][2] = cosf(phi)*cosf(theta);									matrix[2][3] = 0.0f;
	matrix[3][0] =	0.0f;					matrix[3][1] =	0.0f;													matrix[3][2] = 0.0f;													matrix[3][3] = 1.0f;

	this->SetArray();
}

Matrix& Matrix::operator= (const Matrix& mat)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			matrix[i][j] = mat[i][j];
	}

	this->SetArray();

	return *this;
}

#pragma region Operator Overloads
// Indexing
float (&Matrix::operator[] (int index))[4]
{
	assert(index > -1 && index < 4);

	return matrix[index];
}

const float* Matrix::operator[] (int index) const
{
	assert(index > -1 && index < 4);

	return matrix[index];
}

// Matrix Addition
Matrix Matrix::operator+ (const Matrix& mat) const
{
	float newMat[4][4] = {0};
	
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			newMat[i][j] = matrix[i][j] + mat[i][j];
	}


	return Matrix(newMat);
}

Matrix& Matrix::operator+= (const Matrix& mat)
{
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			matrix[i][j] += mat[i][j];
	}

	this->SetArray();

	return *this;
}

// Matrix Subtraction
Matrix Matrix::operator- (const Matrix& mat) const
{
	float newMat[4][4] = {0};
	
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			newMat[i][j] = matrix[i][j] - mat[i][j];
	}


	return Matrix(newMat);
}
Matrix& Matrix::operator-= (const Matrix& mat)
{
	for(int i = 0; i < 16; i++)
	{
		for(int j = 0; j < 4; j++)
			matrix[i][j] -= mat[i][j];
	}

	this->SetArray();

	return *this;
}

// Matrix Scalar Multiplication
Matrix Matrix::operator* (float scalar) const
{
	float newMat[16] = {0};
	
	for(int i = 0; i < 16; i++)
		newMat[i] = matArray[i] * scalar;


	return Matrix(newMat);
}
Matrix&	Matrix::operator*= (float scalar)
{
	for(int i = 0; i < 16; i++)
		matArray[i] *= scalar;

	this->SetMultiArray();

	return *this;
}

// Matrix Multiplication
Matrix Matrix::operator* (const Matrix& mat) const
{
	float temp[16] = {0};

	temp[0] = matrix[0][0]*mat[0][0] + matrix[0][1]*mat[1][0] + matrix[0][2]*mat[2][0] + matrix[0][3]*mat[3][0];
	temp[1] = matrix[0][0]*mat[0][1] + matrix[0][1]*mat[1][1] + matrix[0][2]*mat[2][1] + matrix[0][3]*mat[3][1];
	temp[2] = matrix[0][0]*mat[0][2] + matrix[0][1]*mat[1][2] + matrix[0][2]*mat[2][2] + matrix[0][3]*mat[3][2];
	temp[3] = matrix[0][0]*mat[0][3] + matrix[0][1]*mat[1][3] + matrix[0][2]*mat[2][3] + matrix[0][3]*mat[3][3];
	temp[4] = matrix[1][0]*mat[0][0] + matrix[1][1]*mat[1][0] + matrix[1][2]*mat[2][0] + matrix[1][3]*mat[3][0];
	temp[5] = matrix[1][0]*mat[0][1] + matrix[1][1]*mat[1][1] + matrix[1][2]*mat[2][1] + matrix[1][3]*mat[3][1];
	temp[6] = matrix[1][0]*mat[0][2] + matrix[1][1]*mat[1][2] + matrix[1][2]*mat[2][2] + matrix[1][3]*mat[3][2];
	temp[7] = matrix[1][0]*mat[0][3] + matrix[1][1]*mat[1][3] + matrix[1][2]*mat[2][3] + matrix[1][3]*mat[3][3];
	temp[8] = matrix[2][0]*mat[0][0] + matrix[2][1]*mat[1][0] + matrix[2][2]*mat[2][0] + matrix[2][3]*mat[3][0];
	temp[9] = matrix[2][0]*mat[0][1] + matrix[2][1]*mat[1][1] + matrix[2][2]*mat[2][1] + matrix[2][3]*mat[3][1];
	temp[10] = matrix[2][0]*mat[0][2] + matrix[2][1]*mat[1][2] + matrix[2][2]*mat[2][2] + matrix[2][3]*mat[3][2];
	temp[11] = matrix[2][0]*mat[0][3] + matrix[2][1]*mat[1][3] + matrix[2][2]*mat[2][3] + matrix[2][3]*mat[3][3];
	temp[12] = matrix[3][0]*mat[0][0] + matrix[3][1]*mat[1][0] + matrix[3][2]*mat[2][0] + matrix[3][3]*mat[3][0];
	temp[13] = matrix[3][0]*mat[0][1] + matrix[3][1]*mat[1][1] + matrix[3][2]*mat[2][1] + matrix[3][3]*mat[3][1];
	temp[14] = matrix[3][0]*mat[0][2] + matrix[3][1]*mat[1][2] + matrix[3][2]*mat[2][2] + matrix[3][3]*mat[3][2];
	temp[15] = matrix[3][0]*mat[0][3] + matrix[3][1]*mat[1][3] + matrix[3][2]*mat[2][3] + matrix[3][3]*mat[3][3];

	return Matrix(temp);
}
Matrix&	Matrix::operator*= (const Matrix& mat)
{
	matArray[0] = matrix[0][0]*mat[0][0] + matrix[0][1]*mat[1][0] + matrix[0][2]*mat[2][0] + matrix[0][3]*mat[3][0];
	matArray[1] = matrix[0][0]*mat[0][1] + matrix[0][1]*mat[1][1] + matrix[0][2]*mat[2][1] + matrix[0][3]*mat[3][1];
	matArray[2] = matrix[0][0]*mat[0][2] + matrix[0][1]*mat[1][2] + matrix[0][2]*mat[2][2] + matrix[0][3]*mat[3][2];
	matArray[3] = matrix[0][0]*mat[0][3] + matrix[0][1]*mat[1][3] + matrix[0][2]*mat[2][3] + matrix[0][3]*mat[3][3];
	matArray[4] = matrix[1][0]*mat[0][0] + matrix[1][1]*mat[1][0] + matrix[1][2]*mat[2][0] + matrix[1][3]*mat[3][0];
	matArray[5] = matrix[1][0]*mat[0][1] + matrix[1][1]*mat[1][1] + matrix[1][2]*mat[2][1] + matrix[1][3]*mat[3][1];
	matArray[6] = matrix[1][0]*mat[0][2] + matrix[1][1]*mat[1][2] + matrix[1][2]*mat[2][2] + matrix[1][3]*mat[3][2];
	matArray[7] = matrix[1][0]*mat[0][3] + matrix[1][1]*mat[1][3] + matrix[1][2]*mat[2][3] + matrix[1][3]*mat[3][3];
	matArray[8] = matrix[2][0]*mat[0][0] + matrix[2][1]*mat[1][0] + matrix[2][2]*mat[2][0] + matrix[2][3]*mat[3][0];
	matArray[9] = matrix[2][0]*mat[0][1] + matrix[2][1]*mat[1][1] + matrix[2][2]*mat[2][1] + matrix[2][3]*mat[3][1];
	matArray[10] = matrix[2][0]*mat[0][2] + matrix[2][1]*mat[1][2] + matrix[2][2]*mat[2][2] + matrix[2][3]*mat[3][2];
	matArray[11] = matrix[2][0]*mat[0][3] + matrix[2][1]*mat[1][3] + matrix[2][2]*mat[2][3] + matrix[2][3]*mat[3][3];
	matArray[12] = matrix[3][0]*mat[0][0] + matrix[3][1]*mat[1][0] + matrix[3][2]*mat[2][0] + matrix[3][3]*mat[3][0];
	matArray[13] = matrix[3][0]*mat[0][1] + matrix[3][1]*mat[1][1] + matrix[3][2]*mat[2][1] + matrix[3][3]*mat[3][1];
	matArray[14] = matrix[3][0]*mat[0][2] + matrix[3][1]*mat[1][2] + matrix[3][2]*mat[2][2] + matrix[3][3]*mat[3][2];
	matArray[15] = matrix[3][0]*mat[0][3] + matrix[3][1]*mat[1][3] + matrix[3][2]*mat[2][3] + matrix[3][3]*mat[3][3];

	this->SetMultiArray();

	return *this;
}

// Matrix Inversion
Matrix& Matrix::operator- ()
{
	// If 4th row of matrix is [0,0,0,1], then Matrix is Affine Matrix
	if(matrix[3][0] == 0 && matrix[3][1] == 0 && matrix[3][2] == 0 && matrix[3][3] == 1)
		this->InvertAffine();
	else	// General 4x4 Matrix Inverse
		this->InvertGeneral();


	this->SetArray();

	return *this;
}

// Equality
bool Matrix::operator== (const Matrix& mat) const
{
	return  matrix[0][0] == mat[0][0] && matrix[0][1] == mat[0][1] && matrix[0][2] == mat[0][2] && matrix[0][3] == mat[0][3] && 
			matrix[1][0] == mat[1][0] && matrix[1][1] == mat[1][1] && matrix[1][2] == mat[1][2] && matrix[1][3] == mat[1][3] && 
			matrix[2][0] == mat[2][0] && matrix[2][1] == mat[2][1] && matrix[2][2] == mat[2][2] && matrix[2][3] == mat[2][3] && 
			matrix[3][0] == mat[3][0] && matrix[3][1] == mat[3][1] && matrix[3][2] == mat[3][2] && matrix[3][3] == mat[3][3];
}

bool Matrix::operator!= (const Matrix& mat) const
{
	return !(*this == mat);
}

#pragma endregion

#pragma region Functions
float (&Matrix::GetArray())[16]
{
	return matArray;
}

// Set to and return Identity Matrix
Matrix& Matrix::Identity()
{
	matrix[0][1] = matrix[0][2] = matrix[0][3] = 
	matrix[1][0] = matrix[1][2] = matrix[1][3] =
	matrix[2][0] = matrix[2][1] = matrix[2][3] =
	matrix[3][0] = matrix[3][1] = matrix[3][2] = 0.0f;

	matrix[0][0] = matrix[1][1] = matrix[2][2] = matrix[3][3] = 1.0f;

	this->SetArray();

	return* this;
}

bool Matrix::IsIdentity()
{
	return matrix[0][0] == 1.0f && matrix[0][1] == 0.0f && matrix[0][2] == 0.0f && matrix[0][3] == 0.0f &&
		matrix[1][0] == 0.0f && matrix[1][1] == 1.0f && matrix[1][2] == 0.0f && matrix[1][3] == 0.0f &&
		matrix[2][0] == 0.0f && matrix[2][1] == 0.0f && matrix[2][2] == 1.0f && matrix[2][3] == 0.0f &&
		matrix[3][0] == 0.0f && matrix[3][1] == 0.0f && matrix[3][2] == 0.0f && matrix[3][3] == 1.0f;
}

// Calculate and return the determinent of this Matrix
float Matrix::Determinent() const
{
	return  matrix[0][0]*matrix[1][1]*matrix[2][2]*matrix[3][3] + matrix[0][0]*matrix[1][2]*matrix[2][3]*matrix[3][1] + matrix[0][0]*matrix[1][3]*matrix[2][1]*matrix[3][2] +
			matrix[0][1]*matrix[1][0]*matrix[2][3]*matrix[3][2] + matrix[0][1]*matrix[1][2]*matrix[2][0]*matrix[3][3] + matrix[0][1]*matrix[1][3]*matrix[2][2]*matrix[3][0] +
			matrix[0][2]*matrix[1][0]*matrix[2][1]*matrix[3][3] + matrix[0][2]*matrix[1][1]*matrix[2][3]*matrix[3][0] + matrix[0][2]*matrix[1][3]*matrix[2][0]*matrix[3][1] +
			matrix[0][3]*matrix[1][0]*matrix[2][2]*matrix[3][1] + matrix[0][3]*matrix[1][1]*matrix[2][0]*matrix[3][2] + matrix[0][3]*matrix[1][2]*matrix[2][1]*matrix[3][0] -
			matrix[0][0]*matrix[1][1]*matrix[2][3]*matrix[3][2] - matrix[0][0]*matrix[1][2]*matrix[2][1]*matrix[3][3] - matrix[0][0]*matrix[1][3]*matrix[2][2]*matrix[3][1] -
			matrix[0][1]*matrix[1][0]*matrix[2][2]*matrix[3][3] - matrix[0][1]*matrix[1][2]*matrix[2][3]*matrix[3][0] - matrix[0][1]*matrix[1][3]*matrix[2][0]*matrix[3][2] -
			matrix[0][2]*matrix[1][0]*matrix[2][3]*matrix[3][1] - matrix[0][2]*matrix[1][1]*matrix[2][0]*matrix[3][3] - matrix[0][2]*matrix[1][3]*matrix[2][1]*matrix[3][0] -
			matrix[0][3]*matrix[1][0]*matrix[2][1]*matrix[3][2] - matrix[0][3]*matrix[1][1]*matrix[2][2]*matrix[3][0] - matrix[0][3]*matrix[1][2]*matrix[2][0]*matrix[3][1];

}

// Compute inverse of Euclidean Transformation Matrix	(translation,rotation,reflection)
Matrix& Matrix::InvertEuclidean()
{
	// Set transpose of rotation part
	float temp;
    temp = matrix[0][1];  matrix[0][1] = matrix[1][0];  matrix[1][0] = temp;
    temp = matrix[0][2];  matrix[0][2] = matrix[2][0];  matrix[2][0] = temp;
    temp = matrix[1][2];  matrix[1][2] = matrix[2][1];  matrix[2][1] = temp;

    float x = matrix[0][3];
    float y = matrix[1][3];
    float z = matrix[2][3];
    matrix[0][3] = -(matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z);
    matrix[1][3] = -(matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z);
    matrix[2][3] = -(matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z);

	this->SetArray();

	return *this;
}

// Compute inverse of Affine Transformation Matrix		(translation,rotation,reflection,scaling,shearing)
Matrix& Matrix::InvertAffine()			
{
	Matrix* tempMat;
	float temp[9];

	temp[0] = matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1];
    temp[1] = matrix[0][2] * matrix[2][1] - matrix[0][1] * matrix[2][2];
    temp[2] = matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1];
    temp[3] = matrix[1][2] * matrix[2][0] - matrix[1][0] * matrix[2][2];
    temp[4] = matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0];
    temp[5] = matrix[0][2] * matrix[1][0] - matrix[0][0] * matrix[1][2];
    temp[6] = matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0];
    temp[7] = matrix[0][1] * matrix[2][0] - matrix[0][0] * matrix[2][1];
    temp[8] = matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];

	// check if determinant is 0
	float determinant = (matrix[0][0] * temp[0]) + (matrix[0][1] * temp[3]) + (matrix[0][2] * temp[6]);
	if(fabs(determinant) < 1.0e-5)
		tempMat = new Matrix();
	else
	{
		float invDeterminant = 1.0f / determinant;
		float temp2[16] = {invDeterminant*temp[0], invDeterminant*temp[1], invDeterminant*temp[2], 0.0f,
							invDeterminant*temp[3], invDeterminant*temp[4], invDeterminant*temp[5], 0.0f,
							invDeterminant*temp[6], invDeterminant*temp[7], invDeterminant*temp[8], 0.0f,
							0.0f,					0.0f,					0.0f,					1.0f};

		tempMat = new Matrix(temp2);
	}

	*this = *tempMat;
	float x = matrix[0][3];
	float y = matrix[1][3];
	float z = matrix[2][3];
	matrix[0][3] = -(*tempMat[0][0] * x + *tempMat[0][1] * y + *tempMat[0][2] * z);
	matrix[1][3] = -(*tempMat[1][0] * x + *tempMat[1][1] * y + *tempMat[1][2] * z);
	matrix[2][3] = -(*tempMat[2][0] * x + *tempMat[2][1] * y + *tempMat[2][2] * z);

	return *this;
}

// Compute inverse of General 4x4 Matrix using Cramer's Rule
	// return Identity if not true
Matrix& Matrix::InvertGeneral()
{
	// get cofactors of minor matrices
    float cofactor0 = GetCofactor(matrix[1][1],matrix[1][2],matrix[1][3], matrix[2][1],matrix[2][2],matrix[2][3], matrix[3][1],matrix[3][2],matrix[3][3]);
    float cofactor1 = GetCofactor(matrix[1][0],matrix[1][2],matrix[1][3], matrix[2][0],matrix[2][2],matrix[2][3], matrix[3][0],matrix[3][2],matrix[3][3]);
    float cofactor2 = GetCofactor(matrix[1][0],matrix[1][1],matrix[1][3], matrix[2][0],matrix[2][1], matrix[2][3], matrix[3][0],matrix[3][1],matrix[3][3]);
    float cofactor3 = GetCofactor(matrix[1][0],matrix[1][1],matrix[1][2], matrix[2][0],matrix[2][1], matrix[2][2], matrix[3][0],matrix[3][1],matrix[3][2]);

    // get determinant
    float determinant = matrix[0][0] * cofactor0 - matrix[0][1] * cofactor1 + matrix[0][2] * cofactor2 - matrix[0][3] * cofactor3;
    if(fabs(determinant) <= 1.0e-5)
        return Identity();

    // get rest of cofactors for adj(M)
    float cofactor4 = GetCofactor(matrix[0][1],matrix[0][2],matrix[0][3], matrix[2][1],matrix[2][2],matrix[2][3], matrix[3][1],matrix[3][2],matrix[3][3]);
    float cofactor5 = GetCofactor(matrix[0][0],matrix[0][2],matrix[0][3], matrix[2][0],matrix[2][2],matrix[2][3], matrix[3][0],matrix[3][2],matrix[3][3]);
    float cofactor6 = GetCofactor(matrix[0][0],matrix[0][1],matrix[0][3], matrix[2][0],matrix[2][1], matrix[2][3], matrix[3][0],matrix[3][1],matrix[3][3]);
    float cofactor7 = GetCofactor(matrix[0][0],matrix[0][1],matrix[0][2], matrix[2][0],matrix[2][1], matrix[2][2], matrix[3][0],matrix[3][1],matrix[3][2]);
    float cofactor8 = GetCofactor(matrix[0][1],matrix[0][2],matrix[0][3], matrix[1][1],matrix[1][2], matrix[1][3],  matrix[3][1],matrix[3][2],matrix[3][3]);
    float cofactor9 = GetCofactor(matrix[0][0],matrix[0][2],matrix[0][3], matrix[1][0],matrix[1][2], matrix[1][3],  matrix[3][0],matrix[3][2],matrix[3][3]);
    float cofactor10= GetCofactor(matrix[0][0],matrix[0][1],matrix[0][3], matrix[1][0],matrix[1][1], matrix[1][3],  matrix[3][0],matrix[3][1],matrix[3][3]);
    float cofactor11= GetCofactor(matrix[0][0],matrix[0][1],matrix[0][2], matrix[1][0],matrix[1][1], matrix[1][2],  matrix[3][0],matrix[3][1],matrix[3][2]);
    float cofactor12= GetCofactor(matrix[0][1],matrix[0][2],matrix[0][3], matrix[1][1],matrix[1][2], matrix[1][3],  matrix[2][1], matrix[2][2],matrix[2][3]);
    float cofactor13= GetCofactor(matrix[0][0],matrix[0][2],matrix[0][3], matrix[1][0],matrix[1][2], matrix[1][3],  matrix[2][0], matrix[2][2],matrix[2][3]);
    float cofactor14= GetCofactor(matrix[0][0],matrix[0][1],matrix[0][3], matrix[1][0],matrix[1][1], matrix[1][3],  matrix[2][0], matrix[2][1], matrix[2][3]);
    float cofactor15= GetCofactor(matrix[0][0],matrix[0][1],matrix[0][2], matrix[1][0],matrix[1][1], matrix[1][2],  matrix[2][0], matrix[2][1], matrix[2][2]);

    // build inverse matrix = adj(M) / det(M)
    // adjugate of M is the transpose of the cofactor matrix of M
    float invDeterminant = 1.0f / determinant;
    matrix[0][0] =  invDeterminant * cofactor0;    matrix[0][1] = -invDeterminant * cofactor4;    matrix[0][2] =  invDeterminant * cofactor8;    matrix[0][3] = -invDeterminant * cofactor12;
    matrix[1][0] = -invDeterminant * cofactor1;    matrix[1][1] =  invDeterminant * cofactor5;    matrix[1][2] = -invDeterminant * cofactor9;    matrix[1][3] =  invDeterminant * cofactor13;
    matrix[2][0] =  invDeterminant * cofactor2;    matrix[2][1] = -invDeterminant * cofactor6;    matrix[2][2] =  invDeterminant * cofactor10;    matrix[2][3] = -invDeterminant * cofactor14;
    matrix[3][0] = -invDeterminant * cofactor3;    matrix[3][1] =  invDeterminant * cofactor7;    matrix[3][2] = -invDeterminant * cofactor11;    matrix[3][3] =  invDeterminant * cofactor15;

    return *this;	
}

Matrix Matrix::Inverse3() const
{
	float determinent = matrix[0][0]*matrix[1][1]*matrix[2][2] + matrix[1][0]*matrix[2][1]*matrix[0][2] + 
		matrix[2][0]*matrix[0][1]*matrix[1][2] - matrix[0][0]*matrix[2][1]*matrix[1][2] - 
		matrix[2][0]*matrix[1][1]*matrix[0][2] - matrix[1][0]*matrix[0][1]*matrix[2][2];

	if(determinent == 0)
		return Matrix();

	float mat[16] = {
		matrix[1][1]*matrix[2][2] - matrix[1][2]*matrix[2][1],
		matrix[0][2]*matrix[2][1] - matrix[0][1]*matrix[2][2],
		matrix[0][1]*matrix[1][2] - matrix[0][2]*matrix[1][1],
		0,
		matrix[1][2]*matrix[2][0] - matrix[1][0]*matrix[2][2],
		matrix[0][0]*matrix[2][2] - matrix[0][2]*matrix[2][0],
		matrix[0][2]*matrix[1][0] - matrix[0][0]*matrix[1][2],
		0,
		matrix[1][0]*matrix[2][1] - matrix[1][1]*matrix[2][0],
		matrix[0][1]*matrix[2][0] - matrix[0][0]*matrix[2][1],
		matrix[0][0]*matrix[1][1] - matrix[0][1]*matrix[1][0],
		0,
		0, 0, 0, 1};

	Matrix m(mat);
	return m * (1/determinent);
}

// Calculate and return the Inverse of this Matrix if it exists
// Return the Identity Matrix otherwise
Matrix Matrix::Inverse4() const
{
	float determinent = Determinent();

	if(determinent == 0)
		return Matrix();

	float mat[16] = 
		{	matrix[1][1]*matrix[2][2]*matrix[3][3] + matrix[1][2]*matrix[2][3]*matrix[3][1] + matrix[1][3]*matrix[2][1]*matrix[3][2] - matrix[1][1]*matrix[2][3]*matrix[3][2] - matrix[1][2]*matrix[2][1]*matrix[3][3] - matrix[1][3]*matrix[2][2]*matrix[3][1],
			matrix[0][1]*matrix[2][3]*matrix[3][2] + matrix[0][2]*matrix[2][1]*matrix[3][3] + matrix[0][3]*matrix[2][2]*matrix[3][1] - matrix[0][1]*matrix[2][2]*matrix[3][3] - matrix[0][2]*matrix[2][3]*matrix[3][2] - matrix[0][3]*matrix[2][1]*matrix[3][2],
			matrix[0][1]*matrix[1][2]*matrix[3][3] + matrix[0][2]*matrix[1][3]*matrix[3][1] + matrix[0][3]*matrix[1][1]*matrix[3][2] - matrix[0][1]*matrix[1][3]*matrix[3][2] - matrix[0][2]*matrix[1][1]*matrix[3][3] - matrix[0][2]*matrix[1][2]*matrix[3][1],
			matrix[0][1]*matrix[1][3]*matrix[2][2] + matrix[0][2]*matrix[1][1]*matrix[2][3] + matrix[0][3]*matrix[1][2]*matrix[2][1] - matrix[0][1]*matrix[1][2]*matrix[2][3] - matrix[0][2]*matrix[1][3]*matrix[2][1] - matrix[0][3]*matrix[1][1]*matrix[2][2],
			matrix[1][0]*matrix[2][3]*matrix[3][2] + matrix[1][2]*matrix[2][0]*matrix[3][3] + matrix[1][3]*matrix[2][2]*matrix[3][0] - matrix[1][0]*matrix[2][2]*matrix[3][3] - matrix[1][2]*matrix[2][3]*matrix[3][0] - matrix[1][3]*matrix[2][0]*matrix[3][2],
			matrix[0][0]*matrix[2][2]*matrix[3][3] + matrix[0][2]*matrix[2][1]*matrix[3][2] + matrix[0][3]*matrix[2][0]*matrix[3][2] - matrix[0][0]*matrix[2][3]*matrix[3][2] - matrix[0][2]*matrix[2][0]*matrix[3][3] - matrix[0][3]*matrix[2][2]*matrix[3][0],
			matrix[0][0]*matrix[1][3]*matrix[3][2] + matrix[0][2]*matrix[1][0]*matrix[3][3] + matrix[0][3]*matrix[1][2]*matrix[3][0] - matrix[0][0]*matrix[1][2]*matrix[3][3] - matrix[0][2]*matrix[1][3]*matrix[3][1] - matrix[0][3]*matrix[0][1]*matrix[3][2],
			matrix[0][0]*matrix[1][2]*matrix[2][3] + matrix[0][2]*matrix[1][3]*matrix[2][0] + matrix[0][3]*matrix[1][0]*matrix[2][2] - matrix[0][0]*matrix[1][3]*matrix[2][2] - matrix[0][2]*matrix[1][0]*matrix[2][3] - matrix[0][3]*matrix[1][2]*matrix[2][0],
			matrix[1][0]*matrix[2][1]*matrix[3][3] + matrix[1][1]*matrix[2][3]*matrix[3][0] + matrix[1][3]*matrix[2][0]*matrix[3][1] - matrix[1][0]*matrix[2][3]*matrix[3][1] - matrix[1][1]*matrix[2][0]*matrix[3][3] - matrix[1][3]*matrix[2][1]*matrix[3][0],
			matrix[0][0]*matrix[2][3]*matrix[3][1] + matrix[0][1]*matrix[2][0]*matrix[3][3] + matrix[0][3]*matrix[2][1]*matrix[3][0] - matrix[0][0]*matrix[2][1]*matrix[3][3] - matrix[0][1]*matrix[2][3]*matrix[3][0] - matrix[0][3]*matrix[2][0]*matrix[3][1],
			matrix[0][0]*matrix[1][1]*matrix[3][3] + matrix[0][1]*matrix[1][3]*matrix[3][0] + matrix[0][3]*matrix[1][0]*matrix[3][1] - matrix[0][0]*matrix[1][3]*matrix[3][1] - matrix[0][1]*matrix[1][0]*matrix[3][3] - matrix[0][2]*matrix[1][1]*matrix[3][0],
			matrix[0][0]*matrix[1][3]*matrix[2][1] + matrix[0][1]*matrix[1][0]*matrix[2][3] + matrix[0][3]*matrix[1][1]*matrix[2][0] - matrix[0][0]*matrix[1][1]*matrix[2][3] - matrix[0][1]*matrix[1][3]*matrix[2][0] - matrix[0][3]*matrix[1][0]*matrix[2][1],
			matrix[1][0]*matrix[2][2]*matrix[3][1] + matrix[1][1]*matrix[2][0]*matrix[3][2] + matrix[1][2]*matrix[2][1]*matrix[3][0] - matrix[1][0]*matrix[2][1]*matrix[3][2] - matrix[1][1]*matrix[2][2]*matrix[3][0] - matrix[1][2]*matrix[2][0]*matrix[3][1],
			matrix[0][0]*matrix[2][1]*matrix[3][2] + matrix[0][1]*matrix[2][2]*matrix[3][0] + matrix[0][2]*matrix[2][0]*matrix[3][1] - matrix[0][0]*matrix[2][2]*matrix[3][1] - matrix[0][1]*matrix[2][0]*matrix[3][2] - matrix[0][2]*matrix[2][1]*matrix[3][0],
			matrix[0][0]*matrix[1][2]*matrix[3][1] + matrix[0][1]*matrix[1][0]*matrix[3][2] + matrix[0][2]*matrix[1][1]*matrix[3][0] - matrix[0][0]*matrix[1][1]*matrix[3][2] - matrix[0][1]*matrix[1][2]*matrix[3][0] - matrix[0][2]*matrix[1][0]*matrix[3][1],
			matrix[0][0]*matrix[1][1]*matrix[2][2] + matrix[0][1]*matrix[1][2]*matrix[2][0] + matrix[0][2]*matrix[1][0]*matrix[2][1] - matrix[0][0]*matrix[1][2]*matrix[2][1] - matrix[0][1]*matrix[1][0]*matrix[2][2] - matrix[0][2]*matrix[1][1]*matrix[2][0]};

	Matrix m(mat);
	return m * (1/determinent);
}

//	0	1	2	3
//	4	5	6	7
//	8	9	10	11
//	12	13	14	15
Matrix Matrix::Transposed() const
{
	float mat[4][4] = {0};

								mat[0][1] = matArray[4];	mat[0][2] = matArray[8];	mat[0][3] = matArray[12];
	mat[1][0] = matArray[1];								mat[1][2] = matArray[9];	mat[1][3] = matArray[13];
	mat[2][0] = matArray[2];	mat[2][1] = matArray[3];								mat[2][3] = matArray[14];
	mat[3][0] = matArray[3];	mat[3][1] = matArray[7];	mat[3][2] = matArray[11];

	return Matrix(mat);
}

Matrix& Matrix::TransposeInPlace()
{
								matrix[0][1] = matArray[4];	matrix[0][2] = matArray[8];	matrix[0][3] = matArray[12];
	matrix[1][0] = matArray[1];								matrix[1][2] = matArray[9];	matrix[1][3] = matArray[13];
	matrix[2][0] = matArray[2]; matrix[2][1] = matArray[3];								matrix[2][3] = matArray[14];
	matrix[3][0] = matArray[3];	matrix[3][1] = matArray[7]; matrix[3][2] = matArray[11];

	this->SetArray();

	return *this;
}

// Compute cofactor of 3x3 minor Matrix
float Matrix::GetCofactor(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8) const
{
	return m0 * (m4*m8 - m5*m7)-
			m1 * (m3*m8 - m5*m6) +
			m2 * (m3*m7 - m4*m6);
}

// Return an array of the Euler Angles represented by this Matrix
float* Matrix::EulerAngles() const
{
	float* eulerAngles = new float[3];

	float phi=0, theta=0, psi=0;
	float xyDist = sqrtf(matrix[0][0]*matrix[0][0] + matrix[1][0]*matrix[1][0]);

	// Limit at theta = +/- 90. Stop that here.
	if(xyDist > 0.0001f)
	{
		// atan2f( sin(theta), abs(cos(theta))) = atan2f(tan(theta)) = theta
		theta = atan2f(-matrix[2][0], xyDist);

		// atan2f( cos(theta)sin(psi), cos(theta)cos(psi)) = atan2f(sin(psi), cos(psi)) = atan2f(tan(psi)) = psi
		psi = atan2f(matrix[1][0], matrix[0][0]);

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

	phi /= DEG2RAD; theta /= DEG2RAD; psi /= DEG2RAD;
	eulerAngles[0] = phi;
	eulerAngles[1] = theta;
	eulerAngles[2] = psi;

	return eulerAngles;

/*
	float* eulerAngles = new float[3];
	float h,p,b;

	float sp = -matrix[2][0];
	if(sp <= -1.0f)
		p = -1.570796f;
	else if(sp >= 1.0f)
		p = 1.570796f;
	else
		p = asinf(sp);

	if(fabs(sp) > 0.9999f)
	{
		b = 0.0f;
		h = atan2f(-matrix[0][2], matrix[1][1]);
	}
	else
	{
		h = atan2f(matrix[2][1], matrix[2][2]);
		b = atan2f(matrix[1][0], matrix[0][0]);
	}

	eulerAngles[0] = h * 180/PI;
	eulerAngles[1] = p * 180/PI;
	eulerAngles[2] = b * 180/PI;

	return eulerAngles;*/
}

// Calculate and return Quaternion representation of this rotation Matrix
Quaternion Matrix::ConvertToQuaternion() const
{
/*	float w, x, y, z;

	w = sqrtf(1 + matrix[0][0] + matrix[1][1] + matrix[2][2]) / 2;
	x = sqrtf( MAX(0, 1  + matrix[0][0] - matrix[1][1] - matrix[2][2])) / 2;
	y = sqrtf( MAX(0, 1  - matrix[0][0] + matrix[1][1] - matrix[2][2])) / 2;
	z = sqrtf( MAX(0, 1  - matrix[0][0] - matrix[1][1] + matrix[2][2])) / 2;
	if((x < 0 && (matrix[2][1] - matrix[1][2]) > 0) || (x > 0 && (matrix[2][1] - matrix[1][2]) < 0))
		x *= -1;
	if((y < 0 && (matrix[0][2] - matrix[2][0]) > 0) || (y > 0 && (matrix[0][2] - matrix[2][0]) < 0))
		y *= -1;
	if((z < 0 && (matrix[1][0] - matrix[0][1]) > 0) || (z > 0 && (matrix[1][0] - matrix[0][1]) < 0))
		z *= -1;
*/

/*THIS ONE SUCKS	
	float w,x,y,z;
	w = sqrtf(1.0f + matrix[0][0] + matrix[1][1] + matrix[2][2]) / 2.0f;
	x = (matrix[2][1] - matrix[1][2]) / (4.0f * w);
	y = (matrix[2][0] - matrix[0][2]) / (4.0f * w);
	z = (matrix[1][0] - matrix[0][1]) / (4.0f * w);
*/

	float tr = matrix[0][0] + matrix[1][1] + matrix[2][2];
	float w, x, y, z;

	if(tr > 0)
	{
		float s = sqrtf(tr+1.0f) * 2;
		w = 0.25f * s;
		x = (matrix[2][1] - matrix[1][2]) / s;
		y = (matrix[0][2] - matrix[2][0]) / s;
		z = (matrix[1][0] - matrix[0][1]) / s;
	}
	else if((matrix[0][0] > matrix[1][1]) && (matrix[0][0] > matrix[2][2]))
	{
		float s = sqrtf(1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2]) * 2.0f;
		w = (matrix[2][1] - matrix[1][2]) / s;
		x = 0.25f * s;
		y = (matrix[0][1] + matrix[1][0]) / s;
		z = (matrix[0][2] + matrix[2][0]) / s;
	}
	else if(matrix[1][1] > matrix[2][2])
	{
		float s = sqrtf(1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2]) * 2;
		w = (matrix[0][2] - matrix[2][0]) / s;
		x = (matrix[0][1] + matrix[1][0]) / s;
		y = 0.25f * s;
		z = (matrix[1][2] + matrix[2][1]) / s;
	}
	else
	{
		float s = sqrt(1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1]) * 2;
		w = (matrix[1][0] - matrix[0][1]) / s;
		x = (matrix[0][2] + matrix[2][0]) / s;
		y = (matrix[1][2] + matrix[2][1]) / s;
		z = 0.25f * s;
	}


	return Quaternion(x, y, z, w);
}
#pragma endregion

#pragma region Transformations

// Translate this Matrix by Vector v of components [x, y, z]
Matrix& Matrix::Translate(const Vector& v)
{
	return Translate(v.x, v.y, v.z);
}
Matrix& Matrix::Translate(float x, float y, float z)
{
	// Create Translation Matrix
	Matrix translation;
	translation[0][3] = x;
	translation[1][3] = y;
	translation[2][3] = z;

	// Multiply onto this matrix
	*this = translation * *this;
	return (*this);
}

// Equally scale this Matrix along all axes
Matrix& Matrix::Scale(float scale)
{
	return Scale(scale, scale, scale);
}

// Scale this Matrix along each axis proportional to sx, sy and sz
Matrix& Matrix::Scale(float sx, float sy, float sz)
{
	// Create Scale Matrix
	Matrix scale;
	scale[0][0] = sx;
	scale[1][1] = sy;
	scale[2][2] = sz;

	// Mutliply onto this matrix
	return (*this *= scale);
}

// Rotate this matrix by 'angle' degrees along 'axis' from origin
Matrix& Matrix::Rotate(float angle, const Vector& axis)
{
	return Rotate(angle, axis.x, axis.y, axis.z);
}

// Rotate this matrix by 'angle' along vector [xyz] from origin
Matrix& Matrix::Rotate(float angle, float x, float y, float z)
{
	// Create matrix for rotation
	Matrix mR;
	angle *= DEG2RAD;

	// Normalize vector [xyz] for safety
	float mag = sqrtf(x*x + y*y + z*z);
	x /= mag;	y /= mag;	z /= mag;

	// Calculate the rotation matrix
	float cos = cosf(angle);
	float sin = sinf(angle);
	mR[0][0] = x*x+(1-x*x)*cos;		mR[0][1] = x*y*(1-cos)-(z*sin);	mR[0][2] = x*z*(1-cos)+(y*sin);
	mR[1][0] = y*x*(1-cos)+(z*sin);	mR[1][1] = y*y+(1-y*y)*cos;		mR[1][2] = y*z*(1-cos)-(x*sin);
	mR[2][0] = z*x*(1-cos)-(y*sin);	mR[2][1] = z*y*(1-cos)+(x*sin);	mR[2][2] = z*z+(1-z*z)*cos;

	// Multiply onto this matrix
	return (*this *= mR);
}

// Rotate by 'angle' degrees along X-Axis
Matrix& Matrix::RotateX(float angle)
{
	// Create X-Axis rotation matrix
	Matrix mX;
	angle *= DEG2RAD;

	mX[1][1] = cosf(angle);
	mX[1][2] = -sinf(angle);
	mX[2][1] = sinf(angle);
	mX[2][2] = cosf(angle);

	// Multiply onto this matrix
	return (*this *= mX);
}

// Rotate by 'angle' degrees along Y-Axis
Matrix& Matrix::RotateY(float angle)
{
	// Create X-Axis rotation matrix
	Matrix mY;
	angle *= DEG2RAD;

	mY[0][0] = cosf(angle);
	mY[0][2] = sinf(angle);
	mY[2][0] = -sinf(angle);
	mY[2][2] = cosf(angle);

	// Multiply onto this matrix
	return (*this *= mY);
}

// Rotate by 'angle' degrees along Z-Axis
Matrix& Matrix::RotateZ(float angle)
{
	// Create X-Axis rotation matrix
	Matrix mZ;
	angle *= DEG2RAD;

	mZ[0][0] = cosf(angle);
	mZ[0][1] = -sinf(angle);
	mZ[1][0] = sinf(angle);
	mZ[1][1] = cosf(angle);

	// Multiply onto this matrix
	return (*this *= mZ);
}

// Rotate by 'angle' degrees an Arbitrary Line - a point the line goes through (start) and a direction (end-start)
Matrix& Matrix::RotateArbitraryAxis(float angle, const Vector& start, const Vector& end)
{
	// Create Matrix for rotation and other variables for simplicity
	float a = start.x;	//
	float b = start.y;	//	Point of Rotation
	float c = start.z;	//
	float x = end.x - start.x;	//
	float y = end.y - start.y;	//	Direction of axis to rotate around
	float z = end.z - start.z;	//
	Matrix mR;
	angle *= DEG2RAD;

	// Normalize the direction
	float mag = sqrtf(x*x + y*y + z*z);
	x /= mag;	y /= mag;	z /= mag;

	// Dark Magic
	float cos = cosf(angle);
	float sin = sinf(angle);
	mR[0][0]=x*x+(y*y+z*z)*cos;		mR[0][1]=x*y*(1-cos)-(z*sin);	mR[0][2]=x*z*(1-cos)+(y*sin);	mR[0][3]=(a*(y*y+z*z)-x*(b*y+c*z))*(1-cos)+(b*z-c*y)*sin;
	mR[1][0]=x*y*(1-cos)+(z*sin);	mR[1][1]=y*y+(x*x+z*z)*cos;		mR[1][2]=y*z*(1-cos)-(x*sin);	mR[1][3]=(b*(x*x+z*z)-y*(a*x+c*z))*(1-cos)+(c*x-a*z)*sin;
	mR[2][0]=x*z*(1-cos)-(y*sin);	mR[2][1]=y*z*(1-cos)+(x*sin);	mR[2][2]=z*z+(x*x+y*y)*cos;		mR[2][3]=(c*(x*x+y*y)-z*(a*x+b*y))*(1-cos)+(a*y-b*x)*sin;

	// Multiply onto this Matrix
	return (*this *= mR);
}

Matrix& Matrix::ShearXY(float shx, float shy)
{
	Matrix m;
	m[0][2] = shx;
	m[1][2] = shy;

	return (*this *= m);
}

Matrix& Matrix::ShearYZ(float shy, float shz)
{
	Matrix m;
	m[1][0] = shy;
	m[2][0] = shz;

	return (*this *= m);
}

Matrix& Matrix::ShearZX(float shx, float shz)
{
	Matrix m;
	m[0][1] = shx;
	m[2][1] = shz;

	return (*this *= m);
}

#pragma endregion

// Sets the array matArray[] equal to the equivalent of matrix[][]
void Matrix::SetArray()
{
	int count = 0;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			matArray[count++] = matrix[i][j];
	}
}

// Sets the 2D array matrix[][] equal to the equivalent of matArray[]
void Matrix::SetMultiArray()
{
	int count = 0;
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			matrix[i][j] = matArray[count++];
	}
}

// Return m1+m2
Matrix MatrixAddition(const Matrix& m1, const Matrix& m2)
{
	float newMat[4][4] = {0};
	
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			newMat[i][j] = m1[i][j] + m2[i][j];
	}


	return Matrix(newMat);
}

// Return m1-m2
Matrix MatrixSubtraction(const Matrix& m1, const Matrix& m2)
{
	float newMat[4][4] = {0};
	
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			newMat[i][j] = m1[i][j] - m2[i][j];
	}


	return Matrix(newMat);
}

// Return m*scalar	(Matrix * Scalar)
Matrix MatrixScale(const Matrix& m, float scalar)
{
	float newMat[4][4] = {0};
	
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
			newMat[i][j] = m[i][j] * scalar;
	}


	return Matrix(newMat);
}

// Return m1*m1		(Matrix * Matrix)
Matrix MatrixMultiplication(const Matrix& m1, const Matrix& m2)
{
	float temp[16] = {0};

	temp[0] = m1[0][0]*m2[0][0] + m1[0][1]*m2[1][0] + m1[0][2]*m2[2][0] + m1[0][3]*m2[3][0];
	temp[1] = m1[0][0]*m2[0][1] + m1[0][1]*m2[1][1] + m1[0][2]*m2[2][1] + m1[0][3]*m2[3][1];
	temp[2] = m1[0][0]*m2[0][2] + m1[0][1]*m2[1][2] + m1[0][2]*m2[2][2] + m1[0][3]*m2[3][2];
	temp[3] = m1[0][0]*m2[0][3] + m1[0][1]*m2[1][3] + m1[0][2]*m2[2][3] + m1[0][3]*m2[3][3];
	temp[4] = m1[1][0]*m2[0][0] + m1[1][1]*m2[1][0] + m1[1][2]*m2[2][0] + m1[1][3]*m2[3][0];
	temp[5] = m1[1][0]*m2[0][1] + m1[1][1]*m2[1][1] + m1[1][2]*m2[2][1] + m1[1][3]*m2[3][1];
	temp[6] = m1[1][0]*m2[0][2] + m1[1][1]*m2[1][2] + m1[1][2]*m2[2][2] + m1[1][3]*m2[3][2];
	temp[7] = m1[1][0]*m2[0][3] + m1[1][1]*m2[1][3] + m1[1][2]*m2[2][3] + m1[1][3]*m2[3][3];
	temp[8] = m1[2][0]*m2[0][0] + m1[2][1]*m2[1][0] + m1[2][2]*m2[2][0] + m1[2][3]*m2[3][0];
	temp[9] = m1[2][0]*m2[0][1] + m1[2][1]*m2[1][1] + m1[2][2]*m2[2][1] + m1[2][3]*m2[3][1];
	temp[10] = m1[2][0]*m2[0][2] + m1[2][1]*m2[1][2] + m1[2][2]*m2[2][2] + m1[2][3]*m2[3][2];
	temp[11] = m1[2][0]*m2[0][3] + m1[2][1]*m2[1][3] + m1[2][2]*m2[2][3] + m1[2][3]*m2[3][3];
	temp[12] = m1[3][0]*m2[0][0] + m1[3][1]*m2[1][0] + m1[3][2]*m2[2][0] + m1[3][3]*m2[3][0];
	temp[13] = m1[3][0]*m2[0][1] + m1[3][1]*m2[1][1] + m1[3][2]*m2[2][1] + m1[3][3]*m2[3][1];
	temp[14] = m1[3][0]*m2[0][2] + m1[3][1]*m2[1][2] + m1[3][2]*m2[2][2] + m1[3][3]*m2[3][2];
	temp[15] = m1[3][0]*m2[0][3] + m1[3][1]*m2[1][3] + m1[3][2]*m2[2][3] + m1[3][3]*m2[3][3];

	return Matrix(temp);
}

// Return Quaternion representation of Rotation Matrix m
Quaternion MatrixToQuaternion(const Matrix& m)
{
	float w, x, y, z;

	w = sqrtf(1 + m[0][0] + m[1][1] + m[2][2]) / 2;
	x = (m[2][1] - m[1][2]) / (4 * w);
	y = (m[0][2] - m[2][0]) / (4 * w);
	z = (m[1][0] - m[0][1]) / (4 * w);

	return Quaternion(x, y, z, w);
}

// Return Matrix representation of 'angle' degrees rotation along X-Axis
Matrix MatrixFromXAxisRotation(float angle)
{
	// Create X-Axis rotation matrix
	float mX[4][4] = {0};
	angle *= DEG2RAD;

	mX[0][0] = mX[1][1] = mX[2][2] = mX[3][3] = 1.0f;
	mX[1][1] = cosf(angle);
	mX[1][2] = -sinf(angle);
	mX[2][1] = sinf(angle);
	mX[2][2] = cosf(angle);

	return Matrix(mX);
}

// Return Matrix representation of 'angle' degrees rotation along Y-Axis
Matrix MatrixFromYAxisRotation(float angle)
{
	// Create X-Axis rotation matrix
	float mY[4][4] = {0};
	angle *= DEG2RAD;

	mY[0][0] = mY[1][1] = mY[2][2] = mY[3][3] = 1.0f;
	mY[0][0] = cosf(angle);
	mY[0][2] = sinf(angle);
	mY[2][0] = -sinf(angle);
	mY[2][2] = cosf(angle);

	return Matrix(mY);
}

// Return Matrix representation of 'angle' degrees rotation along Z-Axis
Matrix MatrixFromZAxisRotation(float angle)
{
	// Create X-Axis rotation matrix
	float mZ[4][4] = {0};
	angle *= DEG2RAD;

	mZ[0][0] = mZ[1][1] = mZ[2][2] = mZ[3][3] = 1.0f;
	mZ[0][0] = cosf(angle);
	mZ[0][1] = -sinf(angle);
	mZ[1][0] = sinf(angle);
	mZ[1][1] = cosf(angle);

	return Matrix(mZ);
}

// Return Matrix representation of 'angle' degrees rotation along Vector [xyz] from Origin
Matrix MatrixFromRotationAboutOrigin(float angle, float x, float y, float z)
{
	// Create matrix for rotation
	float mR[4][4] = {0};
	mR[0][0] = mR[1][1] = mR[2][2] = mR[3][3] = 1.0f;
	angle *= DEG2RAD;

	// Normalize vector [xyz] for safety
	float mag = sqrtf(x*x + y*y + z*z);
	x /= mag;	y /= mag;	z /= mag;

	// Calculate the rotation matrix
	float cos = cosf(angle);
	float sin = sinf(angle);
	mR[0][0] = x*x+(1-x*x)*cos;		mR[0][1] = x*y*(1-cos)-(z*sin);	mR[0][2] = x*z*(1-cos)+(y*sin);
	mR[1][0] = y*x*(1-cos)+(z*sin);	mR[1][1] = y*y+(1-y*y)*cos;		mR[1][2] = y*z*(1-cos)-(x*sin);
	mR[2][0] = z*x*(1-cos)-(y*sin);	mR[2][1] = z*y*(1-cos)+(x*sin);	mR[2][2] = z*z+(1-z*z)*cos;

	return Matrix(mR);
}

// Return Matrix representation of 'angle' degrees rotation along Vector [xyz] from origin
Matrix MatrixFromRotationAboutOrigin(float angle, const Vector& vec)
{
	return MatrixFromRotationAboutOrigin(angle, vec.x, vec.y, vec.z);
}

Matrix Matrix::MatrixFromArbitraryAxisRotation(float angle, const Vector& start, const Vector& end)
{
	// Create Matrix for rotation and other variables for simplicity
	float a = start.x;	//
	float b = start.y;	//	Point of Rotation
	float c = start.z;	//
	float x = end.x - start.x;	//
	float y = end.y - start.y;	//	Direction of axis to rotate around
	float z = end.z - start.z;	//
	Matrix mR;
	angle *= DEG2RAD;

	// Normalize the direction
	float mag = sqrtf(x*x + y*y + z*z);
	x /= mag;	y /= mag;	z /= mag;

	// Dark Magic
	float cos = cosf(angle);
	float sin = sinf(angle);
	mR[0][0]=x*x+(y*y+z*z)*cos;		mR[0][1]=x*y*(1-cos)-(z*sin);	mR[0][2]=x*z*(1-cos)+(y*sin);	mR[0][3]=(a*(y*y+z*z)-x*(b*y+c*z))*(1-cos)+(b*z-c*y)*sin;
	mR[1][0]=x*y*(1-cos)+(z*sin);	mR[1][1]=y*y+(x*x+z*z)*cos;		mR[1][2]=y*z*(1-cos)-(x*sin);	mR[1][3]=(b*(x*x+z*z)-y*(a*x+c*z))*(1-cos)+(c*x-a*z)*sin;
	mR[2][0]=x*z*(1-cos)-(y*sin);	mR[2][1]=y*z*(1-cos)+(x*sin);	mR[2][2]=z*z+(x*x+y*y)*cos;		mR[2][3]=(c*(x*x+y*y)-z*(a*x+b*y))*(1-cos)+(a*y-b*x)*sin;

	return mR;
}

Matrix Matrix::LookAt(Vector eye, Vector target, Vector up, bool lh = true)
{
	Vector zAxis = (eye - target).Normalized();
	Vector xAxis = up.CrossProduct(zAxis).Normalized();
	Vector yAxis = lh ? zAxis.CrossProduct(xAxis) : xAxis.CrossProduct(zAxis);

	float mat[] = {xAxis.x, yAxis.x, zAxis.x, 0, xAxis.y, yAxis.y, zAxis.y, 0, xAxis.z, yAxis.z, zAxis.z, 0,
		-xAxis.DotProduct(eye), -yAxis.DotProduct(eye), -zAxis.DotProduct(eye), 1};

	return Matrix(mat);
}