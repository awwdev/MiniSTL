#version 450

layout (location = 0) in vec4 inPos;
layout (location = 1) in vec3 inNor;
layout (location = 2) in vec4 inCol;
layout (location = 3) in vec2 inTex;

layout(binding = 0) uniform Meta { 
    mat4 view;
    mat4 proj;
} meta;

void main() 
{
    gl_Position = meta.proj * meta.view * inPos;
}