#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/graphic_pipeline.hpp"
#include "../game_object/game_object.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../ubo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineDeffereRenderSystem {
		public:
			EngineDeffereRenderSystem(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
				uint32_t width, uint32_t height, VkRenderPass renderPass, 
				VkDescriptorSetLayout globalDescSetLayout,
				std::vector<VkDescriptorImageInfo> forwardPassResourcesInfo[3]);
			~EngineDeffereRenderSystem();

			EngineDeffereRenderSystem(const EngineDeffereRenderSystem&) = delete;
			EngineDeffereRenderSystem& operator = (const EngineDeffereRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, VkDescriptorSet& globalDescSet, std::vector<std::shared_ptr<EngineGameObject>> &gameObjects);
		
		private:
			void createPipelineLayout(VkDescriptorSetLayout globalDescSetLayout);
			void createPipeline(VkRenderPass renderPass);

			void createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, 
				std::vector<VkDescriptorImageInfo> forwardPassResourcesInfo[3]);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;

			std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;
	};
}