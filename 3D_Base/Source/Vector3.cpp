#include "Vector3.h"

float Vector3::Distance(const Vector3& other) const
{
	float dx = x - other.x;
	float dy = y - other.y;
	float dz = z - other.z;

	//‚»‚ê‚¼‚ê‚ğ2æ‚µ‚Äã‚µ‚Ä“n‚·.
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}