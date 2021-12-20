#include <cmath>
#include <cassert>
#include <iostream>
#include <random>

#include "raycast.hpp"
#include "materials.hpp"
#include "shapes.hpp"

Colour mixColour(Colour a, Colour b, double weight)
{
	Colour result;
	result.r = ((1.0 - weight) * a.r + weight * b.r);
	result.g = ((1.0 - weight) * a.g + weight * b.g);
	result.b = ((1.0 - weight) * a.b + weight * b.b);
	return result;
}

class IntersectData
{
public:
	bool hit;
	Coords pos;

	IntersectData(bool _hit, Coords _pos) : hit(_hit), pos(_pos) {}
};

Vec3 randInUnitSphere()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1.0, 1.0);

	double x, y, z;
	do
	{
		x = dis(gen);
		y = dis(gen);
		z = dis(gen);
	} while (std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2) > 1);

	return Vec3(x, y, z);
}

Vec3 randomInUnitDisk() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1.0, 1.0);
	while (true) {
		auto p = Vec3(dis(gen), dis(gen), 0);
		if (std::pow(p.length(), 2) >= 1) continue;
		return p;
	}
}

bool clearPath(Ray ray, double dist, std::vector<Object>& objects)
{
	for (auto& obj : objects)
	{
		HitData hitData;
		bool hit = obj.shape->hit(ray, hitData);
		if (hit && ray.orig.dist(hitData.pos) < dist) return false;
	}
	return true;
}

Colour raycast(Ray ray, std::vector<Object>& objects, std::vector<Light>& lights, int depth)
{
	if (depth == 0) return Colour(0, 0, 0);

	Colour hitColour;

	bool isLightSource = false;
	double intensity = 0;
	bool hit = false;
	double nearest;
	Colour col;
	Material* mat = NULL;

	HitData hitData;

	for (auto& obj : objects)
	{
		HitData testHit;
		bool hitObj = obj.shape->hit(ray, testHit);
		if (hitObj)
		{
			if (!hit || ray.orig.dist(testHit.pos) < nearest)
			{
				hit = true;
				hitData = testHit;
				nearest = ray.orig.dist(testHit.pos);
				col = obj.col;
				mat = obj.mat;
			}
		}
	}

	for (auto& light : lights)
	{
		bool hitObj = light.obj.shape->hit(ray, hitData);
		if(hitObj)
		{
			if (!hit || ray.orig.dist(hitData.pos) < nearest)
			{
				hit = true;
				nearest = ray.orig.dist(hitData.pos);
				isLightSource = true;
				col = light.obj.col;
				intensity = light.intensity;
			}
		}
	}

	if (hit)
	{
		Colour calculated;
		if (isLightSource)
		{
			// Hacky way to do it but it looks good and I can't find any other way
			calculated = Colour(1.0, 1.0, 1.0) - col.inverse() / std::sqrt(intensity);
		}
		else
		{
			double attenuation = mat->attenuation();
			Ray bounce = mat->bounce(ray, hitData);
			calculated = raycast(bounce, objects, lights, depth - 1);
			// This makes the material attenuate the light ray in a realistic way
			//calculated -= col.inverse() * attenuation;
			calculated *= col;
		}
		for (auto& light : lights)
		{
			HitData lightHit;
			Ray test(hitData.pos, hitData.normal);
			bool lightIntersect = light.obj.shape->hit(test, lightHit);
			if (clearPath(Ray(hitData.pos, (lightHit.pos - hitData.pos).unit()), hitData.pos.dist(lightHit.pos), objects))
			{
				Vec3 toLight = lightHit.pos - hitData.pos;
				double dot = std::max(0.0, hitData.normal.dot(toLight.unit()));
				double contribution = std::min(dot * light.intensity / std::pow(toLight.length(), 2), 1.0);
				calculated += light.obj.col * contribution;
			}
		}
		return calculated;
	}
	return Colour(0.8, 0.8, 0.9);
}