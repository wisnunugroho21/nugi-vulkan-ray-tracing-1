#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../device/device.hpp"
#include "../../engine/data/model/vertex_model.hpp"

namespace nugiEngine {
	struct GraphicPipelineConfigInfo {
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;

		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo{};
	};
	
	class EngineGraphicPipeline {
		public:
			class Builder {
				public:
					Builder(EngineDevice& appDevice, VkPipelineLayout pipelineLayout, VkRenderPass renderPass);

					std::vector<VkDynamicState> getDynamicStates() const { return this->dynamicStates; }
					std::vector<VkPipelineShaderStageCreateInfo> getShaderStagesInfo() const { return this->shaderStagesInfo; }

					Builder setDefault(const std::string& vertFilePath, const std::string& fragFilePath);

					Builder setSubpass(uint32_t subpass);
					Builder setBindingDescriptions(std::vector<VkVertexInputBindingDescription> bindingDescriptions);
					Builder setAttributeDescriptions (std::vector<VkVertexInputAttributeDescription> attributeDescriptions);

					Builder setInputAssemblyInfo(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo);
					Builder setRasterizationInfo(VkPipelineRasterizationStateCreateInfo rasterizationInfo);
					Builder setMultisampleInfo(VkPipelineMultisampleStateCreateInfo multisampleInfo);
					Builder setColorBlendAttachment(VkPipelineColorBlendAttachmentState colorBlendAttachment);
					Builder setColorBlendInfo(VkPipelineColorBlendStateCreateInfo colorBlendInfo);
					Builder setDepthStencilInfo(VkPipelineDepthStencilStateCreateInfo depthStencilInfo);
					Builder setDynamicStateInfo(VkPipelineDynamicStateCreateInfo dynamicStateInfo);
					Builder setShaderStagesInfo(std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo);

					std::unique_ptr<EngineGraphicPipeline> build();

				private:
					std::vector<VkDynamicState> dynamicStates{};
					std::vector<VkPipelineShaderStageCreateInfo> shaderStagesInfo{};
					GraphicPipelineConfigInfo configInfo{};
					
					EngineDevice& appDevice;
			};

			EngineGraphicPipeline(EngineDevice& device, const GraphicPipelineConfigInfo& configInfo);
			~EngineGraphicPipeline();

			EngineGraphicPipeline(const EngineGraphicPipeline&) = delete;
			EngineGraphicPipeline& operator =(const EngineDevice&) = delete;

			static std::vector<char> readFile(const std::string& filepath);
			static void createShaderModule(EngineDevice& appDevice, const std::vector<char>& code, VkShaderModule* shaderModule);

			void bind(VkCommandBuffer commandBuffer);

		private:
			EngineDevice& engineDevice;
			VkPipeline graphicPipeline;
			std::vector<VkShaderModule> shaderModules{};

			void createGraphicPipeline(const GraphicPipelineConfigInfo& configInfo);
	};
}