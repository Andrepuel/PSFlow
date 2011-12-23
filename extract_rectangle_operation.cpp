#include "extract_rectangle_operation.h"
#include "connected_space_operation.h"
#include <queue>
#include <cassert>
#include <functional>
#include <algorithm>

ExtractRectangleOperation::~ExtractRectangleOperation() {
}

static double determinant(double aa, double ab, double ba, double bb) {
	return aa*bb-ab*ba;
}
static double determinant(double aa, double ba) {
	return aa*1.0-1.0*ba;
}

Point Point::lineIntersect(const Point& a0, const Point& a1, const Point& b0, const Point& b1) {
	double aQuad = determinant(a0.x,a0.y,a1.x,a1.y);
	double bQuad = determinant(b0.x,b0.y,b1.x,b1.y);
	double aXQuad = determinant(a0.x, a1.x);
	double aYQuad = determinant(a0.y, a1.y);
	double bXQuad = determinant(b0.x, b1.x);
	double bYQuad = determinant(b0.y, b1.y);

	double divisor = determinant(aXQuad, aYQuad, bXQuad, bYQuad);
	double x = determinant(aQuad, aXQuad, bQuad, bXQuad)/divisor;
	double y = determinant(aQuad, aYQuad, bQuad, bYQuad)/divisor;

	return Point(x,y);
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
	for( each.y = std::max(std::min(lineStart.y,lineEnd.y),0); each.y <= std::min(std::max(lineEnd.y,lineStart.y),int(out.height())); ++each.y )
	for( each.x = std::max(std::min(lineStart.x,lineEnd.x),0); each.x <= std::min(std::max(lineEnd.x,lineStart.x),int(out.height())); ++each.x ) {
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

static void nFirst(const std::vector<Point>& in, unsigned n, std::vector<Point>& out, std::function<bool (const Point& a, const Point& b)> compare) {
	if(in.size() < n) {
		out = in;
		std::sort(out.begin(),out.end(),compare);
		return;
	}
	out.resize(n);
	for( unsigned i=0;i<n;++i) {
		out[i] = in[i];
	}

	for( auto each = in.begin(); each != in.end(); ++each ) {
		for( auto eachPos = out.begin(); eachPos != out.end(); ++eachPos ) {
			if( compare(*each,*eachPos) ) {
				out.insert(eachPos,*each);
				out.resize(n);
				break;
			}
		}
	}
};

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
		std::vector<Point>& points = rectangleList.points[i];
		if( points.empty() )
			continue;

		Point* first = convexHull(points);
		Point* longest = first;

		Point* each=first->next;
		unsigned amount = 0;
		while( each != first ) {
			if( longest->distance(*longest->next) < each->distance(*each->next) ) {
				longest = each;
			}
			each = each->next;
			++amount;
		}

		if( amount < 4 ) continue;

		each = longest;
		unsigned loops=0;
		while( loops < amount ) {
			assert(each != NULL);
			assert(each->next != NULL);
			assert(each->next->next != NULL);
			if( Point::angleCossine(*each,*each->next,*each->next,*each->next->next) >= 0.95 ) {
				Point* middle = each->next;
				each->next = middle->next;
				middle->prev = NULL;
				middle->next = NULL;
				each->next->prev = each;
			} else {
				loops++;
				each = each->next;
			}
		};

		std::vector<Point> biggest;
		nFirst(points,4,biggest,
			[](const Point& a, const Point& b) -> bool {
				if( a.next == NULL )
					return false;
				if( b.next == NULL )
					return true;
				return a.distance(*a.next) > b.distance(*b.next);
			}
		);

		if( biggest.size() != 4 || biggest[3].next == NULL ) continue;

		std::vector<Point> orderedLines;
		orderedLines.push_back( *biggest.begin() );
		for( unsigned int i = 0; i < 3; ++i ) {
			Point* closest = NULL;
			for( auto each = biggest.begin(); each != biggest.end(); ++each ) {
				if( closest == NULL || orderedLines[i].next->distance(*each) < orderedLines[i].next->distance(*closest) ) {
					closest = &(*each);
				}
			}
			orderedLines.push_back( *closest );
		}

		std::vector<Point> rectangleCorners;
		for( unsigned int i=0;i<4;++i ) {
//			drawLine(orderedLines[i],*orderedLines[i].next,out,Color::WHITE);
			rectangleCorners.push_back( Point::lineIntersect(orderedLines[i],*orderedLines[i].next,orderedLines[(i+1)%4],*orderedLines[(i+1)%4].next) );
		}

		for( unsigned int i=0;i<4;++i ) {
			drawLine(rectangleCorners[i],rectangleCorners[(i+1)%4],out,Color::WHITE);
		}
	}

	writeBuffer(out,output);
};
