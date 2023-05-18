#include "global_desc_set.hpp"

namespace nugiEngine {
  EngineGlobalDescSet::EngineGlobalDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorBufferInfo rayTraceModelInfo[5]) {
		this->createRasterBuffer(device);
		this->createDescriptor(device, descriptorPool, rayTraceModelInfo);
  }
  
  void EngineGlobalDescSet::createRasterBuffer(EngineDevice& device) {
    this->rasterBuffers.clear();

		for (uint32_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			auto rasterBuffer = std::make_shared<EngineBuffer>(
				device,
				sizeof(RasterUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

			rasterBuffer->map();
			this->rasterBuffers.emplace_back(rasterBuffer);
		}
  }

	void EngineGlobalDescSet::writeRasterBuffer(int frameIndex, RasterUBO* data, VkDeviceSize size, VkDeviceSize offset) {
		this->rasterBuffers[frameIndex]->writeToBuffer(data, size, offset);
		this->rasterBuffers[frameIndex]->flush(size, offset);
	}

  void EngineGlobalDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorBufferInfo rayTraceModelInfo[5]) {
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(5, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.build();

		this->descriptorSets.clear();

		for (uint32_t i = 0; i < this->rasterBuffers.size(); i++) {
			std::shared_ptr<VkDescriptorSet> descSet = std::make_shared<VkDescriptorSet>();

			auto rasterBufferInfo =  this->rasterBuffers[i]->descriptorInfo();

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeBuffer(0, &rasterBufferInfo)
				.writeBuffer(1, &rayTraceModelInfo[0])
				.writeBuffer(2, &rayTraceModelInfo[1])
				.writeBuffer(3, &rayTraceModelInfo[2])
				.writeBuffer(4, &rayTraceModelInfo[3])
				.writeBuffer(5, &rayTraceModelInfo[4])
				.build(descSet.get());

			this->descriptorSets.emplace_back(descSet);
		}
  }
}