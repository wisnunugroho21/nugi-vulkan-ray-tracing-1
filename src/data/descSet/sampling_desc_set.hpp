#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../descriptor/descriptor.hpp"

#include <memory>

namespace nugiEngine {
	class EngineSamplingDescSet {
		public:
			EngineSamplingDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorImageInfo> samplingResourcesInfo[2]);

			VkDescriptorSet getDescriptorSets(int frameIndex) { return this->descriptorSets[frameIndex]; }
			std::shared_ptr<EngineDescriptorSetLayout> getDescSetLayout() const { return this->descSetLayout; }

		private:
      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<VkDescriptorSet> descriptorSets;

			void createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorImageInfo> samplingResourcesInfo[2]);
	};
	
}