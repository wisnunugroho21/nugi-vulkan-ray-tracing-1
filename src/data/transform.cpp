#include "transform.hpp"
#include "../utils/utils.hpp"

namespace nugiEngine {
	glm::mat4 TransformComponent::mat4() {
		const float c3 = glm::cos(this->rotation.z);
		const float s3 = glm::sin(this->rotation.z);
		const float c2 = glm::cos(this->rotation.x);
		const float s2 = glm::sin(this->rotation.x);
		const float c1 = glm::cos(this->rotation.y);
		const float s1 = glm::sin(this->rotation.y);

		return glm::mat4{
			{
				this->scale.x * (c1 * c3 + s1 * s2 * s3),
				this->scale.x * (c2 * s3),
				this->scale.x * (c1 * s2 * s3 - c3 * s1),
				0.0f,
			},
			{
				this->scale.y * (c3 * s1 * s2 - c1 * s3),
				this->scale.y * (c2 * c3),
				this->scale.y * (c1 * c3 * s2 + s1 * s3),
				0.0f,
			},
			{
				this->scale.z * (c2 * s1),
				this->scale.z * (-s2),
				this->scale.z * (c1 * c2),
				0.0f,
			},
			{this->translation.x, this->translation.y, this->translation.z, 1.0f}
		};
	}

	glm::mat3 TransformComponent::normalMatrix() {
		const float c3 = glm::cos(this->rotation.z);
		const float s3 = glm::sin(this->rotation.z);
		const float c2 = glm::cos(this->rotation.x);
		const float s2 = glm::sin(this->rotation.x);
		const float c1 = glm::cos(this->rotation.y);
		const float s1 = glm::sin(this->rotation.y);

		const glm::vec3 invScale = 1.0f / scale;

		return glm::mat3{
			{
				invScale.x * (c1 * c3 + s1 * s2 * s3),
				invScale.x * (c2 * s3),
				invScale.x * (c1 * s2 * s3 - c3 * s1)
			},
			{
				invScale.y * (c3 * s1 * s2 - c1 * s3),
				invScale.y * (c2 * c3),
				invScale.y * (c1 * c3 * s2 + s1 * s3)
			},
			{
				invScale.z * (c2 * s1),
				invScale.z * (-s2),
				invScale.z * (c1 * c2)
			},
		};
	}

	EngineTransform::EngineTransform(EngineDevice &device) : engineDevice{device} {
		this->createTransformBuffers();
	}

	EngineTransform EngineTransform::createFromTransformComponent(EngineDevice &device, std::vector<TransformComponent> transformComponents) {
		TransformData transformData{};

		int transformIndex = 0; 
		for (auto &&transformComponent : transformComponents) {
			transformData.data[transformIndex].modelMatrix = transformComponent.mat4();
			transformData.data[transformIndex].normalMatrix = transformComponent.normalMatrix();
		}

		EngineTransform transform(device);
		transform.writeBuffers(&transformData);

		return transform;
	}

	std::shared_ptr<EngineTransform> EngineTransform::createSharedFromTransformComponent(EngineDevice &device, std::vector<TransformComponent> transformComponents) {
		TransformData transformData{};

		int transformIndex = 0; 
		for (auto &&transformComponent : transformComponents) {
			transformData.data[transformIndex].modelMatrix = transformComponent.mat4();
			transformData.data[transformIndex].normalMatrix = transformComponent.normalMatrix();
		}

		std::shared_ptr<EngineTransform> transform = std::make_shared<EngineTransform>(device);
		transform->writeBuffers(&transformData);

		return transform;
	}

	void EngineTransform::createTransformBuffers() {
		this->transformBuffer = std::make_shared<EngineBuffer>(
			this->engineDevice,
			sizeof(TransformData),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		this->transformBuffer->map();
	}

	void EngineTransform::writeBuffers(TransformData *data, VkDeviceSize size, VkDeviceSize offset) {
		this->transformBuffer->writeToBuffer(data, size, offset);
		this->transformBuffer->flush(size, offset);
	}

	void EngineTransform::writeBuffers(std::vector<TransformComponent> transformComponents, VkDeviceSize size, VkDeviceSize offset) {
		TransformData transformData{};

		int transformIndex = 0; 
		for (auto &&transformComponent : transformComponents) {
			transformData.data[transformIndex].modelMatrix = transformComponent.mat4();
			transformData.data[transformIndex].normalMatrix = transformComponent.normalMatrix();
		}
		
		this->transformBuffer->writeToBuffer(&transformData, size, offset);
		this->transformBuffer->flush(size, offset);
	}
    
} // namespace nugiEngine

