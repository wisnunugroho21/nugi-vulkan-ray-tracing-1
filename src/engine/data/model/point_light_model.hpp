#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"
#include "../../utils/bvh/bvh.hpp"
#include "../../ray_ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	class EnginePointLightModel {
    public:
      EnginePointLightModel(EngineDevice &device, std::shared_ptr<std::vector<PointLight>> lights);

      VkDescriptorBufferInfo getLightInfo() { return this->lightBuffer->descriptorInfo(); }
      VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
      
    private:
      EngineDevice &engineDevice;
      
      std::shared_ptr<EngineBuffer> lightBuffer;
      std::shared_ptr<EngineBuffer> bvhBuffer;

      void createBuffers(std::shared_ptr<std::vector<PointLight>> lights, std::shared_ptr<std::vector<BvhNode>> bvhNodes);
	};
} // namespace nugiEngine
