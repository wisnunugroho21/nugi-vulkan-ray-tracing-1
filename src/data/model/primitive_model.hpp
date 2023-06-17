#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../command/command_buffer.hpp"
#include "../../ray_ubo.hpp"
#include "../../utils/bvh.hpp"
#include "../../utils/utils.hpp"

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

      int getPrimitiveSize() const { return this->primitives->size(); }
      int getBvhSize() const { return this->bvhNodes->size(); }

      void addPrimitive(std::shared_ptr<std::vector<Primitive>> primitives);
      void createBuffers();

      static std::shared_ptr<std::vector<Primitive>> createPrimitivesFromFile(EngineDevice &device, const std::string &filePath);
      
    private:
      EngineDevice &engineDevice;

      std::shared_ptr<std::vector<Primitive>> primitives{};
      std::shared_ptr<std::vector<BvhNode>> bvhNodes{};
      
      std::shared_ptr<EngineBuffer> primitiveBuffer;
      std::shared_ptr<EngineBuffer> bvhBuffer;
      
      std::shared_ptr<std::vector<BvhNode>> createBvhData(std::shared_ptr<std::vector<Primitive>> primitives);
	};
} // namespace nugiEngine
