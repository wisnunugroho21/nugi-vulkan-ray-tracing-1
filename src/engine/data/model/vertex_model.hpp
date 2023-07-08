#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"
#include "../../general_struct.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	class EngineVertexModel {
		public:
			EngineVertexModel(EngineDevice &device, std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<std::vector<uint32_t>> indices, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);

			EngineVertexModel(const EngineVertexModel&) = delete;
			EngineVertexModel& operator = (const EngineVertexModel&) = delete;

			VkDescriptorBufferInfo getVertexInfo() { return this->vertexBuffer->descriptorInfo(); }
			VkDescriptorBufferInfo getIndexInfo() { return this->indexBuffer->descriptorInfo(); }

			void bind(std::shared_ptr<EngineCommandBuffer> commandBuffer);
			void draw(std::shared_ptr<EngineCommandBuffer> commandBuffer);
			
		private:
			EngineDevice &engineDevice;
			
			std::unique_ptr<EngineBuffer> vertexBuffer;
			uint32_t vertextCount;

			std::unique_ptr<EngineBuffer> indexBuffer;
			uint32_t indexCount;

			bool hasIndexBuffer = false;

			void createVertexBuffers(std::shared_ptr<std::vector<Vertex>> vertices, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);
			void createIndexBuffer(std::shared_ptr<std::vector<uint32_t>> indices, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);
	};
} // namespace nugiEngine
