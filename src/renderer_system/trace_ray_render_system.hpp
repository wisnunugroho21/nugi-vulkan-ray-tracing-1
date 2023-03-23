#pragma once

#include "../command/command_buffer.hpp"
#include "../camera/camera.hpp"
#include "../device/device.hpp"
#include "../pipeline/compute_pipeline.hpp"
#include "../game_object/game_object.hpp"
#include "../frame_info.hpp"
#include "../buffer/buffer.hpp"
#include "../descriptor/descriptor.hpp"
#include "../frame_info.hpp"
#include "../ray_ubo.hpp"

#include <memory>
#include <vector>

namespace nugiEngine {
	class EngineTraceRayRenderSystem {
		public:
			EngineTraceRayRenderSystem(EngineDevice& device, std::shared_ptr<EngineDescriptorPool> descriptorPool, 
				uint32_t width, uint32_t height, uint32_t nSample, std::vector<VkDescriptorBufferInfo> buffersInfo);
			~EngineTraceRayRenderSystem();

			EngineTraceRayRenderSystem(const EngineTraceRayRenderSystem&) = delete;
			EngineTraceRayRenderSystem& operator = (const EngineTraceRayRenderSystem&) = delete;

			std::shared_ptr<EngineDescriptorSetLayout> getDescSetLayout() { return this->descSetLayout; }
			std::shared_ptr<VkDescriptorSet> getDescriptorSets(uint32_t index) { return this->descriptorSets[index]; }
			std::vector<std::shared_ptr<EngineImage>> getStorageImages() { return this->storageImages; }
			bool getFramesUpdated(uint32_t index) const { return this->isFrameUpdated[index]; }

			void writeGlobalData(uint32_t frameIndex, RayTraceUbo ubo);
			void render(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex, uint32_t randomSeed = 1);

			bool prepareFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);
			bool transferFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);
			bool finishFrame(std::shared_ptr<EngineCommandBuffer> commandBuffer, uint32_t frameIndex);

			std::vector<bool> isFrameUpdated;

		private:
			void createPipelineLayout();
			void createPipeline();

			void createUniformBuffer();
			void createImageStorages();

			void createDescriptor(std::shared_ptr<EngineDescriptorPool> descriptorPool, std::vector<VkDescriptorBufferInfo> buffersInfo);

			EngineDevice& appDevice;

			std::shared_ptr<EngineDescriptorSetLayout> descSetLayout;
			std::vector<std::shared_ptr<VkDescriptorSet>> descriptorSets;

			std::vector<std::shared_ptr<EngineBuffer>> uniformBuffers;
			std::vector<std::shared_ptr<EngineImage>> storageImages;
			
			VkPipelineLayout pipelineLayout;
			std::unique_ptr<EngineComputePipeline> pipeline;

			uint32_t width, height, nSample;
	};
}