#include <cmath>

#include "vec3.hpp"

Vec3 Vec3::operator/(double divisor)
{
	return Vec3(x / divisor, y / divisor, z / divisor);
}

Vec3 Vec3::operator-(Vec3 n)
{
	return Vec3(x - n.x, y - n.y, z - n.z);
}

Vec3 Vec3::operator+(Vec3 n)
{
	return Vec3(x + n.x, y + n.y, z + n.z);
}

void Vec3::operator+=(Vec3 n)
{
	*this = *this + n;
}

double Vec3::length()
{
	return std::sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
}
Vec3 Vec3::unit()
{
	return *this / length();
}

double Vec3::distance(Vec3 b)
{
	return std::sqrt(std::pow(x - b.x, 2) + std::pow(y - b.y, 2) + std::pow(z - b.z, 2));
}

Vec3 Vec3::operator*(double n)
{
	return Vec3(x * n, y * n, z * n);
}

double Vec3::dot(Vec3 b)
{
	return x * b.x + y * b.y + z * b.z;
}

bool Vec3::operator==(Vec3 b)
{
	return x == b.x && y == b.y && z == b.z;
}

bool Vec3::operator!=(Vec3 b)
{
	return x != b.x || y != b.y || z != b.z;
}

Vec3 Vec3::operator-()
{
	return Vec3(-x, -y, -z);
}