#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/graphic_pipeline.hpp"
#include "../game_object/light_object.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../globalUbo.hpp"
#include "../model/material.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineForwardLightRenderSystem {
		public:
			EngineForwardLightRenderSystem(EngineDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalDescSetLayout);
			~EngineForwardLightRenderSystem();

			EngineForwardLightRenderSystem(const EngineForwardLightRenderSystem&) = delete;
			EngineForwardLightRenderSystem& operator = (const EngineForwardLightRenderSystem&) = delete;

      void update(std::vector<std::shared_ptr<EngineLightObject>> &pointLightObjects, GlobalLight &globalLight);
			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
				VkDescriptorSet &globalDescSet, std::vector<std::shared_ptr<EngineLightObject>> &pointLights);

		private:
			void createPipelineLayout(VkDescriptorSetLayout globalDescSetLayout);
			void createPipeline(VkRenderPass renderPass);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;
	};
}