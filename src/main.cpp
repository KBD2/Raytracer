#include <iostream>
#include <thread>
#include <cassert>
#include <mutex>

#include "camera.hpp"
#include "bitmap.hpp"
#include "raycast.hpp"
#include "conmanip.h"
#include "object.hpp"
#include "materials.hpp"

const int width = 640;
const int height = 480;
const int BLOCK_SIZE = 50;

const int numBlocksX = std::ceil((double)width / BLOCK_SIZE);
const int numBlocksY = std::ceil((double)height / BLOCK_SIZE);
const int numBlocks = numBlocksX * numBlocksY;

auto matDiffuse = std::make_shared<MatDiffuse>();
auto matMetal = std::make_shared<MatMetal>();
auto matMetalFuzz = std::make_shared<MatMetalFuzz>();

std::mutex blockAssignMutex;
bool* blocks;

void doPart(int number, Bitmap& image, Camera& camera, std::vector<Object>& objects, std::vector<Light>& lights)
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
				std::cout << conmanip::setpos(blockX * 2, blockY) << number;
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
						Vec3 unit = Vec3(
							std::sin(rayDelta.pitch) * std::cos(rayDelta.yaw),
							std::cos(rayDelta.pitch),
							std::sin(rayDelta.pitch) * std::sin(rayDelta.yaw)
						);
						calculated += raycast(Ray(camera.pos, unit), objects, lights, MAX_BOUNCES);
					}
				}
				calculated /= 9.0; // 9 AA samples needs to be divided to get the average
				image.setPixel(x + dX, y + dY, calculated.map(std::sqrt)); // We correct the brightness by taking the root
			}
		}
		blockAssignMutex.lock();
		std::cout << conmanip::setpos(blockX * 2, blockY) << "#";
		blockAssignMutex.unlock();
	}
}

int main()
{
	Camera camera(Coords(0, 45, -50), Coords(0, 30, 100), 90, width, height);

	Bitmap image(width, height);

	blocks = new bool[numBlocks];
	memset(blocks, 0, numBlocks * sizeof(bool));

	for (int y = 0; y < numBlocksY; y++)
	{
		for (int x = 0; x < numBlocksX; x++)
		{
			std::cout << "X ";
		}
		std::cout << std::endl;
	}
	std::cout << "X: Unrendered\n#: Rendered\nNumber: Thread ID rendering" << std::endl;

	std::vector<Object> objects;
	objects.push_back(Object(Coords(0, 30, 100), 30, Colour(1.0, 0.0, 0.0), matMetalFuzz));
	objects.push_back(Object(Coords(30, 30, 50), 20, Colour(0.5, 0.7, 0.3), matDiffuse));
	objects.push_back(Object(Coords(-60, 10, 175), 10, Colour(0.3, 0.0, 0.3), matMetal));

	std::vector<Light> lights;
	lights.push_back(Light(Coords(-20, 10, 50), 5, Colour(0.996, 0.773, 0.557), 100.0));

	// Arrays? What are those?

	std::thread t0(doPart, 0, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));
	std::thread t1(doPart, 1, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));
	std::thread t2(doPart, 2, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));
	std::thread t3(doPart, 3, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));
	std::thread t4(doPart, 4, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));
	std::thread t5(doPart, 5, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));
	std::thread t6(doPart, 6, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));
	std::thread t7(doPart, 7, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));

	t0.join();
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();
	t7.join();

	image.save("out.bmp");

	delete[] blocks;
}
