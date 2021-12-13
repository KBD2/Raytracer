#pragma once

class Angle;

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
	void operator-=(Vec3 n);
	Vec3 operator+(Vec3 n);
	Vec3 operator*(double n);
	void operator*=(double n);
	void operator+=(Vec3 n);
	double length();
	Vec3 unit();
	double dist(Vec3 b);
	double dot(Vec3 b);
	bool operator==(Vec3 b);
	bool operator!=(Vec3 b);
	Vec3 operator-();
	Vec3 fromAngle(Angle angle);
};