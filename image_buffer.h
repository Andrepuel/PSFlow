#ifndef _IMAGE_BUFFER_H_
#define _IMAGE_BUFFER_H_

#include <memory>
#include <map>
#include "opengl_controller.h"
#include <iostream>

class ImageBuffer {
public:
	static unsigned int width;
	static unsigned int height;
	ImageBuffer() {
		id = OpenGlController::createTexture(width,height);
	}
	~ImageBuffer() {
		OpenGlController::freeTexture(id);
	}

	void attachAsInput(GLint pos) {
		OpenGlController::attachInput(id,pos);
	}
	void attachAsOutput() {
		OpenGlController::attachOutput(id);
	}

	void setData( void* data ){
		OpenGlController::setTextureData( id, width, height, data );
	}

	void getData( void* data ){
		OpenGlController::getTextureData( id, data );
	}

	static void setSizeFrom(const std::string& imageFile);

	GLuint getId() {
		return id;
	}
private:
	GLuint id;
};
typedef std::shared_ptr<ImageBuffer> ImageBufferPtr;

class ImageBufferPool {
private:
	static ImageBufferPool _instance;
public:
	static ImageBufferPool& instance() {
		return _instance;
	}

	ImageBufferPtr get() {
		for( auto each = buffers.begin(); each != buffers.end(); ++each ) {
			if( each->second == false ) {
				each->second = true;
				return each->first;
			}
		}
		ImageBufferPtr newBuffer( new ImageBuffer() );
		buffers[newBuffer] = true;
		return newBuffer;
	}
	void freeBuffer(ImageBufferPtr toFree) {
		buffers[toFree] = false;
	}

private:
	std::map<ImageBufferPtr,bool> buffers;
};

#endif
