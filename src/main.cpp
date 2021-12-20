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
#include "shapes.hpp"
#include "json.h"

int maxBounces = 25;
int width = 640;
int height = 480;
int blockSize = 50;
int aaSamples = 50;

int numBlocksX;
int numBlocksY;
int numBlocks;

int conOffsetX, conOffsetY;

std::mutex blockAssignMutex;
bool* blocks;

template<typename T> bool getConfigVar(nlohmann::json& config, std::string name, T& var)
{
	if (config.contains(name))
	{
		var = config[name];
		return true;
	}
	else
	{
		std::cout << "Could not get config variable '" << name << "', using defaults" << std::endl;
		return false;
	}
}

void doPart(int number, Bitmap& image, Camera& camera, std::vector<Object>& objects, std::vector<Light>& lights)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1.0, 1.0);

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

				for (int aa = 0; aa < aaSamples; aa++)
				{
					Angle rayDelta = ray.delta(dis(gen) / camera.fovHoriz, dis(gen) / camera.fovVert) / 180 * pi;
					Vec3 unit = Vec3().fromAngle(rayDelta);
					calculated += raycast(Ray(camera.pos, unit), objects, lights, maxBounces);
				}
				calculated /= aaSamples;
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
	nlohmann::json cameraConfig = {
		{"position", {0, 0, 0}},
		{"look_at", {0, 0, 100}},
		{"fov", 90}
	};

	conmanip::console_out_context ctxOut;
	conmanip::console_out console(ctxOut);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	int threadNum = 8;

	int fov = 90;
	std::array<double, 3> camPosArr;
	std::array<double, 3> camDestArr;

	if (configFile)
	{
		try
		{
			configFile >> config;
			getConfigVar<int>(config, "width", width);
			getConfigVar<int>(config, "height", height);
			getConfigVar<int>(config, "max_bounces", maxBounces);
			getConfigVar<int>(config, "block_size", blockSize);
			getConfigVar<int>(config, "threads", threadNum);
			getConfigVar<int>(config, "anti_aliasing_samples", aaSamples);
			getConfigVar<nlohmann::json>(config, "camera", cameraConfig);
			getConfigVar<std::array<double, 3>>(cameraConfig, "position", camPosArr);
			getConfigVar<std::array<double, 3>>(cameraConfig, "look_at", camDestArr);
			getConfigVar<int>(cameraConfig, "fov", fov);
		}
		catch(const std::exception&)
		{
			std::cout << "Empty or malformed JSON file, using defaults" << std::endl;
		}
	}
	else std::cout << "No config file found, using defaults" << std::endl;

	Coords orig(camPosArr[0], camPosArr[1], camPosArr[2]);
	Coords dest(camDestArr[0], camDestArr[1], camDestArr[2]);

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

	auto matDiffuse = MatDiffuse();
	auto matMetal = MatMetal();
	auto matMetalFuzz = MatMetalFuzz();
	auto matGlass = MatGlass();

	Plane ground = Plane(Coords(0, 0, 0), Vec3(1, 1, 0));
	Sphere sphere1 = Sphere(Coords(0, 30, 100), 30);
	Sphere sphere2 = Sphere(Coords(30, 30, 50), 20);
	Sphere sphere3 = Sphere(Coords(-80, 20, 175), 20);
	Sphere sphere4 = Sphere(Coords(7, 45, 0), 10);

	std::vector<Object> objects;
	objects.push_back(Object(&ground, Colour(0.6, 0.6, 0.6), &matDiffuse));
	//objects.push_back(Object(&sphere1, Colour(1.0, 1.0, 1.0), &matMetalFuzz));
	//objects.push_back(Object(&sphere2, Colour(1.0, 0.3, 0.3), &matDiffuse));
	//objects.push_back(Object(&sphere3, Colour(0.7, 0.4, 0.7), &matMetal));
	//objects.push_back(Object(&sphere4, Colour(0.6, 0.6, 1.0), &matGlass));

	std::vector<Light> lights;
	//lights.push_back(Light(Coords(-20, 10, 50), 5, Colour(0.996, 0.773, 0.557), 100.0));

	Camera camera(orig, dest, fov, width, height);

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
