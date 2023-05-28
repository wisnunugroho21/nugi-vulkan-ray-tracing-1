#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/graphic_pipeline.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../globalUbo.hpp"
#include "../data/model/vertex_model.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineSamplingRayRasterRenderSystem {
		public:
			EngineSamplingRayRasterRenderSystem(EngineDevice& device, VkDescriptorSetLayout descriptorSetLayouts, VkRenderPass renderPass);
			~EngineSamplingRayRasterRenderSystem();

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, VkDescriptorSet descriptorSets, std::shared_ptr<EngineVertexModel> model, uint32_t randomSeed = 1);
		
		private:
			void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayouts);
			void createPipeline(VkRenderPass renderPass);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;
	};
}