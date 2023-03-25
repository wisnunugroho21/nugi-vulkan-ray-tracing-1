#include "direct_illumination_sub_renderer.hpp"

#include <assert.h>
#include <array>

namespace nugiEngine {
  EngineDirectIlluminationSubRenderer::EngineDirectIlluminationSubRenderer(EngineDevice &device, int imageCount, int width, int height) 
    : device{device}, width{width}, height{height}
  {
    this->createOutputResources(imageCount);
    this->createDepthResources(imageCount);

    this->createRenderPass(imageCount);
  }

  std::vector<VkDescriptorImageInfo> EngineDirectIlluminationSubRenderer::getOutputInfoResources() {
    std::vector<VkDescriptorImageInfo> descInfos{};
    for (auto &&positionInfo : this->outputResources) {
      descInfos.emplace_back(positionInfo->getDescriptorInfo(VK_IMAGE_LAYOUT_GENERAL));
    }

    return descInfos;
  }

  void EngineDirectIlluminationSubRenderer::createOutputResources(int imageCount) {
    this->outputResources.clear();

    for (int i = 0; i < imageCount; i++) {
      auto positionResource = std::make_shared<EngineImage>(
        this->device, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT
      );

      this->outputResources.push_back(positionResource);
    }
  }

  void EngineDirectIlluminationSubRenderer::createDepthResources(int imageCount) {
    VkFormat depthFormat = this->findDepthFormat();

    this->depthImages.clear();

    for (int i = 0; i < imageCount; i++) {
      auto depthImage = std::make_shared<EngineImage>(
        this->device, this->width, this->height, 1, VK_SAMPLE_COUNT_1_BIT, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT
      );

      this->depthImages.push_back(depthImage);
    }
  }

  void EngineDirectIlluminationSubRenderer::createRenderPass(int imageCount) {
    VkAttachmentDescription outputAttachment{};
    outputAttachment.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    outputAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    outputAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    outputAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    outputAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    outputAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    outputAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    outputAttachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference outputAttachmentRef = {};
    outputAttachmentRef.attachment = 0;
    outputAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    std::vector<VkAttachmentReference> colorAttachmentRefs = { outputAttachmentRef };

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = this->findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
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

    VkSubpassDependency outDependency{};
    outDependency.srcSubpass = 0;
    outDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    outDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    outDependency.dstSubpass = VK_SUBPASS_EXTERNAL;
    outDependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    outDependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    EngineRenderPass::Builder renderPassBuilder = EngineRenderPass::Builder(this->device, this->width, this->height)
      .addAttachments(outputAttachment)
			.addAttachments(depthAttachment)
			.addSubpass(subpass)
			.addDependency(colorDependency)
			.addDependency(depthDependency);

    for (int i = 0; i < imageCount; i++) {
			renderPassBuilder.addViewImages({
        this->outputResources[i]->getImageView(),
        this->depthImages[i]->getImageView(),
      });
    }

		this->renderPass = renderPassBuilder.build();
  }

  VkFormat EngineDirectIlluminationSubRenderer::findDepthFormat() {
    return this->device.findSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }

  void EngineDirectIlluminationSubRenderer::beginRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer, int currentImageIndex) {
		VkRenderPassBeginInfo renderBeginInfo{};
		renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderBeginInfo.renderPass = this->getRenderPass()->getRenderPass();
		renderBeginInfo.framebuffer = this->getRenderPass()->getFramebuffers(currentImageIndex);

		renderBeginInfo.renderArea.offset = { 0, 0 };
		renderBeginInfo.renderArea.extent = { static_cast<uint32_t>(this->width), static_cast<uint32_t>(this->height) };

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		clearValues[4].depthStencil = { 1.0f, 0 };

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

	void EngineDirectIlluminationSubRenderer::endRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		vkCmdEndRenderPass(commandBuffer->getCommandBuffer());
	}

  void EngineDirectIlluminationSubRenderer::transferFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex) {
    this->outputResources[imageIndex]->transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_GENERAL, 
      VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, 
      VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
      commandBuffer);
  }
} // namespace nugiEngine
