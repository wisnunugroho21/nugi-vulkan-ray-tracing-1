#include "point_light_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EnginePointLightModel::EnginePointLightModel(EngineDevice &device, std::shared_ptr<std::vector<PointLight>> pointLights, 
		std::shared_ptr<EngineCommandBuffer> commandBuffer) : engineDevice{device} 
	{
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;

		for (int i = 0; i < pointLights->size(); i++) {
			boundBoxes.push_back(std::make_shared<PointLightBoundBox>(PointLightBoundBox{ i + 1, (*pointLights)[i] }));
		}

		this->createBuffers(pointLights, createBvh(boundBoxes), commandBuffer);
	}

	void EnginePointLightModel::createBuffers(std::shared_ptr<std::vector<PointLight>> pointLights, std::shared_ptr<std::vector<BvhNode>> bvhNodes, 
		std::shared_ptr<EngineCommandBuffer> commandBuffer) 
	{
		auto pointLightBufferSize = sizeof(PointLight) * pointLights->size();
		
		EngineBuffer pointLightStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(pointLightBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		};

		pointLightStagingBuffer.map();
		pointLightStagingBuffer.writeToBuffer(pointLights->data());

		this->pointLightBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(pointLightBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);

		this->pointLightBuffer->copyBuffer(pointLightStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(pointLightBufferSize), commandBuffer);

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

		this->bvhBuffer->copyBuffer(bvhStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(bvhBufferSize), commandBuffer);
	}
    
} // namespace nugiEngine

