#include <cstdint>
#include <string>

class Colour
{
public:
	unsigned char r, g, b;

	Colour()
	{
		r = 0;
		g = 0;
		b = 0;
	}

	Colour(unsigned char _r, unsigned char _g, unsigned char _b)
	{
		r = _r;
		g = _g;
		b = _b;
	}
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