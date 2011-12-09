#ifndef _CONNECTED_SPACE_OPERATION_H_14178941_
#define _CONNECTED_SPACE_OPERATION_H_14178941_

#include "operation.h"
#include "cpu_utils.h"

class ConnectedSpaceOperation : public OperationDefinition {
public:
	virtual ~ConnectedSpaceOperation();
	virtual void operate( const std::vector<ImageBufferPtr>& input, const std::vector<double>&, ImageBufferPtr output );
	static void colorize( Matrix<Color>::Cell pixel, int cluster );
	static void clusterize( const Matrix<Color>& image, Matrix<int>& out);

public:
	struct Recursive;
};

#endif
