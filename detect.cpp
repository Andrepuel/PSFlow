#include "detect.h"
#include "assert.h"
#include <map>
#include <queue>
//#include <boost/random.hpp> Sem random por enquanto

void InvaderDetectOperation::colorize( Matrix<Color>::Cell pixel, int cluster ){
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
/*	
	boost::mt19937 colorRandomGen(cluster);
	boost::uniform_int<> dist(0, 255*255*255);
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > colorRandom(colorRandomGen, dist);

	int togetherColor = colorRandom();
	pixel.get().r = togetherColor & 0xFF;
	pixel.get().g = (togetherColor >> 8) & 0xFF;
	pixel.get().b = (togetherColor >> 16) & 0xFF;
*/
}

void InvaderDetectOperation::maximize( Matrix<Color>::Cell pixel, int cluster ){
	auto maxFound = max.find( cluster );

	if( maxFound == max.end() ){
		max[cluster] = Point(pixel.x(), pixel.y() );
		return;
	}
	if( maxFound->second.first < pixel.x() ){
		maxFound->second.first = pixel.x();
	}	

	maxFound->second.second = ( maxFound->second.second > pixel.y() ? maxFound->second.second : pixel.y() );
}
void InvaderDetectOperation::minimize( Matrix<Color>::Cell pixel, int cluster ){
	auto minFound = min.find( cluster );

	if( minFound == min.end() ){
		min[cluster] = Point(pixel.x(), pixel.y() );
		return;
	}
	if( minFound->second.first > pixel.x() ){
		minFound->second.first = pixel.x();
	}	

	minFound->second.second = ( minFound->second.second < pixel.y() ? minFound->second.second : pixel.y() );
}

struct Recursive {
	inline Recursive() {
	}
	inline Recursive(int y, int x) : ry(y), rx(x)
	{
	}
	int ry, rx;
};

void InvaderDetectOperation::operate( const std::vector<ImageBufferPtr>& inputList, const std::vector<double>&, ImageBufferPtr output ) {
	assert(inputList.size() == 2);
	min.clear();
	max.clear();

	CpuImage input = readBuffer(inputList[0]);
	Matrix<int> clusters( input.height(), input.width() );

	for(unsigned int y = 0; y < input.height(); y++)
	for(unsigned int x = 0; x < input.width(); x++) {
		clusters(y,x) = -1;
	}

	int lastCluster = 0;
	for(unsigned int y = 0; y < input.height(); y++ )
	for(unsigned int x = 0; x < input.width(); x++ ) {
		if( clusters(y,x).get() != -1 )
			continue;
		if( ! input(y,x).get().r ){
			clusters(y,x) = 0;
			continue;
		}
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
				if ( clusters(ry,rx).neighbour(line,column).get() == -1 && input(ry,rx).neighbour(line,column).get().r != 0 ){
					clusters(ry, rx).neighbour(line, column) = lastCluster;
					recurse.push( Recursive(ry+line, rx+column) );
				}
			}
			}	
		};
	}

	CpuImage backImage = readBuffer(inputList[1]);
	for( unsigned int y = 0; y < input.height(); y++)
	for( unsigned int x = 0; x < input.width(); x++) {
		maximize( backImage(y,x) , clusters(y,x).get() );
		minimize( backImage(y,x) , clusters(y,x).get() );
	}

	{
		auto minI = min.begin();
		auto maxI = max.begin();
		for( ; minI != min.end(); ++minI,++maxI) {
			if( minI->first == 0 )
				continue;

			int clusterWidth = maxI->second.first-minI->second.first;
			int clusterHeight = maxI->second.second-minI->second.second;
			int area = clusterHeight*clusterWidth;
			if( area < 500 )
				continue;
			double ratio = double(clusterHeight)/double(clusterWidth);
		//	std::cout << minI->first << ": (" << clusterWidth << "," << clusterHeight << "), ratio:" << ratio;
		//	std::cout << ", area:" << area << std::endl;

			int color = ratio > 1.2 ? 3 : 2;
			Point line = minI->second;
			for( ; line.second <= maxI->second.second; ++line.second) {
				colorize( backImage(line.second,line.first), color );
			}
			for( ; line.first <= maxI->second.first; ++line.first) {
				colorize( backImage(line.second,line.first), color );
			}
			for( ; line.second >= minI->second.second; --line.second) {
				colorize( backImage(line.second,line.first), color );
			}
			for( ; line.first >= minI->second.first; --line.first) {
				colorize( backImage(line.second,line.first), color );
			}
		}
	}

	writeBuffer(backImage,output);
}
