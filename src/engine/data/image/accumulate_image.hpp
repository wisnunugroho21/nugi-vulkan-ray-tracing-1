#pragma once

#include "../../../vulkan/device/device.hpp"
#include "../../../vulkan/buffer/buffer.hpp"
#include "../../../vulkan/descriptor/descriptor.hpp"
#include "../../../vulkan/image/image.hpp"

#include <memory>

namespace nugiEngine {
	class EngineAccumulateImage {
		public:
			EngineAccumulateImage(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount);

			std::vector<VkDescriptorImageInfo> getImagesInfo() const;

			void prepareFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);
			void finishFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);

		private:
			std::vector<std::shared_ptr<EngineImage>> accumulateImages;

			void createAccumulateImages(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount);
	};
	
}