R"(
#version 330

in vec4 vertex;
in vec4	texCoord;
out vec2 uv;

uniform mat4 matrix;

void main(void)
{
    gl_Position = matrix * vertex;
    uv = vertex.xy;
}
)" 