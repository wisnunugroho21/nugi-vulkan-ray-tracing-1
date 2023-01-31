#include "app.hpp"

#include "../camera/camera.hpp"
#include "../mouse_controller/mouse_controller.hpp"
#include "../keyboard_controller/keyboard_controller.hpp"
#include "../buffer/buffer.hpp"
#include "../frame_info.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>
#include <chrono>
#include <iostream>

namespace nugiEngine {
	EngineApp::EngineApp() {
		this->renderer = std::make_unique<EngineRayTraceRenderer>(this->window, this->device);

		this->loadObjects();
		this->recreateSubRendererAndSubsystem();
	}

	EngineApp::~EngineApp() {}

	void EngineApp::run() {
		auto currentTime = std::chrono::high_resolution_clock::now();
		uint32_t t = 0;

		RayTraceUbo ubo = this->updateCamera();

		while (!this->window.shouldClose()) {
			this->window.pollEvents();

			/*auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

			if (t == 10) {
				std::string appTitle = std::string(APP_TITLE) + std::string(" | FPS: ") + std::to_string((1.0f / frameTime));
				glfwSetWindowTitle(this->window.getWindow(), appTitle.c_str());

				t = 0;
			} else {
				t++;
			}

			currentTime = newTime;*/

			if (this->renderer->acquireFrame()) {
				uint32_t imageIndex = this->renderer->getImageIndex();
				uint32_t frameIndex = this->renderer->getFrameIndex();
				uint32_t randomSeed = this->renderer->getRandomSeed();

				if (!this->traceRayRender->isFrameUpdated[imageIndex]) {
					this->traceRayRender->writeGlobalData(imageIndex, ubo);
					this->traceRayRender->isFrameUpdated[imageIndex] = true;
				}

				auto commandBuffer = this->renderer->beginCommand();
				
				this->traceRayRender->prepareFrame(commandBuffer, imageIndex);
				this->traceRayRender->render(commandBuffer, imageIndex, randomSeed);
				this->traceRayRender->finishFrame(commandBuffer, imageIndex);

				std::shared_ptr<VkDescriptorSet> traceRayDescSet = this->traceRayRender->getDescriptorSets(imageIndex);

				this->samplingRayRender->prepareFrame(commandBuffer, imageIndex);
				this->samplingRayRender->render(commandBuffer, imageIndex, traceRayDescSet);
				this->samplingRayRender->finishFrame(commandBuffer, imageIndex);

				this->renderer->endCommand(commandBuffer);
				this->renderer->submitCommand(commandBuffer);

				if (!this->renderer->presentFrame()) {
					this->recreateSubRendererAndSubsystem();
					ubo = this->updateCamera();
				}
			}
		}

		vkDeviceWaitIdle(this->device.getLogicalDevice());
	}

	void EngineApp::loadObjects() {
		RayTraceModelData modeldata{};

		Sphere sphere1{glm::vec3{0.0, -1000.0, 0.0}, 1000.0, 0};
		modeldata.spheres.emplace_back(sphere1);

		Sphere sphere2{glm::vec3{0.0, 1.0, 0.0}, 1.0, 2 };
		modeldata.spheres.emplace_back(sphere2);

		Sphere sphere3{glm::vec3{-4.0, 1.0, 0.0}, 1.0, 0 };
		modeldata.spheres.emplace_back(sphere3);

		Sphere sphere4{glm::vec3{4.0, 1.0, 0.0}, 1.0, 1 };
		modeldata.spheres.emplace_back(sphere4);

		this->models = std::make_unique<EngineRayTraceModel>(this->device, modeldata);
	}

	RayTraceUbo EngineApp::updateCamera() {
		uint32_t width = this->renderer->getSwapChain()->getSwapChainExtent().width;
		uint32_t height = this->renderer->getSwapChain()->getSwapChainExtent().height;

		RayTraceUbo ubo{};

		glm::vec3 lookFrom = glm::vec3(13.0f, 2.0f, 3.0f);
		glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 vup = glm::vec3(0.0f, 1.0f, 0.0f);
		
		float vfov = 20.0f;
		float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		float theta = glm::radians(vfov);
		float h = glm::tan(theta / 2.0f);
		float viewportHeight = 2.0f * h;
		float viewportWidth = aspectRatio * viewportHeight;

		glm::vec3 w = glm::normalize(lookFrom - lookAt);
		glm::vec3 u = glm::normalize(glm::cross(vup, w));
		glm::vec3 v = glm::cross(w, u);

		ubo.origin = lookFrom;
		ubo.horizontal = viewportWidth * u;
		ubo.vertical = viewportHeight * v;
		ubo.lowerLeftCorner = ubo.origin - ubo.horizontal / 2.0f + ubo.vertical / 2.0f - w;
		ubo.background = glm::vec3(0.52f, 0.8f, 0.91f);

		return ubo;
	}

	void EngineApp::recreateSubRendererAndSubsystem() {
		uint32_t nSample = 8;
		
		uint32_t width = this->renderer->getSwapChain()->getSwapChainExtent().width;
		uint32_t height = this->renderer->getSwapChain()->getSwapChainExtent().height;
		std::shared_ptr<EngineDescriptorPool> descriptorPool = this->renderer->getDescriptorPool();
		std::vector<std::shared_ptr<EngineImage>> swapChainImages = this->renderer->getSwapChain()->getswapChainImages();

		std::vector<VkDescriptorBufferInfo> buffersInfo { this->models->getObjectInfo(), this->models->getBvhInfo() };

		this->traceRayRender = std::make_unique<EngineTraceRayRenderSystem>(this->device, descriptorPool, 
			static_cast<uint32_t>(swapChainImages.size()), width, height, nSample, buffersInfo);

		this->samplingRayRender = std::make_unique<EngineSamplingRayRenderSystem>(this->device, descriptorPool, 
			this->traceRayRender->getDescSetLayout(), swapChainImages, width, height);
	}
}