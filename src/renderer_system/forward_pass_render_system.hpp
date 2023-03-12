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
	class EngineForwardPassRenderSystem {
		public:
			EngineForwardPassRenderSystem(EngineDevice& device, VkRenderPass renderPass, std::shared_ptr<EngineDescriptorPool> descriptorPool);
			~EngineForwardPassRenderSystem();

			EngineForwardPassRenderSystem(const EngineForwardPassRenderSystem&) = delete;
			EngineForwardPassRenderSystem& operator = (const EngineForwardPassRenderSystem&) = delete;

			void writeUniformBuffer(int frameIndex, GlobalUBO* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
      void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, std::vector<std::shared_ptr<EngineGameObject>> &gameObjects);

		private:
			void createPipelineLayout();
			void createPipeline(VkRenderPass renderPass);

      void createUniformBuffer();
      void createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool);

			EngineDevice& appDevice;
      std::vector<std::shared_ptr<EngineBuffer>> uniformBuffers;

      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
      std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;
	};
}