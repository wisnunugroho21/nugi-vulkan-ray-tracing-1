#pragma once

#include "../model/raster_model.hpp"
#include "../model/ray_trace_model.hpp"
#include "../texture/texture.hpp"
#include "../ubo.hpp"
#include "../device/device.hpp"
#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	struct GlobalLight {
    Light lights[100];
		int numLight;
  };

	class EngineLight {
		public:
			using id_t = unsigned int;

			EngineLight(id_t id, EngineDevice &device, std::vector<std::shared_ptr<Light>> lights);

			static EngineLight createLight(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights);
			static std::shared_ptr<EngineLight> createSharedLight(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights);

			id_t getId() { return this->objectId; }
			uint32_t getNumLight() { return this->numLight; }

			VkDescriptorBufferInfo getModelInfo() { return this->lightBuffer->descriptorInfo();  }

		private:
			std::shared_ptr<EngineBuffer> lightBuffer;

			uint32_t numLight;
			id_t objectId;

			void createLightBuffers(EngineDevice &device, std::vector<std::shared_ptr<Light>> lights);
	};
}