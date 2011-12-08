#include "connected_space_operation.h"
#include <boost/random.hpp>
#include <cassert>
#include <queue>

void ConnectedSpaceOperation::colorize( Matrix<Color>::Cell pixel, int cluster ){
	static std::map<int, Color> clusters = {
		{0, {0,0,0} },
		{1, {255,0,0} },
		{2, {0,255,0} },
		{3, {0,0,255} },
		{4, {255,0,255} },
		{5, {255,255,0} },
		{6, {255,255,255} },
		{7, {128,0,0} },
		{8, {128,255,0} },
		{9, {128,128,128} },
		{10, {128,128,0} }
	};
	
	if( cluster < 11 ) {
		pixel = clusters[cluster];
		return;
	}
	
	boost::mt19937 colorRandomGen(cluster);
	boost::uniform_int<> dist(0, 255*255*255);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > colorRandom(colorRandomGen, dist);

	int togetherColor = colorRandom();
	pixel.get().r = togetherColor & 0xFF;
	pixel.get().g = (togetherColor >> 8) & 0xFF;
	pixel.get().b = (togetherColor >> 16) & 0xFF;
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

		std::queue<Recursive> recurse;
		recurse.push(Recursive(y,x));

		while( !recurse.empty() ) {
			Recursive recursive = recurse.front();
			recurse.pop();
			int ry = recursive.ry;
			int rx = recursive.rx;

			for( int line = -1; line <= 1; line++){
			for( int column = -1; column <= 1; column++){
				if ( clusters(ry+line,rx+column).get() == -1 && input(ry+line,rx+column).get() == input(ry,rx).get() ){
					clusters(ry, rx).neighbour(line, column) = lastCluster;
					recurse.push( Recursive(ry+line, rx+column) );
				}
			}
			}	
		};
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
		colorize( backImage(y,x), clusters(y,x).get()%11 );

	}}

	writeBuffer(backImage,output);
};

