#ifndef _CPU_UTILS_H_
#define _CPU_UTILS_H_

//Utilities to make operations on cpu.
#include "matrix.h"
#include "image_buffer.h"

struct Color {
	Color() : b(0),g(0),r(0)
	{
	}
	Color(unsigned char r, unsigned char g, unsigned char b) : b(b),g(g),r(r)
	{
	}
	unsigned char b,g,r;
	bool operator==(const Color& that) const {
		return r==that.r&&g==that.g&&b==that.b;
	}

	static Color RED;
	static Color GREEN;
	static Color BLUE;
	static Color WHITE;
};

typedef Matrix<Color> CpuImage;

CpuImage readBuffer(ImageBufferPtr buffer);

void writeBuffer(const CpuImage& cpuimage, ImageBufferPtr buffer);

#endif
