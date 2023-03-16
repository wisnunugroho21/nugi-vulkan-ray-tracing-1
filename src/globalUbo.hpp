#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace nugiEngine {
  #define MAX_LIGHTS 10

  struct PointLight {
    glm::vec4 position{};
    float radius = 0;
    glm::vec4 color{};
  };

  struct GlobalUBO {
    glm::mat4 projection{1.0f};
    glm::mat4 view{1.0f};
    glm::mat4 inverseView{1.0f};
  };

  struct GlobalLight {
    PointLight pointLights[10];
    uint32_t numLight = 0;
  };
}