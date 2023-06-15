#include "object_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#include <tiny_obj_loader.h>

namespace nugiEngine {
	EngineObjectModel::EngineObjectModel(EngineDevice &device, std::vector<std::shared_ptr<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes) : engineDevice{device} {
		this->createBuffers(objects, boundBoxes);
	}

	void EngineObjectModel::createBuffers(std::vector<std::shared_ptr<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes) {
		auto bvhNodes = createBvh(boundBoxes);

		ObjectData data{};
		for (int i = 0; i < objects.size(); i++) {
			data.objects[i] = *objects[i];
		}

		BvhData bvh{};
		for (int i = 0; i < bvhNodes.size(); i++) {
			bvh.bvhNodes[i] = *bvhNodes[i];
		}

		EngineBuffer objectStagingBuffer {
			this->engineDevice,
			sizeof(ObjectData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		objectStagingBuffer.map();
		objectStagingBuffer.writeToBuffer(&data);

		this->objectBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(ObjectData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->objectBuffer->copyBuffer(objectStagingBuffer.getBuffer(), sizeof(ObjectData));

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

