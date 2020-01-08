R"(
#version 330

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 texCoord;

out vec2 uv;

uniform mat4 matrix;

void main(void)
{
    gl_Position = matrix * vertex;
    uv = texCoord;
}
)"