#pragma once

#include "../window/window.hpp"
#include "../device/device.hpp"
#include "../model/ray_trace_model.hpp"
#include "../model/model.hpp"
#include "../descriptor/descriptor.hpp"
#include "../renderer/ray_tracing_renderer.hpp"
#include "../renderer_sub/swapchain_sub_renderer.hpp"
#include "../renderer_system/hadware_ray_tracing_render_system.hpp"


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
			void recreateSubRendererAndSubsystem();

			EngineWindow window{WIDTH, HEIGHT, APP_TITLE};
			EngineDevice device{window};
			EngineDeviceProcedures deviceProcedure{device};
			
			std::unique_ptr<EngineRayTraceRenderer> renderer{};
			std::unique_ptr<EngineRayTracingRenderSystem> rayTracingRenderSystem{};

			std::unique_ptr<EngineRayTraceModel> models;
			std::shared_ptr<EngineModel> quadModels;

			std::vector<std::shared_ptr<EngineGameObject>> gameObjects;

			uint32_t randomSeed = 0;
			bool isRendering = true;
			RayTraceUbo globalUbo;
	};
}