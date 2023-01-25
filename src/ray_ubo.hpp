#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  struct Sphere {
    alignas(16) glm::vec3 center;
    alignas(4) float radius;
    alignas(4) uint32_t materialType;
  };

  struct Triangle {
    alignas(16) glm::vec3 point0;
    alignas(16) glm::vec3 point1;
    alignas(16) glm::vec3 point2;
  };

  struct RayTraceUbo {
    alignas(16) glm::vec3 origin;
    alignas(16) glm::vec3 horizontal;
    alignas(16) glm::vec3 vertical;
    alignas(16) glm::vec3 lowerLeftCorner;
  };

  struct BvhNode {
    int leftNode;
    int rightNode;
    int objIndex;

    alignas(16) glm::vec3 maximum;
    alignas(16) glm::vec3 minimum;
  };

  struct RayTraceObject {
    Triangle triangles[500];
  };

  struct RayTraceBvh {
    BvhNode bvhNodes[500];
  };

  struct RayTracePushConstant {
    int randomSeed;
	};
}