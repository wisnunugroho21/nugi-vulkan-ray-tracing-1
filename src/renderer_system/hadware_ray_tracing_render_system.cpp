#include "hadware_ray_tracing_render_system.hpp"

#include "../swap_chain/swap_chain.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	EngineRayTracingRenderSystem::EngineRayTracingRenderSystem(EngineDevice& device, EngineDeviceProcedures &deviceProcedure, EngineDescriptorPool &descriptorPool, 
		std::vector<EngineTopLevelAccelerationStructure> topLevelAccelStructs, size_t imageCount, uint32_t width, uint32_t height) 
		: appDevice{device}, deviceProcedure{deviceProcedure}, width{width}, height{height}
	{
		this->createStorageImage(imageCount);
		this->createDescriptor(descriptorPool, imageCount, topLevelAccelStructs);
		this->createPipelineLayout();
		this->createPipeline();
	}

	EngineRayTracingRenderSystem::~EngineRayTracingRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

  void EngineRayTracingRenderSystem::createStorageImage(size_t imageCount) {
    this->storageImages.resize(imageCount);
    for (auto &&image : this->storageImages) {
      image = std::make_shared<EngineImage>(this->appDevice, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_B8G8R8A8_UNORM, 
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

      image->transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, 
				0, VK_ACCESS_SHADER_WRITE_BIT);
    }
  }

  void EngineRayTracingRenderSystem::createDescriptor(EngineDescriptorPool &descriptorPool, size_t imageCount, std::vector<EngineTopLevelAccelerationStructure> topLevelAccelStructs) {
    this->rayTracingDescSetLayout = 
			EngineDescriptorSetLayout::Builder(this->appDevice)
				.addBinding(0, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
        .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR)
				.build();

		std::vector<VkAccelerationStructureKHR> accelStructs;
		for (auto &&topStruct : topLevelAccelStructs) {
			accelStructs.emplace_back(topStruct.getAccelStruct());
		}

		// Setup the descriptor for binding our top level acceleration structure to the ray tracing shaders
    VkWriteDescriptorSetAccelerationStructureKHR descriptorAccelStructInfo{};
    descriptorAccelStructInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    descriptorAccelStructInfo.accelerationStructureCount = static_cast<uint32_t>(accelStructs.size());
    descriptorAccelStructInfo.pAccelerationStructures = accelStructs.data();

    this->rayTracingDescSet.resize(imageCount);
		for (int i = 0; i < imageCount; i++) {
			this->rayTracingDescSet[i] = std::make_shared<VkDescriptorSet>();
      
      VkDescriptorImageInfo imageDescriptorInfo{};
      imageDescriptorInfo.imageView   = this->storageImages[i]->getImageView();
      imageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

			EngineDescriptorWriter(*this->rayTracingDescSetLayout, descriptorPool)
				.writeAccelStruct(0, &descriptorAccelStructInfo, static_cast<uint32_t>(accelStructs.size()))
				.writeImage(1, &imageDescriptorInfo)
				.build(this->rayTracingDescSet[i].get());
		}
  }

	void EngineRayTracingRenderSystem::createPipelineLayout() {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { this->rayTracingDescSetLayout->getDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

		if (vkCreatePipelineLayout(this->appDevice.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void EngineRayTracingRenderSystem::createPipeline() {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = EngineRayTracingPipeline::Builder(this->appDevice, this->deviceProcedure, this->pipelineLayout)
			.setDefault("shader/simple_raytrace_shader.rgen.spv", "shader/simple_raytrace_shader.rmiss.spv", "shader/simple_raytrace_shader.rchit.spv")
			.build();
	}

	void EngineRayTracingRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex, std::shared_ptr<EngineImage> swapChainImage) {
		this->pipeline->bind(commandBuffer->getCommandBuffer());

		std::vector<VkDescriptorSet> descriptorSets = { *this->rayTracingDescSet[imageIndex] };

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			this->pipelineLayout,
			0,
			static_cast<uint32_t>(descriptorSets.size()),
			descriptorSets.data(),
			0,
			nullptr
		);

		const uint32_t handleSizeAligned = EngineRayTracingPipeline::aligned_size(this->appDevice.getRayTracingProperties().shaderGroupHandleSize, this->appDevice.getRayTracingProperties().shaderGroupHandleAlignment);

		VkStridedDeviceAddressRegionKHR raygenSBTEntry{};
		raygenSBTEntry.deviceAddress = this->pipeline->getRaygenSBTBuffer()->getDeviceAddress();
		raygenSBTEntry.stride = handleSizeAligned;
		raygenSBTEntry.size = handleSizeAligned;

		VkStridedDeviceAddressRegionKHR missSbtEntry{};
		missSbtEntry.deviceAddress = this->pipeline->getMissgenSBTBuffer()->getDeviceAddress();
		missSbtEntry.stride = handleSizeAligned;
		missSbtEntry.size = handleSizeAligned;

		VkStridedDeviceAddressRegionKHR hitSbtEntry{};
		hitSbtEntry.deviceAddress = this->pipeline->getHitgenSBTBuffer()->getDeviceAddress();
		hitSbtEntry.stride = handleSizeAligned;
		hitSbtEntry.size = handleSizeAligned;

		VkStridedDeviceAddressRegionKHR callableSbtEntry{};

		this->deviceProcedure.vkCmdTraceRaysKHR(
			commandBuffer->getCommandBuffer(),
			&raygenSBTEntry,
			&missSbtEntry,
			&hitSbtEntry,
			&callableSbtEntry,
			this->width,
			this->height,
			1
		);

		swapChainImage->transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 
				0, VK_ACCESS_TRANSFER_WRITE_BIT);

		this->storageImages[imageIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
				VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR, VK_PIPELINE_STAGE_TRANSFER_BIT, 
				VK_ACCESS_SHADER_WRITE_BIT, VK_ACCESS_TRANSFER_READ_BIT);

		swapChainImage->copyImageFromOther(this->storageImages[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandBuffer);

		this->storageImages[imageIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL, 
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
				VK_ACCESS_TRANSFER_READ_BIT, VK_ACCESS_SHADER_WRITE_BIT);

		swapChainImage->transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 
				VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_MEMORY_READ_BIT);
	}
}