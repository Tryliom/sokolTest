#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#if defined(_WIN32)
    #include "malloc.h"
#endif

#include "Logger.h"

#include <string>

const int Channels = 4;

Image::Image()
{
    _width = 0;
    _height = 0;

    _originalWidth = 0;
    _originalHeight = 0;

    _buffer = nullptr;
    _originalBuffer = nullptr;
}

Image::Image(int width, int height, uint8_t* buffer)
{
    _width = width;
    _height = height;

    _originalWidth = width;
    _originalHeight = height;

    _buffer = (uint8_t*) malloc(width * height * Channels);
    _originalBuffer = (uint8_t*) malloc(width * height * Channels);

    memcpy(_buffer, buffer, width * height * Channels);
    memcpy(_originalBuffer, buffer, width * height * Channels);
}

Image::Image(const char* filename)
{
	int width, height, channels;

	stbi_uc* pixels = stbi_load(filename, &width, &height, &channels, 4);

	if (pixels == nullptr)
	{
		LOG_ERROR("Failed to load image: " + std::string(filename));
		return;
	}

	_width = width;
	_height = height;

	_originalWidth = width;
	_originalHeight = height;

	_buffer = (uint8_t*) malloc(width * height * Channels);
	_originalBuffer = (uint8_t*) malloc(width * height * Channels);

	memcpy(_buffer, pixels, width * height * Channels);

	// Convert from RGBA to ARGB
	for (int i = 0; i < width * height; i++)
	{
		const uint32_t r = pixels[i * 4 + 0];
		const uint32_t g = pixels[i * 4 + 1];
		const uint32_t b = pixels[i * 4 + 2];
		const uint32_t a = pixels[i * 4 + 3];

		_buffer[i] = (a << 24) | (r << 16) | (g << 8) | b;
	}

	memcpy(_originalBuffer, _buffer, width * height * Channels);

	stbi_image_free(pixels);
}

void Image::SetRotation(float angle)
{
    _rotation = angle;
}

void Image::SetScale(float factor)
{
	if (factor == _scale) return;

	if (factor < 0.1f) factor = 0.1f;

	_scale = factor;

	_width = _originalWidth * factor;
	_height = _originalHeight * factor;

	free(_buffer);

	_buffer = (uint8_t*) malloc(_width * _height * Channels);

	for (uint32_t y = 0; y < _height; y++)
	{
		for (uint32_t x = 0; x < _width; x++)
		{
			const uint32_t originalX = x / factor;
			const uint32_t originalY = y / factor;

			const uint32_t index = y * _width + x;
			const uint32_t originalIndex = originalY * _originalWidth + originalX;

			_buffer[index] = _originalBuffer[originalIndex];
		}
	}

	ApplyColor();
}

void Image::SetColor(int color)
{
	bool changed = _color != color;

	_color = color;

	if (changed)
	{
		ResetColor();
		ApplyColor();
	}
}

void Image::ApplyColor()
{
	for (uint32_t i = 0; i < _width * _height; i++)
	{
		if (_buffer[i] == 0x00000000) continue;

		const uint8_t r = (_color >> 16) & 0xFF;
		const uint8_t g = (_color >> 8) & 0xFF;
		const uint8_t b = (_color >> 0) & 0xFF;
        const uint8_t a = (_color >> 24) & 0xFF;

		const uint8_t originalR = (_buffer[i] >> 16) & 0xFF;
		const uint8_t originalG = (_buffer[i] >> 8) & 0xFF;
		const uint8_t originalB = (_buffer[i] >> 0) & 0xFF;

		const auto finalR = (uint8_t) ((r + originalR) / 2);
		const auto finalG = (uint8_t) ((g + originalG) / 2);
		const auto finalB = (uint8_t) ((b + originalB) / 2);

		_buffer[i] = (finalR << 16) | (finalG << 8) | finalB << 0 | (a << 24);
	}
}

Image Image::Cut(int x, int y, int width, int height)
{
    auto* buffer = (uint8_t*) malloc(width * height * Channels);

    for (int j = 0; j < height; j++)
    {
        for (int i = 0; i < width; i++)
        {
            const int index = j * width + i;
            const int originalIndex = (j + y) * _width + (i + x);

            buffer[index] = _buffer[originalIndex];
        }
    }

    return { width, height, buffer };
}

void Image::ResetColor()
{
	_width = _originalWidth * _scale;
	_height = _originalHeight * _scale;

	free(_buffer);

	_buffer = (uint8_t*) malloc(_width * _height * Channels);

	for (uint32_t y = 0; y < _height; y++)
	{
		for (uint32_t x = 0; x < _width; x++)
		{
			const uint32_t originalX = x / _scale;
			const uint32_t originalY = y / _scale;

			const uint32_t index = y * _width + x;
			const uint32_t originalIndex = originalY * _originalWidth + originalX;

			_buffer[index] = _originalBuffer[originalIndex];
		}
	}
}
