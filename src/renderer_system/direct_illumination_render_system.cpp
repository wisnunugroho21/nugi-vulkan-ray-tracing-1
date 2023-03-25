#include "direct_illumination_render_system.hpp"

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
	EngineDirectIlluminationRenderSystem::EngineDirectIlluminationRenderSystem(EngineDevice& device, VkRenderPass renderPass, 
		std::vector<VkDescriptorSetLayout> descSetLayouts) : appDevice{device}
	{
		this->createPipelineLayout(descSetLayouts);
		this->createPipeline(renderPass);
	}

	EngineDirectIlluminationRenderSystem::~EngineDirectIlluminationRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineDirectIlluminationRenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayouts) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
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

	void EngineDirectIlluminationRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = EngineGraphicPipeline::Builder(this->appDevice, this->pipelineLayout, renderPass)
			.setDefault("shader/direct_illumination.vert.spv", "shader/direct_illumination.frag.spv")
			.build();
	}

	void EngineDirectIlluminationRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
		std::vector<VkDescriptorSet> descSets, std::shared_ptr<EngineGeometry> gameObject, 
		uint32_t randomSeed)
	{
		this->pipeline->bind(commandBuffer->getCommandBuffer());

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
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
			VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(RayTracePushConstant),
			&pushConstant
		);

		gameObject->rasterModel->bind(commandBuffer);
		gameObject->rasterModel->draw(commandBuffer);
	}
}