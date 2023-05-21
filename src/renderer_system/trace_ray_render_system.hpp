#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/compute_pipeline.hpp"
#include "../game_object/game_object.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../frame_info.hpp"
#include "../ray_ubo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineTraceRayRenderSystem {
		public:
			EngineTraceRayRenderSystem(EngineDevice& device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts, uint32_t width, uint32_t height, uint32_t nSample);
			~EngineTraceRayRenderSystem();

			EngineTraceRayRenderSystem(const EngineTraceRayRenderSystem&) = delete;
			EngineTraceRayRenderSystem& operator = (const EngineTraceRayRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, std::vector<VkDescriptorSet> descriptorSets, uint32_t randomSeed = 1);

		private:
			void createPipelineLayout(std::vector<VkDescriptorSetLayout> descriptorSetLayouts);
			void createPipeline();

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineComputePipeline> pipeline;

			uint32_t width, height, nSample;
	};
}