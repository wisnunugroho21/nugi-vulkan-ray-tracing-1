#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/command/command_buffer.hpp"
#include "../../utils/bvh/bvh.hpp"
#include "../../general_struct.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	class EnginePointLightModel {
    public:
      EnginePointLightModel(EngineDevice &device, std::shared_ptr<std::vector<PointLight>> pointLights, 
        std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);

      VkDescriptorBufferInfo getPointLightInfo() { return this->pointLightBuffer->descriptorInfo(); }
      VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
      
    private:
      EngineDevice &engineDevice;
      
      std::shared_ptr<EngineBuffer> pointLightBuffer;
      std::shared_ptr<EngineBuffer> bvhBuffer;

      void createBuffers(std::shared_ptr<std::vector<PointLight>> pointLights, std::shared_ptr<std::vector<BvhNode>> bvhNodes, 
        std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);
	};
} // namespace nugiEngine
