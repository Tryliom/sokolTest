#pragma once

#include <vector>
#include "Vector.h"
#include "Color.h"

struct Vertex
{
	Vector2F Position = {0, 0};
	Color Color{ 1.f, 1.f, 1.f, 1.f };
    float U = 0, V = 0;
};

struct Texture
{
    int X = 0;
    int Y = 0;
    int Width = 0;
    int Height = 0;
};

enum class TextureName
{
    BottomLeft, Bottom, BottomRight,
    CenterLeft, Center, CenterRight,
    TopLeft, Top, TopRight
};

namespace Window
{
	int GetFrameCount();
	Vector2F ToScreenSpace(Vector2F position);
	Vector2F ToWorldSpace(Vector2F position);
	Vector2F ConvertInputPosition(Vector2F position);

	void AppendVertex(Vertex vertex);
	void DrawCircle(Vector2F position, float radius, Color color, int segments = 50);
	void DrawRect(Vector2F position, Vector2F size, Color color);
	void DrawLine(Vector2F start, Vector2F end, float thickness, Color color);
	void DrawCustomShape(std::vector<Vector2F> points, Color color);

    void DrawCustomShape(std::vector<Vector2F> points, Color color, std::vector<Vector2F> textures);
    void DrawTexture(Vector2F position, Vector2F size, Color color, TextureName texture);
};