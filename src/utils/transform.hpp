#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	struct TransformComponent {
		glm::vec4 translation{0.0f};
		glm::vec4 scale{1.0f};
		glm::vec4 rotation{0.0f};

		glm::mat4 rotationMatrix();
    glm::mat4 inverserotationMatrix();
		glm::mat4 normalMatrix();
	};
}