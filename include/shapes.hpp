#include "vec3.hpp"
#include "camera.hpp"
#include "raycast.hpp"

class Shape
{
public:
	virtual bool hit(Ray& ray, HitData& data) = 0;

	void handleFace(Ray& ray, HitData& data)
	{
		if (ray.dir.dot(data.normal) <= 0.0) data.isFront = true;
		else
		{
			data.isFront = false;
			data.normal = -data.normal;
		}
	}
};

class Sphere : public Shape
{
public:
	Coords pos;
	double rad;

	Sphere(Coords _pos, double _rad) : pos(_pos), rad(_rad) {}

	bool hit(Ray& ray, HitData& data) override
	{
		Vec3 toCentre = ray.orig - pos;
		double a = std::pow(ray.dir.length(), 2);
		double halfB = toCentre.dot(ray.dir);
		double c = std::pow(toCentre.length(), 2) - std::pow(rad, 2);
		double discriminant = std::pow(halfB, 2) - a * c;

		if (discriminant < 0) return false;
		auto sqrtd = std::sqrt(discriminant);

		double root = (-halfB - sqrtd) / a;
		if (root < IMPRECISION_DELTA)
		{
			root = (-halfB + sqrtd) / a;
			if (root < IMPRECISION_DELTA) return false;
		}

		data.pos = ray.orig + ray.dir * root;
		data.normal = (data.pos - pos).unit();
		handleFace(ray, data);
		return true;
	}
};

class Plane : public Shape
{
public:
	Coords point;
	Vec3 normal;

	Plane(Coords _point, Vec3 _normal) : point(_point), normal(_normal) {}

	bool hit(Ray& ray, HitData& data) override
	{
		double root = (point - ray.orig).dot(normal) / ray.dir.dot(normal);
		if (root < IMPRECISION_DELTA) return false;

		data.pos = ray.orig + ray.dir * root;
		data.normal = normal;
		handleFace(ray, data);
		return true;
	}
};