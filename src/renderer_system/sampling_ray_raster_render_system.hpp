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
			EngineSamplingRayRasterRenderSystem(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool,
				uint32_t width, uint32_t height, std::vector<std::shared_ptr<EngineImage>> computeStoreImages, uint32_t nSample,
				VkRenderPass renderPass);
			~EngineSamplingRayRasterRenderSystem();

			EngineSamplingRayRasterRenderSystem(const EngineSamplingRayRasterRenderSystem&) = delete;
			EngineSamplingRayRasterRenderSystem& operator = (const EngineSamplingRayRasterRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, std::shared_ptr<EngineRasterModel> model, uint32_t randomSeed = 1);
		
		private:
			void createPipelineLayout();
			void createPipeline(VkRenderPass renderPass);

			void createAccumulateImages(uint32_t width, uint32_t height);
			void createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<std::shared_ptr<EngineImage>> computeStoreImages, uint32_t nSample);

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineGraphicPipeline> pipeline;

			std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;

			std::vector<std::shared_ptr<EngineImage>> accumulateImages;
	};
}