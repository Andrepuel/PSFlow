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
#include <sstream>

const unsigned int BASE_IMAGES=10;

int main(int argc, char** argv) {
//	if( argc < 3 ) {
//		std::cerr << "Diga arquivo de entrada e arquivo de saída." << std::endl;
//		return 1;
//	}
	WebcamOperation* webcamop = new WebcamOperation(0);
	webcamop->setSizeFrom();
	OpenGlController::initializeGl(argc, argv);
	webcamop->setSizeFrom();
	
//	ImageBuffer::setSizeFrom("Images/empty1.jpg");
//	OperationDefinition::createImageLoader("input", argv[1]);
//	OperationDefinition::createImageLoader("base1", "Images/empty1.jpg");
//	OperationDefinition::createImageLoader("base2", "Images/empty2.jpg");
//	OperationDefinition::createImageSaver("save", argv[2]);

	OperationDefinition::createShader("grayscale", 1, 0, "grayscale.frag");
	OperationDefinition::createShader("maxGrayscale", 1, 0, "maxGrayscale.frag");
	OperationDefinition::createShader("canny", 1, 0, "canny.frag");
	OperationDefinition::createShader("sobel", 1, 0, "sobel.frag");
	OperationDefinition::createShader("diff", 2, 0, "diff.frag");
	OperationDefinition::createShader("distance", 2, 0, "distance.frag");
	OperationDefinition::createShader("junction", 2, 0, "junction.frag");
	OperationDefinition::createShader("threshold", 1, 2, "threshold.frag");
	OperationDefinition::createShader("dilation", 1, 0, "dilation.frag");
	OperationDefinition::createShader("erosion", 1, 0, "erosion.frag");
	OperationDefinition::createShader("red", 1, 0, "red.frag");

	OperationDefinition::create("detect", OperationDefinitionPtr(new InvaderDetectOperation()));
	OperationDefinition::create("webcam", OperationDefinitionPtr(webcamop));
	OperationDefinition::create("view", OperationDefinitionPtr(new ViewOperation));

	for( unsigned int i=0;i<10;++i) {
		Operation consumeWeb("webcam");
		consumeWeb.operate();
		ImageBufferPool::instance().get();
	}

	for( unsigned int i=0;i<BASE_IMAGES;++i) {
		std::stringstream name;
		name << "base" << i;
		OperationDefinition::create(name.str(), OperationDefinitionPtr(new ImageBufferOperation()));
		
		Operation oneFrame("webcam");

		Operation saveBase
		{name.str(),
		{
			oneFrame
		}
		};

		Operation viewFrame
		{"view",
		{
			{"red",
			{
				oneFrame
			}
			}
		}
		};

		saveBase.operate();
		viewFrame.operate();
	}

	while( true ) {
		Operation oneFrame("webcam");

		Operation cascadeJunction
		{"junction",
		{
			{"distance", {{"base0"}, oneFrame}},
			{"distance", {{"base1"}, oneFrame}}
		}
		};
		for(unsigned int i=2;i<BASE_IMAGES;++i) {
			std::stringstream name;
			name << "base" << i;

			cascadeJunction = Operation(
			"junction",
			{
				cascadeJunction,
				{"distance", {{name.str()}, oneFrame}}
			}
			);
		}

		Operation threshold
		{"threshold", {0.25,1.0},
		{
			{"grayscale",
			{
				cascadeJunction
			}
			}
		}
		};

		Operation cleaned = Operation::repeat
		("erosion", 8,
			Operation::repeat
			("dilation", 6,
				Operation::repeat("erosion",2, threshold)
			)
		);

		Operation view
		{"view",
		{
			{"detect",
			{
				cleaned,
				oneFrame
			}
			}
		}
		};

		view.operate();
	}

	return 0;

}

