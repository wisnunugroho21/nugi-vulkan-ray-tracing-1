#pragma once

#include "../window/window.hpp"
#include "../device/device.hpp"
#include "../model/ray_trace_model.hpp"
#include "../model/model.hpp"
#include "../descriptor/descriptor.hpp"
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

			RayTraceUbo updateCamera();
			void recreateSubRendererAndSubsystem();

			EngineWindow window{WIDTH, HEIGHT, APP_TITLE};
			EngineDevice device{window};
			
			std::unique_ptr<EngineHybridRenderer> renderer{};
			std::unique_ptr<EngineSwapChainSubRenderer> swapChainSubRenderer{};
			std::unique_ptr<EngineTraceRayRenderSystem> traceRayRender{};
			std::unique_ptr<EngineSamplingRayRasterRenderSystem> samplingRayRender{};

			std::unique_ptr<EngineRayTraceModel> models;
			std::shared_ptr<EngineModel> quadModels;

			uint32_t randomSeed = 0;
			bool isRendering = true;
	};
}