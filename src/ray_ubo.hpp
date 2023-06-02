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

  struct Primitive {
    Triangle triangle{};
    alignas(4) int materialIndex;
  };

  struct Object {
    alignas(4) int firstBvhIndex;
    alignas(4) int firstPrimitiveIndex;
    alignas(4) int transformIndex;
  };

  struct BvhNode {
    alignas(4) int leftNode = -1;
    alignas(4) int rightNode = -1;
    alignas(4) int leftObjIndex = -1;
    alignas(4) int rightObjIndex = -1;

    alignas(16) glm::vec3 maximum;
    alignas(16) glm::vec3 minimum;
  };

  struct Material {
    alignas(16) glm::vec3 baseColor;
    alignas(4) float metallicness;
    alignas(4) float roughness;
    alignas(4) float fresnelReflect;
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

  struct BvhData {
    BvhNode bvhNodes[200];
	};
}