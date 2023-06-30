#include "trace_ray_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	EngineTraceRayRenderSystem::EngineTraceRayRenderSystem(EngineDevice& device, VkDescriptorSetLayout descriptorSetLayouts, uint32_t width, uint32_t height, uint32_t nSample) : appDevice{device}, width{width}, height{height}, nSample{nSample}
	{
		this->createPipelineLayout(descriptorSetLayouts);
		this->createPipeline();
	}

	EngineTraceRayRenderSystem::~EngineTraceRayRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineTraceRayRenderSystem::createPipelineLayout(VkDescriptorSetLayout descriptorSetLayouts) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
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

	void EngineTraceRayRenderSystem::createPipeline() {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = EngineComputePipeline::Builder(this->appDevice, this->pipelineLayout)
			.setDefault("shader/ray_trace.comp.spv")
			.build();
	}

	void EngineTraceRayRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, VkDescriptorSet descriptorSets, uint32_t randomSeed) {
		this->pipeline->bind(commandBuffer->getCommandBuffer());

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_COMPUTE,
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
			VK_SHADER_STAGE_COMPUTE_BIT,
			0,
			sizeof(RayTracePushConstant),
			&pushConstant
		);

		this->pipeline->dispatch(commandBuffer->getCommandBuffer(), this->width / 8, this->height / 8, this->nSample / 1);
	}
}