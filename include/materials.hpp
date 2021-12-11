#include "raycast.hpp"

Vec3 randInUnitSphere();

class Material
{
public:
	virtual Ray bounce(Ray ray, Coords hit, Vec3 normal, double& _attenuation) = 0;
};

class MatDiffuse : public Material
{
public:
	const double scatter = 1.0;
	const double attenuation = 0.8;

	Ray bounce(Ray ray, Coords hit, Vec3 normal, double& _attenuation) override
	{
		_attenuation = attenuation;
		return Ray(hit, randInUnitSphere().unit() * scatter + normal);
	}
};

class MatMetal : public Material
{
public:
	const double attenuation = 0.6;

	Ray bounce(Ray ray, Coords hit, Vec3 normal, double& _attenuation) override
	{
		_attenuation = attenuation;
		return Ray(hit, ray.dir - normal * normal.dot(ray.dir) * 2.0);
	}
};

class MatMetalFuzz : public Material
{
public:
	const double attenuation = 0.65;
	const double perturbation = 0.2;
	Ray bounce(Ray ray, Coords hit, Vec3 normal, double& _attenuation) override
	{
		_attenuation = attenuation;
		return Ray(hit, ray.dir - normal * normal.dot(ray.dir) * 2.0 + randInUnitSphere() * perturbation);
	}
};