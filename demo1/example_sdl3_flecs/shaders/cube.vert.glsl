#version 450

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;

layout (location = 0) out vec4 out_color;

layout (set = 1, binding = 0) uniform UBO
{
    mat4x4 modelViewProj;
};

void main()
{
    gl_Position = modelViewProj * vec4(in_position, 1.0);
    out_color = vec4(in_color, 1.0) + vec4(gl_Position.x, gl_Position.y, gl_Position.z, 1.0);
}
