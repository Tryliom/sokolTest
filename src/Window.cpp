#include "Window.h"

#include <utility>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "basic-sapp.glsl.h"

#ifdef __EMSCRIPTEN__
#define IMAGE_PATH "assets/"
#else
#define IMAGE_PATH "../assets/"
#endif

#include "Input.h"
#include "Image.h"
#include "Logger.h"

static struct {
	sg_pipeline pip;
	sg_bindings bind;
	sg_pass_action pass_action;
} state;

#pragma region Attributes

const int VertexNbAttributes = 9;

float vertexes[10000];
int vertexesUsed = 0;

uint32_t indices[10000];
int indicesUsed = 0;

int frameCount = 0;
int textureWidth = 0;
int textureHeight = 0;

// Texture 96x96
Texture textures[9] = {
    { 0, 0, 32, 32 }, { 32, 0, 32, 32 }, { 64, 0, 32, 32 },
    { 0, 32, 32, 32 }, { 32, 32, 32, 32 }, { 64, 32, 32, 32 },
    { 0, 64, 32, 32 }, { 32, 64, 32, 32 }, { 64, 64, 32, 32 }
};

Camera camera;

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
		.logger = { .func = slog_func },
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

    //Image image(IMAGE_PATH "snake_head.png");
    //Image image(IMAGE_PATH "drag.png");
    Image image(IMAGE_PATH "walls.png");

    textureWidth = image.GetWidth();
    textureHeight = image.GetHeight();

    state.bind.fs_images[SLOT_tex] = sg_make_image((sg_image_desc)
    {
        .width = textureWidth,
        .height = textureHeight,
        .data = {.subimage = {{{ .ptr = image.GetBuffer(), .size = image.GetBufferSize() }}}},
        .label = "walls-image"
    });

	// Create shader from code-generated sg_shader_desc
	sg_shader shd = sg_make_shader(ui_shader_desc(sg_query_backend()));

	// Create a pipeline object (default render states are fine for triangle)
	sg_pipeline_desc pip_desc = {
		.shader = shd,
		// If the vertex layout doesn't have gaps, don't need to provide strides and offsets
		.layout =
		{
			.attrs =
			{
				{ .format = SG_VERTEXFORMAT_FLOAT3 },
				{ .format = SG_VERTEXFORMAT_FLOAT4 },
                { .format = SG_VERTEXFORMAT_FLOAT2 }
			}
		},
        .colors =
        {
            {
                // Set up the alpha blending
                .blend = {
                    .enabled = true,
                    .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                    .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .op_rgb = SG_BLENDOP_ADD,
                    .src_factor_alpha = SG_BLENDFACTOR_ONE,
                    .dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
                    .op_alpha = SG_BLENDOP_ADD
                }
            }
        },
        .index_type = SG_INDEXTYPE_UINT32,
        .label = "triangle-pipeline",
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

	Input::Update();
	frameCount++;
	OnFrame();

	sg_update_buffer(state.bind.vertex_buffers[0], (sg_range) { .ptr = vertexes, .size = vertexesUsed * VertexNbAttributes * sizeof(*vertexes) });
	sg_update_buffer(state.bind.index_buffer, (sg_range) { .ptr = indices, .size = indicesUsed * sizeof(*indices) });

	sg_draw(0, vertexesUsed * VertexNbAttributes, 1);
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
		.event_cb = Input::OnInput,
		.width = 640,
		.height = 640,
		.window_title = "Game",
		.logger = { .func = slog_func },
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

	Vector2F ToScreenSpace(Vector2F position)
	{
		auto width = (float) sapp_width();
		auto height = (float) sapp_height();

		return {position.X / width * 2 - 1, position.Y / height * 2 - 1};
	}

	Vector2F ToWorldSpace(Vector2F position)
	{
		auto width = (float) sapp_width();
		auto height = (float) sapp_height();

		return {(position.X + 1) / 2 * width, (position.Y + 1) / 2 * height};
	}

	Vector2F ConvertInputPosition(Vector2F position)
	{
		return {position.X, sapp_height() - position.Y};
	}

    std::vector<Vector2F> GetUvs(TextureName texture)
    {
        float width = textures[static_cast<int>(texture)].Width / (float) textureWidth;
        float height = textures[static_cast<int>(texture)].Height / (float) textureHeight;
        float X = textures[static_cast<int>(texture)].X / (float) textureWidth;
        float Y = textures[static_cast<int>(texture)].Y / (float) textureHeight;

        return {
            {X, Y},
            {X + width, Y},
            {X + width, Y + height},
            {X, Y + height}
        };
    }

    Vector2F GetScaledPosition(Vector2F position, Vector2F pivot, Vector2F scale, Vector2F size)
    {
        auto scaledSize = size * scale;
        auto scaledPivot = scaledSize * pivot;

        return position - scaledPivot;
    }

	void AppendVertex(Vertex vertex)
	{
		auto position = ToScreenSpace(vertex.Position);
        auto cameraOffset = ToScreenSpace(camera.Position);
        int vertexIndex = vertexesUsed * VertexNbAttributes;

		vertexes[vertexIndex] = (position.X + cameraOffset.X) * camera.Zoom;
		vertexes[vertexIndex + 1] = (position.Y + cameraOffset.Y) * camera.Zoom;
		vertexes[vertexIndex + 2] = 0;
		vertexes[vertexIndex + 3] = vertex.Color.R;
		vertexes[vertexIndex + 4] = vertex.Color.G;
		vertexes[vertexIndex + 5] = vertex.Color.B;
		vertexes[vertexIndex + 6] = vertex.Color.A;
        vertexes[vertexIndex + 7] = vertex.U;
        vertexes[vertexIndex + 8] = vertex.V;

		vertexesUsed++;
	}

	void DrawCircle(Vector2F position, float radius, Color color, int segments, std::vector<Vector2F> uvs)
	{
        if (uvs.empty())
        {
            uvs = {{ -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }};
        }

		int startIndex = vertexesUsed;

		AppendVertex({{position.X, position.Y}, color, uvs[0].X, uvs[0].Y});

		for (int i = 0; i <= segments; i++)
		{
			float angle = (float) i / (float) segments * 2.f * 3.1415926f;
            int uvIndex = (i % 3) + 1;

			AppendVertex({{position.X + cosf(angle) * radius, position.Y + sinf(angle) * radius}, color, uvs[uvIndex].X, uvs[uvIndex].Y});
		}

		for (int i = 0; i <= segments; i++)
		{
			indices[indicesUsed++] = startIndex + 1;
			indices[indicesUsed++] = startIndex + i + 1;
			indices[indicesUsed++] = startIndex + i + 2;
		}
	}

	void DrawRect(Vector2F position, Vector2F size, Color color, std::vector<Vector2F> uvs)
	{
        if (uvs.empty())
        {
            uvs = {{ -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }};
        }

		DrawCustomShape({
			{position.X, position.Y},
			{position.X + size.X, position.Y},
			{position.X + size.X, position.Y + size.Y},
			{position.X, position.Y + size.Y}
		}, color, std::move(uvs));
	}

	void DrawLine(Vector2F start, Vector2F end, float thickness, Color color, std::vector<Vector2F> uvs)
	{
        if (uvs.empty())
        {
            uvs = {{ -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }};
        }

		Vector2F direction = end - start;
		Vector2F normal = direction.Normalized().Normal();

		Vector2F start1 = start + normal * thickness / 2;
		Vector2F start2 = start - normal * thickness / 2;
		Vector2F end1 = end + normal * thickness / 2;
		Vector2F end2 = end - normal * thickness / 2;

		DrawCustomShape({start1, end1, end2, start2}, color, std::move(uvs));
	}

	void DrawCustomShape(std::vector<Vector2F> points, Color color, std::vector<Vector2F> uvs)
	{
        if (uvs.empty())
        {
            uvs = {{ -1, -1 }, { -1, -1 }, { -1, -1 }, { -1, -1 }};
        }

        int startIndex = vertexesUsed;

        for (int i = 0; i < points.size(); i++)
        {
            AppendVertex({{points[i].X, points[i].Y}, color, uvs[i % 4].X, uvs[i % 4].Y});
        }

        for (int i = 0; i < points.size() - 2; i++)
        {
            indices[indicesUsed++] = startIndex;
            indices[indicesUsed++] = startIndex + i + 1;
            indices[indicesUsed++] = startIndex + i + 2;
        }
	}

    void DrawObject(DrawableObject object)
    {
        std::vector<Vector2F> uvs = {};
        Vector2F position = {0, 0};

        if (object.UseTexture)
        {
            uvs = GetUvs(object.TextureName);
        }

        if (object.Shape->Type == ShapeType::CircleType)
        {
            auto* circle = static_cast<CircleShape*>(object.Shape);

            position = GetScaledPosition(object.Position, object.Pivot, object.Scale, {circle->Radius, circle->Radius});

            DrawCircle(position, circle->Radius * object.Scale.Length(), object.Color, 50, uvs);
        }

        if (object.Shape->Type == ShapeType::RectangleType)
        {
            auto* rect = static_cast<RectangleShape*>(object.Shape);

            position = GetScaledPosition(object.Position, object.Pivot, object.Scale, {rect->Width, rect->Height});

            DrawRect(position, Vector2F{rect->Width, rect->Height} * object.Scale, object.Color, uvs);
        }

        if (object.Shape->Type == ShapeType::LineType)
        {
            auto* line = static_cast<LineShape*>(object.Shape);

            position = GetScaledPosition(object.Position, object.Pivot, object.Scale, {line->Length, line->Thickness});

            DrawLine(position, object.Position + line->Direction * line->Length * object.Scale, line->Thickness * object.Scale.Length(), object.Color, uvs);
        }

        if (object.Shape->Type == ShapeType::CustomType)
        {
            auto* custom = static_cast<CustomShape*>(object.Shape);
            auto positions = std::vector<Vector2F>();

            position = GetScaledPosition(object.Position, object.Pivot, object.Scale, {0, 0});

            for (auto point : custom->Points)
            {
                positions.push_back(position + point);
            }

            DrawCustomShape(positions, object.Color, uvs);
        }
    }

    void MoveCamera(Vector2F position)
    {
        camera.Position += position;
    }

    void Zoom(float scale)
    {
        camera.Zoom += scale;

        if (camera.Zoom < 0.f)
        {
            camera.Zoom = 0.f;
        }
    }
}
