#include "ray_trace_model.hpp"
#include "../utils/utils.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "bvh.hpp"

namespace nugiEngine {
	EngineRayTraceModel::EngineRayTraceModel(EngineDevice &device, RayTraceModelData &datas) : engineDevice{device} {
		auto bvhData = this->createBvhData(datas);
		auto triangleData = this->createTriangleData(datas);

		this->createBuffers(triangleData, bvhData);
	}

	EngineRayTraceModel::~EngineRayTraceModel() {}

	TriangleData EngineRayTraceModel::createTriangleData(const RayTraceModelData &data) {
		TriangleData object;
		for (int i = 0; i < data.triangles.size(); i++) {
			object.triangles[i] = data.triangles[i];
		}

		return object;
	}

	BvhData EngineRayTraceModel::createBvhData(const RayTraceModelData &data) {
		std::vector<TriangleBoundBox> objects;
		for (int i = 0; i < data.triangles.size(); i++) {
			Triangle t = data.triangles[i];
			objects.push_back({i, t});
		}

		auto bvhNodes = createBvh(objects);
		BvhData bvh{};

		for (int i = 0; i < bvhNodes.size(); i++) {
			bvh.bvhNodes[i] = bvhNodes[i];
		}

		return bvh;
	}

	void EngineRayTraceModel::createBuffers(TriangleData &data, BvhData &bvh) {
		EngineBuffer objectStagingBuffer {
			this->engineDevice,
			sizeof(TriangleData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		objectStagingBuffer.map();
		objectStagingBuffer.writeToBuffer(&data);

		this->objectBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(TriangleData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->objectBuffer->copyBuffer(objectStagingBuffer.getBuffer(), sizeof(TriangleData));

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

	std::unique_ptr<EngineRayTraceModel> EngineRayTraceModel::createModelFromFile(EngineDevice &device, const std::string &filePath) {
		RayTraceModelData modelData;
		modelData.loadModel(filePath);

		return std::make_unique<EngineRayTraceModel>(device, modelData);
	}

	void RayTraceModelData::loadModel(const std::string &filePath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::vector<glm::vec3> vertices{};

		uint32_t curIndex = 0;
		for (const auto &shape: shapes) {
			for (const auto &index: shape.mesh.indices) {
				if (index.vertex_index >= 0) {
					vertices.push_back({
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					});
				}
			}
		}

		for (int i = 0; i < vertices.size(); i++) {
			Triangle triangle{
				vertices[i * 3],
				vertices[i * 3 + 1],
				vertices[i * 3 + 2]
			};

			this->triangles.emplace_back(triangle);
		}
	}
    
} // namespace nugiEngine

