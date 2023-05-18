#include "light.hpp"

namespace nugiEngine {
  EngineLight::EngineLight(id_t id, EngineDevice &device, std::vector<std::shared_ptr<Light>> lights) : objectId{id} {
    this->createLightBuffers(device, lights);
  }

  EngineLight EngineLight::createLight(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights) {
    static id_t currentId = 0;
    return EngineLight(currentId++, device, lights);
  }

  std::shared_ptr<EngineLight> EngineLight::createSharedLight(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights) {
    static id_t currentId = 0;
    return std::make_shared<EngineLight>(currentId++, device, lights);
  }

  void EngineLight::createLightBuffers(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights) {
    GlobalLight globalLight{};
    
    int lightIndex = 0;
    for (auto &&light : lights) {
      globalLight.lights[lightIndex] = *light;
      lightIndex++;
    }

    this->numLight = lightIndex;

    EngineBuffer objectStagingBuffer {
			device,
			sizeof(GlobalLight),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		objectStagingBuffer.map();
		objectStagingBuffer.writeToBuffer(&globalLight);

		this->lightBuffer = std::make_shared<EngineBuffer>(
			device,
			sizeof(GlobalLight),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->lightBuffer->copyBuffer(objectStagingBuffer.getBuffer(), sizeof(GlobalLight));
  }
} // namespace nugiEngine
