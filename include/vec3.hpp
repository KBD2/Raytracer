#pragma once

#include "vec3.hpp"

class Vec3
{
public:
	double x, y, z;

	Vec3()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	Vec3(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vec3 operator/(double divisor);
	Vec3 operator-(Vec3 n);
	Vec3 operator+(Vec3 n);
	Vec3 operator*(double n);
	void operator+=(Vec3 n);
	double length();
	Vec3 unit();
	double distance(Vec3 b);
};