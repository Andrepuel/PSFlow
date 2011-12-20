#ifndef _RECTANGLE_OPERATION_DEFINITION_904120141049_H_
#define _RECTANGLE_OPERATION_DEFINITION_904120141049_H_

#include "operation.h"
#include "cpu_utils.h"
#include <vector>

struct Point {
	Point()
	: next(NULL)
	{
	}
	Point(int x, int y) : x(x), y(y),
	next(NULL)
	{
	}

	double distance(const Point& point2 ) const {
		int dx = x-point2.x;
		int dy = y-point2.y;
		return std::sqrt( dx*dx+dy*dy );
	}

	bool operator==(const Point& that) const {
		return this->x==that.x && this->y==that.y;
	}

	int x, y;
	Point* next;
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
