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
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
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
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstant);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { globalDescSetLayout };
		std::vector<VkPushConstantRange> pushConstantRanges = { pushConstantRange };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

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

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = { positionBlendAttachment, albedoBlendAttachment, normalBlendAttachment };

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

	void EngineForwardLightRenderSystem::update(std::vector<std::shared_ptr<EngineLightObject>> &pointLightObjects, GlobalLight &globalLight) {
		int lightIndex = 0;

		for (auto& plo : pointLightObjects) {
			// copy light to ubo
			globalLight.pointLights[lightIndex].position = glm::vec4{ plo->position, 1.0f };
			globalLight.pointLights[lightIndex].color = glm::vec4{ plo->color, plo->intensity };
			globalLight.pointLights[lightIndex].radius = plo->radius;

			lightIndex++;
		}

		globalLight.numLight = lightIndex;
	}

	void EngineForwardLightRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
		VkDescriptorSet &globalDescSet, std::vector<std::shared_ptr<EngineLightObject>> &pointLights) 
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

		for (auto& pl : pointLights) {
			PointLightPushConstant pushConstant{};
			pushConstant.position = glm::vec4{ pl->position, 1.0f };
			pushConstant.color = glm::vec4{ pl->color, pl->intensity };
			pushConstant.radius = pl->radius;

			vkCmdPushConstants(
				commandBuffer->getCommandBuffer(), 
				pipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstant),
				&pushConstant
			);

			vkCmdDraw(commandBuffer->getCommandBuffer(), 6, 1, 0, 0);
		}
	}
}