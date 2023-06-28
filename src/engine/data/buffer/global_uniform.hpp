#pragma once

#include "../../../vulkan/command/command_buffer.hpp"
#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/pipeline/compute_pipeline.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/descriptor/descriptor.hpp"
#include "../../ray_ubo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineGlobalUniform {
		public:
			EngineGlobalUniform(EngineDevice& device);

			std::vector<VkDescriptorBufferInfo> getBuffersInfo() const;

			void writeGlobalData(uint32_t frameIndex, RayTraceUbo ubo);

		private:
      EngineDevice& appDevice;
			std::vector<std::shared_ptr<EngineBuffer>> uniformBuffers;

			void createUniformBuffer();
	};
}