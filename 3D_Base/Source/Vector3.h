#pragma once
#include <cmath>

//x,y,z�����Ƃ�A���������Ȃ���0
struct Vector3
{
	float x, y, z;

	//��������͂����ɂ���ƁA0�ɂȂ��.
	Vector3(float xi = 0, float yi = 0, float zi = 0) :x(xi), y(yi), z(zi)
	{
	};

	//operator�̎��̉��Z�q�ł��ꂼ��{}�̒���ʂ�悤�ɂȂ�.
	Vector3 operator+ (const Vector3& other)const
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 operator- (const Vector3& other)const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	//����Vector3��Vector3�ŉ~�̒�������.
	float Distance(const Vector3& other)const;
};