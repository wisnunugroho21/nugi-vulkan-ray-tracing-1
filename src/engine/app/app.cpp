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

				this->rayTraceUniforms->writeGlobalData(frameIndex, this->rayTraceUbo);
				this->rasterUniform->writeGlobalData(frameIndex, this->rasterUbo);

				auto commandBuffer = this->renderer->beginCommand();

				this->forwardPassSubRenderer->beginRenderPass(commandBuffer, frameIndex);
				this->forwardPassRender->render(commandBuffer, this->forwardPassDescSet->getDescriptorSets(frameIndex), this->vertexModels);
				this->forwardPassSubRenderer->endRenderPass(commandBuffer);

				this->forwardPassSubRenderer->transferFrame(commandBuffer, frameIndex);
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
				this->renderer->submitRenderCommand(commandBuffer);

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

		// this->rayTraceUniforms->writeGlobalData(0, this->globalUbo);
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
		auto pointlights = std::make_shared<std::vector<PointLight>>();
		auto vertices = std::make_shared<std::vector<Vertex>>();
		auto indices = std::make_shared<std::vector<uint32_t>>();

		std::vector<std::shared_ptr<BoundBox>> boundBoxes{};
		std::vector<std::shared_ptr<TransformComponent>> transforms{};

		// ----------------------------------------------------------------------------

		// kanan
		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));
		uint32_t transformIndex = static_cast<uint32_t>(transforms.size() - 1);
		
		objects->emplace_back(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		uint32_t objectIndex = static_cast<uint32_t>(objects->size() - 1);

		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 0.0f, 0.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{-1.0f, 0.0f, 0.0f, 0.0f}, 1u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 555.0f, 0.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{-1.0f, 0.0f, 0.0f, 0.0f}, 1u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 555.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{-1.0f, 0.0f, 0.0f, 0.0f}, 1u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 0.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{-1.0f, 0.0f, 0.0f, 0.0f}, 1u, transformIndex });

		auto rightWallPrimitives = std::make_shared<std::vector<Primitive>>();
		rightWallPrimitives->emplace_back(Primitive{ glm::uvec3(0u, 1u, 2u) });
		rightWallPrimitives->emplace_back(Primitive{ glm::uvec3(2u, 3u, 0u) });

		indices->emplace_back(0u);
		indices->emplace_back(1u);
		indices->emplace_back(2u);
		indices->emplace_back(2u);
		indices->emplace_back(3u);
		indices->emplace_back(0u);

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

		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 0.0f}, 2u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 555.0f, 0.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 0.0f}, 2u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 555.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 0.0f}, 2u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 0.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{1.0f, 0.0f, 0.0f, 0.0f}, 2u, transformIndex });
		
		auto leftWallPrimitives = std::make_shared<std::vector<Primitive>>();
		leftWallPrimitives->emplace_back(Primitive{ glm::uvec3(4u, 5u, 6u) });
		leftWallPrimitives->emplace_back(Primitive{ glm::uvec3(6u, 7u, 4u) });

		indices->emplace_back(4u);
		indices->emplace_back(5u);
		indices->emplace_back(6u);
		indices->emplace_back(6u);
		indices->emplace_back(7u);
		indices->emplace_back(4u);
		
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

		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 0.0f, 0.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 0.0f, 0.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 0.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 0.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, 1.0f, 0.0f, 0.0f}, 0u, transformIndex });

		auto bottomWallPrimitives = std::make_shared<std::vector<Primitive>>();
		bottomWallPrimitives->emplace_back(Primitive{ glm::uvec3(8u, 9u, 10u) });
		bottomWallPrimitives->emplace_back(Primitive{ glm::uvec3(10u, 11u, 8u) });

		indices->emplace_back(8u);
		indices->emplace_back(9u);
		indices->emplace_back(10u);
		indices->emplace_back(10u);
		indices->emplace_back(11u);
		indices->emplace_back(8u);
		
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

		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 555.0f, 0.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, -1.0f, 0.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 555.0f, 0.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, -1.0f, 0.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 555.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, -1.0f, 0.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 555.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, -1.0f, 0.0f, 0.0f}, 0u, transformIndex });

		auto topWallPrimitives = std::make_shared<std::vector<Primitive>>();
		topWallPrimitives->emplace_back(Primitive{ glm::uvec3(12u, 13u, 14u) });
		topWallPrimitives->emplace_back(Primitive{ glm::uvec3(14u, 15u, 12u) });

		indices->emplace_back(12u);
		indices->emplace_back(13u);
		indices->emplace_back(14u);
		indices->emplace_back(14u);
		indices->emplace_back(15u);
		indices->emplace_back(12u);

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

		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 0.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, 0.0f, -1.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{0.0f, 555.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, 0.0f, -1.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 555.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, 0.0f, -1.0f, 0.0f}, 0u, transformIndex });
		vertices->emplace_back(Vertex{ glm::vec4{555.0f, 0.0f, 555.0f, 1.0f}, glm::vec4{0.0f}, glm::vec4{0.0f, 0.0f, -1.0f, 0.0f}, 0u, transformIndex });

		auto frontWallPrimitives = std::make_shared<std::vector<Primitive>>();
		frontWallPrimitives->emplace_back(Primitive{ glm::uvec3(16u, 17u, 18u) });
		frontWallPrimitives->emplace_back(Primitive{ glm::uvec3(18u, 19u, 16u) });

		indices->emplace_back(16u);
		indices->emplace_back(17u);
		indices->emplace_back(18u);
		indices->emplace_back(18u);
		indices->emplace_back(19u);
		indices->emplace_back(16u);

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

		materials->emplace_back(Material{ glm::vec3(186.0f, 186.0f, 186.0f), 0.0f, 0.1f, 0.5f, 0 });
		materials->emplace_back(Material{ glm::vec3(30.0f, 115.0f, 38.0f), 0.0f, 0.1f, 0.5f, 0 });
		materials->emplace_back(Material{ glm::vec3(167.0f, 13.0f, 13.0f), 0.0f, 0.1f, 0.5f, 0 });
		materials->emplace_back(Material{ glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.1f, 0.5f, 1 });

		// ----------------------------------------------------------------------------

		pointlights->emplace_back(PointLight{ glm::vec3(277.5f, 275.0f, 277.5f), glm::vec3(100.0f) });

		// ----------------------------------------------------------------------------

		this->objectModel = std::make_unique<EngineObjectModel>(this->device, objects, boundBoxes);
		this->materialModel = std::make_unique<EngineMaterialModel>(this->device, materials);
		this->lightModel = std::make_unique<EnginePointLightModel>(this->device, pointlights);
		this->transformationModel = std::make_unique<EngineTransformationModel>(this->device, transforms);
		this->vertexModels = std::make_unique<EngineVertexModel>(this->device, vertices, indices);

		this->primitiveModel->createBuffers();

		this->textures.emplace_back(std::make_unique<EngineTexture>(this->device, "textures/viking_room.png"));
		this->numLights = static_cast<uint32_t>(pointlights->size());
	}

	void EngineApp::loadQuadModels() {
		auto vertices = std::make_shared<std::vector<Vertex>>();
		auto indices = std::make_shared<std::vector<uint32_t>>();

		Vertex vertex1 { glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f), glm::vec4(0.0f), glm::vec4(0.0f), 0u, 0u };
		vertices->emplace_back(vertex1);

		Vertex vertex2 { glm::vec4(1.0f, -1.0f, 0.0f, 1.0f), glm::vec4(0.0f), glm::vec4(0.0f), 0u, 0u };
		vertices->emplace_back(vertex2);

		Vertex vertex3 { glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f), glm::vec4(0.0f), 0u, 0u };
		vertices->emplace_back(vertex3);

		Vertex vertex4 { glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f), glm::vec4(0.0f), 0u, 0u };
		vertices->emplace_back(vertex4);

		*indices = {
			0, 1, 2, 2, 3, 0
		};

		this->quadModels = std::make_shared<EngineVertexModel>(this->device, vertices, indices);
	}

	void EngineApp::updateCamera(uint32_t width, uint32_t height) {
		glm::vec3 position = glm::vec3(278.0f, 278.0f, -800.0f);
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 800.0f);
		glm::vec3 vup = glm::vec3(0.0f, 1.0f, 0.0f);

		float near = 0.1f;
		float far = 2000.0f;

		constexpr float theta = glm::radians(40.0f);
		float tanHalfFovy = glm::tan(theta / 2.0f);
		float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		glm::vec3 w = glm::normalize(direction);
		glm::vec3 u = glm::normalize(glm::cross(w, vup));
		glm::vec3 v = glm::cross(w, u);

		this->rayTraceUbo.origin = position;
		this->rayTraceUbo.background = glm::vec3(0.0f);
		this->rayTraceUbo.numLights = this->numLights;

		this->rasterUbo.view = glm::mat4{1.0f};
		this->rasterUbo.view[0][0] = u.x;
    this->rasterUbo.view[1][0] = u.y;
    this->rasterUbo.view[2][0] = u.z;
    this->rasterUbo.view[0][1] = v.x;
    this->rasterUbo.view[1][1] = v.y;
    this->rasterUbo.view[2][1] = v.z;
    this->rasterUbo.view[0][2] = w.x;
    this->rasterUbo.view[1][2] = w.y;
    this->rasterUbo.view[2][2] = w.z;
    this->rasterUbo.view[3][0] = -glm::dot(u, position);
    this->rasterUbo.view[3][1] = -glm::dot(v, position);
    this->rasterUbo.view[3][2] = -glm::dot(w, position);

		this->rasterUbo.projection = glm::mat4{0.0f};
		this->rasterUbo.projection[0][0] = 1.f / (aspectRatio * tanHalfFovy);
    this->rasterUbo.projection[1][1] = 1.f / (tanHalfFovy);
    this->rasterUbo.projection[2][2] = far / (far - near);
    this->rasterUbo.projection[2][3] = 1.f;
    this->rasterUbo.projection[3][2] = -(far * near) / (far - near);
	}

	void EngineApp::recreateSubRendererAndSubsystem() {
		uint32_t width = this->renderer->getSwapChain()->width();
		uint32_t height = this->renderer->getSwapChain()->height();

		this->rayTraceUniforms = std::make_unique<EngineRayTraceUniform>(this->device);
		this->rasterUniform = std::make_unique<EngineRasterUniform>(this->device);

		this->updateCamera(width, height);

		this->swapChainSubRenderer = std::make_unique<EngineSwapChainSubRenderer>(this->device, this->renderer->getSwapChain()->getswapChainImages(), 
			this->renderer->getSwapChain()->getSwapChainImageFormat(), static_cast<int>(this->renderer->getSwapChain()->imageCount()), 
			width, height);

		this->forwardPassSubRenderer = std::make_unique<EngineForwardPassSubRenderer>(this->device, 
			EngineDevice::MAX_FRAMES_IN_FLIGHT, width, height);

		this->rayTraceImage = std::make_unique<EngineRayTraceImage>(this->device, width, height, EngineDevice::MAX_FRAMES_IN_FLIGHT);
		this->accumulateImages = std::make_unique<EngineAccumulateImage>(this->device, width, height, EngineDevice::MAX_FRAMES_IN_FLIGHT);

		VkDescriptorBufferInfo rayTracebuffersInfo[9] { 
			this->objectModel->getObjectInfo(), 
			this->objectModel->getBvhInfo(),
			this->primitiveModel->getPrimitiveInfo(), 
			this->primitiveModel->getBvhInfo(),
			this->vertexModels->getVertexInfo(),
			this->materialModel->getMaterialInfo(),
			this->transformationModel->getTransformationInfo(),
			this->lightModel->getPointLightInfo(),
			this->lightModel->getBvhInfo()
		};

		VkDescriptorBufferInfo forwardPassbuffersInfo[2] {
			this->materialModel->getMaterialInfo(),
			this->transformationModel->getTransformationInfo()
		};

		std::vector<VkDescriptorImageInfo> imagesInfo[2] {
			this->rayTraceImage->getImagesInfo(),
			this->accumulateImages->getImagesInfo()
		};

		std::vector<VkDescriptorImageInfo> resourcesInfo[5] = {
			this->forwardPassSubRenderer->getPositionInfoResources(),
			this->forwardPassSubRenderer->getTextCoordInfoResources(),
			this->forwardPassSubRenderer->getNormalInfoResources(),
			this->forwardPassSubRenderer->getAlbedoColorInfoResources(),
			this->forwardPassSubRenderer->getMaterialInfoResources()
		};

		this->samplingDescSet = std::make_unique<EngineSamplingDescSet>(this->device, this->renderer->getDescriptorPool(), imagesInfo);
		this->forwardPassDescSet = std::make_unique<EngineForwardPassDescSet>(this->device, this->renderer->getDescriptorPool(), this->rasterUniform->getBuffersInfo(), forwardPassbuffersInfo);
		this->rayTraceDescSet = std::make_unique<EngineRayTraceDescSet>(this->device, this->renderer->getDescriptorPool(), this->rayTraceUniforms->getBuffersInfo(), 
			this->rayTraceImage->getImagesInfo(), rayTracebuffersInfo, resourcesInfo);

		this->traceRayRender = std::make_unique<EngineTraceRayRenderSystem>(this->device, this->rayTraceDescSet->getDescSetLayout(), width, height, 1);
		this->forwardPassRender = std::make_unique<EngineForwardPassRenderSystem>(this->device, this->forwardPassSubRenderer->getRenderPass(), this->forwardPassDescSet->getDescSetLayout());
		this->samplingRayRender = std::make_unique<EngineSamplingRenderSystem>(this->device, this->swapChainSubRenderer->getRenderPass(), this->samplingDescSet->getDescSetLayout());
	}
}