#version 450

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec4 in_color;

layout (location = 0) out vec4 out_color;

layout (set = 1, binding = 0) uniform UBO
{
    mat4x4 modelViewProj;
};

void main()
{
    gl_Position = modelViewProj * vec4(in_position, 1.0);
    out_color = vec4(in_color.xyz, 1.0);
}
