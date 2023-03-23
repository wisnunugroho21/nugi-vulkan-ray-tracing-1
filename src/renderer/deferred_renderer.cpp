#include "deferred_renderer.hpp"
#include "../ubo.hpp"

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	EngineDefferedRenderer::EngineDefferedRenderer(EngineWindow& window, EngineDevice& device, 
		VkDescriptorBufferInfo modelInfo[3]) : appDevice{ device }, appWindow{ window }
	{
		this->rayTraceModelInfo[0] = modelInfo[0];
		this->rayTraceModelInfo[1] = modelInfo[1];
		this->rayTraceModelInfo[2] = modelInfo[2];

		this->recreateSwapChain();
		this->createSyncObjects(static_cast<uint32_t>(this->swapChain->imageCount()));

		this->commandBuffers = EngineCommandBuffer::createCommandBuffers(device, EngineDevice::MAX_FRAMES_IN_FLIGHT);
		this->createDescriptorPool(this->swapChain->imageCount());

		this->createRasterBuffer();
		this->createDescriptor();
	}

	EngineDefferedRenderer::~EngineDefferedRenderer() {
		this->descriptorPool->resetPool();
		
    for (size_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(this->appDevice.getLogicalDevice(), this->renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(this->appDevice.getLogicalDevice(), this->imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(this->appDevice.getLogicalDevice(), this->inFlightFences[i], nullptr);
		}
	}

	void EngineDefferedRenderer::recreateSwapChain() {
		auto extent = this->appWindow.getExtent();
		while(extent.width == 0 || extent.height == 0) {
			extent = this->appWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(this->appDevice.getLogicalDevice());

		if (this->swapChain == nullptr) {
			this->swapChain = std::make_unique<EngineSwapChain>(this->appDevice, extent);
		} else {
			std::shared_ptr<EngineSwapChain> oldSwapChain = std::move(this->swapChain);
			this->swapChain = std::make_unique<EngineSwapChain>(this->appDevice, extent, oldSwapChain);

			if (!oldSwapChain->compareSwapFormat(*this->swapChain.get())) {
				throw std::runtime_error("Swap chain image has changed");
			}
		}
	}

	void EngineDefferedRenderer::createDescriptorPool(uint32_t imageCount) {
		this->descriptorPool = 
			EngineDescriptorPool::Builder(this->appDevice)
				.setMaxSets(10)
				.addPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 50)
				.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100)
				.build();
	}

	void EngineDefferedRenderer::createSyncObjects(uint32_t imageCount) {
		imageAvailableSemaphores.resize(EngineDevice::MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(EngineDevice::MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(EngineDevice::MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(this->appDevice.getLogicalDevice(), &semaphoreInfo, nullptr, &this->imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(this->appDevice.getLogicalDevice(), &semaphoreInfo, nullptr, &this->renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(this->appDevice.getLogicalDevice(), &fenceInfo, nullptr, &this->inFlightFences[i]) != VK_SUCCESS) {
					throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	bool EngineDefferedRenderer::acquireFrame() {
		assert(!this->isFrameStarted && "can't acquire frame while frame still in progress");

		std::vector<VkFence> acquireFrameFences = { this->inFlightFences[this->currentFrameIndex] };
		auto result = this->swapChain->acquireNextImage(&this->currentImageIndex, acquireFrameFences, this->imageAvailableSemaphores[this->currentFrameIndex]);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			this->recreateSwapChain();
			return false;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		this->isFrameStarted = true;
		return true;
	}

	void EngineDefferedRenderer::createRasterBuffer() {
		this->rasterBuffers.clear();

		for (uint32_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			auto rasterBuffer = std::make_shared<EngineBuffer>(
				this->appDevice,
				sizeof(RasterUBO),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

			rasterBuffer->map();
			this->rasterBuffers.emplace_back(rasterBuffer);
		}
	}

	void EngineDefferedRenderer::createDescriptor() {
		this->globalDescSetLayout = 
			EngineDescriptorSetLayout::Builder(this->appDevice)
				.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
				.addBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT)
				.build();

		this->globalDescriptorSets.clear();

		for (uint32_t i = 0; i < this->rasterBuffers.size(); i++) {
			VkDescriptorSet descSet{};

			auto globalBufferInfo =  this->rasterBuffers[i]->descriptorInfo();

			EngineDescriptorWriter(*this->globalDescSetLayout, *this->descriptorPool)
				.writeBuffer(0, &globalBufferInfo)
				.writeBuffer(1, &this->rayTraceModelInfo[0])
				.writeBuffer(2, &this->rayTraceModelInfo[1])
				.writeBuffer(3, &this->rayTraceModelInfo[2])
				.build(&descSet);

			this->globalDescriptorSets.emplace_back(descSet);
		}
	}

	void EngineDefferedRenderer::writeGlobalBuffer(int frameIndex, RasterUBO* data, VkDeviceSize size, VkDeviceSize offset) {
		this->rasterBuffers[frameIndex]->writeToBuffer(data, size, offset);
		this->rasterBuffers[frameIndex]->flush(size, offset);
	}

	std::shared_ptr<EngineCommandBuffer> EngineDefferedRenderer::beginCommand() {
		assert(this->isFrameStarted && "can't start command while frame still in progress");

		this->commandBuffers[this->currentFrameIndex]->beginReccuringCommand();
		return this->commandBuffers[this->currentFrameIndex];
	}

	void EngineDefferedRenderer::endCommand(std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		assert(this->isFrameStarted && "can't start command while frame still in progress");
		commandBuffer->endCommand();
	}

	void EngineDefferedRenderer::submitCommands(std::vector<std::shared_ptr<EngineCommandBuffer>> commandBuffer) {
		assert(this->isFrameStarted && "can't submit command if frame is not in progress");
		vkResetFences(this->appDevice.getLogicalDevice(), 1, &this->inFlightFences[this->currentFrameIndex]);

		std::vector<VkSemaphore> waitSemaphores = { this->imageAvailableSemaphores[this->currentFrameIndex] };
		std::vector<VkSemaphore> signalSemaphores = { this->renderFinishedSemaphores[this->currentFrameIndex] };
		std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		EngineCommandBuffer::submitCommands(commandBuffers, this->appDevice.getGraphicsQueue(this->currentFrameIndex), waitSemaphores, waitStages, signalSemaphores, this->inFlightFences[this->currentFrameIndex]);
	}

	void EngineDefferedRenderer::submitCommand(std::shared_ptr<EngineCommandBuffer> commandBuffer) {
		assert(this->isFrameStarted && "can't submit command if frame is not in progress");
		vkResetFences(this->appDevice.getLogicalDevice(), 1, &this->inFlightFences[this->currentFrameIndex]);

		std::vector<VkSemaphore> waitSemaphores = { this->imageAvailableSemaphores[this->currentFrameIndex] };
		std::vector<VkSemaphore> signalSemaphores = { this->renderFinishedSemaphores[this->currentFrameIndex] };
		std::vector<VkPipelineStageFlags> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		commandBuffer->submitCommand(this->appDevice.getGraphicsQueue(this->currentFrameIndex), waitSemaphores, waitStages, signalSemaphores, this->inFlightFences[this->currentFrameIndex]);
	}

	bool EngineDefferedRenderer::presentFrame() {
		assert(this->isFrameStarted && "can't present frame if frame is not in progress");

		std::vector<VkSemaphore> waitSemaphores = {this->renderFinishedSemaphores[this->currentFrameIndex]};
		auto result = this->swapChain->presentRenders(this->appDevice.getPresentQueue(this->currentFrameIndex), &this->currentImageIndex, waitSemaphores);

		this->currentFrameIndex = (this->currentFrameIndex + 1) % EngineDevice::MAX_FRAMES_IN_FLIGHT;
		this->isFrameStarted = false;

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->appWindow.wasResized()) {
			this->appWindow.resetResizedFlag();
			this->recreateSwapChain();

			this->descriptorPool->resetPool();
			this->createDescriptor();

			return false;
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image");
		}

		return true;
	}
}