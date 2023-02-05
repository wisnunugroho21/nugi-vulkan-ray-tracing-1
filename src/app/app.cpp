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

#include <thread>

namespace nugiEngine {
	EngineApp::EngineApp() {
		this->renderer = std::make_unique<EngineHybridRenderer>(this->window, this->device);

		this->loadObjects();
		this->loadQuadModels();
		this->recreateSubRendererAndSubsystem();
	}

	EngineApp::~EngineApp() {}

	void EngineApp::renderLoop() {
		while (this->isRendering) {
			if (this->renderer->acquireFrame()) {
				uint32_t imageIndex = this->renderer->getImageIndex();
				uint32_t frameIndex = this->renderer->getFrameIndex();

				if (!this->traceRayRender->isFrameUpdated[frameIndex]) {
					this->traceRayRender->writeGlobalData(frameIndex, this->globalUbo);
					this->traceRayRender->isFrameUpdated[frameIndex] = true;
				}

				auto commandBuffer = this->renderer->beginCommand();
				this->traceRayRender->prepareFrame(commandBuffer, frameIndex);

				this->traceRayRender->render(commandBuffer, frameIndex, this->randomSeed);
				this->traceRayRender->transferFrame(commandBuffer, frameIndex);
				
				this->swapChainSubRenderer->beginRenderPass(commandBuffer, imageIndex);
				this->samplingRayRender->render(commandBuffer, frameIndex, this->quadModels, this->randomSeed);
				this->swapChainSubRenderer->endRenderPass(commandBuffer);

				this->traceRayRender->finishFrame(commandBuffer, frameIndex);				

				this->renderer->endCommand(commandBuffer);
				this->renderer->submitCommand(commandBuffer);

				if (!this->renderer->presentFrame()) {
					this->recreateSubRendererAndSubsystem();
					this->randomSeed = 0;

					continue;
				}

				if (this->randomSeed >= 50) {
					this->randomSeed = 0;
				} else {
					this->randomSeed++;
				}
			}
		}
	}

	void EngineApp::run() {
		auto currentTime = std::chrono::high_resolution_clock::now();
		uint32_t t = 0;

		this->globalUbo = this->updateCamera();

		if (!this->traceRayRender->isFrameUpdated[0]) {
			this->traceRayRender->writeGlobalData(0, this->globalUbo);
			this->traceRayRender->isFrameUpdated[0] = true;
		}

		std::thread renderThread(&EngineApp::renderLoop, std::ref(*this));

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
		}

		this->isRendering = false;
		renderThread.join();

		vkDeviceWaitIdle(this->device.getLogicalDevice());
	}

	void EngineApp::loadObjects() {
		RayTraceModelData modeldata{};

		modeldata.triangles.emplace_back(Triangle{ glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 555.0f}, 1, 1 });
		modeldata.triangles.emplace_back(Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 0.0f}, 1, 1 });

		modeldata.triangles.emplace_back(Triangle{glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 555.0f, 0.0f}, glm::vec3{0.0f, 555.0f, 555.0f}, 1, 2});
		modeldata.triangles.emplace_back(Triangle{ glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, 1, 2 }); 

		modeldata.triangles.emplace_back(Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 0.0f, 555.0f}, 1, 0 });
		modeldata.triangles.emplace_back(Triangle{ glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, 1, 0 }); 

		modeldata.triangles.emplace_back(Triangle{ glm::vec3{0.0f, 555.0f, 0.0f,}, glm::vec3{555.0f, 555.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 555.0f}, 1, 0 });
		modeldata.triangles.emplace_back(Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 0.0f}, 1, 0 });  

		modeldata.triangles.emplace_back(Triangle{ glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 555.0f, 555.0f}, 1, 0 });
		modeldata.triangles.emplace_back(Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f}, 1, 0 }); 

		modeldata.triangles.emplace_back(Triangle{ glm::vec3{213.0f, 554.0f, 227.0f}, glm::vec3{343.0f, 554.0f, 227.0f}, glm::vec3{343.0f, 554.0f, 332.0f}, 0, 0 });
		modeldata.triangles.emplace_back(Triangle{ glm::vec3{343.0f, 554.0f, 332.0f}, glm::vec3{213.0f, 554.0f, 332.0f}, glm::vec3{213.0f, 554.0f, 227.0f}, 0, 0 });

		modeldata.lambertians.emplace_back(Lambertian{ glm::vec3(1.0f, 1.0f, 1.0f) });
		modeldata.lambertians.emplace_back(Lambertian{ glm::vec3(0.12f, 0.45f, 0.15f) });
		modeldata.lambertians.emplace_back(Lambertian{ glm::vec3(0.65f, 0.05f, 0.05f) });

		Light light{glm::vec3(10.0f, 10.0f, 10.0f)};
		modeldata.lights.emplace_back(light);

		this->models = std::make_unique<EngineRayTraceModel>(this->device, modeldata);
	}

	void EngineApp::loadQuadModels() {
		ModelData modelData{};

		std::vector<Vertex> vertices;

		Vertex vertex1 { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec2(0.0f) };
		vertices.emplace_back(vertex1);

		Vertex vertex2 { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec2(0.0f) };
		vertices.emplace_back(vertex2);

		Vertex vertex3 { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec2(0.0f) };
		vertices.emplace_back(vertex3);

		Vertex vertex4 { glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec2(0.0f) };
		vertices.emplace_back(vertex4);

		modelData.vertices = vertices;
		modelData.indices = {
			0, 1, 2, 2, 3, 0
		};

		this->quadModels = std::make_shared<EngineModel>(this->device, modelData);
	}

	RayTraceUbo EngineApp::updateCamera() {
		uint32_t width = this->renderer->getSwapChain()->getSwapChainExtent().width;
		uint32_t height = this->renderer->getSwapChain()->getSwapChainExtent().height;

		RayTraceUbo ubo{};

		glm::vec3 lookFrom = glm::vec3(278.0f, 278.0f, -800.0f);
		glm::vec3 lookAt = glm::vec3(278.0f, 278.0f, 0.0f);
		glm::vec3 vup = glm::vec3(0.0f, 1.0f, 0.0f);
		
		float vfov = 40.0f;
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
		ubo.background = glm::vec3(0.0f, 0.0f, 0.0f);

		return ubo;
	}

	void EngineApp::recreateSubRendererAndSubsystem() {
		uint32_t nSample = 3;

		uint32_t width = this->renderer->getSwapChain()->width();
		uint32_t height = this->renderer->getSwapChain()->height();
		std::shared_ptr<EngineDescriptorPool> descriptorPool = this->renderer->getDescriptorPool();
		std::vector<std::shared_ptr<EngineImage>> swapChainImages = this->renderer->getSwapChain()->getswapChainImages();

		this->swapChainSubRenderer = std::make_unique<EngineSwapChainSubRenderer>(this->device, this->renderer->getSwapChain()->getswapChainImages(), 
			this->renderer->getSwapChain()->getSwapChainImageFormat(), this->renderer->getSwapChain()->imageCount(), 
			width, height);

		std::vector<VkDescriptorBufferInfo> buffersInfo { this->models->getObjectInfo(), this->models->getBvhInfo(), this->models->getMaterialInfo(), this->models->getLightInfo() };

		this->traceRayRender = std::make_unique<EngineTraceRayRenderSystem>(this->device, descriptorPool, 
			width, height, nSample, buffersInfo);

		this->samplingRayRender = std::make_unique<EngineSamplingRayRasterRenderSystem>(this->device, 
			this->renderer->getDescriptorPool(), width, height, this->traceRayRender->getStorageImages(), 
			nSample, this->swapChainSubRenderer->getRenderPass()->getRenderPass());
	}
}