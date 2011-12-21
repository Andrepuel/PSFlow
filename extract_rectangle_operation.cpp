#include "extract_rectangle_operation.h"
#include "connected_space_operation.h"
#include <queue>
#include <cassert>
#include <functional>

ExtractRectangleOperation::~ExtractRectangleOperation() {
}

static double distancePointLine( const Point& point, const Point& lineStart, const Point& lineEnd ) {
	double lineMag;
	double u;
	Point intersection;

	lineMag = lineEnd.distance( lineStart );

	u = ( ( ( point.x - lineStart.x ) * ( lineEnd.x - lineStart.x ) ) +
	( ( point.y - lineStart.y ) * ( lineEnd.y - lineStart.y ) ) ) /
	( lineMag * lineMag );

	if( u < 0.0f || u > 1.0f ) {
		return std::max(point.distance(lineStart),point.distance(lineEnd));
	}

	intersection.x = lineStart.x + u * ( lineEnd.x - lineStart.x );
	intersection.y = lineStart.y + u * ( lineEnd.y - lineStart.y );

	return point.distance(intersection);
}

static void drawLine( const Point& lineStart, const Point& lineEnd, CpuImage& out, const Color& color=Color({255,255,255}) ) {
	Point each;
	for( each.y = std::min(lineStart.y,lineEnd.y); each.y <= std::max(lineEnd.y,lineStart.y); ++each.y )
	for( each.x = std::min(lineStart.x,lineEnd.x); each.x <= std::max(lineEnd.x,lineStart.x); ++each.x ) {
		if( distancePointLine(each,lineStart,lineEnd) < 1.1 ) {
			out(each.y,each.x).get() = color;
		}
	}
}

static double calcAngle(const Point& a, const Point& b) {
	double len = a.distance(b);
	double x = (b.x-a.x)/len;
	if( b.y < a.y ) {
		return 2.0-x;
	}
	return x;
};

static double calcAngle(const Point& a, const Point& b, double minAngle) {
	double x = calcAngle(a,b);
	if( x < minAngle ) x += 4.0;
	return x;
}

static Point* convexHull(std::vector<Point>& points) {
	Point* leftMost = &(*points.begin() );
	for( auto each = points.begin(); each != points.end(); ++each ) {
		if( each->x < leftMost->x ||
			( each->x == leftMost->x && each->y < leftMost->y )		
		) {
			leftMost = &(*each);
		}
		each->next = NULL;
		each->prev = NULL;
	}

	Point* actual=leftMost;
	double prevAngle=0.0;
	while( actual->next == NULL ) {
		Point* best=NULL;
		double bestAngle = 0.0;
		
		for( auto eachOne = points.begin(); eachOne != points.end(); ++eachOne ) {
			if( *eachOne == *actual )
				continue;
			if( best == NULL ) {
				best = &(*eachOne);
				bestAngle = calcAngle(*actual,*best,prevAngle);
				continue;
			} 
			
			double eachAngle = calcAngle(*actual,*eachOne,prevAngle);
			if ( eachAngle < bestAngle || (eachAngle == bestAngle && actual->distance(*best) < actual->distance(*eachOne) )) {
				best = &(*eachOne);
				bestAngle = eachAngle;
			}
		}
		if( best == NULL )
			best = leftMost;
		prevAngle=calcAngle(*actual,*best);
		actual->next = best;
		actual->next->prev = actual;
		actual = actual->next;
	}
	assert( leftMost->prev != NULL );

	return leftMost;
};

static void checkFor(Point*& greater, Point* candidate, int xFactor, int yFactor) {
	if( (candidate->x*xFactor+candidate->y*yFactor) > (greater->x*xFactor+greater->y*yFactor) ) {
		greater = candidate;
	}
};

void ExtractRectangleOperation::operate( const std::vector<ImageBufferPtr>& inputList, const std::vector<double>&, ImageBufferPtr output, boost::any& extraOutput ) {
	assert( inputList.size() == 2 );

	Color black;
	black.r=0;black.g=0;black.b=0;

	CpuImage thicked = readBuffer(inputList[0]);
	CpuImage components = readBuffer(inputList[1]);
	CpuImage out(thicked.height(),thicked.width(),black);

	extraOutput = RectangleList();
	RectangleList& rectangleList = boost::any_cast<RectangleList&>(extraOutput);

	Matrix<bool> clusters(thicked.height(),thicked.width(),false);
#if 0
	for( int y = 0; y < int(thicked.height()); ++y )
	for( int x = 0; x < int(thicked.width()); ++x ) {
		if( thicked(y,x).get().r > 0 ) {
			out(y,x).get().r = 255;
		} else if( components(y,x).get().r > 0 ) {
			out(y,x).get().r = 25;
		}
	}
#endif
	for( int y = 0; y < int(thicked.height()); ++y )
	for( int x = 0; x < int(thicked.width()); ++x ) {
		if( clusters(y,x).get() ) {
			continue;
		}
		if( components(y,x).get().r == 0 ) {
			clusters(y,x).get() = true;
			continue;
		}

		std::queue<Point> floodfill;
		rectangleList.points.resize( rectangleList.points.size()+1 );
		if( thicked(y,x).get().r > 0 )
			rectangleList.points.back().push_back( Point(x,y) );
		floodfill.push( Point(x,y) );
		while( !floodfill.empty() ) {
			Point each = floodfill.front();
			floodfill.pop();
			
			for( int line=-1; line <= 1; ++line )
			for( int column=-1 ; column <= 1; ++column ) {
				Point neighbour(each.x+column,each.y+line);
				if( !clusters(neighbour.y,neighbour.x).get() ) {
					if( components(neighbour.y,neighbour.x).get().r != 0 ) {
						if( thicked(neighbour.y,neighbour.x).get().r > 0 ) {
							rectangleList.points.back().push_back(neighbour);
						}
						floodfill.push( neighbour );
					}
					clusters(neighbour.y,neighbour.x).get() = true;
				}
			}
		}
	}

	for( unsigned int i=0;i<rectangleList.points.size(); ++i ) {
		if( rectangleList.points[i].empty() )
			continue;

		Point* first = convexHull(rectangleList.points[i]);

		Point* left,* right,* top,* bottom;
		left = first; right = first; top = first; bottom = first;

		Point* each = first->next;
		while( each != first ) {
			checkFor(right,each,-8,-4);
			checkFor(top,each,4,-8);
			checkFor(left,each,8,4);
			checkFor(bottom,each,-4,8);
			each = each->next;
		}
		drawLine(*left,*top,out,Color::RED);
		drawLine(*top,*right,out,Color::GREEN);
		drawLine(*right,*bottom,out,Color::BLUE);
		drawLine(*bottom,*left,out,Color::WHITE);
	}

	writeBuffer(out,output);
};
