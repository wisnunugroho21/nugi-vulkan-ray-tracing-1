#pragma once

#include "../model/raster_model.hpp"
#include "../model/ray_trace_model.hpp"
#include "../texture/texture.hpp"
#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	class EngineGameObject {
		public:
			using id_t = unsigned int;

			static EngineGameObject createGameObject();
			static EngineGameObject createGameObject(EngineDevice &device, RayTraceModelData rayTraceModelData);

			static std::shared_ptr<EngineGameObject> createSharedGameObject();
			static std::shared_ptr<EngineGameObject> createSharedGameObject(EngineDevice &device, RayTraceModelData rayTraceModelData);

			EngineGameObject(const EngineGameObject &) = delete;
			EngineGameObject& operator = (const EngineGameObject &) = delete;
			EngineGameObject(EngineGameObject &&) = default;
			EngineGameObject& operator = (EngineGameObject &&) = default;

			EngineGameObject(id_t id) : objectId{id} {}

			id_t getId() { return this->objectId; }

			TransformComponent transform{};
			std::shared_ptr<EngineRasterModel> rasterModel{};
			std::shared_ptr<EngineRayTraceModel> rayTraceModel{};

		private:
			id_t objectId;
	};
	
}