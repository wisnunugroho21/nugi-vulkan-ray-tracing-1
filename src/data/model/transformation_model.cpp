#include "transformation_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineTransformationModel::EngineTransformationModel(EngineDevice &device, std::vector<std::shared_ptr<Transformation>> transformations) : engineDevice{device} {
		auto transformationData = this->createTransformationData(transformations);
		this->createBuffers(transformationData);
	}

	EngineTransformationModel::EngineTransformationModel(EngineDevice &device, std::vector<std::shared_ptr<TransformComponent>> transformations) : engineDevice{device} {
		auto transformationData = this->createTransformationData(this->convertToMatrix(transformations));
		this->createBuffers(transformationData);
	}

	EngineTransformationModel::~EngineTransformationModel() {}

	std::vector<std::shared_ptr<Transformation>> EngineTransformationModel::convertToMatrix(std::vector<std::shared_ptr<TransformComponent>> transformations) {
		std::vector<std::shared_ptr<Transformation>> newTransforms{};
		for (auto &&transform : transformations) {
			newTransforms.emplace_back(std::make_shared<Transformation>(Transformation{ transform->matrix(), transform->inverseMatrix() }));
		}

		return newTransforms;
	}

	TransformationData EngineTransformationModel::createTransformationData(std::vector<std::shared_ptr<Transformation>> transformations) {
		TransformationData transformationData{};
		for (int i = 0; i < transformations.size(); i++) {
			transformationData.transformations[i] = *transformations[i];
		}

		return transformationData;
	}

	void EngineTransformationModel::createBuffers(TransformationData &data) {
		EngineBuffer transformationStagingBuffer {
			this->engineDevice,
			sizeof(TransformationData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		transformationStagingBuffer.map();
		transformationStagingBuffer.writeToBuffer(&data);

		this->transformationBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(TransformationData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->transformationBuffer->copyBuffer(transformationStagingBuffer.getBuffer(), sizeof(TransformationData));
	} 
} // namespace nugiEngine

