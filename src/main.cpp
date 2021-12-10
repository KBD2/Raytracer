#include <iostream>
#include <thread>
#include <cassert>
#include <mutex>

#include "camera.hpp"
#include "bitmap.hpp"
#include "raycast.hpp"

const int width = 640;
const int height = 480;
const int BLOCK_SIZE = 50;

const int numBlocksX = std::ceil((double)width / BLOCK_SIZE);
const int numBlocksY = std::ceil((double)height / BLOCK_SIZE);
const int numBlocks = numBlocksX * numBlocksY;

std::mutex blockAssignMutex;
bool* blocks;

void doPart(Bitmap& image, Camera& camera, std::vector<Object>& objects)
{
	while (true)
	{
		int blockX = -1;
		int blockY = -1;
		blockAssignMutex.lock();
		for (int block = 0; block < numBlocks; block++)
		{
			if (!blocks[block])
			{
				blockX = block % numBlocksX;
				blockY = block / numBlocksX;
				blocks[block] = true;
				break;
			}
		}
		blockAssignMutex.unlock();
		if (blockX == -1) return;

		unsigned int x = blockX * BLOCK_SIZE;
		unsigned int y = blockY * BLOCK_SIZE;

		for (unsigned int dY = 0; dY < BLOCK_SIZE; dY++)
		{
			if (y + dY >= image.height) break;
			for (unsigned int dX = 0; dX < BLOCK_SIZE; dX++)
			{
				if (x + dX >= image.width) break;
				Colour calculated(0.0, 0.0, 0.0);
				double dTheta = -camera.fovHoriz / image.width * ((double)x + dX);
				double dPhi = camera.fovVert / image.height * ((double)y + dY);
				Angle ray = Angle().fromVec3(camera.viewplaneTL).delta(dTheta, dPhi);

				double deltaAngleH = camera.fovHoriz / image.width / 2;
				double deltaAngleV = camera.fovVert / image.width / 2;
				for (double aaDY = -deltaAngleV; aaDY <= deltaAngleV; aaDY += deltaAngleV)
				{
					for (double aaDX = -deltaAngleH; aaDX <= deltaAngleH; aaDX += deltaAngleH)
					{
						Angle rayDelta = ray.delta(aaDX, aaDY) / 180 * pi;
						// Get the angle to the top left of the viewplane and add a fraction of the FOV to it
						Vec3 unit = Vec3(std::sin(rayDelta.pitch) * std::cos(rayDelta.yaw), cos(rayDelta.pitch), sin(rayDelta.pitch) * sin(rayDelta.yaw));
						calculated += raycast(Ray(camera.pos, unit), objects);
					}
				}
				image.setPixel(x + dX, y + dY, Colour(std::sqrt(calculated.r / 9.0), std::sqrt(calculated.g / 9.0), std::sqrt(calculated.b / 9.0)));
			}
		}

	}
}

int main()
{
	Camera camera(Coords(0, 45, -50), Coords(0, 30, 100), 90, width, height);

	Bitmap image(width, height);

	blocks = new bool[numBlocks];
	memset(blocks, 0, numBlocks * sizeof(bool));

	std::vector<Object> objects;
	objects.push_back(Object(Coords(0, 30, 100), 30, Colour(1.0, 0.0, 0.0)));
	objects.push_back(Object(Coords(30, 30, 50), 20, Colour(0.0, 0.0, 1.0)));

	std::thread t1(doPart, std::ref(image), std::ref(camera), std::ref(objects));
	std::thread t2(doPart, std::ref(image), std::ref(camera), std::ref(objects));
	std::thread t3(doPart, std::ref(image), std::ref(camera), std::ref(objects));
	std::thread t4(doPart, std::ref(image), std::ref(camera), std::ref(objects));
	std::thread t5(doPart, std::ref(image), std::ref(camera), std::ref(objects));
	std::thread t6(doPart, std::ref(image), std::ref(camera), std::ref(objects));
	std::thread t7(doPart, std::ref(image), std::ref(camera), std::ref(objects));
	std::thread t8(doPart, std::ref(image), std::ref(camera), std::ref(objects));

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();
	t8.join();

	image.save("out.bmp");

	delete[] blocks;
}
