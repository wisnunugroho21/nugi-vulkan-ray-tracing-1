#include "forward_pass_desc_set.hpp"

namespace nugiEngine {
  EngineForwardPassDescSet::EngineForwardPassDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, 
    std::vector<VkDescriptorBufferInfo> uniformBufferInfo, VkDescriptorBufferInfo buffersInfo[2]) 
	{
		this->createDescriptor(device, descriptorPool, uniformBufferInfo, buffersInfo);
  }

  void EngineForwardPassDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, 
    std::vector<VkDescriptorBufferInfo> uniformBufferInfo, VkDescriptorBufferInfo buffersInfo[2]) 
	{
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build();
		
		this->descriptorSets.clear();
		for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet{};

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeBuffer(0, &uniformBufferInfo[i])
				.writeBuffer(1, &buffersInfo[0])
				.writeBuffer(2, &buffersInfo[1])
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}