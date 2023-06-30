#include "app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>
#include <chrono>
#include <iostream>
#include <cstdlib>

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
				uint32_t frameIndex = this->renderer->getFrameIndex();
				uint32_t imageIndex = this->renderer->getImageIndex();

				this->globalUniforms->writeGlobalData(frameIndex, this->globalUbo);

				auto commandBuffer = this->renderer->beginCommand();
				this->rayTraceImage->prepareFrame(commandBuffer, frameIndex);

				this->traceRayRender->render(commandBuffer, this->rayTraceDescSet->getDescriptorSets(frameIndex), this->randomSeed);

				this->rayTraceImage->transferFrame(commandBuffer, frameIndex);
				this->accumulateImages->prepareFrame(commandBuffer, frameIndex);
				
				this->swapChainSubRenderer->beginRenderPass(commandBuffer, imageIndex);
				this->samplingRayRender->render(commandBuffer, this->samplingDescSet->getDescriptorSets(frameIndex), this->quadModels, this->randomSeed);
				this->swapChainSubRenderer->endRenderPass(commandBuffer);

				this->rayTraceImage->finishFrame(commandBuffer, frameIndex);
				this->accumulateImages->finishFrame(commandBuffer, frameIndex);

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

		// this->globalUniforms->writeGlobalData(0, this->globalUbo);
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
		this->primitiveModel = std::make_unique<EnginePrimitiveModel>(this->device);

		auto objects = std::make_shared<std::vector<Object>>();
		auto materials = std::make_shared<std::vector<Material>>();
		auto lights = std::make_shared<std::vector<PointLight>>();
		auto vertices = std::make_shared<std::vector<RayTraceVertex>>();

		std::vector<std::shared_ptr<BoundBox>> boundBoxes{};
		std::vector<std::shared_ptr<TransformComponent>> transforms{};

		// ----------------------------------------------------------------------------

		// kanan
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		uint32_t transformIndex = static_cast<uint32_t>(transforms.size() - 1);
		
		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		uint32_t objectIndex = static_cast<uint32_t>(objects->size() - 1);

		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 555.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f} });

		auto rightWallPrimitives = std::make_shared<std::vector<Primitive>>();
		rightWallPrimitives->emplace_back(Primitive{ glm::uvec3(0u, 1u, 2u), 1u });
		rightWallPrimitives->emplace_back(Primitive{ glm::uvec3(2u, 3u, 0u), 1u });

		this->primitiveModel->addPrimitive(rightWallPrimitives, vertices);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], rightWallPrimitives, transforms[transformIndex], vertices }));
		uint32_t boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------
		
		// kiri
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 555.0f, 0.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f} });
		
		auto leftWallPrimitives = std::make_shared<std::vector<Primitive>>();
		leftWallPrimitives->emplace_back(Primitive{ glm::uvec3(4u, 5u, 6u), 2u });
		leftWallPrimitives->emplace_back(Primitive{ glm::uvec3(6u, 7u, 4u), 2u });
		
		this->primitiveModel->addPrimitive(leftWallPrimitives, vertices);
		
		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], leftWallPrimitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------
		
		// bawah
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto bottomWallPrimitives = std::make_shared<std::vector<Primitive>>();
		bottomWallPrimitives->emplace_back(Primitive{ glm::uvec3(4u, 0u, 3u), 0u });
		bottomWallPrimitives->emplace_back(Primitive{ glm::uvec3(3u, 7u, 4u), 0u });
		
		this->primitiveModel->addPrimitive(bottomWallPrimitives, vertices);
		
		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], bottomWallPrimitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------
		
		// atas
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto topWallPrimitives = std::make_shared<std::vector<Primitive>>();
		topWallPrimitives->emplace_back(Primitive{ glm::uvec3(5u, 1u, 2u), 0u });
		topWallPrimitives->emplace_back(Primitive{ glm::uvec3(2u, 6u, 5u), 0u });

		this->primitiveModel->addPrimitive(topWallPrimitives, vertices);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], topWallPrimitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------
		
		// depan
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto frontWallPrimitives = std::make_shared<std::vector<Primitive>>();
		frontWallPrimitives->emplace_back(Primitive{ glm::uvec3(7u, 6u, 2u), 0u });
		frontWallPrimitives->emplace_back(Primitive{ glm::uvec3(2u, 3u, 7u), 0u });

		this->primitiveModel->addPrimitive(frontWallPrimitives, vertices);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], frontWallPrimitives, transforms[transformIndex], vertices }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin();

		// ----------------------------------------------------------------------------

		/* transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(300.0f, 200.0f, 200.0f), glm::vec3(200.0f), glm::vec3(0.0f, glm::radians(180.0f), 0.0f)}));
		transformIndex = static_cast<uint32_t>(transforms.size() - 1);

		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		objectIndex = static_cast<uint32_t>(objects->size() - 1);

		auto flatVasePrimitives = this->primitiveModel->createPrimitivesFromFile(this->device, "models/viking_room.obj", 3u);
		this->primitiveModel->addPrimitive(flatVasePrimitives);

		boundBoxes.emplace_back(std::make_shared<ObjectBoundBox>(ObjectBoundBox{ static_cast<uint32_t>(boundBoxes.size() + 1), (*objects)[objectIndex], flatVasePrimitives, transforms[transformIndex] }));
		boundBoxIndex = static_cast<uint32_t>(boundBoxes.size() - 1);

		transforms[transformIndex]->objectMaximum = boundBoxes[boundBoxIndex]->getOriginalMax();
		transforms[transformIndex]->objectMinimum = boundBoxes[boundBoxIndex]->getOriginalMin(); */

		// ----------------------------------------------------------------------------

		materials->emplace_back(Material{ glm::vec3(0.73f, 0.73f, 0.73f), 0.0f, 0.1f, 0.5f, 0 });
		materials->emplace_back(Material{ glm::vec3(0.12f, 0.45f, 0.15f), 0.0f, 0.1f, 0.5f, 0 });
		materials->emplace_back(Material{ glm::vec3(0.65f, 0.05f, 0.05f), 0.0f, 0.1f, 0.5f, 0 });
		materials->emplace_back(Material{ glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.1f, 0.5f, 1 });

		// ----------------------------------------------------------------------------

		vertices->emplace_back(RayTraceVertex{ glm::vec3{213.0f, 554.0f, 227.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{343.0f, 554.0f, 227.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{343.0f, 554.0f, 332.0f}, glm::vec3{0.0f} });
		vertices->emplace_back(RayTraceVertex{ glm::vec3{213.0f, 554.0f, 332.0f}, glm::vec3{0.0f} });

		lights->emplace_back(PointLight{ glm::vec3(277.5f, 540.0f, 277.5f), 10.0f, glm::vec3(100.0f, 100.0f, 100.0f) });

		// ----------------------------------------------------------------------------

		this->objectModel = std::make_unique<EngineObjectModel>(this->device, objects, boundBoxes);
		this->materialModel = std::make_unique<EngineMaterialModel>(this->device, materials);
		this->lightModel = std::make_unique<EnginePointLightModel>(this->device, lights);
		this->transformationModel = std::make_unique<EngineTransformationModel>(this->device, transforms);
		this->rayTraceVertexModels = std::make_unique<EngineRayTraceVertexModel>(this->device, vertices);

		this->globalUniforms = std::make_unique<EngineGlobalUniform>(this->device);
		this->primitiveModel->createBuffers();

		this->textures.emplace_back(std::make_unique<EngineTexture>(this->device, "textures/viking_room.png"));
		this->numLights = lights->size();
	}

	void EngineApp::loadQuadModels() {
		VertexModelData modelData{};

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

		this->quadModels = std::make_shared<EngineVertexModel>(this->device, modelData);
	}

	RayTraceUbo EngineApp::updateCamera(uint32_t width, uint32_t height) {
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

		ubo.origin = glm::vec3(lookFrom);
		ubo.horizontal = glm::vec3(viewportWidth * u);
		ubo.vertical = glm::vec3(viewportHeight * v);
		ubo.lowerLeftCorner = glm::vec3(lookFrom - viewportWidth * u / 2.0f + viewportHeight * v / 2.0f - w);
		ubo.background = glm::vec3(0.0f);
		ubo.numLights = this->numLights;

		return ubo;
	}

	void EngineApp::recreateSubRendererAndSubsystem() {
		uint32_t width = this->renderer->getSwapChain()->width();
		uint32_t height = this->renderer->getSwapChain()->height();

		this->globalUbo = this->updateCamera(width, height);

		std::shared_ptr<EngineDescriptorPool> descriptorPool = this->renderer->getDescriptorPool();
		std::vector<std::shared_ptr<EngineImage>> swapChainImages = this->renderer->getSwapChain()->getswapChainImages();

		this->swapChainSubRenderer = std::make_unique<EngineSwapChainSubRenderer>(this->device, this->renderer->getSwapChain()->getswapChainImages(), 
			this->renderer->getSwapChain()->getSwapChainImageFormat(), static_cast<int>(this->renderer->getSwapChain()->imageCount()), 
			width, height);

		this->rayTraceImage = std::make_unique<EngineRayTraceImage>(this->device, width, height, static_cast<uint32_t>(this->renderer->getSwapChain()->imageCount()));
		this->accumulateImages = std::make_unique<EngineAccumulateImage>(this->device, width, height, static_cast<uint32_t>(this->renderer->getSwapChain()->imageCount()));

		VkDescriptorBufferInfo buffersInfo[9] { 
			this->objectModel->getObjectInfo(), 
			this->objectModel->getBvhInfo(),
			this->primitiveModel->getPrimitiveInfo(), 
			this->primitiveModel->getBvhInfo(),
			this->rayTraceVertexModels->getVertexnfo(),
			this->lightModel->getLightInfo(),
			this->lightModel->getBvhInfo(),
			this->materialModel->getMaterialInfo(),
			this->transformationModel->getTransformationInfo() 
		};

		std::vector<VkDescriptorImageInfo> imagesInfo[2] {
			this->rayTraceImage->getImagesInfo(),
			this->accumulateImages->getImagesInfo()
		};

		std::vector<VkDescriptorImageInfo> texturesInfo{};
		for (int i = 0; i < this->textures.size(); i++) {
			texturesInfo.emplace_back(this->textures[i]->getDescriptorInfo());
		}

		this->rayTraceDescSet = std::make_unique<EngineRayTraceDescSet>(this->device, this->renderer->getDescriptorPool(), this->globalUniforms->getBuffersInfo(), this->rayTraceImage->getImagesInfo(), buffersInfo, texturesInfo);
		this->samplingDescSet = std::make_unique<EngineSamplingDescSet>(this->device, this->renderer->getDescriptorPool(), imagesInfo);

		this->traceRayRender = std::make_unique<EngineTraceRayRenderSystem>(this->device, this->rayTraceDescSet->getDescSetLayout()->getDescriptorSetLayout(), width, height, 1);
		this->samplingRayRender = std::make_unique<EngineSamplingRayRasterRenderSystem>(this->device, this->samplingDescSet->getDescSetLayout()->getDescriptorSetLayout(), this->swapChainSubRenderer->getRenderPass()->getRenderPass());
	}
}