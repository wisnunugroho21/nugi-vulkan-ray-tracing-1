#include "app.hpp"

#include "../camera/camera.hpp"
#include "../mouse_controller/mouse_controller.hpp"
#include "../keyboard_controller/keyboard_controller.hpp"
#include "../acceleration_structure/bottom_level_acceleration_structure.hpp"
#include "../acceleration_structure/top_level_acceleration_structure.hpp"
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
		this->renderer = std::make_unique<EngineRayTraceRenderer>(this->window, this->device);

		this->loadObjects();
		// this->loadQuadModels();
		this->recreateSubRendererAndSubsystem();
	}

	EngineApp::~EngineApp() {}

	void EngineApp::renderLoop() {
		while (this->isRendering) {
			if (this->renderer->acquireFrame()) {
				uint32_t frameIndex = this->renderer->getFrameIndex();
				uint32_t imageIndex = this->renderer->getImageIndex();

				auto commandBuffer = this->renderer->beginCommand();
				
				this->rayTracingRenderSystem->render(commandBuffer, imageIndex, this->renderer->getSwapChain()->getswapChainImages()[imageIndex]);

				this->renderer->endCommand(commandBuffer);
				this->renderer->submitCommand(commandBuffer);

				if (!this->renderer->presentFrame()) {
					this->recreateSubRendererAndSubsystem();
					this->randomSeed = 0;

					continue;
				}				

				if (frameIndex + 1 == EngineDevice::MAX_FRAMES_IN_FLIGHT) {
					this->randomSeed++;
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
		std::shared_ptr<EngineModel> flatVaseModel = EngineModel::createModelFromFile(this->device, "models/flat_vase.obj");

		auto flatVase = EngineGameObject::createSharedGameObject();
		flatVase->model = flatVaseModel;
		flatVase->transform.translation = {-0.5f, 0.5f, 0.0f};
		flatVase->transform.scale = {3.0f, 1.5f, 3.0f};
		flatVase->color = {1.0f, 1.0f, 1.0f};

		this->gameObjects.push_back(std::move(flatVase)); 

		std::shared_ptr<EngineModel> smoothVaseModel = EngineModel::createModelFromFile(this->device, "models/smooth_vase.obj");

		auto smoothVase = EngineGameObject::createSharedGameObject();
		smoothVase->model = smoothVaseModel;
		smoothVase->transform.translation = {0.5f, 0.5f, 0.0f};
		smoothVase->transform.scale = {3.0f, 1.5f, 3.0f};
		smoothVase->color = {1.0f, 1.0f, 1.0f};

		this->gameObjects.push_back(std::move(smoothVase));
	}

	/* void EngineApp::loadQuadModels() {
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
	} */

	void EngineApp::recreateSubRendererAndSubsystem() {
		std::vector<EngineTopLevelAccelerationStructure> topAccelStructs;

		for (auto &&obj : this->gameObjects) {
			EngineBottomLevelAccelerationStructure bottomStruct{this->device, deviceProcedure, obj->model};
			EngineTopLevelAccelerationStructure topStruct{this->device, deviceProcedure, bottomStruct};

			topAccelStructs.emplace_back(topStruct);
		}

		this->rayTracingRenderSystem = std::make_unique<EngineRayTracingRenderSystem>(
			this->device, this->deviceProcedure, *this->renderer->getDescriptorPool(), topAccelStructs, 
			this->renderer->getSwapChain()->imageCount(), this->renderer->getSwapChain()->width(), 
			this->renderer->getSwapChain()->height()
		);
	}
}