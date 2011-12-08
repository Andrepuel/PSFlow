#include "opengl_controller.h"
#include <iostream>
#include "image_buffer.h"
#include <GL/glut.h>
#include <map>
#include <sstream>
#include <fstream>

using std::string;

static GLuint framebuffer;
static GLuint rect[3];
void OpenGlController::initializeGl(int argc, char** argv) {
	//Glut stuff
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(ImageBuffer::width, ImageBuffer::height);
	glutCreateWindow("");

	//OpenGL stuff
	glewInit();
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glEnable( GL_TEXTURE_2D );
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	//Rectangle Buffer
	glGenBuffers(3, rect);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	GLfloat vertexes[] = {0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };
	glBindBuffer(GL_ARRAY_BUFFER, rect[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), vertexes, GL_STATIC_DRAW);
	
	GLfloat texCoords[] = {0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0};
	glBindBuffer(GL_ARRAY_BUFFER, rect[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

	GLuint faces[] = {0, 1, 2, 0, 2, 3};
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(GLuint), faces, GL_STATIC_DRAW); 

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	//Framebuffer
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

string readSource(const string& filename) {
	string toReturn;

	std::ifstream file(filename.c_str());
	while( !file.eof() ) {
		string buffer;
		std::getline(file,buffer);
		toReturn += buffer;
	}

	return toReturn;
}

static std::map<GLuint,GLuint> programsShader;
GLuint OpenGlController::loadShader(const string& path) {
	string fragSource = readSource(path);

	GLuint fragStorage = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragStorage, 1, (const GLchar**)&fragSource, 0);
	glCompileShader(fragStorage);
	GLint compileStatus;
	glGetShaderiv(fragStorage, GL_COMPILE_STATUS, &compileStatus);
	if( compileStatus == false ) {
		int msgLength;
		glGetShaderiv(fragStorage, GL_INFO_LOG_LENGTH, &msgLength);
		
		char* msg = new char[msgLength];
		glGetShaderInfoLog(fragStorage, msgLength, &msgLength, msg);
		string cppMsg(msg);

		std::cout << msg << std::endl;
		delete msg;
			
		throw cppMsg;
	}

	GLuint programStorage = glCreateProgram();
	glAttachShader(programStorage, fragStorage);
	glLinkProgram(programStorage);
	GLint linkStatus;
	glGetProgramiv(programStorage, GL_LINK_STATUS, &linkStatus);
	if( linkStatus == false ) {
		int msgLength;
		glGetProgramiv(programStorage, GL_INFO_LOG_LENGTH, &msgLength);
		
		char* msg = new char[msgLength];
		glGetProgramInfoLog(programStorage, msgLength, &msgLength, msg);
		string cppMsg(msg);
		delete msg;

		throw cppMsg;
	}

	programsShader[programStorage] = fragStorage;

	glUseProgram(programStorage);
	GLint widthUniform = glGetUniformLocation(programStorage, "width");
	GLint heightUniform = glGetUniformLocation(programStorage, "height");
	if( widthUniform >= 0 )
		glUniform1f(widthUniform, ImageBuffer::width);
	if( heightUniform >= 0 )
		glUniform1f(heightUniform, ImageBuffer::height);

	return programStorage;
}
GLuint OpenGlController::createTexture(unsigned int width, unsigned height) {
	GLuint texId[1];
	glGenTextures(1, texId);

	glBindTexture( GL_TEXTURE_2D, texId[0] );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0 );

	return texId[0];
}

void OpenGlController::freeShader(GLuint programId) {
	glDeleteShader(programsShader[programId]);
	glDeleteProgram(programId);	
}
void OpenGlController::freeTexture(GLuint texId) {
	glDeleteTextures(1, &texId);
}
	
void OpenGlController::setDrawViewport(unsigned width, unsigned height) {
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, 1, 1, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void OpenGlController::setViewport(unsigned width, unsigned height) {
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, 1, 0, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void OpenGlController::attachShader(GLuint shader) {
	glUseProgram(shader);
}
void OpenGlController::attachOutput(GLuint texture) {
	glFramebufferTextureEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);	
}
string uniformName(const std::string& pre, GLint pos) {
	std::stringstream composeName;
	composeName << pre;
	composeName << pos;
	return composeName.str();

}
void OpenGlController::attachInput(GLuint texture, GLint pos) {
	GL_CHECK_ERROR();
	glActiveTexture(GL_TEXTURE0 + pos);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLint currentProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
	if( currentProgram > 0) {
		GLint uniformLocation = glGetUniformLocation( currentProgram, uniformName("t",pos).c_str() );
		if( uniformLocation >= 0 )
			glUniform1i( uniformLocation, pos );
	}
}

void OpenGlController::attachParam(double param, GLint pos) {
	GLint currentProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
	glUniform1f( glGetUniformLocation( currentProgram, uniformName("p",pos).c_str() ), param );
}

void OpenGlController::invokeShader() {
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, rect[0]);
	glVertexPointer(2, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, rect[1]);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect[2]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void OpenGlController::setTextureData( GLuint texture, unsigned width, unsigned height, void* data ){
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, data ); 	
}

void OpenGlController::getTextureData( GLuint texture, void* data ){
	glBindTexture( GL_TEXTURE_2D, texture );
	glGetTexImage( GL_TEXTURE_2D, 0, GL_BGR, GL_UNSIGNED_BYTE, data );
}

void OpenGlController::resizeWindow(unsigned width, unsigned height) {
	glutReshapeWindow(width, height);
}
void OpenGlController::draw() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	setDrawViewport(ImageBuffer::width,ImageBuffer::height);
	
	invokeShader();

	glutSwapBuffers();	
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	setViewport(ImageBuffer::width,ImageBuffer::height);
}
