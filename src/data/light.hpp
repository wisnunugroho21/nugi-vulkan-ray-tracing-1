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

	class EngineLight {
		public:
			using id_t = unsigned int;

			EngineLight(id_t id, EngineDevice &device, std::vector<PointLight> pointLights);

			static EngineLight createLight(EngineDevice &device, std::vector<PointLight> pointLights);
			static std::shared_ptr<EngineLight> createSharedLight(EngineDevice &device, std::vector<PointLight> pointLights);

			EngineLight(const EngineLight &) = delete;
			EngineLight& operator = (const EngineLight &) = delete;
			EngineLight(EngineLight &&) = default;
			EngineLight& operator = (EngineLight &&) = default;

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