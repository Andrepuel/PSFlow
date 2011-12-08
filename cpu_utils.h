#ifndef _CPU_UTILS_H_
#define _CPU_UTILS_H_

//Utilities to make operations on cpu.
#include "matrix.h"
#include "image_buffer.h"

struct Color {
	unsigned char r,g,b;
};

typedef Matrix<Color> CpuImage;

CpuImage readBuffer(ImageBufferPtr buffer);

void writeBuffer(const CpuImage& cpuimage, ImageBufferPtr buffer);

#endif
