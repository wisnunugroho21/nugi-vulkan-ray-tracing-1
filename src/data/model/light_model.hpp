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
  struct LightData {
    Light lights[100];
  };

	class EngineLightModel {
	public:
		EngineLightModel(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights);
		~EngineLightModel();

    VkDescriptorBufferInfo getLightInfo() { return this->lightBuffer->descriptorInfo(); }
    VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
		
	private:
		EngineDevice &engineDevice;
		
    std::shared_ptr<EngineBuffer> lightBuffer;
    std::shared_ptr<EngineBuffer> bvhBuffer;
    
    LightData createLightData(std::vector<std::shared_ptr<Light>> lights);
    BvhData createBvhData(std::vector<std::shared_ptr<Light>> lights);

    void createBuffers(LightData &data, BvhData &bvh);
	};
} // namespace nugiEngine
