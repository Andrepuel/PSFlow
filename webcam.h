#ifndef _WEBCAM_H_
#define _WEBCAM_H_

#include "operation.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

class WebcamOperation : public OperationDefinition {
public:
	WebcamOperation(unsigned int webcam=0);
	virtual ~WebcamOperation();
	virtual void operate(const std::vector<ImageBufferPtr>&, const std::vector<double>&, ImageBufferPtr output );

	void setSizeFrom();
private:
	CvCapture* capture;	

};

#endif
