#include "general_struct.hpp"

namespace nugiEngine {
  bool Vertex::operator == (const Vertex &other) const {
    return this->position == other.position && this->textCoord == other.textCoord 
      && this->materialIndex == other.materialIndex && this->transformIndex == other.transformIndex;
  }

  std::vector<VkVertexInputBindingDescription> Vertex::getVertexBindingDescriptions() {
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getVertexAttributeDescriptions() {
		std::vector<VkVertexInputAttributeDescription> attributeDescription(5);

		attributeDescription[0].binding = 0;
		attributeDescription[0].location = 0;
		attributeDescription[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescription[0].offset = offsetof(Vertex, position);

		attributeDescription[1].binding = 0;
		attributeDescription[1].location = 1;
		attributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescription[1].offset = offsetof(Vertex, textCoord);

		attributeDescription[2].binding = 0;
		attributeDescription[2].location = 2;
		attributeDescription[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescription[2].offset = offsetof(Vertex, normal);

		attributeDescription[3].binding = 0;
		attributeDescription[3].location = 3;
		attributeDescription[3].format = VK_FORMAT_R32_UINT;
		attributeDescription[3].offset = offsetof(Vertex, materialIndex);

		attributeDescription[4].binding = 0;
		attributeDescription[4].location = 4;
		attributeDescription[4].format = VK_FORMAT_R32_UINT;
		attributeDescription[4].offset = offsetof(Vertex, transformIndex);
		return attributeDescription;
	}
}