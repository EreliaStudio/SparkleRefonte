#pragma once

#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "external_libraries/stb_image.h"
#include "structure/graphics/opengl/spk_texture_object.hpp"

namespace spk
{
	class Image : public OpenGL::TextureObject
	{
	public:
		Image(const std::filesystem::path& filePath)
		{
			loadFromFile(filePath);
		}

		void loadFromFile(const std::filesystem::path& filePath)
		{
			int width, height, channels;
			stbi_uc* imageData = stbi_load(filePath.string().c_str(), &width, &height, &channels, 0);
			if (!imageData)
			{
				throw std::runtime_error("Failed to load image: " + filePath.string());
			}

			spk::Vector2UInt size{ static_cast<unsigned int>(width), static_cast<unsigned int>(height) };
			Format format = _determineFormat(channels);

			setData(imageData, size, format, Filtering::Nearest, Wrap::Repeat, Mipmap::Disable);

			stbi_image_free(imageData);
		}

	private:
		Format _determineFormat(int channels) const
		{
			switch (channels)
			{
			case 1: return Format::GreyLevel;
			case 2: return Format::DualChannel;
			case 3: return Format::RGB;
			case 4: return Format::RGBA;
			default: return Format::Error;
			}
		}
	};
}