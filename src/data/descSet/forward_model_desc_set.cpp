#include "forward_model_desc_set.hpp"

namespace nugiEngine {
  EngineForwardModelDescSet::EngineForwardModelDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorBufferInfo modelBuffersInfo[2]) {
		this->createDescriptor(device, descriptorPool, modelBuffersInfo);
  }

  void EngineForwardModelDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorBufferInfo modelBuffersInfo[2]) {
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build();
		
		for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			std::shared_ptr<VkDescriptorSet> descSet = std::make_shared<VkDescriptorSet>();

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeBuffer(0, &modelBuffersInfo[0])
				.writeBuffer(1, &modelBuffersInfo[1])
				.build(descSet.get());

			this->descriptorSets.emplace_back(descSet);
		}
  }
}