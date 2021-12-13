#include <iostream>
#include <thread>
#include <cassert>
#include <mutex>
#include <fstream>

#include "camera.hpp"
#include "bitmap.hpp"
#include "raycast.hpp"
#include "conmanip.h"
#include "object.hpp"
#include "materials.hpp"
#include "json.h"

int randomBounces = 1;
int maxBounces = 5;
int width = 640;
int height = 480;
int blockSize = 50;

int numBlocksX;
int numBlocksY;
int numBlocks;

int conOffsetX, conOffsetY;

auto matDiffuse = std::make_shared<MatDiffuse>();
auto matMetal = std::make_shared<MatMetal>();
auto matMetalFuzz = std::make_shared<MatMetalFuzz>();
auto matGlass = std::make_shared<MatGlass>();

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
				std::cout
					<< conmanip::setpos(conOffsetX + blockX * 2, conOffsetY + blockY)
					<< conmanip::settextcolor(conmanip::console_text_colors::yellow)
					<< number
					<< conmanip::settextcolor(conmanip::console_text_colors::white);
				break;
			}
		}
		blockAssignMutex.unlock();
		if (blockX == -1) return;

		unsigned int x = blockX * blockSize;
		unsigned int y = blockY * blockSize;

		for (int dY = 0; dY < blockSize; dY++)
		{
			if (y + dY >= image.height) break;
			for (int dX = 0; dX < blockSize; dX++)
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
						Vec3 unit = Vec3().fromAngle(rayDelta);
						calculated += raycast(Ray(camera.pos, unit), objects, lights, maxBounces);
					}
				}
				calculated /= 9.0; // 9 AA samples needs to be divided to get the average
				image.setPixel(x + dX, y + dY, calculated.map(std::sqrt)); // We correct the brightness by taking the root
			}
		}
		blockAssignMutex.lock();
		std::cout << conmanip::setpos(conOffsetX + blockX * 2, conOffsetY + blockY) << "#";
		blockAssignMutex.unlock();
	}
}

int main()
{
	std::ifstream configFile("config.json");
	nlohmann::json config;

	conmanip::console_out_context ctxOut;
	conmanip::console_out console(ctxOut);

	int threadNum = 8;

	if (configFile)
	{
		try
		{
			configFile >> config;
			if (config.contains("width")) width = config["width"];
			else std::cout << "No width specifier, defaulting to " << width << std::endl;
			if (config.contains("height")) height = config["height"];
			else std::cout << "No height specifier, defaulting to " << height << std::endl;
			if (config.contains("random_bounces")) randomBounces = config["random_bounces"];
			else std::cout << "No random_bounces specifier, defaulting to " << randomBounces << std::endl;
			if (config.contains("max_bounces")) maxBounces = config["max_bounces"];
			else std::cout << "No max_bounces specifier, defaulting to " << maxBounces << std::endl;
			if (config.contains("block_size")) blockSize = config["block_size"];
			else std::cout << "No block_size specifier, defaulting to " << blockSize << std::endl;
			if (config.contains("threads")) threadNum = config["threads"];
			else std::cout << "No threads specifier, defaulting to " << threadNum << std::endl;
		}
		catch(const std::exception&)
		{
			std::cout << "Empty or malformed JSON file, using defaults" << std::endl;
		}
	}
	else std::cout << "No config file found, using defaults" << std::endl;

	numBlocksX = std::ceil((double)width / blockSize);
	numBlocksY = std::ceil((double)height / blockSize);
	numBlocks = numBlocksX * numBlocksY;

	Bitmap image(width, height);

	blocks = new bool[numBlocks];
	memset(blocks, 0, numBlocks * sizeof(bool));

	conOffsetX = console.getposx();
	conOffsetY = console.getposy();

	for (int y = 0; y < numBlocksY; y++)
	{
		for (int x = 0; x < numBlocksX; x++)
		{
			std::cout << "X ";
		}
		std::cout << std::endl;
	}
	std::cout << "X: Unrendered block\n#: Rendered block\nNumber: Thread ID rendering" << std::endl;

	std::vector<Object> objects;
	objects.push_back(Object(Coords(0, 30, 100), 30, Colour(1.0, 0.0, 0.0), matMetalFuzz));
	objects.push_back(Object(Coords(30, 30, 50), 20, Colour(0.5, 0.7, 0.3), matDiffuse));
	objects.push_back(Object(Coords(-60, 10, 175), 10, Colour(0.3, 0.0, 0.3), matMetal));
	objects.push_back(Object(Coords(7, 45, 0), 10, Colour(1.0, 1.0, 1.0), matGlass));

	std::vector<Light> lights;
	lights.push_back(Light(Coords(-20, 10, 50), 5, Colour(0.996, 0.773, 0.557), 100.0));

	//Camera camera(Coords(0, 45, -50), Coords(0, 30, 100), 90, width, height);
	Camera camera(Coords(7, 45, -50), Coords(7, 45, 0), 90, width, height);

	std::thread* threads = new std::thread[threadNum];

	for (int thread = 0; thread < threadNum; thread++)
	{
		threads[thread] = std::thread(doPart, thread, std::ref(image), std::ref(camera), std::ref(objects), std::ref(lights));
	}

	for (int thread = 0; thread < threadNum; thread++)
	{
		threads[thread].join();
	}

	image.save("out.bmp");

	delete[] threads;
	delete[] blocks;
}
