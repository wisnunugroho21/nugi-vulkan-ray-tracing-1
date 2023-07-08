#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <stdexcept>

#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"

namespace nugiEngine
{
  class EngineImage
  {
    public:
      EngineImage(EngineDevice &appDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, 
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags imageUsage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags memoryPropertyFlags, 
        VkImageAspectFlags aspectFlags);
      EngineImage(EngineDevice &appDevice, uint32_t width, uint32_t height, VkImage image, uint32_t mipLevels, VkFormat format, VkImageAspectFlags aspectFlags);
      ~EngineImage();

      VkImage getImage() const { return this->image; }
      VkImageView getImageView() const { return this->imageView; }
      VkImageLayout getLayout() const { return this->layout; }
      
      VkImageAspectFlags getAspectFlag() { return this->aspectFlags; }
      uint32_t getMipLevels() { return this->mipLevels; }

      VkDescriptorImageInfo getDescriptorInfo(VkImageLayout desiredImageLayout);

      void transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, 
        VkAccessFlags srcAccess, VkAccessFlags dstAccess, uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, 
        uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr, 
        EngineDevice *appDevice = nullptr);

      void copyImageFromOther(std::shared_ptr<EngineImage> srcImage, VkImageLayout srcLayout, VkImageLayout dstLayout, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);
      void copyImageToOther(std::shared_ptr<EngineImage> dstImage, VkImageLayout srcLayout, VkImageLayout dstLayout, std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);

      void generateMipMap(std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr);

      static void transitionImageLayout(std::vector<std::shared_ptr<EngineImage>> images, VkImageLayout oldLayout, VkImageLayout newLayout, 
        VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkAccessFlags srcAccess, VkAccessFlags dstAccess,
        uint32_t srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, uint32_t dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        std::shared_ptr<EngineCommandBuffer> commandBuffer = nullptr, EngineDevice *appDevice = nullptr);

    private:
      EngineDevice &appDevice;

      VkImage image;
      VkImageView imageView;

      VmaAllocation allocation;
      VmaAllocationInfo allocationInfo;

      VkFormat format;
      VkImageAspectFlags aspectFlags;
      VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
      
      uint32_t width, height, mipLevels;
      bool isImageCreatedByUs = false;

      void createImage(VkSampleCountFlagBits numSamples, VkImageTiling tiling, VkImageUsageFlags imageUsage, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags memoryPropertyFlags);
      void createImageView();
  };
  
  
} // namespace nugiEngine
