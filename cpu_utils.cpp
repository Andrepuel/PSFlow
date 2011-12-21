#include "cpu_utils.h"

Color Color::RED = {255,0,0};
Color Color::GREEN = {0,255,0};
Color Color::BLUE = {0,0,255};
Color Color::WHITE = {255,255,255};


CpuImage readBuffer(ImageBufferPtr buffer) {
	CpuImage toReturn( ImageBuffer::height, ImageBuffer::width);
	buffer->getData(toReturn.asArray());
	return toReturn;
}

void writeBuffer(const CpuImage& cpuimage, ImageBufferPtr buffer) {
	buffer->setData(cpuimage.asArray());
}
