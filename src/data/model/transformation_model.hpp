#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../command/command_buffer.hpp"
#include "../../ray_ubo.hpp"
#include "../../utils/utils.hpp"
#include "../../utils/transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
  struct TransformationData {
    Transformation transformations[100];
  };

	class EngineTransformationModel {
	public:
		EngineTransformationModel(EngineDevice &device, std::vector<std::shared_ptr<Transformation>> transformations);
		EngineTransformationModel(EngineDevice &device, std::vector<std::shared_ptr<TransformComponent>> transformations);

		~EngineTransformationModel();

		EngineTransformationModel(const EngineTransformationModel&) = delete;
		EngineTransformationModel& operator = (const EngineTransformationModel&) = delete;

    VkDescriptorBufferInfo getTransformationInfo() { return this->transformationBuffer->descriptorInfo();  }
		
	private:
		EngineDevice &engineDevice;
    std::shared_ptr<EngineBuffer> transformationBuffer;

    std::vector<std::shared_ptr<Transformation>> convertToMatrix(std::vector<std::shared_ptr<TransformComponent>> transformations);
		TransformationData createTransformationData(std::vector<std::shared_ptr<Transformation>> transformations);
    void createBuffers(TransformationData &data);
	};
} // namespace nugiEngine
