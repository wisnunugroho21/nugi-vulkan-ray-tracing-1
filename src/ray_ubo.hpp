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

    struct BvhNode {
        int leftNode;
        int rightNode;
        int objIndex;

        alignas(16) glm::vec3 maximum;
        alignas(16) glm::vec3 minimum;
    };

  struct RayTraceUbo {
    alignas(16) glm::vec3 origin;
    alignas(16) glm::vec3 horizontal;
    alignas(16) glm::vec3 vertical;
    alignas(16) glm::vec3 lowerLeftCorner;
  };

  struct RayTracePushConstant {
    int randomSeed;
	};
}