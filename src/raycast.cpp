#include <cmath>
#include <cassert>
#include <iostream>
#include <random>

#include "raycast.hpp"

Colour mixColour(Colour a, Colour b, double weight)
{
	Colour result;
	result.r = ((1.0 - weight) * a.r + weight * b.r);
	result.g = ((1.0 - weight) * a.g + weight * b.g);
	result.b = ((1.0 - weight) * a.b + weight * b.b);
	return result;
}

class HitData {
public:
	bool hit;
	Coords pos;
	Vec3 normal;
	Colour surfaceCol;

	HitData()
	{
		hit = false;
		pos = Coords();
		normal = Vec3();
		surfaceCol = Colour();
	}

	HitData(bool _hit, Coords _pos, Vec3 _normal, Colour _surfaceCol)
	{
		hit = _hit;
		pos = _pos;
		normal = _normal;
		surfaceCol = _surfaceCol;
	}

	HitData(bool failed) // Just to make it clear when a hit didn't occur
	{
		hit = false;
		pos = Coords();
		normal = Vec3();
		surfaceCol = Colour();
	}
};

HitData calculateHitSphere(Ray ray, Object& obj)
{
	// Set up a quadratic equation using parametric formulae for the ray and sphere
	double a = std::pow(ray.dir.x, 2)
		+ std::pow(ray.dir.y, 2)
		+ std::pow(ray.dir.z, 2);
	double b = -2.0 * (ray.dir.x * (obj.pos.x - ray.orig.x)
					+ ray.dir.y * (obj.pos.y - ray.orig.y)
					+ ray.dir.z * (obj.pos.z - ray.orig.z));
	double c = std::pow(obj.pos.x - ray.orig.x, 2)
		+ std::pow(obj.pos.y - ray.orig.y, 2)
		+ std::pow(obj.pos.z - ray.orig.z, 2)
		- std::pow(obj.rad, 2);

	// If the discriminant isn't above 0, it doesn't intercept
	double discriminant = std::pow(b, 2) - 4.0 * a * c;
	if (discriminant <= 0) return HitData();

	// Find the roots with the quadratic formula
	double rootFirst = (-b + std::sqrt(discriminant)) / (2.0 * a);
	double rootSecond = (-b - std::sqrt(discriminant)) / (2.0 * a);

	// If both are below zero the object is behind the ray origin
	if (rootFirst < 0 && rootSecond < 0) return HitData();

	double nearest;

	// Find the closest intercept to the origin in front of the camera
	if (rootFirst > 0 && rootSecond > 0) nearest = std::min(rootFirst, rootSecond);
	else nearest = std::max(rootFirst, rootSecond);

	Coords hit = ray.orig + (ray.dir * (nearest - IMPRECISION_DELTA));
	
	return HitData(true, hit, (hit - obj.pos).unit(), obj.col);
}

HitData fireRay(Ray ray, std::vector<Object>& objects)
{
	for (auto& obj : objects)
	{
		HitData hit = calculateHitSphere(ray, obj);
		if (hit.hit) return hit;
	}
	if (ray.dir.y < 0)
	{
		double delta = -ray.orig.y / ray.dir.y;
		Coords ground = ray.dir * delta;
		return HitData(true, ground, Vec3(0, 1, 0), Colour(0.6, 0.6, 0.6));
	}
	return HitData(false);
}

bool clearPath(Ray ray, std::vector<Object>& objects)
{
	for (auto& obj : objects)
	{
		if (calculateHitSphere(ray, obj).hit) return false;
	}
	return true;
}

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

Colour raycast(Ray ray, std::vector<Object>& objects)
{
	double multiplier = 1.0;

	// Convert spherical coordinates to unit vector
	int bounces;
	Colour hitColour;

	for (bounces = 0; bounces < MAX_BOUNCES; bounces++)
	{
		HitData hit = fireRay(ray, objects);
		if (hit.hit)
		{
			hitColour = mixColour(hitColour, hit.surfaceCol, multiplier);
			multiplier *= 0.2;
			ray.orig = hit.pos;
			//ray.dir = hit.normal * -2.0 * (ray.dir.dot(hit.normal)) - ray.dir;
			ray.dir = randInUnitSphere().unit() + hit.normal;
		}
		else
		{
			hitColour = mixColour(hitColour, Colour(0.6, 0.7, 0.9), multiplier);
			break;
		}
	}
	return hitColour;
}

//Vec3 light(-10, 40, 50);
/*
double contribution = 0.0;
for (auto& obj : objects)
{
	hit = calculateHitSphere(ray, obj);
	if (hit.hit)
	{
		assert((hit.pos - rayOrig).length() < (obj.pos - ray.orig).length());
		if (clearPath(Ray(hit.pos, (light - hit.pos).unit()), objects))
		{
			Vec3 normal = (hit.pos - obj.pos).unit();
			Vec3 toLight = light - hit.pos;
			double dot = std::max(0.0, normal.dot(toLight.unit()));
			contribution = dot * 100.0 / std::pow(toLight.length() / 2, 2);
		}
		return obj.col * contribution;
	}
}*/