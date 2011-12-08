#include "webcam.h"
#include "image_buffer.h"

WebcamOperation::WebcamOperation(unsigned int webcam) {
	capture = cvCaptureFromCAM( webcam );
	if( !capture ) {
		throw "Unable to open camera";
	}
}
WebcamOperation:: ~WebcamOperation() {
    cvReleaseCapture( &capture );
}
void WebcamOperation::operate(const std::vector<ImageBufferPtr>&, const std::vector<double>&, ImageBufferPtr output ) {
	IplImage* frame = 0;
        frame = cvQueryFrame( capture );

	if( !frame ) {
		throw "Unable to query frame.";
	}

	output->setData(frame->imageData);
}

void WebcamOperation::setSizeFrom() {
	IplImage* frame = 0;
        frame = cvQueryFrame( capture );

	if( !frame ) {
		throw "Unable to query frame.";
	}

	ImageBuffer::width = frame->width;
	ImageBuffer::height = frame->height;

	OpenGlController::setViewport(frame->width,frame->height);
}
