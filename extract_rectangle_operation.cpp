#include "extract_rectangle_operation.h"
#include "connected_space_operation.h"
#include <queue>

ExtractRectangleOperation::~ExtractRectangleOperation() {
}

static double pointDistance( const Point& point1, const Point& point2 ) {
	int dx = point1.x-point2.x;
	int dy = point1.y-point2.y;
	return std::sqrt( dx*dx+dy*dy );
}

static double distancePointLine( const Point& point, const Point& lineStart, const Point& lineEnd ) {
	double lineMag;
	double u;
	Point intersection;

	lineMag = pointDistance( lineEnd, lineStart );

	u = ( ( ( point.x - lineStart.x ) * ( lineEnd.x - lineStart.x ) ) +
	( ( point.y - lineStart.y ) * ( lineEnd.y - lineStart.y ) ) ) /
	( lineMag * lineMag );

	if( u < 0.0f || u > 1.0f ) {
		return std::max(pointDistance(point,lineStart),pointDistance(point,lineEnd));
	}

	intersection.x = lineStart.x + u * ( lineEnd.x - lineStart.x );
	intersection.y = lineStart.y + u * ( lineEnd.y - lineStart.y );

	return pointDistance(point,intersection);
}

static void drawLine( const Point& lineStart, const Point& lineEnd, CpuImage& out ) {
	Point each;
	for( each.y = 0; each.y < int(out.height()); ++each.y )
	for( each.x = 0; each.x < int(out.width()); ++each.x ) {
		if( distancePointLine(each,lineStart,lineEnd) < 1.1 ) {
			out(each.y,each.x).get().r = 255;
			out(each.y,each.x).get().g = 255;
			out(each.y,each.x).get().b = 255;
		}
	}
}

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
		for( auto eachOne = rectangleList.points[i].begin(); eachOne != rectangleList.points[i].end(); ++eachOne ) {
			ConnectedSpaceOperation::colorize(out(eachOne->y,eachOne->x),i+1);
		}
	}

	writeBuffer(out,output);
};
