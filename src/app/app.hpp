#pragma once

#include "../window/window.hpp"
#include "../device/device.hpp"
#include "../model/ray_trace_model.hpp"
#include "../model/model.hpp"
#include "../descriptor/descriptor.hpp"
#include "../renderer/deferred_renderer.hpp"
#include "../renderer_sub/swapchain_sub_renderer.hpp"
#include "../renderer_sub/forward_pass_sub_renderer.hpp"
#include "../renderer_system/forward_pass_render_system.hpp"
#include "../renderer_system/deferred_render_system.hpp"

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
			
			void recreateSubRendererAndSubsystem();

			EngineWindow window{WIDTH, HEIGHT, APP_TITLE};
			EngineDevice device{window};
			
			std::unique_ptr<EngineDefferedRenderer> renderer{};
			std::unique_ptr<EngineForwardPassSubRenderer> forwardPassSubRenderer{};
			std::unique_ptr<EngineSwapChainSubRenderer> swapChainSubRenderer{};
			std::unique_ptr<EngineForwardPassRenderSystem> forwardPassRenderSystem{};
			std::unique_ptr<EngineDeffereRenderSystem> defferedRenderSystem{};

			std::vector<std::shared_ptr<EngineGameObject>> quadModelObjects;
			std::vector<std::shared_ptr<EngineGameObject>> gameObjects;

			bool isRendering = true;
	};
}