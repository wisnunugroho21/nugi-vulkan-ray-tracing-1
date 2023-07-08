#include "forward_pass_sub_renderer.hpp"

#include <assert.h>
#include <array>

namespace nugiEngine {
  EngineForwardPassSubRenderer::EngineForwardPassSubRenderer(EngineDevice &device, int imageCount, int width, int height) 
    : device{device}, width{width}, height{height}
  {
    this->createPositionResources(imageCount);
    this->createTextCoordResources(imageCount);
    this->createNormalResources(imageCount);
    this->createAlbedoColorResources(imageCount);
    this->createMaterialResources(imageCount);
    this->createDepthResources(imageCount);

    this->createRenderPass(imageCount);
  }

  std::vector<VkDescriptorImageInfo> EngineForwardPassSubRenderer::getPositionInfoResources() {
    std::vector<VkDescriptorImageInfo> descInfos{};
    for (auto &&positionInfo : this->positionResources) {
      descInfos.emplace_back(positionInfo->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL));
    }

    return descInfos;
  }

  std::vector<VkDescriptorImageInfo> EngineForwardPassSubRenderer::getTextCoordInfoResources() {
    std::vector<VkDescriptorImageInfo> descInfos{};
    for (auto &&textCoordInfo : this->textCoordResources) {
      descInfos.emplace_back(textCoordInfo->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL));
    }

    return descInfos;
  }
  
  std::vector<VkDescriptorImageInfo> EngineForwardPassSubRenderer::getNormalInfoResources() {
    std::vector<VkDescriptorImageInfo> descInfos{};
    for (auto &&normalInfo : this->normalResources) {
      descInfos.emplace_back(normalInfo->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL));
    }

    return descInfos;
  }

  std::vector<VkDescriptorImageInfo> EngineForwardPassSubRenderer::getAlbedoColorInfoResources() {
      std::vector<VkDescriptorImageInfo> descInfos{};
      for (auto&& albedoColorInfo : this->albedoColorResources) {
          descInfos.emplace_back(albedoColorInfo->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL));
      }

      return descInfos;
  }

  std::vector<VkDescriptorImageInfo> EngineForwardPassSubRenderer::getMaterialInfoResources() {
      std::vector<VkDescriptorImageInfo> descInfos{};
      for (auto&& materialInfo : this->materialResources) {
          descInfos.emplace_back(materialInfo->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL));
      }

      return descInfos;
  }

  void EngineForwardPassSubRenderer::createPositionResources(int imageCount) {
    this->positionResources.clear();

    for (int i = 0; i < imageCount; i++) {
      auto positionResource = std::make_shared<EngineImage>(
        this->device, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
        VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 
        VK_IMAGE_ASPECT_COLOR_BIT
      );

      this->positionResources.push_back(positionResource);
    }
  }

  void EngineForwardPassSubRenderer::createTextCoordResources(int imageCount) {
    this->textCoordResources.clear();

    for (int i = 0; i < imageCount; i++) {
      auto textCoordResource = std::make_shared<EngineImage>(
        this->device, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
        VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 
        VK_IMAGE_ASPECT_COLOR_BIT
      );

      this->textCoordResources.push_back(textCoordResource);
    }
  }

  void EngineForwardPassSubRenderer::createNormalResources(int imageCount) {
    this->normalResources.clear();

    for (int i = 0; i < imageCount; i++) {
      auto normalResource = std::make_shared<EngineImage>(
        this->device, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
        VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 
        VK_IMAGE_ASPECT_COLOR_BIT
      );

      this->normalResources.push_back(normalResource);
    }
  }

  void EngineForwardPassSubRenderer::createAlbedoColorResources(int imageCount) {
    this->albedoColorResources.clear();

    for (int i = 0; i < imageCount; i++) {
      auto colorImage = std::make_shared<EngineImage>(
        this->device, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
        VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 
        VK_IMAGE_ASPECT_COLOR_BIT
      );

      this->albedoColorResources.push_back(colorImage);
    }
  }

  void EngineForwardPassSubRenderer::createMaterialResources(int imageCount) {
    this->materialResources.clear();

    for (int i = 0; i < imageCount; i++) {
      auto materialResource = std::make_shared<EngineImage>(
        this->device, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
        VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 
        VK_IMAGE_ASPECT_COLOR_BIT
      );

      this->materialResources.push_back(materialResource);
    }
  }

  void EngineForwardPassSubRenderer::createDepthResources(int imageCount) {
    VkFormat depthFormat = this->findDepthFormat();

    this->depthImages.clear();

    for (int i = 0; i < imageCount; i++) {
      auto depthImage = std::make_shared<EngineImage>(
        this->device, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        VMA_MEMORY_USAGE_AUTO, VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT, 
        VK_IMAGE_ASPECT_DEPTH_BIT
      );

      this->depthImages.push_back(depthImage);
    }
  }

  void EngineForwardPassSubRenderer::createRenderPass(int imageCount) {
    VkAttachmentDescription positionAttachment{};
    positionAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    positionAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    positionAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    positionAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    positionAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    positionAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    positionAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    positionAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference positionAttachmentRef = {};
    positionAttachmentRef.attachment = 0;
    positionAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkPipelineColorBlendAttachmentState positionColorBlendAttachment{};
    positionColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    positionColorBlendAttachment.blendEnable = VK_FALSE;

    VkAttachmentDescription textCoordAttachment{};
    textCoordAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    textCoordAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    textCoordAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    textCoordAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    textCoordAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    textCoordAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    textCoordAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    textCoordAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference textCoordAttachmentRef = {};
    textCoordAttachmentRef.attachment = 1;
    textCoordAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkPipelineColorBlendAttachmentState textCoordColorBlendAttachment{};
    textCoordColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    textCoordColorBlendAttachment.blendEnable = VK_FALSE;

    VkAttachmentDescription normalAttachment{};
    normalAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    normalAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    normalAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    normalAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    normalAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    normalAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    normalAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    normalAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference normalAttachmentRef = {};
    normalAttachmentRef.attachment = 2;
    normalAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkPipelineColorBlendAttachmentState normalColorBlendAttachment{};
    normalColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    normalColorBlendAttachment.blendEnable = VK_FALSE;

    VkAttachmentDescription albedoColorAttachment{};
    albedoColorAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    albedoColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    albedoColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    albedoColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    albedoColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    albedoColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    albedoColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    albedoColorAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference albedoColorAttachmentRef = {};
    albedoColorAttachmentRef.attachment = 3;
    albedoColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkPipelineColorBlendAttachmentState albedoColorColorBlendAttachment{};
    albedoColorColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    albedoColorColorBlendAttachment.blendEnable = VK_FALSE;

    VkAttachmentDescription materialAttachment{};
    materialAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    materialAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    materialAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    materialAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    materialAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    materialAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    materialAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    materialAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference materialAttachmentRef = {};
    materialAttachmentRef.attachment = 4;
    materialAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkPipelineColorBlendAttachmentState materialColorBlendAttachment{};
    materialColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    materialColorBlendAttachment.blendEnable = VK_FALSE;

    std::vector<VkAttachmentReference> colorAttachmentRefs = { positionAttachmentRef, textCoordAttachmentRef, 
      normalAttachmentRef, albedoColorAttachmentRef, materialAttachmentRef };

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = this->findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 5;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentRefs.size());
    subpass.pColorAttachments = colorAttachmentRefs.data();
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency colorDependency{};
    colorDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    colorDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    colorDependency.srcAccessMask = 0;
    colorDependency.dstSubpass = 0;
    colorDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    colorDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency depthDependency{};
    depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.srcAccessMask = 0;
    depthDependency.dstSubpass = 0;
    depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    EngineRenderPass::Builder renderPassBuilder = EngineRenderPass::Builder(this->device, this->width, this->height)
      .addAttachments(positionAttachment)
      .addAttachments(textCoordAttachment)
      .addAttachments(normalAttachment)
			.addAttachments(albedoColorAttachment)
      .addAttachments(materialAttachment)
			.addAttachments(depthAttachment)
      .addColorBlendAttachments(positionColorBlendAttachment)
      .addColorBlendAttachments(textCoordColorBlendAttachment)
      .addColorBlendAttachments(normalColorBlendAttachment)
      .addColorBlendAttachments(albedoColorColorBlendAttachment)
      .addColorBlendAttachments(materialColorBlendAttachment)
			.addSubpass(subpass)
			.addDependency(colorDependency)
			.addDependency(depthDependency);

    for (int i = 0; i < imageCount; i++) {
			renderPassBuilder.addViewImages({
        this->positionResources[i]->getImageView(),
        this->textCoordResources[i]->getImageView(),
        this->normalResources[i]->getImageView(),
        this->albedoColorResources[i]->getImageView(),
        this->materialResources[i]->getImageView(),
        this->depthImages[i]->getImageView(),
      });
    }

		this->renderPass = renderPassBuilder.build();
  }

  VkFormat EngineForwardPassSubRenderer::findDepthFormat() {
    return this->device.findSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }

  void EngineForwardPassSubRenderer::beginRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer, int currentImageIndex) {
		VkRenderPassBeginInfo renderBeginInfo{};
		renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderBeginInfo.renderPass = this->getRenderPass()->getRenderPass();
		renderBeginInfo.framebuffer = this->getRenderPass()->getFramebuffers(currentImageIndex);

		renderBeginInfo.renderArea.offset = { 0, 0 };
		renderBeginInfo.renderArea.extent = { static_cast<uint32_t>(this->width), static_cast<uint32_t>(this->height) };

		std::array<VkClearValue, 6> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
    clearValues[1].color = { 0.0f, 0.0f, 0.0f, 0.0f };
    clearValues[2].color = { 0.0f, 0.0f, 0.0f, 0.0f };
    clearValues[3].color = { 0.0f, 0.0f, 0.0f, 0.0f };
    clearValues[4].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		clearValues[5].depthStencil = { 1.0f, 0 };

		renderBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer->getCommandBuffer(), &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(this->width);
		viewport.height = static_cast<float>(this->height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{{0, 0}, { static_cast<uint32_t>(this->width), static_cast<uint32_t>(this->height) }};
		vkCmdSetViewport(commandBuffer->getCommandBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer->getCommandBuffer(), 0, 1, &scissor);
	}

	void EngineForwardPassSubRenderer::endRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		vkCmdEndRenderPass(commandBuffer->getCommandBuffer());
	}

  void EngineForwardPassSubRenderer::transferFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex) {
    this->positionResources[imageIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, 
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, 
      VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
      commandBuffer);

    this->textCoordResources[imageIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, 
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, 
      VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
      commandBuffer);

    this->normalResources[imageIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, 
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, 
      VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
      commandBuffer);

    this->albedoColorResources[imageIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, 
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, 
      VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
      commandBuffer);

    this->materialResources[imageIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, 
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, 
      VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
      commandBuffer);
  }
} // namespace nugiEngine