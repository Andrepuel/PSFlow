#include "operation.h"
#include <highgui.h>
#include <assert.h>
#include <sstream>

std::map<std::string,OperationDefinitionPtr> OperationDefinition::operations;

class ShaderOperation : public OperationDefinition {
public:
	ShaderOperation(unsigned int inputImages, unsigned int inputParams, const std::string& shaderPath) {
		this->inputImages = inputImages;
		this->inputParams = inputParams;
		this->shader = OpenGlController::loadShader(shaderPath);
	}
	
	virtual void operate(const std::vector<ImageBufferPtr>& input, const std::vector<double>& params, ImageBufferPtr output) {
		OpenGlController::attachShader(shader);
		attachInput(input);
		attachParams(params);
		output->attachAsOutput();
		OpenGlController::invokeShader();
	}
private:
	GLuint shader;
};

void OperationDefinition::createShader(const std::string& id, unsigned int inputImages, unsigned int inputParams, const std::string& shaderPath) {
	OperationDefinitionPtr shaderOp( new ShaderOperation(inputImages, inputParams, shaderPath) );
	create(id,shaderOp);
}

class ImageLoaderOperation: public OperationDefinition{
public:
	ImageLoaderOperation( const std::string& filename ): filename(filename){
	}

	virtual void operate( const std::vector<ImageBufferPtr>& input, const std::vector<double>&, ImageBufferPtr output ){
		assert( input.size() == 0 );
				
		cv::Mat image = cv::imread(filename);
		output->setData( image.data );
	}	
private:
	std::string filename;
};

void OperationDefinition::createImageLoader(const std::string& id, const std::string& imagePath) {
	OperationDefinitionPtr loaderOp( new ImageLoaderOperation( imagePath ) );
	create( id, loaderOp );
};

class ImageSaverOperation: public OperationDefinition{
public:
	ImageSaverOperation( const std::string& filename ): filename(filename){
	}

	std::string outname(unsigned int i) {
		std::stringstream stream;
		stream << filename << i << ".png";
		return stream.str();
	}

	virtual void operate( const std::vector<ImageBufferPtr>& input, const std::vector<double>&, ImageBufferPtr output ){
		assert( input.size() > 0 );

		unsigned int i=0;	
		for( auto eachInput = input.begin(); eachInput != input.end(); ++eachInput) {
			cv::Mat outputMat(ImageBuffer::height,ImageBuffer::width,CV_8UC3);
			(*eachInput)->getData( outputMat.data );
	
			cv::imwrite( outname(i++), outputMat );
		}
	}

	virtual bool hasOutput() {
		return false;
	}
private:
	std::string filename;
};

void OperationDefinition::createImageSaver( const std::string& id, const std::string& imagePath ){
	OperationDefinitionPtr saverOp( new ImageSaverOperation( imagePath ) );
	create( id, saverOp );	
}

