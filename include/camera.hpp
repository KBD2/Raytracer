#pragma once

#include <cmath>

#include "vec3.hpp"

const double pi = 3.14159265;

typedef Vec3 Coords;

inline double toRads(double n)
{
	return n / 180 * pi;
}

inline double toDegs(double n)
{
	return n / pi * 180;
}

class Angle
{
public:
	double yaw, pitch;

	Angle()
	{
		yaw = 0;
		pitch = 0;
	}

	Angle(double _yaw, double _pitch)
	{
		yaw = _yaw;
		pitch = _pitch;
	}

	Angle fromVec3(Vec3 pos);
	Angle delta(double dYaw, double dPitch);
	Angle operator+(Angle n);
	Angle operator-(Angle n);
	void operator+=(Angle n);
	Angle operator/(double n);
	void operator/=(double divisor);
	Angle operator*(double n);
	void operator*=(double n);
};

class Camera
{
public:
	Coords pos, lookingAt;
	Coords viewplaneTL, viewplaneBR, viewplaneTR, viewplaneBL;
	double fovHoriz, fovVert;

	Camera(Coords coords, Coords _lookingAt, int FOV, int width, int height)
	{
		pos = coords;
		lookingAt = _lookingAt;

		fovHoriz = FOV;
		fovVert = (double)FOV * ((double)height / width); // Image aspect ratio = screen aspect ratio

		Angle lookingAngle = Angle().fromVec3(lookingAt - pos);
		// Get the angle to the top left of the viewplane
		Angle topLeft = lookingAngle.delta(fovHoriz / 2, -fovVert / 2) / 180.0 * pi;

		Angle botRight = lookingAngle.delta(-fovHoriz / 2, fovVert / 2) / 180.0 * pi;
		Angle topRight = lookingAngle.delta(-fovHoriz / 2, -fovVert / 2) / 180.0 * pi;
		Angle botLeft = lookingAngle.delta(fovHoriz / 2, fovVert / 2) / 180.0 * pi;

		Angle half = Angle(fovHoriz, fovVert) / 2;
		half = half / 180.0 * pi;
		// Hypotenuses of the triangles with the distance from the camera to the viewplane center
		// and half the width/height of the viewplane
		double distToViewplaneEdgeHoriz = ((double)width / 2) / std::sin(half.yaw);
		double distToViewplaneEdgeVert = ((double)height / 2) / std::sin(half.pitch);
		// Relative coords to the center of the vertical and horizontal edges of the viewplane
		Vec3 toVertLine = Vec3(0, distToViewplaneEdgeVert * std::sin(half.pitch), distToViewplaneEdgeVert * std::cos(half.pitch));
		Vec3 toHorizLine = Vec3(0, distToViewplaneEdgeHoriz * std::sin(half.yaw), distToViewplaneEdgeHoriz * std::cos(half.yaw));
		// Distance from the camera to the corner of the viewplane
		double toCorner = (toVertLine + toHorizLine).length();

		// Unit vector from the camera to the viewplane corner
		Vec3 unit = Vec3(std::sin(topLeft.pitch) * std::cos(topLeft.yaw), cos(topLeft.pitch), sin(topLeft.pitch) * sin(topLeft.yaw));
		// Multiplied by the distance to get the coordinates
		viewplaneTL = unit * toCorner;
		
		unit = Vec3(std::sin(topRight.pitch) * std::cos(topRight.yaw), cos(topRight.pitch), sin(topRight.pitch) * sin(topRight.yaw));
		viewplaneTR = unit * toCorner;
		unit = Vec3(std::sin(botLeft.pitch) * std::cos(botLeft.yaw), cos(botLeft.pitch), sin(botLeft.pitch) * sin(botLeft.yaw));
		viewplaneBL = unit * toCorner;
		unit = Vec3(std::sin(botRight.pitch) * std::cos(botRight.yaw), cos(botRight.pitch), sin(botRight.pitch) * sin(botRight.yaw));
		viewplaneBR = unit * toCorner;
	}
};