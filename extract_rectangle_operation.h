#ifndef _RECTANGLE_OPERATION_DEFINITION_904120141049_H_
#define _RECTANGLE_OPERATION_DEFINITION_904120141049_H_

#include "operation.h"
#include "cpu_utils.h"
#include <vector>

struct Point {
	Point() {
	}
	Point(int x, int y) : x(x), y(y) {
	}
	int x, y;
};

struct RectangleList {
	std::vector< std::vector<Point> > points;
};

class ExtractRectangleOperation : public OperationDefinition {
public:
	virtual ~ExtractRectangleOperation();
	virtual void operate( const std::vector<ImageBufferPtr>& input, const std::vector<double>&, ImageBufferPtr output, boost::any& );
protected:
	virtual void operate( const std::vector<ImageBufferPtr>& input, const std::vector<double>&, ImageBufferPtr ) {
	}
};

#endif
