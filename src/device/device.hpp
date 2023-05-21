#pragma once

#include "../window/window.hpp"

// std lib headers
#include <string>
#include <vector>

namespace nugiEngine {
  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    uint32_t computeFamily;
    uint32_t transferFamily;

    bool graphicsFamilyHasValue = false;
    bool presentFamilyHasValue = false;
    bool computeFamilyHasValue = false;
    bool transferFamilyHasValue = false;

    bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue && computeFamilyHasValue && transferFamilyHasValue; }
  };

  class EngineDevice {
    public:
    #ifdef NDEBUG
      const bool enableValidationLayers = false;
    #else
      const bool enableValidationLayers = true;
    #endif

      static constexpr int MAX_FRAMES_IN_FLIGHT = 1;

      EngineDevice(EngineWindow &window);
      ~EngineDevice();

      // Not copyable or movable
      EngineDevice(const EngineDevice &) = delete;
      void operator=(const EngineDevice &) = delete;
      EngineDevice(EngineDevice &&) = delete;
      EngineDevice &operator=(EngineDevice &&) = delete;
      
      VkDevice getLogicalDevice() { return this->device; }
      VkPhysicalDevice getPhysicalDevice() { return this->physicalDevice; }

      VkCommandPool getCommandPool() { return this->commandPool; }
      VkSurfaceKHR getSurface() { return this->surface; }

      VkQueue getGraphicsQueue(uint32_t index) { return this->graphicsQueue[index]; }
      VkQueue getPresentQueue(uint32_t index) { return this->presentQueue[index]; }
      VkQueue getComputeQueue(uint32_t index) { return this->computeQueue[index]; }
      VkQueue getTransferQueue(uint32_t index) { return this->transferQueue[index]; }

      QueueFamilyIndices getFamilyIndices() { return this->familyIndices; }
      
      VkPhysicalDeviceProperties getProperties() { return this->properties; }
      VkSampleCountFlagBits getMSAASamples() { return this->msaaSamples; }

      SwapChainSupportDetails getSwapChainSupport() { return this->querySwapChainSupport(this->physicalDevice); }
      QueueFamilyIndices findPhysicalQueueFamilies() { return this->findQueueFamilies(this->physicalDevice); }
      uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
      VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    private:
      // creation function
      void createInstance();
      void setupDebugMessenger();
      void createSurface();
      void pickPhysicalDevice();
      void createLogicalDevice();
      void createCommandPool();

      // helper creation functions
      bool isDeviceSuitable(VkPhysicalDevice device);
      bool checkDeviceExtensionSupport(VkPhysicalDevice device);
      bool checkValidationLayerSupport();
      void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
      void hasGflwRequiredInstanceExtensions();
      std::vector<const char *> getRequiredExtensions();
      QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
      SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
      VkSampleCountFlagBits getMaxUsableFlagsCount();

      // instance
      VkInstance instance;
      VkDebugUtilsMessengerEXT debugMessenger;

      // device & its property
      VkDevice device;
      VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
      VkPhysicalDeviceProperties properties;

      // window system
      EngineWindow &window;
      VkSurfaceKHR surface;

      // command pool
      VkCommandPool commandPool;

      // queue
      std::vector<VkQueue> graphicsQueue;
      std::vector<VkQueue> presentQueue;
      std::vector<VkQueue> computeQueue;
      std::vector<VkQueue> transferQueue;

      // Queue Family Index
      QueueFamilyIndices familyIndices;

      // Anti-aliasing
      VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

      const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
      const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  };

}  // namespace lve