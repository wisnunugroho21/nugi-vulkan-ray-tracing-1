#include "geometry.hpp"

namespace nugiEngine {
  EngineGeometry EngineGeometry::createGeometry() {
    static id_t currentId = 0;
    return EngineGeometry{currentId++};
  }

  EngineGeometry EngineGeometry::createGeometry(EngineDevice &device, RayTraceModelData rayTraceModelData) {
    RasterModelData rasterModelData;

    for (auto &&obj : rayTraceModelData.objects) {
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point0, obj.normal, obj.materialIndex });
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point1, obj.normal, obj.materialIndex });
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point2, obj.normal, obj.materialIndex });
    }

    static id_t currentId = 0;
    auto obj = EngineGeometry{currentId++};

    obj.rasterModel = std::make_shared<EngineRasterModel>(device, rasterModelData);
    obj.rayTraceModel = std::make_shared<EngineRayTraceModel>(device, rayTraceModelData);

    return obj;
  }

  std::shared_ptr<EngineGeometry> EngineGeometry::createSharedGeometry() {
    static id_t currentId = 0;
    return std::make_shared<EngineGeometry>(currentId++);
  }

  std::shared_ptr<EngineGeometry> EngineGeometry::createSharedGeometry(EngineDevice &device, RayTraceModelData rayTraceModelData) {
    RasterModelData rasterModelData;

    for (auto &&obj : rayTraceModelData.objects) {
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point0, obj.normal, obj.materialIndex, obj.transformIndex });
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point1, obj.normal, obj.materialIndex, obj.transformIndex });
      rasterModelData.vertices.emplace_back(Vertex{ obj.triangle.point2, obj.normal, obj.materialIndex, obj.transformIndex });
    }

    static id_t currentId = 0;
    auto obj = std::make_shared<EngineGeometry>(currentId++);

    obj->rasterModel = std::make_shared<EngineRasterModel>(device, rasterModelData);
    obj->rayTraceModel = std::make_shared<EngineRayTraceModel>(device, rayTraceModelData);

    return obj;
  }

} // namespace nugiEngine
