#pragma once

#include "../image/image.hpp"
#include "../renderpass/renderpass.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace nugiEngine {
  class EngineForwardPassSubRenderer {
    public:
      EngineForwardPassSubRenderer(EngineDevice &device, int imageCount, int width, int height);
      std::shared_ptr<EngineRenderPass> getRenderPass() const { return this->renderPass; }

      std::vector<std::shared_ptr<EngineImage>> getAlbedoResources() { return this->albedoResources; }
      std::vector<std::shared_ptr<EngineImage>> getNormalResources() { return this->normalResources; }
      
      std::vector<VkDescriptorImageInfo> getAlbedoInfoResources();
      std::vector<VkDescriptorImageInfo> getNormalInfoResources();

      void beginRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer, int currentImageIndex);
			void endRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer);

      void transferFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex);
      
    private:
      int width, height;
      EngineDevice &device;

      std::vector<std::shared_ptr<EngineImage>> albedoResources;
      std::vector<std::shared_ptr<EngineImage>> normalResources;
      std::vector<std::shared_ptr<EngineImage>> depthImages;

      std::shared_ptr<EngineRenderPass> renderPass;

      VkFormat findDepthFormat();
      
      void createAlbedoResources(int imageCount);
      void createNormalResources(int imageCount);
      void createDepthResources(int imageCount);
      void createRenderPass(int imageCount);
  };
  
} // namespace nugiEngine
