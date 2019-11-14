R"(
#version 330

layout(location = 0) in vec4 vertex;

uniform mat4 transform;

void main(void)
{
    gl_Position = transform * vertex;
}
)" 