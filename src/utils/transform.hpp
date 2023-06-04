#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	struct TransformComponent {
		glm::vec3 translation{0.0f};
		glm::vec3 scale{1.0f};
		glm::vec3 rotation{0.0f};

		glm::mat3 rotationMatrix();
    glm::mat3 inverserotationMatrix();
		glm::mat3 normalMatrix();
	};
}