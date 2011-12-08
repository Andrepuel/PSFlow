#include "view_operation.h"
#include "opengl_controller.h"
#include "image_buffer.h"
#include "assert.h"

ViewOperation::ViewOperation() {
	OpenGlController::resizeWindow(ImageBuffer::width, ImageBuffer::height);
}
void ViewOperation::operate(const std::vector<ImageBufferPtr>& input, const std::vector<double>&, ImageBufferPtr) {
	assert(input.size() == 1);

	OpenGlController::attachShader(0);
	input[0]->attachAsInput(0);
	OpenGlController::draw();
}

