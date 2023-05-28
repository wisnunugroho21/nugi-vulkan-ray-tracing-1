#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/compute_pipeline.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../frame_info.hpp"
#include "../ray_ubo.hpp"

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