#include "geometry_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

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

	std::vector<std::shared_ptr<Object>> EngineGeometryModel::createGeometryObjectsFromFile(EngineDevice &device, const std::string &filePath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::vector<std::shared_ptr<Object>> objects{};

		for (const auto &shape: shapes) {
			uint32_t numTriangle = shape.mesh.indices.size() / 3;

			for (uint32_t i = 0; i < numTriangle; i++) {
				int vertexIndex0 = shape.mesh.indices[3 * i + 0].vertex_index;
				int vertexIndex1 = shape.mesh.indices[3 * i + 1].vertex_index;
				int vertexIndex2 = shape.mesh.indices[3 * i + 2].vertex_index;
				
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

				objects.emplace_back(std::make_shared<Object>(Object{ Triangle{point1, point2, point3}, 0 }));
			}
		}

		return objects;
	} 
} // namespace nugiEngine

