#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../command/command_buffer.hpp"
#include "../../utils/utils.hpp"
#include "../../ray_ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	class EngineRayTraceVertexModel {
		public:
			EngineRayTraceVertexModel(EngineDevice &device, std::shared_ptr<std::vector<RayTraceVertex>> vertices);

			EngineRayTraceVertexModel(const EngineRayTraceVertexModel&) = delete;
			EngineRayTraceVertexModel& operator = (const EngineRayTraceVertexModel&) = delete;

			VkDescriptorBufferInfo getVertexnfo() { return this->vertexBuffer->descriptorInfo(); }
			
		private:
			EngineDevice &engineDevice;
			
			std::unique_ptr<EngineBuffer> vertexBuffer;
			void createVertexBuffers(std::shared_ptr<std::vector<RayTraceVertex>> vertices);
	};
} // namespace nugiEngine
