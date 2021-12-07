class Coords
{
public:
	double x, y, z;

	Coords()
	{
		x = 0.0;
		y = 0.0;
		z = 0.0;
	}

	Coords(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
};

class Camera
{
public:
	Coords pos;
	Coords viewplaneStart, viewplaneEnd;

	Camera(Coords coords, Coords _viewplaneStart, int width, int height, bool alongX)
	{
		pos = coords;
		viewplaneStart = _viewplaneStart;
		viewplaneEnd.x = viewplaneStart.x + alongX ? width : 0;
		viewplaneEnd.y = viewplaneStart.y + alongX ? 0 : width;
		viewplaneEnd.z = viewplaneStart.z - height;
	}
};