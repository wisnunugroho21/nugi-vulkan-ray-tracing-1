#include "output_desc_set.hpp"

namespace nugiEngine {
  EngineOutputDescSet::EngineOutputDescSet(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, uint32_t width, uint32_t height, uint32_t imageCount, std::vector<VkDescriptorImageInfo> outputImagesInfo[1]) {
		this->createAccumulateImages(device, width, height, imageCount);
		this->createRayTraceOutputImages(device, width, height, imageCount);
		this->createDescriptor(device, descriptorPool, outputImagesInfo);
  }
  
  void EngineOutputDescSet::createRayTraceOutputImages(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount) {
   	this->rayTraceOutputImages.clear();

		for (uint32_t i = 0; i < imageCount; i++) {
			auto rayTraceOutputImage = std::make_shared<EngineImage>(
				device, width, height, 
				1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_B8G8R8A8_UNORM, 
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT
			);

			this->rayTraceOutputImages.emplace_back(rayTraceOutputImage);
		}
  }

	void EngineOutputDescSet::createAccumulateImages(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount) {
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

  void EngineOutputDescSet::createDescriptor(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorImageInfo> outputImagesInfo[1]) {
    this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(device)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT)
				.build();

		this->descriptorSets.clear();

		for (uint32_t i = 0; i < this->accumulateImages.size(); i++) {
			std::shared_ptr<VkDescriptorSet> descSet = std::make_shared<VkDescriptorSet>();

			auto accumulateImageInfo =  this->accumulateImages[i]->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL);
			auto rayTraceOutputImageInfo =  this->rayTraceOutputImages[i]->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL);

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeImage(0, &accumulateImageInfo)
				.writeImage(1, &outputImagesInfo[0][i])
				.writeImage(2, &rayTraceOutputImageInfo)
				.build(descSet.get());

			this->descriptorSets.emplace_back(descSet);
		}
  }

	void EngineOutputDescSet::prepareFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		if (this->rayTraceOutputImages[frameIndex]->getLayout() == VK_IMAGE_LAYOUT_UNDEFINED) {
			this->rayTraceOutputImages[frameIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				0, VK_ACCESS_SHADER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
				commandBuffer);
		} else {
			this->rayTraceOutputImages[frameIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
				0, VK_ACCESS_SHADER_WRITE_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
				commandBuffer);
		}
	}

	void EngineOutputDescSet::transferFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
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

		this->rayTraceOutputImages[frameIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
			VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
			commandBuffer);
	}

	void EngineOutputDescSet::finishFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex) {
		this->accumulateImages[frameIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, 0,
			VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, commandBuffer);

		this->rayTraceOutputImages[frameIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
			VK_ACCESS_SHADER_READ_BIT, 0, VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
			commandBuffer);
	}
}