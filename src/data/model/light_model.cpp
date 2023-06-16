#include "light_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineLightModel::EngineLightModel(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights) : engineDevice{device} {
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (int i = 0; i < lights.size(); i++) {
			boundBoxes.push_back(std::make_shared<LightBoundBox>(LightBoundBox{ i, lights[i] }));
		}

		this->createBuffers(lights, createBvh(boundBoxes));
	}

	void EngineLightModel::createBuffers(std::vector<std::shared_ptr<Light>> lights, std::vector<std::shared_ptr<BvhNode>> bvhNodes) {
		auto lightBufferSize = sizeof(Light) * lights.size();
		
		EngineBuffer lightStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(lightBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		lightStagingBuffer.map();
		lightStagingBuffer.writeToBuffer(lights.data());

		this->lightBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(lightBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->lightBuffer->copyBuffer(lightStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(lightBufferSize));

		// -------------------------------------------------

		auto bvhBufferSize = sizeof(BvhNode) * bvhNodes.size();

		EngineBuffer bvhStagingBuffer {
			this->engineDevice,
			sizeof(BvhData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(bvhNodes.data());

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

