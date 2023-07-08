#include "vertex_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineVertexModel::EngineVertexModel(EngineDevice &device, std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<uint32_t>> indices, std::shared_ptr<EngineCommandBuffer> commandBuffer) : engineDevice{device} {
		this->createVertexBuffers(vertices, commandBuffer);
		this->createIndexBuffer(indices, commandBuffer);
	}

	void EngineVertexModel::createVertexBuffers(std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		this->vertextCount = static_cast<uint32_t>(vertices->size());
		assert(vertextCount >= 3 && "Vertex count must be at least 3");

		uint32_t vertexSize = static_cast<uint32_t>(sizeof(Vertex));
		VkDeviceSize bufferSize = vertexSize * vertextCount;

		EngineBuffer stagingBuffer {
			this->engineDevice,
			vertexSize,
			this->vertextCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void *) vertices->data());

		this->vertexBuffer = std::make_unique<EngineBuffer>(
			this->engineDevice,
			vertexSize,
			this->vertextCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);

		this->vertexBuffer->copyBuffer(stagingBuffer.getBuffer(), bufferSize, commandBuffer);
	}

	void EngineVertexModel::createIndexBuffer(std::shared_ptr<std::vector<uint32_t>> indices, std::shared_ptr<EngineCommandBuffer> commandBuffer) { 
		this->indexCount = static_cast<uint32_t>(indices->size());
		this->hasIndexBuffer = this->indexCount > 0;

		if (!this->hasIndexBuffer) {
			return;
		}

		uint32_t indexSize = static_cast<uint32_t>(sizeof(uint32_t));
		VkDeviceSize bufferSize = indexSize * this->indexCount;

		EngineBuffer stagingBuffer {
			this->engineDevice,
			indexSize,
			this->indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer((void *) indices->data());

		this->indexBuffer = std::make_unique<EngineBuffer>(
			this->engineDevice,
			indexSize,
			this->indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VMA_MEMORY_USAGE_AUTO,
			VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT
		);

		this->indexBuffer->copyBuffer(stagingBuffer.getBuffer(), bufferSize, commandBuffer);
	}

	void EngineVertexModel::bind(std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		VkBuffer buffers[] = {this->vertexBuffer->getBuffer()};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer->getCommandBuffer(), 0, 1, buffers, offsets);

		if (this->hasIndexBuffer) {
			vkCmdBindIndexBuffer(commandBuffer->getCommandBuffer(), this->indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void EngineVertexModel::draw(std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		if (this->hasIndexBuffer) {
			vkCmdDrawIndexed(commandBuffer->getCommandBuffer(), this->indexCount, 1, 0, 0, 0);
		} else {
			vkCmdDraw(commandBuffer->getCommandBuffer(), this->vertextCount, 1, 0, 0);
		}
	}
} // namespace nugiEngine

