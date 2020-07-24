#version 450

layout(push_constant) uniform Push {
    mat4 projection;
    uint wnd_width;
    uint wnd_height;
} push;

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec4 inNor;
layout(location = 2) in vec4 inCol;
layout(location = 3) in vec4 inTex;

layout (location = 0) out vec4 outCol;

void main() 
{
    gl_Position = push.projection * inPos;
    outCol      = vec4(1, 1, 1, 0.5);
}