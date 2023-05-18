#pragma once

#include "../device/device.hpp"
#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"
#include "../ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	struct RayTraceModelData {
    std::vector<std::shared_ptr<Object>> objects;
	};

  struct ObjectData {
    Object objects[100];
  };

  struct BvhData {
    BvhNode bvhNodes[100];
	};

	class EngineRayTraceModel {
	public:
		EngineRayTraceModel(EngineDevice &device, RayTraceModelData &data);
		~EngineRayTraceModel();

		EngineRayTraceModel(const EngineRayTraceModel&) = delete;
		EngineRayTraceModel& operator = (const EngineRayTraceModel&) = delete;

    VkDescriptorBufferInfo getModelInfo() { return this->objectBuffer->descriptorInfo();  }
    VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
		
	private:
		EngineDevice &engineDevice;

    std::shared_ptr<EngineBuffer> objectBuffer;
    std::shared_ptr<EngineBuffer> bvhBuffer;

	  ObjectData createObjectData(const RayTraceModelData &data);
    BvhData createBvhData(const RayTraceModelData &data);

    void createBuffers(ObjectData &data, BvhData &bvh);
	};
} // namespace nugiEngine
