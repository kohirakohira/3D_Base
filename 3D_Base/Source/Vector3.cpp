#include "Vector3.h"

float Vector3::Distance(const Vector3& other) const
{
	float dx = x - other.x;
	float dy = y - other.y;
	float dz = z - other.z;

	//���ꂼ���2�悵�āサ�ēn��.
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}