#include "light_object.hpp"

namespace nugiEngine {
  EngineLightObject::EngineLightObject(id_t id, EngineDevice &device, std::vector<PointLight> pointLights) : objectId{id} {
    this->createLightBuffers(device, pointLights);
  }

  EngineLightObject EngineLightObject::createLightObject(EngineDevice &device, std::vector<PointLight> pointLights) {
    static id_t currentId = 0;
    return EngineLightObject(currentId++, device, pointLights);
  }

  std::shared_ptr<EngineLightObject> EngineLightObject::createSharedLightObject(EngineDevice &device, std::vector<PointLight> pointLights) {
    static id_t currentId = 0;
    return std::make_shared<EngineLightObject>(currentId++, device, pointLights);
  }

  void EngineLightObject::createLightBuffers(EngineDevice &device, std::vector<PointLight> pointLights) {
    GlobalLight globalLight{};
    
    int lightIndex = 0;
    for (auto &&pointLight : pointLights) {
      globalLight.pointLights[lightIndex] = pointLight;
      lightIndex++;
    }

    globalLight.numLight = lightIndex;
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
