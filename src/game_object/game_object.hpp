#pragma once

#include "../model/model.hpp"
#include "../texture/texture.hpp"
#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	class EngineGameObject {
		public:
			using id_t = unsigned int;

			static EngineGameObject createGameObject();
			static std::shared_ptr<EngineGameObject> createSharedGameObject();

			EngineGameObject(const EngineGameObject &) = delete;
			EngineGameObject& operator = (const EngineGameObject &) = delete;
			EngineGameObject(EngineGameObject &&) = default;
			EngineGameObject& operator = (EngineGameObject &&) = default;

			EngineGameObject(id_t id) : objectId{id} {}

			id_t getId() { return this->objectId; }

			TransformComponent transform{};
			std::shared_ptr<EngineModel> model{};

		private:
			id_t objectId;
	};
	
}