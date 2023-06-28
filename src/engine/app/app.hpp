#pragma once

#include "../../vulkan/window/window.hpp"
#include "../../vulkan/device/device.hpp"
#include "../../vulkan/texture/texture.hpp"
#include "../../vulkan/buffer/buffer.hpp"
#include "../utils/camera/camera.hpp"
#include "../data/image/accumulate_image.hpp"
#include "../data/image/ray_trace_image.hpp"
#include "../data/model/primitive_model.hpp"
#include "../data/model/object_model.hpp"
#include "../data/model/light_model.hpp"
#include "../data/model/material_model.hpp"
#include "../data/model/transformation_model.hpp"
#include "../data/model/vertex_ray_trace_model.hpp"
#include "../data/buffer/global_uniform.hpp"
#include "../data/descSet/ray_trace_desc_set.hpp"
#include "../data/descSet/sampling_desc_set.hpp"
#include "../renderer/hybrid_renderer.hpp"
#include "../renderer_sub/swapchain_sub_renderer.hpp"
#include "../renderer_system/trace_ray_render_system.hpp"
#include "../renderer_system/sampling_ray_raster_render_system.hpp"

#include <memory>
#include <vector>

#define APP_TITLE "Testing Vulkan"

namespace nugiEngine {
	class EngineApp
	{
		public:
			static constexpr int WIDTH = 800;
			static constexpr int HEIGHT = 800;

			EngineApp();
			~EngineApp();

			EngineApp(const EngineApp&) = delete;
			EngineApp& operator = (const EngineApp&) = delete;

			void run();
			void renderLoop();

		private:
			void loadObjects();
			void loadQuadModels();

			RayTraceUbo updateCamera(uint32_t width, uint32_t height);
			void recreateSubRendererAndSubsystem();

			EngineWindow window{WIDTH, HEIGHT, APP_TITLE};
			EngineDevice device{window};
			
			std::unique_ptr<EngineHybridRenderer> renderer{};
			std::unique_ptr<EngineSwapChainSubRenderer> swapChainSubRenderer{};
			std::unique_ptr<EngineTraceRayRenderSystem> traceRayRender{};
			std::unique_ptr<EngineSamplingRayRasterRenderSystem> samplingRayRender{};

			std::unique_ptr<EngineAccumulateImage> accumulateImages{};
			std::unique_ptr<EngineRayTraceImage> rayTraceImage{};
			std::unique_ptr<EngineGlobalUniform> globalUniforms{};

			std::unique_ptr<EnginePrimitiveModel> primitiveModel{};
			std::unique_ptr<EngineObjectModel> objectModel{};
			std::unique_ptr<EngineLightModel> lightModel{};
			std::unique_ptr<EngineMaterialModel> materialModel{};
			std::unique_ptr<EngineTransformationModel> transformationModel{};
			std::shared_ptr<EngineVertexModel> quadModels{};
			std::shared_ptr<EngineRayTraceVertexModel> rayTraceVertexModels{};

			std::unique_ptr<EngineRayTraceDescSet> rayTraceDescSet{};
			std::unique_ptr<EngineSamplingDescSet> samplingDescSet{};

			std::vector<std::unique_ptr<EngineTexture>> textures{};

			uint32_t randomSeed = 0;
			uint32_t numLights = 0;
			bool isRendering = true;

			RayTraceUbo globalUbo;
	};
}