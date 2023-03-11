#include "bottom_level_acceleration_structure.hpp"

namespace nugiEngine {
	EngineBottomLevelAccelerationStructure::EngineBottomLevelAccelerationStructure(EngineDevice& appDevice, EngineDeviceProcedures& deviceProcedure, std::shared_ptr<EngineModel> model) 
		: appDevice{appDevice}, deviceProcedure{deviceProcedure} 
	{
		this->createBottomLevelAccelerationStructure(model);
	}

  void EngineBottomLevelAccelerationStructure::createBottomLevelAccelerationStructure(std::shared_ptr<EngineModel> model) {
		uint32_t numTriangles = static_cast<uint32_t>(model->getIndexCount()) / 3;
		uint32_t maxVertex = model->getVertexCount();

		VkAccelerationStructureGeometryTrianglesDataKHR triangles{};
		triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
		triangles.vertexData.deviceAddress = model->getVertexBuffer()->getDeviceAddress();
		triangles.maxVertex = maxVertex;
		triangles.vertexStride = sizeof(Vertex);
		triangles.indexType = VK_INDEX_TYPE_UINT32;
		triangles.indexData.deviceAddress = model->getIndexBuffer()->getDeviceAddress();
		triangles.transformData.deviceAddress = 0;

    VkAccelerationStructureGeometryKHR accelerationStructureGeometry{};
		accelerationStructureGeometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		accelerationStructureGeometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
		accelerationStructureGeometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		accelerationStructureGeometry.geometry.triangles = triangles;

		std::vector<VkAccelerationStructureGeometryKHR> geometries{ accelerationStructureGeometry };

    // Get size info
		VkAccelerationStructureBuildGeometryInfoKHR accelerationStructureBuildGeometryInfo{};
		accelerationStructureBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationStructureBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		accelerationStructureBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationStructureBuildGeometryInfo.geometryCount = static_cast<uint32_t>(geometries.size());
		accelerationStructureBuildGeometryInfo.pGeometries = geometries.data();

    VkAccelerationStructureBuildSizesInfoKHR accelerationStructureBuildSizesInfo{};
		accelerationStructureBuildSizesInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

		this->deviceProcedure.vkGetAccelerationStructureBuildSizesKHR(
			this->appDevice.getLogicalDevice(),
			VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
			&accelerationStructureBuildGeometryInfo,
			&numTriangles,
			&accelerationStructureBuildSizesInfo
    );

    this->buffer = std::make_shared<EngineBuffer>(
      this->appDevice,
			accelerationStructureBuildSizesInfo.accelerationStructureSize,
			static_cast<uint32_t>(1),
			VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			1, true
    );

		VkAccelerationStructureCreateInfoKHR accelerationStructureCreateInfo{};
		accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
		accelerationStructureCreateInfo.buffer = this->buffer->getBuffer();
		accelerationStructureCreateInfo.size = accelerationStructureBuildSizesInfo.accelerationStructureSize;
		accelerationStructureCreateInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;

		this->deviceProcedure.vkCreateAccelerationStructureKHR(this->appDevice.getLogicalDevice(), &accelerationStructureCreateInfo, nullptr, &this->accelStruct);

		EngineBuffer stratchBuffer{
			this->appDevice,
			accelerationStructureBuildSizesInfo.buildScratchSize,
			static_cast<uint32_t>(1),
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			1, true
		};

		VkAccelerationStructureBuildGeometryInfoKHR accelerationBuildGeometryInfo{};
		accelerationBuildGeometryInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
		accelerationBuildGeometryInfo.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
		accelerationBuildGeometryInfo.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
		accelerationBuildGeometryInfo.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
		accelerationBuildGeometryInfo.dstAccelerationStructure = this->accelStruct;
		accelerationBuildGeometryInfo.geometryCount = static_cast<uint32_t>(geometries.size());
		accelerationBuildGeometryInfo.pGeometries = geometries.data();
		accelerationBuildGeometryInfo.scratchData.deviceAddress = stratchBuffer.getDeviceAddress();

		std::vector<VkAccelerationStructureBuildGeometryInfoKHR> accelerationBuildGeometryInfoes = { accelerationBuildGeometryInfo };

		VkAccelerationStructureBuildRangeInfoKHR accelerationStructureBuildRangeInfo{};
		accelerationStructureBuildRangeInfo.primitiveCount = numTriangles;
		accelerationStructureBuildRangeInfo.primitiveOffset = 0;
		accelerationStructureBuildRangeInfo.firstVertex = 0;
		accelerationStructureBuildRangeInfo.transformOffset = 0;
		
		std::vector<VkAccelerationStructureBuildRangeInfoKHR*> accelerationBuildStructureRangeInfos = { &accelerationStructureBuildRangeInfo };

		EngineCommandBuffer commandBuffer{this->appDevice};
		commandBuffer.beginSingleTimeCommand();

		this->deviceProcedure.vkCmdBuildAccelerationStructuresKHR(
			commandBuffer.getCommandBuffer(),
			static_cast<uint32_t>(accelerationBuildGeometryInfoes.size()),
			accelerationBuildGeometryInfoes.data(),
			accelerationBuildStructureRangeInfos.data()
		);

		commandBuffer.endCommand();
		commandBuffer.submitCommand(this->appDevice.getGraphicsQueue(0));

		VkAccelerationStructureDeviceAddressInfoKHR accelerationDeviceAddressInfo{};
		accelerationDeviceAddressInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
		accelerationDeviceAddressInfo.accelerationStructure = this->accelStruct;

		this->address = this->deviceProcedure.vkGetAccelerationStructureDeviceAddressKHR(this->appDevice.getLogicalDevice(), &accelerationDeviceAddressInfo);
  }
  
} // namespace nugiEngin 
