#pragma once

#include <functional>
#include "../ray_ubo.hpp"

namespace nugiEngine {
  template <typename T, typename... Rest>
  void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
  };

  struct RayTraceVertexPrimitive {
    std::shared_ptr<std::vector<Primitive>> primitives;
    std::shared_ptr<std::vector<RayTraceVertex>> vertices;
  };

  RayTraceVertexPrimitive loadPrimitiveVertexFromFile(const std::string &filePath, uint32_t materialIndex, uint32_t firstVertexIndex);
} // namespace nugiEngine
