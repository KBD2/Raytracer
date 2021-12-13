#pragma once

#include <vector>

#include "camera.hpp"
#include "bitmap.hpp"
#include "object.hpp"

extern int maxBounces;
const double IMPRECISION_DELTA = 0.001;
extern int randomBounces;

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

Colour raycast(Ray ray, std::vector<Object>& objects, std::vector<Light>& lights, int depth);