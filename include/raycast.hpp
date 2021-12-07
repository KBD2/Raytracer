#pragma once

#include <vector>

#include "camera.hpp"
#include "bitmap.hpp"
#include "object.hpp"

const int steps = 1000;

Colour raycast(Camera* camera, unsigned int row, unsigned int col, std::vector<Object>* objects, unsigned int width, unsigned int height);