#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/graphic_pipeline.hpp"
#include "../data/light.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../ubo.hpp"
#include "../data/material.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineForwardLightRenderSystem {
		public:
			EngineForwardLightRenderSystem(EngineDevice& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descSetLayouts);
			~EngineForwardLightRenderSystem();

			EngineForwardLightRenderSystem(const EngineForwardLightRenderSystem&) = delete;
			EngineForwardLightRenderSystem& operator = (const EngineForwardLightRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, std::vector<VkDescriptorSet> descSets, 
				uint32_t numLight);

		private:
			void createPipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayouts);
			void createPipeline(VkRenderPass renderPass);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;
	};
}