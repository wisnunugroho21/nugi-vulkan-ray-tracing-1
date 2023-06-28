#pragma once

#include "../../vulkan/command/command_buffer.hpp"
#include "../../vulkan/device/device.hpp"
#include "../../vulkan/pipeline/compute_pipeline.hpp"
#include "../../vulkan/buffer/buffer.hpp"
#include "../../vulkan/descriptor/descriptor.hpp"
#include "../../vulkan/swap_chain/swap_chain.hpp"
#include "../utils/camera/camera.hpp"
#include "../ray_ubo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineTraceRayRenderSystem {
		public:
			EngineTraceRayRenderSystem(EngineDevice& device, VkDescriptorSetLayout descriptorSetLayouts, uint32_t width, uint32_t height, uint32_t nSample);
			~EngineTraceRayRenderSystem();

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, VkDescriptorSet descriptorSets, uint32_t randomSeed = 1);

		private:
			void createPipelineLayout(VkDescriptorSetLayout descriptorSetLayouts);
			void createPipeline();

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineComputePipeline> pipeline;

			uint32_t width, height, nSample;
	};
}