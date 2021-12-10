#pragma once

#include <cstdint>
#include <string>

class Colour
{
public:
	double r, g, b;

	Colour()
	{
		r = 0.0;
		g = 0.0;
		b = 0.0;
	}

	Colour(double _r, double _g, double _b)
	{
		r = _r;
		g = _g;
		b = _b;
	}

	Colour operator*(double n);
	void operator*=(double n);
	Colour operator/(double n);
	void operator+=(Colour n);
};

class Bitmap
{
public:
	unsigned int width;
	unsigned int height;

	Bitmap(unsigned int _width, unsigned int _height)
	{
		width = _width;
		height = _height;
		data = new Colour[(uint64_t)(width) * height];
	}

	~Bitmap()
	{
		delete[] data;
	}

	void setPixel(unsigned int x, unsigned int y, Colour colour);

	bool save(std::string filename);

private:
	Colour* data;
};