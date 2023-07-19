#pragma once

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>

namespace nugiEngine {
  struct Vertex {
    glm::vec4 position{};
    glm::vec4 textCoord{};
    glm::vec4 normal{};
    uint32_t materialIndex{}; // Because of hybrid rendering, Material Index also hold by Vertex
    uint32_t transformIndex{}; // Because of hybrid rendering, Transform Index also hold by Vertex

    static std::vector<VkVertexInputBindingDescription> getVertexBindingDescriptions();
    static std::vector<VkVertexInputAttributeDescription> getVertexAttributeDescriptions();

    bool operator == (const Vertex &other) const;
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

  struct PointLight {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
  };

  struct AreaLight {
    alignas(16) glm::vec3 point0;
    alignas(16) glm::vec3 point1;
    alignas(16) glm::vec3 point2; 

    alignas(16) glm::vec3 color;
  };

  struct RayTraceUbo {
    alignas(16) glm::vec3 origin;
    alignas(16) glm::vec3 background;
    uint32_t numLights = 0;
  };

  struct RasterUbo {
    glm::mat4 projection{1.0f};
	  glm::mat4 view{1.0f};
  };

  struct RayTracePushConstant {
    uint32_t randomSeed;
  };
}