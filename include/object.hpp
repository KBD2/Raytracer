#pragma once

#include <random>

#include "camera.hpp"

class Material;
class Shape;

class Object
{
public:
	Shape* shape;
	Colour col;
	Material* mat;


	Object(Shape* _shape, Colour _colour, Material* _mat)
		: shape(_shape), col(_colour), mat(_mat) {}
};

class Light
{
public:
	Object obj;
	double intensity;
};