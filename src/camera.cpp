#include <cmath>

#include "camera.hpp"

Angle Angle::fromVec3(Vec3 pos)
{
	yaw = std::atan2(pos.z, pos.x) * 180 / pi;
	double adjacent = std::sqrt(std::pow(pos.x, 2) + std::pow(pos.z, 2));
	pitch = std::atan2(adjacent, pos.y) * 180 / pi;
	return *this;
}

Angle Angle::delta(double dYaw, double dPitch)
{
	return Angle(yaw + dYaw, pitch + dPitch);
}

Angle Angle::operator+(Angle n)
{
	return Angle(yaw + n.yaw, pitch + n.pitch);
}

Angle Angle::operator-(Angle n)
{
	return Angle(yaw - n.yaw, pitch - n.pitch);
}

void Angle::operator+=(Angle n)
{
	*this = *this + n;
}

Angle Angle::operator/(double n)
{
	return Angle(yaw / n, pitch / n);
}

void Angle::operator/=(double divisor)
{
	yaw /= divisor;
	pitch /= divisor;
}

Angle Angle::operator*(double n)
{
	return Angle(yaw * n, pitch * n);
}

void Angle::operator*=(double n)
{
	yaw *= n;
	pitch *= n;
}