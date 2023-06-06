#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  struct Triangle {
      glm::vec4 point0;
      glm::vec4 point1;
      glm::vec4 point2;
  };

  struct Sphere {
    alignas(16) glm::vec4 center;
    float radius;
  };

  struct Primitive {
    alignas(16) Triangle triangle{};
    int materialIndex;
  };

  struct Object {
      int firstBvhIndex;
      int firstPrimitiveIndex;
      int transformIndex;
  };

  struct BvhNode {
      int leftNode = -1;
      int rightNode = -1;
      int leftObjIndex = -1;
      int rightObjIndex = -1;

      glm::vec4 maximum;
      glm::vec4 minimum;
  };

  struct Material {
      alignas(16) glm::vec4 baseColor;
      float metallicness;
      float roughness;
      float fresnelReflect;
  };

  struct Transformation {
    glm::vec4 translationVector{0.0f};
    glm::vec4 scalingVector{1.0f};
  };

  struct Light {
    Triangle triangle{};
    alignas(16) glm::vec4 color;
  };

  struct RayTraceUbo {
    glm::vec4 origin;
    glm::vec4 horizontal;
    glm::vec4 vertical;
    glm::vec4 lowerLeftCorner;
    glm::vec4 background;
  };

  struct RayTracePushConstant {
    uint32_t randomSeed;
  };

  struct BvhData {
    BvhNode bvhNodes[100];
	};
}