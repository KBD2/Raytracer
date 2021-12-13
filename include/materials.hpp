#include <random>

#include "raycast.hpp"

Vec3 randInUnitSphere();

inline Vec3 reflectVec(Vec3 ray, Vec3 normal)
{
	return ray - normal * normal.dot(ray) * 2.0;
}

class Material
{
public:
	virtual Ray bounce(Ray ray, Coords hit, Vec3 normal, bool inside) = 0;
	virtual double attenuation() = 0;
};

class MatDiffuse : public Material
{
public:
	const double scatter = 1.0;
	double attenuation() override { return 0.8; }

	Ray bounce(Ray ray, Coords hit, Vec3 normal, bool inside) override
	{
		return Ray(hit, randInUnitSphere().unit() * scatter + normal);
	}
	
};

class MatMetal : public Material
{
public:
	double attenuation() override { return 0.6; };

	Ray bounce(Ray ray, Coords hit, Vec3 normal, bool inside) override
	{
		return Ray(hit, reflectVec(ray.dir, normal));
	}
};

class MatMetalFuzz : public Material
{
public:
	double attenuation() override { return 0.65; }
	const double perturbation = 0.2;
	Ray bounce(Ray ray, Coords hit, Vec3 normal, bool inside) override
	{
		return Ray(hit, reflectVec(ray.dir, normal) + randInUnitSphere() * perturbation);
	}
};

class MatGlass : public Material
{
public:
	const double refractiveIndex = 1.33;
	double attenuation() override { return 0.0; }
	Ray bounce(Ray ray, Coords hit, Vec3 normal, bool inside) override
	{
		double ratio = inside ? refractiveIndex : (1.0 / refractiveIndex);
		double cosTheta = std::fmin((-ray.dir).dot(normal), 1.0);
		double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
		if (ratio * sinTheta > 1.0)
		{
			return Ray(hit, reflectVec(ray.dir, normal));
		}
		Vec3 perpendicular = (ray.dir + normal * cosTheta) * ratio;
		Vec3 parallel = normal * -std::sqrt(std::fabs(1.0 - std::pow(perpendicular.length(), 2)));
		Vec3 refracted = (perpendicular + parallel).unit();
		return Ray(hit, refracted);
	}
};