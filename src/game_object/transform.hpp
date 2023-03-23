#pragma once

#include "../device/device.hpp"
#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	struct TransformComponent {
		glm::vec3 translation{0.0f};
		glm::vec3 scale{1.0f};
		glm::vec3 rotation{0.0f};

    glm::mat4 mat4();
    glm::mat3 normalMatrix();
	};

  struct TransformItem {
    glm::mat4 modelMatrix{};
    glm::mat4 normalMatrix{};
  };

	struct TransformData {
		TransformItem data[100];
	};

  class EngineTransform {
    public:
      EngineTransform(EngineDevice &device, TransformData &data);

      static EngineTransform createFromTransformComponent(EngineDevice &device, std::vector<TransformComponent> transformComponents);
      static std::shared_ptr<EngineTransform> createSharedFromTransformComponent(EngineDevice &device, std::vector<TransformComponent> transformComponents);

      EngineTransform(const EngineTransform&) = delete;
      EngineTransform& operator = (const EngineTransform&) = delete;
      
      VkDescriptorBufferInfo getTransformInfo() { return this->materialBuffer->descriptorInfo(); }
      
    private:
      EngineDevice &engineDevice;
      std::shared_ptr<EngineBuffer> materialBuffer;

      void createTransformBuffers(TransformData &data);
  };
}