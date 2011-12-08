#ifndef _IMAGE_BUFFER_OPERATION_H_
#define _IMAGE_BUFFER_OPERATION_H_

#include "operation.h"
#include "image_buffer.h"
#include "cpu_utils.h"

class ImageBufferOperation : public OperationDefinition {
public:
	ImageBufferOperation();
	virtual ~ImageBufferOperation();

	virtual void operate(const std::vector<ImageBufferPtr>&, const std::vector<double>&, ImageBufferPtr output );
private:
	ImageBufferPtr buffer;	
};

#endif
