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
			EngineSamplingRayRasterRenderSystem(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::shared_ptr<EngineDescriptorSetLayout> traceRayDescLayout,
				uint32_t width, uint32_t height, uint32_t swapChainImageCount, VkRenderPass renderPass);
			~EngineSamplingRayRasterRenderSystem();

			EngineSamplingRayRasterRenderSystem(const EngineSamplingRayRasterRenderSystem&) = delete;
			EngineSamplingRayRasterRenderSystem& operator = (const EngineSamplingRayRasterRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex, std::shared_ptr<VkDescriptorSet> traceRayDescSet, std::shared_ptr<EngineModel> model, uint32_t randomSeed = 1);
		
		private:
			void createPipelineLayout(std::shared_ptr<EngineDescriptorSetLayout> traceRayDescLayout);
			void createPipeline(VkRenderPass renderPass);

			void createAccumulateImages(uint32_t width, uint32_t height, uint32_t swapChainImageCount);
			void createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, uint32_t swapChainImageCount);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;

			std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;

			std::vector<std::shared_ptr<EngineImage>> accumulateImages;
	};
}