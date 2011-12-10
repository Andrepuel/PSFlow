#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "image_buffer.h"
#include "opengl_controller.h"
#include <vector>
#include <map>
#include <assert.h>
#include <iostream>

class OperationDefinition;
typedef std::shared_ptr<OperationDefinition> OperationDefinitionPtr;
class OperationDefinition {
public:
	virtual ~OperationDefinition(){}
	static void createImageLoader(const std::string& id, const std::string& imagePath);
	static void createImageSaver(const std::string& id, const std::string& imagePath);
	static void createShader(const std::string& id, unsigned int inputImages, unsigned int inputParams, const std::string& shaderPath);
	static void create(const std::string& id, OperationDefinitionPtr operation) {
		assert( operations.find(id) == operations.end() );
		operations[id] = operation;
	}

	static OperationDefinitionPtr get(const std::string& id) {
		assert( operations.find(id) != operations.end() );
		return operations[id];
	}

	virtual void operate(const std::vector<ImageBufferPtr>& input, const std::vector<double>& params, ImageBufferPtr output)=0;
	virtual bool hasOutput() {
		return true;
	}
private:
	static std::map<std::string,OperationDefinitionPtr> operations;

protected:
	void attachInput(const std::vector<ImageBufferPtr>& input) {
		assert(input.size() == inputImages);

		GLint i = 0;
		for( auto eachBuffer = input.begin(); eachBuffer != input.end(); ++eachBuffer,++i) {
			(*eachBuffer)->attachAsInput(i);
		}
	}
	void attachParams(const std::vector<double>& params) {
		assert(params.size() == inputParams);

		GLint i =0;
		for( auto eachParam = params.begin(); eachParam != params.end(); ++eachParam,++i) {
			OpenGlController::attachParam(*eachParam,i);
		}
	}

	unsigned int inputImages;
	unsigned int inputParams;
};

class Operation {
private:
	struct OperationData;
	typedef std::shared_ptr<OperationData> OperationDataPtr;
	struct OperationData {
		~OperationData() {
			if( output.get() )  {
				ImageBufferPool::instance().freeBuffer(output);
			}
		}
		void requestOutput() {
			outputRequests++;
		}
		void freeOutput() {
			outputRequests--;
			if( outputRequests <= 0 ) {
				ImageBufferPool::instance().freeBuffer(output);
				output.reset();
			}
		}

		ImageBufferPtr operate() {
			if( output ) {
				return output;
			}

			std::vector<ImageBufferPtr> inputs;
			for( auto eachDepend = dependencies.begin(); eachDepend != dependencies.end(); ++eachDepend) {
				inputs.push_back( (*eachDepend)->operate() );
			}
		

			if( definition->hasOutput() )
				output = ImageBufferPool::instance().get();
			
			definition->operate(inputs, params, output);
			for( auto eachDepend = dependencies.begin(); eachDepend != dependencies.end(); ++eachDepend) {
				(*eachDepend)->freeOutput();
			}
			return output;
		}

		int outputRequests;
		OperationDefinitionPtr definition;
		std::vector<OperationDataPtr> dependencies;
		std::vector<double> params;

		std::string tmpName;

		ImageBufferPtr output;
	};
	OperationDataPtr p;
public:
	Operation() { //TODO remover este constructor
	};
	Operation(const std::string& defId) {
		setup(defId, {}, {});
	}
	Operation(const std::string& defId, std::initializer_list<Operation> depends) {
		setup(defId, depends, {});
	}
	Operation(const std::string& defId, std::initializer_list<double> params, std::initializer_list<Operation> depends) {
		setup(defId, depends, params);
	}

	ImageBufferPtr operate() {
		return p->operate();
	}

	static Operation repeat(const std::string& defId, unsigned int times, Operation depend) {
		if( times == 0 )
			return depend;
		Operation actual(defId, {depend});
		for( unsigned int i = 1; i < times; ++i) {
			actual = Operation(defId,{actual});
		};
		return actual;
	}

private:
	void setup(const std::string& defId, std::initializer_list<Operation> depends, std::initializer_list<double> params) {
		p.reset( new OperationData );

		p->outputRequests = 0;
		p->definition = OperationDefinition::get(defId);
		for( auto eachDepend = depends.begin(); eachDepend != depends.end(); ++eachDepend ) {
			p->dependencies.push_back(eachDepend->p);

			eachDepend->p->requestOutput();
		}
		p->params = std::vector<double>(params);
		p->tmpName = defId;
	}
};


#endif
