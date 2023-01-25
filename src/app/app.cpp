#include "app.hpp"

#include "bvh.hpp"

#include "../camera/camera.hpp"
#include "../mouse_controller/mouse_controller.hpp"
#include "../keyboard_controller/keyboard_controller.hpp"
#include "../buffer/buffer.hpp"
#include "../frame_info.hpp"
#include "../ray_ubo.hpp"

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

		RayTraceObject object = this->loadObjects();

		this->recreateSubRendererAndSubsystem(object);
	}

	EngineApp::~EngineApp() {}

	void EngineApp::run() {
		auto currentTime = std::chrono::high_resolution_clock::now();
		uint32_t t = 0;

		RayTraceUbo ubo = this->updateCamera();

		while (!this->window.shouldClose()) {
			this->window.pollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();

			if (t == 10) {
				std::string appTitle = std::string(APP_TITLE) + std::string(" | FPS: ") + std::to_string((1.0f / frameTime));
				glfwSetWindowTitle(this->window.getWindow(), appTitle.c_str());

				t = 0;
			} else {
				t++;
			}

			currentTime = newTime;

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
					RayTraceObject object = this->loadObjects();
					this->recreateSubRendererAndSubsystem(object);
				}
			}
		}

		vkDeviceWaitIdle(this->device.getLogicalDevice());
	}

	RayTraceObject EngineApp::loadObjects() {
		RayTraceObject objectBuffer{};

		glm::vec3 vertices[36] {
			{-.5f, -.5f, -.5f},
			{-.5f, .5f, .5f},
			{-.5f, -.5f, .5f},
			{-.5f, -.5f, -.5f},
			{-.5f, .5f, -.5f},
			{-.5f, .5f, .5f},
 
			// right face (yellow)
			{.5f, -.5f, -.5f},
			{.5f, .5f, .5f},
			{.5f, -.5f, .5f},
			{.5f, -.5f, -.5f},
			{.5f, .5f, -.5f},
			{.5f, .5f, .5f},
 
			// top face (orange, remember y axis points down)
			{-.5f, -.5f, -.5f},
			{.5f, -.5f, .5f},
			{-.5f, -.5f, .5f},
			{-.5f, -.5f, -.5f},
			{.5f, -.5f, -.5f},
			{.5f, -.5f, .5f},
 
			// bottom face (red)
			{-.5f, .5f, -.5f},
			{.5f, .5f, .5f},
			{-.5f, .5f, .5f},
			{-.5f, .5f, -.5f},
			{.5f, .5f, -.5f},
			{.5f, .5f, .5f},
 
			// nose face (blue)
			{-.5f, -.5f, 0.5f},
			{.5f, .5f, 0.5f},
			{-.5f, .5f, 0.5f},
			{-.5f, -.5f, 0.5f},
			{.5f, -.5f, 0.5f},
			{.5f, .5f, 0.5f},
 
			// tail face (green)
			{-.5f, -.5f, -0.5f},
			{.5f, .5f, -0.5f},
			{-.5f, .5f, -0.5f},
			{-.5f, -.5f, -0.5f},
			{.5f, -.5f, -0.5f},
			{.5f, .5f, -0.5f},
		};

		Triangle triangle[12]{};
		for (int i = 0; i < 12; i++) {
			objectBuffer.triangles[i].point0 = vertices[i * 3];
			objectBuffer.triangles[i].point1 = vertices[i * 3 + 1];
			objectBuffer.triangles[i].point2 = vertices[i * 3 + 2];
		}

		return objectBuffer;
	}

	RayTraceBvh buildBvh(Triangle triangles[500]) {
		std::vector<Object0> objects;
		for (uint32_t i = 0; i < 12; i++) {
			Triangle t = triangles[i];
			objects.push_back({i, t});
		}

		auto bvhNodes = createBvh(objects);
		RayTraceBvh traceBvh{};

		for (int i = 0; i < bvhNodes.size(); i++) {
			traceBvh.bvhNodes[i] = bvhNodes[i];
		}

		return traceBvh;
	}

	RayTraceUbo EngineApp::updateCamera() {
		uint32_t width = this->renderer->getSwapChain()->getSwapChainExtent().width;
		uint32_t height = this->renderer->getSwapChain()->getSwapChainExtent().height;

		RayTraceUbo ubo{};

		glm::vec3 lookFrom = glm::vec3(3.0f, 10.0f, 3.0f);
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

		return ubo;
	}

	void EngineApp::recreateSubRendererAndSubsystem(RayTraceObject object) {
		uint32_t nSample = 8;
		
		uint32_t width = this->renderer->getSwapChain()->getSwapChainExtent().width;
		uint32_t height = this->renderer->getSwapChain()->getSwapChainExtent().height;
		std::shared_ptr<EngineDescriptorPool> descriptorPool = this->renderer->getDescriptorPool();
		std::vector<std::shared_ptr<EngineImage>> swapChainImages = this->renderer->getSwapChain()->getswapChainImages();

		auto x = buildBvh(object.triangles);

		this->traceRayRender = std::make_unique<EngineTraceRayRenderSystem>(this->device, descriptorPool, 
			static_cast<uint32_t>(swapChainImages.size()), width, height, nSample, object, x);

		this->samplingRayRender = std::make_unique<EngineSamplingRayRenderSystem>(this->device, descriptorPool, 
			this->traceRayRender->getDescSetLayout(), swapChainImages, width, height);
	}
}