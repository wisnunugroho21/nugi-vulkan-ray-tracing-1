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
#include "../data/material.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineForwardPassRenderSystem {
		public:
			EngineForwardPassRenderSystem(EngineDevice& device, VkRenderPass renderPass, std::vector<VkDescriptorSetLayout> descSetLayout);
			~EngineForwardPassRenderSystem();

			EngineForwardPassRenderSystem(const EngineForwardPassRenderSystem&) = delete;
			EngineForwardPassRenderSystem& operator = (const EngineForwardPassRenderSystem&) = delete;

      void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, std::vector<VkDescriptorSet> descSets, 
				std::shared_ptr<EngineGeometry> gameObjects);

		private:
			void createPipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayout);
			void createPipeline(VkRenderPass renderPass);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;
	};
}