#pragma once

#include <vector>
#include "Vector.h"
#include "Color.h"

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
	void DrawLine(Vector2F start, Vector2F end, float thickness, Color color);
	void DrawCustomShape(std::vector<Vector2F> points, Color color);
};