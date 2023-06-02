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
  struct ObjectData {
    Object objects[200];
  };

	class EngineObjectModel {
	public:
		EngineObjectModel(EngineDevice &device) : engineDevice{device} {}

    VkDescriptorBufferInfo getObjectInfo() { return this->objectBuffer->descriptorInfo();  }
    VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }

    void addObject(std::shared_ptr<Object> object, std::vector<std::shared_ptr<Primitive>> primitives);
    void createBuffers();
		
	private:
		EngineDevice &engineDevice;

    std::vector<std::shared_ptr<Object>> objects{};
    std::vector<std::shared_ptr<BoundBox>> boundBoxes{};
		
    std::shared_ptr<EngineBuffer> objectBuffer;
    std::shared_ptr<EngineBuffer> bvhBuffer;
	};
} // namespace nugiEngine
