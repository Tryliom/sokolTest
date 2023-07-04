#include "Window.h"

void OnFrame()
{
	Window::DrawCircle({ 200, 200 }, 100, Color(0xFF0000FF));
	Window::DrawCircle({ 600, 600 }, 50, Color(0xFFFF00FF));
	Window::DrawRect({ 400, 100 }, { 50, 100 }, Color(0xFFFF00FF));
}
