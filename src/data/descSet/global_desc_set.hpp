#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../descriptor/descriptor.hpp"
#include "../../ubo.hpp"

#include <memory>

namespace nugiEngine {
	class EngineGlobalDescSet {
		public:
			EngineGlobalDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorBufferInfo rayTraceModelInfo[5]);

      std::shared_ptr<VkDescriptorSet> getDescriptorSets(int frameIndex) { return this->descriptorSets[frameIndex]; }
			std::shared_ptr<EngineDescriptorSetLayout> getDescSetLayout() const { return this->descSetLayout; }

			void writeRasterBuffer(int frameIndex, RasterUBO* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

		private:
      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;
			std::vector<std::shared_ptr<EngineBuffer>> rasterBuffers;

      void createRasterBuffer(EngineDevice& device);
			void createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorBufferInfo rayTraceModelInfo[5]);
	};
	
}