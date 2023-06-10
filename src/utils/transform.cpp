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
        (c1 * c2),
        (s1 * c2),
        (-s2)
      },
      {
        (c1 * s2 * s3 - s1 * c3),
        (s1 * s2 * s3 + c1 * c3),
        (c2 * s3)
      },
      {
        (c1 * s2 * c3 + s1 * s3),
        (s1 * s2 * c3 - c1 * s3),
        (c2 * c3)
      }
    };
  }

  glm::mat3 TransformComponent::inverseRotationMatrix() {
    const float c3 = glm::cos(this->rotation.z);
    const float s3 = glm::sin(this->rotation.z);
    const float c2 = glm::cos(this->rotation.y);
    const float s2 = glm::sin(this->rotation.y);
    const float c1 = glm::cos(this->rotation.x);
    const float s1 = glm::sin(this->rotation.x);

    auto rotMat = glm::mat3{
      {
        (c1 * c2),
        (s1 * c2),
        (-s2)
      },
      {
        (c1 * s2 * s3 - s1 * c3),
        (s1 * s2 * s3 + c1 * c3),
        (c2 * s3)
      },
      {
        (c1 * s2 * c3 + s1 * s3),
        (s1 * s2 * c3 - c1 * s3),
        (c2 * c3)
      }
    };

    return glm::inverse(rotMat);
  }
} // namespace nugiEngine