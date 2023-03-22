#include "ray_trace_model.hpp"
#include "../utils/utils.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "bvh.hpp"

namespace nugiEngine {
	EngineRayTraceModel::EngineRayTraceModel(EngineDevice &device, RayTraceModelData &datas) : engineDevice{device} {
		auto bvhData = this->createBvhData(datas);
		auto triangleData = this->createModelData(datas);

		this->createBuffers(triangleData, bvhData);
	}

	EngineRayTraceModel::~EngineRayTraceModel() {}

	ModelData EngineRayTraceModel::createModelData(const RayTraceModelData &data) {
		ModelData object;
		for (int i = 0; i < data.objects.size(); i++) {
			object.objects[i] = data.objects[i];
		}

		return object;
	}

	BvhData EngineRayTraceModel::createBvhData(const RayTraceModelData &data) {
		std::vector<ModelBoundBox> objects;
		for (int i = 0; i < data.objects.size(); i++) {
			Model o = data.objects[i];
			objects.push_back({i, o});
		}

		auto bvhNodes = createBvh(objects);
		BvhData bvh{};

		for (int i = 0; i < bvhNodes.size(); i++) {
			bvh.bvhNodes[i] = bvhNodes[i];
		}

		return bvh;
	}

	void EngineRayTraceModel::createBuffers(ModelData &data, BvhData &bvh) {
		EngineBuffer objectStagingBuffer {
			this->engineDevice,
			sizeof(ModelData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		objectStagingBuffer.map();
		objectStagingBuffer.writeToBuffer(&data);

		this->objectBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(ModelData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->objectBuffer->copyBuffer(objectStagingBuffer.getBuffer(), sizeof(ModelData));

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

