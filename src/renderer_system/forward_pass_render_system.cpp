#include "forward_pass_render_system.hpp"

#include "../swap_chain/swap_chain.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	struct SimplePushConstantData {
		glm::mat4 modelMatrix{1.0f};
		glm::mat4 normalMatrix{1.0f};
	};
	 
	EngineForwardPassRenderSystem::EngineForwardPassRenderSystem(EngineDevice& device, VkRenderPass renderPass, 
    std::vector<VkDescriptorSetLayout> descSetLayout) 
		: appDevice{device} 
	{
		this->createPipelineLayout(descSetLayout);
		this->createPipeline(renderPass);
	}

	EngineForwardPassRenderSystem::~EngineForwardPassRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineForwardPassRenderSystem::createPipelineLayout(std::vector<VkDescriptorSetLayout> descSetLayout) {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descSetLayout.size());
		pipelineLayoutInfo.pSetLayouts = descSetLayout.data();

		if (vkCreatePipelineLayout(this->appDevice.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void EngineForwardPassRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleInfo.sampleShadingEnable = VK_FALSE;
		multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState positionBlendAttachment{};
		positionBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		positionBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState albedoBlendAttachment{};
		albedoBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		albedoBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState normalBlendAttachment{};
		normalBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		normalBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState materialBlendAttachment{};
		materialBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		materialBlendAttachment.blendEnable = VK_FALSE;

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = { positionBlendAttachment, albedoBlendAttachment, normalBlendAttachment, materialBlendAttachment };

		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.logicOpEnable = VK_FALSE;
		colorBlendInfo.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
		colorBlendInfo.pAttachments = colorBlendAttachments.data();

		this->pipeline = EngineGraphicPipeline::Builder(this->appDevice, this->pipelineLayout, renderPass)
			.setDefault("shader/forward_pass.vert.spv", "shader/forward_pass.frag.spv")
			.setColorBlendAttachments(colorBlendAttachments)
			.setColorBlendInfo(colorBlendInfo)
			.setMultisampleInfo(multisampleInfo)
			.build();
	}

	void EngineForwardPassRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, 
		std::vector<VkDescriptorSet> descSets, std::shared_ptr<EngineGeometry> gameObject) 
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

		gameObject->rasterModel->bind(commandBuffer);
		gameObject->rasterModel->draw(commandBuffer);
	}
}