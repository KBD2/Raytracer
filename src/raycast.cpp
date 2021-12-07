#include <cmath>

#include "raycast.hpp"

Colour raycast(Camera* camera, unsigned int col, unsigned int row, std::vector<Object>* objects, unsigned int width, unsigned int height)
{
	Vec3 deltaRow = (camera->viewplaneTR - camera->viewplaneTL) / width * col;
	Vec3 deltaCol = (camera->viewplaneBL - camera->viewplaneTL) / height * row;

	Vec3 viewplanePoint = camera->viewplaneTL + deltaRow + deltaCol;


	Vec3 delta = (viewplanePoint - camera->pos).unit() / 2;
	Coords rayPoint = camera->pos;
	Colour calculated = Colour(0x1E, 0x90, 0xFF);

	for (int step = 0; step < steps; step++)
	{
		bool stop = false;
		for (auto& obj : *objects)
		{
			if (rayPoint.distance(obj.position) < obj.radius)
			{
				calculated = Colour(0xFF, 0x00, 0x00);
				stop = true;
				break;
			}
		}
		if (stop) break;
		if (rayPoint.y < 0)
		{
			calculated = Colour(0x00, 0xFF, 0x00);
			stop = true;
		}
		if (stop) break;
		rayPoint += delta;
	}

	return calculated;
}