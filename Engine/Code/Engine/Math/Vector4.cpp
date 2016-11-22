#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vector3.hpp"


//-----------------------------------------------------------------------------------------------
Vector4 Vector4::Zero = Vector4(0.f, 0.f, 0.f, 0.f);


//-----------------------------------------------------------------------------------------------
Vector4 Vector4::operator*(const Matrix44& mat) const
{
	Vector4 resultant;
	resultant.x = x * mat.data[0] + y * mat.data[4] + z * mat.data[8] + w * mat.data[12];
	resultant.y = x * mat.data[1] + y * mat.data[5] + z * mat.data[9] + w * mat.data[13];
	resultant.z = x * mat.data[2] + y * mat.data[6] + z * mat.data[10] + w * mat.data[14];
	resultant.w = x * mat.data[3] + y * mat.data[7] + z * mat.data[11] + w * mat.data[15];

	return resultant;
}


//-----------------------------------------------------------------------------------------------
void Vector4::operator=(const Vector4& rightVec)
{
	x = rightVec.x;
	y = rightVec.y;
	z = rightVec.z;
	w = rightVec.w;
}


//-----------------------------------------------------------------------------------------------
Vector4 Vector4::operator-(const Vector4& rightVec)
{
	return Vector4(x - rightVec.x, y - rightVec.y, z - rightVec.z, w - rightVec.w);
}


//-----------------------------------------------------------------------------------------------
void Vector4::operator*=(const Matrix44& mat)
{
	*this = *this * mat;
}


//-----------------------------------------------------------------------------------------------
void Vector4::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
}


//-----------------------------------------------------------------------------------------------
Vector3 Vector4::XYZ() const
{
	return Vector3(x, y, z);
}


//-----------------------------------------------------------------------------------------------
float Vector4::Dot(const Vector4& leftVec, const Vector4& rightVec)
{
	return leftVec.x * rightVec.x + leftVec.y * rightVec.y + leftVec.z * rightVec.z + leftVec.w * rightVec.w;
}