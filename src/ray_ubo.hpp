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

    alignas(16) glm::vec3 maximum;
    alignas(16) glm::vec3 minimum;
  };

  struct Material {
    alignas(16) glm::vec3 baseColor;
    float metallicness;
    float roughness;
    float fresnelReflect;
  };

  struct Transformation {
      alignas(16) glm::vec3 translationVector{0.0f};
      alignas(16) glm::vec3 scalingVector{1.0f};
      alignas(16) glm::mat3 rotationMatrix{1.0f};
      alignas(16) glm::mat3 inverseRotationMatrix{1.0f};
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

  struct BvhData {
    BvhNode bvhNodes[100];
	};
}