#pragma once

#include "../model/model.hpp"
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
			EngineLightObject(EngineLightObject &&) = default;
			EngineLightObject& operator = (EngineLightObject &&) = default;

			EngineLightObject(id_t id) : objectId{id} {}

			id_t getId() { return this->objectId; }

			TransformComponent transform{};
			std::shared_ptr<EngineModel> model{};

		private:
			id_t objectId;
	};
	
}