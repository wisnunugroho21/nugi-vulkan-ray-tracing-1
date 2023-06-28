#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"
#include "../../ray_ubo.hpp"
#include "../../utils/transform/transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	class EngineTransformationModel {
		public:
			EngineTransformationModel(EngineDevice &device, std::shared_ptr<std::vector<Transformation>> transformations);
			EngineTransformationModel(EngineDevice &device, std::vector<std::shared_ptr<TransformComponent>> transformationComponents);

			EngineTransformationModel(const EngineTransformationModel&) = delete;
			EngineTransformationModel& operator = (const EngineTransformationModel&) = delete;

			VkDescriptorBufferInfo getTransformationInfo() { return this->transformationBuffer->descriptorInfo();  }
			
		private:
			EngineDevice &engineDevice;
			std::shared_ptr<EngineBuffer> transformationBuffer;

			std::shared_ptr<std::vector<Transformation>> convertToMatrix(std::vector<std::shared_ptr<TransformComponent>> transformations);
			void createBuffers(std::shared_ptr<std::vector<Transformation>> transformations);
	};
} // namespace nugiEngine
