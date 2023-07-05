#include "Window.h"
#include "Input.h"

void OnFrame()
{
	auto mousePosition = Input::GetMousePosition();

	Window::DrawCircle({ 200, 200 }, 100, Color(0xFF0000FF));
	Window::DrawCircle({ 600, 600 }, 50, Color(0xFFFF00FF));
	Window::DrawRect({ 400, 100 }, { 50, 100 }, Color(0xFFFF00FF));

	Window::DrawCustomShape({ { 300, 450 }, { 550, 400 }, { 600, 500 }, { 400, 550 } }, Color(0xFFFFFFFF));

	Window::DrawLine({ 0, 0 }, mousePosition, 5, Color(0xFF00FFFF));

    Window::DrawRect({ 0, 0 }, { 100, 100 }, Color(0xFFFFFFFF));
    Window::DrawRect({ 100, 100 }, { 100, 100 }, Color(0xFF0000FF));
    Window::DrawRect({ 200, 200 }, { 100, 100 }, Color(0xFF00FF00));
    Window::DrawRect({ 300, 300 }, { 100, 100 }, Color(0xFFFF0000));
}
