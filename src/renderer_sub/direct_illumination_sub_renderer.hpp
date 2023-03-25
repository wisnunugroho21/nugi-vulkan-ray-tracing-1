#pragma once

#include "../image/image.hpp"
#include "../renderpass/renderpass.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace nugiEngine {
  class EngineDirectIlluminationSubRenderer {
    public:
      EngineDirectIlluminationSubRenderer(EngineDevice &device, int imageCount, int width, int height);
      std::shared_ptr<EngineRenderPass> getRenderPass() const { return this->renderPass; }
      
      std::vector<VkDescriptorImageInfo> getOutputInfoResources();

      void beginRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer, int currentImageIndex);
			void endRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer);

      void transferFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex);
      
    private:
      int width, height;
      EngineDevice &device;

      std::vector<std::shared_ptr<EngineImage>> outputResources;
      std::vector<std::shared_ptr<EngineImage>> depthImages;

      std::shared_ptr<EngineRenderPass> renderPass;

      VkFormat findDepthFormat();
      
      void createOutputResources(int imageCount);
      void createDepthResources(int imageCount);
      
      void createRenderPass(int imageCount);
  };
  
} // namespace nugiEngine
