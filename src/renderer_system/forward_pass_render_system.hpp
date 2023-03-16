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
#include "../model/material.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineForwardPassRenderSystem {
		public:
			EngineForwardPassRenderSystem(EngineDevice& device, VkRenderPass renderPass, std::shared_ptr<EngineDescriptorPool> descriptorPool, 
				VkDescriptorSetLayout globalDescSetLayout, std::vector<VkDescriptorBufferInfo> modelBuffersInfo);
			~EngineForwardPassRenderSystem();

			EngineForwardPassRenderSystem(const EngineForwardPassRenderSystem&) = delete;
			EngineForwardPassRenderSystem& operator = (const EngineForwardPassRenderSystem&) = delete;

      void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
				VkDescriptorSet &globalDescSet, std::vector<std::shared_ptr<EngineGameObject>> &gameObjects);

		private:
			void createPipelineLayout(VkDescriptorSetLayout globalDescSetLayout);
			void createPipeline(VkRenderPass renderPass);
			void createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorBufferInfo> buffersInfo);

			EngineDevice& appDevice;

      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
      std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;
	};
}