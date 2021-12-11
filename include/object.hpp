#pragma once

#include <random>

#include "camera.hpp"

class Material;

class Object
{
public:
	Coords pos;
	int rad;
	Colour col;
	std::shared_ptr<Material> mat;

	Object()
	{
		pos = Coords();
		rad = 0;
		col = Colour();
		mat = NULL;
	}

	Object(Coords _pos, int _rad, Colour _colour, std::shared_ptr<Material> _mat)
		: pos(_pos), rad(_rad), col(_colour), mat(_mat) {}
};

class Light
{
public:
	Object obj;
	double intensity;

	Light(Coords _pos, int _rad, Colour _col, double _intensity)
		: obj(Object(_pos, _rad, _col, NULL)), intensity(_intensity) {}
};