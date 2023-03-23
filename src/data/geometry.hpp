#pragma once

#include "../model/raster_model.hpp"
#include "../model/ray_trace_model.hpp"
#include "../texture/texture.hpp"
#include "transform.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	class EngineGeometry {
		public:
			using id_t = unsigned int;

			static EngineGeometry createGeometry();
			static EngineGeometry createGeometry(EngineDevice &device, RayTraceModelData rayTraceModelData);

			static std::shared_ptr<EngineGeometry> createSharedGeometry();
			static std::shared_ptr<EngineGeometry> createSharedGeometry(EngineDevice &device, RayTraceModelData rayTraceModelData);

			EngineGeometry(const EngineGeometry &) = delete;
			EngineGeometry& operator = (const EngineGeometry &) = delete;
			EngineGeometry(EngineGeometry &&) = default;
			EngineGeometry& operator = (EngineGeometry &&) = default;

			EngineGeometry(id_t id) : objectId{id} {}

			id_t getId() { return this->objectId; }

			std::shared_ptr<EngineRasterModel> rasterModel{};
			std::shared_ptr<EngineRayTraceModel> rayTraceModel{};

		private:
			id_t objectId;
	};
	
}