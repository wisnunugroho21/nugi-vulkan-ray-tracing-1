#include "point_light_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EnginePointLightModel::EnginePointLightModel(EngineDevice &device, std::shared_ptr<std::vector<PointLight>> lights) : engineDevice{device} {
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (int i = 0; i < lights->size(); i++) {
			boundBoxes.push_back(std::make_shared<LightBoundBox>(LightBoundBox{ i + 1, (*lights)[i] }));
		}

		this->createBuffers(lights, createBvh(boundBoxes));
	}

	void EnginePointLightModel::createBuffers(std::shared_ptr<std::vector<PointLight>> lights, std::shared_ptr<std::vector<BvhNode>> bvhNodes) {
		auto lightBufferSize = sizeof(PointLight) * lights->size();
		
		EngineBuffer lightStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(lightBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		};

		lightStagingBuffer.map();
		lightStagingBuffer.writeToBuffer(lights->data());

		this->lightBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(lightBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);

		this->lightBuffer->copyBuffer(lightStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(lightBufferSize));

		// -------------------------------------------------

		auto bvhBufferSize = sizeof(BvhNode) * bvhNodes->size();

		EngineBuffer bvhStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(bvhBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(bvhNodes->data());

		this->bvhBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(bvhBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);

		this->bvhBuffer->copyBuffer(bvhStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(bvhBufferSize));
	}
    
} // namespace nugiEngine

