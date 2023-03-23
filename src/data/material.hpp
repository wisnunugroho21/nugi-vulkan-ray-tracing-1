#pragma once

#include "../device/device.hpp"
#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
  struct MaterialItem {
    alignas(16) glm::vec3 color{};
    alignas(4) float metallicness;
    alignas(4) float roughness;
    alignas(4) float fresnelReflect;
  };

	struct MaterialData {
		MaterialItem data[100];
	};

	class EngineMaterial {
    public:
      EngineMaterial(EngineDevice &device, MaterialData &data);

      EngineMaterial(const EngineMaterial&) = delete;
      EngineMaterial& operator = (const EngineMaterial&) = delete;
      
      VkDescriptorBufferInfo getMaterialInfo() { return this->materialBuffer->descriptorInfo(); }
      
    private:
      EngineDevice &engineDevice;
      std::shared_ptr<EngineBuffer> materialBuffer;

      void createMaterialBuffers(MaterialData &data);
  };
} // namespace nugiEngine
