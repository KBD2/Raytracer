#include <random>

#include "raycast.hpp"

class Shape;

Vec3 randInUnitSphere();

inline Vec3 reflectVec(Vec3 ray, Vec3 normal)
{
	return ray - normal * normal.dot(ray) * 2.0;
}

class Material
{
public:
	virtual Ray bounce(Ray ray, HitData& hit) = 0;
	virtual double attenuation() = 0;
};

class MatDiffuse : public Material
{
public:
	const double scatter = 1.0;
	double attenuation() override { return 0.8; }

	Ray bounce(Ray ray, HitData& hit) override
	{
		return Ray(hit.pos, randInUnitSphere().unit() * scatter + hit.normal);
	}
	
};

class MatMetal : public Material
{
public:
	double attenuation() override { return 0.6; };

	Ray bounce(Ray ray, HitData& hit) override
	{
		return Ray(hit.pos, reflectVec(ray.dir, hit.normal));
	}
};

class MatMetalFuzz : public Material
{
public:
	double attenuation() override { return 0.65; }
	const double perturbation = 0.2;
	Ray bounce(Ray ray, HitData& hit) override
	{
		return Ray(hit.pos, reflectVec(ray.dir, hit.normal) + randInUnitSphere() * perturbation);
	}
};

class MatGlass : public Material
{
public:
	const double refractiveIndex = 1.33;
	double attenuation() override { return 0.0; }
	Ray bounce(Ray ray, HitData& hit) override
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		double ratio = hit.isFront ? (1.0 / refractiveIndex) : refractiveIndex;
		double cosTheta = std::fmin((-ray.dir).dot(hit.normal), 1.0);
		double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
		if (ratio * sinTheta > 1.0 || reflectance(cosTheta, ratio) > dis(gen))
		{
			return Ray(hit.pos, reflectVec(ray.dir, hit.normal));
		}
		Vec3 perpendicular = (ray.dir + hit.normal * cosTheta) * ratio;
		Vec3 parallel = hit.normal * -std::sqrt(std::fabs(1.0 - std::pow(perpendicular.length(), 2)));
		Vec3 refracted = (perpendicular + parallel).unit();
		return Ray(hit.pos, refracted);
	}

private:
		static double reflectance(double cosine, double ref_idx) {
			// Use Schlick's approximation for reflectance.
			auto r0 = (1 - ref_idx) / (1 + ref_idx);
			r0 = r0 * r0;
			return r0 + (1 - r0) * pow((1 - cosine), 5);
		}
};