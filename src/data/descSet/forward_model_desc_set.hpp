#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../descriptor/descriptor.hpp"
#include "../../ubo.hpp"

#include <memory>

namespace nugiEngine {
	class EngineForwardModelDescSet {
		public:
			EngineForwardModelDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorBufferInfo modelBuffersInfo[2]);

      std::shared_ptr<VkDescriptorSet> getDescriptorSets(int frameIndex) { return this->descriptorSets[frameIndex]; }
			std::shared_ptr<EngineDescriptorSetLayout> getDescSetLayout() const { return this->descSetLayout; }

		private:
      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;

			void createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorBufferInfo modelBuffersInfo[2]);
	};
	
}