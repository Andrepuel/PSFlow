#include "cpu_utils.h"

CpuImage readBuffer(ImageBufferPtr buffer) {
	CpuImage toReturn( ImageBuffer::height, ImageBuffer::width);
	buffer->getData(toReturn.asArray());
	return toReturn;
}

void writeBuffer(const CpuImage& cpuimage, ImageBufferPtr buffer) {
	buffer->setData(cpuimage.asArray());
}
