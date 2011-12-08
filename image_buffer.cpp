#include "image_buffer.h"
#include <highgui.h>
#include "opengl_controller.h"

ImageBufferPool ImageBufferPool::_instance;
unsigned ImageBuffer::height;
unsigned ImageBuffer::width;

void ImageBuffer::setSizeFrom(const std::string& imageFile) {
	cv::Mat image = cv::imread(imageFile);

	width = image.size().width;
	height = image.size().height;

	OpenGlController::setViewport(width, height);

}
