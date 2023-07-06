#pragma once

#include "Vector.h"
#include "Shape.h"
#include "Texture.h"
#include "Color.h"

struct DrawableObject
{
    Vector2F Position = {0, 0};
    Vector2F Pivot = {0, 0};
    Vector2F Scale = {1, 1};
    float Rotation = 0;
    Color Color{ 1.f, 1.f, 1.f, 1.f };

    Shape* Shape = nullptr;
    bool UseTexture = false;
    TextureName TextureName = TextureName::Center;
};