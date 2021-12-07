#include <iostream>
#include <thread>
#include <cassert>

#include "camera.hpp"
#include "bitmap.hpp"
#include "raycast.hpp"


void doRowPart(int x, int y, Bitmap* image, Camera* camera, std::vector<Object>* objects)
{
	for (unsigned int dx = 0; dx < image->width / 4; dx++)
	{
		Colour calculated = raycast(camera, x + dx, y, objects, image->width, image->height);
		image->setPixel(x + dx, y, calculated);
	}
}

int main()
{
	const int width = 640;
	const int height = 480;

	Camera camera(Coords(0, 10, -30), Coords(0, -1, 0), 90, width, height);

	Bitmap image(width, height);

	std::vector<Object> objects;
	objects.push_back(Object(Coords(0, 0, 100), 50));

	for (int y = 0; y < height; y += 2)
	{
		std::cout << (double)y / height * 100 << "% rendered" << std::endl;

		// I don't know if this is the best way to do this
		// TODO: Configure number of threads

		std::thread t1(doRowPart, 0,				y, &image, &camera, &objects);
		std::thread t2(doRowPart, width / 4,		y, &image, &camera, &objects);
		std::thread t3(doRowPart, 2 * (width / 4),	y, &image, &camera, &objects);
		std::thread t4(doRowPart, 3 * (width / 4),	y, &image, &camera, &objects);

		std::thread t5(doRowPart, 0,				y + 1, &image, &camera, &objects);
		std::thread t6(doRowPart, width / 4,		y + 1, &image, &camera, &objects);
		std::thread t7(doRowPart, 2 * (width / 4),	y + 1, &image, &camera, &objects);
		std::thread t8(doRowPart, 3 * (width / 4),	y + 1, &image, &camera, &objects);

		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
		t6.join();
		t7.join();
		t8.join();
	}

	image.save("out.bmp");
}
