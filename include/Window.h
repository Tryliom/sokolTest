#pragma once

#include <cstdint>

struct Vector3F
{
	float X, Y, Z;
};

struct Vector2F
{
	float X, Y;
};

struct Color
{
	Color(float r, float g, float b, float a)
	{
		R = r;
		G = g;
		B = b;
		A = a;
	}
	explicit Color(uint32_t color)
	{
		R = ((color >> 24) & 0xFF) / 255.f;
		G = ((color >> 16) & 0xFF) / 255.f;
		B = ((color >> 8) & 0xFF) / 255.f;
		A = (color & 0xFF) / 255.f;
	}

	float R, G, B, A;
};

struct Vertex
{
	Vector2F Position = {0, 0};
	Color Color{ 1.f, 1.f, 1.f, 1.f };
};

namespace Window
{
	int GetFrameCount();
	void AppendVertex(Vertex vertex);
	void DrawCircle(Vector2F position, float radius, Color color, int segments = 50);
	void DrawRect(Vector2F position, Vector2F size, Color color);
};