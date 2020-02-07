R"(
#version 330

layout(location = 0) in vec4 position;
layout(location = 1) in float radius;
layout(location = 2) in vec4 color;

void main(void)
{
    gl_Position = position;
}
)"