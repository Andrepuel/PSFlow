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
	bool operator!=(const Point& that) const {
		return this->x!=that.x || this->y!=that.y;
	}

	static double angleCossine(const Point& a0, const Point& a1, const Point& b0, const Point& b1) {
		double alen = a0.distance(a1);
		double blen = b0.distance(b1);
		double aunitX = (a1.x-a0.x)/alen;
		double aunitY = (a1.y-a0.y)/alen;
		double bunitX = (b1.x-b0.x)/blen;
		double bunitY = (b1.y-b0.y)/blen;
		return aunitX*bunitX+aunitY*bunitY;
	}

	static Point lineIntersect(const Point& a0, const Point& a1, const Point& b0, const Point& b1);

	double angleCossine(const Point& a, const Point& b) const {
		return angleCossine(*this,a,*this,b);
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
