#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{1.0f, 1.0f, 1.0f};
		glm::vec3 rotation{};

		glm::mat3 rotationMatrix();
    glm::mat3 inverserotationMatrix();
		glm::mat3 normalMatrix();
	};
}