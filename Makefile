CXXFLAGS=-Wall -O2 -g0 -std=c++0x -I/usr/include/opencv

all: psflow

psflow: main.o opengl_controller.o operation.o image_buffer.o detect.o cpu_utils.o webcam.o image_buffer_operation.o view_operation.o connected_space_operation.o extract_rectangle_operation.o
	g++ $^ -o $@ ${CXXFLAGS} -lGL -lglut -lGLEW  -lopencv_highgui -lGLU

main.o: main.cpp operation.h detect.h

opengl_controller.o: opengl_controller.h opengl_controller.cpp

image_buffer.o: image_buffer.h image_buffer.cpp

detect.o: detect.cpp detect.h

cpu_utils.o: cpu_utils.cpp cpu_utils.h

webcam.o: webcam.h webcam.cpp

image_buffer_operation.o: image_buffer_operation.cpp image_buffer_operation.h

view_operation.o: view_operation.h view_operation.cpp

connected_space_operation.o: connected_space_operation.h connected_space_operation.cpp

extract_rectangle_operation.o: extract_rectangle_operation.h extract_rectangle_operation.cpp

clean:
	rm *.o psflow
