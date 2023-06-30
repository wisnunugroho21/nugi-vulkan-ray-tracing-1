#pragma once

#include "../../../vulkan/command/command_buffer.hpp"
#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/pipeline/compute_pipeline.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/descriptor/descriptor.hpp"
#include "../../general_struct.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineRasterUniform {
		public:
			EngineRasterUniform(EngineDevice& device);

			std::vector<VkDescriptorBufferInfo> getBuffersInfo() const;

			void writeGlobalData(uint32_t frameIndex, RasterUbo ubo);

		private:
      EngineDevice& appDevice;
			std::vector<std::shared_ptr<EngineBuffer>> uniformBuffers;

			void createUniformBuffer();
	};
}