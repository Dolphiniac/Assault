#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#include <math.h>
#include <algorithm>


//-----------------------------------------------------------------------------------------------
Matrix44 Matrix44::Identity;
Matrix44 Matrix44::ZForward = Matrix44::MakeZForwardBasis();


//-----------------------------------------------------------------------------------------------
void Matrix44::Transpose()
{
	std::swap(data[1], data[4]);
	std::swap(data[2], data[8]);
	std::swap(data[3], data[12]);
	std::swap(data[6], data[9]);
	std::swap(data[7], data[13]);
	std::swap(data[11], data[14]);
}


//-----------------------------------------------------------------------------------------------
void Matrix44::SetRows(const Vector4& one, const Vector4& two, const Vector4& three, const Vector4& four)
{
	data[0] = one.x;	data[1] = one.y;	data[2] = one.z;	data[3] = one.w;
	data[4] = two.x;	data[5] = two.y;	data[6] = two.z;	data[7] = two.w;
	data[8] = three.x;	data[9] = three.y;	data[10] = three.z;	data[11] = three.w;
	data[12] = four.x;	data[13] = four.y;	data[14] = four.z;	data[15] = four.w;
}


//-----------------------------------------------------------------------------------------------
Vector3 Matrix44::GetForward() const
{
	Vector3 result(data[8], data[9], data[10]);

	return result;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::SetForward(const Vector3& forward)
{
	data[8] = forward.x;
	data[9] = forward.y;
	data[10] = forward.z;
}

//-----------------------------------------------------------------------------------------------
void Matrix44::MakeProjOrthogonal(float nx, float fx, float ny, float fy, float nz, float fz)
{
	float sx = 1.0f / (fx - nx);
	float sy = 1.0f / (fy - ny);
	float sz = 1.0f / (fz - nz);

	ToIdentity();

	data[0] = 2.f * sx;
	data[5] = 2.f * sy;
	data[10] = 2.f * sz;
	data[12] = -(fx + nx) * sx;
	data[13] = -(fy + ny) * sy;
	data[14] = -(fz + nz) * sz;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::MakeProjOrthogonal(float width, float height, float nz, float fz)
{
	float sz = 1.0f / (fz - nz);
	float tx = -1.f;
	float ty = -1.f;
	float tz = -(nz + fz) * sz;

	ToIdentity();

	data[0] = 2.f / width;
	data[5] = 2.f / height;
	data[10] = 2.f * sz;
	data[12] = tx;
	data[13] = ty;
	data[14] = tz;
	//data[14] = -(fz + nz) * sz;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::MakeOrthographic(float width, float height)
{
	MakeProjOrthogonal(width, height, 0.f, .5f);
}


//-----------------------------------------------------------------------------------------------
void Matrix44::MakePerspective(float fovDegreesY, float aspect, float nz, float fz)
{
	float rads = fovDegreesY * DEG2RAD;
	float size = 1.f / (float)tan(rads * .5f);

	float w = size;
	float h = size;
	if (aspect > 1.f)
	{
		w *= aspect;
	}
	else
	{
		h /= aspect;
	}

	float q = 1.f / (fz - nz);

	ToIdentity();

	data[0] = 1.f / w;
	data[5] = 1.f / h;
	data[10] = (fz + nz) * q;
	data[11] = 1.f;
	data[14] = -2.f * nz * fz * q;
	data[15] = 0.f;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::SetTranslation(const Vector3& trans)
{
	data[12] = trans.x;
	data[13] = trans.y;
	data[14] = trans.z;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::SetRotation(float yaw, float pitch, float roll)
{
	Matrix44 yawMat;
	yawMat.MakeRotationY(yaw);

	Matrix44 pitchMat;
	pitchMat.MakeRotationX(pitch);

	Matrix44 rollMat;
	rollMat.MakeRotationZ(roll);

	Matrix44 resultant = rollMat * pitchMat * yawMat;

	data[0] = resultant.data[0];
	data[1] = resultant.data[1];
	data[2] = resultant.data[2];

	data[4] = resultant.data[4];
	data[5] = resultant.data[5];
	data[6] = resultant.data[6];

	data[8] = resultant.data[8];
	data[9] = resultant.data[9];
	data[10] = resultant.data[10];
}


//-----------------------------------------------------------------------------------------------
void Matrix44::SetUniformScale(float uniformScale)
{
	Matrix44 scaleMat;

	scaleMat.data[0] = uniformScale;
	scaleMat.data[5] = uniformScale;
	scaleMat.data[10] = uniformScale;

	*this = scaleMat * *this;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::SetNonUniformScale(const Vector3& scale)
{
	Matrix44 scaleMat;
	scaleMat.data[0] = scale.x;
	scaleMat.data[5] = scale.y;
	scaleMat.data[10] = scale.z;

	*this = scaleMat * *this;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::MakeRotationX(float degrees)
{
	ToIdentity();

	float c = CosDegrees(degrees);
	float s = SinDegrees(degrees);

	data[5] = c;
	data[6] = s;
	data[9] = -s;
	data[10] = c;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::MakeRotationY(float degrees)
{
	ToIdentity();

	float c = CosDegrees(degrees);
	float s = SinDegrees(degrees);

	data[0] = c;
	data[2] = s;
	data[8] = -s;
	data[10] = c;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::MakeRotationZ(float degrees)
{
	ToIdentity();

	float c = CosDegrees(degrees);
	float s = SinDegrees(degrees);

	data[0] = c;
	data[1] = s;
	data[4] = -s;
	data[5] = c;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::MakeTransformationMatrix(float uniformScale, const Vector3& eulerRot, const Vector3& translation)
{
	ToIdentity();
	SetRotation(eulerRot.yaw, eulerRot.pitch, eulerRot.roll);
	SetTranslation(translation);
	SetUniformScale(uniformScale);
}


//-----------------------------------------------------------------------------------------------
Matrix44 Matrix44::operator*(const Matrix44& rightMat) const
{
	Matrix44 resultant;

	resultant.data[0] = data[0] * rightMat.data[0] + data[1] * rightMat.data[4] + data[2] * rightMat.data[8] + data[3] * rightMat.data[12];
	resultant.data[1] = data[0] * rightMat.data[1] + data[1] * rightMat.data[5] + data[2] * rightMat.data[9] + data[3] * rightMat.data[13];
	resultant.data[2] = data[0] * rightMat.data[2] + data[1] * rightMat.data[6] + data[2] * rightMat.data[10] + data[3] * rightMat.data[14];
	resultant.data[3] = data[0] * rightMat.data[3] + data[1] * rightMat.data[7] + data[2] * rightMat.data[11] + data[3] * rightMat.data[15];

	resultant.data[4] = data[4] * rightMat.data[0] + data[5] * rightMat.data[4] + data[6] * rightMat.data[8] + data[7] * rightMat.data[12];
	resultant.data[5] = data[4] * rightMat.data[1] + data[5] * rightMat.data[5] + data[6] * rightMat.data[9] + data[7] * rightMat.data[13];
	resultant.data[6] = data[4] * rightMat.data[2] + data[5] * rightMat.data[6] + data[6] * rightMat.data[10] + data[7] * rightMat.data[14];
	resultant.data[7] = data[4] * rightMat.data[3] + data[5] * rightMat.data[7] + data[6] * rightMat.data[11] + data[7] * rightMat.data[15];

	resultant.data[8] = data[8] * rightMat.data[0] + data[9] * rightMat.data[4] + data[10] * rightMat.data[8] + data[11] * rightMat.data[12];
	resultant.data[9] = data[8] * rightMat.data[1] + data[9] * rightMat.data[5] + data[10] * rightMat.data[9] + data[11] * rightMat.data[13];
	resultant.data[10] = data[8] * rightMat.data[2] + data[9] * rightMat.data[6] + data[10] * rightMat.data[10] + data[11] * rightMat.data[14];
	resultant.data[11] = data[8] * rightMat.data[3] + data[9] * rightMat.data[7] + data[10] * rightMat.data[11] + data[11] * rightMat.data[15];

	resultant.data[12] = data[12] * rightMat.data[0] + data[13] * rightMat.data[4] + data[14] * rightMat.data[8] + data[15] * rightMat.data[12];
	resultant.data[13] = data[12] * rightMat.data[1] + data[13] * rightMat.data[5] + data[14] * rightMat.data[9] + data[15] * rightMat.data[13];
	resultant.data[14] = data[12] * rightMat.data[2] + data[13] * rightMat.data[6] + data[14] * rightMat.data[10] + data[15] * rightMat.data[14];
	resultant.data[15] = data[12] * rightMat.data[3] + data[13] * rightMat.data[7] + data[14] * rightMat.data[11] + data[15] * rightMat.data[15];

	return resultant;
}


//-----------------------------------------------------------------------------------------------
Matrix44 Matrix44::operator*(float scalar) const
{
	Matrix44 result;
	result.data[0] = data[0] * scalar;
	result.data[1] = data[1] * scalar;
	result.data[2] = data[2] * scalar;
	result.data[3] = data[3] * scalar;
	result.data[4] = data[4] * scalar;
	result.data[5] = data[5] * scalar;
	result.data[6] = data[6] * scalar;
	result.data[7] = data[7] * scalar;
	result.data[8] = data[8] * scalar;
	result.data[9] = data[9] * scalar;
	result.data[10] = data[10] * scalar;
	result.data[11] = data[11] * scalar;
	result.data[12] = data[12] * scalar;
	result.data[13] = data[13] * scalar;
	result.data[14] = data[14] * scalar;
	result.data[15] = data[15] * scalar;

	return result;
}


//-----------------------------------------------------------------------------------------------
Matrix44 operator*(float scalar, const Matrix44& mat)
{
	Matrix44 result = mat;
	result *= scalar;
	return result;
}


//-----------------------------------------------------------------------------------------------
Matrix44 Matrix44::operator+(const Matrix44& rightMat) const
{
	Matrix44 result;
	for (int i = 0; i < 16; i++)
	{
		result[i] = data[i] + rightMat.data[i];
	}

	return result;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::operator*=(float scalar)
{
	*this = *this * scalar;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::operator=(const Matrix44& otherMat)
{
	for (int i = 0; i < 16; i++)
	{
		data[i] = otherMat.data[i];
	}
}


//-----------------------------------------------------------------------------------------------
float& Matrix44::operator[](int index)
{
	return data[index];
}


//-----------------------------------------------------------------------------------------------
void Matrix44::MakeLookAt(const Vector3& pos, const Vector3& up, const Vector3& target)
{
	(void)pos;
	(void)up;
	(void)target;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::InvertOrthonormal()
{
	std::swap(data[1], data[4]);
	std::swap(data[2], data[8]);
	std::swap(data[6], data[9]);

	Matrix44 rot = *this;
	rot.data[12] = rot.data[13] = rot.data[14] = 0.f;
	Matrix44 trans;
	trans.data[12] = -data[12];
	trans.data[13] = -data[13];
	trans.data[14] = -data[14];

	ToIdentity();
	*this = trans * rot;
}


//-----------------------------------------------------------------------------------------------
Matrix44 Matrix44::MakeZForwardBasis()
{
	Matrix44 result;
	result.data[10] = -1.f;

	return result;
}


//-----------------------------------------------------------------------------------------------
Matrix44 Matrix44::Inverse() const
{
	float determinant =
		data[0] * data[5] * data[10] * data[15] +
		data[0] * data[6] * data[11] * data[13] +
		data[0] * data[7] * data[9] * data[14] +
		data[1] * data[4] * data[11] * data[14] +
		data[1] * data[6] * data[8] * data[15] +
		data[1] * data[7] * data[10] * data[12] +
		data[2] * data[4] * data[9] * data[15] +
		data[2] * data[5] * data[11] * data[12] +
		data[2] * data[7] * data[8] * data[13] +
		data[3] * data[4] * data[10] * data[13] +
		data[3] * data[5] * data[8] * data[14] +
		data[3] * data[6] * data[9] * data[12] -
		data[0] * data[5] * data[11] * data[14] -
		data[0] * data[6] * data[9] * data[15] -
		data[0] * data[7] * data[10] * data[13] -
		data[1] * data[4] * data[10] * data[15] -
		data[1] * data[6] * data[11] * data[12] -
		data[1] * data[7] * data[8] * data[14] -
		data[2] * data[4] * data[11] * data[13] -
		data[2] * data[5] * data[8] * data[15] -
		data[2] * data[7] * data[9] * data[12] -
		data[3] * data[4] * data[9] * data[14] -
		data[3] * data[5] * data[10] * data[12] -
		data[3] * data[6] * data[8] * data[13];
	ASSERT_OR_DIE(determinant != 0, "Cannot invert this matrix");

	Matrix44 b;
	b.data[0] =
		data[5] * data[10] * data[15] +
		data[6] * data[11] * data[13] +
		data[7] * data[9] * data[14] -
		data[5] * data[11] * data[14] -
		data[6] * data[9] * data[15] -
		data[7] * data[10] * data[13];

	b.data[1] =
		data[1] * data[11] * data[14] +
		data[2] * data[9] * data[15] +
		data[3] * data[10] * data[13] -
		data[1] * data[10] * data[15] -
		data[2] * data[11] * data[13] -
		data[3] * data[9] * data[14];

	b.data[2] =
		data[1] * data[6] * data[15] +
		data[2] * data[7] * data[13] +
		data[3] * data[5] * data[14] -
		data[1] * data[7] * data[14] -
		data[2] * data[5] * data[15] -
		data[3] * data[6] * data[13];

	b.data[3] =
		data[1] * data[7] * data[10] +
		data[2] * data[5] * data[11] +
		data[3] * data[6] * data[9] -
		data[1] * data[6] * data[11] -
		data[2] * data[7] * data[9] -
		data[3] * data[5] * data[10];

	b.data[4] =
		data[4] * data[11] * data[14] +
		data[6] * data[8] * data[15] +
		data[7] * data[10] * data[12] -
		data[4] * data[10] * data[15] -
		data[6] * data[11] * data[12] -
		data[7] * data[8] * data[14];

	b.data[5] =
		data[0] * data[10] * data[15] +
		data[2] * data[11] * data[12] +
		data[3] * data[8] * data[14] -
		data[0] * data[11] * data[14] -
		data[2] * data[8] * data[15] -
		data[3] * data[10] * data[12];

	b.data[6] =
		data[0] * data[7] * data[14] +
		data[2] * data[4] * data[15] +
		data[3] * data[6] * data[12] -
		data[0] * data[6] * data[15] -
		data[2] * data[7] * data[12] -
		data[3] * data[4] * data[14];

	b.data[7] =
		data[0] * data[6] * data[11] +
		data[2] * data[7] * data[8] +
		data[3] * data[4] * data[10] -
		data[0] * data[7] * data[10] -
		data[2] * data[4] * data[11] -
		data[3] * data[6] * data[8];

	b.data[8] =
		data[4] * data[9] * data[15] +
		data[5] * data[11] * data[12] +
		data[7] * data[8] * data[13] -
		data[4] * data[11] * data[13] -
		data[5] * data[8] * data[15] -
		data[7] * data[9] * data[12];

	b.data[9] =
		data[0] * data[11] * data[13] +
		data[1] * data[8] * data[15] +
		data[3] * data[9] * data[12] -
		data[0] * data[9] * data[15] -
		data[1] * data[11] * data[12] -
		data[3] * data[8] * data[13];

	b.data[10] =
		data[0] * data[5] * data[15] +
		data[1] * data[7] * data[12] +
		data[3] * data[4] * data[13] -
		data[0] * data[7] * data[13] -
		data[1] * data[4] * data[15] -
		data[3] * data[5] * data[12];

	b.data[11] =
		data[0] * data[7] * data[9] +
		data[1] * data[4] * data[11] +
		data[3] * data[5] * data[8] -
		data[0] * data[5] * data[11] -
		data[1] * data[7] * data[8] -
		data[3] * data[4] * data[9];

	b.data[12] =
		data[4] * data[10] * data[13] +
		data[5] * data[8] * data[14] +
		data[6] * data[9] * data[12] -
		data[4] * data[9] * data[14] -
		data[5] * data[10] * data[12] -
		data[6] * data[8] * data[13];

	b.data[13] =
		data[0] * data[9] * data[14] +
		data[1] * data[10] * data[12] +
		data[2] * data[8] * data[13] -
		data[0] * data[10] * data[13] -
		data[1] * data[8] * data[14] -
		data[2] * data[9] * data[12];

	b.data[14] =
		data[0] * data[6] * data[13] +
		data[1] * data[4] * data[14] +
		data[2] * data[5] * data[12] -
		data[0] * data[5] * data[14] -
		data[1] * data[6] * data[12] -
		data[2] * data[4] * data[13];

	b.data[15] =
		data[0] * data[5] * data[10] +
		data[1] * data[6] * data[8] +
		data[2] * data[4] * data[9] -
		data[0] * data[6] * data[9] -
		data[1] * data[4] * data[10] -
		data[2] * data[5] * data[8];

	Matrix44 result;
	result = (1.f / determinant) * b;
	return result;
}


//-----------------------------------------------------------------------------------------------
void Matrix44::Invert()
{
	*this = Inverse();
}


//-----------------------------------------------------------------------------------------------
bool Matrix44::operator==(const Matrix44& otherMat) const
{
	for (int i = 0; i < 16; i++)
	{
		if (data[i] != otherMat.data[i])
		{
			return false;
		}
	}

	return true;
}


//-----------------------------------------------------------------------------------------------
bool Matrix44::operator!=(const Matrix44& otherMat) const
{
	return !(*this == otherMat);
}