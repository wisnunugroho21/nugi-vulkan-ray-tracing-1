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
		this->objectModel = std::make_unique<EngineObjectModel>(this->device);

		std::vector<std::shared_ptr<Material>> materials{};
		std::vector<std::shared_ptr<TransformComponent>> transforms{};
		std::vector<std::shared_ptr<Light>> lights{};

		// ----------------------------------------------------------------------------

		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f) }));

		// ----------------------------------------------------------------------------

		int transformIndex = static_cast<int>(transforms.size() - 1);

		// kanan
		auto rightWallObject = std::make_shared<Object>(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		std::vector<std::shared_ptr<Primitive>> rightWallPrimitives;

		rightWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 555.0f} }, 1 }));
		rightWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 0.0f} }, 1 }));
		
		this->primitiveModel->addPrimitive(rightWallPrimitives);
		this->objectModel->addObject(rightWallObject, rightWallPrimitives, transforms[transformIndex]);
		
		// kiri
		auto leftWallObject = std::make_shared<Object>(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		std::vector<std::shared_ptr<Primitive>> leftWallPrimitives{};

		leftWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 555.0f, 0.0f}, glm::vec3{0.0f, 555.0f, 555.0f} }, 2}));
		leftWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 0.0f} }, 2 }));
		
		this->primitiveModel->addPrimitive(leftWallPrimitives);
		this->objectModel->addObject(leftWallObject, leftWallPrimitives, transforms[transformIndex]);
		
		// bawah
		auto bottomWallObject = std::make_shared<Object>(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		std::vector<std::shared_ptr<Primitive>> bottomWallPrimitives{};

		bottomWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 0.0f, 0.0f}, glm::vec3{555.0f, 0.0f, 555.0f} }, 0 }));
		bottomWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 0.0f} }, 0 }));
		
		this->primitiveModel->addPrimitive(bottomWallPrimitives);
		this->objectModel->addObject(bottomWallObject, bottomWallPrimitives, transforms[transformIndex]);
		
		// atas
		auto topWallObject = std::make_shared<Object>(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		std::vector<std::shared_ptr<Primitive>> topWallPrimitives{};

		topWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 555.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 0.0f}, glm::vec3{555.0f, 555.0f, 555.0f} }, 0 }));
		topWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 0.0f} }, 0 }));

		this->primitiveModel->addPrimitive(topWallPrimitives);		
		this->objectModel->addObject(topWallObject, topWallPrimitives, transforms[transformIndex]);
		
		//// depan
		auto frontWallObject = std::make_shared<Object>(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		std::vector<std::shared_ptr<Primitive>> frontWallPrimitives{};

		frontWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 555.0f, 555.0f} }, 0 }));
		frontWallPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{555.0f, 555.0f, 555.0f}, glm::vec3{555.0f, 0.0f, 555.0f}, glm::vec3{0.0f, 0.0f, 555.0f} }, 0 }));

		this->primitiveModel->addPrimitive(frontWallPrimitives);
		this->objectModel->addObject(frontWallObject, frontWallPrimitives, transforms[transformIndex]);

		// ----------------------------------------------------------------------------

		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(265.0f, 0.0f, 295.0f), glm::vec3(1.0f), glm::vec3(0.0f, glm::radians(15.0f), 0.0f)}));
		transformIndex = static_cast<int>(transforms.size() - 1);

		auto firstBoxesObject = std::make_shared<Object>(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		std::vector<std::shared_ptr<Primitive>> firstBoxesPrimitives{};

		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 330.0f, 0.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 330.0f, 0.0f}, glm::vec3{0.0f, 330.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 165.0f}, glm::vec3{165.0f, 330.0f, 165.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 330.0f, 165.0f}, glm::vec3{165.0f, 330.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 0.0f} } , 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 330.0f, 165.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 330.0f, 165.0f}, glm::vec3{165.0f, 330.0f, 165.0f}, glm::vec3{165.0f, 0.0f, 165.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 330.0f, 0.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 330.0f, 0.0f}, glm::vec3{0.0f, 330.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 165.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 165.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 0.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 330.0f, 0.0f}, glm::vec3{165.0f, 330.0f, 0.0f}, glm::vec3{165.0f, 330.0f, 165.0f} }, 0 }));
		firstBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 330.0f, 165.0f}, glm::vec3{0.0f, 330.0f, 165.0f}, glm::vec3{0.0f, 330.0f, 0.0f} }, 0 }));

		this->primitiveModel->addPrimitive(firstBoxesPrimitives);
		this->objectModel->addObject(firstBoxesObject, firstBoxesPrimitives, transforms[transformIndex]);

		// ----------------------------------------------------------------------------

		transforms.emplace_back(std::make_shared<TransformComponent>(TransformComponent{ glm::vec3(130.0f, 0.0f, 65.0f), glm::vec3(1.0f), glm::vec3(0.0f, glm::radians(-18.0f), 0.0f)}));
		transformIndex = static_cast<int>(transforms.size() - 1);

		auto secondBoxesObject = std::make_shared<Object>(Object{ this->primitiveModel->getBvhSize(), this->primitiveModel->getPrimitiveSize(), transformIndex });
		std::vector<std::shared_ptr<Primitive>> secondBoxesPrimitives{};
		
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 165.0f, 0.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 165.0f, 0.0f}, glm::vec3{0.0f, 165.0f, 0.0f}, glm::vec3{0.0f, 0.0f, 0.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 165.0f}, glm::vec3{165.0f, 165.0f, 165.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 165.0f, 165.0f}, glm::vec3{165.0f, 165.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 0.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 165.0f, 165.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 165.0f, 165.0f}, glm::vec3{165.0f, 165.0f, 165.0f}, glm::vec3{165.0f, 0.0f, 165.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{0.0f, 165.0f, 0.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 165.0f, 0.0f}, glm::vec3{0.0f, 165.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 165.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 0.0f}, glm::vec3{165.0f, 0.0f, 165.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 165.0f}, glm::vec3{0.0f, 0.0f, 0.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{0.0f, 165.0f, 0.0f}, glm::vec3{165.0f, 165.0f, 0.0f}, glm::vec3{165.0f, 165.0f, 165.0f} }, 0 }));
		secondBoxesPrimitives.emplace_back(std::make_shared<Primitive>(Primitive{ Triangle{ glm::vec3{165.0f, 165.0f, 165.0f}, glm::vec3{0, 165.0f, 165.0f}, glm::vec3{0.0f, 165.0f, 0.0f} }, 0 }));

		this->primitiveModel->addPrimitive(secondBoxesPrimitives);
		this->objectModel->addObject(secondBoxesObject, secondBoxesPrimitives, transforms[transformIndex]);

		// ----------------------------------------------------------------------------

		materials.emplace_back(std::make_shared<Material>(Material{ glm::vec3(0.73f, 0.73f, 0.73f), 0.0f, 0.1f, 0.5f }));
		materials.emplace_back(std::make_shared<Material>(Material{ glm::vec3(0.12f, 0.45f, 0.15f), 0.0f, 0.1f, 0.5f }));
		materials.emplace_back(std::make_shared<Material>(Material{ glm::vec3(0.65f, 0.05f, 0.05f), 0.0f, 0.1f, 0.5f }));
		materials.emplace_back(std::make_shared<Material>(Material{ glm::vec3(0.73f, 0.73f, 0.73f), 0.0f, 0.1f, 0.5f }));

		// ----------------------------------------------------------------------------

		lights.emplace_back(std::make_shared<Light>(Light{ Triangle{ glm::vec3{213.0f, 554.0f, 227.0f}, glm::vec3{343.0f, 554.0f, 227.0f}, glm::vec3{343.0f, 554.0f, 332.0f} }, glm::vec3(100.0f, 100.0f, 100.0f)} ));
		lights.emplace_back(std::make_shared<Light>(Light{ Triangle{ glm::vec3{343.0f, 554.0f, 332.0f}, glm::vec3{213.0f, 554.0f, 332.0f}, glm::vec3{213.0f, 554.0f, 227.0f} }, glm::vec3(100.0f, 100.0f, 100.0f)} ));

		// ----------------------------------------------------------------------------

		this->materialModel = std::make_unique<EngineMaterialModel>(this->device, materials);
		this->lightModel = std::make_unique<EngineLightModel>(this->device, lights);
		this->transformationModel = std::make_unique<EngineTransformationModel>(this->device, transforms);

		this->globalUniforms = std::make_unique<EngineGlobalUniform>(this->device);

		this->primitiveModel->createBuffers();
		this->objectModel->createBuffers();
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

		return ubo;
	}

	void EngineApp::recreateSubRendererAndSubsystem() {
		uint32_t width = this->renderer->getSwapChain()->width();
		uint32_t height = this->renderer->getSwapChain()->height();

		this->globalUbo = this->updateCamera(width, height);

		std::shared_ptr<EngineDescriptorPool> descriptorPool = this->renderer->getDescriptorPool();
		std::vector<std::shared_ptr<EngineImage>> swapChainImages = this->renderer->getSwapChain()->getswapChainImages();

		this->swapChainSubRenderer = std::make_unique<EngineSwapChainSubRenderer>(this->device, this->renderer->getSwapChain()->getswapChainImages(), 
			this->renderer->getSwapChain()->getSwapChainImageFormat(), this->renderer->getSwapChain()->imageCount(), 
			width, height);

		this->rayTraceImage = std::make_unique<EngineRayTraceImage>(this->device, width, height, this->renderer->getSwapChain()->imageCount());
		this->accumulateImages = std::make_unique<EngineAccumulateImage>(this->device, width, height, this->renderer->getSwapChain()->imageCount());

		VkDescriptorBufferInfo buffersInfo[8] { 
			this->objectModel->getObjectInfo(), 
			this->objectModel->getBvhInfo(),
			this->primitiveModel->getPrimitiveInfo(), 
			this->primitiveModel->getBvhInfo(),
			this->lightModel->getLightInfo(),
			this->lightModel->getBvhInfo(),
			this->materialModel->getMaterialInfo(),
			this->transformationModel->getTransformationInfo() 
		};

		std::vector<VkDescriptorImageInfo> imagesInfo[2] {
			this->rayTraceImage->getImagesInfo(),
			this->accumulateImages->getImagesInfo()
		};

		this->rayTraceDescSet = std::make_unique<EngineRayTraceDescSet>(this->device, this->renderer->getDescriptorPool(), this->globalUniforms->getBuffersInfo(), this->rayTraceImage->getImagesInfo(), buffersInfo);
		this->samplingDescSet = std::make_unique<EngineSamplingDescSet>(this->device, this->renderer->getDescriptorPool(), imagesInfo);

		this->traceRayRender = std::make_unique<EngineTraceRayRenderSystem>(this->device, this->rayTraceDescSet->getDescSetLayout()->getDescriptorSetLayout(), width, height, 1);
		this->samplingRayRender = std::make_unique<EngineSamplingRayRasterRenderSystem>(this->device, this->samplingDescSet->getDescSetLayout()->getDescriptorSetLayout(), this->swapChainSubRenderer->getRenderPass()->getRenderPass());
	}
}