#include "Window.h"

void OnFrame()
{
	Window::DrawCircle({ 0, 0 }, 0.25f, Color(0xFF0000FF));
	Window::DrawCircle({ 0.5f, 0.5f }, 0.15f, Color(0xFFFF00FF));
	Window::DrawRect({ -0.6f, -0.6f }, { 0.1f, 0.1f }, Color(0xFFFF00FF));
}
