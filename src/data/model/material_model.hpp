#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../command/command_buffer.hpp"
#include "../../ray_ubo.hpp"
#include "../../utils/utils.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	class EngineMaterialModel {
		public:
			EngineMaterialModel(EngineDevice &device, std::vector<std::shared_ptr<Material>> materials);

			EngineMaterialModel(const EngineMaterialModel&) = delete;
			EngineMaterialModel& operator = (const EngineMaterialModel&) = delete;

			VkDescriptorBufferInfo getMaterialInfo() { return this->materialBuffer->descriptorInfo();  }
			
		private:
			EngineDevice &engineDevice;
			std::shared_ptr<EngineBuffer> materialBuffer;

			void createBuffers(std::vector<std::shared_ptr<Material>> materials);
	};
} // namespace nugiEngine
