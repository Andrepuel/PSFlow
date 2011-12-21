#ifndef _RECTANGLE_OPERATION_DEFINITION_904120141049_H_
#define _RECTANGLE_OPERATION_DEFINITION_904120141049_H_

#include "operation.h"
#include "cpu_utils.h"
#include <vector>

struct Point {
	Point()
	: next(NULL),prev(NULL)
	{
	}
	Point(int x, int y) : x(x), y(y),
	next(NULL),prev(NULL)
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

	double angleCossine(const Point& a, const Point& b) const {
		double alen = distance(a);
		double blen = distance(b);
		double aunitX = (a.x-x)/alen;
		double aunitY = (a.y-y)/alen;
		double bunitX = (b.x-x)/blen;
		double bunitY = (b.y-y)/blen;
		return aunitX*bunitX+aunitY*bunitY;
	}

	double angleCossine() const {
		return angleCossine(*next,*prev);
	}

	int x, y;
	Point* next,* prev;
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
