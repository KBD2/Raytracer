#include <cmath>
#include <cassert>
#include <iostream>
#include <random>

#include "raycast.hpp"
#include "materials.hpp"

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

IntersectData calculateHitSphere(Ray ray, Object& obj)
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
	if (discriminant <= 0) return IntersectData(false, Coords());

	// Find the roots with the quadratic formula
	double rootFirst = (-b + std::sqrt(discriminant)) / (2.0 * a);
	double rootSecond = (-b - std::sqrt(discriminant)) / (2.0 * a);

	// Find the closest intercept to the origin in front of the camera
	if (rootFirst < IMPRECISION_DELTA) rootFirst = 0;
	if (rootSecond < IMPRECISION_DELTA) rootSecond = 0;

	// If both are below zero the object is behind the ray origin
	if (rootFirst <= 0 && rootSecond <= 0) return IntersectData(false, Coords());

	double nearest;
	if (rootFirst > 0 && rootSecond > 0) nearest = std::min(rootFirst, rootSecond);
	else nearest = std::max(rootFirst, rootSecond);

	Coords hit = ray.orig + (ray.dir * nearest);
	
	return IntersectData(true, hit);
}

bool clearPath(Ray ray, double dist, std::vector<Object>& objects)
{
	for (auto& obj : objects)
	{
		IntersectData hit = calculateHitSphere(ray, obj);
		if (hit.hit && ray.orig.dist(hit.pos) < dist) return false;
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
	bool inside = false;
	Coords pos;
	Vec3 normal;
	Colour col;
	std::shared_ptr<Material> mat;

	for (auto& obj : objects)
	{
		IntersectData hitSphere = calculateHitSphere(ray, obj);
		if (hitSphere.hit)
		{
			if (!hit || ray.orig.dist(hitSphere.pos) < ray.orig.dist(pos))
			{
				hit = true;
				pos = hitSphere.pos;
				if (ray.orig.dist(obj.pos) >= obj.rad) normal = (hitSphere.pos - obj.pos).unit();
				else
				{
					normal = (obj.pos - hitSphere.pos).unit();
					inside = true;
				}
				col = obj.col;
				mat = obj.mat;
			}
		}
	}
	for (auto& light : lights)
	{
		IntersectData hitSphere = calculateHitSphere(ray, light.obj);
		if(hitSphere.hit)
		{
			if (!hit || ray.orig.dist(hitSphere.pos) < ray.orig.dist(pos))
			{
				hit = true;
				isLightSource = true;
				normal = (hitSphere.pos - light.obj.pos).unit();
				col = light.obj.col;
				intensity = light.intensity;
			}
		}
	}
	if (!hit && ray.dir.y < 0)
	{
		hit = true;
		double delta = -ray.orig.y / ray.dir.y;
		pos = ray.orig + ray.dir * delta;
		normal = Vec3(0, 1, 0);
		col = Colour(0.6, 0.6, 0.6);
		mat = std::make_shared<MatDiffuse>();
	}

	if (hit)
	{
		Colour averaged;
		if (isLightSource)
		{
			// Hacky way to do it but it looks good and I can't find any other way
			averaged = Colour(1.0, 1.0, 1.0) - col.inverse() / std::sqrt(intensity);
		}
		else for (int i = 0; i < randomBounces; i++)
		{
			double attenuation = mat->attenuation();
			Ray bounce = mat->bounce(ray, pos, normal, inside);
			Colour calculated = raycast(bounce, objects, lights, depth - 1);
			// This makes the material attenuate the light ray in a realistic way
			calculated -= col.inverse() * attenuation;
			averaged += calculated / randomBounces;
		}
		for (auto& light : lights)
		{
			IntersectData lightIntersect = calculateHitSphere(Ray(pos, (light.obj.pos - pos).unit()), light.obj);
			if (clearPath(Ray(pos, (lightIntersect.pos - pos).unit()), pos.dist(lightIntersect.pos), objects))
			{
				Vec3 toLight = lightIntersect.pos - pos;
				double dot = std::max(0.0, normal.dot(toLight.unit()));
				double contribution = std::min(dot * light.intensity / std::pow(toLight.length(), 2), 1.0);
				averaged += light.obj.col * contribution;
			}
		}
		return averaged;
	}
	return Colour(0.6, 0.7, 0.9);
}