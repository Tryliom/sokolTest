#pragma once

#include <vector>
#include "Vector.h"

enum ShapeType
{
    CircleType,
    RectangleType,
    LineType,
    CustomType
};

struct Shape
{
    explicit Shape(ShapeType type) : Type(type) {}

    ShapeType Type = ShapeType::CircleType;
};

struct CircleShape : public Shape
{
    explicit CircleShape(float radius) : Shape(ShapeType::CircleType), Radius(radius) {}

    float Radius = 0;
};

struct RectangleShape : public Shape
{
    RectangleShape(float width, float height) : Shape(ShapeType::RectangleType), Width(width), Height(height) {}

    float Width = 0;
    float Height = 0;
};

struct LineShape : public Shape
{
    explicit LineShape(float thickness, Vector2F direction, float length) : Shape(ShapeType::LineType), Thickness(thickness), Direction(direction), Length(length) {}

    float Thickness = 0;
    Vector2F Direction = {0, 0};
    float Length = 0;
};

struct CustomShape : public Shape
{
    explicit CustomShape(std::vector<Vector2F> points) : Shape(ShapeType::CustomType), Points(points) {}

    std::vector<Vector2F> Points;
};