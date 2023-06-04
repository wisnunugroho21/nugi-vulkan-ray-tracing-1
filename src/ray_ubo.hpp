#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  struct Triangle {
    glm::vec3 point0;
    glm::vec3 point1;
    glm::vec3 point2;
  };

  struct Sphere {
    glm::vec3 center;
    float radius;
  };

  struct Primitive {
    Triangle triangle{};
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

    glm::vec3 maximum;
    glm::vec3 minimum;
  };

  struct Material {
    glm::vec3 baseColor;
    float metallicness;
    float roughness;
    float fresnelReflect;
  };

  struct Transformation {
    glm::vec3 translationVector{0.0f};
    glm::mat3 rotationMatrix{0.0f};
    glm::vec3 scalingVector{1.0f};
    glm::mat3 rotationInverseMatrix{0.0f};
    glm::mat3 normalMatriks{1.0f};
  };

  struct Light {
    Triangle triangle{};
    glm::vec3 color;
  };

  struct RayTraceUbo {
    glm::vec3 origin;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 lowerLeftCorner;
    glm::vec3 background;
  };

  struct RayTracePushConstant {
    uint32_t randomSeed;
  };

  struct BvhData {
    BvhNode bvhNodes[100];
	};
}