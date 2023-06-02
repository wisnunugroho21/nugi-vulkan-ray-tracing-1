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
  struct PrimitiveData {
    Primitive primitives[100];
  };

	class EnginePrimitiveModel {
	public:
		EnginePrimitiveModel(EngineDevice &device) : engineDevice{device} {}

    VkDescriptorBufferInfo getPrimitiveInfo() { return this->primitiveBuffer->descriptorInfo();  }
    VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }

    uint32_t getPrimitiveSize() const { return static_cast<uint32_t>(this->primitives.size()); }
    uint32_t getBvhSize() const { return static_cast<uint32_t>(this->bvhNodes.size()); }

    void addPrimitive(std::vector<std::shared_ptr<Primitive>> primitives);
    void createBuffers();

    static std::vector<std::shared_ptr<Primitive>> createPrimitivesFromFile(EngineDevice &device, const std::string &filePath);
		
	private:
		EngineDevice &engineDevice;

    std::vector<std::shared_ptr<Primitive>> primitives{};
    std::vector<std::shared_ptr<BvhNode>> bvhNodes{};
		
    std::shared_ptr<EngineBuffer> primitiveBuffer;
    std::shared_ptr<EngineBuffer> bvhBuffer;
    
    std::vector<std::shared_ptr<BvhNode>> createBvhData(std::vector<std::shared_ptr<Primitive>> primitives);
	};
} // namespace nugiEngine
