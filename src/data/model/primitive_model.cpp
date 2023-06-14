#include "primitive_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace nugiEngine {
	void EnginePrimitiveModel::addPrimitive(std::vector<std::shared_ptr<Primitive>> curPrimitives) {
		auto curBvhNodes = this->createBvhData(curPrimitives);

		for (int i = 0; i < curBvhNodes.size(); i++) {
			this->bvhNodes.emplace_back(curBvhNodes[i]);
		}

		for (int i = 0; i < curPrimitives.size(); i++) {
			this->primitives.emplace_back(curPrimitives[i]);
		}
	}

	std::vector<std::shared_ptr<BvhNode>> EnginePrimitiveModel::createBvhData(std::vector<std::shared_ptr<Primitive>> primitives) {
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (int i = 0; i < primitives.size(); i++) {
			boundBoxes.push_back(std::make_shared<PrimitiveBoundBox>(PrimitiveBoundBox{ i, primitives[i] }));
		}

		return createBvh(boundBoxes);
	}

	void EnginePrimitiveModel::createBuffers() {
		PrimitiveData data{};
		for (int i = 0; i < this->primitives.size(); i++) {
			data.primitives[i] = *this->primitives[i];
		}

		BvhData bvh{};
		for (int i = 0; i < this->bvhNodes.size(); i++) {
			bvh.bvhNodes[i] = *this->bvhNodes[i];
		}

		EngineBuffer primitiveStagingBuffer {
			this->engineDevice,
			sizeof(PrimitiveData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		primitiveStagingBuffer.map();
		primitiveStagingBuffer.writeToBuffer(&data);

		this->primitiveBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(PrimitiveData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->primitiveBuffer->copyBuffer(primitiveStagingBuffer.getBuffer(), sizeof(PrimitiveData));

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

	std::vector<std::shared_ptr<Primitive>> EnginePrimitiveModel::createPrimitivesFromFile(EngineDevice &device, const std::string &filePath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::vector<std::shared_ptr<Primitive>> primitives{};


		return primitives;
	} 
} // namespace nugiEngine

