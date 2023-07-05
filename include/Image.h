#pragma once

#include <cstdint>

class Image
{
public:
    Image();
    Image(int width, int height, uint8_t* buffer);
	explicit Image(const char* filename);

private:
	int _originalWidth;
    int _originalHeight;

    uint8_t* _originalBuffer;

    int _width;
    int _height;

    uint8_t* _buffer;

	float _scale = 1.f;
    float _rotation = 0.f;
	uint32_t _color = 0xFFFFFFFF;

	void ApplyColor();
	void ResetColor();

public:
	[[nodiscard]] int GetWidth() const { return _width; }
	[[nodiscard]] int GetHeight() const { return _height; }

    uint8_t* GetBuffer() { return _buffer; }
    [[nodiscard]] size_t GetBufferSize() const { return _width * _height * 4; }

	[[nodiscard]] float GetScale() const { return _scale; }
    [[nodiscard]] float GetRotation() const { return _rotation; }

    void SetRotation(float angle);
	void SetScale(float factor);
    void SetColor(int color);

    Image Cut(int x, int y, int width, int height);
};