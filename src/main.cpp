#define SOKOL_IMPL
#define SOKOL_GLCORE33

#include "sokol_gfx.h"
#include "sokol_app.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "basic-sapp.glsl.h"

// application state
static struct
{
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init(void)
{
    sg_desc desc = {
            .context = sapp_sgcontext()
    };

    sg_setup(&desc);

    // a vertex buffer with 3 vertices
    float vertices[] = {
            // positions            // colors
            0.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f
    };
    sg_buffer_desc sgmDesc = {
        .data = SG_RANGE(vertices),
        .label = "triangle-vertices"
    };
    state.bind.vertex_buffers[0] = sg_make_buffer(sgmDesc);

    // create shader from code-generated sg_shader_desc
    sg_shader shd = sg_make_shader(triangle_shader_desc(sg_query_backend()));
    sg_pipeline_desc sgpDesc = {
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
    sg_begin_default_pass(&state.pass_action, sapp_width(), sapp_height());
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 3, 1);
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
            .height = 480,
            .window_title = "Triangle (sokol-app)"
    };
}