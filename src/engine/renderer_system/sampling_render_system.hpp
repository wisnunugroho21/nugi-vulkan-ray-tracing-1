#pragma once

#include "../../vulkan/command/command_buffer.hpp"
#include "../../vulkan/device/device.hpp"
#include "../../vulkan/pipeline/graphic_pipeline.hpp"
#include "../../vulkan/buffer/buffer.hpp"
#include "../../vulkan/descriptor/descriptor.hpp"
#include "../../vulkan/swap_chain/swap_chain.hpp"
#include "../../vulkan/renderpass/renderpass.hpp"
#include "../data/model/vertex_model.hpp"
#include "../utils/camera/camera.hpp"
#include "../general_struct.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineSamplingRenderSystem {
		public:
			EngineSamplingRenderSystem(EngineDevice& device, EngineRenderPass renderPass, VkDescriptorSetLayout descriptorSetLayouts);
			~EngineSamplingRenderSystem();

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, VkDescriptorSet descriptorSets, std::shared_ptr<EngineVertexModel> model, uint32_t randomSeed = 1);
		
		private:
			void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayouts);
			void createPipeline(EngineRenderPass renderPass);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;
	};
}