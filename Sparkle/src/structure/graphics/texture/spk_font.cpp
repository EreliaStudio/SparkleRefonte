#include "structure/graphics/texture/spk_font.hpp"

namespace spk
{
	void Font::Glyph::rescale(const Vector2& p_scaleRatio)
	{
		for (size_t i = 0; i < 4; i++)
		{
			UVs[i] *= p_scaleRatio;
		}
	}

	void Font::Atlas::_rescaleGlyphs(const Vector2& p_scaleRatio)
	{
		for (auto& [key, element] : _glyphs)
		{
			element.rescale(p_scaleRatio);
		}
	}

	void Font::Atlas::_resizeData(const Vector2UInt& p_size)
	{
		std::vector<uint8_t> newPixels(p_size.x * p_size.y, 0x00);
		std::vector<bool> newUsedPixels(p_size.x * p_size.y, false);

		for (size_t x = 0; x < _size.x; x++)
		{
			for (size_t y = 0; y < _size.y; y++)
			{
				newPixels[x + y * p_size.x] = _pixels[x + y * _size.x];
			}
		}

		_rescaleGlyphs(static_cast<Vector2>(_size) / static_cast<Vector2>(p_size));
		_size = p_size;
		_quadrantSize = _size / 2;
		_pixels.swap(newPixels);
	}

	void Font::Atlas::_applyGlyphPixel(const uint8_t* p_pixelsToApply, const Vector2Int& p_glyphPosition, const Vector2UInt& p_glyphSize)
	{
		while ((p_glyphPosition.x + p_glyphSize.x >= _size.x) || (p_glyphPosition.y + p_glyphSize.y >= _size.y))
		{
			_resizeData(_size * 2);
		}
		for (size_t x = 0; x < p_glyphSize.x; x++)
		{
			for (size_t y = 0; y < p_glyphSize.y; y++)
			{
				_pixels[(p_glyphPosition.x + x) + (p_glyphPosition.y + y) * _size.x] = p_pixelsToApply[x + y * p_glyphSize.x];
			}
		}
	}

	void Font::Atlas::_uploadTexture()
	{
		setData(
			_pixels.data(),
			_size,
			OpenGL::TextureObject::Format::GreyLevel,
			OpenGL::TextureObject::Filtering::Nearest,
			OpenGL::TextureObject::Wrap::Repeat,
			OpenGL::TextureObject::Mipmap::Disable
		);
		_needUpload = false;
	}

	Font::Atlas::Atlas(const stbtt_fontinfo& p_fontInfo, const std::string& p_fontData, const size_t& p_textSize, const size_t& p_outlineSize) :
		_textSize(p_textSize), _outlineSize(p_outlineSize), _fontInfo(p_fontInfo)
	{
		Font::Glyph spaceGlyph;

		spaceGlyph.step = Vector2Int(p_textSize / 2.0f, 0);

		_glyphs[L' '] = spaceGlyph;
		_resizeData(Vector2UInt(124, 124));

		_currentQuadrant = Quadrant::TopLeft;
		_quadrantAnchor = Vector2Int(0, 0);
		_quadrantSize = _size;
		_nextGlyphAnchor = _quadrantAnchor;
		_nextLineAnchor = _quadrantAnchor;
	}

	void Font::Atlas::loadGlyphs(const std::wstring& p_glyphsToLoad)
	{
		for (const auto& c : p_glyphsToLoad)
		{
			operator[](c);
		}
	}

	const Font::Glyph& Font::Atlas::operator[](const wchar_t& p_char)
	{
		return (glyph(p_char));
	}

	const Font::Glyph& Font::Atlas::glyph(const wchar_t& p_char)
	{
		if (_glyphs.contains(p_char) == false)
		{
			_loadGlyph(p_char);
		}
		return _glyphs.at(p_char);
	}

	Font::Font(const std::filesystem::path& p_path)
	{
		_loadFileData(p_path);
	}

	Vector2UInt Font::Atlas::computeCharSize(const wchar_t& p_char)
	{
		return (operator[](p_char).size);
	}

	Vector2UInt Font::Atlas::computeStringSize(const std::string& p_string)
	{
		int totalWidth = 0;
		int maxHeight = 0;
		int minHeight = 0;

		for (size_t i = 0; i < p_string.size(); i++)
		{
			const Glyph& glyph = operator[](p_string[i]);

			if (i != p_string.size() - 1)
				totalWidth += glyph.step.x;
			else
				totalWidth += glyph.size.x;

			maxHeight = std::max(maxHeight, glyph.positions[3].y);
			minHeight = std::min(minHeight, glyph.positions[0].y);
		}

		int totalHeight = maxHeight - minHeight;
		return Vector2UInt(totalWidth, totalHeight);
	}

	Vector2UInt Font::computeCharSize(const wchar_t& p_char, size_t p_size, size_t p_outlineSize)
	{
		return (atlas(Font::Size(p_size, p_outlineSize)).computeCharSize(p_char));
	}

	Vector2UInt Font::computeStringSize(const std::string& p_string, size_t p_size, size_t p_outlineSize)
	{
		return (atlas(Font::Size(p_size, p_outlineSize)).computeStringSize(p_string));
	}

	Font::Size Font::computeOptimalTextSize(const std::string& p_string, float p_outlineSizeRatio, const Vector2UInt& p_textArea)
	{
		std::vector<size_t> deltas = { 100u, 50u, 20u, 10u, 1u };
		Font::Size result = 2;

		if (p_string == "")
		{
			size_t resultTextSize = p_textArea.y;
			size_t resultOutlineSize = static_cast<size_t>(resultTextSize * p_outlineSizeRatio);
			resultTextSize -= resultOutlineSize * 2;

			return (Font::Size(resultTextSize, resultOutlineSize));
		}

		for (int i = 0; i < static_cast<int>(deltas.size()); i++)
		{
			if (deltas[i] > p_textArea.y)
				continue;

			bool enough = false;
			while (enough == false)
			{
				size_t textSize = result.text + deltas[i];
				size_t outlineSize = static_cast<size_t>(textSize * p_outlineSizeRatio);
				textSize -= outlineSize * 2;

				Vector2UInt tmp_size = computeStringSize(p_string, textSize, outlineSize);

				if (tmp_size.x >= p_textArea.x || tmp_size.y >= p_textArea.y)
				{
					enough = true;
				}
				else
				{
					result.text += deltas[i];
				}
			}
		}

		result.outline = static_cast<size_t>(result.text * p_outlineSizeRatio);
		result.text -= result.outline * 2;

		return (result);
	}

	Font::Atlas& Font::atlas(const Size& p_size)
	{
		auto key = std::make_tuple(p_size.text, p_size.outline);
		if (_atlases.find(key) == _atlases.end())
		{
			_atlases.emplace(key, Atlas(_fontInfo, _fontData, p_size.text, p_size.outline));
		}
		return _atlases.at(key);
	}
}