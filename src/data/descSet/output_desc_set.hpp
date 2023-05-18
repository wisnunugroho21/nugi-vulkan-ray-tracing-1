#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../descriptor/descriptor.hpp"
#include "../../image/image.hpp"

#include <memory>

namespace nugiEngine {
	class EngineOutputDescSet {
		public:
			EngineOutputDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, uint32_t width, uint32_t height, uint32_t imageCount);

      std::shared_ptr<VkDescriptorSet> getDescriptorSets(int frameIndex) { return this->descriptorSets[frameIndex]; }
			std::shared_ptr<EngineDescriptorSetLayout> getDescSetLayout() const { return this->descSetLayout; }

			void prepareFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);
			void transferFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);
			void finishFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);

		private:
      std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;

			std::vector<std::shared_ptr<EngineImage>> rayTraceOutputImages;
			std::vector<std::shared_ptr<EngineImage>> accumulateImages;

      void createRayTraceOutputImages(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount);
			void createAccumulateImages(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount);
			void createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool);
	};
	
}