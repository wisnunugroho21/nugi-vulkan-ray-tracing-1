#pragma once

#include "../device/device.hpp"
#include "../buffer/buffer.hpp"
#include "../command/command_buffer.hpp"
#include "../ray_ubo.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>
#include <memory>

namespace nugiEngine {
	struct RayTraceModelData {
    std::vector<Sphere> spheres;
    std::vector<Lambertian> lambertians;
    std::vector<Light> lights;

		void loadModel(const std::string &filePath);
	};

  struct TriangleData {
    Triangle triangles[100];
  };

  struct SphereData {
    Sphere spheres[100];
  };

  struct BvhData {
    BvhNode bvhNodes[100];
	};

  struct MaterialData {
    Lambertian lambertians[100];
  };

  struct LightData {
    Light lights[100];
  };

	class EngineRayTraceModel {
	public:
		EngineRayTraceModel(EngineDevice &device, RayTraceModelData &data);
		~EngineRayTraceModel();

		EngineRayTraceModel(const EngineRayTraceModel&) = delete;
		EngineRayTraceModel& operator = (const EngineRayTraceModel&) = delete;

    VkDescriptorBufferInfo getObjectInfo() { return this->objectBuffer->descriptorInfo();  }
    VkDescriptorBufferInfo getBvhInfo() { return this->bvhBuffer->descriptorInfo(); }
    VkDescriptorBufferInfo getMaterialInfo() { return this->materialBuffer->descriptorInfo();  }
    VkDescriptorBufferInfo getLightInfo() { return this->lightBuffer->descriptorInfo(); }

    static std::unique_ptr<EngineRayTraceModel> createModelFromFile(EngineDevice &device, const std::string &filePath);
		
	private:
		EngineDevice &engineDevice;
		
    std::shared_ptr<EngineBuffer> objectBuffer;
    std::shared_ptr<EngineBuffer> bvhBuffer;
    std::shared_ptr<EngineBuffer> materialBuffer;
    std::shared_ptr<EngineBuffer> lightBuffer;

	  SphereData createObjectData(const RayTraceModelData &data);
    BvhData createBvhData(const RayTraceModelData &data);
    MaterialData createMaterialData(const RayTraceModelData &data);
    LightData createLightData(const RayTraceModelData &data);

    void createBuffers(SphereData &data, BvhData &bvh, MaterialData &material, LightData &light);
	};
} // namespace nugiEngine
