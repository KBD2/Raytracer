#include <fstream>
#include <cmath>
#include <cstring>
#include <iostream>
#include <cassert>

#include "bitmap.hpp"

void Bitmap::setPixel(unsigned int x, unsigned int y, Colour colour)
{
	if (x >= width || y >= height) return;
	data[y * width + x] = colour;
}

bool Bitmap::save(std::string filename)
{
	std::ofstream file;
	file.open(filename, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!file.is_open())
	{
		std::cout << "Couldn't open \"" << filename << "\"!" << std::endl;
		return false;
	}

	// https://en.wikipedia.org/wiki/BMP_file_format

	const int headerSize = 12;
	const int dibSize = 40;
	const int colourPlanes = 1;
	const int bpp = 24;

	unsigned int rowSize = (int)(std::ceil(24.0 * width / 32.0) * 4.0);
	int pixelArraySize = rowSize * height;
	int offset = headerSize + dibSize;
	int fileSize = pixelArraySize + headerSize + dibSize;

	char fileHeaderData[] = {
		/* Bitmap header */
		'B', 'M',
		0x00, 0x00, 0x00, 0x00, // File size
		0x00, 0x00, 0x00, 0x00, // App-specific data
		0x36, 0x00, 0x00, 0x00, // Pixel array offset (14 byte header + 40 byte DIB)
		/* DIB header */
		0x28, 0x00, 0x00, 0x00, // DIB size (40 bytes)
		0x00, 0x00, 0x00, 0x00, // Width
		0x00, 0x00, 0x00, 0x00, // Height
		0x01, 0x00,	0x18, 0x00, // Colour planes and BPP
		0x00, 0x00, 0x00, 0x00, // Compression method (none in this case)
		0x00, 0x00, 0x00, 0x00, // Pixel array size
		0x13, 0x0B, 0x00, 0x00, // Print resolution
		0x13, 0x0B, 0x00, 0x00, // ^^^
		0x00, 0x00, 0x00, 0x00, // Colours in palette (we aren't using it)
		0x00, 0x00, 0x00, 0x00  // Important colours (all of them)
	};
	// Won't work on big-endian systems
	std::memcpy(fileHeaderData + 0x02, &fileSize, 4);
	std::memcpy(fileHeaderData + 0x12, &width, 4);
	std::memcpy(fileHeaderData + 0x16, &height, 4);
	std::memcpy(fileHeaderData + 0x22, &pixelArraySize, 4);

	file.write(fileHeaderData, sizeof(fileHeaderData));

	assert(rowSize >= width); // Avoid the compiler complaining cause it thinks rowSize could be 2 or less

	char* rowData = new char[rowSize];

	std::memset(rowData, 0, rowSize);

	for (int y = height - 1; y >= 0; y--)
	{
		for (int x = 0; (unsigned int)x < width; x++)
		{
			uint64_t colourIndex = (uint64_t)y * width + x;
			// Little-endian means we flip the normal order
			rowData[x * 3] = data[colourIndex].b;
			rowData[x * 3 + 1] = data[colourIndex].g;
			rowData[x * 3 + 2] = data[colourIndex].r;
		}
		file.write(rowData, rowSize);
	}

	file.close();

	delete[] rowData;

	return true;
}