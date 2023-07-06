#include <cmath>

#include "Window.h"
#include "Input.h"
#include "sokol_app.h"

DrawableObject movableObject = {
    .Position = { 300, 300 },
    .Pivot = { 0.5f, 0.5f },
    .Shape = new RectangleShape(100, 100)
};
float speed = 3;

void OnFrame()
{
	auto mousePosition = Input::GetMousePosition();
    auto previousMousePosition = Input::GetPreviousMousePosition();

    if (Input::IsKeyHeld(SAPP_KEYCODE_A))
    {
        movableObject.Position.X -= speed;
    }
    if (Input::IsKeyHeld(SAPP_KEYCODE_D))
    {
        movableObject.Position.X += speed;
    }
    if (Input::IsKeyHeld(SAPP_KEYCODE_W))
    {
        movableObject.Position.Y += speed;
    }
    if (Input::IsKeyHeld(SAPP_KEYCODE_S))
    {
        movableObject.Position.Y -= speed;
    }

    if (Input::IsKeyHeld(SAPP_KEYCODE_Z))
    {
        Window::Zoom(0.01f);
    }
    if (Input::IsKeyHeld(SAPP_KEYCODE_X))
    {
        Window::Zoom(-0.01f);
    }

    if (Input::IsMouseButtonHeld(SAPP_MOUSEBUTTON_LEFT))
    {
        Window::MoveCamera(mousePosition - previousMousePosition);
    }

    auto frame = Window::GetFrameCount();
    float scale = std::sin(frame / 100.f) * 0.5f + 0.5f;

    Window::DrawObject(movableObject);

    Window::DrawObject({
           .Position = { 500, 500 },
           .Pivot = { 0.5f, 0.5f },
           //.Scale = { scale, scale },
           .Rotation = frame / 100.f,
           .Shape = new RectangleShape(100, 100),
           .UseTexture = true,
           .TextureName = TextureName::BottomRight
    });
}
