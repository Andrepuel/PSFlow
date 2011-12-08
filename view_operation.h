#ifndef _VIEW_OPERATION_H_
#define _VIEW_OPERATION_H_

#include "operation.h"

class ViewOperation : public OperationDefinition {
public:
	ViewOperation();
	virtual void operate(const std::vector<ImageBufferPtr>&, const std::vector<double>&, ImageBufferPtr output );

	virtual bool hasOutput() {
		return false;
	}
};

#endif
