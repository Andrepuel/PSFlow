#include <iostream>
#include <memory>
#include <initializer_list>
#include <vector>
#include <assert.h>
#include "opengl_controller.h"
//#include "image_buffer.h"
#include "operation.h"
#include "detect.h"
#include "webcam.h"
#include "image_buffer_operation.h"
#include "view_operation.h"
#include "connected_space_operation.h"
#include "extract_rectangle_operation.h"
#include <sstream>

const unsigned int BASE_IMAGES=10;

int main(int argc, char** argv) {
	WebcamOperation* webcamop = new WebcamOperation(0);
	webcamop->setSizeFrom();
	OpenGlController::initializeGl(argc, argv);
	webcamop->setSizeFrom();
	
//	ImageBuffer::setSizeFrom("Images/empty1.jpg");
//	OperationDefinition::createImageLoader("input", argv[1]);
//	OperationDefinition::createImageSaver("save", argv[2]);

	OperationDefinition::createShader("grayscale", 1, 0, "grayscale.frag");
	OperationDefinition::createShader("maxGrayscale", 1, 0, "maxGrayscale.frag");
	OperationDefinition::createShader("canny", 1, 0, "canny.frag");
	OperationDefinition::createShader("sobel", 1, 0, "sobel.frag");
	OperationDefinition::createShader("diff", 2, 0, "diff.frag");
	OperationDefinition::createShader("mult", 2, 0, "mult.frag");
	OperationDefinition::createShader("distance", 2, 0, "distance.frag");
	OperationDefinition::createShader("junction", 2, 0, "junction.frag");
	OperationDefinition::createShader("threshold", 1, 2, "threshold.frag");
	OperationDefinition::createShader("dilation", 1, 0, "dilation.frag");
	OperationDefinition::createShader("erosion", 1, 0, "erosion.frag");
	OperationDefinition::createShader("red", 1, 0, "red.frag");
	OperationDefinition::createShader("equality", 1, 0, "equality.frag");
	OperationDefinition::createShader("fewColors", 1, 1, "fewColors.frag");
	OperationDefinition::createShader("gpu_connected_space", 1, 0, "gpu_connected_space.frag");
	OperationDefinition::createShader("thick", 1, 0, "thick.frag");

	OperationDefinition::create("connected_space", OperationDefinitionPtr(new ConnectedSpaceOperation()));
	OperationDefinition::create("detect", OperationDefinitionPtr(new InvaderDetectOperation()));
	OperationDefinition::create("webcam", OperationDefinitionPtr(webcamop));
	OperationDefinition::create("view", OperationDefinitionPtr(new ViewOperation));
	OperationDefinition::create("extract_rectangle", OperationDefinitionPtr(new ExtractRectangleOperation()));

	while( true ) {
		Operation oneFrame("webcam");
		Operation oneFrame2("webcam");

		Operation border
		{"maxGrayscale",
		{
			{"threshold",{0.8,1.0},
			{
				{"sobel",
				{
					oneFrame
				}
				}
			}
			}
		}
		}
		;

		Operation thicked
		{"thick",
		{
			border
		}
		}
		;

		Operation extractRectangle("extract_rectangle", {thicked,border} );

		Operation view
		{"view",
		{
			extractRectangle
		//	border
		}
		};
		

		view.operate();
	}

	return 0;

}

