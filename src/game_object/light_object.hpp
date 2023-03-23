#pragma once

#include "../model/raster_model.hpp"
#include "../texture/texture.hpp"
#include "../ubo.hpp"
#include "../device/device.hpp"
#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	struct GlobalLight {
    PointLight pointLights[100];
    uint32_t numLight = 0;
  };

	class EngineLightObject {
		public:
			using id_t = unsigned int;

			EngineLightObject(id_t id, EngineDevice &device, std::vector<PointLight> pointLights);

			static EngineLightObject createLightObject(EngineDevice &device, std::vector<PointLight> pointLights);
			static std::shared_ptr<EngineLightObject> createSharedLightObject(EngineDevice &device, std::vector<PointLight> pointLights);

			EngineLightObject(const EngineLightObject &) = delete;
			EngineLightObject& operator = (const EngineLightObject &) = delete;
			EngineLightObject(EngineLightObject &&) = default;
			EngineLightObject& operator = (EngineLightObject &&) = default;

			id_t getId() { return this->objectId; }
			uint32_t getNumLight() { return this->numLight; }

			VkDescriptorBufferInfo getModelInfo() { return this->lightBuffer->descriptorInfo();  }

		private:
			std::shared_ptr<EngineBuffer> lightBuffer;
			uint32_t numLight;
			id_t objectId;

			void createLightBuffers(EngineDevice &device, std::vector<PointLight> pointLights);
	};
}