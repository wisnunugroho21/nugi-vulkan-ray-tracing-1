#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> getVertexBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions();

		bool operator == (const Vertex &other) const {
			return this->position == other.position && this->color == other.color && this->normal == other.normal 
				&& this->uv == other.uv;
		}
	};

	struct VertexModelData
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
	};

	class EngineVertexModel {
	public:
		EngineVertexModel(EngineDevice &device, const VertexModelData &data);

		EngineVertexModel(const EngineVertexModel&) = delete;
		EngineVertexModel& operator = (const EngineVertexModel&) = delete;

		void bind(std::shared_ptr<EngineCommandBuffer> commandBuffer);
		void draw(std::shared_ptr<EngineCommandBuffer> commandBuffer);
		
	private:
		EngineDevice &engineDevice;
		
		std::unique_ptr<EngineBuffer> vertexBuffer;
		uint32_t vertextCount;

		std::unique_ptr<EngineBuffer> indexBuffer;
		uint32_t indexCount;

		bool hasIndexBuffer = false;

		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffer(const std::vector<uint32_t> &indices);
	};
} // namespace nugiEngine
