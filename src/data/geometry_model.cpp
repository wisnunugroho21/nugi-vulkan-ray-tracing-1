#include "geometry_model.hpp"
#include "../utils/utils.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "bvh.hpp"

namespace nugiEngine {
	EngineGeometryModel::EngineGeometryModel(EngineDevice &device, std::vector<std::shared_ptr<Object>> objects) : engineDevice{device} {
		auto bvhData = this->createBvhData(objects);
		auto objectData = this->createObjectData(objects);

		this->createBuffers(objectData, bvhData);
	}

	EngineGeometryModel::~EngineGeometryModel() {}

	BvhData EngineGeometryModel::createBvhData(std::vector<std::shared_ptr<Object>> objects) {
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (int i = 0; i < objects.size(); i++) {
			boundBoxes.push_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ i, objects[i] }));
		}

		auto bvhNodes = createBvh(boundBoxes);
		BvhData bvh{};

		for (int i = 0; i < bvhNodes.size(); i++) {
			bvh.bvhNodes[i] = bvhNodes[i];
		}

		return bvh;
	}

	ObjectData EngineGeometryModel::createObjectData(std::vector<std::shared_ptr<Object>> objects) {
		ObjectData objectData{};
		for (int i = 0; i < objects.size(); i++) {
			objectData.objects[i] = *objects[i];
		}

		return objectData;
	}

	void EngineGeometryModel::createBuffers(ObjectData &data, BvhData &bvh) {
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

