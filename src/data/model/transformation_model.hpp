#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../command/command_buffer.hpp"
#include "../../ray_ubo.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	class EngineTransformationModel {
		public:
			EngineTransformationModel(EngineDevice &device, std::shared_ptr<std::vector<Transformation>> transformations);
			EngineTransformationModel(EngineDevice &device, std::shared_ptr<std::vector<TransformComponent>> transformationComponents);

			EngineTransformationModel(const EngineTransformationModel&) = delete;
			EngineTransformationModel& operator = (const EngineTransformationModel&) = delete;

			VkDescriptorBufferInfo getTransformationInfo() { return this->transformationBuffer->descriptorInfo();  }
			
		private:
			EngineDevice &engineDevice;
			std::shared_ptr<EngineBuffer> transformationBuffer;

			std::shared_ptr<std::vector<Transformation>> convertToMatrix(std::shared_ptr<std::vector<TransformComponent>> transformations);
			void createBuffers(std::shared_ptr<std::vector<Transformation>> transformations);
	};
} // namespace nugiEngine
