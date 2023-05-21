#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/graphic_pipeline.hpp"
#include "../game_object/game_object.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../globalUbo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineSamplingRayRasterRenderSystem {
		public:
			EngineSamplingRayRasterRenderSystem(EngineDevice& device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts, VkRenderPass renderPass);
			~EngineSamplingRayRasterRenderSystem();

			EngineSamplingRayRasterRenderSystem(const EngineSamplingRayRasterRenderSystem&) = delete;
			EngineSamplingRayRasterRenderSystem& operator = (const EngineSamplingRayRasterRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, std::vector<VkDescriptorSet> descriptorSets, std::shared_ptr<EngineModel> model, uint32_t randomSeed = 1);
		
		private:
			void createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
			void createPipeline(VkRenderPass renderPass);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;
	};
}