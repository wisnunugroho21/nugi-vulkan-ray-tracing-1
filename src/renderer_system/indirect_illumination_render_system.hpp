#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/compute_pipeline.hpp"
#include "../data/geometry.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../frame_info.hpp"
#include "../ubo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineIndirectIlluminationRenderSystem {
		public:
			EngineIndirectIlluminationRenderSystem(EngineDevice& device, uint32_t width, uint32_t height, 
				std::vector<VkDescriptorSetLayout> descSetLayouts);
			~EngineIndirectIlluminationRenderSystem();

			EngineIndirectIlluminationRenderSystem(const EngineIndirectIlluminationRenderSystem&) = delete;
			EngineIndirectIlluminationRenderSystem& operator = (const EngineIndirectIlluminationRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
				std::vector<VkDescriptorSet> descSets, uint32_t randomSeed = 1);

		private:
			void createPipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayouts);
			void createPipeline();

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineComputePipeline> pipeline;

			uint32_t width, height;
	};
}