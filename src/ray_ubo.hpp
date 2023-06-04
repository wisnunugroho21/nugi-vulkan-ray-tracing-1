#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  struct Triangle {
    alignas(16) glm::vec4 point0;
    alignas(16) glm::vec4 point1;
    alignas(16) glm::vec4 point2;
  };

  struct Sphere {
    alignas(16) glm::vec4 center;
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

    alignas(16) glm::vec4 maximum;
    alignas(16) glm::vec4 minimum;
  };

  struct Material {
    alignas(16) glm::vec4 baseColor;
    alignas(4) float metallicness;
    alignas(4) float roughness;
    alignas(4) float fresnelReflect;
  };

  struct Transformation {
    alignas(16) glm::vec3 translationVector{0.0f};
    alignas(16) glm::mat3 rotationMatrix{0.0f};
    alignas(16) glm::vec3 scalingVector{1.0f};
    alignas(16) glm::mat3 rotationInverseMatrix{0.0f};
    alignas(16) glm::mat3 normalMatriks{1.0f};
  };

  struct Light {
    Triangle triangle{};
    alignas(16) glm::vec4 color;
  };

  struct RayTraceUbo {
    alignas(16) glm::vec4 origin;
    alignas(16) glm::vec4 horizontal;
    alignas(16) glm::vec4 vertical;
    alignas(16) glm::vec4 lowerLeftCorner;
    alignas(16) glm::vec4 background;
  };

  struct RayTracePushConstant {
    alignas(4) uint32_t randomSeed;
  };

  struct BvhData {
    BvhNode bvhNodes[100];
	};
}