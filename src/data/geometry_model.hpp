#pragma once

#include "../device/device.hpp"
#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"
#include "../ray_ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
  struct ObjectData {
    Object objects[100];
  };

  struct BvhData {
    BvhNode bvhNodes[100];
	};

	class EngineGeometryModel {
	public:
		EngineGeometryModel(EngineDevice &device, std::vector<std::shared_ptr<Object>> objects);
		~EngineGeometryModel();

    VkDescriptorBufferInfo getObjectInfo() { return this->objectBuffer->descriptorInfo();  }
    VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
		
	private:
		EngineDevice &engineDevice;
		
    std::shared_ptr<EngineBuffer> objectBuffer;
    std::shared_ptr<EngineBuffer> bvhBuffer;
    
    ObjectData createObjectData(std::vector<std::shared_ptr<Object>> objects);
    BvhData createBvhData(std::vector<std::shared_ptr<Object>> objects);

    void createBuffers(ObjectData &data, BvhData &bvh);
	};
} // namespace nugiEngine
