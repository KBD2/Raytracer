#pragma once

#include "camera.hpp"

class Object
{
public:
	Coords position;
	int radius;

	Object()
	{
		position = Coords(0, 0, 0);
		radius = 0;
	}

	Object(Coords pos, int rad)
	{
		position = pos;
		radius = rad;
	}
};