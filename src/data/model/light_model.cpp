#include "light_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineLightModel::EngineLightModel(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights) : engineDevice{device} {
		auto bvhData = this->createBvhData(lights);
		auto lightData = this->createLightData(lights);

		this->createBuffers(lightData, bvhData);
	}

	EngineLightModel::~EngineLightModel() {}

	BvhData EngineLightModel::createBvhData(std::vector<std::shared_ptr<Light>> lights) {
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (int i = 0; i < lights.size(); i++) {
			boundBoxes.push_back(std::make_shared<LightBoundBox>(LightBoundBox{ i, lights[i] }));
		}

		auto bvhNodes = createBvh(boundBoxes);
		BvhData bvh{};

		for (int i = 0; i < bvhNodes.size(); i++) {
			bvh.bvhNodes[i] = *bvhNodes[i];
		}

		return bvh;
	}

	LightData EngineLightModel::createLightData(std::vector<std::shared_ptr<Light>> lights) {
		LightData lightData{};
		for (int i = 0; i < lights.size(); i++) {
			lightData.lights[i] = *lights[i];
		}

		return lightData;
	}

	void EngineLightModel::createBuffers(LightData &data, BvhData &bvh) {
		EngineBuffer lightStagingBuffer {
			this->engineDevice,
			sizeof(LightData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		lightStagingBuffer.map();
		lightStagingBuffer.writeToBuffer(&data);

		this->lightBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(LightData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->lightBuffer->copyBuffer(lightStagingBuffer.getBuffer(), sizeof(LightData));

		// -------------------------------------------------

		EngineBuffer bvhStagingBuffer {
			this->engineDevice,
			sizeof(BvhData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(&bvh);

		this->bvhBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(BvhData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->bvhBuffer->copyBuffer(bvhStagingBuffer.getBuffer(), sizeof(BvhData));
	}
    
} // namespace nugiEngine

