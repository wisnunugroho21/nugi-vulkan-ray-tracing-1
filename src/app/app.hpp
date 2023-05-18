#pragma once

#include "../window/window.hpp"
#include "../device/device.hpp"
#include "../model/ray_trace_model.hpp"
#include "../model/raster_model.hpp"
#include "../data/descSet/global_desc_set.hpp"
#include "../data/descSet/forward_model_desc_set.hpp"
#include "../data/descSet/forward_output_desc_set.hpp"
#include "../data/descSet/output_desc_set.hpp"
#include "../descriptor/descriptor.hpp"
#include "../renderer/deferred_renderer.hpp"
#include "../renderer_sub/swapchain_sub_renderer.hpp"
#include "../renderer_sub/forward_pass_sub_renderer.hpp"
#include "../renderer_system/forward_light_render_system.hpp"
#include "../renderer_system/forward_pass_render_system.hpp"
#include "../renderer_system/ray_trace_render_system.hpp"
#include "../renderer_system/sampling_render_system.hpp"

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

			void createDescriptor(uint32_t width, uint32_t height, uint32_t imageCount);
			void recreateSubRendererAndSubsystem();

			EngineWindow window{WIDTH, HEIGHT, APP_TITLE};
			EngineDevice device{window};
			
			std::unique_ptr<EngineDefferedRenderer> renderer{};

			std::unique_ptr<EngineForwardPassSubRenderer> forwardPassSubRenderer{};
			std::unique_ptr<EngineSwapChainSubRenderer> swapChainSubRenderer{};

			std::unique_ptr<EngineForwardPassRenderSystem> forwardPassRenderSystem{};
			std::unique_ptr<EngineForwardLightRenderSystem> forwardLightRenderSystem{};
			std::unique_ptr<EngineRayTraceRenderSystem> rayTraceRenderSystem{};
			std::unique_ptr<EngineSamplingRenderSystem> samplingRenderSystem{};

			std::shared_ptr<EngineGeometry> quadModelObject;
			std::shared_ptr<EngineGeometry> gameObject;
			std::shared_ptr<EngineLight> lightObject;
			std::shared_ptr<EngineMaterial> materials;
			std::shared_ptr<EngineTransform> transform;

			std::shared_ptr<EngineGlobalDescSet> globalDescSet;
			std::shared_ptr<EngineForwardOutputDescSet> forwardOutputDescSet;
			std::shared_ptr<EngineOutputDescSet> outputDescSet;

			bool isRendering = true;
			uint32_t randomSeed = 0;
	};
}