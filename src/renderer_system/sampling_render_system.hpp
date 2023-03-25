#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/graphic_pipeline.hpp"
#include "../data/geometry.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../ubo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineSamplingRenderSystem {
		public:
			EngineSamplingRenderSystem(EngineDevice& device, VkRenderPass renderPass, 
				std::vector<VkDescriptorSetLayout> descSetLayouts);
			~EngineSamplingRenderSystem();

			EngineSamplingRenderSystem(const EngineSamplingRenderSystem&) = delete;
			EngineSamplingRenderSystem& operator = (const EngineSamplingRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
				std::vector<VkDescriptorSet> descSets, std::shared_ptr<EngineGeometry> gameObject, 
				uint32_t randomSeed);
		
		private:
			void createPipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayouts);
			void createPipeline(VkRenderPass renderPass);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;

			std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;
	};
}