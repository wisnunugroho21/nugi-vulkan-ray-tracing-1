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
	};
	 
	EngineForwardPassRenderSystem::EngineForwardPassRenderSystem(EngineDevice& device, VkRenderPass renderPass, 
    std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorBufferInfo> buffersInfo) : appDevice{device} {

		this->createUniformBuffer();
		this->createDescriptor(descriptorPool, buffersInfo);
		this->createPipelineLayout();
		this->createPipeline(renderPass);
	}

	EngineForwardPassRenderSystem::~EngineForwardPassRenderSystem() {
		vkDestroyPipelineLayout(this->appDevice.getLogicalDevice(), this->pipelineLayout, nullptr);
	}

	void EngineForwardPassRenderSystem::createPipelineLayout() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { this->descSetLayout->getDescriptorSetLayout() };
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

	void EngineForwardPassRenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(this->pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleInfo.sampleShadingEnable = VK_FALSE;
		multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		this->pipeline = EngineGraphicPipeline::Builder(this->appDevice, this->pipelineLayout, renderPass)
			.setDefault("shader/forward_pass.vert.spv", "shader/forward_pass.frag.spv")
			.setMultisampleInfo(multisampleInfo)
			.build();
	}

  void EngineForwardPassRenderSystem::createUniformBuffer() {
    this->uniformBuffers.clear();

		for (uint32_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			auto uniformBuffer = std::make_shared<EngineBuffer>(
				this->appDevice,
				sizeof(GlobalUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

			uniformBuffer->map();
			this->uniformBuffers.emplace_back(uniformBuffer);
		}
  }

  void EngineForwardPassRenderSystem::createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorBufferInfo> buffersInfo) {
		this->descSetLayout = 
			EngineDescriptorSetLayout::Builder(this->appDevice)
				.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();

		this->descriptorSets.resize(this->uniformBuffers.size());
		
		for (int i = 0; i < this->uniformBuffers.size(); i++) {
			this->descriptorSets[i] = std::make_shared<VkDescriptorSet>();
			auto uniformBufferInfo = this->uniformBuffers[i]->descriptorInfo();

			EngineDescriptorWriter(*this->descSetLayout, *descriptorPool)
				.writeBuffer(0, &uniformBufferInfo)
				.writeBuffer(1, &buffersInfo[0])
				.build(this->descriptorSets[i].get());
		}
  }

  void EngineForwardPassRenderSystem::writeUniformBuffer(int frameIndex, GlobalUBO* data, VkDeviceSize size, VkDeviceSize offset) {
		this->uniformBuffers[frameIndex]->writeToBuffer(data, size, offset);
		this->uniformBuffers[frameIndex]->flush(size, offset);
	}

	void EngineForwardPassRenderSystem::render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, std::vector<std::shared_ptr<EngineGameObject>> &gameObjects) {
		this->pipeline->bind(commandBuffer->getCommandBuffer());
    std::vector<VkDescriptorSet> descSets = { *this->descriptorSets[frameIndex] };

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

		for (auto& obj : gameObjects) {
			SimplePushConstantData pushConstant{};
			pushConstant.modelMatrix = obj->transform.mat4();

			vkCmdPushConstants(
				commandBuffer->getCommandBuffer(), 
				pipelineLayout, 
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&pushConstant
			);

			obj->model->bind(commandBuffer);
			obj->model->draw(commandBuffer);
		}
	}
}