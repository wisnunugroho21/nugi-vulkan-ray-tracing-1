#pragma once

#include "../device/device.hpp"
#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	struct Vertex {
		alignas(16) glm::vec3 position{};
		alignas(16) glm::vec3 normal{};
		alignas(4) uint32_t materialIndex = 0;
		alignas(4) uint32_t transformIndex = 0;

		static std::vector<VkVertexInputBindingDescription> getVertexBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions();

		bool operator == (const Vertex &other) const {
			return this->position == other.position && this->normal == other.normal && this->materialIndex == other.materialIndex 
				&& this->transformIndex == other.transformIndex;
		}
	};

	struct RasterModelData {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		void loadModel(const std::string &filePath, uint32_t materialIndex);
	};

	class EngineRasterModel
	{
	public:
		EngineRasterModel(EngineDevice &device, const RasterModelData &data);
		~EngineRasterModel();

		EngineRasterModel(const EngineRasterModel&) = delete;
		EngineRasterModel& operator = (const EngineRasterModel&) = delete;

		static std::unique_ptr<EngineRasterModel> createModelFromFile(EngineDevice &device, const std::string &filePath, uint32_t materialIndex);

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
