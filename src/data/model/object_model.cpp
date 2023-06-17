#include "object_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#include <tiny_obj_loader.h>

namespace nugiEngine {
	EngineObjectModel::EngineObjectModel(EngineDevice &device, std::shared_ptr<std::vector<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes) : engineDevice{device} {
		this->createBuffers(objects, createBvh(boundBoxes));
	}

	void EngineObjectModel::createBuffers(std::shared_ptr<std::vector<Object>> objects, std::shared_ptr<std::vector<BvhNode>> bvhNodes) {
		auto objectBufferSize = sizeof(Object) * objects->size();

		EngineBuffer objectStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(objectBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		objectStagingBuffer.map();
		objectStagingBuffer.writeToBuffer(objects->data());

		this->objectBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(objectBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->objectBuffer->copyBuffer(objectStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(objectBufferSize));

		// -------------------------------------------------

		auto bvhBufferSize = sizeof(BvhNode) * bvhNodes->size();

		EngineBuffer bvhStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(bvhBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(bvhNodes->data());

		this->bvhBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(bvhBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->bvhBuffer->copyBuffer(bvhStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(bvhBufferSize));
	} 
} // namespace nugiEngine

