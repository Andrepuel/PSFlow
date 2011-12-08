#ifndef _OPENGL_CONTROLLER_H_
#define _OPENGL_CONTROLLER_H_

#include <string>
#include <GL/glew.h>
#include <GL/gl.h>

class OpenGlController {
public:
	static void initializeGl(int argc, char** argv);
	
	static GLuint loadShader(const std::string& );
	static GLuint createTexture(unsigned int width, unsigned height);
	static void setTextureData( GLuint texture, unsigned width, unsigned height, void* data );
	static void getTextureData( GLuint texture, void* data );
	static void freeShader(GLuint);
	static void freeTexture(GLuint);

	static void setViewport(unsigned width, unsigned height);
	static void setDrawViewport(unsigned width, unsigned height);
	static void attachShader(GLuint);
	static void attachOutput(GLuint texture);
	static void attachInput(GLuint texture, GLint pos);
	static void attachParam(double, GLint pos);
	
	static void invokeShader();
	static void draw();
	static void resizeWindow(unsigned width, unsigned height);
};

#define GL_CHECK_ERROR() do { \
		GLenum __gl_error = glGetError(); \
		if (__gl_error != GL_NO_ERROR) { \
			std::cerr << "CHECK_ERROR failed at " << __FILE__ << ":" << __LINE__ << ":" \
					<< " " << gluErrorString(__gl_error) << std::endl; \
			abort();  \
		} \
	} while(0);

#endif
