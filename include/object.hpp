#pragma once

#include "camera.hpp"

class Object
{
public:
	Coords pos;
	int rad;
	Colour col;

	Object()
	{
		pos = Coords(0, 0, 0);
		rad = 0;
	}

	Object(Coords _pos, int _rad, Colour _colour)
	{
		pos = _pos;
		rad = _rad;
		col = _colour;
	}
};