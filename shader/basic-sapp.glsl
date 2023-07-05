#pragma sokol @vs vs
in vec4 position;
in vec4 color0;
in vec2 uv0;

out vec4 color;
out vec2 uv;

void main()
{
    gl_Position = position;
    color = color0;
    uv = uv0;
}
#pragma sokol @end

#pragma sokol @fs fs
uniform sampler2D tex;

in vec4 color;
in vec2 uv;

out vec4 frag_color;

void main()
{
    // Get pixel color alpha
    vec4 tex_color = texture(tex, uv) * color;

    frag_color = tex_color;
}

#pragma sokol @end

#pragma sokol @program ui vs fs
