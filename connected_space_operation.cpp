#include "connected_space_operation.h"
#include <boost/random.hpp>
#include <cassert>
#include <queue>
#include <vector>

void ConnectedSpaceOperation::colorize( Matrix<Color>::Cell pixel, int cluster ){
	static std::vector<Color> clusters = {
		{0,0,0},
		{255,0,0},
		{0,255,0},
		{0,0,255},
		{255,0,255},
		{255,255,0},
		{255,255,255},
		{128,0,0},
		{128,255,0},
		{128,128,128},
		{128,128,0}
	};
	
	if( cluster < clusters.size() ) {
		pixel = clusters[cluster];
		return;
	}
	
	for( int i=clusters.size(); i <= cluster; ++i ) {
		boost::mt19937 colorRandomGen(i);
		boost::uniform_int<> dist(0, 255*255*255);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > colorRandom(colorRandomGen, dist);
	
		int togetherColor = colorRandom();
		clusters.push_back(Color());
		clusters[i].r = togetherColor & 0xFF;
		clusters[i].g = (togetherColor >> 8) & 0xFF;
		clusters[i].b = (togetherColor >> 16) & 0xFF;
	}

	pixel = clusters[cluster];
}

ConnectedSpaceOperation::~ConnectedSpaceOperation() {
};

struct ConnectedSpaceOperation::Recursive {
	inline Recursive() {
	}
	inline Recursive(int y, int x) : ry(y), rx(x)
	{
	}
	int ry, rx;
};

static int distance(const Color& colorA, const Color& colorB) {
	return std::abs(colorA.r-colorB.r)+std::abs(colorA.g-colorB.g)+std::abs(colorA.b-colorB.b);
}

static void checkFor(ConnectedSpaceOperation::Recursive& greater, const ConnectedSpaceOperation::Recursive& candidate, int xFactor, int yFactor) {
	if( (candidate.rx*xFactor+candidate.ry*yFactor) > (greater.rx*xFactor+greater.ry*yFactor) ) {
		greater.ry = candidate.ry;
		greater.rx = candidate.rx;
	}
};

void ConnectedSpaceOperation::clusterize( const Matrix<Color>& input, Matrix<int>& clusters) {
	assert(input.width() == clusters.width() );
	assert(input.height() == clusters.height() );

	for(unsigned int y = 0; y < input.height(); y++)
	for(unsigned int x = 0; x < input.width(); x++) {
		clusters(y,x) = -1;
	}

	int lastCluster = 0;
	for(unsigned int y = 0; y < input.height(); y++ )
	for(unsigned int x = 0; x < input.width(); x++ ) {
		if( clusters(y,x).get() != -1 )
			continue;
		clusters(y,x) = ++lastCluster;

		Recursive left(y,x);
		Recursive right(y,x);
		Recursive top(y,x);
		Recursive bottom(y,x);

		int area=1;
		std::queue<Recursive> recurse;
		std::vector<Recursive> undoer;
		recurse.push(Recursive(y,x));

		while( !recurse.empty() ) {
			Recursive recursive = recurse.front();
			if( area < 250 )
				undoer.push_back(recursive);
			recurse.pop();
			int ry = recursive.ry;
			int rx = recursive.rx;

			checkFor(left,recursive,-4,-1);
			checkFor(top,recursive,1,-4);
			checkFor(right,recursive,4,1);
			checkFor(bottom,recursive,-1,4);

			for( int line = -1; line <= 1; line++){
			for( int column = -1; column <= 1; column++){
				if ( clusters(ry+line,rx+column).get() == -1 && distance(input(ry+line,rx+column).get(), input(ry,rx).get()) <= 20 ){
					clusters(ry, rx).neighbour(line, column) = lastCluster;
					++area;
					recurse.push( Recursive(ry+line, rx+column) );
				}
			}
			}	
		};
		clusters(left.ry,left.rx) = -2;
		clusters(right.ry,right.rx) = -2;
		clusters(top.ry,top.rx) = -2;
		clusters(bottom.ry,bottom.rx) = -2;

		if( area < 250 ) {
			for( auto each = undoer.begin(); each != undoer.end(); ++each ) {
				clusters(each->ry,each->rx) = 0;
			}
			--lastCluster;
		}
	}

};

void ConnectedSpaceOperation::operate( const std::vector<ImageBufferPtr>& inputList, const std::vector<double>&, ImageBufferPtr output ) {
	assert( inputList.size() == 1 );
	
	CpuImage input = readBuffer(inputList[0]);
	Matrix<int> clusters( input.height(), input.width() );
	clusterize(input,clusters);

	CpuImage backImage(input.height(),input.width() );

	for( unsigned int y = 0; y < input.height(); y++) {
	for( unsigned int x = 0; x < input.width(); x++) {
		if( clusters(y,x).get() == -2 ) {
			for( int line=-2;line<=2;++line)
			for( int col=-2;col<=2;++col) {
				backImage(y,x).neighbour(line,col).get().r = 255;
				backImage(y,x).neighbour(line,col).get().g = 255;
				backImage(y,x).neighbour(line,col).get().b = 255;
			}
		} else {
			colorize( backImage(y,x), clusters(y,x).get() );
		}

	}}

	writeBuffer(backImage,output);
};
