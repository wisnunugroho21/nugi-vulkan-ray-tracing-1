#include "deferred_render_system.hpp"

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
	EngineDeffereRenderSystem::EngineDeffereRenderSystem(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, 
		uint32_t width, uint32_t height, VkRenderPass renderPass, VkDescriptorSetLayout globalDescSetLayout, 
		std::vector<VkDescriptorImageInfo> forwardPassResourcesInfo[3]) : appDevice{device}
	{
		this->createDescriptor(descriptorPool, forwardPassResourcesInfo);

		this->createPipelineLayout(globalDescSetLayout);
		this->createPipeline(renderPass);
	}

	EngineDeffereRenderSystem::~EngineDeffereRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineDeffereRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalDescSetLayout) {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(RayTracePushConstant);

		std::vector<VkPushConstantRange> pushConstantRanges = { pushConstantRange };
		std::vector<VkDescriptorSetLayout> descSetLayouts = { globalDescSetLayout, this->descSetLayout->getDescriptorSetLayout() };

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

	void EngineDeffereRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		this->pipeline = EngineGraphicPipeline::Builder(this->appDevice, this->pipelineLayout, renderPass)
			.setDefault("shader/deferred.vert.spv", "shader/deferred.frag.spv")
			.build();
	}

	void EngineDeffereRenderSystem::createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, 
		std::vector<VkDescriptorImageInfo> forwardPassResourcesInfo[3])  
	{
		this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(this->appDevice)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();
				
		this->descriptorSets.clear();

		for (size_t i = 0; i < forwardPassResourcesInfo[0].size(); i++) {
			auto descSet = std::make_shared<VkDescriptorSet>();

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeImage(0, &forwardPassResourcesInfo[0][i])
				.writeImage(1, &forwardPassResourcesInfo[1][i])
				.writeImage(2, &forwardPassResourcesInfo[2][i])
				.build(descSet.get());

			this->descriptorSets.emplace_back(descSet);
		}
	}

	void EngineDeffereRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
		VkDescriptorSet& globalDescSet, std::vector<std::shared_ptr<EngineGameObject>> &gameObjects, uint32_t randomSeed)
	{
		this->pipeline->bind(commandBuffer->getCommandBuffer());
		std::vector<VkDescriptorSet> descpSet = { globalDescSet, *this->descriptorSets[frameIndex] };

		vkCmdBindDescriptorSets(
			commandBuffer->getCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			this->pipelineLayout,
			0,
			static_cast<uint32_t>(descpSet.size()),
			descpSet.data(),
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

		for (auto &&obj : gameObjects) {
			obj->rasterModel->bind(commandBuffer);
			obj->rasterModel->draw(commandBuffer);
		}
	}
}