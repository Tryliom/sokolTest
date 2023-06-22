#define SOKOL_IMPL
#define SOKOL_GLCORE33

#include <iostream>
#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "basic-sapp.glsl.h"

struct Vector3F
{
    float X, Y, Z;
};

struct Vector2F
{
    float X, Y;
};

struct Color
{
    Color(float r, float g, float b, float a)
    {
        R = r;
        G = g;
        B = b;
        A = a;
    }
    explicit Color(uint32_t color)
    {
        R = ((color >> 24) & 0xFF) / 255.f;
        G = ((color >> 16) & 0xFF) / 255.f;
        B = ((color >> 8) & 0xFF) / 255.f;
        A = (color & 0xFF) / 255.f;
    }

    float R, G, B, A;
};

struct Vertex
{
    Vector3F Position = {0, 0, 0};
    Color Color{ 1.f, 1.f, 1.f, 1.f };
};

int frameCount = 0;

float vertexs[1000];
int vertexCount = 0;

uint32_t indices[1000];
int indexCount = 0;

void AppendVertex(Vertex vertex)
{
    vertexs[vertexCount * 7] = vertex.Position.X;
    vertexs[vertexCount * 7 + 1] = vertex.Position.Y;
    vertexs[vertexCount * 7 + 2] = vertex.Position.Z;
    vertexs[vertexCount * 7 + 3] = vertex.Color.R;
    vertexs[vertexCount * 7 + 4] = vertex.Color.G;
    vertexs[vertexCount * 7 + 5] = vertex.Color.B;
    vertexs[vertexCount * 7 + 6] = vertex.Color.A;

    vertexCount++;
}

void DrawCircle(Vector2F position, float radius, Color color, int segments = 50)
{
    int startIndex = vertexCount;

    AppendVertex({{position.X, position.Y}, color});

    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / (float) segments * 2.f * 3.1415926f;

        AppendVertex({{position.X + cosf(angle) * radius, position.Y + sinf(angle) * radius}, color});
    }

    for (int i = 0; i <= segments; i++)
    {
        indices[indexCount++] = startIndex;
        indices[indexCount++] = startIndex + i + 1;
        indices[indexCount++] = startIndex + i + 2;
    }
}

void DrawRect(Vector2F position, Vector2F size, Color color)
{
    int startIndex = vertexCount;

    AppendVertex({{position.X, position.Y + size.Y}, color});
    AppendVertex({{position.X + size.X, position.Y + size.Y}, color});
    AppendVertex({{position.X + size.X, position.Y}, color});
    AppendVertex({{position.X, position.Y}, color});

    indices[indexCount++] = startIndex;
    indices[indexCount++] = startIndex + 1;
    indices[indexCount++] = startIndex + 2;
    indices[indexCount++] = startIndex + 3;
    indices[indexCount++] = startIndex + 2;
    indices[indexCount++] = startIndex + 0;
}

// application state
static struct
{
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init()
{
    sg_desc desc = {
        .context = sapp_sgcontext(),
        .logger.func = slog_func
    };

    sg_setup(&desc);

    DrawCircle({0, 0}, 0.5f, Color(0xFF0000FF));

    sg_buffer_desc sgmDesc = {
        .data = {&vertexs, vertexCount * 7 * sizeof(*vertexs)},
        .label = "triangle-vertices"
    };
    state.bind.vertex_buffers[0] = sg_make_buffer(sgmDesc);

    sg_buffer_desc sgiDesc = {
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = {&indices, indexCount * sizeof(*indices)},
        .label = "triangle-indices"
    };

    state.bind.index_buffer = sg_make_buffer(sgiDesc);

    // create shader from code-generated sg_shader_desc
    sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));
    sg_pipeline_desc sgpDesc = {
        .index_type = SG_INDEXTYPE_UINT32,
        .shader = shd,
        .layout = {
            .attrs = {
                [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_vs_color0].format = SG_VERTEXFORMAT_FLOAT4
            }
        },
        .label = "triangle-pipeline"
    };

    // create a pipeline object (default render states are fine for triangle)
    state.pip = sg_make_pipeline(sgpDesc);

    // a pass action to clear framebuffer to black
    state.pass_action = (sg_pass_action) {
        .colors = {{ .load_action=SG_LOADACTION_CLEAR, .clear_value={ 0.0f, 0.0f, 0.0f, 1.0f }}}
    };
}

void frame()
{
    frameCount++;
    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, vertexCount * 7, 1);
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
    return (sapp_desc) {
            .init_cb = init,
            .frame_cb = frame,
            .cleanup_cb = cleanup,
            .width = 640,
            .height = 640,
            .window_title = "Triangle (sokol-app)",
            .logger.func = slog_func,
            //.win32_console_create = true
    };
}