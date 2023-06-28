#include "accumulate_image.hpp"

namespace nugiEngine {
  EngineAccumulateImage::EngineAccumulateImage(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount) {
		this->createAccumulateImages(device, width, height, imageCount);
  }

	std::vector<VkDescriptorImageInfo> EngineAccumulateImage::getImagesInfo() const {
		std::vector<VkDescriptorImageInfo> imagesInfo{};
		
		for (int i = 0; i < this->accumulateImages.size(); i++) {
			imagesInfo.emplace_back(accumulateImages[i]->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL));
		}

		return imagesInfo;
	}

	void EngineAccumulateImage::createAccumulateImages(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount) {
   	this->accumulateImages.clear();

		for (uint32_t i = 0; i < imageCount; i++) {
			auto accumulateImage = std::make_shared<EngineImage>(
				device, width, height, 
				1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_B8G8R8A8_UNORM, 
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT
			);

			this->accumulateImages.emplace_back(accumulateImage);
		}
  }

	void EngineAccumulateImage::prepareFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		if (this->accumulateImages[frameIndex]->getLayout() == VK_IMAGE_LAYOUT_UNDEFINED) {
			this->accumulateImages[frameIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
				0, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, 
				VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
		} else {
			this->accumulateImages[frameIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
				VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
		}
	}

	void EngineAccumulateImage::finishFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->accumulateImages[frameIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, 0,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);
	}
}