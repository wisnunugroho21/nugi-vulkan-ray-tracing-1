#include "sampling_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	EngineSamplingRenderSystem::EngineSamplingRenderSystem(EngineDevice& device, std::shared_ptr<EngineRenderPass> renderPass, std::shared_ptr<EngineDescriptorSetLayout> descriptorSetLayouts)
		: appDevice{device}
	{
		this->createPipelineLayout(descriptorSetLayouts->getDescriptorSetLayout());
		this->createPipeline(renderPass);
	}

	EngineSamplingRenderSystem::~EngineSamplingRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineSamplingRenderSystem::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayouts) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(RayTracePushConstant);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayouts;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(this->appDevice.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void EngineSamplingRenderSystem::createPipeline(std::shared_ptr<EngineRenderPass> renderPass) {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = EngineGraphicPipeline::Builder(this->appDevice, renderPass, this->pipelineLayout)
			.setDefault("shader/sampling.vert.spv", "shader/sampling.frag.spv")
			.build();
	}

	void EngineSamplingRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, VkDescriptorSet descriptorSets, std::shared_ptr<EngineVertexModel> model, uint32_t randomSeed) {
		this->pipeline->bind(commandBuffer->getCommandBuffer());

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			this->pipelineLayout,
			0,
			1,
			&descriptorSets,
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