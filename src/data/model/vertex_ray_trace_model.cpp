#include "vertex_ray_trace_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineRayTraceVertexModel::EngineRayTraceVertexModel(EngineDevice &device, std::shared_ptr<std::vector<RayTraceVertex>> vertices) : engineDevice{device} {
		this->createVertexBuffers(vertices);
	}

	void EngineRayTraceVertexModel::createVertexBuffers(std::shared_ptr<std::vector<RayTraceVertex>> vertices) {
		auto vertexBufferSize = sizeof(RayTraceVertex) * vertices->size();

		EngineBuffer stagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(vertexBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer(vertices->data());

		this->vertexBuffer = std::make_unique<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(vertexBufferSize),
			1,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->vertexBuffer->copyBuffer(stagingBuffer.getBuffer(), static_cast<VkDeviceSize>(vertexBufferSize));
	} 
} // namespace nugiEngine

