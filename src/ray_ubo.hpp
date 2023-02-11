#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  struct Triangle {
    alignas(16) glm::vec3 point0;
    alignas(16) glm::vec3 point1;
    alignas(16) glm::vec3 point2;
  };

  struct Sphere {
    alignas(16) glm::vec3 center;
    alignas(4) float radius;
  };

  struct Object {
    Triangle triangle{};

    alignas(4) uint32_t materialType;
    alignas(4) uint32_t materialIndex;
  };
  

  struct BvhNode {
    alignas(4) int leftNode = -1;
    alignas(4) int rightNode = -1;
    alignas(4) int leftObjIndex = -1;
    alignas(4) int rightObjIndex = -1;

    alignas(16) glm::vec3 maximum;
    alignas(16) glm::vec3 minimum;
  };

  struct Lambertian {
    alignas(16) glm::vec3 albedoColor;
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
     alignas(4) uint32_t randomSeed;
  };
}