#pragma once

#include "../../device/device.hpp"
#include "../../buffer/buffer.hpp"
#include "../../descriptor/descriptor.hpp"
#include "../../texture/texture.hpp"

#include <memory>

namespace nugiEngine {
	class EngineAccumulateImage {
		public:
			EngineAccumulateImage(EngineDevice& device, std::vector<std::shared_ptr<EngineTexture>> textures);

			std::vector<VkDescriptorImageInfo> getImagesInfo() const;

		private:
			std::vector<std::shared_ptr<EngineTexture>> textures;

			void createTextureImages(EngineDevice& device, uint32_t width, uint32_t height, uint32_t imageCount);
	};
	
}