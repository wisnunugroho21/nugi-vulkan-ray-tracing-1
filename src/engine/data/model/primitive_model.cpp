#include "primitive_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace nugiEngine {
	EnginePrimitiveModel::EnginePrimitiveModel(EngineDevice &device) : engineDevice{device} {
		this->primitives = std::make_shared<std::vector<Primitive>>();
		this->bvhNodes = std::make_shared<std::vector<BvhNode>>();
	}

	void EnginePrimitiveModel::addPrimitive(std::shared_ptr<std::vector<Primitive>> curPrimitives, std::shared_ptr<std::vector<Vertex>> vertices) {
		auto curBvhNodes = this->createBvhData(curPrimitives, vertices);

		for (int i = 0; i < curBvhNodes->size(); i++) {
			this->bvhNodes->emplace_back((*curBvhNodes)[i]);
		}

		for (int i = 0; i < curPrimitives->size(); i++) {
			this->primitives->emplace_back((*curPrimitives)[i]);
		}
	}

	std::shared_ptr<std::vector<BvhNode>> EnginePrimitiveModel::createBvhData(std::shared_ptr<std::vector<Primitive>> primitives, std::shared_ptr<std::vector<Vertex>> vertices) {
		std::vector<std::shared_ptr<BoundBox>> boundBoxes;
		for (uint32_t i = 0; i < primitives->size(); i++) {
			boundBoxes.push_back(std::make_shared<PrimitiveBoundBox>(PrimitiveBoundBox{ i + 1, (*primitives)[i], vertices }));
		}

		return createBvh(boundBoxes);
	}

	void EnginePrimitiveModel::createBuffers(std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		auto primitiveBufferSize = sizeof(Primitive) * this->primitives->size();

		EngineBuffer primitiveStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(primitiveBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		};

		primitiveStagingBuffer.map();
		primitiveStagingBuffer.writeToBuffer(this->primitives->data());

		this->primitiveBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(primitiveBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);

		this->primitiveBuffer->copyBuffer(primitiveStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(primitiveBufferSize), commandBuffer);

		// -------------------------------------------------

		auto bvhBufferSize = sizeof(BvhNode) * this->bvhNodes->size();

		EngineBuffer bvhStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(bvhBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		};

		bvhStagingBuffer.map();
		bvhStagingBuffer.writeToBuffer(this->bvhNodes->data());

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

	/* std::shared_ptr<std::vector<Primitive>> EnginePrimitiveModel::createPrimitivesFromFile(EngineDevice &device, const std::string &filePath, uint32_t materialIndex) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		auto primitives = std::make_shared<std::vector<Primitive>>();

		for (const auto &shape: shapes) {
			uint32_t numTriangle = static_cast<uint32_t>(shape.mesh.indices.size()) / 3;

			for (uint32_t i = 0; i < numTriangle; i++) {
				int vertexIndex0 = shape.mesh.indices[3 * i + 0].vertex_index;
				int vertexIndex1 = shape.mesh.indices[3 * i + 1].vertex_index;
				int vertexIndex2 = shape.mesh.indices[3 * i + 2].vertex_index;

				int textCoordIndex0 = shape.mesh.indices[3 * i + 0].texcoord_index;
				int textCoordIndex1 = shape.mesh.indices[3 * i + 1].texcoord_index;
				int textCoordIndex2 = shape.mesh.indices[3 * i + 2].texcoord_index;
				
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

				glm::vec3 texel0 = {
					attrib.texcoords[2 * textCoordIndex0 + 0],
					attrib.texcoords[2 * textCoordIndex1 + 0],
					attrib.texcoords[2 * textCoordIndex2 + 0]
				};

				glm::vec3 texel1 = {
					attrib.texcoords[2 * textCoordIndex0 + 1],
					attrib.texcoords[2 * textCoordIndex1 + 1],
					attrib.texcoords[2 * textCoordIndex2 + 1]
				};

				primitives->emplace_back(Primitive{ Triangle{point1, point2, point3}, TextureCoordinate{texel0, texel1}, materialIndex });
			}
		}

		return primitives;
	} */ 
} // namespace nugiEngine

