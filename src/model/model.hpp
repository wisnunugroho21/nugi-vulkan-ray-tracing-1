#pragma once

#include "../device/device.hpp"
#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine
{
	struct Vertex {
		glm::vec3 position{};

		static std::vector<VkVertexInputBindingDescription> getVertexBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions();

		bool operator == (const Vertex &other) const {
			return this->position == other.position;
		}
	};

	struct ModelData {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		void loadModel(const std::string &filePath);
	};

	class EngineModel {
	public:
		EngineModel(EngineDevice &device, const ModelData &data);
		~EngineModel();

		EngineModel(const EngineModel&) = delete;
		EngineModel& operator = (const EngineModel&) = delete;

		std::shared_ptr<EngineBuffer> getVertexBuffer() const { return this->vertexBuffer; }
		std::shared_ptr<EngineBuffer> getIndexBuffer() const { return this->indexBuffer; }

		uint32_t getVertexCount() const { return this->vertextCount; }
		uint32_t getIndexCount() const { return this->indexCount; }

		void bind(std::shared_ptr<EngineCommandBuffer> commandBuffer);
		void draw(std::shared_ptr<EngineCommandBuffer> commandBuffer);

		static std::unique_ptr<EngineModel> createModelFromFile(EngineDevice &device, const std::string &filePath);
		
	private:
		EngineDevice &engineDevice;
		
		std::shared_ptr<EngineBuffer> vertexBuffer;
		std::shared_ptr<EngineBuffer> indexBuffer;

		uint32_t vertextCount;
		uint32_t indexCount;

		bool hasIndexBuffer = false;

		void createVertexBuffers(const std::vector<Vertex> &vertices);
		void createIndexBuffer(const std::vector<uint32_t> &indices);
	};
} // namespace nugiEngine
