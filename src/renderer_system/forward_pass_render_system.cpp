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
    std::shared_ptr<EngineDescriptorPool> descriptorPool, VkDescriptorSetLayout globalDescSetLayout, 
		VkDescriptorBufferInfo modelBuffersInfo[2]) 
		: appDevice{device} 
	{
		this->createDescriptor(descriptorPool, modelBuffersInfo);
		this->createPipelineLayout(globalDescSetLayout);
		this->createPipeline(renderPass);
	}

	EngineForwardPassRenderSystem::~EngineForwardPassRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineForwardPassRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalDescSetLayout) {
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { globalDescSetLayout, this->descSetLayout->getDescriptorSetLayout() };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

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

		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = { positionBlendAttachment, albedoBlendAttachment, normalBlendAttachment };

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

  void EngineForwardPassRenderSystem::createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, 
		VkDescriptorBufferInfo modelBuffersInfo[2]) 
	{
		this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(this->appDevice)
				.addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
				.build();
		
		for (int i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			auto descSet = std::make_shared<VkDescriptorSet>();

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeBuffer(0, &modelBuffersInfo[0])
				.writeBuffer(1, &modelBuffersInfo[1])
				.build(descSet.get());

			this->descriptorSets.emplace_back(descSet);
		}
  }

	void EngineForwardPassRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, 
		VkDescriptorSet &globalDescSet, std::shared_ptr<EngineGameObject> gameObject) 
	{
		this->pipeline->bind(commandBuffer->getCommandBuffer());
		std::vector<VkDescriptorSet> descSets = { globalDescSet, *this->descriptorSets[frameIndex] };

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