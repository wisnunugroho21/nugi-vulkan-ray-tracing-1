#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  struct RayTraceVertex {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec2 texel;

    bool operator == (const RayTraceVertex &other) const {
			return this->position == other.position && this->texel == other.texel;
		}
  };

  struct Primitive {
    alignas(16) glm::uvec3 indices;
    uint32_t materialIndex;
  };

  struct Object {
    uint32_t firstBvhIndex = 0;
    uint32_t firstPrimitiveIndex = 0;
    uint32_t transformIndex;
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
    uint32_t textureIndex;
  };

  struct Transformation {
    glm::mat4 pointMatrix{1.0f};
    glm::mat4 pointInverseMatrix{1.0f};
    glm::mat4 dirInverseMatrix{1.0f};
    glm::mat4 normalInverseMatrix{1.0f};
  };

  struct Light {
    alignas(16) glm::uvec3 indices;
    alignas(16) glm::vec3 color;
  };

  struct RayTraceUbo {
    alignas(16) glm::vec3 origin;
    alignas(16) glm::vec3 horizontal;
    alignas(16) glm::vec3 vertical;
    alignas(16) glm::vec3 lowerLeftCorner;
    alignas(16) glm::vec3 background;
    uint32_t numLights = 0;
  };

  struct RayTracePushConstant {
    uint32_t randomSeed;
  };
}