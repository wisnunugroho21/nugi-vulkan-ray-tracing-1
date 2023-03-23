#pragma once

#include "../model/raster_model.hpp"
#include "../texture/texture.hpp"
#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	class EngineLightObject {
		public:
			using id_t = unsigned int;

			static EngineLightObject createLightObject();
			static std::shared_ptr<EngineLightObject> createSharedLightObject();

			EngineLightObject(const EngineLightObject &) = delete;
			EngineLightObject& operator = (const EngineLightObject &) = delete;

			EngineLightObject(id_t id) : objectId{id} {}

			id_t getId() { return this->objectId; }

			glm::vec3 position{};
			float radius;

			glm::vec3 color{};
			float intensity;

		private:
			id_t objectId;
	};
}