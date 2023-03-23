#include "forward_light_render_system.hpp"

#include "../swap_chain/swap_chain.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	struct PointLightPushConstant {
		alignas(16) glm::vec4 position{};
		alignas(4) float radius;
		alignas(16) glm::vec4 color{};		
	};
	 
	EngineForwardLightRenderSystem::EngineForwardLightRenderSystem(EngineDevice& device, VkRenderPass renderPass, 
    VkDescriptorSetLayout globalDescSetLayout) : appDevice{device} 
	{
		this->createPipelineLayout(globalDescSetLayout);
		this->createPipeline(renderPass);
	}

	EngineForwardLightRenderSystem::~EngineForwardLightRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineForwardLightRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalDescSetLayout) {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { globalDescSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

		if (vkCreatePipelineLayout(this->appDevice.getLogicalDevice(), &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void EngineForwardLightRenderSystem::createPipeline(VkRenderPass renderPass) {
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
			.setDefault("shader/forward_light.vert.spv", "shader/forward_light.frag.spv")
			.setBindingDescriptions({})
			.setAttributeDescriptions({})
			.setColorBlendAttachments(colorBlendAttachments)
			.setColorBlendInfo(colorBlendInfo)
			.setMultisampleInfo(multisampleInfo)
			.build();
	}

	void EngineForwardLightRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
		VkDescriptorSet &globalDescSet, uint32_t numLight) 
	{
		this->pipeline->bind(commandBuffer->getCommandBuffer());
		std::vector<VkDescriptorSet> descSets = { globalDescSet };

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

		vkCmdDraw(commandBuffer->getCommandBuffer(), 6 * numLight, 1, 0, 0);
	}
}