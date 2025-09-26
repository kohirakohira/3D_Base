#pragma once
#include <cmath>

//x,y,z²‚ğ‚Æ‚éA‰Šú‰»‚µ‚È‚¢‚Æ0
struct Vector3
{
	float x, y, z;

	//ˆø”‚ğ“ü—Í‚¹‚¸‚É‚¢‚é‚ÆA0‚É‚È‚é‚æ.
	Vector3(float xi = 0, float yi = 0, float zi = 0) :x(xi), y(yi), z(zi)
	{
	};

	//operator‚ÌŸ‚Ì‰‰Zq‚Å‚»‚ê‚¼‚ê{}‚Ì’†‚ğ’Ê‚é‚æ‚¤‚É‚È‚é.
	Vector3 operator+ (const Vector3& other)const
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 operator- (const Vector3& other)const
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	//‚±‚ÌVector3‚ÆVector3‚Å‰~‚Ì’¼ü‹——£.
	float Distance(const Vector3& other)const;
};