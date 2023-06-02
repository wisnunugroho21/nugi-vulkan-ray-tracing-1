#include "primitive_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace nugiEngine {
	void EnginePrimitiveModel::addPrimitive(std::vector<std::shared_ptr<Primitive>> curPrimitives) {
		auto curBvhNodes = this->createBvhData(primitives);

		for (auto &&node : curBvhNodes) {
			this->bvhNodes.emplace_back(node);
		}

		for (auto &&prim : curPrimitives) {
			this->primitives.emplace_back(curPrimitives);
		}
	}

	std::vector<std::shared_ptr<BvhNode>> EnginePrimitiveModel::createBvhData(std::vector<std::shared_ptr<Primitive>> primitives) {
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (uint32_t i = 0; i < primitives.size(); i++) {
			boundBoxes.push_back(std::make_shared<PrimitiveBoundBox>(PrimitiveBoundBox{ i, primitives[i] }));
		}

		return createBvh(boundBoxes);
	}

	void EnginePrimitiveModel::createBuffers() {
		PrimitiveData data{};
		for (uint32_t i = 0; i < this->primitives.size(); i++) {
			data.primitives[i] = *this->primitives[i];
		}

		BvhData bvh{};
		for (uint32_t i = 0; i < this->bvhNodes.size(); i++) {
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

		for (const auto &shape: shapes) {
			for (uint32_t indexFace = 0; indexFace < shape.mesh.material_ids.size(); indexFace++) {
				int vertexIndex0 = shape.mesh.indices[3 * indexFace + 0].vertex_index;
				int vertexIndex1 = shape.mesh.indices[3 * indexFace + 1].vertex_index;
				int vertexIndex2 = shape.mesh.indices[3 * indexFace + 2].vertex_index;
				
				glm::vec3 point1 = {
					attrib.vertices[3 * vertexIndex0 + 0],
					attrib.vertices[3 * vertexIndex0 + 1],
					attrib.vertices[3 * vertexIndex0 + 2]
				};

				glm::vec3 point2 = {
					attrib.vertices[3 * vertexIndex1 + 0],
					attrib.vertices[3 * vertexIndex1 + 1],
					attrib.vertices[3 * vertexIndex1 + 2]
				};

				glm::vec3 point3 = {
					attrib.vertices[3 * vertexIndex2 + 0],
					attrib.vertices[3 * vertexIndex2 + 1],
					attrib.vertices[3 * vertexIndex2 + 2]
				};

				primitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{point1, point2, point3}, 0 }));
			}
		}

		return primitives;
	} 
} // namespace nugiEngine

