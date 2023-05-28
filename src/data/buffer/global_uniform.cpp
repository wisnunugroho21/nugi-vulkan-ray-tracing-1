#include "global_uniform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <array>
#include <string>

namespace nugiEngine {
	EngineGlobalUniform::EngineGlobalUniform(EngineDevice& device) : appDevice{device} {
		this->createUniformBuffer();
	}

	std::vector<VkDescriptorBufferInfo> EngineGlobalUniform::getBuffersInfo() const {
		std::vector<VkDescriptorBufferInfo> buffersInfo{};
		
		for (int i = 0; i < this->uniformBuffers.size(); i++) {
			buffersInfo.emplace_back(uniformBuffers[i]->descriptorInfo());
		}

		return buffersInfo;
	}

	void EngineGlobalUniform::createUniformBuffer() {
		this->uniformBuffers.clear();

		for (uint32_t i = 0; i < EngineDevice::MAX_FRAMES_IN_FLIGHT; i++) {
			auto uniformBuffer = std::make_shared<EngineBuffer>(
				this->appDevice,
				sizeof(RayTraceUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

			uniformBuffer->map();
			this->uniformBuffers.emplace_back(uniformBuffer);
		}
	}

	void EngineGlobalUniform::writeGlobalData(uint32_t frameIndex, RayTraceUbo ubo) {
		this->uniformBuffers[frameIndex]->writeToBuffer(&ubo);
		this->uniformBuffers[frameIndex]->flush();
	}
}