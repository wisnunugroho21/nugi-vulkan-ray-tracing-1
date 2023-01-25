#pragma once

#include "../device/device.hpp"
#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	struct Triangle {
    alignas(16) glm::vec3 point0;
    alignas(16) glm::vec3 point1;
    alignas(16) glm::vec3 point2;
  };

	struct ModelData {
    std::vector<Triangle> triangles;
		void loadModel(const std::string &filePath);
	};

  struct BvhNode {
    int leftNode;
    int rightNode;
    int objIndex;

    alignas(16) glm::vec3 maximum;
    alignas(16) glm::vec3 minimum;
  };

  struct TriangleData {
    Triangle triangles[500];
  };
  

  struct BvhData {
    BvhNode bvhNodes[500];
	};

  struct NumData {
    uint32_t objSize;
    uint32_t bvhSize;
  };

	class EngineRayTraceModel {
	public:
		EngineRayTraceModel(EngineDevice &device, ModelData &data);
		~EngineRayTraceModel();

		EngineRayTraceModel(const EngineRayTraceModel&) = delete;
		EngineRayTraceModel& operator = (const EngineRayTraceModel&) = delete;

    VkDescriptorBufferInfo getObjectInfo() { return this->objectBuffer->descriptorInfo();  }
    VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
    VkDescriptorBufferInfo getNumInfo() { return this->numBuffer->descriptorInfo(); }

    static std::unique_ptr<EngineRayTraceModel> createModelFromFile(EngineDevice &device, const std::string &filePath);
		
	private:
		EngineDevice &engineDevice;
		
    std::shared_ptr<EngineBuffer> objectBuffer;
    std::shared_ptr<EngineBuffer> bvhBuffer;
    std::shared_ptr<EngineBuffer> numBuffer;

    NumData numData;

    BvhData createBvhData(const ModelData &data);
    void createBuffers(ModelData &data, BvhData &bvh);
	};
} // namespace nugiEngine
