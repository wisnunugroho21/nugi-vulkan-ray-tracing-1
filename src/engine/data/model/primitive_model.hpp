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
	class EnginePrimitiveModel {
    public:
      EnginePrimitiveModel(EngineDevice &device);

      VkDescriptorBufferInfo getPrimitiveInfo() { return this->primitiveBuffer->descriptorInfo();  }
      VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }

      uint32_t getPrimitiveSize() const { return static_cast<uint32_t>(this->primitives->size()); }
      uint32_t getBvhSize() const { return static_cast<uint32_t>(this->bvhNodes->size()); }

      void addPrimitive(std::shared_ptr<std::vector<Primitive>> primitives, std::shared_ptr<std::vector<Vertex>> vertices);
      void createBuffers(std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);

      // static std::shared_ptr<std::vector<Primitive>> createPrimitivesFromFile(EngineDevice &device, const std::string &filePath, uint32_t materialIndex);
      
    private:
      EngineDevice &engineDevice;

      std::shared_ptr<std::vector<Primitive>> primitives{};
      std::shared_ptr<std::vector<BvhNode>> bvhNodes{};
      
      std::shared_ptr<EngineBuffer> primitiveBuffer;
      std::shared_ptr<EngineBuffer> bvhBuffer;
      
      std::shared_ptr<std::vector<BvhNode>> createBvhData(std::shared_ptr<std::vector<Primitive>> primitives, std::shared_ptr<std::vector<Vertex>> vertices);
	};
} // namespace nugiEngine
