#ifndef _DETECT_H_
#define _DETECT_H_

#include "operation.h"
#include <map>
#include "cpu_utils.h"

class InvaderDetectOperation : public OperationDefinition {
public:
	virtual void operate( const std::vector<ImageBufferPtr>& input, const std::vector<double>&, ImageBufferPtr output );
private:
	void colorize( Matrix<Color>::Cell pixel, int cluster );
	void maximize( Matrix<Color>::Cell pixel, int cluster );
	void minimize( Matrix<Color>::Cell pixel, int cluster );

	typedef std::pair<int,int> Point;
	std::map< int, Point > min, max;
};

#endif
