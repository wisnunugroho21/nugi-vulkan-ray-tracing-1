#pragma once

#include "../device/device.hpp"
#include "../image/image.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace nugiEngine {
  class EngineRenderPass {
    public:
      class Builder {
        public:
          Builder(EngineDevice &appDevice, int width, int height);

          Builder addSubpass(VkSubpassDescription subpass);
          Builder addAttachments(VkAttachmentDescription attachment);
          Builder addDependency(VkSubpassDependency dependency);
          Builder addViewImages(std::vector<VkImageView> viewImages);
          Builder addColorBlendAttachments(VkPipelineColorBlendAttachmentState colorBlendAttachment);

          std::shared_ptr<EngineRenderPass> build();

        private:
          int width, height;
          EngineDevice &appDevice;
          
          std::vector<VkSubpassDescription> subpasses;
          std::vector<VkAttachmentDescription> attachments;
          std::vector<VkSubpassDependency> dependencies;
          std::vector<std::vector<VkImageView>> viewImages;
          std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
      };

      EngineRenderPass(EngineDevice &appDevice, std::vector<std::vector<VkImageView>> viewImages, VkRenderPassCreateInfo renderPassInfo, 
        int width, int height, std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments);
      ~EngineRenderPass();

      VkFramebuffer getFramebuffers(int index) const { return this->framebuffers[index]; }
      VkRenderPass getRenderPass() const { return this->renderPass; }
      std::vector<VkPipelineColorBlendAttachmentState> getColorBlendAttachments() const { return this->colorBlendAttachments; }

    private:
      EngineDevice &appDevice;

      std::vector<VkFramebuffer> framebuffers;
      VkRenderPass renderPass;
      std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;

      void createRenderPass(VkRenderPassCreateInfo renderPassInfo);
      void createFramebuffers(std::vector<std::vector<VkImageView>> viewImages, int width, int height);
  };
} // namespace nugiEngin 


