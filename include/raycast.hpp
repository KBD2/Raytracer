#pragma once

#include <vector>

#include "camera.hpp"
#include "bitmap.hpp"
#include "object.hpp"

const int MAX_BOUNCES = 50;
const double IMPRECISION_DELTA = 0.001;

class Ray
{
public:
	Coords orig;
	Vec3 dir;

	Ray(Coords _orig, Vec3 _dir)
	{
		orig = _orig;
		dir = _dir;
	}
};

Colour raycast(Ray ray, std::vector<Object>& objects);