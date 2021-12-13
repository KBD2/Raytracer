#include <cmath>

#include "vec3.hpp"
#include "camera.hpp"

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

double Vec3::dist(Vec3 b)
{
	return std::sqrt(std::pow(b.x - x, 2) + std::pow(b.y - y, 2) + std::pow(b.z - z, 2));
}

Vec3 Vec3::operator*(double n)
{
	return Vec3(x * n, y * n, z * n);
}

void Vec3::operator*=(double n)
{
	x *= n;
	y *= n;
	z *= n;
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

void Vec3::operator-=(Vec3 n)
{
	x -= n.x;
	y -= n.y;
	z -= n.z;
}

Vec3 Vec3::fromAngle(Angle angle)
{
	x = std::sin(angle.pitch) * std::cos(angle.yaw);
	y = std::cos(angle.pitch);
	z = std::sin(angle.pitch) * std::sin(angle.yaw);

	return *this;
}