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
		auto triangleData = this->createObjectData(datas);
		auto materialData = this->createMaterialData(datas);
		auto lightData = this->createLightData(datas);

		this->createBuffers(triangleData, bvhData, materialData, lightData);
	}

	EngineRayTraceModel::~EngineRayTraceModel() {}

	ObjectData EngineRayTraceModel::createObjectData(const RayTraceModelData &data) {
		ObjectData object;
		for (int i = 0; i < data.objects.size(); i++) {
			object.objects[i] = data.objects[i];
		}

		return object;
	}

	MaterialData EngineRayTraceModel::createMaterialData(const RayTraceModelData &data) {
		MaterialData materials;
		for (int i = 0; i < data.materials.size(); i++) {
			materials.materials[i] = data.materials[i];
		}

		return materials;
	}

	LightData EngineRayTraceModel::createLightData(const RayTraceModelData &data) {
		LightData lights;
		for (int i = 0; i < data.lights.size(); i++) {
			lights.lights[i] = data.lights[i];
		}

		return lights;
	}

	BvhData EngineRayTraceModel::createBvhData(const RayTraceModelData &data) {
		std::vector<std::shared_ptr<BoundBox>> objects;
		for (int i = 0; i < data.objects.size(); i++) {
			Object o = data.objects[i];

			objects.push_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ i, o }));
		}

		auto bvhNodes = createBvh(objects);
		BvhData bvh{};

		for (int i = 0; i < bvhNodes.size(); i++) {
			bvh.bvhNodes[i] = bvhNodes[i];
		}

		return bvh;
	}

	void EngineRayTraceModel::createBuffers(ObjectData &data, BvhData &bvh, MaterialData &material, LightData &light) {
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

		EngineBuffer materialStagingBuffer {
			this->engineDevice,
			sizeof(MaterialData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		materialStagingBuffer.map();
		materialStagingBuffer.writeToBuffer(&material);

		this->materialBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(MaterialData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->materialBuffer->copyBuffer(materialStagingBuffer.getBuffer(), sizeof(MaterialData));

		EngineBuffer lightStagingBuffer {
			this->engineDevice,
			sizeof(LightData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		lightStagingBuffer.map();
		lightStagingBuffer.writeToBuffer(&light);

		this->lightBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(LightData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->lightBuffer->copyBuffer(lightStagingBuffer.getBuffer(), sizeof(LightData));
	}

	std::unique_ptr<EngineRayTraceModel> EngineRayTraceModel::createModelFromFile(EngineDevice &device, const std::string &filePath) {
		RayTraceModelData modelData;
		modelData.loadModel(filePath);

		return std::make_unique<EngineRayTraceModel>(device, modelData);
	}

	void RayTraceModelData::loadModel(const std::string &filePath) {
		
	}
    
} // namespace nugiEngine

