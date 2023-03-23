#pragma once

#include "../window/window.hpp"
#include "../device/device.hpp"
#include "../swap_chain/swap_chain.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../command/command_buffer.hpp"
#include "../ubo.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace nugiEngine {
	class EngineDefferedRenderer {
		public:
			EngineDefferedRenderer(EngineWindow& window, EngineDevice& device, 
				VkDescriptorBufferInfo rayTraceModelInfo[3]);
			~EngineDefferedRenderer();

			EngineDefferedRenderer(const EngineDefferedRenderer&) = delete;
			EngineDefferedRenderer& operator = (const EngineDefferedRenderer&) = delete;

			std::shared_ptr<EngineSwapChain> getSwapChain() const { return this->swapChain; }
			std::shared_ptr<EngineDescriptorPool> getDescriptorPool() const { return this->descriptorPool; }
			VkDescriptorSet getGlobalDescriptorSets(int frameIndex) { return this->globalDescriptorSets[frameIndex]; }
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

			void createRasterBuffer();
			void createDescriptor();
			
			void writeGlobalBuffer(int frameIndex, RasterUBO* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

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

			std::vector<std::shared_ptr<EngineBuffer>> rasterBuffers;

			std::shared_ptr<EngineDescriptorPool> descriptorPool;
			std::shared_ptr<EngineDescriptorSetLayout> globalDescSetLayout;
			std::vector<VkDescriptorSet> globalDescriptorSets;
			VkDescriptorBufferInfo rayTraceModelInfo[3];

			std::vector<VkSemaphore> imageAvailableSemaphores;
			std::vector<VkSemaphore> renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;

			uint32_t currentImageIndex = 0, currentFrameIndex = 0;
			bool isFrameStarted = false;
	};
}