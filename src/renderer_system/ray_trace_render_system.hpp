#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/compute_pipeline.hpp"
#include "../data/geometry.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../frame_info.hpp"
#include "../ubo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineRayTraceRenderSystem {
		public:
			EngineRayTraceRenderSystem(EngineDevice& device, uint32_t width, uint32_t height, 
				std::vector<VkDescriptorSetLayout> descSetLayouts);
			~EngineRayTraceRenderSystem();

			EngineRayTraceRenderSystem(const EngineRayTraceRenderSystem&) = delete;
			EngineRayTraceRenderSystem& operator = (const EngineRayTraceRenderSystem&) = delete;

			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, std::vector<VkDescriptorSet> descSets, 
				uint32_t randomSeed = 1);

		private:
			void createPipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayouts);
			void createPipeline();

			EngineDevice& appDevice;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineComputePipeline> pipeline;

			uint32_t width, height;
	};
}