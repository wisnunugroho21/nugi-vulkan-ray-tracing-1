#include "deferred_render_system.hpp"

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
	EngineDeffereRenderSystem::EngineDeffereRenderSystem(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, 
		uint32_t width, uint32_t height, std::vector<std::shared_ptr<EngineImage>> positionImages, VkRenderPass renderPass) 
		: appDevice{device}
	{
		this->createDescriptor(descriptorPool, positionImages);

		this->createPipelineLayout();
		this->createPipeline(renderPass);
	}

	EngineDeffereRenderSystem::~EngineDeffereRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineDeffereRenderSystem::createPipelineLayout() {
		std::vector<VkDescriptorSetLayout> descSetLayouts = { this->descSetLayout->getDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descSetLayouts.data();

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

	void EngineDeffereRenderSystem::createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<std::shared_ptr<EngineImage>> positionImages) {
		this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(this->appDevice)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();
				
		this->descriptorSets.clear();

		for (auto &&positionImage : positionImages) {
			auto descSet = std::make_shared<VkDescriptorSet>();
			auto positionImageInfo = positionImage->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL);

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeImage(0, &positionImageInfo)
				.build(descSet.get());

			this->descriptorSets.emplace_back(descSet);
		}
	}

	void EngineDeffereRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, std::vector<std::shared_ptr<EngineGameObject>> &gameObjects) {
		this->pipeline->bind(commandBuffer->getCommandBuffer());
		std::vector<VkDescriptorSet> descpSet = { *this->descriptorSets[frameIndex] };

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

		for (auto &&obj : gameObjects) {
			obj->model->bind(commandBuffer);
			obj->model->draw(commandBuffer);
		}
	}
}