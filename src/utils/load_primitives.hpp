#pragma once

#include <memory>
#include "../ray_ubo.hpp"

namespace nugiEngine {
  struct RayTraceVertexPrimitive {
    std::shared_ptr<std::vector<Primitive>> primitives;
    std::shared_ptr<std::vector<RayTraceVertex>> vertices;
  };

  RayTraceVertexPrimitive loadPrimitiveVertexFromFile(const std::string &filePath, uint32_t materialIndex, uint32_t firstVertexIndex);
};