#include "ray_trace_desc_set.hpp"

namespace nugiEngine {
  EngineRayTraceDescSet::EngineRayTraceDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorBufferInfo> uniformBufferInfo, 
		std::vector<VkDescriptorImageInfo> rayTraceImageInfo, VkDescriptorBufferInfo buffersInfo[9], std::vector<VkDescriptorImageInfo> resourcesInfo[5]) 
	{
		this->createDescriptor(device, descriptorPool, uniformBufferInfo, rayTraceImageInfo, buffersInfo, resourcesInfo);
  }

  void EngineRayTraceDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorBufferInfo> uniformBufferInfo, 
		std::vector<VkDescriptorImageInfo> rayTraceImageInfo, VkDescriptorBufferInfo buffersInfo[9], std::vector<VkDescriptorImageInfo> resourcesInfo[5]) 
	{
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(5, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(6, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(7, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(8, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(9, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(10, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(11, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(12, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(13, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(14, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(15, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT)
				.build();
		
	this->descriptorSets.clear();
		for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorSet descSet{};

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeImage(0, &rayTraceImageInfo[i])
				.writeBuffer(1, &uniformBufferInfo[i])
				.writeBuffer(2, &buffersInfo[0])
				.writeBuffer(3, &buffersInfo[1])
				.writeBuffer(4, &buffersInfo[2])
				.writeBuffer(5, &buffersInfo[3])
				.writeBuffer(6, &buffersInfo[4])
				.writeBuffer(7, &buffersInfo[5])
				.writeBuffer(8, &buffersInfo[6])
				.writeBuffer(9, &buffersInfo[7])
				.writeBuffer(10, &buffersInfo[8])
				.writeImage(11, &resourcesInfo[0][i])
				.writeImage(12, &resourcesInfo[1][i])
				.writeImage(13, &resourcesInfo[2][i])
				.writeImage(14, &resourcesInfo[3][i])
				.writeImage(15, &resourcesInfo[4][i])
				.build(&descSet);

			this->descriptorSets.emplace_back(descSet);
		}
  }
}