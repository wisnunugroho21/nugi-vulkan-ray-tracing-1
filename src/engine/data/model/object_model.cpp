#include "object_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#include <tiny_obj_loader.h>

namespace nugiEngine {
	EngineObjectModel::EngineObjectModel(EngineDevice &device, std::shared_ptr<std::vector<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes, std::shared_ptr<EngineCommandBuffer> commandBuffer) : engineDevice{device} {
		this->createBuffers(objects, createBvh(boundBoxes), commandBuffer);
	}

	void EngineObjectModel::createBuffers(std::shared_ptr<std::vector<Object>> objects, std::shared_ptr<std::vector<BvhNode>> bvhNodes, std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		auto objectBufferSize = sizeof(Object) * objects->size();

		EngineBuffer objectStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(objectBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		};

		objectStagingBuffer.map();
		objectStagingBuffer.writeToBuffer(objects->data());

		this->objectBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(objectBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);

		this->objectBuffer->copyBuffer(objectStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(objectBufferSize), commandBuffer);

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

