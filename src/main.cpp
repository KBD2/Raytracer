#include <iostream>

#include "camera.hpp"
#include "bitmap.hpp"

const int width = 100;
const int height = 67;

int main()
{
    Camera camera(Coords(0, 0, 0), Coords(-100, 33, 100), width, height, true);

    Bitmap image(width, height);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            image.setPixel(x, y, Colour(x, y, 0));
        }
    }

    image.save("out.bmp");
}
