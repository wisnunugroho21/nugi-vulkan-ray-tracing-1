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
  };

	struct MaterialData {
		MaterialItem data[100];
	};

	class EngineMaterial {
    public:
      EngineMaterial(EngineDevice &device, MaterialData &data);
      ~EngineMaterial();

      EngineMaterial(const EngineMaterial&) = delete;
      EngineMaterial& operator = (const EngineMaterial&) = delete;
      
      std::shared_ptr<EngineBuffer> getMaterialBuffer() { return this->materialBuffer; }
      VkDescriptorBufferInfo getMaterialInfo() { return this->materialBuffer->descriptorInfo(); }
      
    private:
      EngineDevice &engineDevice;
      std::shared_ptr<EngineBuffer> materialBuffer;

      void createMaterialBuffers(MaterialData &data);
  };
} // namespace nugiEngine
