#include "transform.hpp"

namespace nugiEngine {
  glm::mat4 TransformComponent::getPointMatrix() {
    glm::mat4 curTransf = glm::mat4{1.0f};

    curTransf = glm::rotate(curTransf, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    curTransf = glm::rotate(curTransf, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    curTransf = glm::rotate(curTransf, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    auto originScalePosition = (this->objectMaximum - this->objectMinimum) / 2.0f + this->objectMinimum;
    curTransf = glm::translate(curTransf, -1.0f * originScalePosition);
    curTransf = glm::scale(curTransf, this->scale);
    curTransf = glm::translate(curTransf, originScalePosition);

    curTransf = glm::translate(curTransf, this->translation);
    return curTransf;
  }

  glm::mat4 TransformComponent::getPointInverseMatrix() {
    return glm::inverse(this->getPointMatrix());
  }

  glm::mat4 TransformComponent::getDirInverseMatrix() {
    glm::mat4 curTransf = glm::mat4{1.0f};

    curTransf = glm::rotate(curTransf, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    curTransf = glm::rotate(curTransf, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    curTransf = glm::rotate(curTransf, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    auto originScalePosition = (this->objectMaximum - this->objectMinimum) / 2.0f + this->objectMinimum;
    curTransf = glm::translate(curTransf, -1.0f * originScalePosition);
    curTransf = glm::scale(curTransf, this->scale);
    curTransf = glm::translate(curTransf, originScalePosition);

    curTransf = glm::inverse(curTransf);
    return curTransf;
  }

  glm::mat4 TransformComponent::getNormalMatrix() {
    glm::mat4 curTransf = glm::mat4{1.0f};

    curTransf = glm::rotate(curTransf, this->rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    curTransf = glm::rotate(curTransf, this->rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    curTransf = glm::rotate(curTransf, this->rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    auto originScalePosition = (this->objectMaximum - this->objectMinimum) / 2.0f + this->objectMinimum;
    curTransf = glm::translate(curTransf, -1.0f * originScalePosition);
    curTransf = glm::scale(curTransf, this->scale);
    curTransf = glm::translate(curTransf, originScalePosition);

    return glm::inverseTranspose(curTransf);
  }
} // namespace nugiEngine