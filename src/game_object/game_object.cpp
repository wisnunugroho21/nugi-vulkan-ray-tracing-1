#include "game_object.hpp"

namespace nugiEngine {
  EngineGameObject EngineGameObject::createGameObject() {
    static id_t currentId = 0;
    return EngineGameObject{currentId++};
  }

  EngineGameObject EngineGameObject::createGameObject(EngineDevice &device, RayTraceModelData rayTraceModelData) {
    RasterModelData rasterModelData;

    for (auto &&obj : rayTraceModelData.objects) {
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point0, obj.normal, obj.materialIndex });
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point1, obj.normal, obj.materialIndex });
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point2, obj.normal, obj.materialIndex });
    }

    static id_t currentId = 0;
    auto obj = EngineGameObject{currentId++};

    obj.rasterModel = std::make_shared<EngineRasterModel>(device, rasterModelData);
    obj.rayTraceModel = std::make_shared<EngineRayTraceModel>(device, rayTraceModelData);

    return obj;
  }

  std::shared_ptr<EngineGameObject> EngineGameObject::createSharedGameObject() {
    static id_t currentId = 0;
    return std::make_shared<EngineGameObject>(currentId++);
  }

  std::shared_ptr<EngineGameObject> EngineGameObject::createSharedGameObject(EngineDevice &device, RayTraceModelData rayTraceModelData) {
    RasterModelData rasterModelData;

    for (auto &&obj : rayTraceModelData.objects) {
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point0, obj.normal, obj.materialIndex, obj.transformIndex });
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point1, obj.normal, obj.materialIndex, obj.transformIndex });
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point2, obj.normal, obj.materialIndex, obj.transformIndex });
    }

    static id_t currentId = 0;
    auto obj = std::make_shared<EngineGameObject>(currentId++);

    obj->rasterModel = std::make_shared<EngineRasterModel>(device, rasterModelData);
    obj->rayTraceModel = std::make_shared<EngineRayTraceModel>(device, rayTraceModelData);

    return obj;
  }

} // namespace nugiEngine
