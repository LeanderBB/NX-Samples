#version 430 core

layout (location = 0) uniform mat4 mvp;

layout (location = 0) in vec4 position;

void main(void)
{
    gl_Position = mvp * position;
}