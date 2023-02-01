#include "sampling_ray_raster_render_system.hpp"

#include "../swap_chain/swap_chain.hpp"
#include "../ray_ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	EngineSamplingRayRasterRenderSystem::EngineSamplingRayRasterRenderSystem(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, 
		std::shared_ptr<EngineDescriptorSetLayout> traceRayDescLayout, uint32_t width, uint32_t height, uint32_t swapChainImageCount, VkRenderPass renderPass) : appDevice{device}
	{
		this->createAccumulateImages(width, height, swapChainImageCount);
		this->createDescriptor(descriptorPool, swapChainImageCount);

		this->createPipelineLayout(traceRayDescLayout);
		this->createPipeline(renderPass);
	}

	EngineSamplingRayRasterRenderSystem::~EngineSamplingRayRasterRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineSamplingRayRasterRenderSystem::createPipelineLayout(std::shared_ptr<EngineDescriptorSetLayout> traceRayDescLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(RayTracePushConstant);

		std::vector<VkDescriptorSetLayout> descSetLayouts = { traceRayDescLayout->getDescriptorSetLayout(), this->descSetLayout->getDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(this->appDevice.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void EngineSamplingRayRasterRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = EngineGraphicPipeline::Builder(this->appDevice, this->pipelineLayout, renderPass)
			.setDefault("shader/ray_trace_sampling.vert.spv", "shader/ray_trace_sampling.frag.spv")
			.build();
	}

	void EngineSamplingRayRasterRenderSystem::createAccumulateImages(uint32_t width, uint32_t height, uint32_t swapChainImageCount) {
		this->accumulateImages.clear();

		for (uint32_t i = 0; i < swapChainImageCount; i++) {
			auto accumulateImage = std::make_shared<EngineImage>(
				this->appDevice, width, height, 
				1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_B8G8R8A8_UNORM, 
				VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT, 
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT
			);

			accumulateImage->transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, 
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
				0, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT);

			this->accumulateImages.emplace_back(accumulateImage);
		}
	}

	void EngineSamplingRayRasterRenderSystem::createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, uint32_t swapChainImageCount) {
		this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(this->appDevice)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();
				
		this->descriptorSets.clear();

		for (uint32_t i = 0; i < swapChainImageCount; i++) {
			auto descSet = std::make_shared<VkDescriptorSet>();

			auto accumulateImage = this->accumulateImages[i];
			auto accumulateImageInfo = accumulateImage->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL);

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeImage(0, &accumulateImageInfo)
				.build(descSet.get());

			this->descriptorSets.emplace_back(descSet);
		}
	}

	void EngineSamplingRayRasterRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex, std::shared_ptr<VkDescriptorSet> traceRayDescSet, std::shared_ptr<EngineModel> model, uint32_t randomSeed) {
		this->pipeline->bind(commandBuffer->getCommandBuffer());

		VkDescriptorSet descpSet[2] = { *traceRayDescSet, *this->descriptorSets[imageIndex] };

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			this->pipelineLayout,
			0,
			2,
			descpSet,
			0,
			nullptr
		);

		RayTracePushConstant pushConstant{};
		pushConstant.randomSeed = randomSeed;

		vkCmdPushConstants(
			commandBuffer->getCommandBuffer(), 
			this->pipelineLayout, 
			VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(RayTracePushConstant),
			&pushConstant
		);

		model->bind(commandBuffer);
		model->draw(commandBuffer);
	}
}