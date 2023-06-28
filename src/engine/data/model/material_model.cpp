#include "material_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineMaterialModel::EngineMaterialModel(EngineDevice &device, std::shared_ptr<std::vector<Material>> materials) : engineDevice{device} {
		this->createBuffers(materials);
	}

	void EngineMaterialModel::createBuffers(std::shared_ptr<std::vector<Material>> materials) {
		auto materialBufferSize = sizeof(Material) * materials->size();

		EngineBuffer materialStagingBuffer {
			this->engineDevice,
			static_cast<VkDeviceSize>(materialBufferSize),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		materialStagingBuffer.map();
		materialStagingBuffer.writeToBuffer(materials->data());

		this->materialBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			static_cast<VkDeviceSize>(materialBufferSize),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->materialBuffer->copyBuffer(materialStagingBuffer.getBuffer(), static_cast<VkDeviceSize>(materialBufferSize));
	} 
} // namespace nugiEngine

