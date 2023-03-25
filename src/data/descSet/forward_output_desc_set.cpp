#include "forward_output_desc_set.hpp"

namespace nugiEngine {
	
  EngineForwardOutputDescSet::EngineForwardOutputDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorImageInfo> forwardPassResourcesInfo[4]) {
		this->createDescriptor(device, descriptorPool, forwardPassResourcesInfo);
  }

  void EngineForwardOutputDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorImageInfo> forwardPassResourcesInfo[4]) {
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();
				
		this->descriptorSets.clear();

		for (size_t i = 0; i < forwardPassResourcesInfo[0].size(); i++) {
			std::shared_ptr<VkDescriptorSet> descSet = std::make_shared<VkDescriptorSet>();

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeImage(0, &forwardPassResourcesInfo[0][i])
				.writeImage(1, &forwardPassResourcesInfo[1][i])
				.writeImage(2, &forwardPassResourcesInfo[2][i])
				.writeImage(3, &forwardPassResourcesInfo[3][i])
				.build(descSet.get());

			this->descriptorSets.emplace_back(descSet);
		}
  }
}