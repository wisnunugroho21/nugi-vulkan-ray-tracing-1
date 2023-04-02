#include "indirect_illumination_render_system.hpp"

#include "../swap_chain/swap_chain.hpp"
#include "../ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	EngineIndirectIlluminationRenderSystem::EngineIndirectIlluminationRenderSystem(EngineDevice& device, uint32_t width, uint32_t height, 
		std::vector<VkDescriptorSetLayout> descSetLayouts) : appDevice{device}, width{width}, height{height}
	{
		this->createPipelineLayout(descSetLayouts);
		this->createPipeline();
	}

	EngineIndirectIlluminationRenderSystem::~EngineIndirectIlluminationRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineIndirectIlluminationRenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayouts) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(RayTracePushConstant);

		std::vector<VkPushConstantRange> pushConstantRanges = { pushConstantRange };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

		if (vkCreatePipelineLayout(this->appDevice.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void EngineIndirectIlluminationRenderSystem::createPipeline() {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = EngineComputePipeline::Builder(this->appDevice, this->pipelineLayout)
			.setDefault("shader/indirect_illumination.comp.spv")
			.build();
	}

	void EngineIndirectIlluminationRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex, 
		std::vector<VkDescriptorSet> descSets, uint32_t randomSeed) 
	{
		this->pipeline->bind(commandBuffer->getCommandBuffer());

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_COMPUTE,
			this->pipelineLayout,
			0,
			static_cast<uint32_t>(descSets.size()),
			descSets.data(),
			0,
			nullptr
		);

		RayTracePushConstant pushConstant{};
		pushConstant.randomSeed = randomSeed;

		vkCmdPushConstants(
			commandBuffer->getCommandBuffer(), 
			this->pipelineLayout, 
			VK_SHADER_STAGE_COMPUTE_BIT,
			0,
			sizeof(RayTracePushConstant),
			&pushConstant
		);

		this->pipeline->dispatch(commandBuffer->getCommandBuffer(), this->width / 8, this->height / 8, 1);
	}
}