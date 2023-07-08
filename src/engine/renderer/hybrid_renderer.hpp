#pragma once

#include "../../vulkan/window/window.hpp"
#include "../../vulkan/device/device.hpp"
#include "../../vulkan/swap_chain/swap_chain.hpp"
#include "../../vulkan/buffer/buffer.hpp"
#include "../../vulkan/descriptor/descriptor.hpp"
#include "../../vulkan/command/command_buffer.hpp"
#include "../general_struct.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace nugiEngine {
	class EngineHybridRenderer
	{
		public:
			EngineHybridRenderer(EngineWindow& window, EngineDevice& device);
			~EngineHybridRenderer();

			EngineHybridRenderer(const EngineHybridRenderer&) = delete;
			EngineHybridRenderer& operator = (const EngineHybridRenderer&) = delete;

			std::shared_ptr<EngineSwapChain> getSwapChain() const { return this->swapChain; }
			std::shared_ptr<EngineDescriptorPool> getDescriptorPool() const { return this->descriptorPool; }
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
				assert(this->isFrameStarted && "cannot get frame index when frame is not in progress");
				return this->currentImageIndex;
			}

			std::shared_ptr<EngineCommandBuffer> beginCommand();
			void endCommand(std::shared_ptr<EngineCommandBuffer>);

			void submitRenderCommands(std::vector<std::shared_ptr<EngineCommandBuffer>> commandBuffer);
			void submitRenderCommand(std::shared_ptr<EngineCommandBuffer> commandBuffer);
			void submitLoadCommand(std::shared_ptr<EngineCommandBuffer> commandBuffer);

			bool acquireFrame();
			bool presentFrame();

		private:
			void recreateSwapChain();
			void createSyncObjects(uint32_t imageCount);
			void createDescriptorPool();

			EngineWindow& appWindow;
			EngineDevice& appDevice;

			std::shared_ptr<EngineSwapChain> swapChain;
			std::vector<std::shared_ptr<EngineCommandBuffer>> commandBuffers;

			std::shared_ptr<EngineDescriptorPool> descriptorPool;

			std::vector<VkSemaphore> imageAvailableSemaphores, renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;
			VkSemaphore resourceLoadedSemaphore;

			uint32_t currentImageIndex = 0, currentFrameIndex = 0;
			bool isFrameStarted = false;
	};
}