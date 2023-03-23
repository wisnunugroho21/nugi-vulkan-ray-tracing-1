#include "material.hpp"
#include "../utils/utils.hpp"

namespace nugiEngine {
	EngineMaterial::EngineMaterial(EngineDevice &device, MaterialData &data) : engineDevice{device} {
		this->createMaterialBuffers(data);
	}

	void EngineMaterial::createMaterialBuffers(MaterialData &data) {
		EngineBuffer stagingBuffer {
			this->engineDevice,
			sizeof(MaterialData),
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.map();
		stagingBuffer.writeToBuffer(&data);

		this->materialBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(MaterialData),
			1,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		this->materialBuffer->copyBuffer(stagingBuffer.getBuffer(), sizeof(MaterialData));
	}
    
} // namespace nugiEngine

