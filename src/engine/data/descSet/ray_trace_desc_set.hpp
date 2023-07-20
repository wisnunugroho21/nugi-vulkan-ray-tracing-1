#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/descriptor/descriptor.hpp"

#include <memory>

namespace nugiEngine {
	class EngineRayTraceDescSet {
		public:
			EngineRayTraceDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorBufferInfo> uniformBufferInfo, std::vector<VkDescriptorImageInfo> rayTraceImageInfo, VkDescriptorBufferInfo buffersInfo[9], std::vector<VkDescriptorImageInfo> texturesInfo[2]);

			VkDescriptorSet getDescriptorSets(int frameIndex) { return this->descriptorSets[frameIndex]; }
			std::shared_ptr<EngineDescriptorSetLayout> getDescSetLayout() const { return this->descSetLayout; }

		private:
      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<VkDescriptorSet> descriptorSets;

			void createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorBufferInfo> uniformBufferInfo, std::vector<VkDescriptorImageInfo> rayTraceImageInfo, VkDescriptorBufferInfo buffersInfo[9], std::vector<VkDescriptorImageInfo> texturesInfo[2]);
	};
	
}