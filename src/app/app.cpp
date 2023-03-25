#include "app.hpp"

#include "../camera/camera.hpp"
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
		this->loadObjects();
		this->loadQuadModels();
		this->createDescriptor();

		this->renderer = std::make_unique<EngineDefferedRenderer>(this->window, this->device);
		this->recreateSubRendererAndSubsystem();
	}

	EngineApp::~EngineApp() {}

	void EngineApp::renderLoop() {
		auto viewObject = EngineGeometry::createGeometry();

		EngineCamera camera{};

		while (this->isRendering) {
			auto aspect = this->renderer->getSwapChain()->extentAspectRatio();

			// camera.setViewYXZ(viewObject.transform.translation, viewObject.transform.rotation);
			camera.setViewDirection({ 278.0f, 278.0f, -800.0f }, { 0.0f, 0.0f, 800.0f }, { 0.0f, 1.0f, 0.0f });
			camera.setPerspectiveProjection(glm::radians(40.0f), aspect, 0.1f, 2000.0f);

			if (this->renderer->acquireFrame()) {
				uint32_t frameIndex = this->renderer->getFrameIndex();
				uint32_t imageIndex = this->renderer->getImageIndex();

				RasterUBO ubo{};
				ubo.projection = camera.getProjectionMatrix();
				ubo.view = camera.getViewMatrix();
				ubo.realCameraPos = camera.getRealCameraPos();
				this->globalDescSet->writeGlobalBuffer(frameIndex, &ubo);

				std::vector<VkDescriptorSet> forwardPassDescSets = { *this->globalDescSet->getDescriptorSets(frameIndex), *this->forwardModelDescSet->getDescriptorSets(frameIndex) };
				std::vector<VkDescriptorSet> forwardLightDescSets = { *this->globalDescSet->getDescriptorSets(frameIndex) };
				std::vector<VkDescriptorSet> deferredDescSets = { *this->globalDescSet->getDescriptorSets(frameIndex), *this->forwardOutputDescSet->getDescriptorSets(frameIndex) };

				auto commandBuffer = this->renderer->beginCommand();

				this->forwardPassSubRenderer->beginRenderPass(commandBuffer, imageIndex);
				this->forwardPassRenderSystem->render(commandBuffer, frameIndex, forwardPassDescSets, this->gameObject);
				this->forwardLightRenderSystem->render(commandBuffer, frameIndex, forwardLightDescSets, this->lightObject->getNumLight());
				this->forwardPassSubRenderer->endRenderPass(commandBuffer);

				this->forwardPassSubRenderer->transferFrame(commandBuffer, imageIndex);

				this->swapChainSubRenderer->beginRenderPass(commandBuffer, imageIndex);
				this->deferredRenderSystem->render(commandBuffer, imageIndex, deferredDescSets, this->quadModelObject, this->randomSeed);
				this->swapChainSubRenderer->endRenderPass(commandBuffer);				
								
				this->renderer->endCommand(commandBuffer);
				this->renderer->submitCommand(commandBuffer);
				
				if (!this->renderer->presentFrame()) {
					this->recreateSubRendererAndSubsystem();
					this->createDescriptor();
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

	/* void EngineApp::loadObjects() {
		std::shared_ptr<EngineRasterModel> roomModel = EngineRasterModel::createModelFromFile(this->device, "models/CornellBox.obj", 0);

		auto roomObject = EngineGeometry::createSharedGeometry();
		roomObject->model = roomModel;
		roomObject->transform.translation = {0.0f, 0.0f, 0.0f};
		roomObject->transform.scale = {1.0f, 1.0f, 1.0f};
		roomObject->transform.rotation = {0.0f, 0.0f, 0.0f};

		this->gameObjects.emplace_back(std::move(roomObject));

		MaterialItem matItem { glm::vec3(1.0, 0.0, 0.0) };
		MaterialData materialData{};
		materialData.data[0] = matItem;

		auto pointLight = EngineLight::createSharedLight();
		pointLight->color = glm::vec3(1.0f);
		pointLight->intensity = 1.0f;
		pointLight->position = glm::vec3(0.0f, 1.0f, 0.0f);
		pointLight->radius = 0.1f;

		this->lightObjects.emplace_back(std::move(pointLight));

		this->materials = std::make_shared<EngineMaterial>(this->device, materialData);
	} */

	void EngineApp::loadObjects() {
		RayTraceModelData modeldata{};

		std::vector<Model> models{};

		models.emplace_back(Model{ Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 555.0f, 0.0f}, glm::vec3{0.0f, 555.0f, 555.0f} }, glm::vec3(1.0f, 0.0f, 0.0f), 2, 0 });
		models.emplace_back(Model{ Triangle{ glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 0.0f} }, glm::vec3(1.0f, 0.0f, 0.0f), 2, 0 });

		models.emplace_back(Model{ Triangle{ glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 555.0f} }, glm::vec3(-1.0f, 0.0f, 0.0f), 1, 0 });
		models.emplace_back(Model{ Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 0.0f} }, glm::vec3(-1.0f, 0.0f, 0.0f), 1, 0 });

		models.emplace_back(Model{ Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 0.0f, 555.0f} }, glm::vec3(0.0f, 1.0f, 0.0f), 0, 0 });
		models.emplace_back(Model{ Triangle{ glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 0.0f} }, glm::vec3(0.0f, 1.0f, 0.0f), 0, 0 });

		models.emplace_back(Model{ Triangle{ glm::vec3{0.0f, 555.0f, 0.0f,}, glm::vec3{555.0f, 555.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 555.0f} }, glm::vec3(0.0f, -1.0f, 0.0f), 0, 0 });
		models.emplace_back(Model{ Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 0.0f} }, glm::vec3(0.0f, -1.0f, 0.0f), 0, 0 });

		models.emplace_back(Model{ Triangle{ glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 555.0f, 555.0f} }, glm::vec3(0.0f, 0.0f, -1.0f), 0, 0 });
		models.emplace_back(Model{ Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f} }, glm::vec3(0.0f, 0.0f, -1.0f), 0, 0 });

		modeldata.objects = models;
		this->gameObject = EngineGeometry::createSharedGeometry(this->device, modeldata);

		// ----------------------------------------------------------------------------

		MaterialData materialData{};

		MaterialItem matItem1 { glm::vec3(0.73f, 0.73f, 0.73f), 1.0f, 0.001f, 0.5f };
		materialData.data[0] = matItem1;

		MaterialItem matItem2 { glm::vec3(0.65f, 0.05f, 0.05f), 0.0f, 0.0f, 0.0f };
		materialData.data[1] = matItem2;

		MaterialItem matItem3 { glm::vec3(0.12f, 0.45f, 0.15f), 0.0f, 0.0f, 0.0f };
		materialData.data[2] = matItem3;

		this->materials = std::make_shared<EngineMaterial>(this->device, materialData);

		// ----------------------------------------------------------------------------

		std::vector<PointLight> pointLights{};

		PointLight pointLight{};
		pointLight.sphere.center = glm::vec3(277.5f, 500.0f, 277.5f);
		pointLight.sphere.radius = 10.0f;
		pointLight.color = glm::vec3(1.0f);

		pointLights.emplace_back(pointLight);
		this->lightObject = EngineLight::createSharedLight(this->device, pointLights);

		// ----------------------------------------------------------------------------		

		std::vector<TransformComponent> transformComponents{};

		TransformComponent transformComponent{};
		transformComponent.rotation = glm::vec3(0.0f);
		transformComponent.scale = glm::vec3(1.0f);
		transformComponent.translation = glm::vec3(0.0f);

		transformComponents.emplace_back(transformComponent);
		this->transform = EngineTransform::createSharedFromTransformComponent(this->device, transformComponents);
	}

	void EngineApp::loadQuadModels() {
		RasterModelData modelData{};

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

		this->quadModelObject = EngineGeometry::createSharedGeometry();
		this->quadModelObject->rasterModel = std::make_shared<EngineRasterModel>(this->device, modelData);
	}

	void EngineApp::createDescriptor() {
		auto descriptorPool = this->renderer->getDescriptorPool();

		VkDescriptorBufferInfo globalBufferInfo[3] = { this->lightObject->getModelInfo(), this->gameObject->rayTraceModel->getModelInfo(), this->gameObject->rayTraceModel->getBvhInfo() };
		VkDescriptorBufferInfo forwardModelBuffersInfo[2] = { this->materials->getMaterialInfo(), this->transform->getTransformInfo() };
		std::vector<VkDescriptorImageInfo> forwardOutputBuffersInfo[4] = { 
			this->forwardPassSubRenderer->getPositionInfoResources(), 
			this->forwardPassSubRenderer->getAlbedoInfoResources(), 
			this->forwardPassSubRenderer->getNormalInfoResources(),
			this->forwardPassSubRenderer->getMaterialInfoResources()
		};
		
		this->globalDescSet = std::make_shared<EngineGlobalDescSet>(this->device, descriptorPool, globalBufferInfo);
		this->forwardModelDescSet = std::make_shared<EngineForwardModelDescSet>(this->device, descriptorPool, forwardModelBuffersInfo);
		this->forwardOutputDescSet = std::make_shared<EngineForwardOutputDescSet>(this->device, descriptorPool, forwardOutputBuffersInfo);
	}

	void EngineApp::recreateSubRendererAndSubsystem() {
		uint32_t nSample = 4;

		uint32_t width = this->renderer->getSwapChain()->width();
		uint32_t height = this->renderer->getSwapChain()->height();
		uint32_t imageCount = this->renderer->getSwapChain()->imageCount();

		auto imageFormat = this->renderer->getSwapChain()->getSwapChainImageFormat();
		auto swapChainImages = this->renderer->getSwapChain()->getswapChainImages();

		this->forwardPassSubRenderer = std::make_unique<EngineForwardPassSubRenderer>(this->device, imageCount, width, height);
		this->swapChainSubRenderer = std::make_unique<EngineSwapChainSubRenderer>(this->device, swapChainImages, imageFormat, imageCount, width, height);

		auto forwardRenderPass = this->forwardPassSubRenderer->getRenderPass()->getRenderPass();
		auto swapChainRenderPass = this->swapChainSubRenderer->getRenderPass()->getRenderPass();

		std::vector<VkDescriptorSetLayout> forwardPassDescLayouts = { this->globalDescSet->getDescSetLayout()->getDescriptorSetLayout(), this->forwardModelDescSet->getDescSetLayout()->getDescriptorSetLayout() };
		std::vector<VkDescriptorSetLayout> forwardLightDescLayouts = {  this->globalDescSet->getDescSetLayout()->getDescriptorSetLayout() };
		std::vector<VkDescriptorSetLayout> deferredDescLayouts = { this->globalDescSet->getDescSetLayout()->getDescriptorSetLayout(), this->forwardOutputDescSet->getDescSetLayout()->getDescriptorSetLayout() };

		this->forwardPassRenderSystem = std::make_unique<EngineForwardPassRenderSystem>(this->device, forwardRenderPass, forwardPassDescLayouts);
		this->forwardLightRenderSystem = std::make_unique<EngineForwardLightRenderSystem>(this->device, forwardRenderPass, forwardLightDescLayouts);
		this->deferredRenderSystem = std::make_unique<EngineDeffereRenderSystem>(this->device, swapChainRenderPass, deferredDescLayouts);
	}
}