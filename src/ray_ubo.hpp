#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  struct Triangle {
    alignas(16) glm::vec3 point0{};
    alignas(16) glm::vec3 point1{};
    alignas(16) glm::vec3 point2{};
  };

  struct Sphere {
    alignas(16) glm::vec3 center{};
    float radius;
  };

  struct TextureCoordinate {
    alignas(16) glm::vec3 texel0{};
    alignas(16) glm::vec3 texel1{};
  };

  struct Primitive {
    alignas(16) Triangle triangle{};
    alignas(16) TextureCoordinate textCoord{};
    int materialIndex;
  };

  struct Object {
    uint32_t firstBvhIndex = 0;
    uint32_t firstPrimitiveIndex = 0;
    int transformIndex;
  };

  struct BvhNode {
    uint32_t leftNode = 0;
    uint32_t rightNode = 0;
    uint32_t leftObjIndex = 0;
    uint32_t rightObjIndex = 0;

    alignas(16) glm::vec3 maximum;
    alignas(16) glm::vec3 minimum;
  };

  struct Material {
    alignas(16) glm::vec3 baseColor;
    float metallicness;
    float roughness;
    float fresnelReflect;
    int textureIndex;
  };

  struct Transformation {
    glm::mat4 pointMatrix{1.0f};
    glm::mat4 pointInverseMatrix{1.0f};
    glm::mat4 dirInverseMatrix{1.0f};
    glm::mat4 normalInverseMatrix{1.0f};
  };

  struct Light {
    Triangle triangle{};
    alignas(16) glm::vec3 color;
  };

  struct RayTraceUbo {
    alignas(16) glm::vec3 origin;
    alignas(16) glm::vec3 horizontal;
    alignas(16) glm::vec3 vertical;
    alignas(16) glm::vec3 lowerLeftCorner;
    alignas(16) glm::vec3 background;
  };

  struct RayTracePushConstant {
    uint32_t randomSeed;
  };
}