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
		this->renderer = std::make_unique<EngineDefferedRenderer>(this->window, this->device);

		this->loadObjects();
		this->loadQuadModels();
		this->recreateSubRendererAndSubsystem();
	}

	EngineApp::~EngineApp() {}

	void EngineApp::renderLoop() {
		auto viewObject = EngineGameObject::createGameObject();
		viewObject.transform.translation.z = -2.5f;

		EngineCamera camera{};
		EngineMouseController mouseController{};
		EngineKeyboardController keyboardController{};

		while (this->isRendering) {
			auto aspect = this->renderer->getSwapChain()->extentAspectRatio();

			camera.setViewYXZ(viewObject.transform.translation, viewObject.transform.rotation);
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);

			if (this->renderer->acquireFrame()) {
				uint32_t frameIndex = this->renderer->getFrameIndex();
				uint32_t imageIndex = this->renderer->getImageIndex();

				GlobalUBO ubo{};
				ubo.projection = camera.getProjectionMatrix();
				ubo.view = camera.getViewMatrix();
				ubo.inverseView = camera.getInverseViewMatrix();

				this->forwardPassRenderSystem->writeUniformBuffer(frameIndex, &ubo);

				auto commandBuffer = this->renderer->beginCommand();

				this->forwardPassSubRenderer->beginRenderPass(commandBuffer, imageIndex);
				this->forwardPassRenderSystem->render(commandBuffer, frameIndex, this->gameObjects);
				this->forwardPassSubRenderer->endRenderPass(commandBuffer);

				this->forwardPassSubRenderer->transferFrame(commandBuffer, imageIndex);

				this->swapChainSubRenderer->beginRenderPass(commandBuffer, imageIndex);
				this->defferedRenderSystem->render(commandBuffer, imageIndex, this->quadModelObjects);
				this->swapChainSubRenderer->endRenderPass(commandBuffer);				
								
				this->renderer->endCommand(commandBuffer);
				this->renderer->submitCommand(commandBuffer);
				
				if (!this->renderer->presentFrame()) {
					this->recreateSubRendererAndSubsystem();
				}			
			}
		}
	}

	void EngineApp::run() {
		auto currentTime = std::chrono::high_resolution_clock::now();
		uint32_t t = 0;

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
		std::shared_ptr<EngineModel> flatVaseModel = EngineModel::createModelFromFile(this->device, "models/flat_vase.obj", 0);

		auto flatVase = EngineGameObject::createSharedGameObject();
		flatVase->model = flatVaseModel;
		flatVase->transform.translation = {-0.5f, 0.5f, 0.0f};
		flatVase->transform.scale = {3.0f, 1.5f, 3.0f};

		this->gameObjects.push_back(std::move(flatVase)); 

		std::shared_ptr<EngineModel> smoothVaseModel = EngineModel::createModelFromFile(this->device, "models/smooth_vase.obj", 0);

		auto smoothVase = EngineGameObject::createSharedGameObject();
		smoothVase->model = smoothVaseModel;
		smoothVase->transform.translation = {0.5f, 0.5f, 0.0f};
		smoothVase->transform.scale = {3.0f, 1.5f, 3.0f};

		this->gameObjects.push_back(std::move(smoothVase));

		MaterialItem matItem { glm::vec3(1.0, 0.0, 0.0) };
		MaterialData materialData{};
		materialData.data[0] = matItem;

		this->materials = std::make_shared<EngineMaterial>(this->device, materialData);
	}

	void EngineApp::loadQuadModels() {
		ModelData modelData{};

		std::vector<Vertex> vertices;

		Vertex vertex1 { glm::vec3(-1.0f, -1.0f, 0.0f) };
		vertices.emplace_back(vertex1);

		Vertex vertex2 { glm::vec3(1.0f, -1.0f, 0.0f) };
		vertices.emplace_back(vertex2);

		Vertex vertex3 { glm::vec3(1.0f, 1.0f, 0.0f) };
		vertices.emplace_back(vertex3);

		Vertex vertex4 { glm::vec3(-1.0f, 1.0f, 0.0f) };
		vertices.emplace_back(vertex4);

		modelData.vertices = vertices;
		modelData.indices = {
			0, 1, 2, 2, 3, 0
		};

		auto quadObject = EngineGameObject::createSharedGameObject();
		quadObject->model = std::make_shared<EngineModel>(this->device, modelData);

		this->quadModelObjects.emplace_back(quadObject);
	}

	void EngineApp::recreateSubRendererAndSubsystem() {
		uint32_t nSample = 4;

		uint32_t width = this->renderer->getSwapChain()->width();
		uint32_t height = this->renderer->getSwapChain()->height();
		uint32_t imageCount = this->renderer->getSwapChain()->imageCount();
		auto imageFormat = this->renderer->getSwapChain()->getSwapChainImageFormat();

		auto descriptorPool = this->renderer->getDescriptorPool();
		auto swapChainImages = this->renderer->getSwapChain()->getswapChainImages();

		std::vector<VkDescriptorBufferInfo> buffersInfo = { this->materials->getMaterialInfo() };

		this->forwardPassSubRenderer = std::make_unique<EngineForwardPassSubRenderer>(this->device, imageCount, width, height);
		this->swapChainSubRenderer = std::make_unique<EngineSwapChainSubRenderer>(this->device, swapChainImages, imageFormat, imageCount, width, height);

		std::vector<std::vector<VkDescriptorImageInfo>> forwardPassResourcesInfo = { this->forwardPassSubRenderer->getAlbedoInfoResources(), this->forwardPassSubRenderer->getNormalInfoResources() };

		this->forwardPassRenderSystem = std::make_unique<EngineForwardPassRenderSystem>(this->device, this->forwardPassSubRenderer->getRenderPass()->getRenderPass(), descriptorPool, buffersInfo);
		this->defferedRenderSystem = std::make_unique<EngineDeffereRenderSystem>(this->device, descriptorPool, width, height, 
			this->swapChainSubRenderer->getRenderPass()->getRenderPass(), forwardPassResourcesInfo);
	}
}