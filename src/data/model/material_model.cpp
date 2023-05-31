#include "material_model.hpp"

#include <cstring>
#include <iostream>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace nugiEngine {
	EngineMaterialModel::EngineMaterialModel(EngineDevice &device, std::vector<std::shared_ptr<Material>> materials) : engineDevice{device} {
		auto materialData = this->createMaterialData(materials);
		this->createBuffers(materialData);
	}

	EngineMaterialModel::~EngineMaterialModel() {}

	MaterialData EngineMaterialModel::createMaterialData(std::vector<std::shared_ptr<Material>> materials) {
		MaterialData materialData{};
		for (int i = 0; i < materials.size(); i++) {
			materialData.materials[i] = *materials[i];
		}

		return materialData;
	}

	void EngineMaterialModel::createBuffers(MaterialData &data) {
		EngineBuffer materialStagingBuffer {
			this->engineDevice,
			sizeof(MaterialData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		materialStagingBuffer.map();
		materialStagingBuffer.writeToBuffer(&data);

		this->materialBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(MaterialData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->materialBuffer->copyBuffer(materialStagingBuffer.getBuffer(), sizeof(MaterialData));
	} 
} // namespace nugiEngine

