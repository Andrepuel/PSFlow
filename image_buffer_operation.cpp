#include "image_buffer_operation.h"
#include "opengl_controller.h"
#include <sstream>
#include <opencv/highgui.h>

ImageBufferOperation::ImageBufferOperation() 
{
	buffer = ImageBufferPool::instance().get();
}
ImageBufferOperation::~ImageBufferOperation() {
	ImageBufferPool::instance().freeBuffer(buffer);
}

int tmpbuf=0;

void ImageBufferOperation::operate(const std::vector<ImageBufferPtr>& input, const std::vector<double>&, ImageBufferPtr output) {
	GL_CHECK_ERROR();
	if( input.size() > 0 ) {
		OpenGlController::attachShader(0);
		input[0]->attachAsInput(0);
		buffer->attachAsOutput();
		OpenGlController::invokeShader();

	} else {
		OpenGlController::attachShader(0);
		buffer->attachAsInput(0);
		output->attachAsOutput();
		OpenGlController::invokeShader();

	}
	GL_CHECK_ERROR();
}
