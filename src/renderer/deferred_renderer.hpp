#pragma once

#include "../window/window.hpp"
#include "../device/device.hpp"
#include "../swap_chain/swap_chain.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../command/command_buffer.hpp"
#include "../globalUbo.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace nugiEngine {
	class EngineDefferedRenderer {
		public:
			EngineDefferedRenderer(EngineWindow& window, EngineDevice& device);
			~EngineDefferedRenderer();

			EngineDefferedRenderer(const EngineDefferedRenderer&) = delete;
			EngineDefferedRenderer& operator = (const EngineDefferedRenderer&) = delete;

			std::shared_ptr<EngineSwapChain> getSwapChain() const { return this->swapChain; }
			std::shared_ptr<EngineDescriptorPool> getDescriptorPool() const { return this->descriptorPool; }
			std::shared_ptr<VkDescriptorSet> getGlobalDescriptorSets(int frameIndex) { return this->globalDescriptorSets[frameIndex]; }
			std::shared_ptr<EngineDescriptorSetLayout> getGlobalDescSetLayout() const { return this->globalDescSetLayout; }

			bool isFrameInProgress() const { return this->isFrameStarted; }

			VkCommandBuffer getCommandBuffer() const { 
				assert(this->isFrameStarted && "cannot get command buffer when frame is not in progress");
				return this->commandBuffers[this->currentFrameIndex]->getCommandBuffer();
			}

			uint32_t getFrameIndex() {
				assert(this->isFrameStarted && "cannot get frame index when frame is not in progress");
				return this->currentFrameIndex;
			}

			uint32_t getImageIndex() {
				assert(this->isFrameStarted && "cannot get image index when frame is not in progress");
				return this->currentImageIndex;
			}

			void createGlobalBuffer();
			void createLightBuffer();

			void createDescriptor();
			
			void writeGlobalBuffer(int frameIndex, GlobalUBO* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
			void writeLightBuffer(int frameIndex, GlobalLight* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

			std::shared_ptr<EngineCommandBuffer> beginCommand();
			void endCommand(std::shared_ptr<EngineCommandBuffer>);

			void submitCommands(std::vector<std::shared_ptr<EngineCommandBuffer>> commandBuffer);
			void submitCommand(std::shared_ptr<EngineCommandBuffer> commandBuffer);

			bool acquireFrame();
			bool presentFrame();

		private:
			void recreateSwapChain();
			void createSyncObjects(uint32_t imageCount);
			void createDescriptorPool(uint32_t imageCount);

			EngineWindow& appWindow;
			EngineDevice& appDevice;

			std::shared_ptr<EngineSwapChain> swapChain;
			std::vector<std::shared_ptr<EngineCommandBuffer>> commandBuffers;

			std::vector<std::shared_ptr<EngineBuffer>> globalBuffers;
			std::vector<std::shared_ptr<EngineBuffer>> lightBuffers;

			std::shared_ptr<EngineDescriptorPool> descriptorPool;
      std::shared_ptr<EngineDescriptorSetLayout> globalDescSetLayout;
      std::vector<std::shared_ptr<VkDescriptorSet>> globalDescriptorSets;

			std::vector<VkSemaphore> imageAvailableSemaphores;
			std::vector<VkSemaphore> renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;

			uint32_t currentImageIndex = 0, currentFrameIndex = 0;
			bool isFrameStarted = false;
	};
}