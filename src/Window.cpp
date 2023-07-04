#include "Window.h"

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "basic-sapp.glsl.h"

static struct {
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;
} state;

#pragma region Attributes

float vertexes[10000];
int vertexesUsed = 0;

uint32_t indices[10000];
int indicesUsed = 0;

int frameCount = 0;

#pragma endregion

#pragma region SokolFunctions

void OnFrame();

void Clear()
{
	vertexesUsed = 0;
	indicesUsed = 0;
}

static void init()
{
	sg_desc desc = (sg_desc){
		.logger = { .func = slog_func},
		.context = sapp_sgcontext()
	};
	sg_setup(desc);

	state.bind.vertex_buffers[0] = sg_make_buffer((sg_buffer_desc){
		.size = sizeof(vertexes),
		.usage = SG_USAGE_DYNAMIC,
		.label = "triangle-vertices",
	});

	state.bind.index_buffer = sg_make_buffer((sg_buffer_desc){
		.size = sizeof(indices),
		.type = SG_BUFFERTYPE_INDEXBUFFER,
		.usage = SG_USAGE_DYNAMIC,
		.label = "triangle-indices",
	});

	// Create shader from code-generated sg_shader_desc
	sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));

	// Create a pipeline object (default render states are fine for triangle)
	sg_pipeline_desc pip_desc = {
		.shader = shd,
		// If the vertex layout doesn't have gaps, don't need to provide strides and offsets
		.layout =
		{
			.attrs =
			{
				{ .format = SG_VERTEXFORMAT_FLOAT3 },
				{ .format = SG_VERTEXFORMAT_FLOAT4 }
			}
		},
		.index_type = SG_INDEXTYPE_UINT32,
		.label = "triangle-pipeline"
	};
	state.pip = sg_make_pipeline(pip_desc);

	// a pass action to clear framebuffer to black
	state.pass_action = (sg_pass_action)
	{
		.colors = { { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.0f, 0.0f, 0.0f, 1.0f } } }
	};
}

void frame()
{
	sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
	sg_apply_pipeline(state.pip);
	sg_apply_bindings(&state.bind);

	// Clear buffers
	Clear();

	frameCount++;
	OnFrame();

	sg_update_buffer(state.bind.vertex_buffers[0], (sg_range) { .ptr = vertexes, .size = vertexesUsed * 7 * sizeof(*vertexes) });
	sg_update_buffer(state.bind.index_buffer, (sg_range) { .ptr = indices, .size = indicesUsed * sizeof(*indices) });

	sg_draw(0, vertexesUsed * 7, 1);
	sg_end_pass();
	sg_commit();
}

void cleanup()
{
	sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[])
{
	(void) argc;
	(void) argv;

	return (sapp_desc){
		.init_cb = init,
		.frame_cb = frame,
		.cleanup_cb = cleanup,
		.width = 640,
		.height = 640,
		.window_title = "Triangle (sokol-app)",
		.icon.sokol_default = true,
		.logger.func = slog_func,
		//.win32_console_create = true // Use it if you want to see console output
	};
}

#pragma endregion

namespace Window
{
	int GetFrameCount()
	{
		return frameCount;
	}

	void AppendVertex(Vertex vertex)
	{
		int width = sapp_width();
		int height = sapp_height();

		vertexes[vertexesUsed * 7] = (vertex.Position.X / width) * 2 - 1;
		vertexes[vertexesUsed * 7 + 1] = (vertex.Position.Y / height) * 2 - 1;
		vertexes[vertexesUsed * 7 + 2] = 0;
		vertexes[vertexesUsed * 7 + 3] = vertex.Color.R;
		vertexes[vertexesUsed * 7 + 4] = vertex.Color.G;
		vertexes[vertexesUsed * 7 + 5] = vertex.Color.B;
		vertexes[vertexesUsed * 7 + 6] = vertex.Color.A;

		vertexesUsed++;
	}

	void DrawCircle(Vector2F position, float radius, Color color, int segments)
	{
		int startIndex = vertexesUsed;

		AppendVertex({{position.X, position.Y}, color});

		for (int i = 0; i <= segments; i++)
		{
			float angle = (float) i / (float) segments * 2.f * 3.1415926f;

			AppendVertex({{position.X + cosf(angle) * radius, position.Y + sinf(angle) * radius}, color});
		}

		for (int i = 0; i <= segments; i++)
		{
			indices[indicesUsed++] = startIndex + 1;
			indices[indicesUsed++] = startIndex + i + 1;
			indices[indicesUsed++] = startIndex + i + 2;
		}
	}

	void DrawRect(Vector2F position, Vector2F size, Color color)
	{
		DrawCustomShape({
			{position.X, position.Y},
			{position.X + size.X, position.Y},
			{position.X + size.X, position.Y + size.Y},
			{position.X, position.Y + size.Y}
		}, color);
	}

	void DrawLine(Vector2F start, Vector2F end, float thickness, Color color)
	{
		Vector2F direction = end - start;
		Vector2F normal = direction.Normalized().Normal();

		Vector2F start1 = start + normal * thickness / 2;
		Vector2F start2 = start - normal * thickness / 2;
		Vector2F end1 = end + normal * thickness / 2;
		Vector2F end2 = end - normal * thickness / 2;

		DrawCustomShape({start1, end1, end2, start2}, color);
	}

	void DrawCustomShape(std::vector<Vector2F> points, Color color)
	{
		int startIndex = vertexesUsed;

		for (auto& point : points)
		{
			AppendVertex({{point.X, point.Y}, color});
		}

		for (int i = 0; i < points.size() - 2; i++)
		{
			indices[indicesUsed++] = startIndex;
			indices[indicesUsed++] = startIndex + i + 1;
			indices[indicesUsed++] = startIndex + i + 2;
		}
	}
}
