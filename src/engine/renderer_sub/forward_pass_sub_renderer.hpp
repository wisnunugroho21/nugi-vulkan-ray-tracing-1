#pragma once

#include "../../vulkan/image/image.hpp"
#include "../../vulkan/renderpass/renderpass.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace nugiEngine {
  class EngineForwardPassSubRenderer {
    public:
      EngineForwardPassSubRenderer(EngineDevice &device, int imageCount, int width, int height);
      std::shared_ptr<EngineRenderPass> getRenderPass() const { return this->renderPass; }
      
      std::vector<VkDescriptorImageInfo> getPositionInfoResources();
      std::vector<VkDescriptorImageInfo> getTextCoordInfoResources();
      std::vector<VkDescriptorImageInfo> getNormalInfoResources();
      std::vector<VkDescriptorImageInfo> getAlbedoColorInfoResources();
      std::vector<VkDescriptorImageInfo> getMaterialInfoResources();

      void beginRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer, int currentImageIndex);
			void endRenderPass(std::shared_ptr<EngineCommandBuffer> commandBuffer);

      void transferFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t imageIndex);
      
    private:
      int width, height;
      EngineDevice &device;

      std::vector<std::shared_ptr<EngineImage>> positionResources;
      std::vector<std::shared_ptr<EngineImage>> textCoordResources;
      std::vector<std::shared_ptr<EngineImage>> normalResources;
      std::vector<std::shared_ptr<EngineImage>> albedoColorResources;
      std::vector<std::shared_ptr<EngineImage>> materialResources;
      std::vector<std::shared_ptr<EngineImage>> depthImages;

      std::shared_ptr<EngineRenderPass> renderPass;

      VkFormat findDepthFormat();
      
      void createPositionResources(int imageCount);
      void createTextCoordResources(int imageCount);
      void createNormalResources(int imageCount);
      void createAlbedoColorResources(int imageCount);
      void createMaterialResources(int imageCount);
      void createDepthResources(int imageCount);
      
      void createRenderPass(int imageCount);
  };
  
} // namespace nugiEngine