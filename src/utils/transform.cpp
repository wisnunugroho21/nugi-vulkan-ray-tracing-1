#include "transform.hpp"

namespace nugiEngine {
  glm::mat3 TransformComponent::rotationMatrix() {
    const float c3 = glm::cos(this->rotation.z);
    const float s3 = glm::sin(this->rotation.z);
    const float c2 = glm::cos(this->rotation.y);
    const float s2 = glm::sin(this->rotation.y);
    const float c1 = glm::cos(this->rotation.x);
    const float s1 = glm::sin(this->rotation.x);

    return glm::mat3{
      {
        (s1 * s2 * c3 - c1 * s3),
        (s1 * s2 * s3 + c1 * c3),
        (s1 * c2)
      },
      {
        (c2 * c3),
        (c2 * s3),
        (-s2)
      },
      {
        (c1 * s2 * c3 + s1 * s3),
        (c1 * s2 * s3 - s1 * c3),
        (c1 * c2)
      }
    };
  }

  glm::mat3 TransformComponent::inverserotationMatrix() {
    return glm::inverse(this->rotationMatrix());
  }

  glm::mat3 TransformComponent::normalMatrix() {
    const float c3 = glm::cos(this->rotation.z);
    const float s3 = glm::sin(this->rotation.z);
    const float c2 = glm::cos(this->rotation.x);
    const float s2 = glm::sin(this->rotation.x);
    const float c1 = glm::cos(this->rotation.y);
    const float s1 = glm::sin(this->rotation.y);

    const glm::vec3 invScale = 1.0f / scale;
    
    return glm::mat3{
      {
        invScale.x * (c1 * c3 + s1 * s2 * s3),
        invScale.x * (c2 * s3),
        invScale.x * (c1 * s2 * s3 - c3 * s1)
      },
      {
        invScale.y * (c3 * s1 * s2 - c1 * s3),
        invScale.y * (c2 * c3),
        invScale.y * (c1 * c3 * s2 + s1 * s3)
      },
      {
        invScale.z * (c2 * s1),
        invScale.z * (-s2),
        invScale.z * (c1 * c2)
      }
    };
  }
  
} // namespace nugiEngine