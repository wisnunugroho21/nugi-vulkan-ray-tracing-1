#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../command/command_buffer.hpp"
#include "../../ray_ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
  struct MaterialData {
    Material materials[100];
  };

	class EngineMaterialModel {
	public:
		EngineMaterialModel(EngineDevice &device, std::vector<std::shared_ptr<Material>> materials);
		~EngineMaterialModel();

		EngineMaterialModel(const EngineMaterialModel&) = delete;
		EngineMaterialModel& operator = (const EngineMaterialModel&) = delete;

    VkDescriptorBufferInfo getMaterialInfo() { return this->materialBuffer->descriptorInfo();  }
		
	private:
		EngineDevice &engineDevice;
		
    std::shared_ptr<EngineBuffer> materialBuffer;

    MaterialData createMaterialData(std::vector<std::shared_ptr<Material>> materials);

    void createBuffers(MaterialData &data);
	};
} // namespace nugiEngine
