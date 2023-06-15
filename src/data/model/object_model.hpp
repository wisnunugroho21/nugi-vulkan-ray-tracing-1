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
    Object objects[100];
  };

	class EngineObjectModel {
	public:
		EngineObjectModel(EngineDevice &device, std::vector<std::shared_ptr<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes) : engineDevice{device} {}

    VkDescriptorBufferInfo getObjectInfo() { return this->objectBuffer->descriptorInfo();  }
    VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
    
	private:
		EngineDevice &engineDevice;
		
    std::shared_ptr<EngineBuffer> objectBuffer;
    std::shared_ptr<EngineBuffer> bvhBuffer;

    void createBuffers(std::vector<std::shared_ptr<Object>> objects, std::vector<std::shared_ptr<BoundBox>> boundBoxes);
	};
} // namespace nugiEngine
