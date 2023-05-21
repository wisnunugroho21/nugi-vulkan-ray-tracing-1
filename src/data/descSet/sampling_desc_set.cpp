#include "sampling_desc_set.hpp"

namespace nugiEngine {
  EngineSamplingDescSet::EngineSamplingDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorImageInfo> samplingResourcesInfo[2]) {
		this->createDescriptor(device, descriptorPool, samplingResourcesInfo);
  }

  void EngineSamplingDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorImageInfo> samplingResourcesInfo[2]) {
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();
		
	this->descriptorSets.clear();
	for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet{};

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeImage(0, &samplingResourcesInfo[0][i])
				.writeImage(1, &samplingResourcesInfo[1][i])
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}