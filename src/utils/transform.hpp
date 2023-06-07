#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace nugiEngine {
	struct TransformComponent {
		glm::vec4 translation{0.0f};
		glm::vec4 scale{1.0f};
		glm::vec4 rotation{0.0f};

		glm::mat4 originTransfMatrix();
    glm::mat4 inverseOriginTransfMatrix();
    glm::mat4 directionTransfMatrix();
    glm::mat4 inverseDirectionTransfMatrix();
    glm::mat4 normalMatrix();
	};
}